/*
 * control.c
 *
 * Created: 4/13/2014 3:48:46 PM
 *  Author: asr
 */

#warning "TODO: implement echo"
#warning "Assuming that usart_rx() is bit by bit. if this is wrong, so is our code."

#include "util.h"
#include "usart.h"
#include "IR.h"
#include "open_interface.h"
#include "sonar.h"
#include "servo.h"
#include "comm.h"
#include "txq.h"
#include "control.h"
#include "r_error.h"


control controller;  // Lone instance of the `control` struct.



/**
 * Reads a single byte via `usart_rx()` and checks that it is a start byte.
 * This will block until a byte of some sort is received, i.e., there is no timeout.
 */
void check_for_start() {
	if (usart_rx() != signal_start) {
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
void ping_handler() {
	txq_enqueue(signal_start);
	txq_enqueue(signal_ping);
	txq_enqueue(signal_stop);
}


void echo_handler() {
	#warning "TODO"
}


void error_handler() {
	#warning "TODO? Does this need to be more generic? or possibly more specific."
	r_error(error_bad_request,"Bad signal request.");
}


void lcd_system(){
	switch(usart_rx())
	{
		case 0:
			lcd_init();
			break;
		case 1:
			#warning "TODO: This is incorrect:"
			//lcd_puts(controller.data[0]);
			break;
		case 2:
			lcd_clear();
			break;
		default:
			r_error(error_bad_request, "Bad LCD Command");
			break;
	}
}



void oi_system(){
	switch(usart_rx())
	{
		case 0:
			#warning "TODO: add parameters:"
			//oi_init();
			break;
		default:
			r_error(error_bad_request, "Bad OI Command");
			break;
	}
}

void sonar_system(){
	switch(usart_rx())
	{
		case 0:
			sonar_init();
			break;
		case 1:
			#warning "TODO: There doesn't seem to be such a function:"
			//sonar_calibrate();//TODO
			break;
		case 2:
			sonar_reading();
			break;
		default:
			r_error(error_bad_request, "Bad sonar Command");
			break;
	}
}


#warning "TODO: fix the servo handlers"
void servo_system(){
	switch(usart_rx())
	{
		case 0:
			servo_init();
			break;
		case 1:
			//servo_calibrate();//TODO
			break;
		case 2:
			//servo_state();//TODO
			break;
		case 3:
			servo_angle(controller.data[0],true); //read from data[0], then wait to finish moving.
			break;
		case 4:
			//servo_pulse_width();//TODO
			break;
		default:
			r_error(error_bad_request, "Bad servo Command");
			break;
	}
}

void ir_system(){
	switch(usart_rx())
	{
		case 0:
			IR_init();
			break;
		case 1:
			#warning "Add parameter"
			//IR_calibrate();
			break;
		case 2:
			IR_reading();
			break;
		default:
			r_error(error_bad_request, "Bad IR Command");
			break;
	}
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
message_handler() {
	uint8_t t = usart_rx();  // the message type.
	switch (t) {
	case signal_error:
		#warning "TODO"
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