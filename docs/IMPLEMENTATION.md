## COMMON 
#### In the common direcotry we have the following methods that are useful to everything in shared
##### shared.c
* `char** tokenize(char*  message)`
    *  *Use*: split the message into an array with all the inputs after the = sign 
    *  determine size of arrray by size of = signs 
    *  Locate the | and the = sign 
    *  split the string between these places and put the value in the array 
    *  do this for all portential values
    *  return array 
* `int print_log(char* message, char* filename, char* IPport, char* tofrom);`
    * *Use:* prints the message to the speceified log file 
    * Calculates the time stamp 
    * Does string manipulation to get file and path 
    * Opens the file with append 
    * Does string manipulation to print everything in the proper order onto the file 

#### Methods for validate messages and the op codes function table 
* `int ValidateMessage(char* message)`  
    * *Use:* Make sure the messages are in the right format 
    * check to make sure no punctuation besides | or =
    * split the string into an array
    * validate all the parameters in the array via specific op code
    * If issues splitting, return 1
    * go to the op code's specific validation function and check all parts 
    * returns value recieved from the op code 
* `static int gsAgent(char* parameters[], int total);`
    * *Use:* checks the parameters in gsAgent  
    * Check all parameters are of the right type for this op code, if not, return 3
    * Check all parameters are present and only the right ones are present, if not, return 3
    * Check there are the right amount of parameters, if not, return 5
* `static int gsClue(char* parameters[], int total)`
    * *Use:* checks the parameters in gsAgent  
    * Check all parameters are of the right type for this op code, if not, return 3
    * Check all parameters are present and only the right ones are present, if not, return 3
    * Check there are the right amount of parameters, if not, return 5
* `static int gsClaimed(char* parameters[], int total);`
    * *Use:* checks the parameters in gsAgent  
    * Check all parameters are of the right type for this op code, if not, return 3
    * Check all parameters are present and only the right ones are present, if not, return 3
    * Check there are the right amount of parameters, if not, return 5
* `static int gsSecret(char* parameters[], int total);`
    * *Use:* checks the parameters in gsAgent  
    * Check all parameters are of the right type for this op code, if not, return 3
    * Check all parameters are present and only the right ones are present, if not, return 3
    * Check there are the right amount of parameters, if not, return 5
* `static int teamRecord(char* parameters[], int total);`
    * *Use:* checks the parameters in gsAgent  
    * Check all parameters are of the right type for this op code, if not, return 3
    * Check all parameters are present and only the right ones are present, if not, return 3
    * Check there are the right amount of parameters, if not, return 5
* `static int gaStatus(char* parameters[], int total);`
    * *Use:* checks the parameters in gsAgent  
    * Check all parameters are of the right type for this op code, if not, return 3
    * Check all parameters are present and only the right ones are present, if not, return 3
    * Check there are the right amount of parameters, if not, return 5
* `static int gameStatus(char* parameters[], int total);`
    * *Use:* checks the parameters in gsAgent  
    * Check all parameters are of the right type for this op code, if not, return 3
    * Check all parameters are present and only the right ones are present, if not, return 3
    * Check there are the right amount of parameters, if not, return 5
* `static int gsResponse(char* parameters[], int total);`
    * *Use:* checks the parameters in gsAgent  
    * Check all parameters are of the right type for this op code, if not, return 3
    * Check all parameters are present and only the right ones are present, if not, return 3
    * Check there are the right amount of parameters, if not, return 5
* `static int gaHint(char* parameters[], int total);`
    * *Use:* checks the parameters in gsAgent  
    * Check all parameters are of the right type for this op code, if not, return 3
    * Check all parameters are present and only the right ones are present, if not, return 3
    * Check there are the right amount of parameters, if not, return 5
* `static int faLocation(char* parameters[], int total);`
    * *Use:* checks the parameters in gsAgent  
    * Check all parameters are of the right type for this op code, if not, return 3
    * Check all parameters are present and only the right ones are present, if not, return 3
    * Check there are the right amount of parameters, if not, return 5
* `static int faClaim(char* parameters[], int total);`
    * *Use:* checks the parameters in gsAgent  
    * Check all parameters are of the right type for this op code, if not, return 3
    * Check all parameters are present and only the right ones are present, if not, return 3
    * Check there are the right amount of parameters, if not, return 5
