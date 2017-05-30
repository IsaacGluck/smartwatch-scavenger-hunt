/*****************************************************************/
/* This program          */
/*****************************************************************/
#ifndef _MESSAGE_HANDLER_H
#define _MESSAGE_HANDLER_H

#include <pebble.h>


char* create_fa_location(char* statusReq);

char* create_fa_claim(char* kragId);

char* create_fa_log(char* text);

void incoming_message(char* message);

void message_GAME_OVER(char** tokenized_message);

void message_GAME_STATUS(char** tokenized_message);

void message_GS_RESPONSE(char** tokenized_message);

void message_GA_HINT(char** tokenized_message);




#endif
