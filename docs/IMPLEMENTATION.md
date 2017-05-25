## COMMON 
In the common file we will have the following methods that are useful to everything 
* `int ValidateMessage(char* message)`  
    * *Use:* Make sure the messages are in the right format 
    * check to make sure no punctuation besides | or =
    * check to make sure no spaces
    * If everything is right, return 1 
    * If something is wrong, return 0 
* `int hexStringToDecimal(char* hex)`
    *  *Use*: returns the decimal value of the hex string 
    *  create a place in memory for the int 
    *  use sscanf to convert into decimal 
    *  return decimal
* `char[] messagesToArray(char*  line)`
    *  *Use*: split the message into an array with all the inputs after the = sign 
    *  determine size of arrray by size of = signs 
    *  Locate the | and the = sign 
    *  split the string between these places and put the value in the array 
    *  do this for all portential values
    *  return array 

## GUIDE AGENT 

### Methods 
#### The Guide Agent will have the following methods: 
* `void validateArguments(const in argc, char *argv[])` 
    * *Use*: Validates command lines arguments 
    * *PsuedoCode*:  
        * Takes in Arugments passed to main 
        * Splits them into variables 
        * Make sure the right number of arguments are given 
        * Make sure each input is of the right type and not NULL 
        * Exist with a status of non zero if anything is wrong
* `int ConnectSocket(const int argc, char* argv[], struct sockaddr_in *themp)`
    * *Use*: Creates and connects the Guide Agent to the Game Server
    * *PsuedoCode*: 
      * look up hostname specified by command line
      * Initialize fields of the server adress 
      * Create socket 
      * Return Socket 
* `FILE CreateGameStruct(Gamestruct g, argv[])` 
    * *Use*: Creates game struct, creates log file
    * *Psuedocode*: 
        * Takes all arguments passed in and places them into the game struct using setter methods 
        * Tries to open log file, if it does, returns pointer to log file 
        * If not, exits with a zero status 
    
* `char* SendStatus(GameStruct g)`
    * *Use*: sends the server the current game status in a specified way 
    * *PsuedoCode*: 
        * Retrieves the game Id, Guide ID, team, and player from the Gamestruct 
        * Determines if a status is requested
        * Malloc a string 
        * Does string manipulation to get everything in the right order and format 
        * Returns the string which is the op code to be sent 
* `char* SendHint(GameStruct g)`
    * *Use*: sends a hint to a specific field agent 
    * *PsudoCode*: 
        * Asks user to input who the hint is going to and stores that in a variable
        * Asks the user what the hint is and stores that in a variable 
        * Retrieves the gameID, team
        * Look up peddlieID with the persons name
        * if name does not exist, print "non existant name" 
        * Malloc a string
        * Put the string together in the proper format using string manipulation 
        * return the string to be sent across the server 
* `void DealWithInfo(char* line, GameStruct g)`
    * *Use*: takes in recieved line, prints to file, send arguments next function
    * *PsuedoCode*: 
        * Prints the line to the file 
        * Does string manipulation to determine the op code
        * Parses the rest of the string into an array
        * Send the Array to the right function in the function table Via the Opcode determined
        * If opcode doesnt exist, print error message 
* `void StatusUpdate(char[] arr, GameStruct g)`
    * *Use*: updates the game with opCode status update information 
    * *PsuedoCode*: 
        * array[0] should be game ID so check that this is the same game ID that the game struct has
        * Array[1] is the guide ID so make sure that this is the right ID of the guide via the game struct 
        * Array[2] is the number of claimed krags and update this in the game struct if it is not currently accurate 
        * Array[3] is the number of krags in total and make sure this is also correct 
        * If any data is not correct, print the error message 
* `void AgentUpdate(char[] arr, GameStruct g)`
    * *Use*: updates the agent info with opCode agent information
    * *PsuedoCode*: 
        * Array[0] is the game ID so check that
        * Array[1] is the pebble ID. Look up player and chec that Array[2] (team) and Array[3] (player name) are all correct. 
        * Update the latitute Array[4]
        * Update the longitude Array[5]
        * Update last contacted Array[6] 
* `void Clue(char[] arr, GameStruct g)`
    * *Use*: updates the clue bag
    * *Psuedocode*: 
        * Check Array[0] (game ID) and Array[1] (guide ID) with the game struct 
        * If error print error message and do nothing 
        * Add Krag ID to the hashtable of Krags 
        * Add Clue to the hashtable of Clues 
        * Print the new clue out for the guide agent 
* `void Secret(Char[] arr, GameStruct g);`
    * *Use*: updates the secret 
    * *PsuedoCode*: 
        * Check Array[0] (game ID) and Array[1] (guide ID) to make sure they are correct with the game struct 
        * If not correct, print error message
        * Update the secret in the game struct with Array[2] 
        * Print the new secret message out for the guide agent 
