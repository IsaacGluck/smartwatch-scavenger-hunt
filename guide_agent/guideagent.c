//include statements
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bag.h"
#include "hashtable.h"
#include "../common/shared.h"

//opCode=GAME_STATUS|gameId=FEED|guideId=1d2f3e4a|numClaimed=6|numKrags=8
//opCode=GS_SECRET|gameId=FEED|guideId=1d2f3e4a|secret=com__dafd_ _cie____50
//opCode=GS_AGENT|gameId=FEED|pebbleId=8080477D|team=fab4|player=Morgan|latitude=93.706552|longitude=-102.287418|lastContact=28
//opCode=GS_CLUE|gameId=FEED|guideId=1d2f3e4a|kragId=4321|clue=new clue
//opCode=GS_CLAIMED|gameId=FEED|guideId=1d2f3e4a|pebbleId=8080477D|kragId=4321|latitude=43.706552|longitude=-72.287418
//opCode=TEAM_RECORD|gameId=FEED|team=ali|numPlayers=5|numClaimed=12
//opCode=GAME_OVER|gameId=FEED|secret=computer science 50 rocks!




struct gameStruct {
  char* gameID;	     
  char* guideID;
  char* teamName; 
  int totalKrags; 
  char* secret; 
  int claimedKrags; 
  int update; 
  int firstGameStatus; //0 if it is the first, one forever after 
  char* playerName;
  hashtable_t* agents;
  hashtable_t* krags; 
  bag_t* hints; 
  bag_t* gameOverInfo; 	      
};

struct agent {
	char* team;
	char* name;
	char* pebbleID;
	float lat; 
	float lon;
	int lastContact; 
};

struct krag {
	char* kragID; 
	char* clue;
	char* idOfClaimer;
	float lat;
	float lon; 
};

struct gameOver {
	char* gameID;
	char* team;
	int numPlayers;
	int numClaimed; 

};




//Other methods 
static void gameStatus(char* parameters[], void* g);
static void gsAgent(char* parameters[], void* g);
static void gsClue(char* parameters[], void* g);
static void gsClaimed(char* parameters[], void* g);
static void gsSecret(char* parameters[], void* g);
static void gsResponse(char* parameters[], void* g);
static void gameOver(char* parameters[], void* g);
static void teamRecord(char* parameters[], void* g);

static void checkArgs(const int argc, char *argv[], char* variables[]); 
static void createGameStruct(void* g, char* variables[]);
static int stringHexToDec(char* hex);
static void dealWithInfo(void* g, char* m);
static char* GA_STATUSReturn(void* g);
static char* GA_HINTReturn(void* g);
static void gameStructPrint(void* g); 
static void agentPrint(FILE *fp, const char *key, void *item);
static void kragPrint(FILE *fp, const char *key, void *item);
static void endingPrint(FILE *fp, void *item);
static void hintPrint(FILE *fp, void *item);

