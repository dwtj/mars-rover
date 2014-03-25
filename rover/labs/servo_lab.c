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

void servo_lab()
{
	lcd_init();
	lcd_puts("Servo Lab");
	servo_init();
	servo_angle(0, true);
	/*
	while(1){
			lprintf("%u", TCNT3);
			wait_ms(500);
	}*/
}