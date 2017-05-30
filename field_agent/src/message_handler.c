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


char* create_fa_location(char* statusReq)
{
	char* FA_LOCATION = malloc(500);
	if (FA_LOCATION == NULL) {
		return NULL;
	}
	
	char buff[500] = "";

	strncat(buff, "opCode=FA_LOCATION|gameId=", sizeof(buff));
	strncat(buff, FA_INFO->gameID, sizeof(buff) - strlen(buff));
	strncat(buff, "|pebbleId=", sizeof(buff) - strlen(buff));
	strncat(buff, FA_INFO->pebbleID, sizeof(buff) - strlen(buff));
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
	strncat(buff, FA_INFO->gameID, sizeof(buff) - strlen(buff));
	strncat(buff, "|pebbleId=", sizeof(buff) - strlen(buff));
	strncat(buff, FA_INFO->pebbleID, sizeof(buff) - strlen(buff));
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
	strncat(buff, FA_INFO->pebbleID, sizeof(buff) - strlen(buff));
	strncat(buff, "|text=", sizeof(buff));
	strncat(buff, text, sizeof(buff) - strlen(buff));

	memcpy(FA_LOG, buff, 500);


	return FA_LOG;
}





void incoming_message(char* message)
{
	if (validate_message(message) != 0) { // it was not validated
		APP_LOG(APP_LOG_LEVEL_INFO, "Message could not be validated: %s\n", message);
		return; // ignore the message
	}

	char **opcode_data = getOpCode(message);
	char *opCode = opcode_data[0];

	if (strcmp(opCode, GAME_OVER) == 0) {
		APP_LOG(APP_LOG_LEVEL_INFO, "GAME_OVER received, ending game.\n");
		message_GAME_OVER(message);
	} else if (strcmp(opCode, GAME_STATUS) == 0){
		APP_LOG(APP_LOG_LEVEL_INFO, "GAME_STATUS received, updating...\n");
		message_GAME_STATUS(message);
	} else if (strcmp(opCode, GS_RESPONSE) == 0) {
		APP_LOG(APP_LOG_LEVEL_INFO, "GAME_STATUS received, updating...\n");
		message_GS_RESPONSE(message);
	} else {
		APP_LOG(APP_LOG_LEVEL_INFO, "Received message: %s\n", message); // just log don't do anything
	}

	free(opcode_data);
}


// opCode=GAME_OVER|gameId=FEED|secret=computer science 50 rocks!
void message_GAME_OVER(char* message)
{
	char** tokenized_message = tokenize(message);

	char* message_gameID = NULL;
	char* secret = NULL;

	for (int i = 0; i < 6; i++) {
		if (strcmp(tokenized_message[i], "gameID") == 0) {
			message_gameID = tokenized_message[i+1];
		}

		if (strcmp(tokenized_message[i], "secret") == 0) {
			secret = tokenized_message[i+1];
		}
	}

	if (message_gameID == NULL || secret == NULL) {
		return;
	}

	if (strcmp(message_gameID, FA_INFO->gameID) != 0) { // wrong gameID
		APP_LOG(APP_LOG_LEVEL_INFO, "Wrong gameID in: %s\n", message); // just log don't do anything
		return;
	}

	char secret_buff[200];
	snprintf(secret_buff, sizeof(secret_buff), "Game over!\n The secret was: %s", secret);

	strcpy(FA_INFO->known_chars, secret_buff);
	FA_INFO->game_over_received = true;


	free(tokenized_message);
}

