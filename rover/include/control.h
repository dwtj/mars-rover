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
#include "open_interface.h"


#ifndef CONTROL_H_
#define CONTROL_H_

/**
 * The following enumerated types are classes of header codes that head
 * packets being sent between the the control program and the rover.
 */


/**
 * Codes to distinguish between different types of control signals:
 */
typedef enum {
	signal_null = 0,    // NULL
	signal_start = 1,   // Start of message
	signal_stop = 2,    // End of message
} signal;
#define NUM_SIGNAL_CODES 3


/**
 * Codes to distinguish between different types of messages:
 */
typedef enum {
	mesg_error = 0,
	mesg_ping = 1,
	mesg_echo = 2,
    mesg_rng_seed = 3,
	mesg_command = 4,
} mesg;
#define NUM_MESG_CODES 5


/**
 * Codes to distinguish between different subsystems:
 */
typedef enum {
	lcd = 0,
	oi = 1,
	sonar = 2,
	servo = 3,
	ir = 4,
	rng = 5,
} subsystems;
#define NUM_SUBSYS_CODES 6


#define DATA_FRAME_MAX_LEN 100
typedef struct {
	bool running;  // Checked by handlers to see if they should stop.
	uint8_t data[DATA_FRAME_MAX_LEN];
	uint8_t data_len;
	oi_t oi_state;
} control_t;  // To be used to hold the state as messages are being interpreted.


extern control_t control;


void read_frame();


void write_frame();


#endif /* CONTROL_H_ */
