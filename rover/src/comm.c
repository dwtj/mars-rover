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




static uint8_t query_null_handler() {
	// do nothing
	return 0;
}

static uint8_t query_ping_handler() {
	lcd_putc('.');
	return 0;
}


/* Assign query handler functions to an array.
   The query code itself is used to index into the array to reach the handler. */
uint8_t (*query_handlers[])(char *) = {
	query_null_handler,
	query_ping_handler
};






bool is_valid_query_code(query type) {
	return (bool) type >= 0 && type < num_query_codes;
}






bool connect()
{
	// Some preconditions for opening a connection.
	
	// Check for protocol definition errors:
	/*
	if (query.num_query_codes != NUM_QUERY_CODES) {
		return false;
	}
	*/
	
	//Check for handler definition errors:
	/*
	if ((sizeof(query_handlers) / sizeof(query_handlers[0]) == NUM_QUERY_CODES) {
		return false;
	}
	*/
	
	/* Connect */
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
		query q = (query) USART_Receive();
		is_valid_query_code(q);
		
		query_handlers[q]((void *) 0);
	}
}