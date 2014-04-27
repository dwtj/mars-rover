#include <stdlib.h>
#include <stdbool.h>
#include "util.h"
#include "open_interface.h"
#include "r_error.h"
#include "control.h"

/// Allocate memory for a the sensor data
oi_t* oi_alloc() {
	return calloc(1, sizeof(oi_t));
}

/// Free memory from a pointer to the sensor data struct
void oi_free(oi_t *self) {
	free(self);
}

/// Initialize the Create
void oi_init(oi_t *self) {
	// Setup USART1 to communicate to the iRobot Create using serial (baud = 57600)
	UBRR1L = 16; // UBRR = (FOSC/16/BAUD-1);
	UCSR1B = (1 << RXEN) | (1 << TXEN);
	UCSR1C = (3 << UCSZ10);

	// Starts the SCI. Must be sent first
	oi_byte_tx(OI_OPCODE_START);
	oi_byte_tx(OI_OPCODE_BAUD);

	oi_byte_tx(8); // baud code for 28800
	wait_ms(100);
	
	// Set the baud rate on the Cerebot II to match the Create's baud
	UBRR1L = 33; // UBRR = (FOSC/16/BAUD-1);

	// Use Full mode, unrestricted control
	oi_byte_tx(OI_OPCODE_FULL);
	oi_set_leds(1, 1, 7, 255);
	
	oi_update(self);
	oi_update(self); // call twice to clear distance/angle
}



/// Update the Create. This will update all the sensor data and store it in the oi_t struct.
void oi_update(oi_t *self) {
	int i;

	// Clear the receive buffer
	while (UCSR1A & (1 << RXC)) 
		i = UDR1;

	// Query a list of sensor values
	oi_byte_tx(OI_OPCODE_SENSORS);
	// Send the sensor packet ID
	oi_byte_tx(OI_SENSOR_PACKET_GROUP6); 

	// Read all the sensor data
	char *sensor = (char *) self;
	for (i = 0; i < 52; i++) {
		// read each sensor byte
		*(sensor++) = oi_byte_rx();
	}
	
	sensor = (char *) self;
	
	// Fix byte ordering for multi-byte members of the struct
	self->distance				 = (sensor[12] << 8) + sensor[13];
	self->angle					= (sensor[14] << 8) + sensor[15];
	self->voltage				  = (sensor[17] << 8) + sensor[18];
	self->current				  = (sensor[19] << 8) + sensor[20];
	self->charge				   = (sensor[22] << 8) + sensor[23];
	self->capacity				 = (sensor[24] << 8) + sensor[25];
	self->wall_signal			  = (sensor[26] << 8) + sensor[27];
	self->cliff_left_signal		= (sensor[28] << 8) + sensor[29];
	self->cliff_frontleft_signal   = (sensor[30] << 8) + sensor[31]; 
	self->cliff_frontright_signal  = (sensor[32] << 8) + sensor[33];
	self->cliff_right_signal	   = (sensor[34] << 8) + sensor[35];
	self->cargo_bay_voltage		= (sensor[41] << 8) + sensor[42];
	self->requested_velocity	   = (sensor[48] << 8) + sensor[42];
	self->requested_radius		 = (sensor[50] << 8) + sensor[51];
	self->requested_right_velocity = (sensor[52] << 8) + sensor[53];
	self->requested_left_velocity  = (sensor[54] << 8) + sensor[55];
	
	wait_ms(35); // reduces USART errors that occur when continuously transmitting/receiving
}



/// Sets the LEDs on the iRobot.
/**
* Set the state of the three LEDs on the iRobot (Power, Play, Advance).
* @play_led		uint8_t either 0 (off) or 1 (on)
* @advance_led	 uint8_t either 0 (off) or 1 (on)
* @power_color	 uint8_t the color of the power LED; 0 = green, 255 = red
* @power_intensity uint8_t the intensity of the power LED; 0 = off, 255 = full intensity
*/
void oi_set_leds(uint8_t play_led, uint8_t advance_led, uint8_t power_color, uint8_t power_intensity) {
	// LED Opcode
	oi_byte_tx(OI_OPCODE_LEDS);

	// Set the Play and Advance LEDs
	oi_byte_tx(advance_led << 3 && play_led << 2);

	// Set the power led color
	oi_byte_tx(power_color);

	// Set the power led intensity
	oi_byte_tx(power_intensity);
}



