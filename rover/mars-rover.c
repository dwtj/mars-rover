/*
 * mars-rover.c
 *
 * Created: 2/14/2014 2:53:31 PM
 *  Author: flaviarc
 */

#include "labs.h"
#include "control.h"
#include "open_interface.h"
#include "objects.h"

#define RICK_ROLL			0
#define IMERPIAL_MARCH 		1
#define MARIO_UNDERWORLD	3
#define MARIO_UNDERWATER	7
#define VICTORY_FANFARE		0

/// Load three songs onto the iRobot
/**
 * Before calling this method, be sure to initialize the open interface by calling:
 *
 *   oi_t* sensor = oi_alloc();
 *   oi_init(sensor); 
 *
 */
void load_song1() {

	unsigned char rickrollNumNotes = 11;
	unsigned char rickrollNotes[11]	= {53, 55, 48, 55, 57, 60, 58, 57, 53, 55, 48};
	unsigned char rickrollDuration[11] = {48, 64, 16, 48, 48, 8,  8,  8,  48, 64, 64};
	oi_load_song(RICK_ROLL, rickrollNumNotes, rickrollNotes, rickrollDuration);
	oi_play_song(RICK_ROLL);
	
}

void load_song2() {
	
	unsigned char ImperialMarchNumNotes = 19;
	unsigned char ImperialMarchNotes[19]	 = {55, 55, 55, 51, 58, 55, 51, 58, 55, 0,  62, 62, 62, 63, 58, 54, 51, 58, 55};
	unsigned char ImperialMarchDurations[19] = {32, 32, 32, 20, 12, 32, 20, 12, 32, 32, 32, 32, 32, 20, 12, 32, 20, 12, 32};
	oi_load_song(IMERPIAL_MARCH, ImperialMarchNumNotes, ImperialMarchNotes, ImperialMarchDurations);
	oi_play_song(IMERPIAL_MARCH);
}

void load_song3() {
	unsigned char mario1NumNotes = 49;
	unsigned char mario1Notes[49]	= {48, 60, 45, 57, 46, 58,  0, 48, 60, 45, 57, 46, 58,  0, 41, 53, 38, 50, 39, 51,  0, 41, 53, 38, 50, 39, 51,  0, 51, 50, 49, 48, 51, 50, 44, 43, 49, 48, 54, 53, 52, 58, 57, 56, 51, 47, 46, 45, 44 };
	unsigned char mario1Duration[49] = {12, 12, 12, 12, 12, 12, 62, 12, 12, 12, 12, 12, 12, 62, 12, 12, 12, 12, 12, 12, 62, 12, 12, 12, 12, 12, 12, 48,  8,  8,  8, 24, 24, 24, 24, 24, 24,  8,  8,  8,  8,  8,  8, 16, 16, 16, 16, 16, 16 };
	oi_load_song(MARIO_UNDERWORLD, mario1NumNotes, mario1Notes, mario1Duration);
	oi_play_song(MARIO_UNDERWORLD);
}

void load_song4() {	
	unsigned char mario2NumNotes = 65;
	unsigned char mario2Notes[65]	= {50, 52, 54, 55, 57, 58, 59, 59, 59, 59, 59, 55, 64, 63, 64, 0, 55, 57, 59, 60, 62, 64, 63, 65, 64,  0, 55, 62, 61, 62,  0, 55, 57, 59, 60, 61, 62, 55, 65, 64,  0, 55, 67, 67, 67, 67, 69,  0, 67, 65, 65, 65, 65, 67,  0, 65, 64, 57, 59, 65, 64, 64, 64, 59, 60 };
	unsigned char mario2Duration[65] = {18, 18, 18, 18, 18, 18,  9,  9, 18, 18, 45,  9, 54, 54, 54, 9,  9,  9,  9,  9,  9, 54, 36, 18, 54, 45,  9, 54, 54, 54,  9,  9,  9,  9,  9,  9, 54, 36, 18, 54, 45,  9, 54, 54, 54, 18, 18,  9,  9, 54, 54, 54, 18, 18,  9,  9, 54, 18, 18, 18,  9,  9, 27,  9, 54 };
	oi_load_song(MARIO_UNDERWATER, mario2NumNotes, mario2Notes, mario2Duration);
	oi_play_song(MARIO_UNDERWATER);
}

void load_song5() {
	
	unsigned char victoryNumFanfare = 53;
	char victoryFanfareNotes[53] = {55,60,64,60,60,60,60,65,65,55,65,55,60,64,55,
		57,59,60,56,58,60,55,60,64,62,64,62,65,
		62,65,65,65,65,64,60,60,60,58,62,64,
		60,55,55,60,55,65,59,65,60,60,60,60,60};
	
	//char duration[8] = {14, 14, 14, 14, 30, 30, 18, 18, 18}; //These probably need to be edited.
		char duration[53] = {18, 18, 18, 18, 18, 18,  9,  9, 18, 18, 45,  9, 54, 54, 54, 9,  9,  9,  9,  9,  9, 54, 36, 18, 54, 45,  9, 54, 54, 54,  9,  9,  9,  9,  9,  9, 54, 36, 18, 54, 45,  9, 54, 54, 54, 18, 18,  9,  9, 54, 54, 54, 18, 18,  9,  9, 54, 18, 18, 18,  9,  9, 27,  9, 54 }; //These probably need to be edited.
	oi_load_song(VICTORY_FANFARE,victoryNumFanfare, victoryFanfareNotes, duration);
	oi_play_song(0);
}


void playSong(void){
	
	switch(wait_button("Select song")){
		
		case 1:
			load_song1(); //plays Rick Roll song -whats this song???
			break;
		case 2:
			load_song2(); //plays the Imperial March
			break;
		case 3:
			load_song3(); //plays Mario Underworld
			break;
		case 4:
			load_song4(); //plays Mario Underwater song
			break;
		case 5:
			load_song5(); //plays Victory fanfare - not well
			break;
		
		default:
			lcd_puts("Invalid selection.");
			break;
		
	}
}


static void part2()
{
	#define MY_RX_BUFSIZE 200
	static char buf[MY_RX_BUFSIZE];
	
	scan_results results;
	objects_scan(&results);
	
	//wait_button("here0");
	
	snprintf(buf, MY_RX_BUFSIZE, "Objects Found: %d\n\n", results.n);
	
	//wait_button("here1");
	
	usart_tx_buf(buf);
	
	//wait_button("here2");
	
	lprintf("%s", buf);
	
	//wait_button("here3");
	
	uint8_t minwidth = 180;
	//uint8_t min_id = 0;
	uint16_t min_midpoint = 0;
	uint8_t delta;
	
	for (int i = 0; i < results.n; i++) {
		delta = results.objects[i].theta2 - results.objects[i].theta1;
		if (delta < minwidth){
			minwidth = delta;
			min_midpoint = (results.objects[i].theta2 + results.objects[i].theta1) / 2;
			//min_id = i;
		}
		snprint_object(buf, MY_RX_BUFSIZE, results.objects + i);
		usart_tx_buf(buf);
		usart_tx_buf("\n\n");
	}
	
	// go to midpoint of min_id;
	servo_angle(min_midpoint, true);
	
	wait_button("");
}

int main()
{		
	oi_t *sensor_data = oi_alloc();
			
	init_push_buttons();
	lcd_init();
	oi_init(sensor_data);
	servo_init();
	init_push_buttons();
	lcd_init();

//	while(1){
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
				playSong();
				break;
			case 5: //Debugging
				move_dist(sensor_data, 20, 100);
				break;
			default:
				lcd_puts("Invalid selection.");
				break;
		}
//	}

}
