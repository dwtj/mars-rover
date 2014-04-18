/*
 * comm.h
 *
 * Created: 2/20/2014 10:16:43 AM
 *  Author: Flavia and David
 */ 


#ifndef COMM_H
#define COMM_H

#include <stdbool.h>

#include <avr/io.h>

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

#endif /* COMM_H */
