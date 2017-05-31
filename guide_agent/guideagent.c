//include statements
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bag.h"
#include "hashtable.h"
#include "../common/shared.h"
#include "../common/common.h"

#include <stdbool.h>
#include <unistd.h>	      // read, write, close
#include <strings.h>	      // bcopy, bzero
#include <netdb.h>	      // socket-related structures
#include <arpa/inet.h>	      // socket-related calls
#include <sys/select.h>	      // select-related stuff 
#include "file.h"	


//first game struct that stores all the infrmation for the game 
struct gameStruct {
  char* gameID;	     //game ID 
  char* guideID; 	//guide ID 
  char* teamName; 	//team name 
  int totalKrags; 	//# of krags that can be find 
  char* secret; 	//Secret message as of right now 
  int claimedKrags; 	//krags that team has changed 
  int update; //update weather or not an update is needed 
  int firstGameStatus; //0 if it is the first, one forever after 
  char* playerName;		//name of guide agent 
  char* ipaddress; 	//ip adress of the server 
  hashtable_t* agents; //table of agents 
  hashtable_t* krags; 	//table of krags 
  bag_t* hints; 	//bag of all the hints 
  bag_t* gameOverInfo; 	//holds the informaton after game is over 
};

//struct information for each agent that is part of the game 
struct agent {
	char* team; //team name 
	char* name; //individuals name 
	char* pebbleID; //person's pebble ID 
	float lat;  //lat unsigned int 
	float lon; //long unsigned int 
	int lastContact; //last contact 
};

//struct to hold information about the krags 
struct krag {
	char* kragID; //id hex of the krag
	char* clue; //clue text with the crag 
	char* idOfClaimer; //id of person who claimed this krag 
	float lat; //lat
	float lon;  //long
};

//struct to hold game over infomration one per a team 
struct gameOver {
	char* gameID; //game ID 
	char* team; //team name 
	int numPlayers; //num of players on this team 
	int numClaimed;  //num of claimed krags 

};



/**************** file-local constants and functions ****************/
static const int BUFSIZE = 1024;     // read/write buffer size

//Socket methods 
static int handle_stdin(int comm_sock, struct sockaddr_in *themp, void* g); //deals with socket STDIN
static void handle_socket(int comm_sock, struct sockaddr_in *themp, void *g); //deal with socket getting text from server 

//Function table methods 
static void gameStatus(char* parameters[], void* g); 
static void gsAgent(char* parameters[], void* g); 
static void gsClue(char* parameters[], void* g);
static void gsClaimed(char* parameters[], void* g);
static void gsSecret(char* parameters[], void* g);
static void gsResponse(char* parameters[], void* g);
static void gameOver(char* parameters[], void* g);
static void teamRecord(char* parameters[], void* g);
static void faLocation(char* parameters[], void* g);
static void faClaim(char* parameters[], void* g);
static void faLog(char* parameters[], void* g);

//Game Agent methods 
static int checkArgs(const int argc, char *argv[], char* variables[], struct sockaddr_in *themp); //check inputs
static void createGameStruct(void* g, char* variables[]); //initialize all of the things in game struct 
static void dealWithInfo(void* g, char* m, char* firstpart, int secondpart); //deal with the messages that surver sends 
static char* GA_STATUSReturn(void* g); //sends a status to server 
static char* GA_HINTReturn(void* g, char* h); //sends hint to server
static void freememory(void* g); //frees the memory in the game 

//print methods 
static void gameStructPrint(void* g); //prints all game struct info 
static void agentPrint(void *arg, const char *key, void *item); //prints agent info 
static void endingPrint(FILE *fp, void *item); //prints the ending game over info 
static void foundkragPrint(void *arg, const char *key, void *item); //prints the found krags 
static void unfoundkragPrint(void *arg, const char *key, void *item); //prints the unfound krags
static void mydeletehash(void *item);
static void mydeletebag(void *item);
static void mydeletebagend(void *item);



//Struct that is a function table 
//holds methods for each of the op codes 
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
  { "FA_LOCATION", faLocation },
  { "FA_CLAIM", faClaim },
  { "FA_LOG", faLog }, 
  { NULL, NULL }
};


