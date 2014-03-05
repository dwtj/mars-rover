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



void IR_lab() {
	lcd_init();
	IR_init();
	
	while(1){
		uint16_t v = IR_run();
		lprintf("%" PRIu16 ", %f", v, IR_analytical_conv(v));
		wait_ms(1000); // wait 1s before next call
	}
}