* `static int faLog(char* parameters[], int total);`
    * *Use:* checks the parameters in gsAgent  
    * Check all parameters are of the right type for this op code, if not, return 3
    * Check all parameters are present and only the right ones are present, if not, return 3
    * Check there are the right amount of parameters, if not, return 5
* `static int gameOver(char* parameters[], int total);`
    * *Use:* checks the parameters in gsAgent  
    * Check all parameters are of the right type for this op code, if not, return 3
    * Check all parameters are present and only the right ones are present, if not, return 3
    * Check there are the right amount of parameters, if not, return 5

#### Helper Methods for the pebble
* `char ishex (unsigned char c);`
    * *Use*: determines if the char is a hex char 
    * Makes sure the char is the right character to be hex and the right number    
* `double string_to_double(char* num);`
    * *Use*: Converts the given char* to a double and returns that 
    * uses number manipulation ad string manipulation 
    * returns the double 
* `int string_to_int(char *str);`
    * *Use*: Converts the given char* to a int and returns that 
    * uses number manipulation ad string manipulation 
    * returns the int 

##### common.c
* `unsigned int stringHexToDec(char* hex)`
    * *Use*: Converts a string hexidemical to a decimal int 
    * uses scanf to get the decimal 
    * returns the decima; 
* `char *decToStringHex(unsigned int dec)`
    * *Use:* Converts a number to the string hex counter 
    * uses sprintf to convert to string
    * returns the string 
* `char* getIP(int comm_sock, struct sockaddr_in them)`
    * *Use*: gets the IP adress of the server 
    * Finds the IP address and mallocs it into a string 
    * adds the numbers at the end as well
    * returns the IP adress 
* `char **getOpCode(char *message)`
    * *Use*: gets the opCode from a message 
    * finds the op code and places that into array 0
    * puts everything else itno array 1 

## GUIDE AGENT 

### Methods 
#### The Guide Agent will have the following methods: 
* `int main(const int argc, char *argv[])`
    * *Use*: Runs the code and continuously waits for input from STDIN or the server in a while loop. Calls upon needed functions 
    * *PsuedoCode*:
        *  Check arguments
        *  create the game struct
        *  send first game status method
        *  continiously look for STDIN or message from the server and call upon correct method to deal with that
* `void checkArgs(const in argc, char *argv[], char* variables[], struct sockaddr_in *themp)` 
    * *Use*: Validates command lines arguments 
    * *PsuedoCode*:  
        * Takes in Arugments passed to main 
        * Splits them into variables 
        * Make sure the right number of arguments are given 
        * Make sure each input is of the right type and not NULL
        * Checks the length of all variables and their types 
        * initializes fields of server address 
        * create the socket
        * Exist with a status of non zero if anything is wrong
* `static void createGameStruct(void* g, char* variables[])` 
    * *Use*: Creates game struct and initalizes all of the variables in it
    * *Psuedocode*: 
        * Takes all arguments passed in and places them into the game struct using setter methods 
        * Create the needed hashtable and bag requests
        * initialize data we dont have yet to zero or an empty string 
    
* `static char* GA_STATUSReturn(void* g)`
    * *Use*: sends the server the current game status in a specified way via the format given. 
    * *PsuedoCode*: 
        * Retrieves the game Id, Guide ID, team, and player from the Gamestruct passed in  
        * Malloc a string 
        * Does string manipulation to get everything in the right order and format 
        * Returns the string which is the op code to be sent 
* `static char* GA_HINTReturn(void* g, char* h)`
    * *Use*: returns the properly formatted code to send a hint with the inserted data  
    * *PsudoCode*: 
        * parses the string to find out who the hint is for and what the hint is 
        * Retrieves the gameID, team from the struct given 
        * if the person to send the hint to is *, send it to everyone and set that to pebble ID 
        * Otherwise, use the pebble ID given
        * if pebbleID  does not exist, print "non existant name" 
        * Malloc a string
        * Put the string together in the proper format using string manipulation 
        * return the string to be sent across the server 
* `static void dealWithInfo(void* g, char* m, char* firstpart, int secondpart)`
    * *Use*: takes in recieved line, prints to file, send arguments next function depending on their op code 
    * *PsuedoCode*: 
        * Prints the line to the file using the common log print method 
        * Determine the total amount of poles for the size of the array
        * Does string manipulation to determine the op code
        * Parses the rest of the string into an array of determined size
        * Send the Array to the right function in the function table Via the Opcode determined
        * If opcode doesnt exist, print error message 
        * free data
