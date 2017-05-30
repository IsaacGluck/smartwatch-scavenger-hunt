/*
 * gs_message_handler.h - headler file for a function tables used for gameserver
 *
 * Handles message received from field agent or guide agent
 * 
 * Returning value: Required response (Not performed by this method)
 *
 * Common Returning value
 * 0: Successfully handled message, and nothing to be done
 * -1: SH_ERROR_INVALID_MESSAGE
 * -2: SH_ERROR_INVALID_OPCODE
 * -3: SH_ERROR_INVALID_FIELD
 * -4: SH_ERROR_INVALID_GAME_ID
 * -5: SH_ERROR_INVALID_TEAMNAME
 * -6: SH_ERROR_INVALID_PLAYERNAME
 * -7: SH_ERROR_INVALID_ID
 * -8: SH_DUPLICATE_PLAYERNAME
 * -99: malloc error
 *
 * FA_LOCATION specific returning value
 * 1: Respond with GAME_STATUS
 *
 * FA_CLAIM specific returning value
 * 1: Respond with SH_CLAIMED, GS_CLUE, and GS_SECRET
 * 2: Respond with SH_CLAIMED and finish the game
 * 3: Respond with SH_CLAIMED_ALREADY
 *
 * GA_STATUS specific returning value
 * 1: Respond with GAME_STATUS and GS_AGENT
 * 2: Respond with GAME_STATUS, GS_AGENT, and reveal two clue
 *
 * GA_HINT specific returning value
 * 1: Send the message to all FA in the team
 * 2: Forward the message to the specified player
 *
 * Kazuma Honjo, May 2017
 */

#ifndef __GS_MESSAGE_HANDLER_H
#define __GS_MESSAGE_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsstruct.h"


/**************** function for dispatch table ****************/
int fn_fa_location(char *rest_of_line, game_info_t *gi, struct sockaddr_in them);
int fn_fa_claim(char *rest_of_line, game_info_t *gi, struct sockaddr_in them);
int fn_fa_log(char *rest_of_line, game_info_t *gi, struct sockaddr_in them);
int fn_ga_status(char *rest_of_line, game_info_t *gi, struct sockaddr_in them);
int fn_ga_hint(char *rest_of_line, game_info_t *gi, struct sockaddr_in them);
int fn_game_status(char *rest_of_line, game_info_t *gi, struct sockaddr_in them);
int fn_gs_agent(char *rest_of_line, game_info_t *gi, struct sockaddr_in them);
int fn_gs_clue(char *rest_of_line, game_info_t *gi, struct sockaddr_in them);
int fn_gs_claimed(char *rest_of_line, game_info_t *gi, struct sockaddr_in them);
int fn_gs_secret(char *rest_of_line, game_info_t *gi, struct sockaddr_in them);
int fn_gs_response(char *rest_of_line, game_info_t *gi, struct sockaddr_in them);
int fn_game_over(char *rest_of_line, game_info_t *gi, struct sockaddr_in them);
int fn_team_record(char *rest_of_line, game_info_t *gi, struct sockaddr_in them);


/**************** function dispatch table ****************/
const struct {
    const char *opCode;
    int (*func)(char *rest_of_line, game_info_t *gi, struct sockaddr_in them);
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

#endif // __GS_MESSAGE_HANDLER_H
