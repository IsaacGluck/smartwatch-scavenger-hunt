/*****************************************************************/
/* This program          */
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
	int time_passed;
	char* latitude;
	char* longitude;
	int num_claimed;
	int num_left;
	char* end_message;
	char** hints_received;
	int num_hints;

	bool game_started;
	bool game_over_received;
	bool wrong_name;
	bool krag_claimed;
	bool krag_claimed_already;

	char* krag_to_submit;
	bool submit_krag;
} fieldagent_info_t;

fieldagent_info_t *FA_INFO; // glabal for all windows to get and change data


void create_info();
void delete_info();
void print_FA();


#endif