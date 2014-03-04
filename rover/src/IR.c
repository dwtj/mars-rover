/*
 * IR.c
 *
 * Created: 2/28/2014 2:22:58 PM
 *  Author: flaviarc
 */ 

#include <avr/io.h>

void IR_init()
{
	uint8_t mask = 0;
	
	mask |= 0xC0;  // Set voltage reference selection to internal 2.56V
	// mask |= 0x20;  // ADC left adjust result
	mask |= 0x02;  // Enabling single ended input -> ADC2
	
	ADMUX = mask;
	
	
	mask = 0; //clearing mask for reuse
	
	mask |= 0x80;  // set ADEN
	// mask |= 0x40;  // set ADSC
	// mask |= 0x20;  // set ADFR
	// mask |= 0x10;  // set ADIF
	mask |= 0x07;  // set prescalar to 125KHz frequency using dividing factor 128
	
	ADCSRA = mask; //Enable, running mode, interrupt and clock select
}


void IR_start() {
	ADCSRA |= 0x40;
}

uint16_t IR_read()
{
	while (ADCSRA & 0x40) {
		;  // wait while ADSC is high -> conversion is still in process
	}

	return ADC; //Includes the high and low bytes
}

uint16_t IR_run() {
	IR_start();
	return(IR_read());
}


// Converts the given voltage measurement `v` to the analytical approximation of the distance,
// as derived from datasheet information. This does not use any form of calibration.
float IR_analytical_conv(uint16_t v) {
	static const float b = -0.168;
	static const float m = 0.45;
	static const float l = 0.42;
	
	return m / (v - b*m) - l;
}