* `static void freememory(void * g)`
    * *Use:* Frees the memory in the game struct 
    *  *psuedocode*: 
        * calls all delete methods for hashtable and bags 
        * frees memory with malloced strings 
# Function table functions 
* `static void gameStatus(char * parameters [], void * g); `
    * *Use*: updates the game with opCode status update information 
    * *PsuedoCode*: 
        * array[0] should be game ID so check that this is the same game ID that the game struct has
        * Array[1] is the guide ID so make sure that this is the right ID of the guide via the game struct 
        * Array[2] is the number of claimed krags and update this in the game struct if it is not currently accurate 
        * Array[3] is the number of krags in total and make sure this is also correct 
        * If any data is not correct, print the error message 
        * print error message at the end 
* `static void gsAgent(char* parameters[], void* g);`
    * *Use*: updates the agent info with opCode agent information
    * *PsuedoCode*: 
        * Array[0] is the game ID so check that
        * Array[1] is the pebble ID. Look up player and chec that Array[2] (team) and Array[3] (player name) are all correct. 
        * if the player is correct then update everything 
        * Update the latitute Array[4]
        * Update the longitude Array[5]
        * Update last contacted Array[6] 
        * if the player does not yet exist, create a new player with all of this information 
* `void Clue(char[] arr, GameStruct g)`
    * *Use*: updates the clue bag
    * *Psuedocode*: 
        * Check Array[0] (game ID) and Array[1] (guide ID) with the game struct 
        * If error print error message and do nothing 
        * Add Krag ID to the hashtable of Krags 
        * Add Clue to the hashtable of Clues 
        * Print the new clue out for the guide agent 
* `static void gsSecret(char* parameters[], void* g);`
    * *Use*: updates the secret when given the secret op code
    * *PsuedoCode*: 
        * Check Array[0] (game ID) and Array[1] (guide ID) to make sure they are correct with the game struct 
        * If not correct, print error message
        * Update the secret in the game struct with Array[2] 
        * Print the new secret message out for the guide agent 
*  `static void gsResponse(char* parameters[], void* g);`
    * *Use:* looks at response code from server 
    * *Psuedocode:* 
        * Check Array[0] and make sure the Game ID given matches the one in the game struct 
        * Agent will then look at the response code and print it so recovery action is taken 
*  `static void gsClaimed(char* parameters[], void* g)`
    * *Use*: Gives and updates information about the krag claimed 
    * *PsuedoCode:*
        * Check Array[0] (Game ID) and Array[1] (guide ID) with the information from the game struct 
        * look up the krag from the hashtable of players using the krag ID (Array[3])
        * if the krag doesnt exist then do nothing
        * if the krag does exist, update its parameters and set the lat, long and id of the claimer 
* `static void gsClue(char* parameters[], void* g)`
    * *Use:* updates the game struct when a clue was given and keeps track of the krags
    * *psuedocode*: 
        * get a clue for a krag
        * check to see if we have seen that clue before  
        * if not, create a new krag for that clue. Initialize values as empty, null or 0 except for the clue and krag ID 
        * Insert this krag into the hash table of krags 
* `static void teamRecord(char* parameters[], void* g)`
    * *Use*: stores the end of the game informaton to be printed later 
    * *psuedoCode*: 
        * create a new game over struct 
        * instantiate all teh data with the array parameters gave 
        * add this to the bag of game over structs 
* `static void gameOver(char* parameters[], void* g)`
    * *Use*: prints all the team information and exits the game 
    * *Psuedocode*: 
        * loops through the bag of game over struct
        * prints the information
        * prints the final secret given
        * calls memory free method 
        * exits the game 
* `static void faLocation(char* parameters[], void* g);`
    * *Use*: here in case error occurs. Does nothing 
    * *Psuedocode*: 
        * does nothing 
        * if there is an error comes here instead of seg faulting 
* `static void faClaim(char* parameters[], void* g);`
    * *Use*: here in case error occurs. Does nothing 
    * *Psuedocode*: 
        * does nothing 
        * if there is an error comes here instead of seg faulting 
