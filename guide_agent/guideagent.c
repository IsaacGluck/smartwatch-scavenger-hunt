//include statements
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bag.h"
#include "hashtable.h"
#include "../common/shared.h"

#include <stdbool.h>
#include <unistd.h>	      // read, write, close
#include <strings.h>	      // bcopy, bzero
#include <netdb.h>	      // socket-related structures
#include <arpa/inet.h>	      // socket-related calls
#include <sys/select.h>	      // select-related stuff 
#include "file.h"	



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
  char* ipaddress; 
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



/**************** file-local constants and functions ****************/
static const int BUFSIZE = 1024;     // read/write buffer size
static int handle_stdin(int comm_sock, struct sockaddr_in *themp, void* g);
static void handle_socket(int comm_sock, struct sockaddr_in *themp, void *g);

//Other methods 
static void gameStatus(char* parameters[], void* g);
static void gsAgent(char* parameters[], void* g);
static void gsClue(char* parameters[], void* g);
static void gsClaimed(char* parameters[], void* g);
static void gsSecret(char* parameters[], void* g);
static void gsResponse(char* parameters[], void* g);
static void gameOver(char* parameters[], void* g);
static void teamRecord(char* parameters[], void* g);

static int checkArgs(const int argc, char *argv[], char* variables[], struct sockaddr_in *themp);
static void createGameStruct(void* g, char* variables[]);
static void dealWithInfo(void* g, char* m, char* firstpart, int secondpart);
static char* GA_STATUSReturn(void* g);
static char* GA_HINTReturn(void* g, char* h);
static void gameStructPrint(void* g); 
static void agentPrint(void *arg, const char *key, void *item);
static void endingPrint(FILE *fp, void *item);
static void hintPrint(FILE *fp, void *item);
static void foundkragPrint(void *arg, const char *key, void *item);
static void unfoundkragPrint(void *arg, const char *key, void *item);
static char* getIP(int comm_sock, struct sockaddr_in *themp);



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

	 // the server's address
  	struct sockaddr_in them;


