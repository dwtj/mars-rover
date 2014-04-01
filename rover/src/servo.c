/*
 * servo.c
 *
 * Created: 3/14/2014 3:45:46 PM
 *  Author: dwtj
 */ 

#include <stdbool.h>
#include <stdint.h>
#include "lcd.h"
#include "servo.h"
#include <avr/io.h>
#include <math.h>
#include <util.h>
const uint16_t TOP = 43000; // period is 21.5ms


void servo_manual_calib()
{
	int8_t move = 0;
		
	// All degrees in this function are only rough (i.e. uncalibrated) approximations.
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

void servo_init()
{
	TCCR3A = 0b00100011; // waveform generation mode 15, output compare from channel B
	TCCR3B = 0b00011010; // 1/8 prescaler
	TCCR3C = 0;  // Not using force output compares
	ETIMSK = 0;  // Not using interupts
	OCR3A = TOP;	//set top
	
	DDRE = 0b00010000;  // Set data direction on Pin 4 of Port E to input.
	
	servo_angle(90, true);
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
		wait_ms(500);  // TODO: make the wait time vary w.r.t. the change in angle.
	}
}


void servo_enable() {
	TCCR3A |= 0b00100000;
}


void servo_disable() {
	TCCR3A &= 0b11001111;
}