static const struct {
  const char *opCodes;
  void (*func)(char* parameters[], void* g);
} codes[] = {
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


int main(const int argc, char *argv[]){	
	char* variables[argc-1];
	checkArgs(argc, argv, variables); 



	struct gameStruct *game = malloc(sizeof(struct gameStruct));

	createGameStruct(game, variables);
	gameStructPrint(game);
	printf("%s", GA_STATUSReturn(game));

	char* l = readlinep(stdin); 
	while(l!=NULL){

	/*printf("message: %s\n", l);
	if(strcmp(l, "hint")==0){
		printf("going to hunt mesage rn\n");
		printf("HINT: %s\n", GA_HINTReturn(game));
		printf("Old hints: \n");
		bag_print(game->hints, stdout, hintPrint);
	}
	else{
		dealWithInfo(game, l);
	}

	printf("GOT HERE! %s", game->gameID);*/
	printf("I Have message: %s\n", l);
	printf("Nuber : %d\n", validate_message(l));

	free(l); 
	l = readlinep(stdin); //get next line 
	
	}


	exit(0);
}

static char* GA_HINTReturn(void* g){
	//opCode=GA_HINT|gameId=|guideId=|team=|player=|pebbleId=|hint=
	struct gameStruct* game = g; 
	printf("Enter who you want to send a hint to\n"); 
	char* personToSendHint = readlinep(stdin); 
	hashtable_t * agents = game->agents; 
	if(hashtable_find(agents, personToSendHint)!= NULL){
		printf("Enter the hint you want to send %s \n", personToSendHint); 
		char* hint = readlinep(stdin); 
		char* returnstr = ""; 
		if(hint!=NULL){
			struct agent* curragent = hashtable_find(agents, personToSendHint); 
			char* begin = "opCode=GA_HINT|gameId=|guideId=|team=|player=|pebbleId=|hint="; 
			int math = strlen(begin) + strlen(game->gameID) + strlen(game->guideID) + strlen(game->teamName) + strlen(curragent->name) + strlen(curragent->pebbleID) +strlen(hint) +1; 
			returnstr = malloc(math); 
			strcat(returnstr, "opCode=GA_HINT|gameId="); 
			strcpy(returnstr, game->gameID); 
			strcpy(returnstr, "|guideId="); 
			strcpy(returnstr, game->guideID); 	
			strcpy(returnstr, "|team="); 
			strcpy(returnstr, game->teamName);
			strcpy(returnstr, "|player="); 
			strcpy(returnstr, curragent->name);  
			strcpy(returnstr, "|pebbleId="); 
			strcpy(returnstr, curragent->pebbleID);
			strcpy(returnstr, "|hint="); 
			strcpy(returnstr, hint);  
			bag_t* hints = game->hints; 
			bag_insert(hints, returnstr);  

		}
		return returnstr; 

	}
	else{
		printf("%s is not an agent and does not exist\n", personToSendHint); 
		return NULL;
	}
	

}

static char* GA_STATUSReturn(void* g){
	//opCode=GA_STATUS|gameId=|guideId=|team=|player=|statusReq=
	struct gameStruct* game = g; 
	printf("alive in here to start\n");
	char* begin = "opCode=GA_STATUS|gameId=|guideId=|team=|player=|statusReq="; 
	char* gID = game->gameID; 
	int try = strlen(gID);
	printf("try: %d\n", try);
	int math = strlen(begin) + strlen(game->gameID) + strlen(game->guideID) + strlen(game->teamName) + strlen(game->playerName) + (game->update) +1 ; 
	printf("math: %d\n", math);
	char* returnstr = malloc(math);
	strcpy(returnstr, "opCode=GA_STATUS|gameId=");
	strcat(returnstr, game->gameID);
	strcat(returnstr, "|guideId=");
	strcat(returnstr, game->guideID);
	strcat(returnstr, "|team=");
	strcat(returnstr, game->teamName);
	strcat(returnstr, "|player=");
	strcat(returnstr, game->playerName);
	strcat(returnstr, "|statusReq=");

	char str[10]; 
	sprintf(str,"%d", game->update);

	strcat(returnstr, str);

	return returnstr;
}

static void dealWithInfo(void* g, char* m){

	//get totall amount of things needed for array 
	struct gameStruct* game = g; 
	char* message = malloc(strlen(m));
	strcpy(message, m);


	int total = 0; 
	for(int i = 0; i< strlen(message); i++){
		if(message[i] == '|'){
			total++; 
		}
	}

	printf("%d\n", total);
	//create the array 
	// char** array[total+1];
	char** array = malloc(sizeof(char*) * (total + 1));
	char* pointer = message; // pointer to beginning of string


	array[0] = pointer;
	printf("%s\n", array[0]);
	int count = 1;
	int message_length = strlen(message);
	

	for(int i = 0; i < message_length; i++){
		if(pointer[i] == '|'){	
			pointer[i] = '\0';
			array[count] = &pointer[i + 1];
			printf("%s\n", &pointer[i]);
			count++;  
		}
	} 

	char* array2[total+1]; 
	for(int i = 0; i<=total; i++){
		char* wequal = strchr(array[i], '='); 

		char* finalstring = &wequal[1];
		array2[i] = finalstring;

	}


	for(int i = 0; i <= total; i++){
		printf("array: %s\n", array[i]);
	}


	int fn;
	for (fn = 0; codes[fn].opCodes != NULL; fn++) {
  		if (strcmp(array2[0], codes[fn].opCodes) == 0) {
  			(*codes[fn].func)(array2, g);
  			break;
  		}
	}
	

 	

	if (codes[fn].opCodes == NULL){
  		printf("Unknown command: '%s'\n", array2[0]);
 	 }



}


static void createGameStruct(void* g, char* variables[]){
	struct gameStruct* game = g; 
	char* guideId = variables[0];
	printf("GUIDE ID? %s\n", variables[0]);
	char* team = variables[1];
	char* player = variables[2];


	game->gameID = "0";
	game->firstGameStatus = 0; 
	game->guideID = guideId;
	game->playerName = player; 
	game->teamName = team; 
	game->totalKrags = 0;
	game->secret = "";
	game->claimedKrags = 0; 
	game->update = 1; 
	hashtable_t* agents = hashtable_new(5);
	game->agents = agents;
	hashtable_t* krags = hashtable_new(10);
	game->krags = krags; 
	bag_t* hints = bag_new(); 
	game->hints = hints;
	bag_t* end = bag_new(); 
	game->gameOverInfo = end; 

}

static void checkArgs(const int argc, char *argv[], char* variables[]){
	//Check to make sure right amount of arguments 
	if(argc != 6){
		exit(1); 
	}

	char* strHexGameID; 
	int id = 0;
	char* team;
	int t = 0;
	char* player;
	int pl = 0;
	char* host;
	int h = 0;
	char* port; 
	int po = 0; 

	//Loop through and determine all the arguments. 
	for(int i = 1; i< argc; i++){
		const char ch = '=';
   		char *ret;
   		ret = strchr(argv[i], ch);


   		//no equals sign, null
   		if(ret == NULL){
   			exit(6);
   		}

   		char next = ret[1]; 
   		char* afterEquals;
   		afterEquals = strchr(ret, next); 


   		int totsize = strlen(argv[i]); 
   		int equalssize = strlen(ret); 
   		int beforeEqualSize = totsize - equalssize; 

   		char beforeEquals[beforeEqualSize +1]; 
   		for(int j = 0; j< beforeEqualSize; j++){
   			beforeEquals[j] = argv[i][j]; 
   		}
   		beforeEquals[beforeEqualSize] = '\0'; 

   		//printf("before: %s after: %s \n", beforeEquals, afterEquals);

   		if(strcmp(beforeEquals, "guideId") == 0){
   			if(id == 0){
   				id = 1;
   				strHexGameID = afterEquals;
   			}
   			else{
   				printf("already have guideID");
   				exit(7);
   			}
   		}
   		if(strcmp(beforeEquals, "team") == 0){
   			if(t == 0){
   				t = 1;
   				team = afterEquals;
   			}
   			else{
   				printf("already have team");
   				exit(7);
   			}
   		}

   		if(strcmp(beforeEquals, "player") == 0){
   			if(pl == 0){
   				pl = 1;
   				player = afterEquals;
   			}
   			else{
   				printf("already have player");
   				exit(7);
   			}
   		}

   		if(strcmp(beforeEquals, "host") == 0){
   			if(h == 0){
   				h = 1;
   				host = afterEquals;
   			}
   			else{
   				printf("already have host");
   				exit(7);
   			}
   		}

   		if(strcmp(beforeEquals, "port") == 0){
   			if(po == 0){
   				po = 1;
   				port = afterEquals;
   			}
   			else{
   				printf("already have port");
   				exit(7);
   			}
		}


	}

	printf("Guide ID: %s, Team: %s, Player: %s, Host: %s, Port: %s\n", strHexGameID, team, player, host, port);


	//check to amke sure hex string is in the right bounds 
	if (strlen(strHexGameID) < 1 || strlen(strHexGameID) > 8){
		printf("not right length hex string ");
		exit(2); 
	}

	//Make sure the str ID has no spaces in it 
	for(int i =0; strHexGameID[i] != '\0'; i++){
		int curr = strHexGameID[i]; 
		if (isspace(curr)){ //if soemthing is a space 
			printf("no spaces allowed in hex game id" ); 
			exit(2);
		}
	} 


	//Make sure the team has no spaces in it 
	for(int i =0; team[i] != '\0'; i++){
		int curr = team[i]; 
		if (isspace(curr)){ //if soemthing is a space 
			printf("no spaces allowed in team id" ); 
			exit(3);
		}
	}

	//check to make srue team is the right size 
	if (strlen(team) > 10 || strlen(team) < 1) {
		printf("not the right size ");
		exit(3); 
	} 



		for(int i =0; player[i] != '\0'; i++){
		int curr = player[i]; 
		if (isspace(curr)){ //if soemthing is a space 
			printf("no spaces allowed in player id" ); 
			exit(4);
		}
	}

	//check to make srue team is the right size 
	if (strlen(player) > 10 || strlen(player) < 1) {
		printf("not the right size ");
		exit(4); 
	} 

	variables[0] = strHexGameID;
	variables[1] = team;
	variables[2] = player;
	variables[3] = host;
	variables[4] = port;
 

}

static int stringHexToDec(char* hex){
	int decimalNumber;
	sscanf(hex,"%x", &decimalNumber);
	return decimalNumber;
}


static void gameStatus(char* parameters[], void* g){
	
	//opCode=GAME_STATUS|gameId=|guideId=|numClaimed=|numKrags=
	struct gameStruct* game = g; 

	//printf("%s %s \n", game->guideID, parameters[2]);
	if(game->firstGameStatus==0 && strcmp(parameters[2], game->guideID)== 0){
		game->gameID = parameters[1]; 
		game->totalKrags = atoi(parameters[4]);
		game->claimedKrags = atoi(parameters[3]);
		game->firstGameStatus = 1; 
	}
	else if (strcmp(game->gameID, parameters[1])==0 && strcmp(game->guideID, parameters[2])==0 && game->totalKrags == atoi(parameters[4])){
		printf("IN ELSE IF\n");
		game->claimedKrags = atoi(parameters[3]);
	}

	gameStructPrint(g);
	printf("%s\n", game->gameID);
}

static void gsAgent(char* parameters[], void* g){
	printf("In gsagent method\n");
	struct gameStruct* game = g; 

	//opCode=GS_AGENT|gameId=|pebbleId=|team=|player=|latitude=|longitude=|lastContact=
	if(strcmp(game->gameID, parameters[1])==0 && strcmp(game->teamName, parameters[3])==0){
		printf("In mega if statement\n");
		struct agent* curragent;

		if(hashtable_find(game->agents, parameters[4])!=NULL){
			printf("In found if statement\n");
			curragent = hashtable_find(game->agents, parameters[4]); 
			if(strcmp(curragent->pebbleID, parameters[2])==0 ){
				printf("In found if statment w pebble ID matching\n");

				curragent->lat = atof(parameters[5]);
				curragent->lon = atof(parameters[6]);
				curragent->lastContact = atoi(parameters[7]);
			}
		}
		else{
			printf("In else statement\n");

			struct agent *curragent = malloc(sizeof(struct agent));
			curragent->name = parameters[4];
			curragent->pebbleID = parameters[2];
			curragent->team = parameters[3];


			curragent->lat = atof(parameters[5]);
			curragent->lon = atof(parameters[6]);
			curragent->lastContact = atoi(parameters[7]);
			printf("print %s \n", parameters[4]);
			hashtable_insert(game->agents, parameters[4], curragent);


		}
	}

	gameStructPrint(game);


}
static void gsClue(char* parameters[], void* g){
	//opCode=GS_CLUE|gameId=FEED|guideId=0707|kragId=07E1|clue=A stone building for religious services, under the third archway.

	printf("In gsclue method\n");
	struct gameStruct* game = g; 
	hashtable_t* krags= game->krags; 
	if(strcmp(game->gameID, parameters[1])==0 && strcmp(game->guideID, parameters[2])==0){
		if(hashtable_find(krags, parameters[3])== NULL){
			struct krag *currkrag = malloc(sizeof(struct krag));
			currkrag->kragID = parameters[3];
			currkrag->clue= parameters[4];
			currkrag->idOfClaimer = "";
			currkrag->lat = 0;
			currkrag->lon = 0;
			hashtable_insert(game->krags, parameters[3], currkrag);
		}
	}

	gameStructPrint(game);

}
static void gsClaimed(char* parameters[], void* g){
	//opCode=GS_CLAIMED|gameId=|guideId=|pebbleId=|kragId=|latitude=|longitude=

	printf("In gsclaimed method\n");
	struct gameStruct* game = g; 
	hashtable_t* krags= game->krags; 
	if(strcmp(game->gameID, parameters[1])==0 && strcmp(game->guideID, parameters[2])==0){
		if(hashtable_find(krags, parameters[4])!=NULL){
			struct krag* currkrag = hashtable_find(krags, parameters[4]); 
			currkrag->idOfClaimer = parameters[3];
			currkrag->lat = atof(parameters[5]);
			currkrag->lon = atof(parameters[6]);
		}

	}

	gameStructPrint(game);


}
static void gsSecret(char* parameters[], void* g){
	printf("In secret method\n");
	struct gameStruct* game = g; 

	//opCode=GS_SECRET|gameId=FEED|guideId=0707|secret=com_____ _cie____50
	printf("|%s| |%s| %d\n", game->gameID, parameters[1], strcmp(game->gameID, parameters[1]));
	if(strcmp(game->gameID, parameters[1])==0 && strcmp(game->guideID, parameters[2])==0){
		game->secret= parameters[3];
	}
	gameStructPrint(g);


}
static void gsResponse(char* parameters[], void* g){
	printf("In respone method\n");


}
static void gameOver(char* parameters[], void* g){
	printf("In gameover method\n");
	//opCode=GAME_OVER|gameId=|secret=
	struct gameStruct* game = g; 

	if(strcmp(game->gameID, parameters[1])==0){
		printf("***GAME OVER***\n");
		printf("The final secret is %s", parameters[2]);
		struct gameStruct* game = g; 
		bag_t* ending = game->gameOverInfo; 
		bag_print(ending, stdout, endingPrint);
		exit(0);
	}

}

static void teamRecord(char* parameters[], void* g){
	//opCode=TEAM_RECORD|gameId=|team=|numClaimed=|numPlayers=
	printf("In teamrecord method\n");
	struct gameOver* end = malloc(sizeof(gameOver)); 
	end->gameID= parameters[1];
	end->team = parameters[2];
	end->numClaimed = atoi(parameters[3]);
	end->numPlayers = atoi(parameters[4]); 
	struct gameStruct* game = g; 
	bag_insert(game->gameOverInfo, end); 

}

static void gameStructPrint(void* g){
  struct gameStruct* game = g; 

  printf("***CURRENT GAME STRUCT***\n");
  printf("The gameID is: %s , The guideID is %s , the player name is: %s,  the Team name is %s \n", game->gameID, game->guideID, game->playerName, game->teamName); 
  printf("The total num of krags are %d, the total num collected is %d, and the secret is %s\n", game->totalKrags, game->claimedKrags, game->secret);
  printf("The first game status is: %d, the update status is: %d\n", game->firstGameStatus, game->update);
  hashtable_print(game->agents, stdout, agentPrint);
  printf("\n");
  hashtable_print(game->krags, stdout, kragPrint);

}

static void agentPrint(FILE *fp, const char *key, void *item){
	struct agent* curragent = item;
	printf("team: %s, name: %s, pebbleID: %s\n", curragent->team, curragent->name, curragent->pebbleID);
	printf("lat: %f , long: %f, lastContact: %d", curragent->lat, curragent->lon, curragent->lastContact);

}

static void kragPrint(FILE *fp, const char *key, void *item){
	struct krag* currkrag = item;
	printf("krag ID: %s, Clue: %s, pebbleID: %s\n", currkrag->kragID, currkrag->clue, currkrag->idOfClaimer);
	printf("lat: %f , long: %f", currkrag->lat, currkrag->lon);

}

static void endingPrint(FILE *fp, void *item){
	struct gameOver* end = item;
	printf("gameID: %s, team: %s, num players: %d, num found: %d\n", end->gameID, end->team, end->numPlayers, end->numClaimed);
}

static void hintPrint(FILE *fp, void *item){
	char* hint = item;
	printf("%s\n", hint);
}
