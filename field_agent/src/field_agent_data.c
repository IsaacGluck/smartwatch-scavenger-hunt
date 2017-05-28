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
	FA_INFO->latitude = 0;
	FA_INFO->longitude = 0;
	FA_INFO->num_claimed = 0;
}


void delete_info()
{
	if (FA_INFO != NULL) {
		free(FA_INFO);
	}
}