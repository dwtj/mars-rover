/*
 * servo.h
 *
 * Created: 3/25/2014 4:10:34 PM
 *  Author: dwtj
 */ 


#ifndef SERVO_H_
#define SERVO_H_

#include <stdbool.h>
#include <stdint.h>

void servo_pulse_proportion(float p);
void servo_pulse_width(uint16_t pw);
void servo_init();
void servo_angle(uint8_t deg, bool wait);
void servo_enable();
void servo_manual_calib();
void servo_disable();
void servo_system();


#endif /* SERVO_H_ */
