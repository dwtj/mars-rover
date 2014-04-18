/*
 * txq.h
 *
 * Created: 4/13/2014 3:12:23 PM
 *  Author: asr
 */ 


#ifndef TXQ_H_
#define TXQ_H_

#include <stdint.h>


/*A queue to store transmission from rover to control
  functions related to this are prefixed with txq*/
#define TXQ_BUFF_SIZE 100
typedef struct{
	uint8_t buff[TXQ_BUFF_SIZE];
	uint8_t write_index;  // points to the next element to be added to the queue.
	uint8_t read_index;   // points to the next element to be removed from the queue.
	uint8_t num_elements;
}txq_t;

void txq_init();

void txq_drain();

uint8_t txq_dequeue();


void txq_enqueue(uint8_t val);



#endif /* TXQ_H_ */