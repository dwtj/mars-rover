/*
 * control.c
 *
 * Created: 4/13/2014 3:48:46 PM
 *  Author: asr
 */

#warning "TODO: IMPLEMENT PING, implement echo, DETERMINE WHERE SUBSYSTEM_ID, SIGNAL_ID, how to read the fricken data frame in general."
#warning "Assuming that usart_receive() is bit by bit. if this is wrong, so is our code."

#include "util.h"
#include "util.c"
#include "IR.h"
#include "open_interface.h"
#include "sonar.h"
#include "servo.h"

//this function waits for a stop bit. it will be called a lot.
void wait_for_end()
{
	if(USART_Receive() != signal.signal_stop)
	{
		rError(error_txq, "Did not receive expected stop bit"); //something went wrong!
	}
}

void null_handler() {
	;  // Do Nothing.
}

void ping_handler() {
	USART_Transmit(signal.signal_ping);
	wait_for_end();
}

void error_handler() {
	rError(error_bad_request,"Bad signal request.");//TODO? Does this need to be more generic? or possibly more specific
}


void lcd_system(){
	switch(USART_Receive())
	{
		case 0:
			lcd_init();
					break;
		case 1:
			lcd_puts(controller.data[0]);//This might be an issue? TODO.
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
			oi_init(); //add parameters
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
			sonar_calibrate();//TODO
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
	void ir_system(){
		switch(USART_Receive())
		{
			case 0:
				IR_init();
				break;
			case 1:
				IR_calibrate(); //add parameter
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

void signal_handler(){
	int id = USART_Receive();
	subsystem_callers[id]();
}



#warning "TODO: RX ISR to modify the `control` global variable."

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





void dispatch()
{
	while(1) {
		handlers[controller.current]();
	}
}

}