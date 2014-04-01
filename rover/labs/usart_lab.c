/*
 * SerialCommunicationUsingUSART.c
 *
 * Created: 2/14/2014 2:53:31 PM
 *  Author: flaviarc
 */

#include "lcd.h"
#include "util.h"
#include "usart.h"
#include <stdint.h>
#include <stdbool.h>

#define BUF_LEN 20

void print_serial_to_lcd(uint8_t echo)
{
	char buf[BUF_LEN + 1];
	unsigned char i;
	unsigned char c;
	
	while(1) {
		for (i = 0; i < BUF_LEN; i++)
		{
			c = USART_Receive();
			if (echo) {
				USART_Transmit(c);
			}
			
			if (c == '\r') {
				if (echo) {
					USART_Transmit('\n');
				}
				break;
			}
			
			lprintf("%i: %c", i, c);
			buf[i] = c;
		}
		buf[i] = '\0';
		lcd_clear();
		lcd_puts(buf);
	}
}


void send_messages()
{
	char *messages[] = {
		"",								// 0
		"Spam! Spam! Spam!",			// 1
		"Sparkle like diamonds",		// 2
		"Yes, after breakfast.",		// 3
		"Blue, no green, Ahhhhh!!!",	// 4
		"No",							// 5
		"Yes"							// 6
	};
		
	uint8_t prev = 0, cur = 0;
	
	// TODO: prevent display of message from program selection.
	//wait_ms(100);  // this isn't enough
	
	while(1){
		cur = read_push_buttons();
		cur = 0 <= cur && cur <= 6 ? cur : 0;  // guard against invalid message selection.
		
		if (cur != prev) {
			if (cur != 0) {
				USART_transmit_buffer(messages[cur]);
				USART_transmit_buffer("\r\n");
			}
			prev = cur;
		}
		
		wait_ms(100);  // to reduce effects of bouncing.
	}
}

void part1(void)
{
	USART_Init(1);  // init bluetooth
	print_serial_to_lcd(0);
}

void part2 (void){
	USART_Init(1);  // init bluetooth
	print_serial_to_lcd(1);
}

void part3 (void)
{
	USART_Init(1);  // init bluetooth
	send_messages();
}

void part4() {
	USART_Init(1);  // init bluetooth
	send_messages();
}

int main()
{
	lcd_init();
	init_push_buttons();
	
	char prog;
	
	while (1) {
		lcd_clear();
		prog = wait_button("Choose program:");
		switch (prog) {
			case 1:
				part1();
				break;
			case 2:
				part2();
				break;
			case 3:
				part3();
				break;
			case 4:
				part4();
				break;
			default:
				;  // do nothing
		}
	}
}