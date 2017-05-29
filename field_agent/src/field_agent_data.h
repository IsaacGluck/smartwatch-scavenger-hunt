/*****************************************************************/
/* This program          */
/*****************************************************************/
#ifndef __FIELD_AGENT_DATA_H
#define __FIELD_AGENT_DATA_H

#include <pebble.h>


// Global Structs
typedef struct fieldagent_info {
	char *gameID;
	char* pebbleId;
	char* name;
	char* team;
	int time_passed;
	char* latitude;
	char* longitude;
	int num_claimed;
	int num_left;
	char* known_chars;
	char** hints_received;
} fieldagent_info_t;

fieldagent_info_t *FA_INFO; // glabal for all windows to get and change data


void create_info();
void delete_info();
void print_FA();


#endif