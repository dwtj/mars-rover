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
