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

/**
 * Expects a floating point value (strictly) between 0.0 and 1.0.
 */
static void set_pulse_proportion(float p){
	// The actual waveform generated is outputted on PE4 of channel B
	OCR3B = (uint16_t) roundf(TOP * p);
	lprintf("%u", OCR3B);
}

void servo_init(){
	//Does not enable wave form generation
	TCCR3A = 0b00100011; // waveform generation mode 15, output compare from channel B
	TCCR3B = 0b00011010; // 1/8 prescaler
	TCCR3C = 0;  // Not using force output compares
	ETIMSK = 0;  // Not using interupts
	OCR3A = TOP;	//set top
	
	servo_angle(90, true);
}


void servo_angle(uint8_t deg, bool wait)
{
	// TODO
	float p = 0;
	switch (deg) {
		case 0:
			p = 0.023256;
			break;
		case 90:
			p = 0.05814;
			break;
		case 180:
			p = 0.093023;
			break;
		default:
			;
			// TODO: behavior undefined
	}	
	
	set_pulse_proportion(p);
	if (wait) {
		wait_ms(2000);
	}
}


void servo_enable() {
	TCCR3A |= 0b00100000;
}


void servo_disable() {
	TCCR3A &= 0b11001111;
}
