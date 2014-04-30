/*
 * mars-rover.c
 *
 * Created: 2/14/2014 2:53:31 PM
 *  Author: flaviarc
 */

#include "control.h"
#include "IR.h"
#include "lcd.h"
#include "movement.h"
#include "objects.h"
#include "open_interface.h"
#include "r_error.h"
#include "servo.h"
#include "sonar.h"
#include "timer.h"
#include "txq.h"
#include "usart.h"
#include "util.h"

#include "labs.h"
#include "tests.h"


// Note that this does not initialize the `oi` subsystem, since that
// initializer blocks.
void init_subsystems()
{
	ir_init();
    lcd_init();
	servo_init();
	sonar_init();
	txq_init();
	usart_init();
    init_push_buttons();
}



int main()
{
	init_subsystems();

    switch (wait_button("Select Mode")) {
    case 1:
	    control_mode();
        break;

    case 2:
        send_mesg_test_mode();
        break;

    case 3:
	    objects_lab();
        break;

    case 4:
		test_songs_mode();
        break;

	case 5:
		readings_stream_mode();
		break;

	case 6:
		servo_manual_calib();
		break;

    default:
        lcd_puts("Invalid selection.");
        break;
    }
}
