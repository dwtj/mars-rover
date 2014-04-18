/*
 * txq.c
 *
 * Created: 4/13/2014 3:12:06 PM
 *  Author: asr
 */

#include <stdint.h>

#include "txq.h"
#include "util.h"
#include "r_error.h"
#include "usart.h"


static volatile txq_t txq;

void txq_init() {
	txq.read_index=0;
	txq.write_index=0;
	txq.num_elements=0;
}

// Reads and returns an element from the queue and decreases number of elements.
// If there are zero elements in the queue, then a `error_txq` error is sent to control.
uint8_t txq_dequeue()
{
	uint8_t ret;
	
	if(txq.num_elements == 0) {
		r_error(error_txq, "Transmission queue is empty.");
	}
	
	ret = txq.buff[txq.read_index];
	
	// Wrap around if read_index would underflow (go negative).
	if (txq.read_index < TXQ_BUFF_SIZE - 1) {
		txq.read_index++;
	} else {
		txq.read_index = 0;
	}
	
	txq.num_elements--;
	return ret;
}



// Adds an element to the queue and increases number of elements.
// If there are `TXQ_BUFF_SIZE` number of elements in the queue, and the user
// attempts to enqueue another element, then a `error_txq` error is sent to control.
void txq_enqueue(uint8_t val)
{
	if(txq.num_elements == TXQ_BUFF_SIZE) {
		r_error(error_txq, "Transmission queue is full.");
	}
	
	txq.buff[txq.write_index] = val;
	
	// Wrap around if write_index would overflow.
	if(txq.write_index < TXQ_BUFF_SIZE - 1) {
		txq.write_index++;
	} else {
		txq.write_index = 0;
	}
	
	txq.num_elements++;
}



void txq_drain() {
	while (txq.num_elements > 0) {
		usart_tx(txq_dequeue());
	}
}



#warning "TODO: implement the two ISRs to implement the txq."