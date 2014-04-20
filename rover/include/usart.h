#ifndef USART_H
#define USART_H

#include <stdint.h>
#include <stdbool.h>

#define USART_UBRR 16
#define USART_2X false
// BAUD = 57.6KHz with this UBRR, if FOSC is 16MHz and double-speed is not set.

void usart_init();

uint8_t usart_rx(void);

void usart_drain_rx(void);

void usart_tx(uint8_t data);

void usart_tx_buf(char *buf);

void usart_RX_ISR_enable();

void usart_RX_ISR_disable();

#endif  /* USART_H */