//socket_setup(const int argc, char* program, char* hostname, int port, struct sockaddr_in *themp)

  	// set up a socket on which to communicate

  	// prompt the user to write a message

  	char* variables[argc-1];
 

	int comm_sock = checkArgs(argc, argv, variables, &them);



	struct gameStruct *game = malloc(sizeof(struct gameStruct));

	createGameStruct(game, variables);
	//gameStructPrint(game);
	//printf("%s", GA_STATUSReturn(game));

	printf("Hello %s! Welcome to the Game!\n", game->playerName);
	printf("Type 'hint,personHintIsGoingTo,hintText to send a hint to the specified person\n");
	printf("Type 'print' to print out all the information currently associated with the game\n"); 

	printf("\nTo Begin, type 'start' \n");

	int firsttime = 0; 
  	// read from either the socket or stdin, whichever is ready first;
  	// if stdin, read a line and send it to the socket;
  	// if socket, receive message from socket and write to stdout.
 	while (true) {	      // loop exits on EOF from stdin



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

    if(firsttime == 0){
    	firsttime = 1; 
    	char* statustosend = GA_STATUSReturn(game); 

    	if(statustosend != NULL){
    		printf("%s\n");
    		game->ipaddress = getIP(comm_sock, &them);
    		print_log(statustosend, "guideagent.log", game->ipaddress, "TO");
    		if (sendto(comm_sock, statustosend, strlen(statustosend), 0, 
		    	(struct sockaddr *) &them, sizeof(* &them)) < 0) {
	    		perror("sending in datagram socket");
	   			 exit(6);
	  		}
    	}

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

  close(comm_sock);
  putchar('\n');
  return 0;
	
}

static char* GA_HINTReturn(void* g, char* h){
	//opCode=GA_HINT|gameId=|guideId=|team=|player=|pebbleId=|hint=
	struct gameStruct* game = g; 

	char* pointer = h;

	//splits the array 
	int message_length = strlen(h);
	char* array[3];
	int count = 1; 
	array[0] = pointer;
	int commacount = 0; 
	for(int i = 0; i < message_length; i++){
		if(pointer[i] == ','){	
			pointer[i] = '\0';
			array[count] = &pointer[i + 1];
			count++; 
			commacount++;  
		}
		if(commacount>=2){
			break;
		}
	} 

	char* personToSendHint = array[1];

	hashtable_t * agents = game->agents; 
	if(hashtable_find(agents, personToSendHint)!= NULL){
		char* hint = array[2]; 
		char* returnstr = ""; 
		printf("hint: %s\n", hint);
		if(hint!=NULL){
			struct agent* curragent = hashtable_find(agents, personToSendHint); 
			char* begin = "opCode=GA_HINT|gameId=|guideId=|team=|player=|pebbleId=|hint="; 
			int math = strlen(begin) + strlen(game->gameID) + strlen(game->guideID) + strlen(game->teamName) + strlen(curragent->name) + strlen(curragent->pebbleID) +strlen(hint) +1; 
			returnstr = malloc(math); 
			strcpy(returnstr, "opCode=GA_HINT|gameId="); 
			strcat(returnstr, game->gameID); 
			strcat(returnstr, "|guideId="); 
			strcat(returnstr, game->guideID); 	
			strcat(returnstr, "|team="); 
			strcat(returnstr, game->teamName);
			strcat(returnstr, "|player="); 
			strcat(returnstr, curragent->name);  
			strcat(returnstr, "|pebbleId="); 
			strcat(returnstr, curragent->pebbleID);
			strcat(returnstr, "|hint="); 
			strcat(returnstr, hint);  
			bag_t* hints = game->hints; 
			bag_insert(hints, returnstr);  

		}
		printf("IN hint method return str: %s", returnstr);
		print_log(returnstr, "guideagent.log", game->ipaddress, "TO");

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

	//printf("This is the returnstr: %s\n", returnstr);
	return returnstr;
}

static void dealWithInfo(void* g, char* m, char* firstpart, int secondpart){

	//get totall amount of things needed for array 
	char* message = malloc(strlen(m));
	strcpy(message, m);

	char* ipaddress = malloc(strlen(firstpart) + secondpart + 2); 
	
	strcpy(ipaddress, firstpart);
	strcat(ipaddress, "@");
	//printf("here i am alive and well and happy \n");

	char stringnum[10];
	sprintf(stringnum, "%d", secondpart);

	strcat(ipaddress, stringnum);

	//printf("i made the string i need to make: %s \n", ipaddress);

	print_log(m, "guideagent.log", ipaddress, "FROM");


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
	//printf("%s\n", array[0]);
	int count = 1;
	int message_length = strlen(message);
	

	for(int i = 0; i < message_length; i++){
		if(pointer[i] == '|'){	
			pointer[i] = '\0';
			array[count] = &pointer[i + 1];
			//printf("%s\n", &pointer[i]);
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
		//printf("array: %s\n", array[i]);
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
	//printf("GUIDE ID? %s\n", variables[0]);
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
	game->ipaddress= "";
	hashtable_t* agents = hashtable_new(5);
	game->agents = agents;
	hashtable_t* krags = hashtable_new(10);
	game->krags = krags; 
	bag_t* hints = bag_new(); 
	game->hints = hints;
	bag_t* end = bag_new(); 
	game->gameOverInfo = end; 

}

static char* getIP(int comm_sock, struct sockaddr_in *themp){
 printf("HERE I AM\n");
  struct sockaddr_in sender;		 // sender of this message
  struct sockaddr *senderp = (struct sockaddr *) &sender;
  socklen_t senderlen = sizeof(sender);  // must pass address to length
  char buf[BUFSIZE];	      // buffer for reading data from socket
  printf("once again\n");

  char* firstpart = inet_ntoa(sender.sin_addr);

  int secondpart = ntohs(sender.sin_port);

	char* ipaddress = malloc(strlen(firstpart) + secondpart + 2); 
	
	strcpy(ipaddress, firstpart);
	strcat(ipaddress, "@");
	//printf("here i am alive and well and happy \n");

	printf("feeling lost but now and then\n");
	char stringnum[10];
	sprintf(stringnum, "%d", secondpart);

	strcat(ipaddress, stringnum);

	printf("ipadress return : %s", ipaddress);
	return ipaddress;

}

static int checkArgs(const int argc, char *argv[], char* variables[], struct sockaddr_in *themp){
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

	//printf("Guide ID: %s, Team: %s, Player: %s, Host: %s, Port: %s\n", strHexGameID, team, player, host, port);


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

	struct hostent *hostp = gethostbyname(host);
	if (hostp == NULL) {
    	fprintf(stderr, "%s: unknown host '%s'\n", argv[0], host);
    	exit(3);
  	}

  // Initialize fields of the server address
  	themp->sin_family = AF_INET;
  	bcopy(hostp->h_addr_list[0], &themp->sin_addr, hostp->h_length);
  	themp->sin_port = htons(atoi(port));

  // Create socket
  	int comm_sock = socket(AF_INET, SOCK_DGRAM, 0);
  	if (comm_sock < 0) {
   	 perror("opening datagram socket");
    	exit(2);
 	}

  	return comm_sock;
 

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
		//printf("IN ELSE IF\n");
		game->claimedKrags = atoi(parameters[3]);
	}

	gameStructPrint(g);
	//printf("%s\n", game->gameID);
}

static void gsAgent(char* parameters[], void* g){
	//printf("In gsagent method\n");
	struct gameStruct* game = g; 

	//opCode=GS_AGENT|gameId=|pebbleId=|team=|player=|latitude=|longitude=|lastContact=
	if(strcmp(game->gameID, parameters[1])==0 && strcmp(game->teamName, parameters[3])==0){
	//	printf("In mega if statement\n");
		struct agent* curragent;

		if(hashtable_find(game->agents, parameters[4])!=NULL){
			//printf("In found if statement\n");
			curragent = hashtable_find(game->agents, parameters[4]); 
			if(strcmp(curragent->pebbleID, parameters[2])==0 ){
				//printf("In found if statment w pebble ID matching\n");

				curragent->lat = atof(parameters[5]);
				curragent->lon = atof(parameters[6]);
				curragent->lastContact = atoi(parameters[7]);
				printf("agent %s has been updated to Latitude: %f Longitude: %f Last Contact: %d", parameters[4], curragent->lat, curragent->lon, curragent->lastContact);
			}
		}
		else{
			//printf("In else statement\n");

			struct agent *curragent = malloc(sizeof(struct agent));
			curragent->name = parameters[4];
			curragent->pebbleID = parameters[2];
			curragent->team = parameters[3];


			curragent->lat = atof(parameters[5]);
			curragent->lon = atof(parameters[6]);
			curragent->lastContact = atoi(parameters[7]);
			//printf("print %s \n", parameters[4]);
			hashtable_insert(game->agents, parameters[4], curragent);
			printf("agent %s has been added and is at Latitude: %f Longitude: %f Last Contact: %d", parameters[4], curragent->lat, curragent->lon, curragent->lastContact);



		}
	}

	//gameStructPrint(game);


}
static void gsClue(char* parameters[], void* g){
	//opCode=GS_CLUE|gameId=FEED|guideId=0707|kragId=07E1|clue=A stone building for religious services, under the third archway.

	//printf("In gsclue method\n");
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
			printf("The new clue is: %s", currkrag->clue);

		}
	}

	//gameStructPrint(game);

}
static void gsClaimed(char* parameters[], void* g){
	//opCode=GS_CLAIMED|gameId=|guideId=|pebbleId=|kragId=|latitude=|longitude=

	//printf("In gsclaimed method\n");
	struct gameStruct* game = g; 
	hashtable_t* krags= game->krags; 
	if(strcmp(game->gameID, parameters[1])==0 && strcmp(game->guideID, parameters[2])==0){
		if(hashtable_find(krags, parameters[4])!=NULL){
			struct krag* currkrag = hashtable_find(krags, parameters[4]); 
			currkrag->idOfClaimer = parameters[3];
			currkrag->lat = atof(parameters[5]);
			currkrag->lon = atof(parameters[6]);
			printf("The krag coorsponding to this clue: %s was claimed at Lat: %f Long: %f\n", currkrag->clue, currkrag->lat, currkrag->lon); 
		}

	}

	//gameStructPrint(game);


}
static void gsSecret(char* parameters[], void* g){
	//printf("In secret method\n");
	struct gameStruct* game = g; 

	//opCode=GS_SECRET|gameId=FEED|guideId=0707|secret=com_____ _cie____50
	//printf("|%s| |%s| %d\n", game->gameID, parameters[1], strcmp(game->gameID, parameters[1]));
	if(strcmp(game->gameID, parameters[1])==0 && strcmp(game->guideID, parameters[2])==0){
		game->secret= parameters[3];
	}
	//gameStructPrint(g);
	printf("The updated secret is now: %s", game->secret);


}
static void gsResponse(char* parameters[], void* g){
	printf("In respone method\n");


}

static void gameOver(char* parameters[], void* g){
	//printf("In gameover method\n");
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
	struct gameOver* end = malloc(sizeof(struct gameOver)); 
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
  printf("Agents:\n");
  hashtable_iterate(game->agents, stdout, agentPrint);
  printf("Unfound Krags: \n");
  hashtable_iterate(game->krags, stdout, unfoundkragPrint);
  printf("Found Krags \n");
  hashtable_iterate(game->krags, stdout, foundkragPrint);


}

static void agentPrint(void *arg, const char *key, void *item){
	struct agent* curragent = item;
	printf("name: %s, team: %s, pebbleID: %s\n", curragent->name, curragent->team, curragent->pebbleID);
	printf("	lat: %f , long: %f, lastContact: %d\n", curragent->lat, curragent->lon, curragent->lastContact);

}

static void unfoundkragPrint(void *arg, const char *key, void *item){
	struct krag* currkrag = item;
	if(strcmp(currkrag->idOfClaimer,"") == 0 && currkrag->lat == 0 && currkrag->lon==0){
		printf("There is this clue: %s for an unfound krag\n", currkrag->clue);
	} 
	
}
static void foundkragPrint(void *arg, const char *key, void *item){
	struct krag* currkrag = item;

	if(strcmp(currkrag->idOfClaimer,"") != 0){
		printf("krag ID: %s, Clue: %s, pebbleID: %s\n", currkrag->kragID, currkrag->clue, currkrag->idOfClaimer);
		printf("	lat: %f , long: %f\n", currkrag->lat, currkrag->lon);
	}
}

static void endingPrint(FILE *fp, void *item){
	struct gameOver* end = item;
	printf("gameID: %s, team: %s, num players: %d, num found: %d\n", end->gameID, end->team, end->numPlayers, end->numClaimed);
}

static void hintPrint(FILE *fp, void *item){
	char* hint = item;
	printf("%s\n", hint);
}


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

	if(response[0] == 'h' && response[1] == 'i' && response[2] == 'n' && response[3]=='t'){
		fullLine = GA_HINTReturn(game, response);
	}
	else if (strcmp(response, "print")==0){
		gameStructPrint(g);
		fullLine = NULL;
	}
	else{
		fullLine= response;
	}
	
	if(fullLine!=NULL){

	  if (themp->sin_family != AF_INET) {
	    printf("I am confused: server is not AF_INET.\n");
	    fflush(stdout);
	    return 0;
	  } 

	  if (sendto(comm_sock, fullLine, strlen(fullLine), 0, 
		     (struct sockaddr *) themp, sizeof(*themp)) < 0) {
	    perror("sending in datagram socket");
	    exit(6);
	  }
	}
	
  
  free(response);

  return 1;
}

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
		dealWithInfo(g, buf, inet_ntoa(sender.sin_addr), ntohs(sender.sin_port));
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