int main(const int argc, char *argv[]){	
	// the server's address
  	struct sockaddr_in them;



  	// set up a socket on which to communicate
  	char* variables[argc-1];
	int comm_sock = checkArgs(argc, argv, variables, &them);
	
	//creates the game struct 
	struct gameStruct *game = malloc(sizeof(struct gameStruct));
	createGameStruct(game, variables);
	
	//Game into prompt
	printf("Hello %s! Welcome to the Game!\n", game->playerName);
	printf("Type 'hint,personHintIsGoingTo,hintText to send a hint to the specified person\n");
	printf("Type 'print' to print out all the information currently associated with the game\n"); 

	printf("\nTo Begin, type 'start' \n");

	//sets variable to keep track of if its the first time through the infinite true while loops 
	int firsttime = 0; 
  	
  	//Runs the bulk of the code
  	//checks if the surver has sent a message or STDIN 
  	// responds accordingly to each prompt and updates data structures 
 	while (true) {	      

	// for use with select()
    fd_set rfds;	      // set of file descriptors we want to read
    
    // Watch stdin (fd 0) and the UDP socket to see when either has input.
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);	      // stdin
    FD_SET(comm_sock, &rfds); // the UDP socket
    int nfds = comm_sock+1;   // highest-numbered fd in rfds

    // Wait for input on either source
    int select_response = select(nfds, &rfds, NULL, NULL, NULL);
    // note: 'rfds' updated

    //if this is the first time in the while loop, must send the game status 
    if(firsttime == 0){
    	firsttime = 1; //set to one so never come in here again 
    	char* statustosend = GA_STATUSReturn(game);  //get the game status from game status method 

    	if(statustosend != NULL){
    		game->ipaddress = getIP(comm_sock, them); //get the IP Adress and set it forever 
    		print_log(statustosend, "guideagent.log", game->ipaddress, "TO"); //print it to the log file
    		if (sendto(comm_sock, statustosend, strlen(statustosend), 0,  //Code from class to send to server
		    	(struct sockaddr *) &them, sizeof(* &them)) < 0) {
	    		perror("sending in datagram socket");
	   			 exit(8);
	  		}

    	}
    	free(statustosend);

    }
    
    if (select_response < 0) {
      // some error occurred
      perror("select()");
      exit(9);
    } else if (select_response == 0) {
      // timeout occurred; this should not happen
    } else if (select_response > 0) {
      // some data is ready on either source, or both

     	 if (FD_ISSET(0, &rfds)) {
			if (handle_stdin(comm_sock, &them, game) == EOF) {
	  			break; // exit loop if EOF on stdin
				}
      		}

      	if (FD_ISSET(comm_sock, &rfds)) {
			handle_socket(comm_sock, &them, game);
     	 }

      // print a fresh prompt
      printf(": ");
      fflush(stdout);
    }
  }

  //close the socket 
  close(comm_sock);
  putchar('\n');
  return 0;
	
}

