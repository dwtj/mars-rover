/**
 * clock-intr.c: timer- and interrupt-based clock program
 * This is skeleton code. You need to make it complete.
 *
 * @author Zhao Zhang
 * @date 06/26/2012
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "lcd.h"
#include "util.h"
#include "timer.h"

volatile static unsigned char hours = 12, minutes = 0, seconds = 0;

static void display_time(void) {
	lprintf("%02d:%02d:%02d", hours, minutes, seconds);
}


static void increment_hours(void)
{
	hours++;
	if (hours >= 13){
		hours = 1;
	}
}

static void increment_minutes(void)
{
	minutes++;
	if (minutes >= 60) {
		increment_hours();
		minutes = 0;
	}
}

static void increment_seconds(void)
{
	seconds++;
	if (seconds >= 60){
		increment_minutes();
		seconds = 0;
	}
}


static void decrement_hours(void)
{
	if (hours <= 1) {
		hours = 12;
		} else {
		hours --;
	}
}

static void decrement_minutes(void)
{
	if (minutes == 0) {
		decrement_hours();
		minutes = 59;
		} else {
		minutes--;
	}
}

static void decrement_seconds(void)
{
	if (seconds == 0) {
		decrement_minutes();
		seconds = 59;
	} else {
		seconds--;
	}

}


/* adjusts the time stored in the global variables according to what button the user is pushing. */
static void adjust_time(void)
{
	static unsigned char old_button = 0, button = 0;
	old_button = button;
	button = read_push_buttons();
			
	if (button == old_button) {
		switch (button) {
			case 1:
				decrement_seconds();
				break;
		
			case 2:
				increment_seconds();
				break;
				
			case 3:
				decrement_minutes();
				break;
				
			case 4:
				increment_minutes();
				break;
			
			case 5:
				decrement_hours();
				break;
				
			case 6:
				increment_hours();
				break;
				
			default:
				break;  // do nothing
		}			
	}
}


/**
 * Timer interrupt source 1: the function will be called every one second
 * (you need define CLOCK_COUNT correctly) 
 */
ISR (TIMER1_COMPA_vect)
{
	increment_seconds();
	display_time();
}


/**
 * Timer interrupt source 2: the function will be called every 200 milliseconds
 * (you need define CHECK_COUNT correctly) 
 */
ISR (TIMER3_COMPA_vect)
{
	adjust_time();
	display_time();
}


/// Displays the time of day on the LCD.
// Implements a "busy" clock, that is, without interrupts.
/**
 * Displays the time of day on the LCD in the format "HH:MM:SS".  Allows the user to adjust the time using
 * the push buttons.  The push buttons will be checked every 200ms, and will update the screen every time
 * the time is adjusted. Only uses the left-most button as control.
 *
 * SW6 increment hours
 * SW5 decrement hours
 * SW4 increment minutes
 * SW3 decrement minutes
 * SW2 increment seconds
 * SW1 decrement seconds
 */

void part1()
{
	init_push_buttons();
	lcd_init();

	// Make initial time be 05:30:00.
	hours = 5;
	minutes = 30;
	seconds = 0;
	
	// `sub_second` counts how many of fifths of a second have passed since the last
	// time that `second` was incremented.
	unsigned char sub_second = 0;

	while (1) {
		
		sub_second = (sub_second + 1) % 5;
		if (sub_second == 0) {
			increment_seconds();  // updates global variables from time passing
		}
		adjust_time();  // updates global variables from buttons being pushed.
	
		display_time();
		wait_ms(200);
	}
}



/// Displays the time of day on the LCD
// Implements the clock using interrupts.
/**
 * Displays the time of day on the LCD in the format "HH:MM:SS".  Allows the user to adjust the time using
 * the push buttons.  The push buttons will be checked every 200ms, and will update the screen every time
 * the time is adjusted.
 *
 * SW6 increment hours
 * SW5 decrement hours
 * SW4 increment minutes
 * SW3 decrement minutes
 * SW2 increment seconds
 * SW1 decrement seconds
 */
void part2() {
	timer_init();
	while (1){
		;
	}
}


// TODO
void part3() {
	;
}


int main() {

	lcd_init();
	//timer_init();  // ???
	init_push_buttons();
	
	char prog;
	
	while (1) {	
		lcd_clear();
		prog = wait_button("Choose program:");
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
				;  // do nothing
		}
	}
}
