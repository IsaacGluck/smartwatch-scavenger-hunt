/*****************************************************************/
/* This program          */
/*****************************************************************/
#ifndef __FIELD_AGENT_DATA_H
#define __FIELD_AGENT_DATA_H

#include <pebble.h>


// Global Structs
typedef struct fieldagent_info {
	char *gameID;
	char* pebbleID;
	char* name;
	char* team;
	int time_passed;
	char* latitude;
	char* longitude;
	int num_claimed;
	int num_left;
	char* known_chars;
	char** hints_received;

	bool game_started;

	char* krag_to_submit;
	bool submit_krag;
} fieldagent_info_t;

fieldagent_info_t *FA_INFO; // glabal for all windows to get and change data


void create_info();
void delete_info();
void print_FA();


#endif