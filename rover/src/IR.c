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
#include "control.h"
#include "IR.h"

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

float IR_reading() {
	IR_start();
	return(IR_conv(IR_read()));
}



/**
 * Assumes that IR reading has been initiated via `IR_start()`.
 */
uint16_t IR_read()
{
	while (ADCSRA & 0x40) {
		;  // wait while ADSC is high -> conversion is still in process
	}

	return ADC; //Includes the high and low bytes
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
	
	usart_tx_buf(buf);
}



/*
 * Implments the third-order polynomial conversion from IR ADC readings, `d`,
 * to distances (in cm) as described in `sensors/ir/calibration_data.md`.
 */
float IR_conv(uint16_t d)
{
    // The intercept and the first, second, and third-order coefficients:
    const static float coef[] = {100.5, -0.2811, 3.148e-4, -1.254e-7};
    return coef[0] + d * (coef[1] + d * (coef[2] + d * coef[3]));
}




uint16_t IR_run() {
	#warning "`IR_run()` is deprecated. Use `IR_reading()` instead."
	IR_start();
	return(IR_read());
}


/*
 * Runs the human-directed IR calibration routine. If `bam_send` is `true`,
 * then a serial connection over bluetooth (BAM) will be opened and all
 * recorded sample data will be sent across as ascii text (see the
 * `send_dist_reading()` function). If `save_means` is `true`, then for every
 * measured distance, 9cm - 50cm, the mean of the samples taken at this
 * distance will be saved to `calib_data[]`.
 */
void IR_calibrate(bool bam_send, bool save_means)
{
	lcd_init();
	init_push_buttons();
	if (bam_send) {
		usart_init(1);
		usart_tx_buf("Distances, Readings\n");
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
            if (save_means) {
			    avg += ((float) sample) / NUM_CALIB_SAMPLES;
            }
			wait_ms(20);
		}
		
        if (save_means) {
		    calib_data[dist] = (uint16_t) round(avg);
        }
	}
}

//Handler from the ir_system, moved from control for readability.
void ir_system(){
	switch(usart_rx())
	{
		case 0:
		IR_init();
		break;
		case 1:
		#warning "Add parameter"
		//IR_calibrate();
		break;
		case 2:
		IR_reading();
		break;
		default:
		r_error(error_bad_request, "Bad IR Command");
		break;
	}
}
