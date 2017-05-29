// Conditional inclusion for platform specific builds
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared.h"
#include <ctype.h>




int validate_message(char* message); 

static int gameStatus(char* parameters[]);
static int gsAgent(char* parameters[]);
static int gsClue(char* parameters[]);
static int gsClaimed(char* parameters[]);
static int gsSecret(char* parameters[]);
static int gsResponse(char* parameters[]);
static int gameOver(char* parameters[]);
static int teamRecord(char* parameters[]);
static int faLocation(char* parameters[]);
static int faClaim(char* parameters[]);
static int faLog(char* parameters[]);
static int gaStatus(char* parameters[]);
static int gaHint(char* parameters[]);

static const struct {
  const char *opCodes;
  int (*func)(char* parameters[]);
} codes[] = {

  { "FA_LOCATION", faLocation },
  { "FA_CLAIM", faClaim },
  { "FA_LOG", faLog },
  { "GA_STATUS", gaStatus },
  { "GA_HINT", gaHint },
  { "GAME_STATUS", gameStatus },
  { "GS_AGENT", gsAgent },
  { "GS_CLUE", gsClue },
  { "GS_CLAIMED", gsClaimed },
  { "GS_SECRET", gsSecret },
  { "GS_RESPONSE", gsResponse },
  { "GAME_OVER", gameOver },
  { "TEAM_RECORD", teamRecord },
  { NULL, NULL }
};

/*const char *version_str = "1.0";

void print_shared() {
  printf("*** shared function! (v%s) ***", version_str);
}*/


int validate_message(char* message){
	printf("IN THIS METHOD MEH\n");
	//check size max is 8191, if not, return -1 
	if(strlen(message)> 8191 && strlen(message)>0){
		return -1;  
	}

	//counts the total num of spaces there should be in the array 
	int total = 0; 
	for(int i = 0; i< strlen(message); i++){
		if(message[i] == '|' || message[i] == '='){
			total++; 
		}
	}
	//not enough arguments in general for this to make sense 
	if (total <=1){
		return 1; 
	}
	//array isnt made correctly 
	char** array = malloc(sizeof(char*) * (total + 1));
	if(array == NULL){
		return 1; 
	}

	char* pointer = message;

	//splits the array 
	int message_length = strlen(message);

	int count = 1; 
	array[0] = pointer;
	for(int i = 0; i < message_length; i++){
		if(pointer[i] == '|' || pointer[i] == '='){	
			pointer[i] = '\0';
			array[count] = &pointer[i + 1];
			printf("%s\n", &pointer[i]);
			count++;  
		}
	} 

	//array wasnt set right 
	for(int i = 0; i <= total; i++){
		if(array[i] == NULL){
			return 1; 
		}
	}


	//Sends command to log file
	int fn;
	for (fn = 0; codes[fn].opCodes != NULL; fn++) {
  		if (strcmp(array[1], codes[fn].opCodes) == 0) {
  			return((*codes[fn].func)(array));
  		}
	}
	

 	

	if (codes[fn].opCodes == NULL){
  		printf("Unknown command: '%s'\n", array[1]);
  		return(1); 
 	 }


	//send to right opcode method 
		//if error about parameters types return 2 
		//if duplicates retrn 3
		//if errors about parameters that there should be return 4 
	return 0;
}


static int gameStatus(char* parameters[]){
	char* opcode=  parameters[0];
	if(strcmp(opcode, "opCode")==1){
		return 4; 
	}
	char* gameid = parameters[2];
	if(strcmp(gameid, "gameId")==1){
		return 4; 
	}
	char* guideid = parameters[4]; 
	if(strcmp(guideid, "guideId")==1){
		return 4; 
	}
	int numclaimed = atoi(parameters[6]); 
	int total = atoi(parameters[8]); 


	if(strlen(parameters[3])>0 && strlen(parameters[3])<9){
		for(int i = 0; i< strlen(parameters[3]); i++){
			if(!isxdigit(parameters[3][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}
	
	printf("checking parameter 5\n");
	if(strlen(parameters[5])>0 && strlen(parameters[5])<9){
		for(int i = 0; i< strlen(parameters[5]); i++){
			if(!isxdigit(parameters[5][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	if(total <0 || numclaimed > total || numclaimed<0){
		return 3;
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
static int gsAgent(char* parameters[]){
	return 0;
}
static int gsClue(char* parameters[]){
	return 0;
}
static int gsClaimed(char* parameters[]){
	return 0;
}
static int gsSecret(char* parameters[]){
	return 0;
}
static int gsResponse(char* parameters[]){
	return 0;
}
static int gameOver(char* parameters[]){
	return 0;
}
static int teamRecord(char* parameters[]){
	return 0;
}
static int faLocation(char* parameters[]){
	return 0;
}
static int faClaim(char* parameters[]){
	return 0;
}
static int faLog(char* parameters[]){
	return 0;
}
static int gaStatus(char* parameters[]){
	char* opcode=  parameters[0];
	if(strcmp(opcode, "opCode")==1){
		return 4; 
	}
	char* gameid = parameters[2];
	if(strcmp(gameid, "gameId")==1){
		return 4; 
	}
	char* guideid = parameters[4]; 
	if(strcmp(guideid, "guideId")==1){
		return 4; 
	}
	char* team = parameters[6];
	if(strcmp(team, "team")==1){
		return 4; 
	}
	char* player = parameters[8]; 
	if(strcmp(player, "player")==1){
		return 4; 
	}
	printf("Checking names\n");

	int statusreq = atoi(parameters[10]);
	if(statusreq==1){
		return 4; 
	}	

	printf("Checking parament 3\n");
	if(strlen(parameters[3])>0 && strlen(parameters[3])<9){
		for(int i = 0; i< strlen(parameters[3]); i++){
			if(!isxdigit(parameters[3][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}
	
	printf("checking parameter 5\n");
	if(strlen(parameters[5])>0 && strlen(parameters[5])<9){
		for(int i = 0; i< strlen(parameters[5]); i++){
			if(!isxdigit(parameters[5][i])){
				return 3; 
			}
		}
	}
	else{
		return 3; 
	}

	printf("checking other parameter lengths\n");
	if(strlen(parameters[7]) < 0 || strlen(parameters[7]) > 11){
		return 3; 
	}

	if(strlen(parameters[9]) < 0 || strlen(parameters[9]) > 11){
		return 3; 
	}
	if( !(atoi(parameters[11]) == 0 || atoi(parameters[11]) == 1) ){
		return 3;
	}
	return 0;
}
static int gaHint(char* parameters[]){
	return 0;
}


