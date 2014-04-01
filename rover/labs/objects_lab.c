/*
 * objects.c
 *
 * Created: 4/1/2014 4:20:08 PM
 *  Author: dwtj
 */ 

/**
 * Sends raw sonar and IR distance readings as the tower scans from 0 degrees to 180 degrees.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>

#include "labs.h"
#include "servo.h"
#include "sonar.h"
#include "usart.h"
#include "lcd.h"
#include "util.h"
#include "IR.h"



static void part1()
{
	#define RX_BUFSIZE 20
	static char buf[RX_BUFSIZE];
	
	uint8_t deg;
	uint8_t len;
	
	
	for (deg = 0; deg <= 180; deg += 2)
	{
		servo_angle(deg, true);
		snprintf(buf, RX_BUFSIZE, "%3d, ", deg);
		USART_transmit_buffer(buf);
		USART_transmit_buffer("\n");
	}
}


static void part2()
{
	
	
}

void objects_lab()
{
	init_push_buttons();
	lcd_init();
	servo_init();
	USART_Init(1);  // init bluetooth
			
	lcd_puts("Objects Lab Startup...");
	wait_ms(1000);
	lcd_clear();

	uint8_t prog = wait_button("Select Program...");
	switch (prog) {
	case 1:
		part1();
		break;
	case 2:
		part2();
		break;
	default:
		lcd_puts("Invalid Program.");
	}
}