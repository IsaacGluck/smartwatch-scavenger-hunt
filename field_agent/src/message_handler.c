/*****************************************************************/
/* This program          */
/*****************************************************************/

#include "message_handler.h"
#include "field_agent_data.h"
#include "../../common/shared.h"
#include "../../common/common.h"

static char GAME_STATUS[12] = "GAME_STATUS";
static char GS_RESPONSE[12] = "GS_RESPONSE";
static char GAME_OVER[10] = "GAME_OVER";
static char GA_HINT[8] = "GA_HINT";
static char start_gameID[2] = "0";

char* create_fa_location(char* statusReq)
{
	char* FA_LOCATION = malloc(500);
	if (FA_LOCATION == NULL) {
		return NULL;
	}
	
	char buff[500] = "";

	strncat(buff, "opCode=FA_LOCATION|gameId=", sizeof(buff));
	strncat(buff, FA_INFO->gameId, sizeof(buff) - strlen(buff));
	strncat(buff, "|pebbleId=", sizeof(buff) - strlen(buff));
	strncat(buff, FA_INFO->pebbleId, sizeof(buff) - strlen(buff));
	strncat(buff, "|team=", sizeof(buff) - strlen(buff));
	strncat(buff, FA_INFO->team, sizeof(buff) - strlen(buff));
	strncat(buff, "|player=", sizeof(buff) - strlen(buff));
	strncat(buff, FA_INFO->name, sizeof(buff) - strlen(buff));
	strncat(buff, "|latitude=", sizeof(buff) - strlen(buff));
	strncat(buff, FA_INFO->latitude, sizeof(buff) - strlen(buff));
	strncat(buff, "|longitude=", sizeof(buff) - strlen(buff));
	strncat(buff, FA_INFO->longitude, sizeof(buff) - strlen(buff));
	strncat(buff, "|statusReq=", sizeof(buff) - strlen(buff));
	strncat(buff, statusReq, sizeof(buff) - strlen(buff));

	memcpy(FA_LOCATION, buff, 500);


	return FA_LOCATION;
}


char* create_fa_claim(char* kragId)
{
	char* FA_CLAIM = malloc(500);
	if (FA_CLAIM == NULL) {
		return NULL;
	}


	char buff[500] = "";

	strncat(buff, "opCode=FA_CLAIM|gameId=", sizeof(buff));
	strncat(buff, FA_INFO->gameId, sizeof(buff) - strlen(buff));
	strncat(buff, "|pebbleId=", sizeof(buff) - strlen(buff));
	strncat(buff, FA_INFO->pebbleId, sizeof(buff) - strlen(buff));
	strncat(buff, "|team=", sizeof(buff) - strlen(buff));
	strncat(buff, FA_INFO->team, sizeof(buff) - strlen(buff));
	strncat(buff, "|player=", sizeof(buff) - strlen(buff));
	strncat(buff, FA_INFO->name, sizeof(buff) - strlen(buff));
	strncat(buff, "|latitude=", sizeof(buff) - strlen(buff));
	strncat(buff, FA_INFO->latitude, sizeof(buff) - strlen(buff));
	strncat(buff, "|longitude=", sizeof(buff) - strlen(buff));
	strncat(buff, FA_INFO->longitude, sizeof(buff) - strlen(buff));
	strncat(buff, "|kragId=", sizeof(buff) - strlen(buff));
	strncat(buff, kragId, sizeof(buff) - strlen(buff));

	memcpy(FA_CLAIM, buff, 500);


	return FA_CLAIM;
}


char* create_fa_log(char* text)
{
	char* FA_LOG = malloc(500);
	if (FA_LOG == NULL) {
		return NULL;
	}

	// text can be max 141 characters
	if ((int)strlen(text) > 141) {
		text[141] = '\0';
	}

	char buff[500] = "";

	strncat(buff, "opCode=FA_LOG|pebbleId=", sizeof(buff));
	strncat(buff, FA_INFO->pebbleId, sizeof(buff) - strlen(buff));
	strncat(buff, "|text=", sizeof(buff));
	strncat(buff, text, sizeof(buff) - strlen(buff));

	memcpy(FA_LOG, buff, 500);


	return FA_LOG;
}





