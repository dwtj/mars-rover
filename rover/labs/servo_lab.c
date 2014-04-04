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
	uint8_t angles[] = {0, 45, 90, 135, 180};
	while(1)
	{
		// SW1 -> 0 degrees, SW2 -> 45 degrees, SW3 -> 90 degrees, 
		// SW4 -> 135 degrees, SW5 -> 180 degrees 
		
		uint8_t prog = wait_button("Select Angle...");

		/*
		if (1 <= prog && prog <= 5) {
			servo_angle(angles[prog], true);
		} else {
			lcd_puts("Invalid Angle.");
			wait_ms(1500);
		}
		*/
		

		switch (prog) {
		case 1:
			servo_angle(0, true);
			break;
		case 2:
			servo_angle(45, true);
			break;
		case 3:
			servo_angle(90, true);
			break;
		case 4:
			servo_angle(135, true);
			break;
		case 5:
			servo_angle(180, true);
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