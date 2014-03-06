/*
 * comm.c
 *
 * Created: 2/20/2014 10:17:36 AM
 *  Author: Flavia and David
 */


#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "comm.h"
#include "lcd.h"




static bool is_connected = false;




static uint8_t signal_null_handler() {
	// do nothing
	return 0;
}

static uint8_t signal_ping_handler() {
	lcd_putc('.');
	return 0;
}


/**
 * Assign signal handler functions to an array. The signal code itself is used
 * to index into the array to reach the handler.
 */
uint8_t (*signal_handlers[])(char *) = {
	signal_null_handler,
	signal_ping_handler
};






bool is_valid_signal_code(signal type) {
	return (bool) type >= 0 && type < num_signal_codes;
}






bool connect()
{
	USART_Init(1);
	is_connected = true;
	return true;
}




void command_mode() {
	connect();
	while (true) {
		;  // do nothing
	}
}




ISR(USART0_RX_vect) {
	if (is_connected) {
		// TODO: disable the handler until a correct connection has been verified.
		// TODO: only read if data is available
		signal s = (signal) USART_Receive();
		is_valid_signal_code(s);
		
		signal_handlers[s]((void *) 0);
	}
}