void incoming_message(char* message)
{
	int validated = validate_message(message);
	if (validated != 0) { // it was not validated
		APP_LOG(APP_LOG_LEVEL_INFO, "Message could not be validated with code %d: %s\n", validated, message);
		return; // ignore the message
	}

	// char **opcode_data = getOpCode(message);
	// char *opCode = opcode_data[0];
	char** opcode_data = tokenize(message);
	char *opCode = opcode_data[1];



	if (strcmp(opCode, GAME_OVER) == 0) {
		APP_LOG(APP_LOG_LEVEL_INFO, "GAME_OVER received, ending game.\n");
		message_GAME_OVER(opcode_data);
	} else if (strcmp(opCode, GAME_STATUS) == 0){
		APP_LOG(APP_LOG_LEVEL_INFO, "GAME_STATUS received, updating...\n");
		message_GAME_STATUS(opcode_data);
	} else if (strcmp(opCode, GS_RESPONSE) == 0) {
		APP_LOG(APP_LOG_LEVEL_INFO, "GS_RESPONSE received with code %s\n", opCode);
		message_GS_RESPONSE(opcode_data);
	} else if (strcmp(opCode, GA_HINT) == 0) {
		APP_LOG(APP_LOG_LEVEL_INFO, "GA_HINT received, updating list of hints.\n");
		message_GA_HINT(opcode_data);
	} else {
		APP_LOG(APP_LOG_LEVEL_INFO, "Received message: %s\n", message); // just log don't do anything
	}

	free(opcode_data);
}


// opCode=GAME_OVER|gameId=FEED|secret=computer science 50 rocks!
void message_GAME_OVER(char** tokenized_message)
{

	char* message_gameId = NULL;
	char* secret = NULL;

	for (int i = 0; i < 6; i++) {
		if (strcmp(tokenized_message[i], "gameId") == 0) {
			message_gameId = tokenized_message[i+1];
		}

		if (strcmp(tokenized_message[i], "secret") == 0) {
			secret = tokenized_message[i+1];
		}
	}

	if (message_gameId == NULL || secret == NULL) {
		return;
	}

	if (strcmp(message_gameId, FA_INFO->gameId) != 0) { // wrong gameId
		APP_LOG(APP_LOG_LEVEL_INFO, "Wrong gameId in: %s\n", tokenized_message[1]); // just log don't do anything
		return;
	}

	char secret_buff[200];
	snprintf(secret_buff, sizeof(secret_buff), "Game over!\n The secret was: %s", secret);

	strcpy(FA_INFO->end_message, secret_buff);
	FA_INFO->game_over_received = true;


}

// opCode=GAME_STATUS|gameId=FEED|guideId=0707|numClaimed=5|numKrags=8
void message_GAME_STATUS(char** tokenized_message)
{

	char* message_gameId = NULL;
	char* numClaimed_s = NULL;
	char* numKrags_s = NULL;

	for (int i = 0; i < 10; i++) {
		if (strcmp(tokenized_message[i], "gameId") == 0) {
			message_gameId = tokenized_message[i+1];
		}

		if (strcmp(tokenized_message[i], "numClaimed") == 0) {
			numClaimed_s = tokenized_message[i+1];
		}

		if (strcmp(tokenized_message[i], "numKrags") == 0) {
			numKrags_s = tokenized_message[i+1];
		}
	}

	if (message_gameId == NULL || numClaimed_s == NULL || numKrags_s == NULL) {
		return;
	}

	if (strcmp(message_gameId, FA_INFO->gameId) != 0 && strcmp(FA_INFO->gameId, start_gameID) != 0) { // wrong gameId
		APP_LOG(APP_LOG_LEVEL_INFO, "Wrong gameId in: %s\n", tokenized_message[1]); // just log don't do anything
		return;
	}

	if (strcmp(FA_INFO->gameId, start_gameID) == 0) {
		strcpy(FA_INFO->gameId, message_gameId);
	}

	int numClaimed = string_to_int(numClaimed_s);
	int numKrags = string_to_int(numKrags_s);

	FA_INFO->num_claimed = numClaimed;
	FA_INFO->num_left = numKrags - numClaimed;

}


