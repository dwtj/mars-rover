/**
 * sonar.c
 * By David Johnston
 *
 * The `PING)))` sensor datasheet says that measurement range is 2cm to 3m. It
 * also says that the echo pulse has a minimum length of 115μs and a maximum
 * length of 18.6ms.
 *
 * The prescaler was chosen such that (a) we have at least 0.5cm precision in
 * our measurements, and (b) two or more counter overflows will not occur for a
 * measurement of any valid range, i.e., 2cm - 3m. Choosing the one-eighth
 * prescaler satisfies these requirements.
 */



#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "util.h"
#include "lcd.h"


/**
 * Holds the number of counter ticks that transpired while the pin was high
 * as measured by the most recent sonar reading.
 */

volatile static uint16_t sonar_echo_width = 0;
volatile static enum {NOT_READY, READY, LOW, HIGH, DONE, FAILED} sonar_state = NOT_READY;
static char *sonar_state_labels[] = {"Not Ready", "Ready", "Low", "High", "Done", "Failed"};




/**
 * If `edge` is logical true, then subsequent rising edges from the sonar sensor will
 * generate events. Otherwise, subsequent falling edges will generate events.
 */
static void select_event_edge(bool edge)
{
	// A zero bit on ICNCn (i.e. TCCR1B bit 6) will cause falling edges to raise
	// events, while a one bit will cause rising edges to raise events.
	if (edge) {
		TCCR1B |= 0x01000000;
	} else {
		TCCR1B &= 0b10111111;
	}
}



/**
 * If `b` is logical true, then input capture events are enabled. Else, these
 * events are disabled.
 */
static void IC_event_enable(bool b)
{
	if (b) {
		TIMSK |= 0b00100000;
	} else {
		TIMSK &= 0b11011111;
	}
}



static bool is_IC_event_enabled() {
	return TICIE1 & 0b00100000;
}



/**
 * Initializes Timer/Counter 1 so that we can measure for how how long the sonar
 * sensor will assert.
 */
void sonar_init(void)
{	
	TCCR1A = 0x00;  // Set compare output mode to normal on each of the three
					// channels. Also sets waveform generation mode bits are
					// cleared.
	
	TCCR1B = 0x00; 
	
	// The two additional waveform generation mode bits in register B are also
	// cleared, so the timer will be in normal mode.
	
	TCCR1B |= 0x02;  // Set the prescaler (i.e. three LSBs of TCCR1B) s.t. the
					 // freq. will be one eighth of the system clock.
	
	select_event_edge(true);  // Set the rising edge to generate events.
	IC_event_enable(false);   // Initially, events do not cause interrupts.
	
	TCCR1B |= 0x80;  // Enable input noise canceler.
	
	TCCR1C = 0x00;   // Not using output compare, so we do not use TCCR1C.
	
	sonar_state = READY;
}




/**
 * Subsequent functions in this file presume that the initializations of
 * `sonar_init()` have been successfully performed. They also assume that only
 * one sonar reading is being attempted at a time, and that sufficient time
 * (i.e. 200μs) is allowed to elapse between readings.
 */



/**
 * Sends a pulse to the sonar sensor. This will initiates a reading on the
 * sonar sensor if a reading is not already under way and a sufficient delay
 * (200 microseconds) has passed since the last reading was completed.
 *
 * Before use, ensure that relevant IC interrupt have been disabled, so that
 * their handers will not be called because of the pulse being generated.
 */
void sonar_pulse()
{
	uint8_t mask = 0b00010000;
	DDRD |= mask;  // make pin 4 output; 0 means input, 1 means output
    PORTD |= mask; // set pin 4
	
	wait_ms(1);  // TODO: make this smaller.
	
	mask = ~mask;
    PORTD &= mask; // clear pin 4
    DDRD &= mask;  // make pin 4 an input again.
}




 /**
  * Assuming that a pulse was just sent to the sonar sensor, this function polls the
  * the input pin, waiting for the value on the pin to rise then fall. It returns
  * the number of counter ticks that transpired while the pin was high.
  */
static uint16_t get_sonar_signal_width()
{
	uint16_t start = 0;
	uint8_t mask = 0b00010000;  // Used to select data from pin 4.
	
	lcd_clear();
	lcd_puts("Waiting for Rising Edge.");
	while((PORTD & mask) == 0) {
		;  // do nothing
	}
	start = TCNT1;
	
	lcd_clear();
	lcd_puts("Waiting for Falling Edge.");
	while ((PORTD & mask) != 0) {
		;  // do nothing
	}
	lcd_clear();
	
	return TCNT1 - start;
}


/**
 * Gives the length of time that it takes for `n` ticks to transpire, measured
 * in microseconds, according to the prescaler being used. Results round down.
 */
static uint16_t ticks_to_time(uint16_t n) {
    return n / 2;
}




/**
 * Gives distance in cm from the object, given that `t` microseconds transpired
 * before the echo returned to the SONAR sensor.
 */
static uint16_t time_to_dist(uint16_t t) {
    return 34000 * t;
}



uint16_t sonar_reading_polling() {
	IC_event_enable(false);  // disable IC events
	sonar_pulse();
	return time_to_dist(ticks_to_time(get_sonar_signal_width()));
}


char *sonar_get_state() {
	return sonar_state_labels[sonar_state];
}



/**
 * Performs a single sonar reading, returning a measure of the distance to the
 * nearest object. The function will wait after the reading has been performed
 * for long enough that it is safe to immediately start another reading.
 *
 * A zero will be returned if the sonar reading process failed.
 */
uint16_t sonar_reading()
{
	uint16_t d;
	
	if (sonar_state != READY) {
		return 0;
	}
	
	select_event_edge(true);  // Listen for the next rising edge.
	IC_event_enable(false);
	sonar_pulse();
	IC_event_enable(true);

	if ((PORTD & 0b00010000) != 0x00) {
		return 0;
	}
	
	// Sonar pin is currently low. Waiting for echo pulse.
	sonar_state = LOW;  

	while (sonar_state != DONE) {
		// Wait for rising and falling handlers to complete
		// TODO: fail if two overflow events occur.
		if (sonar_state == FAILED) {
			return 0;
		}
	}

	d = sonar_echo_width;
	sonar_state = NOT_READY;
	
    d = time_to_dist(ticks_to_time(sonar_echo_width));
	
	wait_ms(1);  // TODO: Make this smaller.

	sonar_state = READY;
	
	IC_event_enable(false);

    return d;
}




ISR (TIMER1_CAPT_vect)
{
	if ((sonar_state == LOW) && (PORTD & 0b00010000))
	{
		// The sonar sensor's state has risen from low to high:
		sonar_state = HIGH;
		sonar_echo_width = ICR1;
		select_event_edge(false);  // Listen for the next falling edge.
	}
	else if ((sonar_state == HIGH) && ((PORTD & 0b00010000) == 0x00))
	{
		// The sonar sensor's state has fallen from high to low, so the reading is done:
		sonar_echo_width -= ICR1; // get difference of clock counter
		sonar_state = DONE;
	}
	else
	{
		wait_button("here");
		sonar_echo_width = 0;
		sonar_state = FAILED;
	}
}