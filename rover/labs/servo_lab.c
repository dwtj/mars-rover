/**
 * servo_lab.c
 */

#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>

#include "servo.h"
#include "sonar.h"
#include "util.h"

#include "lcd.h"

static void part1() {
	servo_angle(0, true);
	/*
	while(1){
			lprintf("%u", TCNT3);
			wait_ms(500);
	}*/
}

static void part2() {
	servo_manual_calib();
}

static void part3()
{
	while(1)
	{
		// SW1 -> 0 degrees, SW2 -> 45 degrees, SW3 -> 90 degrees, 
		// SW4 -> 135 degrees, SW5 -> 180 degrees 
		uint8_t prog = wait_button("Select Angle...");

		switch (prog) {
		case 1:
			servo_angle(0, false);
			break;
		case 2:
			servo_angle(45, false);
			break;
		case 3:
			servo_angle(90, false);
			break;
		case 4:
			servo_angle(135, false);
			break;
		case 5:
			servo_angle(180, false);
			break;
		default:
			lcd_puts("Invalid Angle.");
	
		}
	}
}



void servo_lab()
{
	init_push_buttons();
	lcd_init();
	servo_init();
	
	lcd_puts("Servo Lab Startup...");
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
	case 3:
		part3();
		break;
	default:
		lcd_puts("Invalid Program.");
	}
}