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








/**
 * Calling this function will drop the rover into `comm_mode`, in which it
 * gives over most of its autonomous functions to commands coming from a remote
 * control system.
 */
void comm_mode() {
	USART_Init(1);
        enable_RX_ISR();
	while (true) {
                ; // do nothing.
	}
}

void enable_RX_ISR() {
    UCSR0B |= 0b10000000;
}

void disable_RX_ISR() {
    UCSR0B &= 0b01111111;
}




ISR(USART0_RX_vect) {
    disable_RX_ISR();
    // TODO: disable the handler until a correct connection has been
    // TODO: only read if data is available
    signal s = (signal) USART_Receive();
    is_valid_signal_code(s);
               
    signal_handlers[s]((void *) 0);
    // TODO
    enable_RX_ISR();
}
