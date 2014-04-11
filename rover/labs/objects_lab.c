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
#include <stdlib.h>

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
	
	for (deg = 0; deg <= 180; deg += 1)
	{
		lcd_puts(".");
		servo_angle(deg, true);
		snprintf(buf, RX_BUFSIZE, "%3d, %3.1f, %3.1f, ", deg, (double) IR_reading(), (double) sonar_reading());
		USART_transmit_buffer(buf);
		USART_transmit_buffer("\r\n");
		
		wait_ms(100);  // Needed to prevent the sonar from being called too frequently.
	}
}





static void part2()
{
	#define MY_RX_BUFSIZE 200
	static char buf[MY_RX_BUFSIZE];
		
	scan_results results = objects_scan();
	for (int i = 0; i < results.n; i++) {
		snprint_object(buf, MY_RX_BUFSIZE, results.objects + i);
		USART_transmit_buffer(buf);
		USART_transmit_buffer("\n\n");
	}
	
	free(results.objects);
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
	while(1) {
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
}