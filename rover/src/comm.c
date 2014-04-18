/*
 * comm.c
 *
 * Created: 2/20/2014 10:17:36 AM
 *  Author: Flavia and David
 */


#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "comm.h"
#include "lcd.h"
#include "util.h"
#include "control.h"



bool is_valid_signal(signal sig) {
	return 0 <= sig && sig < NUM_SIGNAL_CODES;
}