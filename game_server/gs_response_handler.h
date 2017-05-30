/*
 * gs_response_handler - header file for handleing response
 *
 * Kazuma Honjo, May 2017
 */

#ifndef __GS_RESPONSE_HANDLER_H
#define __GS_RESPONSE_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsstruct.h"

/* respond based on the resulting value of message handler function
 * If result = integer: Command
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
 * -9: SH_ERROR_DUPLICATE_FIELD
 * -10: SH_CLAIMED
 * -11: SH_CLAIMED_ALREADY
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