// opCode=GS_RESPONSE|gameId=0707|respCode=SH_ERROR_INVALId_OPCODE|text=Unrecognized opCode 'GA_FOO'
void message_GS_RESPONSE(char** tokenized_message)
{

	char* message_gameId = NULL;
	char* respCode = NULL;
	char* text = NULL;

	for (int i = 0; i < 8; i++) {
		if (strcmp(tokenized_message[i], "gameId") == 0) {
			message_gameId = tokenized_message[i+1];
		}

		if (strcmp(tokenized_message[i], "respCode") == 0) {
			respCode = tokenized_message[i+1];
		}

		if (strcmp(tokenized_message[i], "text") == 0) {
			text = tokenized_message[i+1];
		}
	}

	if (message_gameId == NULL || respCode == NULL || text == NULL) {
		return;
	}

	if (strcmp(message_gameId, FA_INFO->gameId) != 0 && strcmp(FA_INFO->gameId, start_gameID) != 0) { // wrong gameId
		APP_LOG(APP_LOG_LEVEL_INFO, "Wrong gameId in: %s\n", tokenized_message[1]); // just log don't do anything
		return;
	}

	if(strcmp(respCode, "SH_ERROR_INVALId_PLAYERNAME") == 0 || strcmp(respCode, "SH_ERROR_DUPLICATE_PLAYERNAME") == 0) {
		FA_INFO->wrong_name = true;
	}

	if(strcmp(respCode, "SH_CLAIMED_ALREADY") == 0) {
		FA_INFO->krag_claimed_already = true;
	}

	if(strcmp(respCode, "SH_CLAIMED") == 0) {
		FA_INFO->krag_claimed = true;
	}

	if (strcmp(respCode, "SH_ERROR_INVALID_ID") == 0) {
		FA_INFO->invalid_krag_claimed = true;
	}

	if (strcmp(respCode, "SH_ERROR_INVALID_MESSAGE") == 0) {
		FA_INFO->invalid_message = true;
	}


}


// opCode=GA_HINT|gameId=FEED|guideId=0707|team=aqua|player=Alice|pebbleId=8080477D|hint=Bob, look inside the cafe!
void message_GA_HINT(char** tokenized_message)
{

	char* message_gameId = NULL;
	char* team = NULL;
	char* name = NULL;
	char* pebbleId = NULL;
	char* hint = NULL;

	for (int i = 0; i < 14; i++) {
		if (strcmp(tokenized_message[i], "gameId") == 0) {
			message_gameId = tokenized_message[i+1];
		}

		if (strcmp(tokenized_message[i], "team") == 0) {
			team = tokenized_message[i+1];
		}

		if (strcmp(tokenized_message[i], "player") == 0) {
			name = tokenized_message[i+1];
		}

		if (strcmp(tokenized_message[i], "pebbleId") == 0) {
			pebbleId = tokenized_message[i+1];
		}

		if (strcmp(tokenized_message[i], "hint") == 0) {
			hint = tokenized_message[i+1];
		}
	}

	if (message_gameId == NULL || team == NULL || name == NULL || pebbleId == NULL || hint == NULL) {
		return;
	}

	if(strcmp(FA_INFO->gameId, message_gameId) != 0) {
		APP_LOG(APP_LOG_LEVEL_INFO, "Wrong gameId in: %s\n", tokenized_message[1]);
		return;
	}

	if(strcmp(FA_INFO->team, team) != 0) {
		APP_LOG(APP_LOG_LEVEL_INFO, "Wrong team in: %s\n", tokenized_message[1]);
		return;
	}

	if(strcmp(FA_INFO->pebbleId, pebbleId) != 0) {
		APP_LOG(APP_LOG_LEVEL_INFO, "Wrong pebbleId in: %s\n", tokenized_message[1]);
		return;
	}

	for(int i = 9; i > 0; i--) {
		strcpy(FA_INFO->hints_received[i], FA_INFO->hints_received[i - 1]); // shift up
	}

	strcpy(FA_INFO->hints_received[0], hint);

}