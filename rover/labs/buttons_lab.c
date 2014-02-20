#include "lcd.h"
#include "util.h"

void part1()
{
	char button = 1;
	while(1) { // loop indefinitely
		button = read_push_buttons();
		lprintf("Button: %d", button);
	}
}


void part2()
{
	int sum = 0;
	signed char read;
	while(1) {
		read = read_shaft_encoder();
		if (read != 0) {
			sum += read;
			lprintf("Sum equals %i", sum);
		}
	}
}

void part3 ()
{
	int sum = 0;
	signed char read;
	while(1) {
		move_stepper_motor_by_step(5, read_shaft_encoder());
	}
}

/*
void test()
{
	while(read_push_buttons() != 6) {
		lprintf("%h", PINC && 0xC0);
		wait_ms();
	}
}
*/

int main()
{
	lcd_init();
	init_push_buttons();
	shaft_encoder_init();
	stepper_init();
	
	while (1) {
		char prog;
		
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
			default:
				;  // do nothing
		}
	}
}