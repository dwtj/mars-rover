/**
 * sonar.h
 */

#ifndef SONAR_H
#define SONAR_H

void sonar_init(void);

void sonar_pulse(void);

char *sonar_get_state(void);

float sonar_reading(void);

uint16_t sonar_reading_polling(void);

#endif // SONAR_H
