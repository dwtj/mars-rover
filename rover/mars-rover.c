/*
 * mars-rover.c
 *
 * Created: 2/14/2014 2:53:31 PM
 *  Author: flaviarc
 */

#include "labs.h"
#include "control.h"

int main()
{
    init_push_buttons();
    lcd_init();

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
    default:
        lcd_puts("Invalid selection.");
        break;
    }
}
