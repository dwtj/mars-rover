/*
 * timer.h
 *
 * Created: 2/7/2014 3:41:44 PM
 *  Author: flaviarc
 */ 


#ifndef TIMER_H
#define TIMER_H

#define CLOCK_COUNT 15625 // The number of system clock cycles in 1 second
#define CHECK_COUNT  3125 // The number of system clock cycles in 200 ms
//#define CHECK_COUNT  10000 // The number of system clock cycles in 200 ms
//15625 -> 6MHz divided by 1024
//3125 -> result/5

void timer_init(void);

#endif /* TIMER_H */