//This method returns a string that is sent to the server
//hints must be in the proper format for them to work
//void g is the game struct, h is the hint string that was given 
static char* GA_HINTReturn(void* g, char* h){
	//gets the struct 
	struct gameStruct* game = g; 

	char* pointer = h;

	//splits the array by commas 
	int message_length = strlen(h);
	char* array[3];
	int count = 1;  //array placement count 
	array[0] = pointer;
	int commacount = 0;  //comma count, only want to use the first two commas 
	for(int i = 0; i < message_length; i++){ //go through message 
		if(pointer[i] == ','){	//if its a comma 
			pointer[i] = '\0'; //add new line 
			array[count] = &pointer[i + 1]; //move pointer place 
			count++;  //increase 
			commacount++;  //increase 
		}
		if(commacount>=2){ //if passed comma count break out of this loop because dont want to change more commas 
			break;
		}
	} 

	//the second parameter is person to send the array to 
	char* personToSendHint = array[1];

	//get the agents hashtable 
	hashtable_t * agents = game->agents; 


	//make sure the person exists in the hashtable 
	if(hashtable_find(agents, personToSendHint)!= NULL || strcmp(personToSendHint, "*")==0){ 
		char* hint = array[2]; 
		char* returnstr = ""; 
		if(hint!=NULL){ //if the hint exists 

			 //gets the agent form the table 
			char* begin = "opCode=GA_HINT|gameId=|guideId=|team=|player=|pebbleId=|hint="; 
			//malloc math 

			int math;
			if(strcmp(personToSendHint, "*")==0){
				math = strlen(begin) + strlen(game->gameID) + strlen(game->guideID) + strlen(game->teamName) + strlen(game->playerName) +strlen(hint) +2; 

			}
			else{
				struct agent* curragent = hashtable_find(agents, personToSendHint); 
				math = strlen(begin) + strlen(game->gameID) + strlen(game->guideID) + strlen(game->teamName) + strlen(game->playerName) + strlen(curragent->pebbleID) +strlen(hint) +1; 

			}
			//string to return at the end of all this 
			returnstr = malloc(math); 
			//much cating to make the string in the right formal 
			strcpy(returnstr, "opCode=GA_HINT|gameId="); 
			strcat(returnstr, game->gameID); 
			strcat(returnstr, "|guideId="); 
			strcat(returnstr, game->guideID); 	
			strcat(returnstr, "|team="); 
			strcat(returnstr, game->teamName);
			strcat(returnstr, "|player="); 
			strcat(returnstr, game->playerName);  
			strcat(returnstr, "|pebbleId="); 
			if(strcmp(personToSendHint, "*")==0){
				strcat(returnstr, personToSendHint);
			}
			else{
				struct agent* curragent = hashtable_find(agents, personToSendHint); 
				strcat(returnstr, curragent->pebbleID);
			}
		
			strcat(returnstr, "|hint="); 
			strcat(returnstr, hint);  
			//insert the hint into the bag of hints 
			bag_t* hints = game->hints; 
			bag_insert(hints, returnstr);  

		}
		print_log(returnstr, "guideagent.log", game->ipaddress, "TO"); //add to log 
		//printf("hint: %s\n", returnstr);
		return returnstr; 

	}
	else{
		printf("%s is not an agent and does not exist\n", personToSendHint); 
		return NULL;
	}
	

}


//This method returns a string that is sent to the server
//void g is the game struct
static char* GA_STATUSReturn(void* g){
	//get the struct 
	struct gameStruct* game = g; 

	char* begin = "opCode=GA_STATUS|gameId=|guideId=|team=|player=|statusReq="; 
	//math of the malloc 
	int math = strlen(begin) + strlen(game->gameID) + strlen(game->guideID) + strlen(game->teamName) + strlen(game->playerName) + (game->update) +1 ; 
	//malloc string to return 
	char* returnstr = malloc(math);
	//cat and copy to get return string in the right order 
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
	//sprintf(str,"%d", game->update);
	strcat(returnstr, str);

	return returnstr; //return the final string 
}

//this method takes the message and splits it
//it then sends the array into the proper function for that op code 
static void dealWithInfo(void* g, char* m, char* firstpart, int secondpart){

	//get totall amount of things needed for array 
	
	char* message;
	message = malloc(strlen(m)+1);
	
	strcpy(message, m);

	struct gameStruct* game = g; 

	//print the message to the log file 
	print_log(m, "guideagent.log", game->ipaddress, "FROM");

	//count the total amount of poles because that will be almost the size of the array 
	int total = 0; 
	for(int i = 0; i< strlen(message); i++){
		if(message[i] == '|'){ //if there is a bar, increment count 
			total++; 
		}
	}

	//create the array 
	char** array = malloc(sizeof(char*) * (total + 1));
	char* pointer = message; // pointer to beginning of string

	array[0] = pointer; //first one of the array 

	int count = 1; //place in array for loop bellow 
	int message_length = strlen(message); //message length
	
	//for all thebars, change them into the end of string character and update pointer 
	for(int i = 0; i < message_length; i++){
		if(pointer[i] == '|'){	
			pointer[i] = '\0';
			array[count] = &pointer[i + 1]; //update pointer to the following character 
			count++;  //increament count of the array 
		}
	} 

	//must go through the array again and split it by the equals sign
	char* array2[total+1]; 
	for(int i = 0; i<=total; i++){
		char* wequal = strchr(array[i], '=');  

		char* finalstring = &wequal[1]; //get string after equal sign but not equal sign 
		array2[i] = finalstring; //save that value 

	}
 
	//function table code adapted from notes 
	int fn;
	for (fn = 0; codes[fn].opCodes != NULL; fn++) { //loop through all the functions 
  		if (strcmp(array2[0], codes[fn].opCodes) == 0) { //if a function command matches the one given
 				(*codes[fn].func)(array2, g); //send the array to that function
  				break;
  		}
  		
	}

	//if none match, print unknown command
	if (codes[fn].opCodes == NULL){
  		printf("Unknown command: '%s'\n", array2[0]);
 	 }
  	free(message);
 	free(array);
}


