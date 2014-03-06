/*
 * IR.c
 *
 * Created: 2/28/2014 2:22:58 PM
 *  Author: flaviarc
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "util.h"
#include "lcd.h"
#include "usart.h"

#define MAX_DIST 50
#define MIN_DIST 9
#define NUM_CALIB_SAMPLES 50

uint16_t calib_data[MAX_DIST + 1] = {0};

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


// Converts the given quantized voltage measurement `d` to the analytical approximation of the distance,
// as derived from datasheet information. This does not use any form of calibration.
float IR_analytical_conv(uint16_t d) {
	static const float vRef = 2.56;
	static const float slope = 0.042977;
	static const float intercept = -0.009167;
	
	float v = (vRef/1024) * d;
	
	return 1 / (slope * v + intercept) - 0.42;
}



/* Sends a pair of datapoints from a distance calibration sample reading. */
void send_dist_reading(uint8_t dist, uint16_t reading)
{
	// static const size_t bufsize = 100;
	static char buf[100];
	snprintf(buf, 100, "%d, %d\n", dist, reading);
	
	USART_transmit_buffer(buf);
}


void IR_calibrate(bool bam_send)
{
	lcd_init();
	init_push_buttons();
	if (bam_send) {
		USART_Init(1);
		USART_transmit_buffer("Distances, Readings\n");
	}
	
	float avg;
	uint16_t sample = 0;
	
	wait_ms(500);
	// TODO: ensure strict monotonicity of `calib_data`
	
	for (int dist = MIN_DIST; dist <= MAX_DIST; dist++)
	{
		lcd_clear();
		lprintf("Ready to Start %d cm", dist);
		wait_button(NULL);
		lcd_clear();
		lcd_puts("Sampling...");
		wait_ms(500);
		avg = 0.0;
		for (int i = 0; i < NUM_CALIB_SAMPLES; i++) {
			// running average:
			sample = IR_run();
			if (bam_send) {
				send_dist_reading(dist, sample);
			}
			avg += ((float) sample) / ((float) NUM_CALIB_SAMPLES);
			wait_ms(20);
		}
		calib_data[dist] = (uint16_t) round(avg);
	}
}



/* returns 0 if conversion failed */
uint8_t IR_conv(uint16_t d)
{
	// assumes that the elements of `calib_data` are monotonically non-increasing (i.e. strictly decreasing)
	
	float mid;
	
	if (calib_data[MIN_DIST] < d) {
		return 0;
	}
	
	for (int i = MIN_DIST; i <= MAX_DIST; i++)
	{
		if (calib_data[i] < d && d < calib_data[i - 1]) {
			mid = ((float) calib_data[i] + calib_data[i - 1]) / 2.0;
			return d < mid ? i : i - 1;
		}
		
		if (d == calib_data[i]) {
			return d;
		}
	}
	
	// Else, `d` must be less than than calib_data[MAX_DIST], or any other element of `calib_data`, i.e.
	// the object is far away.
	return 0;
}