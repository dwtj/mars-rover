/*
 * control.c
 *
 * Created: 4/13/2014 3:48:46 PM
 *  Author: asr
 */

#warning "TODO: URGENT: ISR x 2: 1 on TX_COMPLETE [or something similar], the other is when the queue stops being empty, ie when num_elements goes from 0 to one, or shortly after. check every few ms?"
#warning "TODO: CONTINUTED: Copy items from txq to hardware buffer (when the hardware buffer is ready to take input. There exists an interript for when the hardware buffer is ready to recieive (txq buffer read perhaps?). We want to shift into the hardware buffer from TXQ. But maybe we don't want two, because interference. (??)"

/************************************************************************/
/* put this in TXQ.C
	ISR_TXBUFF{														
 	usart_tx(txq_dequeue())		
	 if !(txq_num_elements)
	 {
		 ENABLE NUMBER 2, YO
	 }
 	}
 	
 	ISR_TIMER/OCR{
 		//ONLY ACTIVE WHEN THERE ARE ZERO ELEMENTS
 		//constantly polling txq. 
		 if(txq.num_elements)
		 {//this disables itself
			 usart_tx(txq.dequueue)
		 }
 		}                                                                     */
/************************************************************************/


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


control controller;  // Lone instance of the `control` struct.



/**
 * Reads a single byte via `usart_rx()` and checks that it is a start byte.
 * This will block until a byte of some sort is received, i.e., there is no timeout.
 */
void check_for_start() {
	uint8_t byte = usart_rx();
	if (byte != signal_start) {
		lprintf("start check: %d", byte);
		wait_button("");
		r_error(error_txq, "Did not receive expected start byte.");
	}
}


/**
 * Reads a single byte via `usart_rx()` and checks that it is a start byte.
 * This will block until a byte of some sort is received, i.e., there is no timeout.
 */
void check_for_end() {
	if (usart_rx() != signal_stop) {
		r_error(error_txq, "Did not receive expected stop byte.");
	}
}


void null_handler() {
	;  // Do Nothing.
}


// Receives a signal, and transmits an equivalent signal. 
// Note that this is not the /same/ signal we receive, just equivalent.
void ping_handler() 
{
	lcd_putc('p');
	//wait_button("DEBUG: ping_handler");
	txq_enqueue(signal_start);
	txq_enqueue(signal_ping);
	txq_enqueue(signal_stop);
	txq_drain();
}


void echo_handler()
{
	lcd_putc('e');
	uint8_t data_length = usart_rx();
	//We don't want to deal with that multi-frame garbage here.
	if(data_length > MAX_DATA_LENGTH || data_length < 1)
	{
		r_error(error_bad_data_length, "Length should be between 1 and MAX_DATA_LENGTH");
	}
	int i = 0;
	//encode
	for(i =0; i< data_length; i++)
	{
		controller.data[i] = usart_rx();
	}
	uint8_t real_length = usart_rx();
	uint8_t more = usart_rx();
	check_for_end();
	
	//send it back;
	txq_enqueue(signal_start);
	for(i=0; i<real_length; i++)
	{
		txq_enqueue(controller.data[i]);
	}
	txq_enqueue(signal_stop);
	txq_drain();
	
	//More data is comoing? 
	//it should go back to echo_handler! 
	if(more == 1)
	{
		check_for_start();
		message_handler();
	}
}


void error_handler() {
	r_error(error_bad_request,"Bad signal request.");
}


void rng_system(){
	switch(usart_rx())
	{
		#warning "TODO?"
		default:
			r_error(error_bad_request, "Bad RNG Command");
			break;
	}
}


void (*subsystem_handlers[NUM_SUBSYS_CODES])() = {
	lcd_system,   // 0
	oi_system,    // 1
	sonar_system, // 2
	servo_system, // 3
	ir_system,    // 4
	rng_system,   // 5
};



/**
 * Reads the Subsystem ID of the current message to decide which subsystem
 * should handle the rest of the message.
 */
void command_handler() {
	uint8_t subsys = usart_rx();
	if (0 <= subsys && subsys < NUM_SUBSYS_CODES) {
		subsystem_handlers[subsys]();
	} else {
		r_error(error_bad_request, "Invalid subsystem ID.");
	}
}


/**
 * Reads the Message Type of the current message being received and calls
 * the appropriate handler.
 */
void message_handler() {
	uint8_t t = usart_rx();  // the message type.
	switch (t) {
	case signal_error:
		error_handler();
		break;
	case signal_ping:
		ping_handler();
		break;
	case signal_echo:
		echo_handler();
		break;
	case signal_command:
		command_handler();
		break;
	default:
		r_error(error_bad_request, "Received an invalid Message ID byte.");
	}
}


/**
 * Calling this function will drop the rover into "control mode", in which the
 * rover gives over most of its autonomous functions to commands coming from
 * the remote control system.
 */
void control_mode()
{
	lcd_init();
	lcd_puts("Control Mode");
	usart_init(1);
	wait_ms(1000);
	lcd_clear();

	// Receive and handle messages from `control` indefinitely:
	while (true) {
		check_for_start();
		message_handler();  // The message type.
		check_for_end();
	}
}

//leftover from comm.c. Probably unnecessary.
bool is_valid_signal(signal sig) {
	return 0 <= sig && sig < NUM_SIGNAL_CODES;
}
