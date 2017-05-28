/*****************************************************************/
/* This program          */
/*****************************************************************/
#include "field_agent_data.h"

static char teamName[7] = "views6";
static char init_gameID[2] = "0";


void create_info()
{
	FA_INFO = malloc(sizeof(fieldagent_info_t)); // must free this later
	FA_INFO->team = teamName; // initialize the team name to the set name
	FA_INFO->gameID = init_gameID; // initialize the gameID to 0
	FA_INFO->time_passed = 0;
	FA_INFO->num_claimed = 0;
}


void delete_info()
{
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