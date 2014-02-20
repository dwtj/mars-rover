#include "lcd.h"
#include "util.h"

#define lcd_width 20
char lcdmsg[] = "                    " // 20 spaces
	            "A Message Can Be Put Here.";


/// Simple 'Hello, world' program
/**
 * This program prints "Hello, world" to the LCD screen
 * @author Chad Nelson
 * @date 06/26/2012
 */


// Prints `str` to the lcd screen, unless its length is less `n`, in which case
// only the first `n` characters are printed.
int lcd_nputs(char *str, int n) {
	char *cur = str;
	while (cur - str < n && *cur != '\0') {
		lcd_putc(*cur);
		cur++;
	}
	return cur - str;
}


int main (void) {
	// Initialize the the LCD.  This also clears the screen.
	lcd_init();
	char *view;
	while(1){
		for (view = lcdmsg; *view != '\0'; view++){
			lcd_clear();
			lcd_nputs(view, lcd_width);
			wait_ms(300);
		}
	}
	// Clear the LCD screen and print "Hello, world" on the LCD
	// lcd_puts("Hello, world");// Replace lprintf with lcd_puts
                                 // step through in debug mode
                                 // and explain to TA how it
                                 // works

	// Notes: Open util.h to see what functions are available for you to use.
	// You will need to use these functions in future labs.
    
	// It is recommended that you use only lcd_init(), lprintf(), lcd_putc, and lcd_puts from lcd.h.
    
	return 0;
}
