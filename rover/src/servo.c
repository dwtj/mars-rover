/*
 * servo.c
 *
 * Created: 3/14/2014 3:45:46 PM
 *  Author: dwtj
 */ 

const uint16_t TOP = 43000; //period is 21.5ms

void servo_init(){
	TCCR3A = 0b00100011; //waveform generation mode 15, output compare from channel B
	TCCR3B = 0b00011010; //1/8 prescaler
	TCCR3C = 0; //TODO
	ETIMSK = 0;
	OCR3A = TOP;	//set top
}


/**
 * Expects a floating point value (strictly) between 0.0 and 1.0.
 */
static void set_pulse_proportion(float p){
	OCR3B = (uint16_t) (TOP * p);
}