/// Drive wheels directly; speeds are in mm / sec
void oi_set_wheels(int16_t right_wheel, int16_t left_wheel) {
	oi_byte_tx(OI_OPCODE_DRIVE_WHEELS);
	oi_byte_tx(right_wheel>>8);
	oi_byte_tx(right_wheel & 0xff);
	oi_byte_tx(left_wheel>>8);
	oi_byte_tx(left_wheel& 0xff);
}


//Handler for OI, moved from control.
void oi_system()
{
	enum {
		oi_command_init = 0,
		oi_command_move = 1,
		oi_command_rotate = 2,
		oi_command_play_song = 3,
		oi_command_dump = 4,
	} oi_command = usart_rx();

	txq_enqueue(oi_command);

	switch (oi_command) {
		case oi_command_init:

			oi_init(&(control.oi_state));
			break;


		case oi_command_move:
	
			if(rx_frame()) {
				r_error(error_frame,"Move should not have multiple frames.");
			}

			struct {
				uint8_t speed;
				uint8_t dist;
				bool stream;
			} *move_data = (void *) &control.data;

			#warning "Stream functionality to be implemented later."
			//Stream returns the distance traveled

			move_dist(&(control.oi_state), move_data->dist, move_data->speed);
			break;

		case oi_command_rotate:

			if (rx_frame()) {
				r_error(error_bad_message, "Rotate should only have one data frame.");
			}

			int16_t *angle = &(control.data[0]);

			if (control.data_len != sizeof(*angle)) {
				r_error(error_bad_message, "Received too much data with rotate "
																	   "message.");
			}

			turn(&(control.oi_state), *angle);
			break;
			
			//Sing me a song.
		case oi_command_play_song:
			;
			//assuming that we get two data frames, the first containing the notes and the second containing the durations.
			int j;
				struct {
					uint8_t n; //The number of notes present in the song
					//char data[n];
					uint8_t index;
				} *song_data = (void *) &control.data;
		
			//	int j;
		 
			while(rx_frame()) { //this should happen twice please
				char tmp_notes[song_data->n];
				char tmp_durs[song_data->n];
			
				for(j =0; j<song_data->n; j++) {
					//tmp_notes[n]; TODO: broken
					#warning "oi_command_play_song not implemented"
				}

			}
		
			break;

		case oi_command_dump:
			lcd_putc('D');  // DEBUG
			//copies all of the data from OI_UPDATE and transmits to Control.
			oi_update(&(control.oi_state));
			memcpy(control.data, &control.oi_state, sizeof(control.oi_state));
			control.data_len = sizeof(control.oi_state);
			tx_frame(false);
			lcd_putc('E');  // DEBUG
			break;

		default:
			r_error(error_bad_message, "Bad OI Command");
			break;
	}
}

/// Loads a song onto the iRobot Create
void oi_load_song(int song_index, int num_notes, unsigned char *notes, unsigned char *duration) {
	int i;
	oi_byte_tx(OI_OPCODE_SONG);
	oi_byte_tx(song_index);
	oi_byte_tx(num_notes);
	
	for (i=0;i<num_notes;i++) {
		oi_byte_tx(notes[i]);
		oi_byte_tx(duration[i]);
	}
}


/// Plays a given song; use oi_load_song(...) first
void oi_play_song(int index){
	oi_byte_tx(OI_OPCODE_PLAY);
	oi_byte_tx(index);
}


/// Runs default go charge program; robot will search for dock
void go_charge(void) {
	char charging_state=0;
	
	//Calling demo that will cause Create to seek out home base
	oi_byte_tx(OI_OPCODE_MAX);
	oi_byte_tx(0x01);
	
	//Control is returned immediately, so need to check for docking status
	DDRB &= ~0x80; //Setting pin7 to input
	PORTB |= 0x80; //Setting pullup on pin7
	
	do {
		charging_state = PINB >> 7;
	} while (charging_state == 0);
}



// Transmit a byte of data over the serial connection to the Create
void oi_byte_tx(unsigned char value) {
	// Wait until the transmit buffer is empty
	while (!(UCSR1A & (1 << UDRE)));

	UDR1 = value;
}



// Receive a byte of data from the Create serial connection. Blocks until a byte is received.
unsigned char oi_byte_rx(void) {
	// wait until a byte is received (Receive Complete flag, RXC, is set)
	while (!(UCSR1A & (1 << RXC)));

	return UDR1;
}
