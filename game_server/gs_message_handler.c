/*
 * gs_message_handler - a function tables used for gameserver
 *
 * Handles message received from field agent or guide agent
 *
 * Kazuma Honjo, May 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**************** functions ****************/
static int fn_fa_location;
static int fn_fa_claim;
static int fn_fa_log;
static int fn_ga_status;
static int fn_ga_hint;
static int fn_game_status;
static int fn_gs_agent;
static int fn_gs_clue;
static int fn_gs_claimed;
static int fn_gs_secret;
static int fn_gs_response;
static int fn_game_over;
static int fn_team_record;

/**************** function dispatch table ****************/
static const struct {
    const char *command
    void (*func)(char *rest_of_line);
} dispatch[] = {
    { "FA_LOCATION", fn_fa_location },
    { "FA_CLAIM", fn_fa_claim },
    { "FA_LOG", fn_fa_log },
    { "GA_STATUS", fn_ga_status },
    { "GA_HINT", fn_ga_hint },
    { "GAME_STATUS", fn_game_status },
    { "GS_AGENT", fn_gs_agent },
    { "GS_CLUE", fn_gs_clue },
    { "GS_CLAIMED", fn_gs_claimed },
    { "GS_SECRET", fn_gs_secret },
    { "GS_RESPONSE", fn_gs_response },
    { "GAME_OVER", fn_game_over },
    { "TEAM_RECORD", fn_team_record },
    { NULL, NULL }
};


static int fn_fa_location;
static int fn_fa_claim;
static int fn_fa_log;
static int fn_ga_status;
static int fn_ga_hint;
static int fn_game_status;
static int fn_gs_agent;
static int fn_gs_clue;
static int fn_gs_claimed;
static int fn_gs_secret;
static int fn_gs_response;
static int fn_game_over;
static int fn_team_record;
