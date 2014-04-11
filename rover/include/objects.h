/*
 * objects.h
 *
 * Created: 4/4/2014 4:26:54 PM
 *  Author: dwtj
 */ 


#ifndef OBJECTS_H_
#define OBJECTS_H_


// The maximum number of degrees which can be recorded for a single object.
#define CUR_OBJECT_WIDTH_MAX 30


// The maximum supported number of objects that the FSM can find in a scan.
#define MAX_OBJECTS 10


// An object observed by the scan_FSM.
typedef struct {
	uint8_t theta1;   // The first angle at which the object was observed.
	uint8_t theta2;   // The last angle at which the object was observed.
	float dist;       // The distance from the rover to the object.
	float width;      // The linear width of this object.
} object;

typedef struct {
	object objects[MAX_OBJECTS];  // The array of objects that was found.
	uint8_t n;                    // The number of objects found.
} scan_results;

void objects_scan(scan_results *results);

uint16_t snprint_object(char *buf, uint16_t bufsize, object *obj);



typedef struct {
	float sonar;
	uint16_t ir;
} readings;





#define NUM_STATES 4
typedef enum {A = 0, B = 1, C = 2, D = 3} scan_FSM_states;





typedef struct {
	scan_FSM_states state;        // The current state that this scan_FSM is in.
	object objects[MAX_OBJECTS];  // The object that have so far been found by this current scan.
	uint8_t objects_found;        // The number of object found in the current scan, and their scanning process is **complete**.
	uint8_t cur_angle;            // The current angle at which the tower is directed. Range is 0 to 180 degrees (inclusive).
	readings cur_object_readings[CUR_OBJECT_WIDTH_MAX];  // Readings from each degree since the beginning of the object currently being observed.
	uint8_t cur_object_width;     // The number of degrees since the beginning of the object currently being observed.
} scan_FSM;


#endif /* OBJECTS_H_ */