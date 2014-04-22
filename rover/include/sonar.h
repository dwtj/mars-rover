/**
 * sonar.h
 */

#include <stdbool.h>

#ifndef SONAR_H
#define SONAR_H

void sonar_init(void);

void sonar_pulse(void);

char *sonar_get_state(void);

float sonar_reading(bool raw);

uint16_t sonar_reading_polling(void);

void sonar_system();

#endif // SONAR_H
