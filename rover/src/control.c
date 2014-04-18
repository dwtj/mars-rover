/*
 * control.c
 *
 * Created: 4/13/2014 3:48:46 PM
 *  Author: asr
 */

#warning "TODO: implement echo"
#warning "Assuming that usart_receive() is bit by bit. if this is wrong, so is our code."

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
 * Reads a single byte via `USART_Receive()` and checks that it is a start byte.
 * This will block until a byte of some sort is received, i.e., there is no timeout.
 */
void check_for_start() {
	if(USART_Receive() != signal_start) {
		r_error(error_txq, "Did not receive expected start bit.");
	}
}


/**
 * Reads a single byte via `USART_Receive()` and checks that it is a start byte.
 * This will block until a byte of some sort is received, i.e., there is no timeout.
 */
void check_for_end() {
	if(USART_Receive() != signal_stop) {
		r_error(error_txq, "Did not receive expected stop bit.");
	}
}

void null_handler() {
	;  // Do Nothing.
}

//receives a signal, and transmits an equivalent signal. 
//note that this is not the /same/ signal we receive, but it should be equivalent.
void ping_handler() {
	txq_enqueue(signal_start);
	txq_enqueue(signal_ping);
	txq_enqueue(signal_stop);
}

void error_handler() {
	#warning "TODO? Does this need to be more generic? or possibly more specific."
	r_error(error_bad_request,"Bad signal request.");
}


void lcd_system(){
	switch(USART_Receive())
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
	switch(USART_Receive())
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
	switch(USART_Receive())
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

void servo_system(){
	switch(USART_Receive())
	{
		case 0:
			servo_init();
			break;
		case 1:
			servo_calibrate();//TODO
			break;
		case 2:
			servo_state();//TODO
			break;
		case 3:
			servo_angle(controller.data[0],true); //read from data[0], then wait to finish moving.
			break;
		case 4:
			servo_pulse_width();//TODO
			break;
		default:
			r_error(error_bad_request, "Bad servo Command");
			break;
	}
}

void ir_system(){
	switch(USART_Receive())
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
	switch(USART_Receive())
	{
		#warning "TODO?"
		default:
			r_error(error_bad_request, "Bad RNG Command");
			break;
	}
}


void (*subsystem_callers[NUM_SUBSYS_CODES])() = {
	lcd_system,   // 0
	oi_system,    // 1
	sonar_system, // 2
	servo_system, // 3
	ir_system,    // 4
	rng_system,   // 5
};

void signal_handler() {
	int id = USART_Receive();
	subsystem_callers[id]();
}



#warning "TODO: RX ISR to modify the `controller` global variable."

#warning "Is this still being used?"
/*
void (*handlers[NUM_SIGNAL_CODES])() = {
	null_handler,  // 0
	null_handler,
	null_handler,
	null_handler,
	null_handler,
	
	null_handler,  // 5
	null_handler,  // 6
	ping_handler,  // 7
	error_handler,  // 8
	signal_handler, //9
};
*/



#warning "Is this being used?"
/*
void dispatch()
{
	while(1) {
		handlers[controller.current]();
	}
}
*/


/**
 * Calling this function will drop the rover into "control mode", in which the
 * rover gives over most of its autonomous functions to commands coming from
 * the remote control system.
 */
void control_mode() {
	USART_Init(1);
	while (true) {
		// Receive and handle messages from `control` indefinitely:
		check_for_start();
		signal_handler();  // Calls appropriate handlers.
		check_for_end();
	}
}