//this method initializes everything in the game struct 
static void createGameStruct(void* g, char* variables[]){
	struct gameStruct* game = g; 
	//have some info from the inputs
	char* guideId = variables[0];
	char* team = variables[1];
	char* player = variables[2];


	game->gameID = "0"; //starts as 0
	game->firstGameStatus = 0; 
	game->guideID = guideId; 
	game->playerName = player; 
	game->teamName = team; 
	game->totalKrags = 0;
	game->secret = ""; 
	game->claimedKrags = 0; 
	game->update = 1; 
	game->ipaddress= "";
	hashtable_t* agents = hashtable_new(5); //hashtable of agents to set and put in hashtable 
	game->agents = agents;
	hashtable_t* krags = hashtable_new(10); //hashtable of krags to set and put in krags 
	game->krags = krags; 
	bag_t* hints = bag_new();  //bag of hitn data 
	game->hints = hints;
	bag_t* end = bag_new();  //bag of end data 
	game->gameOverInfo = end; 

}

//check the arguments and make sure they are all valid and can be recieved in random order 
static int checkArgs(const int argc, char *argv[], char* variables[], struct sockaddr_in *themp){
	//Check to make sure right amount of arguments 
	if(argc != 6){
		exit(1); 
	}

	//variables and booleans for everything 
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

   		//gets the string after the equals sign 
   		char next = ret[1]; 
   		char* afterEquals;
   		afterEquals = strchr(ret, next); 

   		//totall size of all arguments 
   		int totsize = strlen(argv[i]); 

   		//get string before the equals
   		int equalssize = strlen(ret); 
   		int beforeEqualSize = totsize - equalssize; 
   		char beforeEquals[beforeEqualSize +1]; 
   		//insert before equals into the array 
   		for(int j = 0; j< beforeEqualSize; j++){
   			beforeEquals[j] = argv[i][j]; 
   		}
   		beforeEquals[beforeEqualSize] = '\0'; 


   		//check what the op code is 
   		//if we have not already checked that one, flag it as checked and set the parameter equal to the after equals 
   		//do this for each time in the loop 
   		//if already exists, ignore 
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


	//make sure there are no spaces in the player id 
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

	//put these into an array to be sent to later methods 
	variables[0] = strHexGameID;
	variables[1] = team;
	variables[2] = player;
	variables[3] = host;
	variables[4] = port;

	//set up the connection to the surver 
	struct hostent *hostp = gethostbyname(host);
	if (hostp == NULL) {
    	fprintf(stderr, "%s: unknown host '%s'\n", argv[0], host);
    	exit(5);
  	}

  // Initialize fields of the server address
  	themp->sin_family = AF_INET;
  	bcopy(hostp->h_addr_list[0], &themp->sin_addr, hostp->h_length);
  	themp->sin_port = htons(atoi(port));

  // Create socket
  	int comm_sock = socket(AF_INET, SOCK_DGRAM, 0);
  	if (comm_sock < 0) {
   	 perror("opening datagram socket");
    	exit(5);
 	}

  	return comm_sock; //return socket
 

}

//This is the method that update the game struct wehn the game status op code is here 
static void gameStatus(char* parameters[], void* g){
	
	struct gameStruct* game = g; 

	//if this is the first game status, must set the game ID and the krag information 
	if(game->firstGameStatus==0 && strcmp(parameters[2], game->guideID)== 0){
		//printf("This is the game ID why isnt it working: %s \n", parameters[1]);
		char* gid = malloc(strlen(parameters[1])+1); 
		strcpy(gid, parameters[1]);
		game->gameID = gid; //set game id 
		game->totalKrags = atoi(parameters[4]); //set krag info
		game->claimedKrags = atoi(parameters[3]); //set krag info
		game->firstGameStatus = 1;  //wnat a status request 
	}
	//otherwise, if everything else is still equals and correct, just update the claimed amount 
	else if (strcmp(game->gameID, parameters[1])==0 && strcmp(game->guideID, parameters[2])==0 && game->totalKrags == atoi(parameters[4])){
		game->claimedKrags = atoi(parameters[3]);
	}
	//print the game struct since the first time 
	gameStructPrint(g);
}

