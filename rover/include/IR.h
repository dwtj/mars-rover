/*
 * IR.h
 *
 * Created: 2/28/2014 2:23:16 PM
 *  Author: flaviarc
 */ 


#ifndef IR_H_
#define IR_H_

#include <stdint.h>

void IR_init(); 

void IR_start();  // returns immediately
 
uint16_t IR_run();  // starts conversion and polls until conversion is complete, returning the converted value

uint16_t IR_read();  // polls until conversion is complete

float IR_analytical_conv(uint16_t v);  // no calibration approximation

#endif /* IR_H_ */