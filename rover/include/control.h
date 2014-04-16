/*
 * control.h
 *
 * Created: 4/13/2014 3:48:58 PM
 *  Author: asr
 */ 


#ifndef CONTROL_H_
#define CONTROL_H_

#define MAX_DATA_LENGTH 100
typedef struct{
	bool running; //Checked by handlers to see if they should stop.
	signal current; //Stores the most recent command issued from python.
	uint8_t data[100];
	uint8_t data_idx;
}control;



#endif /* CONTROL_H_ */