//This is the method that update the game struct wehn the gs agent op code is here 
static void gsAgent(char* parameters[], void* g){
	struct gameStruct* game = g; 

	//if the paraemters are all right 
	if(strcmp(game->gameID, parameters[1])==0 && strcmp(game->teamName, parameters[3])==0){

		struct agent* curragent;
		//if the agent does already exists 
		if(hashtable_find(game->agents, parameters[4])!=NULL){
			//get the current agent 
			curragent = hashtable_find(game->agents, parameters[4]); 
			if(strcmp(curragent->pebbleID, parameters[2])==0 ){ //if the pebble id also matches 
				//update the new parameters 
				curragent->lat = atof(parameters[5]);
				curragent->lon = atof(parameters[6]);
				curragent->lastContact = atoi(parameters[7]);
				printf("agent %s has been updated to Latitude: %f Longitude: %f Last Contact: %d", parameters[4], curragent->lat, curragent->lon, curragent->lastContact);
			}
		}
		else{ //the agent does not exist yet 
			//create a new agent and set its parameters with the info we have 
			struct agent *curragent = malloc(sizeof(struct agent));
			char* n = malloc(strlen(parameters[4])+1);
			strcpy(n, parameters[4]);
			curragent->name = n;
			char* pid = malloc(strlen(parameters[2])+1);
			strcpy(pid, parameters[2]);
			curragent->pebbleID = pid;
			char* t = malloc(strlen(parameters[3])+1);
			strcpy(t, parameters[3]);
			curragent->team = t;
			curragent->lat = atof(parameters[5]);
			curragent->lon = atof(parameters[6]);
			curragent->lastContact = atoi(parameters[7]);
			hashtable_insert(game->agents, n, curragent); //insert ths into a hashtable 
			printf("agent %s has been added and is at Latitude: %f Longitude: %f Last Contact: %d", parameters[4], curragent->lat, curragent->lon, curragent->lastContact);
		}
	}
}

//This is the method that update the game struct wehn the gs clue op code is here 
static void gsClue(char* parameters[], void* g){
	struct gameStruct* game = g; 

	//get the hashtable of krags 
	hashtable_t* krags= game->krags; 
	//if all the parameters are good 
	if(strcmp(game->gameID, parameters[1])==0 && strcmp(game->guideID, parameters[2])==0){
		if(hashtable_find(krags, parameters[3])== NULL){ //if the krag is not there 
			//create a new krag and fill in its information 
			struct krag *currkrag = malloc(sizeof(struct krag));
			char* kid = malloc(strlen(parameters[3])+1);
			strcpy(kid, parameters[3]);
			currkrag->kragID = kid;

			char* c = malloc(strlen(parameters[4])+1);
			strcpy(c, parameters[4]);
			currkrag->clue= c;
			currkrag->idOfClaimer = "";
			currkrag->lat = 0;
			currkrag->lon = 0;
			hashtable_insert(game->krags, parameters[3], currkrag); //insert it into the table 
			printf("The new clue is: '%s' \n", currkrag->clue);

		}
	}
}

//This is the method that update the game struct wehn the gs claimed op code is here 
static void gsClaimed(char* parameters[], void* g){

	struct gameStruct* game = g; 
	hashtable_t* krags= game->krags;  //get the krags 
	//if all the parameters are wroking 
	if(strcmp(game->gameID, parameters[1])==0 && strcmp(game->guideID, parameters[2])==0){
		if(hashtable_find(krags, parameters[4])!=NULL){ //if the krag exists 
			struct krag* currkrag = hashtable_find(krags, parameters[4]); //get the current krag 
			//update parameters 
			char* idoc = malloc(strlen(parameters[3])+1);
			strcpy(idoc, parameters[3]);
			currkrag->idOfClaimer = idoc;
			currkrag->lat = atof(parameters[5]);
			currkrag->lon = atof(parameters[6]);
			printf("The krag coorsponding to this clue: %s was claimed at Lat: %f Long: %f\n", currkrag->clue, currkrag->lat, currkrag->lon); 
		}

	}

}

