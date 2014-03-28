/*
 * servo.h
 *
 * Created: 3/25/2014 4:10:34 PM
 *  Author: dwtj
 */ 


#ifndef SERVO_H_
#define SERVO_H_

void servo_pulse_proportion(float p);
void servo_init();
void servo_angle(uint8_t deg, bool wait);
void servo_enable();
void servo_manual_calib();
void servo_disable();



#endif /* SERVO_H_ */