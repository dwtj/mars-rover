/*
 * comm.h
 *
 * Created: 2/20/2014 10:16:43 AM
 *  Author: Flavia and David
 */ 


#ifndef COMM_H
#define COMM_H

#include <avr/io.h>

/* The following enumerated types are classes of header codes that head
   packets being sent between the the control program and the rover. */

/* The `signal` codes are used by the control program to initiate a signal
   sequence with the rover. The rover's responses are headed by the same
   code. */

typedef enum {
	signal_null = 0,
	signal_ping = 1,
	num_signal_codes
} signal;
#define NUM_SIGNAL_CODES 2


// Check for some compile-time protocol definition errors:
/*
#if num_signal_codes != NUM_SIGNAL_CODES
#error "Protocol Definition Error: The Number of signal codes is not consistent."
#endif
*/

/* Declares an array of functions, each of which will handle a signal request. */
extern uint8_t (*signal_handlers[NUM_SIGNAL_CODES])(char *);


void enable_RX_ISR();

void disable_RX_ISR();

bool is_valid_signal_code(signal type);

#endif /* COMM_H */