*  `void Response(char[] arr, GameStruct g)`
    * *Use:* looks at response code from server 
    * *Psuedocode:* 
        * Check Array[0] and make sure the Game ID given matches the one in the game struct 
        * Agent will then look at the response code and take recovery method if need be 
*  `void Claimed(char[] arr, GameStruct g)`
    * *Use*: Gives and updates information about the krag claimed 
    * *PsuedoCode:*
        * Check Array[0] (Game ID) and Array[1] (guide ID) with the information from the game struct 
        * look up the player from the hashtable of players using the pebble ID (Array[3])
        * If the player does not exist or the guide agent is not correct, print error message and exit the method 
        * Create a new Krag object by using the kragID (Array[4]) and the lat and long (Array[5] and Array[6]) 
        * Make sure the krag ID does not exist in the hashtable of krags already 
* `char* TeamRecord(char[] arr, GameStruct g)`
    * *Use*: prints end of game team information
    * *psuedoCode*: 
        * validate Array[0] (game id) with the game struct 
        * print array[1] team and then the number of players on the team array[2] and the number of krags claimed array[3]
        * Malloc a string 
        * Store all of these into a string and return that string 
* `void WriteToLog(FILE filename, char* message)`
    * *Use*: writes the char* and the time stamp to the log file
    * *Psuedocode*: 
        * open the file 
        * if the file is not writable exit 
        * cat the message to the end of the file 
        * close the file 
* `void PrintAgentUpdate(GameStruct g, int pebbleID)`
    * *Use*:  prints when an agent is updated
    * *PseudoCode*: 
        * using getter methods, receiver the agent information from looking it up with the pebble ID in the game struct 
        * put all the information into a nicely layed out char 
        * print the char for the guide agent 
* `void PrintClue(GameStruct g, char* clue)`
    * *Use*:  prints when an clue is updated
    * *PseudoCode*: 
        * get the information from the game struct pertaining to the clue 
        * Print the clue in an organized fashion and all other information associated with it from the game struct 
* `void PrintSecret(char* secret)`
    * *use*: prints when the secret is updated
    * *psuedocode*:
        * print the char* secret that is passed out to the console 
* `void MegaPrint(gameStruct g)`
    * *Use*: prints when the user requests all information reprinted
    * *Psuedocode*: 
        * print all the relative information stored in the game struct
        * Print the game id, agent id, team name, number of krags found, total number of krags, current secret 
        * for each guide agent print their pebble id and location 
        * print a list of all the clues recieved
        * print the list of all the hints sent 

### Data Structures 
* Hashtable 
    * Krags stored in a hash table because want to be able to find a krag and the information associated with it. Can do this best with a hash table and store the information with the krag id as the key 
    * Player is also kept in a hashtable because can store the releavnt information in the player struct and look it up with a key which is the player ID 
* Bag
    * Hints use a bag because we just need to store the char* and iterate through it to print htem out. Easy to add and iterate through a bag 
* Agent Struct 
    * Used to store information about an agent on the team     
    * *Variables:* char* name, hex pebbleID, Int Latitude, Int Longitude 
* Krag Struct 
    * Used to keep track of Krags and their relative information 
    * *Variables:* char* kragID, int latitude, int longitude 
* Function Struct 
    * Use: Store the functions for the different op codes in this table array so can use them and look them up easily 
    * *Variables:* char* command, coid (*func)(Array Of Tokens)  
    * arrayOfFuncs[] = { {OpCode, FunctionName} {OpCode, FunctionName} }
* GameStruct 
    * Used to keep track of all releveant game data  
    * *Variables*: Hex GameID, Hex GuideID, char* TeamName, int TotalKrags, char* Secret, int ClaimedKrags, int LastContacted, int Update, char* PlayerName 
    * Other Data Structures contained 
        * Bag char* Hints
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
* Each string that is malloced to send to the game server will be freed after it is print 



## GAME SERVER

