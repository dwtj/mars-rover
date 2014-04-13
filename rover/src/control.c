/*
 * control.c
 *
 * Created: 4/13/2014 3:48:46 PM
 *  Author: asr
 */

void null_handler() {
	;  // Do Nothing.
}

void ping_handler() {
	#warning "TODO"
}

void error_handler() {
	#warning "TODO"
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
};

void dispatch()
{
	while(1) {
		handlers[control.current]();
	}
}