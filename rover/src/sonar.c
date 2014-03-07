/**
 * sonar.c
 * By David Johnston
 *
 * The `PING)))` sensor datasheet says that measurement range is 2cm to 3m. It
 * also says that the echo pulse has a minimum length of 115μs and a maximum
 * length of 18.6ms.
 *
 * We desire 0.5cm precision in our mesurements.
 *
 * The prescaler was chosen such that (a) we have at least 0.5cm precision in
 * our measurements, and (b) two or more counter overflows will not occur for a
 * measurement of any valid range, i.e., 2cm - 3m.
 */




/**
 * Initializes counter 1 so that we can measure for how how long the sonar
 * sensor will assert.
 */
void sonar_init(void)
{
    #warning "Function not yet implemented."


    // set counter prescaler
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
 * This function temporarily disables some relevant interupts, so that their
 * handers will not be called because of the pulse being generated.
 */
void sonar_pulse()
{
    #warning "Function not yet implemented."

    // disable interrupt
    // change direction
    // set
    // reset
    // restore direction
    // re-enable interrupt
}





/**
 * Assuming that a pulse was just sent to the sonar sensor, this function polls
 * the input pin, waiting for the the value on the pin to rise then fall. It
 * returns the number of counter ticks that transpired while the pin was high.
 */

static uint16_t sonar_signal_width(void)
{
    #warning "Function not yet implemented."
}




/**
 * Gives the length of time that it takes for `n` ticks to transpire, measured
 * in microseconds, according to the prescaler being used.
 */
static uint16_t ticks_to_time(uint16_t n)
{
    #warning "Function not yet implemented."
}






/**
 * Gives distance from the object, given that `t` microseconds transpired
 * before the echo returned to the SONAR sensor.
 */
static uint16_t time_to_dist(uint16_t t)
{
    #warning "Function not yet implemented."
}






/**
 * Performs a single sonar reading, returning a measure of the distance to the
 * nearest object. If `wait_after` is `true`, then the function will wait after
 * the reading has been performed for long enough that it is safe to
 * immediately start another reading.
 */
static uint16_t sonar_reading(bool wait_after)
{
    uint16_t d;

    sonar_pulse();
    d = time_to_dist(ticks_to_time(sonar_signal_width()));

    if (wait_after == true) {
        wait_ms(1);
    }

    return d;
}
