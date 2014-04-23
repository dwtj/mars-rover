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

void IR_init(); 

void IR_start();  // returns immediately
 
// Warning: This is deprecated. Use IR_reading() instead.
uint16_t IR_run();  // starts conversion and polls until conversion is complete, returning the converted value

float IR_reading();//returns converted data

uint16_t IR_read();  // polls until conversion is complete, returns raw data

float IR_analytical_conv(uint16_t v);  // no calibration approximation

float IR_conv(uint16_t d);

void IR_calibrate(bool bam_send, bool save_means);

//Handler for IR system
void ir_system();

#endif /* IR_H_ */
