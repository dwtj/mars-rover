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
	lprintf("%u", OCR3B);
}


#warning "TODO: fix the servo handlers"
void servo_system(){
	switch (usart_rx()) {
    case 0:
        servo_init();
        break;
	//calibrate
    case 1:
		//calibrate not handled on this end
        break;
	//servo state
    case 2:
        //servo_state();//TODO
        break;
	//servo move angle
    case 3:
		if(rx_frame())
		{
			r_error(error_frame, "Servo expected single data frame.");
		}
		struct {
			uint8_t angle;
			bool wait;
		} *servo_data = (void *) &control.data;
		
        servo_angle(servo_data->angle, servo_data->wait);
        break;
	//servo pulse width
    case 4:
		if(rx_frame()){
			r_error(error_frame,"Pulse Width expected but one frame.");
		}
		
		struct {
			float p;
		} *servo_data2 = (void *) &control.data;
		
		set_pulse_proportion(servo_data2->p);
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
	TCCR3C = 0;          // Not using force output compares
	ETIMSK = 0;          // Not using interrupts
	OCR3A = TOP;	     // set top
	
	DDRE = 0b00010000;   // Set data direction on Pin 4 of Port E to output.
	
	servo_angle(90, true);
}





/**
 * Returns an approximate number of milliseconds that the system should wait before being certain
 * that the servo has reached its intended angle.
 */
static uint16_t servo_move_wait_time(uint8_t old_angle, uint8_t new_angle)
{
	int16_t rv;
	rv = abs(new_angle - old_angle);
	rv = 7 * rv + 20;
	return rv;
}



/**
 * Moves servo to the desired angle `deg`, measured in degrees. Uses the simple analytical
 * approximation method derived from lab manual to compute the PWM pulse width. If `wait`
 * is `true`, then the function will wait for two seconds before returning. If `wait` is
 * `false`, then the function will immediately return, that is, before the servo has reached
 * the desired angle.
 */
void servo_angle(uint8_t deg, bool wait)
{
	static uint8_t old_deg = 0;
	
	if (deg < 0 || deg > 180) {
		return;
	}
	
	/*
	// The pulse period is 21.5ms.
	// 0 degrees was recommended to be 0.5ms.
	// 180 degrees was recommended to be 2.0ms.
	
	float p = 3.87594e-4 * deg + 0.023256;
	set_pulse_proportion(p);
	*/
	
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
