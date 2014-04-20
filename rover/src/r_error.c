/*
 * r_error.c
 *
 * Created: 4/17/2014 9:19:35 PM
 *  Author: dwtj
 */ 


#include <stdbool.h>

#include "r_error.h"
#include "control.h"



bool has_errored = false;



// Stop the currently executing command.
void abort_command() {
	control.running = false;
	#warning "need to stop adding to the queue and empty it."
}


/**
 * Transmits an error to control, prints an error code on LCD, then halts
 * execution on the rover (i.e. enters an infinite loop).
 */
void r_error(uint8_t err_num, char* mesg)
{
	#warning "I suspect that this needs to be revised."
    /*
	usart_tx(signal_start);
	usart_tx(mesg_error);
	usart_tx(err_num);
	usart_tx_buf(mesg);
	usart_tx(signal_stop);
    */
	lprintf("Error: %d: %s", err_num, mesg);
	abort_command();
	while(1) {
		; // Halt indefinitely because something bad happened.
	}
}
