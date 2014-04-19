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
#include "lcd.h"
#include "util.h"
#include "r_error.h"



#ifndef CONTROL_H_
#define CONTROL_H_

/* The following enumerated types are classes of header codes that head
   packets being sent between the the control program and the rover. */

/* The `signal` codes are used by the control program to initiate a signal
   sequence with the rover. The rover's responses are headed by the same
   code. */

#warning "TODO: refactor these into control signals and message types."
typedef enum {
	signal_null = 0,    // NULL
	signal_start = 1,   // Start of message
	signal_stop = 2,    // End of message
	
	signal_error = 3,
	signal_ping = 4,
	signal_echo = 5,
	signal_command = 6,
} signal;
#define NUM_SIGNAL_CODES 7

// IDs for each of the rover's subsystems.
typedef enum {
	lcd = 0,
	oi = 1,
	sonar = 2,
	servo = 3,
	ir = 4,
	rng = 5,
} subsystems;
#define NUM_SUBSYS_CODES 6



#define MAX_DATA_LENGTH 100
typedef struct {
	bool running; //Checked by handlers to see if they should stop.
	signal current; //Stores the most recent command issued from python.
	uint8_t data[MAX_DATA_LENGTH];
	uint8_t data_idx;
} control;  // To be used to hold the state as messages are being interpreted.

extern control controller;


#endif /* CONTROL_H_ */