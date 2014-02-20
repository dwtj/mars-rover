#include "open_interface.h"
#include "movement.h"
#include "lcd.h"
#include "util.h"
#include <math.h>


void part1(oi_t *sensor_data) {
	move_dist(sensor_data, 1000);
}

void part2(oi_t *sensor_data) {
	int i;
	int angle = 90;
	for (i = 0; i < 4; i++) {
		move_dist(sensor_data, 500);
		turn(sensor_data, angle);
	}
}

void part3(oi_t *sensor_data) {
	navigate_dist(sensor_data, 2000);
}

int main()
{
	lcd_init();
	oi_t *sensor_data = oi_alloc();
	
	
	while (1) {
		lcd_clear();
		int prog = wait_button("Select program.");
	
		lprintf("Running program %i.", prog);
	
		oi_init(sensor_data);

		switch (prog) {
			case 1:
				part1(sensor_data);
				break;
			case 2:
				part2(sensor_data);
				break;
			case 3:
				part3(sensor_data);
				break;
			default:
				lcd_clear();
				lcd_puts("Invalid Program.");
				break;
		}
	}
	

	oi_free(sensor_data);
}