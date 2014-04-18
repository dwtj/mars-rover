/*
 * control.h
 *
 * Created: 4/13/2014 3:48:58 PM
 *  Author: asr
 */ 

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "comm.h"
#include "lcd.h"
#include "util.h"



#ifndef CONTROL_H_
#define CONTROL_H_

#define MAX_DATA_LENGTH 100
typedef struct{
	bool running; //Checked by handlers to see if they should stop.
	signal current; //Stores the most recent command issued from python.
	uint8_t data[MAX_DATA_LENGTH];
	uint8_t data_idx;
} control;  // To be used to hold the state as messages are being interpreted.

extern control controller;


#endif /* CONTROL_H_ */