### Data structures
**Struct _game_info_**
- Time the game has started (indicates the elapsed time since start of the game)
- Number of krags
- Number of agents (#FA + #GA)
- Number of teams (# of Struct Team)
- Secret file name
- Set of krags (clue will be the key)
- Set of Team (team name will be the key)

**Struct _Team_**
- GA
- Set of FA
- Number of krags claimed (int)
- Revealed krags (set of char *)
- Partly revealed secret string (char *)

**Struct _FA_**
- Name (char *)
- pebbleId (int)
- sockaddr (int)
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
- kragId (int)
- Set for keeping track the team that have claimed the krag


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
    3. Otherwise exit with non-zero status
    4. If game over (All krags revieled or by input GAMEOVER):
        1. Send a game summary (TEAM_RECORD) to all players
        2. Send a message indicating the end-of-game (GAME_OVER) to all players
        3. Break
    5. Otherwise continue loop
3. Free all memory
4. Exit with 0 status

**`int handle_message(char *opcode, char *rest_of_line, game_infor *gi)`**
// Use dispatch table and common function
If opCode=
**FA_LOCATION**
1. Validate the message fields; ignore the invalid messages
2. If gameId == 0
    1. If team is not known, register team
    2. If pebbleId is not known, register pebbleId and associate it with given player and team
    3. If there is already a player with the same name in the team, ignore and return -6
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

**`void send message to(char *responseCode, char *name, char *team, game_infor *gi)`**
    If responseCode = 
        GAME_STATUS (sends to GA)
            opCode=GAME_STATUS|gameId=|guideId=|numClaimed=|numKrags=
            Write the message in the log file (logs/gameserver.log)

        GS_AGENT (sends to GA)
            opCode=GA_AGENT|gameId=|pebbleId=|team=|player=|latitude=|longitude=|lastContact=
            Write the message in the log file (logs/gameserver.log)

        GS_CLUE (sends to GA)  
            opCode=GS_CLUE|gameId=|guideId=|latitude=|longitude=|clue=
            Write the message in the log file (logs/gameserver.log)

        GS_SECRET (sends to GA)
            opCode=GS_SECRET|gmaId=|guideId=|secret=
            Write the message in the log file (logs/gameserver.log)

        GS_RESPONSE (sends to GA/FA)
            opCode=GS_RESPONSE|gameId=|respCode=|test=
            Write the message in the log file (logs/gameserver.log)

        GAME_OVER (sends to GA/FA)
            opCode=GAME_OVER|gameId=|secret=
            Write the message in the log file (logs/gameserver.log)

        TEAM_RECORD (sends to GA/FA)
            opCode=TEAM_RECORD|gameId=|team=|numClaimed=|numPlayers=
            Write the message in the log file (logs/gameserver.log)


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
### Methods

- `int main(void);`
    - *Pseudocode*
        - Calls init, app_event_loop(), and deinit 

- `static void init();`
    - *Pseudocode*
        - Create main Window element.
        - Set handlers to manage the elements inside the window.
        - Register our tick_handler function with TickTimerService.
        - Show the Window on the watch, with animated=true.
        - Choose name window is displayed from the start.
        - Set the handlers for AppMessage inbox/outbox events. Set these handlers BEFORE calling open, otherwise you might miss a message.
        - open the app message communication protocol. Request as much space as possible, because our messages can be quite large at times.
        - setup the FA_INFO `fieldagent_info_t` struct

- `static void deinit();`
	- *Pseudocode*
        - Destroy the window, unsubscribe from sensors, and free memory.

- `static void main_window_load(Window *window);`
	- *Pseudocode*
        - Setup the choose name window and add the menu layer.

- `static void main_window_unload(Window *window);`
	- *Pseudocode*
        - Destroy the menu layer.

- `static void tick_handler(struct tm *tick_time, TimeUnits units_changed);`
	- *Pseudocode*
        - Send a FA_LOCATION every 15 seconds
        - Request location every 10 seconds
        - Check for messages every 5 seconds

- `static void update_time();`
	- *Pseudocode*
        - update the time and display it

- `static void inbox_received_callback(DictionaryIterator *iterator, void *context);`
	- *Pseudocode*
        - check the dictionary iterator to see if a message was received
        - call the correct function depending on if there's a message

- `static void outbox_sent_callback(DictionaryIterator *iterator, void *context);`
	- *Pseudocode*
        - log the message sent

- `static void inbox_dropped_callback(AppMessageResult reason, void *context);`
	- *Pseudocode*
        - log the incoming message dropped

- `static void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context);`
	- *Pseudocode*
        - log the failed to send message

- `static void request_pebbleId();`
	- *Pseudocode*
        - prepare the outbox
        - send a request pebble ID message
        - log errors and success messages

- `static void request_location();`
	- *Pseudocode*
        - prepare the outbox
        - send a request location message
        - log errors and success messages

- `static void send_message(char *message);`
	- *Pseudocode*
        - prepare the outbox
        - send the message provided
        - log errors and success messages

- `uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context);`
	- *Pseudocode*
        - return the number of rows in the choose name menu

- `void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context);`
	- *Pseudocode*
        - draw the rows of the choose name menu - the four names to choose from and the join game text

- `int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context);`
    - *Pseudocode*
        - return the height of each row in the choose name menu

- `void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);`
	- *Pseudocode*
           -  Called when the user joins the game
           -  Sets the name they chose in the FA_INFO struct
           -  switches to the main game screen window
           -  sends a FA_LOCATION message to let the server know the FA joined the game

- `void send_FA_LOCATION();`
	- *Pseudocode*
        -  create and validate the FA_LOCATION message and send it

- `void send_FA_CALIM();`
	- *Pseudocode*
        -  create and validate the FA_CALIM message and send it

- `void send_FA_LOG();`
	- *Pseudocode*
        -  create and validate the FA_LOG message and send it








