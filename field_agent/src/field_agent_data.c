/*****************************************************************/
/* This program          */
/*****************************************************************/
#include "field_agent_data.h"

static char teamName[7] = "views6";
static char init_gameID[2] = "0";
static int max_num_hints = 10;
static int max_hint_size = 141;
static char start_string[5] = "None";




// char *gameID;
// 	char* pebbleID;
// 	char* name;
// 	char* team;
// 	int time_passed;
// 	char* latitude;
// 	char* longitude;
// 	int num_claimed;
// 	int num_left;
// 	char* known_chars;
// 	char** hints_received;

// 	bool game_started;

// 	char* krag_to_submit;

void create_info()
{
	FA_INFO = malloc(sizeof(fieldagent_info_t)); // must free this later

	FA_INFO->gameID = malloc(100);
	FA_INFO->pebbleID = malloc(100);
	FA_INFO->name = malloc(100);
	FA_INFO->team = malloc(100);
	FA_INFO->latitude = malloc(100);
	FA_INFO->longitude = malloc(100);
	FA_INFO->known_chars = malloc(100);
	FA_INFO->krag_to_submit = malloc(100);
	FA_INFO->hints_received = malloc(max_num_hints * sizeof(char*));

	if (FA_INFO->gameID == NULL || FA_INFO->pebbleID == NULL || FA_INFO->name == NULL || FA_INFO->team == NULL ||
		FA_INFO->latitude == NULL || FA_INFO->longitude == NULL || FA_INFO->known_chars == NULL ||
		FA_INFO->hints_received == NULL || FA_INFO->krag_to_submit == NULL) {
		return;
	}

	for (int i = 0; i < max_num_hints; i++){
		FA_INFO->hints_received[i] = malloc(max_hint_size + 1);
		if (FA_INFO->hints_received[i] != NULL) {
			strcpy(FA_INFO->hints_received[i], start_string);
		}
	}


	strcpy(FA_INFO->team, teamName);
	// FA_INFO->team = teamName; // initialize the team name to the set name
	strcpy(FA_INFO->gameID, init_gameID);
	// FA_INFO->gameID = init_gameID; // initialize the gameID to 0
	FA_INFO->time_passed = 0;
	FA_INFO->num_claimed = 0;

	FA_INFO->game_started = false;

	FA_INFO->submit_krag = false;
}


void delete_info()
{
	if (FA_INFO->gameID != NULL) {
		free(FA_INFO->gameID);
	}
	if (FA_INFO->pebbleID != NULL) {
		free(FA_INFO->pebbleID);
	}
	if (FA_INFO->name != NULL) {
		free(FA_INFO->name);
	}
	if (FA_INFO->team != NULL) {
		free(FA_INFO->team);
	}
	if (FA_INFO->latitude != NULL) {
		free(FA_INFO->latitude);
	}
	if (FA_INFO->longitude != NULL) {
		free(FA_INFO->longitude);
	}
	if (FA_INFO->known_chars != NULL) {
		free(FA_INFO->known_chars);
	}
	if (FA_INFO->krag_to_submit != NULL) {
		free(FA_INFO->krag_to_submit);
	}

	if (FA_INFO->hints_received != NULL) {
		for (int i = 0; i < max_num_hints; i++){
			if (FA_INFO->hints_received[i] != NULL) {
				free(FA_INFO->hints_received[i]);
			}
		}
		free(FA_INFO->hints_received);
	}


	if (FA_INFO != NULL) {
		free(FA_INFO);
	}
}





// Test functions
// typedef struct fieldagent_info {
// 	char *gameID;
// 	char* pebbleId;
// 	char* name;
// 	char* team;
// 	double latitude;
// 	double longitude;
// 	int num_claimed;
// 	int num_left;
// 	char* known_chars;
// 	char** hints_received;
// } fieldagent_info_t;
void print_FA()
{
	if (FA_INFO == NULL) {
		APP_LOG(APP_LOG_LEVEL_INFO, "FA_INFO is NULL.");
		return;
	}


	APP_LOG(APP_LOG_LEVEL_INFO, 
		"\nFA_INFO STRUCT:\n gameID: %s\n name: %s\n team: %s\n Lat: %d\n Long: %d\n Time: %d\n",
		 FA_INFO->gameID, FA_INFO->name, FA_INFO-> team, (int)FA_INFO->latitude, (int)FA_INFO->longitude, FA_INFO->time_passed);

}