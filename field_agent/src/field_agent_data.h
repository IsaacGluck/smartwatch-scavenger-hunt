/*****************************************************************/
/* This program defines the FA_INFO struct and the global FA_INFO itself.*/
/*****************************************************************/
#ifndef __FIELD_AGENT_DATA_H
#define __FIELD_AGENT_DATA_H

#include <pebble.h>


// Global Structs
typedef struct fieldagent_info {
	char *gameId;
	char* pebbleId;
	char* name;
	char* team;
	char* latitude;
	char* longitude;
	char* end_message;
	char* krag_to_submit;
	char** hints_received;

	int num_hints;
	int num_claimed;
	int num_left;
	int time_passed;

	bool game_started;
	bool game_over_received;
	bool wrong_name;
	bool krag_claimed;
	bool krag_claimed_already;
	bool invalid_krag_claimed;
	bool submit_krag;
} fieldagent_info_t;

fieldagent_info_t *FA_INFO; // glabal for all windows to get and change data

// Initializes the FA_INFO defined in field_agent_data.h
void create_info();

// free the memory associated with the FA_INFO struct
void delete_info();

// Test function to print some of the fields of the FA_INFO
void print_FA();


#endif