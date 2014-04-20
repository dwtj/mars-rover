/*
 * txq.h
 *
 * Created: 4/13/2014 3:12:23 PM
 *  Author: asr
 */ 


#ifndef TXQ_H_
#define TXQ_H_

#include <stdint.h>

volatile uint8_t ISR2_flag;

/**
 * A queue to store transmission from rover to control.
 * Functions related to its usage are prefixed with txq.
 */
#define TXQ_BUFF_SIZE 255
typedef struct{
	uint8_t buff[TXQ_BUFF_SIZE];
	uint8_t write_index;  // Index of next element to be added to the queue.
	uint8_t read_index;   // Index of next element to be removed from the queue.
	uint8_t num_elements;
} txq_t;

void txq_init();

void txq_drain();

uint8_t txq_dequeue();


void txq_enqueue(uint8_t val);



#endif /* TXQ_H_ */
