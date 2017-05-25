//include statements
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bag.h"
#include "hashtable.h"

struct gameStruct {
  char* gameID;	     
  char* guideID;
  char* teamName; 
  int totalKrags; 
  char* secret; 
  int claimedKrags; 
  int update; 
  char* playerName;
  hashtable_t* agents;
  hashtable_t* krags; 
  bag_t* hints; 	      
};

struct agent {
	char* name;
	char* pebbleID;
	int lat; 
	int lon;
	int lastContact; 
};

struct krag {
	char* kragID; 
	int lat;
	int lon; 
};




//Other methods 
static void gameStatus(char* parameters[]);
static void gsAgent(char* parameters[]);
static void gsClue(char* parameters[]);
static void gsClaimed(char* parameters[]);
static void gsSecret(char* parameters[]);
static void gsResponse(char* parameters[]);
static void gameOver(char* parameters[]);
static void teamRecord(char* parameters[]);

static void checkArgs(const int argc, char *argv[], char* variables[]); 
static void createGameStruct(void* g, char* variables[]);
static int stringHexToDec(char* hex);
static void dealWithInfo(void* g, char* message);


static const struct {
  const char *opCodes;
  void (*func)(char* parameters[]);
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

	/*for(int i = 0; i< argc-1; i++){
		printf("%s ", variables[i]);
	}*/ 

	struct gameStruct *game = malloc(sizeof(struct gameStruct));

	createGameStruct(game, variables);
	
	char* l = readlinep(stdin); 
	while(l!=NULL){

	printf("message: %s\n", l);
	dealWithInfo(game, l);

	free(l); 
	l = readlinep(stdin); //get next line 

	}


	exit(0);
}

static void dealWithInfo(void* g, char* message){
	int total = 0; 
	for(int i = 0; i< strlen(message); i++){
		if(message[i] == '|'){
			total++; 
		}
	}
	char* split[total+1];

	char* changingmessage = message; 
	for(int i = 0; i<= total; i++){
		char* afterEqual = strchr(changingmessage, '=');
		printf("Afterequal: %s", afterEqual);
		int lenToEqual = strlen(afterEqual);
		char* toBar = strchr(changingmessage, '|');
		printf("afterbar: %s", tobar);

		if(toBar==NULL){
			char final[strlen(changingmessage)-lenToEqual];
			for(int i = 0; i<strlen(changingmessage)-lenToEqual; i++){
				final[i] = changingmessage[i+1]; 
			}
			split[i] ==  final;
		}
		else{
			char final[strlen(toBar)-lenToEqual];
			int count = 0; 
			for(int i = lenToEqual; i<strlen(toBar); i++){
				final[count] = changingmessage[i];
				count++;
			}
			char* t[strlen(changingmessage)-1]; 
			for(int i = 0; i < strlen(changingmessage)-1 ; i++){
				t[i] = changingmessage[i+1]; 
			}
			changingmessage = t; 
		}


	} 

	for(int i = 0; i <= total; i++){
		printf("%s ", split[i]);
	}

}


static void createGameStruct(void* g, char* variables[]){
	struct gameStruct* game = g; 
	char* guideId = variables[0];
	char* team = variables[1];
	char* player = variables[2];


	game->gameID = 0;
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

	printf("Game ID: %s, Team: %s, Player: %s, Host: %s, Port: %s\n", strHexGameID, team, player, host, port);


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


static void gameStatus(char* parameters[]){

}
static void gsAgent(char* parameters[]){

}
static void gsClue(char* parameters[]){

}
static void gsClaimed(char* parameters[]){

}
static void gsSecret(char* parameters[]){

}
static void gsResponse(char* parameters[]){

}
static void gameOver(char* parameters[]){

}
static void teamRecord(char* parameters[]){

}
