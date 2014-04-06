/*
 * objects.c
 *
 * Created: 4/4/2014 4:26:36 PM
 *  Author: dwtj
 */ 


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


#include "servo.h"
#include "sonar.h"
#include "IR.h"
#include "objects.h"


/************************************************************************/
/* Definitions for the object scan finite state machine.                */
/************************************************************************/


typedef struct {
	float sonar;
	uint16_t ir;
} readings;


// The maximum number of degrees which can be recorded for a single object.
#define CUR_OBJECT_WIDTH_MAX 30

// The maximum supported number of objects that the FSM can find in a scan.
#define MAX_OBJECTS 10


#define NUM_STATES 4
typedef enum {A = 0, B = 1, C = 2, D = 3} scan_FSM_states;



typedef struct {
	scan_FSM_states state;    // The current state that this scan_FSM is in.
	object objects[MAX_OBJECTS];  // The object that have so far been found by this current scan.
	uint8_t objects_found;    // The number of object found in the current scan, and their scanning process is **complete**.
	uint8_t cur_angle;        // The current angle at which the tower is directed. Range is 0 to 180 degrees (inclusive).
	readings cur_object_readings[CUR_OBJECT_WIDTH_MAX];  // Readings from each degree since the beginning of the object currently being observed.
	uint8_t cur_object_width;  // The number of degrees since the beginning of the object currently being observed.
} scan_FSM;



static void transA(scan_FSM *fsm);
static void transB(scan_FSM *fsm);
static void transC(scan_FSM *fsm);
static void transD(scan_FSM *fsm);


void (*scan_FSM_trans[NUM_STATES]) (scan_FSM *state) = {transA, transB, transC, transD};




/************************************************************************/
/* Helper functions for the object scan finite state machine.           */
/************************************************************************/

void FSM_init(scan_FSM *fsm, uint8_t init_angle) {
	fsm->state = A;
	fsm->objects_found = 0;
	fsm->cur_angle = init_angle;
	fsm->cur_object_width = 0;
};



float object_dist(uint8_t n, readings rs[n]) {
	// This implementation averages together all of the readings from both IR and sonar.
	float sum = 0;
	for (int i = 0; i < n; i++) {
		sum += rs[i].sonar;
		sum += rs[i].ir;
	}
	return sum / (2 * n);
}

float object_width(uint8_t theta1, uint8_t theta2, float dist) {
	#warning "`object_width()` is implementing angular rather than linear width."
	return abs(theta2 - theta1);  // TODO: make this linear width instead of angular width.
}


void object_found_cleanup(scan_FSM *fsm) {
	// Another object has been found.
	fsm->objects[fsm->objects_found].dist = object_dist(fsm->cur_object_width, fsm->cur_object_readings);
	fsm->objects_found++;
	
	// The readings in `fsm->cur_object_readings[]` buffer is now garbage.
	fsm->cur_object_width = 0;
}


static bool is_object_there(readings *rs)
{
	const uint8_t low = 30, high = 85;
	return (low <= rs->ir && rs->ir <= high) && (low <= rs->sonar && rs->sonar <= high);
}




/************************************************************************/
/* Transition functions for the object scan finite state machine.       */
/************************************************************************/

static void transA(scan_FSM *fsm)
{
	readings rs = {.ir = IR_run(), .sonar = sonar_reading()};
	if (is_object_there(&rs)) {
		// A *potential* new object was found.
		// TODO: bail-out because of too-many objects found!
		fsm->objects[fsm->objects_found].theta1 = fsm->cur_angle;
		fsm->cur_object_readings[0] = rs;
		fsm->cur_object_width = 1;
		fsm->state = B;
	} else {
		fsm->state = A;
	}
}

static void transB(scan_FSM *fsm)
{
	readings rs = {.ir = IR_run(), .sonar = sonar_reading()};
	if (is_object_there(&rs)) {
		// Now convinced that the potential object is an actual object.
		// Add another reading to the data structure.
		fsm->cur_object_readings[1] = rs;
		fsm->cur_object_width = 2;
		fsm->state = C;
	} else {
		// The potential object was not actually there.
		fsm->cur_object_width = 0;
		fsm->state = A;
	}
}

static void transC(scan_FSM *fsm)
{
	// TODO: bail out if the width is too wide for the available buffer:
	// if (fsm->cur_object_width == MAX_OBJECT_WIDTH - 1)
	//     do something
	
	readings rs = {.ir = IR_run(), .sonar = sonar_reading()};
	if (is_object_there(&rs))
	{
		// Still looking at an actual object and waiting for the end of it.
		fsm->cur_object_readings[fsm->cur_object_width] = rs;
		fsm->cur_object_width++;
		
		fsm->state = C;
	}
	else
	{
		// There is *potentially* no more of the object to be observed, i.e., the previous
		// angle was the last angle that was part of the object.

		fsm->objects[fsm->objects_found].theta2 = fsm->cur_angle - 1;
		
		// However, we might still be looking at the object, so we record another reading.
		fsm->cur_object_readings[fsm->cur_object_width] = rs;
		fsm->cur_object_width++;
		
		fsm->state = D;
	}
}

static void transD(scan_FSM *fsm)
{
	readings rs = {.ir = IR_run(), .sonar = sonar_reading()};
	if (is_object_there(&rs))
	{
		// TODO: bail out if the width is too wide for the available buffer:
		// if (fsm->cur_object_width == MAX_OBJECT_WIDTH - 1)
		//     do something
		
		// The last reading was erroneous. Still looking at an actual object and waiting for the end of it.
		fsm->cur_object_readings[fsm->cur_object_width] = rs;
		fsm->cur_object_width++;
		
		fsm->state = C;
	}
	else 
	{
		// Convinced that no object is there anymore.
		// Previously stored `theta2` was correct.
		object_found_cleanup(fsm);
		
		fsm->state = A;
	}
}






/************************************************************************/
/* Initiates the object scan finite state machine.                      */
/************************************************************************/

scan_results objects_scan()
{
	scan_results rv;
	scan_FSM fsm;
	FSM_init(&fsm, 0);  // Starting at degree zero.
		
	for (uint8_t deg = 0; deg <= 180; deg += 1)
	{
		servo_angle(deg, true);
		fsm.cur_angle = deg;
		scan_FSM_trans[fsm.state](&fsm);
	}
	
	// Copy results into `rv`:
	rv.n = fsm.objects_found;
	rv.objects = malloc(rv.n * sizeof(object));
	for (uint8_t i = 0; i < rv.n; i++) {
		rv.objects[i] = fsm.objects[i];
	}
	
	return rv;
}