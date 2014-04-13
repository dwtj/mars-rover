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

typedef enum
{
	// The following seven signals match the ASCII control codes.
	signal_nul = 0,   // NULL
	signal_soh = 1,   // Start of heading
	signal_stx = 2,   // Start of text
	signal_etx = 3,   // End of text
	signal_eot = 4,   // End of transmission
	signal_enq = 5,   // Enquiry
	signal_ack = 6,   // Acknowledge
	
	signal_ping = 7,
	signal_error = 8,
	
	num_signal_codes
} signal;
#define NUM_SIGNAL_CODES 9



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
