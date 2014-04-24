/**
 * sonar.h
 */

#include <stdbool.h>

#ifndef SONAR_H
#define SONAR_H

void sonar_init(void);

void sonar_pulse(void);

char *sonar_get_state(void);

uint16_t sonar_raw_reading();

float sonar_reading();

uint16_t sonar_reading_polling(void);

void sonar_system();

#endif // SONAR_H
