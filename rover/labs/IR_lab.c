/*
 * IR_lab.c
 *
 * Created: 2/21/2014 1:35:24 PM
 *  Author: flaviarc and David
 */ 

#include "lcd.h"
#include "util.h"
#include "usart.h"
#include "IR.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>



void IR_test_analytical_conv()
{
	while(1) {
		uint16_t d = IR_run();
		lprintf("%" PRIu16 ", %f", d, IR_analytical_conv(d));
		wait_ms(1000); // wait 1s before next call
	}
}


void IR_test_calib_conv()
{
        // Send calibration data over BAM, and do not save means:
	IR_calibrate(true, false);

	while(true) {
		uint16_t d = IR_run();
		lprintf("%" PRIu16 ", %f", d, IR_conv(d));
		wait_ms(1000); // wait 1s before next call
	}
}



void IR_lab()
{
	char prog;
	
	lcd_init();
	lcd_clear();
	IR_init();
	
	prog = wait_button("Choose program:");
	switch (prog) {
		case 1:
			IR_test_analytical_conv();
			break;
		case 2:
			IR_test_calib_conv();
			break;
		case 3:
			//part3();
			break;
		default:
			;  // do nothing
	}
}