//This is the method that update the game struct wehn the gs secret op code is here 
static void gsSecret(char* parameters[], void* g){

	struct gameStruct* game = g; 

	//if all the parameters are correct 
	if(strcmp(game->gameID, parameters[1])==0 && strcmp(game->guideID, parameters[2])==0){
		char* s = malloc(strlen(parameters[3])+1);
		strcpy(s, parameters[3]);
		game->secret= s; //update the secret 
	}
	printf("The updated secret is now: %s", game->secret);
}

//This is the method that update the game struct wehn the gs response op code is here 
static void gsResponse(char* parameters[], void* g){
	//dont do anything here because it was logged and thats all that matters for now 
}

//This is the method that update the game struct wehn the game over op code is here 
static void gameOver(char* parameters[], void* g){
	struct gameStruct* game = g; 

	//if the paramter is right 
	if(strcmp(game->gameID, parameters[1])==0){
		printf("***GAME OVER***\n");
		printf("The final secret is %s", parameters[2]);
		struct gameStruct* game = g; 
		bag_t* ending = game->gameOverInfo;  //print the information that came from the team record method
		bag_print(ending, stdout, endingPrint);
		freememory(g); 
		exit(0);
	}

}
//This is the method that update the game struct wehn the teamRecord op code is here 
static void teamRecord(char* parameters[], void* g){
	//create a new node for the end bag 
	//printf("here ending stuff\n");
	struct gameOver* end = malloc(sizeof(struct gameOver)); 
	//set the parameters for all the new end node 
	end->gameID= parameters[1];
	end->team = parameters[3];
	end->numClaimed = atoi(parameters[2]);
	end->numPlayers = atoi(parameters[4]); 
	struct gameStruct* game = g; 
	bag_insert(game->gameOverInfo, end);  //insert the end data into the bag 

}

//This is the method that update the game struct wehn the fa location op code is here 
static void faLocation(char* parameters[], void* g){
	//do nothing becuase this should never actually come here 
}
//This is the method that update the game struct wehn the fa claim op code is here 
static void faClaim(char* parameters[], void* g){
	//do nothing becuase this should never actually come here 
}
//This is the method that update the game struct wehn the falog op code is here 
static void faLog(char* parameters[], void* g){
	//do nothing becuase this should never actually come here 
}

//This is the metod that prints all the info in the game struct 
static void gameStructPrint(void* g){
  struct gameStruct* game = g; 

  printf("***CURRENT GAME STRUCT***\n");
  printf("The gameID is: %s , The guideID is %s , the player name is: %s,  the Team name is %s \n", game->gameID, game->guideID, game->playerName, game->teamName); 
  printf("The total num of krags are %d, the total num collected is %d, and the secret is %s\n", game->totalKrags, game->claimedKrags, game->secret);
  printf("Agents:\n");
  hashtable_iterate(game->agents, stdout, agentPrint); //prints the agents 
  printf("Unfound Krags: \n");
  hashtable_iterate(game->krags, stdout, unfoundkragPrint); //prints unfound krags 
  printf("Found Krags \n");
  hashtable_iterate(game->krags, stdout, foundkragPrint); //prints found krags 
}

//prints all the agents 
static void agentPrint(void *arg, const char *key, void *item){
	struct agent* curragent = item;
	printf("name: %s, team: %s, pebbleID: %s\n", curragent->name, curragent->team, curragent->pebbleID);
	printf("	lat: %f , long: %f, lastContact: %d\n", curragent->lat, curragent->lon, curragent->lastContact);

}

//prints all the unfound krags 
static void unfoundkragPrint(void *arg, const char *key, void *item){
	struct krag* currkrag = item;
	if(strcmp(currkrag->idOfClaimer,"") == 0 && currkrag->lat == 0 && currkrag->lon==0){ //checls if the found id is still empty 
		printf("There is this clue: '%s' for an unfound krag\n", currkrag->clue);
	} 
	
}
//prints all the found krags 
static void foundkragPrint(void *arg, const char *key, void *item){
	struct krag* currkrag = item;

	if(strcmp(currkrag->idOfClaimer,"") != 0){ //checks if the found pebble id is set to somethign 
		printf("krag ID: %s, Clue: %s, pebbleID: %s\n", currkrag->kragID, currkrag->clue, currkrag->idOfClaimer);
		printf("	lat: %f , long: %f\n", currkrag->lat, currkrag->lon);
	}
}
//prints all the game over strings at the end 
static void endingPrint(FILE *fp, void *item){
	struct gameOver* end = item;
	printf("\ngameID: %s, team: %s, num players: %d, num found: %d\n", end->gameID, end->team, end->numPlayers, end->numClaimed);
}


