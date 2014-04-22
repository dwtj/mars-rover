/*
 * control.c
 *
 * Created: 4/13/2014 3:48:46 PM
 *  Author: asr
 */



#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>


#include "util.h"
#include "usart.h"
#include "IR.h"
#include "open_interface.h"
#include "sonar.h"
#include "servo.h"
#include "txq.h"
#include "control.h"
#include "r_error.h"
#include "lcd.h"



control_t control;  // Lone instance of the `control` struct.



/**
 * Reads a single frame from the current message being received on the serial
 * connection into `control.data`. Returns whether another frame is coming.
 * Note that the *real* data length of this frame is what ends up being stored
 * `control.data_len`.
 *
 * To be clear, whatever was previously stored in `control.data` and
 * `control.data_len` is clobbered.
 */
bool rx_frame()
{
    bool rv;

    // Get the (expected) length of this data frame:
    control.data_len = usart_rx();
    lcd_putc(control.data_len + '0');  // DEBUG
	if(control.data_len > DATA_FRAME_MAX_LEN) {
		r_error(error_bad_message, "Data frame length must not exceed DATA_FRAME_MAX_LEN");
	}

    // Copy bytes coming from serial into `control.data`.
    lcd_putc('['); // DEBUG
    int i = 0;
    for (i = 0; i < control.data_len; i++) {
        control.data[i] = usart_rx();
        lcd_putc('.'); // DEBUG
    }
    lcd_putc(']'); // DEBUG

    // Notice that `i` now holds the expected data frame length.
    control.data_len = usart_rx();
    lcd_putc(control.data_len + '0');  // DEBUG
    if (control.data_len > i) {
        lprintf("%d, %d", i, control.data_len);  // DEBUG
        wait_button("");  // DEBUG
		r_error(error_bad_message, "The real data frame length cannot be larger than the expected data frame length.");
    }

    // The last byte in this frame indicates whether another frame is coming:
    rv = usart_rx();
    lcd_putc(rv ? 'y' : 'n');  // DEBUG
    return rv;
}



/**
 * Adds the complete contents of `control.data` as a frame, along with
 * the appropriate framing bytes to the `txq`. The argument `another_frame`
 * indicates whether the another frame byte should be either 0 or 1.
 */
void tx_frame(bool another_frame)
{
    txq_enqueue(control.data_len);
    for (int i = 0; i < control.data_len; i++) {
        txq_enqueue(control.data[i]);
    }
    txq_enqueue(control.data_len);
    txq_enqueue(another_frame);
}



/**
 * A `ping` message has been received, so this function transmits an
 * equivalent `ping` message in response.
 */
static void ping_handler() 
{
	lcd_putc('p');  // DEBUG: the ping handler has started
	txq_enqueue(signal_start);
	txq_enqueue(mesg_ping);
	txq_enqueue(signal_stop);
	txq_drain();
}


/**
 * This should be called after the current message has been determined to be of
 * type `mesg_echo`. It reads the data stored in the message's sequence of
 * data frames and responds with a message whose data frames contain the same
 * data.
 */
static void echo_handler()
{
	lcd_putc('e');  // DEBUG: the echo handler has started

    // Start of the response message:
	txq_enqueue(signal_start);
	txq_enqueue(mesg_echo);

    // In each iteration, a data frame is de-framed
    bool another_frame = true;
    while (another_frame)
    {
        another_frame = rx_frame();
        tx_frame(another_frame);
        #warning "txq_drain() is a temporary measure until asynchronous drainage is implemented."
	    txq_drain();
    }


    // End of the response message:
	txq_enqueue(signal_stop);
	txq_drain();
}



/**
 * This should be called after the current message has been determined to be
 * of type `mesg_command`. It reads the Subsystem ID of the current message to
 * decide which subsystem should handle the rest of the message.
 */
static void command_handler()
{
    // Choices for functions to be called next.
    static void (*subsystem_handlers[NUM_SUBSYS_CODES])() = {
        lcd_system,   // 0
        oi_system,    // 1
        sonar_system, // 2
        servo_system, // 3
        ir_system,    // 4
    };

    // Use the supplied `subsys` code to choose which handler to use.
	uint8_t subsys = usart_rx();
	if (0 <= subsys && subsys < NUM_SUBSYS_CODES) {
		subsystem_handlers[subsys]();
	} else {
		r_error(error_bad_message, "Invalid subsystem ID.");
	}
}



static void seed_rng_handler() {
    #warning "TODO: seed_rng_handler() is not yet implemented"
}



/**
 * Reads the Message Type of the current message being received and calls
 * the appropriate handler.
 */
static void mesg_handler()
{
    // Choices for functions to be called next.
    static void (*mesg_handlers[NUM_MESG_CODES])() = {
        error_handler,     // 0
        ping_handler,      // 1
        echo_handler,      // 2
        command_handler,   // 3
        seed_rng_handler,  // 4
    };

    // Use the supplied `mesg_id` to choose which handler to use.
	uint8_t mesg_id = usart_rx();  // the message type.
    if (0 <= mesg_id && mesg_id < NUM_MESG_CODES) {
        mesg_handlers[mesg_id]();
    } else {
		r_error(error_bad_message, "Received an invalid Message ID byte.");
    }
}



/**
 * Calling this function will drop the rover into "control mode", in which the
 * rover gives over most of its autonomous functions to commands coming from
 * the remote control system.
 */
void control_mode()
{
    uint8_t byte;
    char mesg[60];  // Size is somewhat arbitrary, but should be big enough.

	lcd_init();
	lcd_puts("Control Mode");
	usart_init();
	wait_ms(1000);
	lcd_clear();

    txq_init();

    usart_drain_rx();

	// Receive and handle messages from `control` indefinitely:
	while (true)
    {
        // Check for start byte indefinitely:
        byte = usart_rx();
	    if (byte != signal_start) {
            sprintf(mesg, "Received %u instead of expected start byte.", byte);
            r_error(error_txq, mesg);
	    }
        lcd_putc('(');  // DEBUG: found start byte

		mesg_handler();  // Calls the appropriate sequence of handlers.

        // Check for stop byte indefinitely:
        byte = usart_rx();
        if (byte != signal_stop) {
            sprintf(mesg, "Received %u instead of expected stop byte.", byte);
		    r_error(error_txq, mesg);
	    }
        txq_drain();
        lcd_putc(')');  // DEBUG: found stop byte
	}
}
