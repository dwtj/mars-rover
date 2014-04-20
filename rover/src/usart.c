
// Code adapted from Atmel Data sheet page 176, 180. Changed to use macros that refer to the USART unit 0.

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#include "util.h"
#include "usart.h"
#include "r_error.h"


/**
 * Checks whether a data overrun has occurred since the UDR0 was last read.
 */
static bool _has_data_overrun() {
    return UCSR0A & (1 << DOR0);
}


static bool _has_parity_error() {
    return UCSR0A & (1 << UPE0);
}


static bool _has_frame_error() {
    return UCSR0A & (1 << FE0);
}



void usart_init()
{
	/* Set baud rate via the UBRR */
	UBRR0H = USART_UBRR >> 8;
	UBRR0L = USART_UBRR;

	UCSR0A = USART_2X ? 0x02 : 0x00;  // Enable double speed?

	UCSR0B = 0b00011000; // Enable both receiver and transmitter.

    UCSR0C = 0x00;
    UCSR0C |= 0b0 << UMSEL0;  // Set asynchronous mode.
    //UCSR0C |= 0b10 << UPM0;   // Set parity mode to even.
    UCSR0C |= 0b00 << UPM0;   // Set parity mode to even.
    UCSR0C |= 0b1 << USBS0;   // Use two stop bits.
    UCSR0C |= 0b11 << UCSZ0;  // Set character size to be 8-bits.
}



uint8_t usart_rx(void)
{
    if (_has_data_overrun()) {
        r_error(error_data_overrun, "A data overrun error was discovered when attempting to read another byte.");
    }

    if (_has_parity_error()) {
        r_error(error_parity, "A parity error was discovered when attempting to read another byte.");
    }

    if (_has_frame_error()) {
        r_error(error_frame, "A frame error was discovered when attempting to read another byte.");
    }

	/* Wait for data to be received */
	while (!(UCSR0A & (1<<RXC0))) {
	    ;
    }
    #warning "DEBUG: Why does adding some wait time change the behavior?"
    //wait_ms(50);
	/* Get and return received data from buffer */
	return UDR0;
}



/**
 * Reads and ignores data from usart until there is no more.
 */
void usart_drain_rx()
{
    while (UCSR0A & (1 << RXC0)) {
        uint8_t nowhere = UDR0;
    }
}



void usart_tx(uint8_t data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE)) )
		;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}


	
void usart_tx_buf(char *buf) {
	for (char *cur = buf; *cur != '\0'; cur++) {
		usart_tx(*cur);
	}
}



void usart_RX_ISR_enable() {
	UCSR0B |= 0b10000000;
}



void usart_RX_ISR_disable() {
	UCSR0B &= 0b01111111;
}
