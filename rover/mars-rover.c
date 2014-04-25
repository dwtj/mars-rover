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
		char song[] = {96, 96, 96, 96, 92, 94, 96, 94, 96};
		char duration[] = {8, 8, 8, 8, 12, 12, 8, 8, 8}; //These probably need to be edited.
		oi_load_song(0,9, song[0], duration[0]);//??
		oi_play_song(0);
		
//     init_push_buttons();
//     lcd_init();
// 
//     switch (wait_button("Select Mode")) {
//     case 1:
// 	    control_mode();
//         break;
//     case 2:
//         send_mesg_test_mode();
//         break;
//     case 3:
// 	    objects_lab();
//         break;
//     default:
//         lcd_puts("Invalid selection.");
//         break;
//     }
}
