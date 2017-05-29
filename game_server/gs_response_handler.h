

#ifndef __GS_RESPONSE_HANDLER_H
#define __GS_RESPONSE_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsstruct.h"

/* respond based on the resulting value of message handler function
 */
void respond(char *opCode, int result, int comm_sock, struct sockaddr_in them, game_info_t *gi, char *message_from);

#endif //__GS_RESPONSE_HANDLER_H
