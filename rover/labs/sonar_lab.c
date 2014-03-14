/**
 * sonar_lab.c
 */

#include <stdint.h>
#include <stdbool.h>

#include "sonar.h"
#include "util.h"

#include "lcd.h"

/**
 * Demonstrates activation of the `PING)))` sensor. Check the green light.
 */
static void part1()
{
    while (true) {
        sonar_pulse();
        wait_ms(1000);
    }
}


/**
 * Demonstrates that the system can send a pulse and measure the amount of time
 * that the subsequent echo pulse stays high.
 */
static void part2()
{
    while (1) {
		lprintf("%u", sonar_reading());
		wait_ms(1000);
    }
}

static void part3()
{
    while (1) {
	    while (wait_button("") == 0) {
		    ;  // wait until user presses a button.
	    }
	    lprintf("%d", sonar_reading_polling());
    }
}

void sonar_lab(void)
{
    init_push_buttons();
    lcd_init();
    sonar_init();

	lcd_puts("Startup...");
	wait_ms(1000);
	lcd_clear();

    uint8_t prog = wait_button("Select Program...");
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
        default:
            ; // do nothing
    }
}