// opCode=GAME_STATUS|gameId=FEED|guideId=0707|numClaimed=5|numKrags=8
void message_GAME_STATUS(char* message)
{
	char** tokenized_message = tokenize(message);

	char* message_gameID = NULL;
	char* numClaimed_s = NULL;
	char* numKrags_s = NULL;

	for (int i = 0; i < 10; i++) {
		if (strcmp(tokenized_message[i], "gameID") == 0) {
			message_gameID = tokenized_message[i+1];
		}

		if (strcmp(tokenized_message[i], "numClaimed") == 0) {
			numClaimed_s = tokenized_message[i+1];
		}

		if (strcmp(tokenized_message[i], "numKrags") == 0) {
			numKrags_s = tokenized_message[i+1];
		}
	}

	if (message_gameID == NULL || numClaimed_s == NULL || numKrags_s == NULL) {
		return;
	}

	if (strcmp(message_gameID, FA_INFO->gameID) != 0) { // wrong gameID
		APP_LOG(APP_LOG_LEVEL_INFO, "Wrong gameID in: %s\n", message); // just log don't do anything
		return;
	}

	int numClaimed = string_to_int(numClaimed_s);
	int numKrags = string_to_int(numKrags_s);

	FA_INFO->num_claimed = numClaimed;
	FA_INFO->num_left = numKrags - numClaimed;

	free(tokenized_message);
}




// SH_ERROR_INVALID_MESSAGE
// SH_ERROR_INVALID_OPCODE
// SH_ERROR_INVALID_FIELD
// SH_ERROR_DUPLICATE_FIELD
// SH_ERROR_INVALID_GAME_ID
// SH_ERROR_INVALID_TEAMNAME
// SH_ERROR_INVALID_ID


// SH_ERROR_INVALID_PLAYERNAME
// SH_ERROR_DUPLICATE_PLAYERNAME
// SH_CLAIMED
// SH_CLAIMED_ALREADY

// opCode=GS_RESPONSE|gameId=0707|respCode=SH_ERROR_INVALID_OPCODE|text=Unrecognized opCode 'GA_FOO'
void message_GS_RESPONSE(char* message)
{
	char** tokenized_message = tokenize(message);

	char* message_gameID = NULL;
	char* respCode = NULL;
	char* text = NULL;

	for (int i = 0; i < 8; i++) {
		if (strcmp(tokenized_message[i], "gameID") == 0) {
			message_gameID = tokenized_message[i+1];
		}

		if (strcmp(tokenized_message[i], "respCode") == 0) {
			respCode = tokenized_message[i+1];
		}

		if (strcmp(tokenized_message[i], "text") == 0) {
			text = tokenized_message[i+1];
		}
	}

	if (message_gameID == NULL || respCode == NULL || text == NULL) {
		return;
	}

	if (strcmp(message_gameID, FA_INFO->gameID) != 0) { // wrong gameID
		APP_LOG(APP_LOG_LEVEL_INFO, "Wrong gameID in: %s\n", message); // just log don't do anything
		return;
	}

	if(strcmp(respCode, "SH_ERROR_INVALID_PLAYERNAME") == 0 || strcmp(respCode, "SH_ERROR_DUPLICATE_PLAYERNAME") == 0) {
		FA_INFO->wrong_name = true;
	}

	if(strcmp(respCode, "SH_CLAIMED_ALREADY") == 0) {
		FA_INFO->krag_claimed_already = true;
	}

	if(strcmp(respCode, "SH_CLAIMED") == 0) {
		FA_INFO->krag_claimed = true;
	}


	free(tokenized_message);
}



// void double_to_string(double to_str, char *str)
// {
// 	// Get before the .
//   int before = (int)to_str;

//   // Get after the .
//   float after = to_str - (float)before;

//   // make part a string
//   int_to_string(before, str);
//   int len = strlen(str);

//   str[len] = '.';  // add the .

//   // Get the value of fraction part upto given no.
//   // of points after dot. The third parameter is needed
//   // to handle cases like 233.007
//   fpart = fpart * pow(10, afterpoint);

//   intToStr((int)fpart, res + i + 1, afterpoint);

// }







// void int_to_string(int to_str, char str[])
// {
//     char temp[strlen(str)];

//     // move the int backwards to temp
//     int i = 0;
//     while (to_str) { // while to_str is not 0    
//         temp[i] = (to_str % 10) + '0'; // add '0' to get ascii code of number
//         to_str = to_str / 10; // get rid of last digit
//         i++;
//     }

//     // reverse temp
//     int j = i-1;
//     for (int k = 0; k < i; k++) {
//         str[k] = temp[j];
//       	j--;
//     }
//     str[i] = '\0';
// }