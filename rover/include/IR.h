/*
 * IR.h
 *
 * Created: 2/28/2014 2:23:16 PM
 *  Author: flaviarc
 */ 

#include <stdbool.h>

#ifndef IR_H_
#define IR_H_

#include <stdint.h>
#include <stdbool.h>

void ir_init(); 

void ir_start();  // returns immediately
 
uint16_t ir_raw_reading();  // Returns unconverted (i.e. raw) IR data.

float ir_reading();  // Returns converted IR data.

float ir_analytical_conv(uint16_t v);  // no calibration approximation

float ir_conv(uint16_t d);

void ir_calibrate(bool bam_send, bool save_means);

//Handler for IR system
void ir_system();

#endif /* IR_H_ */
