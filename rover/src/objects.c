/*
 * objects.c
 *
 * Created: 4/4/2014 4:26:36 PM
 *  Author: dwtj
 */ 


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>


#include "servo.h"
#include "sonar.h"
#include "IR.h"
#include "objects.h"
#include "util.h"


/************************************************************************/
/* Definitions for the object scan finite state machine.                */
/************************************************************************/




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



/**
 * This computes the linear width of a circular object using given parameters.
 */
/*
#warning "this implementation does not work."
float object_width(uint8_t theta1, uint8_t theta2, float dist) {
	double x = tan((theta2 - theta1) / 2.0);
	return 2 * dist * x / (1 - x);
}
*/


/**
 * This computes the angular width of the object.
 */
float object_width(uint8_t theta1, uint8_t theta2, float dist) {
	return theta2 - theta1;
}


void object_found_cleanup(scan_FSM *fsm)
{
	// Another object has been found.
	object *cur_obj = fsm->objects + fsm->objects_found;  // This is the object being finalized.
	
	cur_obj->dist = object_dist(fsm->cur_object_width, fsm->cur_object_readings);
	cur_obj->width = object_width(cur_obj->theta1, cur_obj->theta2, cur_obj->dist);
	fsm->objects_found++;
	
	// The readings in `fsm->cur_object_readings[]` buffer is now garbage.
	fsm->cur_object_width = 0;
}


static bool is_object_there(readings *rs)
{
	const uint8_t low = 5, high = 92;
	return (low <= rs->ir && rs->ir <= high) && (low <= rs->sonar && rs->sonar <= high);
}




/************************************************************************/
/* Transition functions for the object scan finite state machine.       */
/************************************************************************/

static void transA(scan_FSM *fsm)
{
	readings rs = {.ir = IR_reading(), .sonar = sonar_reading(true)};
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
	readings rs = {.ir = IR_reading(), .sonar = sonar_reading(true)};
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
	
	readings rs = {.ir = IR_reading(), .sonar = sonar_reading(true)};
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
	readings rs = {.ir = IR_reading(), .sonar = sonar_reading(true)};
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




/**
 * Prints the contents of the given object into the given string buffer.
 * Returns the total number of characters printed.
 */
uint16_t snprint_object(char *buf, uint16_t bufsize, object *obj)
{	
	static const char *format = "theta1: %3d\n"
					            "theta2: %3d\n"
					            "dist:   %2.1f\n"
					            "width:  %2.1f\n";
	
	return snprintf(buf, bufsize, format, obj->theta1, obj->theta2, obj->dist, obj->width);
}






/************************************************************************/
/* Initiates the object scan finite state machine.                      */
/************************************************************************/

void objects_scan(scan_results *results)
{
	scan_FSM fsm;
	FSM_init(&fsm, 0);  // Starting at degree zero.
		
	for (uint8_t deg = 0; deg <= 180; deg += 1)
	{
		servo_angle(deg, true);
		fsm.cur_angle = deg;
        if (A <= fsm.state && fsm.state <= D) {
		    scan_FSM_trans[fsm.state](&fsm);
        } else {
		    wait_button("ERROR");
        }
	}
	
	// Copy results generated in `fsm` into `*results`:
	results->n = fsm.objects_found;
	for (uint8_t i = 0; i < fsm.objects_found && i < MAX_OBJECTS; i++) {
		results->objects[i] = fsm.objects[i];
	}
	
	return;
}