//taken from examples 
/**************** handle_stdin ****************/
/* stdin has input ready; read a line and send it to the client.
 * return EOF if EOF was encountered on stdin;
 * return 0 if there is no client to whom we can send;
 * return 1 if message sent successfully.
 * exit on any socket error.
 */
static int
handle_stdin(int comm_sock, struct sockaddr_in *themp, void* g)
{
   struct gameStruct* game = g; 
  char *response = readlinep(stdin);
  char* fullLine;
  if (response == NULL) 
    return EOF;

	//if stdin starts with hint
	if(response[0] == 'h' && response[1] == 'i' && response[2] == 'n' && response[3]=='t'){
		fullLine = GA_HINTReturn(game, response); //send this to the hint method 
	}
	else if (strcmp(response, "print")==0){ //if it says print, 
		gameStructPrint(g); //send this to the print method 
		fullLine = NULL;
	}
	else if(strcmp(response, "update")==0){
		fullLine = GA_STATUSReturn(game); 
	}
	else{
		fullLine= response; //just send whatever irrelevant thing came 
	}
	
	if(fullLine!=NULL){

	  if (themp->sin_family != AF_INET) {
	    printf("I am confused: server is not AF_INET.\n");
	    fflush(stdout);
	    return 0;
	  } 
	  //send the response to the surver 
	  if (sendto(comm_sock, fullLine, strlen(fullLine), 0, 
		     (struct sockaddr *) themp, sizeof(*themp)) < 0) {
	    perror("sending in datagram socket");
	    exit(10);
	  }
	}
	
  
  free(response);

  return 1;
}

//taken from the examples from class 
/**************** handle_socket ****************/
/* Socket has input ready; receive a datagram and print it.
 * 'themp' should be a valid address representing the expected sender.
 * Exit on any socket error.
 */
static void
handle_socket(int comm_sock, struct sockaddr_in *themp, void* g)
{
  // socket has input ready
  struct sockaddr_in sender;		 // sender of this message
  struct sockaddr *senderp = (struct sockaddr *) &sender;
  socklen_t senderlen = sizeof(sender);  // must pass address to length
  char buf[BUFSIZE];	      // buffer for reading data from socket
  int nbytes = recvfrom(comm_sock, buf, BUFSIZE-1, 0, senderp, &senderlen);

  if (nbytes < 0) {
    perror("receiving from socket");
    exit(1);
  } else {
    buf[nbytes] = '\0';     // null terminate string

    // where was it from?
    if (sender.sin_family != AF_INET) {
      printf("From non-Internet address: Family %d\n", sender.sin_family);
    } else {
      // was it from the expected server?
      if (sender.sin_addr.s_addr == themp->sin_addr.s_addr && 
	  sender.sin_port == themp->sin_port) {
	// print the message
      	//printf("got this message: %s, validate gives: %d\n", buf, validate_message(buf));
      	if(validate_message(buf)==0){

			dealWithInfo(g, buf, inet_ntoa(sender.sin_addr), ntohs(sender.sin_port));
      	}
      } else {
	printf("[%s@%05d]: %s\n",
	       inet_ntoa(sender.sin_addr),
	       ntohs(sender.sin_port),
	       buf);
      }
    }
    fflush(stdout);
  }
}

static void freememory(void* g){
	struct gameStruct* game = g; 
	hashtable_t* agents = game->agents; 
	hashtable_delete(agents, mydeletehash); 
	hashtable_t* krags = game->krags;
	hashtable_delete(krags, mydeletehash); 
	bag_t* hints = game->hints; 
	bag_delete(hints, mydeletebag);
	bag_t* over = game->gameOverInfo; 
	bag_delete(over, mydeletebagend);
	free(game->ipaddress);
	free(game);


}

static void
mydeletehash(void *item){
  if (item != NULL) { //if item is not equal to null, free it 

     free(item); //calls counters delete because the item is a counter
  }
}

static void 
mydeletebag(void *item)
{
	;
}

static void 
mydeletebagend(void *item)
{
	free(item);
}