* `static void faLog(char* parameters[], void* g)`
    * *Use*: here in case error occurs. Does nothing 
    * *Psuedocode*: 
        * does nothing 
        * if there is an error comes here instead of seg faulting 
## Print methods 
* `static void agentPrint(void *arg, const char *key, void *item)`
    * *Use*:  prints all the agents information 
    * *PseudoCode*: 
        * used as a helper method to hashtable 
        * print all the agents information in an organized set up 
* `static void foundkragPrint(void *arg, const char *key, void *item)`
    * *Use*:  prints all the found krags 
    * *PseudoCode*: 
        * used as a helper method to hashtable iterate. 
        * if there is a krag that has been found and has a latitude and longitute, it prints the information assocaited with that krag 
* `static void unfoundkragPrint(void *arg, const char *key, void *item)`
    * *Use*:  prints all the unfound krags 
    * *PseudoCode*: 
        * used as a helper method to hashtable iterate. 
        * if there is a unkrag that has been found and has a latitude and longitute of 0, it prints the information assocaited with that krag 

* `static void endingPrint(FILE *fp, void *item)`
    * *use*: prints the information team is given
    * *psuedocode*:
        * used as a helper method to bag iterate
        * loops through the bag and prints all the information in the team struct in an organzied format 
* `static void gameStructPrint(void* g);`
    * *Use*: prints when the user requests all information reprinted
    * *Psuedocode*: 
        * print all the relative information stored in the game struct
        * Print the game id, agent id, team name, number of krags found, total number of krags, current secret 
        * for each guide agent print their pebble id and location 
        * print a list of all the krags left to find and the krags that have been found 
## delete methods 
* `static void mydeletehash(void *item)`
    * *Use*: deletes all the items in the hash table 
    * *Psuedocode*: 
        * gets the void * item 
        * removes all the items in the hashtable by calling free on them 
* `static void mydeletebag(void *item)`
    * *Use*: deletes all the items in the bag
    * *Psuedocode*: 
        * gets the void * item 
        * removes all the items in the bag by calling free on them 
* `static void mydeletebagend(void *item)`
    * *Use*: deletes all the items in the bag for the bag that keeps all the ending information
    * *Psuedocode*: 
        * Doesnt actually do anything just needed to iterate through the bag. 

### Data Structures 
* Hashtable 
    * Krags stored in a hash table because want to be able to find a krag and the information associated with it. Can do this best with a hash table and store the information with the krag id as the key 
    * Player is also kept in a hashtable because can store the releavnt information in the player struct and look it up with a key which is the player ID 
* Bag
    * Hints use a bag because we just need to store the char* and iterate through it to print htem out. Easy to add and iterate through a bag 
* Agent Struct 
    * Used to store information about an agent on the team     
    * *Variables:* char* name, hex pebbleID, Int Latitude, Int Longitude 
* GameOver struct 
    * Used to store the team information the server gives the game agent at the end of the game 
    * *variables:* char* gameID, char* team, int numplayers, int numclaimed 
* Krag Struct 
    * Used to keep track of Krags and their relative information 
    * *Variables:* char* kragID, int latitude, int longitude, clue, char* pebble ID of person who found them
* Function Struct 
    * Use: Store the functions for the different op codes in this table array so can use them and look them up easily 
    * *Variables:* char* command, coid (*func)(Array Of Tokens)  
    * arrayOfFuncs[] = { {OpCode, FunctionName} {OpCode, FunctionName} }
* GameStruct 
    * Used to keep track of all releveant game data  
    * *Variables*: Hex GameID, Hex GuideID, char* TeamName, int TotalKrags, char* Secret, int ClaimedKrags, int LastContacted, int Update, char* PlayerName 
    * Other Data Structures contained 
        * Bag char* Hints
        * Bag char* GameOver 
        * HashTable Krags Krags
        * Hahtable Agent Agent, 

### Storage, Files, and Messages 
* Each received and sent message is written into the Guide Agent Log File. 
* The Log file will be opened in the WriteToFile method and closed in that as well. 
* Error messages will be printed to console if response code is received 
* Program exits with non zero status if any error occurs with initilizeing standard arguments or opening the log file 

