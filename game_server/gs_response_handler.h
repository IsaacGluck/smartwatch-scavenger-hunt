

#ifndef __GS_RESPONSE_HANDLER_H
#define __GS_RESPONSE_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsstruct.h"

/* respond based on the resulting value of message handler function
 */
void respond(char *opCode, int result, int comm_sock, struct sockaddr_in them, game_info_t *gi, char *message_from);

/* Send GAME OVER to all agents
 * opCode=GAME_OVER|gameId=|secret=
 */
void send_game_over(int comm_sock, game_info_t *gi);

/* Send TEAM RECORD to all agents
 * opCode=TEAM_RECORD|gameId=|team=|numClaimed=|numPlayers=
 */
void send_team_record(int comm_sock, game_info_t *gi);


#endif //__GS_RESPONSE_HANDLER_H
