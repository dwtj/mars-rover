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


control controller;  // Lone instance of the `control` struct.


//this function waits for a stop bit. it will be called a lot.
void wait_for_end()
{
	if(USART_Receive() != signal_stop)
	{
		rError(error_txq, "Did not receive expected stop bit"); //something went wrong!
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
	wait_for_end();
}

void error_handler() {
	rError(error_bad_request,"Bad signal request."); //TODO? Does this need to be more generic? or possibly more specific.
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
			rError(error_bad_request, "Bad LCD Command");
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
			rError(error_bad_request, "Bad OI Command");
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
			rError(error_bad_request, "Bad sonar Command");
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
			rError(error_bad_request, "Bad servo Command");
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
			rError(error_bad_request, "Bad IR Command");
			break;
	}
}

void rng_system(){
	switch(USART_Receive())
	{//TODO?
		default:
			rError(error_bad_request, "Bad RNG Command");
			break;
	}
}


void (*subsystem_callers[NUM_SUBSYS_CODES])() = {
	lcd_system, //0
	oi_system, //1
	sonar_system, //2
	servo_system, //3
	ir_system, //4
	rng_system, //5
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