### Memory and Resource Management 
* Log file is only file and this will be opened and closed within the write to file message so no memory is leaked
* After the game over op code has been recieved, the game struct and all of its compenents such as the Krags hash table, Agent Hashtable, and hints bag will be freed. 
* Each string that is malloced to send to the game server is freed after it is print 



## GAME SERVER

### Data structures
**Struct _game_info_**
- Time the game has started (indicates the elapsed time since start of the game)
- Number of krags (int)
- Number of agents (#FA + #GA)
- Number of teams (# of Struct Team)
- Secret string (char *)
- Set of krags (clue will be the key)
- Set of Team (team name will be the key)
- Game ID (unsigned int)
- Game status (int) (0 indicates the game is ongoing)

**Struct _Team_**
- Team name (cahr *)
- Number of agents int team (#FA + #GA)
- GA
- Set of FA
- Number of krags claimed (int)
- Number of revealed krags (int)
- Partly revealed secret string (char *)

**Struct _FA_**
- Name (char *)
- pebbleId (int)
- Struct sockaddr_in
- Longitude (float)
- Latitude (float)
- Last-contact-time (time_t)

**Struct _GA_**
- Name (char *)
- guideId (int)
- sockaddr (int)
- Last-contact-time (time_t)

**Struct _krag_**
- Latitude (float)
- Longitude (float)
- kragId (unsigned int)
- Clue (char *)
- Set for keeping track the team that have claimed the krag
- Set for keeping track the team that have revealed the krag


### Pseudo code
#### Methods
**`int main(const int argc, char *argv[])`**
1. Parse command line argument ()
2. While the game is not over, perform the following
    1. Listen to message
    2. If successfully received message:
        1. Write the message in the log file (logs/gameserver.log) () 
        2. Parse message ()
        3. Handle message ()
            1.	Depending on the return value, respond correctly. Explained below.
        4. Present summary
    4. Otherwise exit with non-zero status
    5. If "GAME OVER" is entered in stdin,  game over
    6. If game over (All krags revieled or by input "GAME OVER"), get out of the loop 
    7. Otherwise continue loop
3. Send a game summary (TEAM_RECORD) to all players
4.Send a message indicating the end-of-game (GAME_OVER) to 
5. Free all memory
6. Exit with 0 status

**`int message_handler(char *opcode, char *rest_of_line, game_infor *gi)`**
// Use dispatch table and common function
If opCode=
**FA_LOCATION**
1. Validate the message fields; ignore the invalid messages
2. If gameId == 0
    1. If team is not known, register team
    2. If pebbleId is not known, register pebbleId and associate it with given player and team
    3. If there is already a player with the same name in the team, ignore and return -8
        **Same name is not allowed in one team**
    4. Return 1
3. If gameId != 0
    1. Validate gameId, pebbleId, team, and player
        1. If gameId is incorrect, return -4
        2. If pebbleId is incorrect, return -7
        3. If team is incorrect, return -5
        4. If player is incorrect, return -6
    2. If valid input:
        1. Update records regarding location and last-contact-time of this FA
        2. If statusReq==1, Return 1
        3. Otherwise return 0

 **FA_CLAIM**
1. Validate the message fields; ignore the invalid messages and return -3
2. If kragId is not known; ignore and return -7
3. Confirm the given latitude|longitude is within 10 meters of the known position of the identified krag
    1. Return -1 if latitude|longitude is not within 10 meters of the krag
4. If the krag has not been claimed by this team:
    1. Mark it as ‘claimed’ and send a SH_CLAIMED response to the FA
    2. If this is not the last krag to be claimed, return 1
     3. If this is the last krag to be claimed, return 2
5. Else if the krag has already has been claimed by this team, return 3

**FA_LOG**
1. Return 0

**GA_STATUS**
1. Validate the message fields; ignore the invalid messaegs and return -3
2. If gameId == 0
    1. If team is not known, register team
    2. If guideId is not known, register guideId and associate it with given player and team names
    3. Else, verify that team matches a known team and associate guideId with given player name
    4. If guide is already in the team; ignore and return -5
    5. If no error return 1
3. If gameId != 0
    1. Validate the gameId, guideId, team, and player name
        1. If gameId is incorrect, return -4
        2. If guideId is incorrect, return -7
        3. If team is incorrect, return -5
        4. If player is incorrect, return -6
    2. If valid message:
        1. Update records regarding last-contact-time of this GA
        2. If statusReq==1, return 1
    3. Otherwise return 0

**GA_HINT**
1. Validate the gameId, guideId, team and player
    1. Is gameId the same with current gameId
        1. If error return -4
    2. Is guideId associated with given team and player
        1. If error return return -7
    3. If valid:
        1. Update records regarding last-contact-time of this GA
        2. If pebbleId is *, return 1
        3. Else if pebbleId is known player in the team, return 2
        4. Else if pebbleId is not known player in the team, return -7

**GAME_STATUS**
1. Return -2

**GS_AGENT**
1. Return -2

**GS_CLUE**
1. Return -2

**GS_CLAIMED**
1. Return -2

**GS_SECRET**
1. Return -2

**GS_RESPONSE**
1. Return -2

**GAME_OVER**
1. Return -2

**TEAM_RECORD**
1. Return -2

**not found**
1. Return -2

**`void respond(char *opCode, int result, int comm_sock, struct sockaddr_in them,
game_info_t *gi, char *message_from)`**
1. If result is < 0, error
    If result = 
        -1, respond with SH_ERROR_INVALID_MESSAGE to whom sent the message
        -2, respond with SH_ERROR_INVALID_OPCODE to whom sent the message
        -3, respond with SH_ERROR_INVALID_FIELD to whom sent the message
        -4, respond with SH_ERROR_INVALID_GAME_ID to whom sent the message
        -5, respond with SH_ERROR_INVALID_TEAMNAME to whom sent the message
        -6, respond with SH_ERROR_INVALID_PLAYERNAME to whom sent the message
        -7, respond with SH_ERROR_INVALID_ID to whom sent the message
        -8, respond with SH_ERROR_DUPLICATE_PLAYERNAME to whom sent the message
        -9, respond with SH_ERROR_DUPLICATE_FIELD to whom sent the message
        -10, respond with SH_CLAIMED to whom sent the message
        -11, respond with SH_CLAIMED_ALREADY to whom sent the message
    Write the message in the log file (logs/gameserver.log)

2. If opCode is
    1. FA_LOCATION
        1. respond with game status
    2. FA_CLAIM
        1. If result = 1
            1. Respond with SH_CLAIMED to whom it sent the message
            2. Respond with GS_CLAIMED to the guide
            3. Respond with maximum two (may be one or none) GS_CLUE to the guide
                1. Check how many clue have been revealed to this team
                2. If this is less than the number of the krag in the game, send clue and increment the count of revealed krags
                3. Do this again if condition satisfies
            4. Respond with GS_SECRET to the guide
        2. If result = 2
            1. Respond with SH_CLAIMED to whom it sent the message
            2. Respond with GS_CLAIMED to the guide
            3. Change the game status and end the game
        3. If result = 3
            1. Respond with SH_CLAIMED_ALREADY to whom sent the message
    3. GA_STATUS
        1. If result = 1
            1. Respond with GAME_STATUS to the guide
            2. Respond with GS_AGENT to the guide
        2. If result = 2
            1. Respond with GAME_STATUS to the guide
            2. Respond with GS_AGENT to the guide
            3. Respond with maximum two (may be one or none) GS_CLUE to the guide
    4. GA_HINT
        1. If result = 1
            1. Send GA_HINT to all field agent in the same team
        2. If result = 2
            1. Send GA_HINT to the specified field agent
    5. FA_LOG
        1. If result = 0
            1. Write to the log file (fieldagents.log)
    Write the message in the log file (logs/gameserver.log)

#### Functions
**`Parse command line argument(const int argc, char *argv[], game_infor *gi)`**
    1. Check the following:
        1. Number of arguments
        2. Valid gameID
        3. Readable and correct format kiff
        4. Readable and correct format sf
        5. Valid port
    2. If any error:
        1. Print message
        2. Exit with non-zero status
    3. Otherwise:
        1. Build set of krags()
        2. Create a DGRAM socket
        3. Bind it to the given port number

**`Build set of krags(char *kiff, game_infor *gi)`**
    1. For each line in the file:
        1. Check there are no space
        2. Create the krag
        3. Add it to the set of krag stored in game_info 


### Returning value operation
**Common**
0: Successfully handled message (can be error and ignore). Nothing to be done
-1: SH_ERROR_INVALID_MESSAGE
-2: SH_ERROR_INVALID_OPCODE
-3: SH_ERROR_INVALID_FIELD
-4: SH_ERROR_INVALID_GAME_ID
-5: SH_ERROR_INVALID_TEAMNAME
-6: SH_ERROR_INVALID_PLAYERNAME
-7: SH_ERROR_INVALID_ID
-8: SH_ERROR_DUPLICATE_FIELD
-9: SH_DUPLICATE_PLAYERNAME
-10: SH_CLAIMED
-11: SH_CLAIMED_ALREADY
-99: malloc error

**FA_LOCATION**
1: Respond with GAME_STATUS

**FA_CLAIM**
1: Respond with SH_CLAIMED, GS_CLUE, and GS_SECRET
    - Send two (may be one or zero) randomly chosen clues, in the form of GS_CLUE messages to the GA on same team
    - Update this team’s copy of the secret so as to reveal characters of the string
    - Send the updated secret, via a GS_SECRET message to the GA on same team
2: Respond with SH_CLAIMED and finish the game
3: Respond with SH_CLAIMED_ALREADY

**GA_STATUS**
1: Respond with GAME_STATUS and GS_AGENT

**GA_HINT**
1: Send the message to all FA in the team
2: Forward the message to the specified player


### Resource management and Persistent storage
* After game is finished successfully, either by opCode or stdin, all memory used for running the game will be freed. Therefore, no data will be left.
* Log file will record all the input message from GA and FA, and output message to GA and FA. Therefore, one can track the game by viewing the log file.
* Any error message will be printed in stdout.



## FIELD AGENT
### Modules

#### field_agent_data
##### Structs
-   ```c
    typedef struct fieldagent_info {
        char *gameId;
        char* pebbleId;
        char* name;
        char* team;
        char* latitude;
        char* longitude;
        char* end_message;
        char* krag_to_submit;
        char** hints_received;

        int num_hints;
        int num_claimed;
        int num_left;
        int time_passed;

        bool game_started;
        bool game_over_received;
        bool wrong_name;
        bool krag_claimed;
        bool krag_claimed_already;
        bool submit_krag;
    } fieldagent_info_t;
    ```

##### Variables
-   `fieldagent_info_t *FA_INFO;`

##### Methods
-   `void create_info();`
    - malloc the char* fields of the struct and the struct itself
    - initialize as many fields as possible

-   `void delete_info();`
    - free the memory associated with the fields of the struct
    - free the struct

-   `void print_FA();`
    - add to the logs certain fields of the FA_INFO


 
#### field_agent
##### Variables
- `static time_t start;`
- `static char* error_message;`
- `static int malloc_size = 200;`
- `char pebbleId_init_string[5] = "init";`

##### Methods
- `int main(void);`
    - Call init, app_event_loop(), and deinit

- `static void init();`
    - Call create_info()
    - malloc the error_message
    - Register our tick_handler function with TickTimerService
    - Push the choose name window
    - Set the start time
    - Set the handlers for AppMessage inbox/outbox events
    - Open the app message communication protocol

- `static void deinit();`
    - Pop all the windows
    - Unsubscribe from the timer
    - Call delete_info() to remove memory associated with the FA_INFO
    - Free the error_message

- `static void tick_handler(struct tm *tick_time, TimeUnits units_changed);`
    - If submit_krag is true, send an FA_CLAIM
    - If game_over_received is true, pop all the windows and push a dialog_message_window with a game over message
    - If wrong_name is true, pop windows, push the choose_name window, and push a dialog_message_window with a error_message
    - If krag_claimed is true, push a dialog_message_window with a error_message message, update the FA_INFO struct
    - request a location update every 10 seconds
    - Every 15 seconds, if the game has started and the pebbleId has been fetched, send a FA_LOCATION, and update the time elapsed

- `static void update_time();`
    -

- `static void inbox_received_callback(DictionaryIterator *iterator, void *context);`
    -

- `static void outbox_sent_callback(DictionaryIterator *iterator, void *context);`
    -

- `static void inbox_dropped_callback(AppMessageResult reason, void *context);`
    -

- `static void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context);`
    -

- `static void request_pebbleId();`
    -

- `static void request_location();`
    -

- `static void send_message(char *message);`
    -




#### choose_name

// File global variables
static Window *s_main_window_choose_name;
static MenuLayer *choose_name_menulayer;

// Variables to be used throughout the menu
static char isaac[6] = "Isaac";
static char morgan[7] = "Morgan";
static char laya[5] = "Laya";
static char kazuma[7] = "Kazuma";
static int s_current_selection = 1; // for choosing the name, start at 1 from the menu

// Setup the radion menu layer to choose a name
//
static uint16_t get_num_rows_callback_choose_name(MenuLayer *menu_layer, uint16_t section_index, void *context);

//
static void draw_row_callback_choose_name(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context);

// 
static int16_t get_cell_height_callback_choose_name(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context);

//
static void select_callback_choose_name(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

// main_window
static void window_load_choose_name(Window *window);

// window_unload_choose_name
static void window_unload_choose_name(Window *window);


void choose_name_window_push();

void main_menu_reload_pass_up();





#### main_menu

static Window *s_main_window_main_menu = NULL;
static MenuLayer *s_menu_layer_main_menu = NULL;
static PinWindow *pin_window;

static int s_current_selection_main_menu = 0;

void pin_window_complete_callback(PIN pin, void *context);

static uint16_t get_num_rows_callback_main_menu(MenuLayer *menu_layer, uint16_t section_index, void *context);

static void draw_row_callback_main_menu(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context);

static int16_t get_cell_height_callback_main_menu(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context);

static void select_callback_main_menu(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static void window_load_main_menu(Window *window);

static void window_unload_main_menu(Window *window);


void main_menu_window_push();

void main_menu_reload();






#### pin_window

typedef struct {
  char digits[PIN_WINDOW_NUM_CELLS + 1];
} PIN;

typedef void (*PinWindowComplete)(PIN pin, void *context);

typedef struct PinWindowCallbacks {
    PinWindowComplete pin_complete;
} PinWindowCallbacks;

typedef struct {
  Window *window;
  TextLayer *main_text, *sub_text;
  Layer *selection;
  GColor highlight_color;
  StatusBarLayer *status;
  PinWindowCallbacks callbacks;

  PIN pin;
  char field_buffs[PIN_WINDOW_NUM_CELLS][2];
  int8_t field_selection;
} PinWindow;


static char* selection_handle_get_text(int index, void *context);

static void selection_handle_complete(void *context);

static void selection_handle_inc(int index, uint8_t clicks, void *context);

static void selection_handle_dec(int index, uint8_t clicks, void *context);


PinWindow* pin_window_create(PinWindowCallbacks callbacks);

void pin_window_destroy(PinWindow *pin_window);

void pin_window_push(PinWindow *pin_window, bool animated);

void pin_window_pop(PinWindow *pin_window, bool animated);

bool pin_window_get_topmost_window(PinWindow *pin_window);

void pin_window_set_highlight_color(PinWindow *pin_window, GColor color);






#### hints_window

static Window *s_main_window_hints_window = NULL;
static MenuLayer *s_menu_layer_hints_window = NULL;
static int s_current_selection_main_menu = 0;


static uint16_t get_num_rows_callback_hints_window(MenuLayer *menu_layer, uint16_t section_index, void *context);

static void draw_row_callback_hints_window(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context);

static int16_t get_cell_height_callback_hints_window(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context);

static void select_callback_hints_window(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static void window_load_hints_window(Window *window);

static void window_unload_hints_window(Window *window);


void hints_window_window_push();




#### message_dialog

static Window *s_window;
static TextLayer *s_text_layer;
static char* message = NULL;


static void window_load(Window *window);

static void window_unload(Window *window);


void dialog_message_window_push(char* input_message);




#### location

typedef struct location_struct {
    char* latitude;
    char* longitude;
} location_t;

location_t *parse_location(char* location_s);



#### message_handler

static char GAME_STATUS[12] = "GAME_STATUS";
static char GS_RESPONSE[12] = "GS_RESPONSE";
static char GAME_OVER[10] = "GAME_OVER";
static char GA_HINT[8] = "GA_HINT";

char* create_fa_location(char* statusReq);

char* create_fa_claim(char* kragId);

char* create_fa_log(char* text);

void incoming_message(char* message);

void message_GAME_OVER(char* message);

void message_GAME_STATUS(char* message);

void message_GS_RESPONSE(char* message);

void message_GA_HINT(char* message);








