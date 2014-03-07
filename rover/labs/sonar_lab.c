/**
 * sonar_lab.c
 */

#include <stdint.h>
#include <stdbool.h>

#include "sonar.h"
#include "util.h"


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
    while (true) {
        while (wait_buttons("Press a button...") == 0) {
            sonar_pulse();
            lprintf("%d", sonar_signal_width());
        }
    }
}

static void part3()
{
    #warning "Function not yet implemented."
}

void sonar_lab(void)
{
    buttons_init();
    lcd_init();
    sonar_init();

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
