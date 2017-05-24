## GAME AGENT 

### Methods 
#### The Game Agent will have the following methods: 
* `void validateArguments(const in argc, char *argv[])` 
    * *Use*: Validates command lines arguments 
    * *PsuedoCode*:  
        * Takes in Arugments passed to main 
        * Splits them into variables 
        * Make sure the right number of arguments are given 
        * Make sure each input is of the right type and not NULL 
        * Exist with a status of non zero if anything is wrong
* `int ConnectSocket(const int argc, char* argv[], struct sockaddr_in *themp)`
    * *Use*: Creates and connects the Game Agent to the Game Server
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
        * Print the new clue out for the game agent 
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
        * If the player does not exist or the game agent and guide agent is not correct, print error message and exit the method 
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
        * for each game agent print their pebble id and location 
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