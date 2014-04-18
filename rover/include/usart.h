#ifndef USART_H
#define USART_H

#include <stdint.h>

//#define FOSC 1843200  // Clock Speed //commented out because FOSC defined elsewhere to be 1600000. Why was it this number???
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

void usart_init (unsigned char type);

unsigned char usart_rx( void );

void usart_tx(uint8_t data);

void usart_tx_buf(char *buf);

#endif  /* USART_H */