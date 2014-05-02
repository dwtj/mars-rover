/*
 * movement.h
 *
 * Created: 1/26/2014 3:14:59 PM
 *  Author: flaviarc & DiamondStone
 */ 


#ifndef MOVEMENT_H
#define MOVEMENT_H


// Fields `x` and `y` are measures of the travel distance in a cartesian space measured in millimeters
// such that the initial direction is along the positive y axis. `angle` is the net angle by which the robot
// has turned during the move.
typedef struct {
	int x;
	int y;
	int angle;
	uint8_t success;
} move_t;

//The values that we have to send back to the controller for the movement function
typedef struct {
	uint16_t travelled;
	uint8_t flag;
} movement_data_t;

//rotated stores the actual amount rotated by the servo
typedef struct{
	uint16_t rotated;	
} rotation_data_t;

// If `dist` is a positive value, then the robot moves forward at full speed
// until it has moved  forward `dist` millimeters. If `dist` is negative, then it
// moves backwards at full speed until it has moved `dist` millimeters backwards.
// If `dist` is zero, it does not move.
movement_data_t move_dist(oi_t *sensor_data, int dist, int spd);
//int move_dist(oi_t *sensor_data, int dist);

// Similar to `move_dist`, if `angle` is positive, then the robot rotates in-place
// counter-clockwise until it has moved `angle` degrees. If `angle` is negative, then it
// rotates in-place until it has moved `angle` degrees. If `angle` is zero, it does not
// move. If
rotation_data_t turn (oi_t *sensor_data, int angle);


// Attempts to travel the given `dist` (measured in mm) forward, and if bumps are discovered,
// making course corrections as needed. If `dist` is non-positive, then the robot does nothing.
// The function fails if while performing a course correction, another bump is discovered.
// If successful, the function returns the number of bumps detected and successfully
// compensated for. If fails, a negative value is returned.
int navigate_dist(oi_t *sensor_data, int dist);


#endif /* MOVEMENT_H */