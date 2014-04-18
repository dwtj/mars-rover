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
	controller.running = false;
	#warning "need to stop adding to the queue and empty it."
}


//Transmit an error to control, print error code on LCD, then cease executing the command and rover.
void r_error(uint8_t err_num, char* msg)
{
	#warning "I suspect that this needs to be revised."
	usart_tx(signal_start);
	usart_tx(signal_error);
	usart_tx(err_num);
	usart_tx_buf(msg);
	usart_tx(signal_stop);
	lprintf("Error: %d", err_num);
	abort_command();
	while(1) {
		; // Halt indefinitely because something bad happened.
	}
}