
#include "open_interface.h"
#include "songs.h"

/// Load three songs onto the iRobot
/**
 * Before calling this method, be sure to initialize the open interface by calling:
 *
 *   oi_t* sensor = oi_alloc();
 *   oi_init(sensor); 
 *
 */


static uint8_t rickrollNumNotes = 11;
static uint8_t rickrollNotes[11] = {53, 55, 48, 55, 57, 60, 58, 57, 53, 55, 48};
static uint8_t rickrollDurations[11] = {48, 64, 16, 48, 48, 8,  8,  8,  48, 64, 64};



static uint8_t ImperialMarchNumNotes = 19;
static uint8_t ImperialMarchNotes[19] =
	{55, 55, 55, 51, 58, 55, 51, 58, 55, 0,  62, 62, 62, 63, 58, 54, 51, 58, 55};
static uint8_t ImperialMarchDurations[19] =
	{32, 32, 32, 20, 12, 32, 20, 12, 32, 32, 32, 32, 32, 20, 12, 32, 20, 12, 32};



static uint8_t mario1NumNotes = 49;
static uint8_t mario1Notes[49] =
	{48, 60, 45, 57, 46, 58,  0, 48, 60, 45, 57, 46, 58,  0, 41, 53, 38, 50,
	 39, 51,  0, 41, 53, 38, 50, 39, 51,  0, 51, 50, 49, 48, 51, 50, 44, 43,
	 49, 48, 54, 53, 52, 58, 57, 56, 51, 47, 46, 45, 44 };
static uint8_t mario1Duration[49] =
	{12, 12, 12, 12, 12, 12, 62, 12, 12, 12, 12, 12, 12, 62, 12, 12, 12, 12,
	 12, 12, 62, 12, 12, 12, 12, 12, 12, 48,  8,  8,  8, 24, 24, 24, 24, 24,
	 24,  8,  8,  8,  8,  8,  8, 16, 16, 16, 16, 16, 16 };



static uint8_t mario2NumNotes = 65;
static uint8_t mario2Notes[65] =
	{50, 52, 54, 55, 57, 58, 59, 59, 59, 59, 59, 55, 64, 63, 64, 0, 55, 57,
	 59, 60, 62, 64, 63, 65, 64,  0, 55, 62, 61, 62,  0, 55, 57, 59, 60, 61,
	 62, 55, 65, 64,  0, 55, 67, 67, 67, 67, 69,  0, 67, 65, 65, 65, 65, 67,
	 0, 65, 64, 57, 59, 65, 64, 64, 64, 59, 60 };
static uint8_t mario2Duration[65] =
	{18, 18, 18, 18, 18, 18,  9,  9, 18, 18, 45,  9, 54, 54, 54,  9,  9,  9,
	  9,  9,  9, 54, 36, 18, 54, 45,  9, 54, 54, 54,  9,  9,  9,  9,  9,  9,
	 54, 36, 18, 54, 45,  9, 54, 54, 54, 18, 18,  9,  9, 54, 54, 54, 18, 18,
	  9,  9, 54, 18, 18, 18,  9,  9, 27,  9, 54 };



static uint8_t victoryNumFanfare = 53;
static uint8_t victoryFanfareNotes[53] = {55,60,64,60,60,60,60,65,65,55,65,55,60,64,55,
		57,59,60,56,58,60,55,60,64,62,64,62,65,
		62,65,65,65,65,64,60,60,60,58,62,64,
		60,55,55,60,55,65,59,65,60,60,60,60,60};
static uint8_t victoryFanfareDurations[53] = {18, 18, 18, 18, 18, 18,  9,  9, 18, 18, 45,  9, 54, 54, 54, 9,  9,  9,  9,  9,  9, 54, 36, 18, 54, 45,  9, 54, 54, 54,  9,  9,  9,  9,  9,  9, 54, 36, 18, 54, 45,  9, 54, 54, 54, 18, 18,  9,  9, 54, 54, 54, 18, 18,  9,  9, 54, 18, 18, 18,  9,  9, 27,  9, 54 }; //These probably need to be edited.



void songs_load(uint8_t song_id)
{
	switch (song_id) {
	case RICK_ROLL:
		oi_load_song(RICK_ROLL, rickrollNumNotes, rickrollNotes, rickrollDurations);
		oi_play_song(RICK_ROLL);
		break;

	case IMPERIAL_MARCH:
		oi_load_song(IMPERIAL_MARCH, ImperialMarchNumNotes,
				     ImperialMarchNotes, ImperialMarchDurations);
		oi_play_song(IMPERIAL_MARCH);
		break;

	case MARIO_UNDERWORLD:
		oi_load_song(MARIO_UNDERWORLD, mario1NumNotes, mario1Notes, mario1Duration);
		oi_play_song(MARIO_UNDERWORLD);
		break;

	case MARIO_UNDERWATER:
		oi_load_song(MARIO_UNDERWATER, mario2NumNotes, mario2Notes, mario2Duration);
		oi_play_song(MARIO_UNDERWATER);
		break;

	case VICTORY_FANFARE:
		oi_load_song(VICTORY_FANFARE,victoryNumFanfare,
		             victoryFanfareNotes, victoryFanfareDurations);
		oi_play_song(VICTORY_FANFARE);
		break;
		
	default:
		; // TODO: report that song is not defined
	}
}
