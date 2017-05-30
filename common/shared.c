#ifdef NOPEBBLE // we are *not* building for pebble
// Conditional inclusion for platform specific builds
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "shared.h"
#include "time.h"
#include <ctype.h>
#include <stdbool.h>
#else // we are building for pebble
#include <pebble.h>
#endif




int validate_message(char* m);
char** tokenize(char* message);

#ifdef NOPEBBLE // we are *not* building for pebble
int print_log(char* message, char* filename, char* IPport, char* tofrom);

static int gsAgent(char* parameters[], int total);
static int gsClue(char* parameters[], int total);
static int gsClaimed(char* parameters[], int total);
static int gsSecret(char* parameters[], int total);
static int teamRecord(char* parameters[], int total);
static int gaStatus(char* parameters[], int total);
#endif


static int gameStatus(char* parameters[], int total);
static int gsResponse(char* parameters[], int total);
static int gaHint(char* parameters[], int total);
static int faLocation(char* parameters[], int total);
static int faClaim(char* parameters[], int total);
static int faLog(char* parameters[], int total);
static int gameOver(char* parameters[], int total);


// helpers for pebble
char ishex (unsigned char c);
double string_to_double(char* num);
int string_to_int(char *str);

static const struct {
  const char *opCodes;
  int (*func)(char* parameters[], int total);
} codes[] = {

  { "FA_LOCATION", faLocation },
  { "FA_CLAIM", faClaim },
  { "FA_LOG", faLog },
  { "GAME_STATUS", gameStatus },
  { "GAME_OVER", gameOver },
  { "GS_RESPONSE", gsResponse },
  { "GA_HINT", gaHint },

  #ifdef NOPEBBLE // we are *not* building for pebble
  { "GA_STATUS", gaStatus },
  { "GS_AGENT", gsAgent },
  { "GS_CLUE", gsClue },
  { "GS_CLAIMED", gsClaimed },
  { "GS_SECRET", gsSecret },
  { "TEAM_RECORD", teamRecord },
  #endif

  { NULL, NULL }
};

const char *version_str = "1.0";

void print_shared() {
  printf("*** shared function! (v%s) ***", version_str);
}


int validate_message(char* m){
	// printf("IN THIS METHOD MEH\n");
	//check size max is 8191, if not, return -1 

	char* message = malloc(strlen(m)+1);
	strcpy(message, m); 

	if(strlen(message)> 8191 && strlen(message)>0){
        free(message);
		return -1;  
	}

	int total = 0; 
	for(int i = 0; i< (int)strlen(message); i++){
		if(message[i] == '|' || message[i] == '='){
			total++; 
		}
	}

	char** array = tokenize(message);
	if (array == NULL) {
    
    free(message);
		return 1;
	}

	//array wasnt set right 
	for(int i = 0; i <= total; i++){
		if(array[i] == NULL){
            free(message);
            free(array);
			return 1; 
		}
	}

	int fn;
	for (fn = 0; codes[fn].opCodes != NULL; fn++) {
  		if (strcmp(array[1], codes[fn].opCodes) == 0) {
            int result = (*codes[fn].func)(array, total+1);
            free(message);
            free(array);
  			return result;
  		}
	}
	
	if (codes[fn].opCodes == NULL){
        free(message);
        free(array);
  		// printf("Unknown command: '%s'\n", array[1]);
  		return 6;
 	 }

	//send to right opcode method 
		//if error about parameters types return 2 
		//if duplicates retrn 3
		//if errors about parameters that there should be return 4 
	free(message);
    free(array);
	return 0;
}


char** tokenize(char* message)
{
	int total = 0; 
	for(int i = 0; i< (int)strlen(message); i++){
		if(message[i] == '|' || message[i] == '='){
			total++; 
		}
	}
	
	if (total <=1){
		return NULL; 
	}
	
	char** array = malloc(sizeof(char*) * (total + 1));
	if(array == NULL){
		return NULL; 
	}

	char* pointer = message;

	int message_length = strlen(message);

	int count = 1; 
	array[0] = pointer;
	for(int i = 0; i < message_length; i++){
		if(pointer[i] == '|' || pointer[i] == '='){	
			pointer[i] = '\0';
			array[count] = &pointer[i + 1];
			count++;  
		}
	}
	
	return array;
}


#ifdef NOPEBBLE // we are *not* building for pebble

