#include <stdio.h>
#include <stdint.h>

#include "IR.h"
#include "sonar.h"
#include "tests.h"
#include "control.h"
#include "usart.h"
#include "txq.h"
#include "util.h"


void send_mesg_test_mode()
{
   	lcd_init();
	init_push_buttons();

	usart_init();
	txq_init();
	usart_drain_rx();

	lcd_puts("Send Mesg Test Mode");
	wait_ms(1000);
	lcd_clear();

	char echo_mesg[] = "foobar";  // Assumed to be less than DATA_FRAME_MAX_LEN.
 
	while (true)
	{
		switch (wait_button("Select Message")) {
		case mesg_ping:

			txq_enqueue(signal_start);
			txq_enqueue(mesg_ping);
			txq_enqueue(signal_stop);
			txq_drain();
			break;

		case mesg_echo:

			txq_enqueue(signal_start);
			txq_enqueue(mesg_echo);

			// Copy the echo message into the `control`:
			strcpy(control.data, echo_mesg);
			control.data_len = strlen(echo_mesg);

			// Enqueue the frame. No more frames coming.
			tx_frame(false);

			txq_enqueue(signal_start);
			txq_drain();
			break;

		default:
			wait_button("Invalid selection.");
			break;
		}
	}
}



void readings_stream_mode()
{
	const uint8_t BUF_LENGTH = 100;
	char buf[BUF_LENGTH];

	enum {
		reading_ir = 1,
		reading_sonar = 2,
		reading_sonar_mode = 3
	} reading;

	init_push_buttons();
	lcd_init();

	ir_init();
	sonar_init();

	usart_init();
	usart_drain_rx();

	while (true) {
		switch (wait_button("Readings Stream")) {
		case reading_ir:
			snprintf(buf, BUF_LENGTH, "IR: %u\n", ir_raw_reading());
			break;

		case reading_sonar:
			snprintf(buf, BUF_LENGTH, "Sonar: %u\n", sonar_raw_reading());
			break;

		case reading_sonar_mode:
			while (true) {
				snprintf(buf, BUF_LENGTH, "Sonar: %u\n", sonar_raw_reading());
				usart_tx_buf(buf);
			}
			break;

		default:
			buf[0] = '\0';
		}

		usart_tx_buf(buf);
	}
}
