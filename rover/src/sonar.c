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

#include "util.h"


/**
 * Holds the number of counter ticks that transpired while the pin was high
 * as measured by the most recent sonar reading.
 */

volatile static uint16_t sonar_signal_width;
volatile static bool sonar_signal_width_ready;




/**
 * If `edge` is logical true, then subsequent rising edges from the sonar sensor will
 * generate events. Otherwise, subsequent falling edges will generate events.
 */
static void select_event_edge(bool edge)
{
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
    #warning "Function not yet implemented."
	
	TCCR1A = 0x00;  // Set compare output mode to normal on each of the three
					// channels. Also sets waveform generation mode bits are
					// cleared.
	
	TCCR1B = 0x00; 
	
	// The two additional waveform generation mode bits register B are also
	// cleared, so the timer will be in normal mode.
	
	TCCR1B |= 0x02;  // Set the prescaler (i.e. three LSBs) s.t. freq. will be
					 // one eighth of the system clock.
	
	select_event_edge(true);  // Set the rising edge to generate events.
	IC_event_enable(false);
	
	TCCR1B |= 0x80;  // Enable input noise canceler.
	
	// Do not use TCCR1C.
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
 * Ensure that relevant IC interrupt have been disabled, so that their
 * handers will not be called because of the pulse being generated.
 */
void sonar_pulse()
{
	uint8_t mask = 0b00010000;
	DDRA |= mask;  // make pin 4 output; 0 means input, 1 means output
    PORTA |= mask; // set pin 4
	
	wait_ms(1);  // TODO: make this smaller.
	
	mask = ~mask;
    PORTA &= mask; // clear pin 4
    DDRA &= mask;  // make pin 4 an input again.
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








/**
 * Performs a single sonar reading, returning a measure of the distance to the
 * nearest object. If `wait_after` is `true`, then the function will wait after
 * the reading has been performed for long enough that it is safe to
 * immediately start another reading.
 */
uint16_t sonar_reading(bool wait_after)
{
	IC_event_enable(false);
	sonar_pulse();
	IC_event_enable(true);
	
	select_event_edge(true);  // Listen for the next rising edge.
	
    uint16_t d;

	while (!sonar_signal_width_ready){
		;  // wait
	}

    d = time_to_dist(ticks_to_time(sonar_signal_width));
	
	sonar_signal_width_ready = false;

    if (wait_after == true) {
        wait_ms(1);
    }
	
	IC_event_enable(false);

    return d;
}





ISR(TIMER1_CAPT_vect) {
	static uint16_t counter = 0;
	if (PORTA & 0b00010000) {
		// Handle rising edge event:
		counter = ICR1;
		select_event_edge(false);  // Listen for the next falling edge.
	} else {
		// Handle falling edge event:
		sonar_signal_width = counter - ICR1; //get difference of clock counter
		sonar_signal_width_ready = true;
	}
}