static int gsAgent(char* parameters[], int total){
	if(total != 16){
		return 5;
	}

	char* opcode=  parameters[0];
	if(strcmp(opcode, "opCode")!=0){
		return 4; 
	}
	char* gameid = parameters[2];
	if(strcmp(gameid, "gameId")!=0){
		return 4; 
	}

	char* pebbleID = parameters[4];
	if(strcmp(pebbleID, "pebbleId")!=0){
		return 4; 
	}

	char* team = parameters[6];
	if(strcmp(team, "team")!=0){
		return 4; 
	}
	char* player = parameters[8]; 
	if(strcmp(player, "player")!=0){
		return 4; 
	}

	char* lat = parameters[10];
	if(strcmp(lat, "latitude") !=0){
		return 4;
	}

	char* lon = parameters[12]; 
	if(strcmp(lon, "longitude")!=0){
		return 4;
	}
	char* lastcontact = parameters[14]; 
	if(strcmp(lastcontact, "lastContact")!=0){
		return 4;
	}

	// printf("Checking game id\n");
	if(strlen(parameters[3])>0 && strlen(parameters[3])<9){
		for(int i = 0; i< strlen(parameters[3]); i++){
			if(!ishex(parameters[3][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	//checking hex for parameter 5 pebble id
	if(strlen(parameters[5])>0 && strlen(parameters[5])<9){
		for(int i = 0; i< strlen(parameters[5]); i++){
			if(!ishex(parameters[5][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	//team
	if((int)strlen(parameters[7]) < 0 || (int)strlen(parameters[7]) > 11){
		return 3; 
	}

	//player
	if((int)strlen(parameters[9]) < 0 || (int)strlen(parameters[9]) > 11){
		return 3; 
	}

	//lat 
	int latitude = string_to_double(parameters[11]); 
	if(latitude< -90 || latitude > 90){
		return 3; 
	}

	int longitude = string_to_double(parameters[13]); 
	if(longitude< -180 || longitude > 180){
		return 3; 
	}

	int lc = string_to_double(parameters[15]); 
	if(lc <0 ){
		return 3;
	}

	//comparing for the same answers
	if(strcmp(parameters[3], parameters[5]) == 0 || strcmp(parameters[3], parameters[7]) == 0 || strcmp(parameters[3], parameters[9]) == 0 || strcmp(parameters[3], parameters[11]) == 0 || strcmp(parameters[3], parameters[13]) == 0 || strcmp(parameters[3], parameters[15]) == 0){
		return 3; 
	}
	if(strcmp(parameters[5], parameters[7]) == 0 || strcmp(parameters[5], parameters[9]) == 0 || strcmp(parameters[5], parameters[11]) == 0 || strcmp(parameters[5], parameters[13]) == 0 || strcmp(parameters[5], parameters[15]) == 0){
		return 3; 
	}
	if(strcmp(parameters[7], parameters[9]) == 0 || strcmp(parameters[7], parameters[11]) == 0 || strcmp(parameters[7], parameters[13]) == 0 || strcmp(parameters[7], parameters[15]) == 0){
		return 3; 
	}
	if( strcmp(parameters[9], parameters[11]) == 0 || strcmp(parameters[9], parameters[13]) == 0 || strcmp(parameters[9], parameters[15]) == 0){
		return 3; 
	}
	if(strcmp(parameters[11], parameters[13]) == 0 || strcmp(parameters[11], parameters[15]) == 0){
		return 3;
	}
	if(strcmp(parameters[13], parameters[15]) == 0){
		return 3;
	}



	return 0;
}

static int gsClue(char* parameters[], int total){
	if(total != 10){
		return 5;
	}

	char* opcode=  parameters[0];
	if(strcmp(opcode, "opCode")!=0){
		return 4; 
	}
	char* gameid = parameters[2];
	if(strcmp(gameid, "gameId")!=0){
		return 4; 
	}
	char* guideid = parameters[4]; 
	if(strcmp(guideid, "guideId")!=0){
		return 4; 
	}
	char* kragid = parameters[6];
	if(strcmp(kragid, "kragId")!=0){
		return 4; 
	}
	char* clue = parameters[8]; 
	if(strcmp(clue, "clue")!=0){
		return 4; 
	}

	if(strlen(parameters[3])>0 && strlen(parameters[3])<9){
		for(int i = 0; i< strlen(parameters[3]); i++){
			if(!ishex(parameters[3][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}
	
	if(strlen(parameters[5])>0 && strlen(parameters[5])<9){
		for(int i = 0; i< strlen(parameters[5]); i++){
			if(!ishex(parameters[5][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}
	if(strlen(parameters[7])>0 && strlen(parameters[7])<5){
		for(int i = 0; i< strlen(parameters[7]); i++){
			if(!ishex(parameters[7][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	if(strlen(parameters[9])<1){
		return 3;
	}

	if(strcmp(parameters[3], parameters[5]) == 0 || strcmp(parameters[3], parameters[7]) == 0 || strcmp(parameters[3], parameters[9]) == 0){
		return 3; 
	}
	if(strcmp(parameters[5], parameters[7]) == 0 || strcmp(parameters[5], parameters[9]) == 0){
		return 3; 
	}
	if(strcmp(parameters[7], parameters[9]) == 0){
		return 3; 
	}


	return 0;
}

static int gsClaimed(char* parameters[], int total){
	if(total != 14){
		return 5;
	}

	char* opcode=  parameters[0];
	if(strcmp(opcode, "opCode")!=0){
		return 4; 
	}
	char* gameid = parameters[2];
	if(strcmp(gameid, "gameId")!=0){
		return 4; 
	}
	char* guideid = parameters[4]; 
	if(strcmp(guideid, "guideId")!=0){
		return 4; 
	}
	char* pebbleId = parameters[6];
	if(strcmp(pebbleId, "pebbleId")!=0){
		return 4; 
	}
	char* kragId = parameters[8]; 
	if(strcmp(kragId, "kragId")!=0){
		return 4; 
	}
	char* lat = parameters[10];
	if(strcmp(lat, "latitude") !=0){
		return 4;
	}

	char* lon = parameters[12]; 
	if(strcmp(lon, "longitude")!=0){
		return 4;
	}

	//game ID
	if(strlen(parameters[3])>0 && strlen(parameters[3])<9){
		for(int i = 0; i< strlen(parameters[3]); i++){
			if(!ishex(parameters[3][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}
	
	//guide ID 
	if(strlen(parameters[5])>0 && strlen(parameters[5])<9){
		for(int i = 0; i< strlen(parameters[5]); i++){
			if(!ishex(parameters[5][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}
	//pebble ID
	if(strlen(parameters[7])>0 && strlen(parameters[7])<9){
		for(int i = 0; i< strlen(parameters[7]); i++){
			if(!ishex(parameters[7][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}
	//krag ID
	if(strlen(parameters[9])>0 && strlen(parameters[9])<5){
		for(int i = 0; i< strlen(parameters[9]); i++){
			if(!ishex(parameters[9][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	//check lat
	int latitude = string_to_double(parameters[11]); 
	if(latitude< -90 || latitude > 90){
		return 3; 
	}
	//check long
	int longitude = string_to_double(parameters[13]); 
	if(longitude< -180 || longitude > 180){
		return 3; 
	}

	if(strcmp(parameters[3], parameters[5]) == 0 || strcmp(parameters[3], parameters[7]) == 0 || strcmp(parameters[3], parameters[9]) == 0 || strcmp(parameters[3], parameters[11]) == 0 || strcmp(parameters[3], parameters[13]) == 0 ){
		return 3; 
	}
	if(strcmp(parameters[5], parameters[7]) == 0 || strcmp(parameters[5], parameters[9]) == 0 || strcmp(parameters[5], parameters[11]) == 0 || strcmp(parameters[5], parameters[13]) == 0 ){
		return 3; 
	}
	if(strcmp(parameters[7], parameters[9]) == 0 || strcmp(parameters[7], parameters[11]) == 0 || strcmp(parameters[7], parameters[13]) == 0){
		return 3; 
	}
	if( strcmp(parameters[9], parameters[11]) == 0 || strcmp(parameters[9], parameters[13]) == 0){
		return 3; 
	}
	if(strcmp(parameters[11], parameters[13]) == 0 ){
		return 3;
	}



	return 0;
}

static int gsSecret(char* parameters[], int total){
	if(total != 8){
		return 5;
	}

	char* opcode=  parameters[0];
	if(strcmp(opcode, "opCode")!=0){
		return 4; 
	}
	char* gameid = parameters[2];
	if(strcmp(gameid, "gameId")!=0){
		return 4; 
	}
	char* guideid = parameters[4]; 
	if(strcmp(guideid, "guideId")!=0){
		return 4; 
	}
	char* secret = parameters[6]; 
	if(strcmp(secret, "secret")!=0){
		return 4; 
	}
	//game ID
	if(strlen(parameters[3])>0 && strlen(parameters[3])<9){
		for(int i = 0; i< strlen(parameters[3]); i++){
			if(!ishex(parameters[3][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	//checking hex for parameter guide id
	if(strlen(parameters[5])>0 && strlen(parameters[5])<9){
		for(int i = 0; i< strlen(parameters[5]); i++){
			if(!ishex(parameters[5][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	if(strlen(parameters[7])> 140){
		return 3;
	}

	//check duplicates
	if(strcmp(parameters[3], parameters[5]) == 0 || strcmp(parameters[3], parameters[7]) == 0 ){
		return 3; 
	}
	if(strcmp(parameters[5], parameters[7]) == 0 ){
		return 3; 
	}



	return 0;
}

static int teamRecord(char* parameters[], int total){
	if(total != 10){
		return 5;
	}

	char* opcode=  parameters[0];
	if(strcmp(opcode, "opCode")!=0){
		return 4; 
	}
	char* gameid = parameters[2];
	if(strcmp(gameid, "gameId")!=0){
		return 4; 
	}
	char* team = parameters[4]; 
	if(strcmp(team, "team")!=0){
		return 4; 
	}
	char* numClaimed = parameters[6];
	if(strcmp(numClaimed, "numClaimed")!=0){
		return 4; 
	}
	char* numPlayers = parameters[8]; 
	if(strcmp(numPlayers, "numPlayers")!=0){
		return 4; 
	}

	//game ID
	if(strlen(parameters[3])>0 && strlen(parameters[3])<9){
		for(int i = 0; i< strlen(parameters[3]); i++){
			if(!ishex(parameters[3][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}


	//team
	if((int)strlen(parameters[5]) < 0 || (int)strlen(parameters[5]) > 11){
		return 3; 
	}

	//num claimed and num players 
	if(string_to_int(parameters[7]) < 0 || string_to_int(parameters[9]) <= 0){
		return 3; 
	}

	return 0;
}

static int gaStatus(char* parameters[], int total){
	if(total != 12){
		return 5;
	}

	char* opcode=  parameters[0];
	if(strcmp(opcode, "opCode")!=0){
		return 4; 
	}
	char* gameid = parameters[2];
	if(strcmp(gameid, "gameId")!=0){
		return 4; 
	}
	char* guideid = parameters[4]; 
	if(strcmp(guideid, "guideId")!=0){
		return 4; 
	}
	char* team = parameters[6];
	if(strcmp(team, "team")!=0){
		return 4; 
	}
	char* player = parameters[8]; 
	if(strcmp(player, "player")!=0){
		return 4; 
	}
	// printf("Checking names\n");

	int statusreq = string_to_int(parameters[10]);
	if(statusreq==1){
		return 4; 
	}	

	// printf("Checking parament 3\n");
	if(strlen(parameters[3])>0 && strlen(parameters[3])<9){
		for(int i = 0; i< strlen(parameters[3]); i++){
			if(!ishex(parameters[3][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}
	
	// printf("checking parameter 5\n");
	if(strlen(parameters[5])>0 && strlen(parameters[5])<9){
		for(int i = 0; i< strlen(parameters[5]); i++){
			if(!ishex(parameters[5][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	// printf("checking other parameter lengths\n");
	if((int)strlen(parameters[7]) < 0 || (int)strlen(parameters[7]) > 11){
		return 3; 
	}

	if((int)strlen(parameters[9]) < 0 || (int)strlen(parameters[9]) > 11){
		return 3; 
	}
	if( !(string_to_int(parameters[11]) == 0 || string_to_int(parameters[11]) == 1) ){
		return 3;
	}

	if(strcmp(parameters[3], parameters[5]) == 0 || strcmp(parameters[3], parameters[7]) == 0 || strcmp(parameters[3], parameters[9]) == 0 || strcmp(parameters[3], parameters[11]) == 0){
		return 3; 
	}
	if(strcmp(parameters[5], parameters[7]) == 0 || strcmp(parameters[5], parameters[9]) == 0 || strcmp(parameters[5], parameters[11]) == 0){
		return 3; 
	}
	if(strcmp(parameters[7], parameters[9]) == 0 || strcmp(parameters[7], parameters[11]) == 0) {
		return 3; 
	}
	if(strcmp(parameters[9], parameters[11]) == 0){
		return 3;
	}

	return 0;
}

int print_log(char* message, char* filename, char* IPport, char* tofrom){
	char timestamp[27];
  time_t clk = time(NULL);
 	snprintf(timestamp, sizeof(timestamp), "(%s", ctime(&clk));
 	timestamp[25] = ')';

	char* totalfilename = malloc(strlen("../logs/") + strlen(filename) + 1); 
	strcpy(totalfilename, "../logs/"); 
	strcat(totalfilename, filename);

	
	FILE *file = fopen(totalfilename, "a"); 

	if(file == NULL){
        free(totalfilename);
		return 1;
	}

	fprintf(file, "%s %s %s: %s\n", timestamp, tofrom, IPport, message); 

	free(totalfilename);
	fclose(file);

	return 0;
}

#endif


// FA required

static int gameStatus(char* parameters[], int total){
	if(total != 10){
		return 5;
	}

	char* opcode=  parameters[0];
	if(strcmp(opcode, "opCode")!=0){
		return 4; 
	}
	char* gameid = parameters[2];
	if(strcmp(gameid, "gameId")!=0){
		return 4; 
	}
	char* guideid = parameters[4]; 
	if(strcmp(guideid, "guideId")!=0){
		return 4; 
	}
	int numclaimed = string_to_int(parameters[7]); 
	int tot = string_to_int(parameters[9]); 


	if(strlen(parameters[3])>0 && strlen(parameters[3])<9){
		for(int i = 0; i< (int)strlen(parameters[3]); i++){
			if(!ishex(parameters[3][i])){
				return 21; 
			}
		}
	}
	else{
		return 22; 
	}
	
	// printf("checking parameter 5\n");
	if(strlen(parameters[5])>0 && strlen(parameters[5])<9){
		for(int i = 0; i< (int)strlen(parameters[5]); i++){
			if(!ishex(parameters[5][i])){
				return 23; 
		}	}
	}
	else{
		return 2; 
	}
	if(tot <0 || numclaimed > tot || numclaimed<0){
		return 25;
	}

	//check duplicates
	if(strcmp(parameters[3], parameters[5]) == 0 || strcmp(parameters[3], parameters[7]) == 0 || strcmp(parameters[3], parameters[9]) == 0){
		return 3; 
	}
	if(strcmp(parameters[5], parameters[7]) == 0 || strcmp(parameters[5], parameters[9]) == 0){
		return 3; 
	}
	if(strcmp(parameters[7], parameters[9]) == 0){
		return 3; 
	}

	return 0;
}

static int gsResponse(char* parameters[], int total){
	return 100;

	if(total != 8){
		return 5;
	}

	char* opcode=  parameters[0];
	if(strcmp(opcode, "opCode")!=0){
		return 4; 
	}
	char* gameid = parameters[2];
	if(strcmp(gameid, "gameId")!=0){
		return 4; 
	}
	char* respcode = parameters[4];
	if(strcmp(respcode, "respCode")!=0){
		return 4; 
	}
	char* text = parameters[6];
	if(strcmp(text, "text")!=0){
		return 4; 
	}

	//game ID
	if(strlen(parameters[3])>0 && strlen(parameters[3])<9){
		for(int i = 0; i < (int)strlen(parameters[3]); i++){
			if(!ishex(parameters[3][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	if(strlen(parameters[7])>140){
		return 3; 
	}

	char* array[11]; 

	array[0] = "SH_ERROR_INVALID_MESSAGE";
	array[1] = "SH_ERROR_INVALID_OPCODE";
	array[2] = "SH_ERROR_INVALID_FIELD";
	array[3] = "SH_ERROR_DUPLICATE_FIELD"; 
	array[4] = "SH_ERROR_INVALID_GAME_ID"; 
	array[5] = "SH_ERROR_INVALID_TEAMNAME"; 
	array[6] = "SH_ERROR_INVALID_PLAYERNAME";
	array[7] = "SH_ERROR_DUPLICATE_PLAYERNAME";
	array[8] = "SH_ERROR_INVALID_ID"; 
	array[9] = "SH_CLAIMED"; 
	array[10] = "SH_CLAIMED_ALREADY"; 


	for(int i = 0; i< 11; i++){
		// printf("|%s| , |%s|\n", parameters[5], array[i]);
		if(strcmp(parameters[5], array[i])== 0){
			return 0;
		}
	}
	return 1; 
}

static int gaHint(char* parameters[], int total){
	if(total != 14){
		return 5;
	}

	char* opcode=  parameters[0];
	if(strcmp(opcode, "opCode")!=0){
		return 4; 
	}
	char* gameid = parameters[2];
	if(strcmp(gameid, "gameId")!=0){
		return 4; 
	}
	char* guideid = parameters[4]; 
	if(strcmp(guideid, "guideId")!=0){
		return 4; 
	}
	char* team = parameters[6];
	if(strcmp(team, "team")!=0){
		return 4; 
	}
	char* player = parameters[8]; 
	if(strcmp(player, "player")!=0){
		return 4; 
	}
	char* pebbleID = parameters[10]; 
	if(strcmp(pebbleID, "pebbleId")!=0){
		return 4; 
	}
	char* hint = parameters[12]; 
	if(strcmp(hint, "hint")!=0){
		return 4; 
	}

	//gmae id 
	if((int)strlen(parameters[3])>0 && (int)strlen(parameters[3])<9){
		for(int i = 0; i < (int)strlen(parameters[3]); i++){
			if(!ishex(parameters[3][i])){
				return 3; 
			}
		}
	}
	else{

		return 3; 
	}
	
	//guide id 
	if((int)strlen(parameters[5])>0 && (int)strlen(parameters[5])<9){
		for(int i = 0; i < (int)strlen(parameters[5]); i++){
			if(!ishex(parameters[5][i])){

				return 3; 
			}
		}
	}
	else{

		return 3; 
	}

	//team
	if((int)strlen(parameters[7]) < 0 || (int)strlen(parameters[7]) > 11){
		return 3; 
	}

	//player 
	if((int)strlen(parameters[9]) < 0 || (int)strlen(parameters[9]) > 11){
		return 3; 
	}

	//pebbleId
	if(strlen(parameters[11])==1){
		if(strcmp(parameters[11], "*") == 0){
			return 3; 
		}
	}
	else{
		if((int)strlen(parameters[11])>0 && (int)strlen(parameters[11])<9){
			for(int i = 0; i < (int)strlen(parameters[11]); i++){
				if(!ishex(parameters[11][i])){
					return 3; 
				}
			}
		}
		else{

			return 3; 
		}
	}

	if(strlen(parameters[13])>140){
		// printf("return 1\n");
		return 3; 
	}

	//comparing for the same answers
	if(strcmp(parameters[3], parameters[5]) == 0 || strcmp(parameters[3], parameters[7]) == 0 || strcmp(parameters[3], parameters[9]) == 0 || strcmp(parameters[3], parameters[11]) == 0 || strcmp(parameters[3], parameters[13]) == 0 ){
		return 3; 
	}
	if(strcmp(parameters[5], parameters[7]) == 0 || strcmp(parameters[5], parameters[9]) == 0 || strcmp(parameters[5], parameters[11]) == 0 || strcmp(parameters[5], parameters[13]) == 0 ){
		return 3; 
	}
	if(strcmp(parameters[7], parameters[9]) == 0 || strcmp(parameters[7], parameters[11]) == 0 || strcmp(parameters[7], parameters[13]) == 0 ){
		return 3; 
	}
	if( strcmp(parameters[9], parameters[11]) == 0 || strcmp(parameters[9], parameters[13]) == 0 ){
		return 3; 
	}
	if(strcmp(parameters[11], parameters[13]) == 0 ){
		return 3;
	}

	return 0;
}

static int faLocation(char* parameters[], int total){
	if(total != 16){
		return 5;
	}

	char* opcode=  parameters[0];
	if(strcmp(opcode, "opCode")!=0){
		return 4; 
	}
	char* gameid = parameters[2];
	if(strcmp(gameid, "gameId")!=0){
		return 4; 
	}

	char* pebbleID = parameters[4];
	if(strcmp(pebbleID, "pebbleId")!=0){
		return 4; 
	}

	char* team = parameters[6];
	if(strcmp(team, "team")!=0){
		return 4; 
	}
	char* player = parameters[8]; 
	if(strcmp(player, "player")!=0){
		return 4; 
	}

	char* lat = parameters[10];
	if(strcmp(lat, "latitude") !=0){
		return 4;
	}

	char* lon = parameters[12]; 
	if(strcmp(lon, "longitude")!=0){
		return 4;
	}
	char* statusReq = parameters[14]; 
	if(strcmp(statusReq, "statusReq")!=0){
		return 4;
	}

	if(strlen(parameters[3])>0 && strlen(parameters[3])<9){
		for(int i = 0; i < (int)strlen(parameters[3]); i++){
			if(!ishex(parameters[3][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	//checking hex for parameter 5 pebble id
	if(strlen(parameters[5])>0 && strlen(parameters[5])<9){
		for(int i = 0; i < (int)strlen(parameters[5]); i++){
			if(!ishex(parameters[5][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	//team
	if((int)strlen(parameters[7]) < 0 || (int)strlen(parameters[7]) > 11){
		return 3; 
	}

	//player
	if((int)strlen(parameters[9]) < 0 || (int)strlen(parameters[9]) > 11){
		return 3; 
	}

	//lat 
	int latitude = string_to_double(parameters[11]); 
	if(latitude< -90 || latitude > 90){
		return 3; 
	}

	int longitude = string_to_double(parameters[13]); 
	if(longitude< -180 || longitude > 180){
		return 3; 
	}
	//status req
	int sr = string_to_int(parameters[15]); 
	if(!(sr ==1 || sr == 0 )){
		return 3;
	}

	//comparing for the same answers
	if(strcmp(parameters[3], parameters[5]) == 0 || strcmp(parameters[3], parameters[7]) == 0 || strcmp(parameters[3], parameters[9]) == 0 || strcmp(parameters[3], parameters[11]) == 0 || strcmp(parameters[3], parameters[13]) == 0 || strcmp(parameters[3], parameters[15]) == 0){
		return 3; 
	}
	if(strcmp(parameters[5], parameters[7]) == 0 || strcmp(parameters[5], parameters[9]) == 0 || strcmp(parameters[5], parameters[11]) == 0 || strcmp(parameters[5], parameters[13]) == 0 || strcmp(parameters[5], parameters[15]) == 0){
		return 3; 
	}
	if(strcmp(parameters[7], parameters[9]) == 0 || strcmp(parameters[7], parameters[11]) == 0 || strcmp(parameters[7], parameters[13]) == 0 || strcmp(parameters[7], parameters[15]) == 0){
		return 3; 
	}
	if( strcmp(parameters[9], parameters[11]) == 0 || strcmp(parameters[9], parameters[13]) == 0 || strcmp(parameters[9], parameters[15]) == 0){
		return 3; 
	}
	if(strcmp(parameters[11], parameters[13]) == 0 || strcmp(parameters[11], parameters[15]) == 0){
		return 3;
	}
	if(strcmp(parameters[13], parameters[15]) == 0){
		return 3;
	}


	return 0;
}

static int faClaim(char* parameters[], int total){
	if(total != 16){
		return 5;
	}

	char* opcode=  parameters[0];
	if(strcmp(opcode, "opCode")!=0){
		return 4; 
	}
	char* gameid = parameters[2];
	if(strcmp(gameid, "gameId")!=0){
		return 4; 
	}

	char* pebbleID = parameters[4];
	if(strcmp(pebbleID, "pebbleId")!=0){
		return 4; 
	}

	char* team = parameters[6];
	if(strcmp(team, "team")!=0){
		return 4; 
	}
	char* player = parameters[8]; 
	if(strcmp(player, "player")!=0){
		return 4; 
	}

	char* lat = parameters[10];
	if(strcmp(lat, "latitude") !=0){
		return 4;
	}

	char* lon = parameters[12]; 
	if(strcmp(lon, "longitude")!=0){
		return 4;
	}
	char* kragId = parameters[14]; 
	if(strcmp(kragId, "kragId")!=0){
		return 4;
	}

	//game ID 
	if(strlen(parameters[3])>0 && strlen(parameters[3])<9){
		for(int i = 0; i < (int)strlen(parameters[3]); i++){
			if(!ishex(parameters[3][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	//checking hex for parameter 5 pebble id
	if(strlen(parameters[5])>0 && strlen(parameters[5])<9){
		for(int i = 0; i < (int)strlen(parameters[5]); i++){
			if(!ishex(parameters[5][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	//team
	if((int)strlen(parameters[7]) < 0 || (int)strlen(parameters[7]) > 11){
		return 3; 
	}

	//player
	if((int)strlen(parameters[9]) < 0 || (int)strlen(parameters[9]) > 11){
		return 3; 
	}

	//lat 
	float latitude = string_to_double(parameters[11]); 
	if(latitude< -90 || latitude > 90){
		return 3; 
	}

	float longitude = string_to_double(parameters[13]); 
	if(longitude< -180 || longitude > 180){
		return 3; 
	}

	//kraig ID
	if(strlen(parameters[15])>0 && strlen(parameters[15])<5){
		for(int i = 0; i < (int)strlen(parameters[15]); i++){
			if(!ishex(parameters[15][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	//comparing for the same answers
	if(strcmp(parameters[3], parameters[5]) == 0 || strcmp(parameters[3], parameters[7]) == 0 || strcmp(parameters[3], parameters[9]) == 0 || strcmp(parameters[3], parameters[11]) == 0 || strcmp(parameters[3], parameters[13]) == 0 || strcmp(parameters[3], parameters[15]) == 0){
		return 3; 
	}
	if(strcmp(parameters[5], parameters[7]) == 0 || strcmp(parameters[5], parameters[9]) == 0 || strcmp(parameters[5], parameters[11]) == 0 || strcmp(parameters[5], parameters[13]) == 0 || strcmp(parameters[5], parameters[15]) == 0){
		return 3; 
	}
	if(strcmp(parameters[7], parameters[9]) == 0 || strcmp(parameters[7], parameters[11]) == 0 || strcmp(parameters[7], parameters[13]) == 0 || strcmp(parameters[7], parameters[15]) == 0){
		return 3; 
	}
	if( strcmp(parameters[9], parameters[11]) == 0 || strcmp(parameters[9], parameters[13]) == 0 || strcmp(parameters[9], parameters[15]) == 0){
		return 3; 
	}
	if(strcmp(parameters[11], parameters[13]) == 0 || strcmp(parameters[11], parameters[15]) == 0){
		return 3;
	}
	if(strcmp(parameters[13], parameters[15]) == 0){
		return 3;
	}	


	return 0;
}

static int faLog(char* parameters[], int total){
	if(total != 6){
		return 5;
	}

	char* opcode=  parameters[0];
	if(strcmp(opcode, "opCode")!=0){
		return 4; 
	}
	char* pebbleId = parameters[2];
	if(strcmp(pebbleId, "pebbleId")!=0){
		return 4; 
	}
	char* text = parameters[4]; 
	if(strcmp(text, "text")!=0){
		return 4; 
	}

	//hexID
	if(strlen(parameters[3])>0 && strlen(parameters[3])<9){
		for(int i = 0; i < (int)strlen(parameters[3]); i++){

			if(!ishex(parameters[3][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	if(strlen(parameters[5])>140){

		return 3;
	}

	if(strcmp(parameters[3], parameters[5])==0){

		return 3;
	}

	return 0;
}

static int gameOver(char* parameters[], int total){
	if(total != 6){
		return 5;
	}

	char* opcode=  parameters[0];
	if(strcmp(opcode, "opCode")!=0){
		return 4; 
	}
	char* gameid = parameters[2];
	if(strcmp(gameid, "gameId")!=0){
		return 4; 
	}
	char* secret = parameters[4]; 
	if(strcmp(secret, "secret")!=0){
		return 4; 
	}

	//game ID
	if(strlen(parameters[3])>0 && strlen(parameters[3])<9){
		for(int i = 0; i < (int)strlen(parameters[3]); i++){
			if(!ishex(parameters[3][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	if(strlen(parameters[5])> 140){
		return 3;
	}
	return 0;
}


#define UC(c)	((unsigned char)c)

char ishex (unsigned char c)
{
  if ( ( c >= UC('0') && c <= UC('9')) ||
	   ( c >= UC('a') && c <= UC('f')) ||
	   ( c >= UC('A') && c <= UC('F')) )
	  return 1;
  return 0;
}

// https://crackprogramming.blogspot.com/2012/10/implement-atof.html
double string_to_double(char* num)
{
	if (!num || !*num)
		return 0; 
	double integerPart = 0;
	double fractionPart = 0;
	int divisorForFraction = 1;
	int sign = 1;
	bool inFraction = false;
	/*Take care of +/- sign*/
	if (*num == '-') {
		++num;
		sign = -1;
	} else if (*num == '+') {
	  ++num;
	}
	
	while (*num != '\0') {
		if (*num >= '0' && *num <= '9') {
			if (inFraction) {
				/*See how are we converting a character to integer*/
				fractionPart = fractionPart*10 + (*num - '0');
				divisorForFraction *= 10;
			} else {
			   integerPart = integerPart*10 + (*num - '0');
			}
		} else if (*num == '.') {
			if (inFraction)
				return sign * (integerPart + fractionPart/divisorForFraction);
			else
				inFraction = true;
		} else {
			return sign * (integerPart + fractionPart/divisorForFraction);
		}
		++num;
	}

	return sign * (integerPart + fractionPart/divisorForFraction);
}

// http://www.geeksforgeeks.org/write-your-own-atoi/
int string_to_int(char *str)
{
  int res = 0;  // Initialize result
  int sign = 1;  // Initialize sign as positive
  int i = 0;  // Initialize index of first digit
    
  // If number is negative, then update sign
  if (str[0] == '-')
  {
    sign = -1;  
    i++;  // Also update index of first digit
  }
    
  // Iterate through all digits and update the result
  for (; str[i] != '\0'; ++i)
    res = res*10 + str[i] - '0';
  
  // Return result with sign
  return sign*res;
}
