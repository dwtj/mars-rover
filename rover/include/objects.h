/*
 * objects.h
 *
 * Created: 4/4/2014 4:26:54 PM
 *  Author: dwtj
 */ 


#ifndef OBJECTS_H_
#define OBJECTS_H_

// An object observed by the scan_FSM.
typedef struct {
	uint8_t theta1;
	uint8_t theta2;
	float dist;
	float width;
} object;

typedef struct {
	object *objects;  // The array of objects that was found
	uint8_t n;    
} scan_results;

scan_results objects_scan();

#endif /* OBJECTS_H_ */