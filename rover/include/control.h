/*
 * control.h
 *
 * Created: 4/13/2014 3:48:58 PM
 *  Author: asr
 */ 

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
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
	signal_null = 0,	// NULL
	signal_start = 1,   // Start of message
	signal_stop = 2,	// End of message
} signal;
#define NUM_SIGNAL_CODES 3


/**
 * Codes to distinguish between different types of messages:
 */
typedef enum {
	mesg_error = 0,
	mesg_ping = 1,
	mesg_echo = 2,
	mesg_command = 3,
	mesg_rng_seed = 4,
	mesg_scan = 5,
} mesg;
#define NUM_MESG_CODES 6


/**
 * Codes to distinguish between different subsystems:
 */
typedef enum {
	subsys_lcd = 0,
	subsys_oi = 1,
	subsys_sonar = 2,
	subsys_servo = 3,
	subsys_ir = 4,
} subsys_t;
#define NUM_SUBSYS_CODES 5




enum {
	full_distance = 0,
	left_bumper = 1,
	right_bumper = 2,
	left_and_right_bumper = 3,
	front_left_cliff = 4,
	front_right_cliff = 5,
	left_cliff = 6,
	right_cliff = 7,
	white_tape_front_left = 8,
	white_tape_front_right = 9,
	white_tape_left = 10,
	white_tape_right = 11,
	left_wheel = 12,
	right_wheel = 13,
	middle_wheel = 14
} stop_flag;



// The maximum amount of data that can be put into a single data frame as
// specified by the message protocol. (Should be between 50 and 255.)
#define DATA_FRAME_MAX_LEN 100

// To be used as a buffer for messages that are being sent and recieved.
typedef struct {
	bool running;  // Checked by handlers to see if they should stop.
	uint8_t data[DATA_FRAME_MAX_LEN + 1];  // Last byte is to hold null term.
	uint8_t data_len;
	oi_t oi_state;
} control_t;


extern control_t control;


void dist_reading_handler(subsys_t);

void tx_frame(bool another_frame);

bool rx_frame();

#endif /* CONTROL_H_ */
