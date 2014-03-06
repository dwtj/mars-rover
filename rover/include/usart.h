#ifndef USART_H
#define USART_H

#include <stdint.h>

#define FOSC 1843200  // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

void USART_Init (unsigned char type);

unsigned char USART_Receive( void );

void USART_Transmit(uint8_t data);

void USART_transmit_buffer(char *buf);

#endif  /* USART_H */