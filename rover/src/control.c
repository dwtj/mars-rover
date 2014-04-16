/*
 * control.c
 *
 * Created: 4/13/2014 3:48:46 PM
 *  Author: asr
 */

//IMMEDIATE TODO: IMPLEMENT PING, implement echo, DETERMINE WHERE SUBSYSTEM_ID, SIGNAL_ID, how to read the fricken data frame in general.

//TODO: MANY OF THE SUBSYSTEM COMMANDS MAY NEED ARGUMENTS.

#include "util.h"
#include "util.c"

void null_handler() {
	;  // Do Nothing.
}

void ping_handler() {
	#warning "TODO"
}

void error_handler() {
	rError(error_bad_request,"Bad signal request.");//TODO? Does this need to be more generic? or possibly more specific
}

void signal_handler(){
	//TODO ERROR FIXME
		subsystem_callers[SUBSYSTEM_ID]();//THIS CODE IS WRONG.
		//where is the subsystem information being stored? 
}

/*
void init_handler() {
	switch (control.data[0]) {
	case subsystems.lcd:
		lcd_init();
		break;
	case subsystems.oi:
		oi_init();
		break;
	case subsystems.sonar:
		sonar_init();
		break;
	case subsystems.servo:
		servo_init();
		break;
	case subsystems.ir:
		IR_init();
		break;
	case subsystems.rng:
		rng_init();
		break;
	
	default:
		rError(error.error_bad_request, "Bad init request.");
	}
}
*/


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

void lcd_system(){
	switch(COMMAND_ID)
	{
		case 0:
			lcd_init();
			break;
		case 1:
			lcd_puts(data[0]);
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
	switch(COMMAND_ID)
	{
		case 0:
			oi_init();
			break;
		default:
			rError(error_bad_request, "Bad OI Command");
		break;
	}
}

void sonar_system(){
	switch(COMMAND_ID)
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
	switch(COMMAND_ID)
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
			servo_angle();
			break;
		case 4:
			servo_pulse_width();//TODO
			break;
		default:
			rError(error_bad_request, "Bad servo Command");
		break;
	}
void ir_system(){
	switch(COMMAND_ID)
	{
		case 0:
			IR_init();
			break;
		case 1:
			IR_calibrate();
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
	switch(COMMAND_ID)
	{//TODO?
		default:
		rError(error_bad_request, "Bad RNG Command");
		break;
	}
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
void dispatch()
{
	while(1) {
		handlers[control.current]();
	}
}