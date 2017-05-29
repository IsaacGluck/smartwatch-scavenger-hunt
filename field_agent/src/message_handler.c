/*****************************************************************/
/* This program          */
/*****************************************************************/

#include "message_handler.h"
#include "field_agent_data.h"




char* create_fa_location(char* statusReq)
{
	char* FA_LOCATION = malloc(500);
	if (FA_LOCATION == NULL) {
		return NULL;
	}
	
	char buff[500] = "";
	// snprintf(buff, sizeof(buff),
	// 	"opCode=FA_LOCATION|gameId=%s|pebbleId=%s|team=%s|player=%s|latitude=%s|longitude=%s|statusReq=%s",
	// 	FA_INFO->gameID, FA_INFO->pebbleID, FA_INFO->team, FA_INFO->name, FA_INFO->latitude, FA_INFO->longitude, statusReq);

	// APP_LOG(APP_LOG_LEVEL_INFO, "\nCREATED: %s\n\n", buff);

	// strcpy(FA_LOCATION, buff);

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



	// APP_LOG(APP_LOG_LEVEL_INFO, "\nCREATED: %s\n\n", FA_LOCATION);

	return FA_LOCATION;
}


char* create_fa_claim(char* kragId)
{
	char* FA_CLAIM = malloc(500);
	if (FA_CLAIM == NULL) {
		return NULL;
	}


	char buff[500] = "";
	// snprintf(buff, sizeof(buff),
	// 	"opCode=FA_LOCATION|gameId=%s|pebbleId=%s|team=%s|player=%s|latitude=%s|longitude=%s|statusReq=%s",
	// 	FA_INFO->gameID, FA_INFO->pebbleID, FA_INFO->team, FA_INFO->name, FA_INFO->latitude, FA_INFO->longitude, statusReq);

	// APP_LOG(APP_LOG_LEVEL_INFO, "\nCREATED: %s\n\n", buff);

	// strcpy(FA_LOCATION, buff);

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





	// char temp[500];
	// snprintf(temp, sizeof(temp),
	// 	"opCode=FA_CLAIM|gameId=%s|pebbleId=%s|team=%s|player=%s|latitude=%s|longitude=%s|kragId=%s",
	// 	FA_INFO->gameID, FA_INFO->pebbleID, FA_INFO->team, FA_INFO->name, FA_INFO->latitude, FA_INFO->longitude, kragId);

	return FA_CLAIM;
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