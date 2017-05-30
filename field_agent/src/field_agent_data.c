/*****************************************************************/
/* This program sets up the main data structure to be passed*/
/*****************************************************************/
#include "field_agent_data.h"

// File global variables
static char teamName[7] = "views6";
static char pebbleId_init_string[5] = "init";
static char init_gameId[2] = "0";
static int max_num_hints = 10;
static int max_hint_size = 141;
static char start_string[5] = "None";
static int malloc_size = 200;
static char init_loc[4] = "0.0";


// Initializes the FA_INFO defined in field_agent_data.h
void create_info()
{
	FA_INFO = malloc(sizeof(fieldagent_info_t)); // must free this later

	FA_INFO->gameId = malloc(malloc_size);
	FA_INFO->pebbleId = malloc(malloc_size);	
	FA_INFO->name = malloc(malloc_size);
	FA_INFO->team = malloc(malloc_size);
	FA_INFO->latitude = malloc(malloc_size);
	FA_INFO->longitude = malloc(malloc_size);
	FA_INFO->end_message = malloc(malloc_size);
	FA_INFO->krag_to_submit = malloc(malloc_size);
	FA_INFO->hints_received = malloc(max_num_hints * sizeof(char*));

	if (FA_INFO->gameId == NULL || FA_INFO->pebbleId == NULL || FA_INFO->name == NULL || FA_INFO->team == NULL ||
		FA_INFO->latitude == NULL || FA_INFO->longitude == NULL || FA_INFO->hints_received == NULL ||
		FA_INFO->end_message == NULL || FA_INFO->krag_to_submit == NULL) {
		return;
	}

	// Initialize the hints to None
	for (int i = 0; i < max_num_hints; i++){
		FA_INFO->hints_received[i] = malloc(max_hint_size + 1);
		if (FA_INFO->hints_received[i] != NULL) {
			strcpy(FA_INFO->hints_received[i], start_string);
		}
	}

	// Initialize as many of the fields as possible
	strcpy(FA_INFO->team, teamName);
	strcpy(FA_INFO->gameId, init_gameId);
	strcpy(FA_INFO->pebbleId, pebbleId_init_string);
	strcpy(FA_INFO->latitude, init_loc);
	strcpy(FA_INFO->longitude, init_loc);	
	FA_INFO->time_passed = 0;
	FA_INFO->num_claimed = 0;
	FA_INFO->num_hints = 0;

	FA_INFO->game_started = false;
	FA_INFO->game_started = false;
	FA_INFO->wrong_name = false;
	FA_INFO->krag_claimed = false;
	FA_INFO->krag_claimed_already = false;
	FA_INFO->submit_krag = false;
	FA_INFO->invalid_krag_claimed = false;
}

// free the memory associated with the FA_INFO struct
void delete_info()
{
	if (FA_INFO->gameId != NULL) {
		free(FA_INFO->gameId);
	}
	if (FA_INFO->pebbleId != NULL) {
		free(FA_INFO->pebbleId);
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
	if (FA_INFO->end_message != NULL) {
		free(FA_INFO->end_message);
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
		FA_INFO = NULL;
	}
}



// Test function to print some of the fields of the FA_INFO
void print_FA()
{
	if (FA_INFO == NULL) {
		APP_LOG(APP_LOG_LEVEL_INFO, "FA_INFO is NULL.");
		return;
	}


	APP_LOG(APP_LOG_LEVEL_INFO, 
		"\nFA_INFO STRUCT:\n gameId: %s\n name: %s\n team: %s\n Lat: %d\n Long: %d\n Time: %d\n",
		 FA_INFO->gameId, FA_INFO->name, FA_INFO-> team, (int)FA_INFO->latitude, (int)FA_INFO->longitude, FA_INFO->time_passed);

}