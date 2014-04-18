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



//Stop the currently executing command.
void abort_command() {
	controller.running = false;
	#warning "need to stop adding to the queue and empty it."
}


//Transmit an error to control, print error code on LCD, then cease executing the command and rover.
void r_error(uint8_t err_num, char* msg)
{
	USART_Transmit(signal_start);
	USART_Transmit(signal_error);
	USART_Transmit(err_num);
	USART_transmit_buffer(msg);
	USART_Transmit(signal_stop);
	lprintf("Error: %d", err_num);
	abort_command();
	while(1); //Stop running this bad code.
}