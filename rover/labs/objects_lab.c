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
#include "objects.h"



static void part1()
{
	#define RX_BUFSIZE 50
	static char buf[RX_BUFSIZE];
	
	uint8_t deg;
	uint8_t len;
	
	
	for (deg = 0; deg <= 180; deg += 1)
	{
		lcd_puts(".");
		servo_angle(deg, true);
		// TODO: reading sonar a second time causes system to halt.
		//snprintf(buf, RX_BUFSIZE, "%3d, %d, %.1f, ", deg, IR_run(), (double) sonar_reading());
		//snprintf(buf, RX_BUFSIZE, "%3d, %.1f, ", deg, (double) sonar_reading());  // DEBUG
		snprintf(buf, RX_BUFSIZE, "%3d, %d, ", deg, IR_run());  // DEBUG
		USART_transmit_buffer(buf);
		USART_transmit_buffer("\r\n");
	}
}





static void part2()
{
		#define MY_RX_BUFSIZE 20
		static char buf[MY_RX_BUFSIZE];
		
		uint8_t deg;
		uint8_t len;
		
		
		for (deg = 0; deg <= 180; deg += 1)
		{
			objects_scan();
			// TODO
			USART_transmit_buffer(buf);
			USART_transmit_buffer("\n");
		}
}

void objects_lab()
{
	init_push_buttons();
	IR_init();
	sonar_init();
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