/*
 * objects.c
 *
 * Created: 4/4/2014 4:26:36 PM
 *  Author: dwtj
 */ 


#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>

#include "servo.h"
#include "sonar.h"
#include "IR.h"



#define MAX_OBS 10
typedef struct {
	uint8_t theta1;
	uint8_t theta2;
	uint8_t dist;
} object_obs;



typedef enum {A = 0, B = 1, C = 2, D = 3} FSM_states;
#define NUM_STATES 4


typedef struct {
	FSM_states state;
	object_obs obs[MAX_OBS];
	uint8_t objects_found;
	uint8_t cur_angle;
} FSM;

void FSM_init(FSM *fsm, uint8_t init_angle) {
	fsm->state = A;
	fsm->objects_found = 0;
	fsm->cur_angle = init_angle;
};

static bool is_object_there()
{
	const uint8_t low = 30, high = 85;
	uint16_t ir = IR_run();
	float sonar = sonar_reading();
	return (low <= ir && ir <= high) && (low <= sonar && sonar <= high);
}

static void transA(FSM *fsm) {
	if (is_object_there()) {
		fsm->state = B;
		// TODO: save current angle somewhere
	} else {
		fsm->state = A;
	}
}

static void transB(FSM *fsm) {
	if (is_object_there()) {
		// convinced that an object is there.
		// TODO: make atomic add of a object observation to data structure.
		fsm->state = C;
	} else {
		fsm->state = A;
	}
}

static void transC(FSM *fsm) {
	if (is_object_there()) {
		fsm->state = C;
	} else {
		fsm->state = D;
	}
}

static void transD(FSM *fsm) {
	if (is_object_there()) {
		fsm->state = C;
	} else {
		// convinced that no object is there anymore.
		fsm->state = A;
	}
}



void objects_scan()
{
	void (*trans[NUM_STATES]) (FSM *state) = {transA, transB, transC, transD};
	FSM fsm;
	FSM_init(&fsm, 0);  // Starting at degree zero.
		
	for (uint8_t deg = 0; deg <= 180; deg += 1)
	{
		servo_angle(deg, true);
		fsm.cur_angle = deg;
		trans[fsm.state](&fsm);
	}
}