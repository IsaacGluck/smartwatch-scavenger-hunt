

#ifndef __GS_MESSAGE_HANDLER_H
#define __GS_MESSAGE_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsstruct.h"


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
