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
#include <stdbool.h>

#include "util.h"
#include "lcd.h"
#include "r_error.h"
#include "sonar.h"
#include "usart.h"
#include "control.h"


/**
 * Holds the number of counter ticks that transpired while the pin was high
 * as measured by the most recent sonar reading.
 */

volatile static enum {NOT_READY, READY, LOW, HIGH, DONE, FAILED} sonar_state = NOT_READY;
static char *sonar_state_labels[] = {"Not Ready", "Ready", "Low", "High", "Done", "Failed"};
typedef enum {FALLING, RISING} edge_t;

volatile uint16_t sonar_echo_rise = 0;
volatile uint16_t sonar_echo_fall = 0;




/**
 * Makes the given `edge` generate events.
 */
static void select_event_edge(edge_t edge)
{
	// A zero bit on ICNCn (i.e. TCCR1B bit 6) will cause falling edges to raise
	// events, while a one bit will cause rising edges to raise events.
	if (edge == RISING) {
		TCCR1B |= 0b01000000;
	} else if (edge == FALLING) {
		TCCR1B &= 0b10111111;
	}
}



/**
 * If `b` is logical true, then input capture events are enabled. Else, these
 * events are disabled.
 */
static void sonar_event_enable(bool b)
{
	if (b) {
		TIMSK |= 0b00100000;
	} else {
		TIMSK &= 0b11011111;
	}
}



static bool is_sonar_event_enabled() {
	return TIMSK & 0b00100000;
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
	sonar_event_enable(false);   // Initially, events do not cause interrupts.
	
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
static float time_to_dist(float t) {
    return 0.017 * t; //divide distance conversion by two to account for the journey to and from the object
}



uint16_t sonar_reading_polling() {
	sonar_event_enable(false);  // disable IC events
	sonar_pulse();
	return time_to_dist(ticks_to_time(get_sonar_signal_width()));
}


char *sonar_get_state() {
	return sonar_state_labels[sonar_state];
}

///handler for the sonar. n-> number of readings that we have to be performed, raw-> whether we should return the raw value from 
///ADC, rand-> ignoring this for now, timestamps-> whether we have to send back the time difference in between every taken reading 
void sonar_system()
{
	switch(usart_rx()) {
		case 0:
			sonar_init();
			break;
		case 1:
            sonar_reading_handler();
			break;
		default:
			r_error(error_bad_message, "Bad sonar Command");
			break;
	}
}


void sonar_reading_handler()
{
    // The interface used to access the request parameters encoded in the
    // data frame of the received message:
    struct {
        uint8_t n;
        bool raw;
        bool random;
        bool timestamps;
    } *request = (void *) &control.data;

    if(rx_frame() == true)
    {
        r_error(error_frame, "A sonar reading request message should not have multiple data frames.");
    }	
    if(control.data_len != sizeof(*request)) 
    {
        r_error(error_frame, "Did not receive the anticipated number of data bytes in sonar reading request message.");
    }
    
    // Number of raw readings to be put into a response data frame:
    #define SONAR_RAW_PER_FRAME (DATA_FRAME_MAX_LEN / sizeof(uint16_t))

    // Number of converted data readings to be put into a response data frame:
    #define SONAR_CONV_PER_FRAME (DATA_FRAME_MAX_LEN / sizeof(float))

    // The interface for interpreting `control.data` as an array into which
    // we put either raw data or converted data:
    union {
        uint16_t raw[SONAR_RAW_PER_FRAME];
        float conv[SONAR_CONV_PER_FRAME];
    } *response = (void *) &control.data;

    // An index into a an array of `response`:
    uint8_t i;

    // The number of readings which have been either:
    uint16_t readings_sent = 0;

    while (readings_sent < request->n)
    {
        // Start generating a new response frame.

        if (sonar_request->raw)
        {
            // Place as many raw readings into `control.data` as will fit.
            while (i < SONAR_RAW_PER_FRAME && readings_sent < request->n) {
                response.raw[i] = sonar_raw_reading();
                readings_sent++;
                i++;
            }
        }
        else
        {
            // Place as many converted readings into `control.data` as will fit.
            while (i < SONAR_CONV_PER_FRAME && readings_sent < request->n) {
                response.conv[i] = sonar_reading();
                readings_sent++;
                i++;
            }
        }

        // Frame and send the contents of `control.data` to `control`, and
        // indicate whether a subsequent frame must still be sent as well:
        control.data_len = i;
        tx_frame(readings < request->n);
        txq_drain();
    }
}




/**
 * Performs a single sonar reading, returning a measure of the distance to the
 * nearest object in centimeters. The function will wait after the reading has
 * been performed for long enough that it is safe to immediately start another
 * reading.
 */
float sonar_reading()
{
    return time_to_dist(ticks_to_time(sonar_raw_reading()));
}



/**
 * Performs a single sonar reading, returning the number of clock ticks that
 * transpired while the echo pulse was high. This is an unconverted measure
 * of the distance between the robot and the nearest object detected by the
 * sonar. The function will wait after the reading has been performed
 * for long enough that it is safe to immediately start another reading.
 */
uint16_t sonar_raw_reading()
{
	uint16_t rv;
	
	if (sonar_state != READY) {
		return 0;
	}
	
	sonar_event_enable(false);
	sonar_pulse();
	sonar_event_enable(true);
	
	// Sonar pin is currently low. Waiting for echo pulse.
	select_event_edge(RISING);
	sonar_state = LOW;

	// Wait for rising and falling handlers to complete or for a failure to occur
	while (sonar_state != DONE) {
		// TODO: fail if two overflow events occur while waiting.
		if (sonar_state == FAILED) {
			return 0.0;
		}
	}
	
	sonar_event_enable(false);

	rv = ticks_to_time(sonar_echo_fall - sonar_echo_rise);
	
	sonar_state = NOT_READY;
	wait_ms(1);  // TODO: Make this smaller.
	sonar_state = READY;
	
    return rv;
}





ISR (TIMER1_CAPT_vect)
{
	if (sonar_state == NOT_READY) {
		return;
	}
	
	if ((sonar_state == LOW)/* && (PORTD & 0b00010000) */)
	{
		// The sonar sensor's state has risen from low to high:
		sonar_state = HIGH;
		//sonar_echo_width = ICR1;
		sonar_echo_rise = ICR1;  // DEBUG
		select_event_edge(FALLING);  // Listen for the next falling edge.
	}
	else if ((sonar_state == HIGH)/* && ((PORTD & 0b00010000) == 0x00)*/)
	{
		// The sonar sensor's state has fallen from high to low, so the reading is done:
		//sonar_echo_width = ICR1 - sonar_echo_width;
		sonar_echo_fall = ICR1;  // DEBUG
		sonar_state = DONE;
	}
	else
	{
		sonar_echo_rise = 0;
		sonar_echo_fall = 0;
		sonar_state = FAILED;
	}
}
