/*
 * comm.c
 *
 * Created: 2/20/2014 10:17:36 AM
 *  Author: Flavia and David
 */


#include <stdbool.h>

#include "comm.h"




bool is_valid_signal(signal sig) {
	return 0 <= sig && sig < NUM_SIGNAL_CODES;
}