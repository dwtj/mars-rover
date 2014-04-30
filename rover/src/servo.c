/*
 * servo.c
 *
 * Created: 3/14/2014 3:45:46 PM
 *  Author: dwtj
 */ 

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <math.h>
#include <util.h>
#include "lcd.h"
#include "servo.h"
#include "control.h"

const uint16_t TOP = 43000; // period is 21.5ms


void servo_manual_calib()
{
	int8_t move = 0;
		
	// All degrees in this function are only rough (i.e. uncalibrated) approximations,
	// generated from the lab manual and the datasheet.
	OCR3B = 2500; // about 90 degrees
	
	while(1) {
		lprintf("%d", OCR3B);
		switch (wait_button("")) {
		case 1:
			move = -17;  // 1 degree CW
			break;
		case 2:
			move = -42;  // 2.5 degrees CW
			break;
		case 3:
			move = -83;  // 5 degrees CW
			break;
		case 4:
			move = 83;  // 5 degrees CCW
			break;
		case 5:
			move = 42;  // 2.5 degrees CCW
			break;
		case 6:
			move = 17;  // 1 degrees CCW
			break;
		default:
			move = 0;
		}
		
		OCR3B += move;
		wait_ms(200);
	}
}

/**
 * Expects a floating point value (strictly) between 0.0 and 1.0.
 */
static void set_pulse_proportion(float p){
	// The actual waveform generated is outputted on PE4 of channel B
	OCR3B = (uint16_t) roundf(TOP * p);
}


static void set_pulse_width(uint16_t pulse_width) {
	OCR3B = pulse_width;
}



#warning "TODO: fix the servo handlers"
void servo_system()
{
	enum {
		command_init = 0,
		command_state = 1,
		command_angle = 2,
		command_pulse_width = 3,
	} command_id = usart_rx();

	txq_enqueue(command_id);

	switch (command_id) {
	case command_init:
		servo_init();
		break;

	case command_state:
		#warning "Servo state not yet implemented."
		r_error(error_unimplemented, "`command_state` is not yet implemented.")
		break;

	case command_angle:
		if(rx_frame())
		{
			r_error(error_frame, "Servo expected single data frame.");
		}
		struct {
			uint8_t angle;
			bool wait;
		} *servo_data = (void *) &control.data;
	
		if(rx_frame())
		{
			r_error(error_frame, "Servo expected single data frame.");
		}

		if (control.data_len != sizeof(*servo_data)) {
			r_error(error_frame, "The expected data length is different from the action.");  
		}
		
		servo_angle(servo_data->angle, servo_data->wait);
		break;
		
	case command_pulse_width:
		if(rx_frame()){
			r_error(error_frame,"Pulse Width expected but one frame.");
		}

		uint16_t *pulse_width = (void *) &control.data;
		
		if (control.data_len != sizeof(*pulse_width)) {
			r_error (error_frame, "The expected data length is different from the action.");  
		}
		
		set_pulse_width(*pulse_width);
		break;
		
	default:
			r_error(error_bad_message, "Bad servo Command");
			break;
	}
}


void servo_init()
{
	TCCR3A = 0b00100011; // WGM 15, output compare from channel B
	TCCR3B = 0b00011010; // 1/8 prescaler
	TCCR3C = 0;		  // Not using force output compares
	ETIMSK = 0;		  // Not using interrupts
	OCR3A = TOP;		 // set top
	
	DDRE = 0b00010000;   // Set data direction on Pin 4 of Port E to output.
	
	servo_angle(90, true);
}





/**
 * Returns an approximate number of milliseconds that the system should wait
 * before being certain that the servo has reached its intended angle.
 */
static uint16_t servo_move_wait_time(uint8_t old_angle, uint8_t new_angle)
{
	int16_t rv;
	rv = abs(new_angle - old_angle);
	rv = 7 * rv + 20;
	return rv;
}



/**
 * Moves servo to the desired angle `deg`, measured in degrees. Uses the
 * simple analytical approximation method derived from lab manual to compute
 * the PWM pulse width. If `wait` is `true`, then the function will wait for
 * two seconds before returning. If `wait` is `false`, then the function will
 * immediately return, that is, before the servo has reached the desired angle.
 */
void servo_angle(uint8_t deg, bool wait)
{
	static uint8_t old_deg = 0;
	
	if (deg < 0 || deg > 180) {
		return;
	}
	
	/* Simple least squares regression from calibration data on robot 13: */
	OCR3B = round(18.6*deg + 1056.8);

	if (wait) {
		wait_ms(servo_move_wait_time(old_deg, deg));
	}
	
	old_deg = deg;
}







void servo_enable() {
	TCCR3A |= 0b00100000;
}


void servo_disable() {
	TCCR3A &= 0b11001111;
}
