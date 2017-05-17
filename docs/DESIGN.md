# Views From The 6 - Final Project
### Kazuma Honjo, Morgan Sorbaro, Isaac Gluck


## Major Design Decisions:
* Team: views6
* Players
    * Isaac
    * Kazuma
    * Morgan
    * Laya
* Proxy Server Port: 2144
* Game Server Port: 2872
* Modules
    * field-agent
    * guide-agent
    * guide-server


## Common file Functions
* hex_to_int (char *hex);
    returns int

* int _to_hex (int);
    returns char *

* parse_message(char *message);
    Check the format of the message. 
        *alphabet=alphabet[|alphabet=alphabet]*
    No space
    If valid input: return char ** ( [0]: opCode [1]: string without opCode )
    If any error return NULL

* write_message_to_logfile(char *message, char *filePathName, int IP, int port, int mode);
    Where:
        Message is the message to be written
        filePathName is the path to the log file
        IP is the IP address to be shown; -1 if Mode=2
        Port is the port number; -1 if Mode=2
        Mode: 
            0 if the message is "From" someone
            1 if the message is "To" someone
            2 if the message is shown as "Note"
        Print out the message following the log file format

* Checks message syntax
* Validates opCode type
* socket-parse (used by GA, and GS)


## Group Member Roles:
1. Game Server - Kazuma Honjo
2. Game Agent - Morgan Sorbaro
3. Field Agent - Isaac Gluck




## Game Agent

### Inputs and Outputs

* -Command line: ./guideagent guideId=... team=... player=... host=... port=...
* ./guideagent = name of program
    *  guideId=... = hexadecimal ID num for this player
    *  team=... = name of team to which the agent belongs 
    *  player=... = provides name of the guide agent
    * host=... = provides the host name of the Game Server 
    * port=... = provides the port number of the Game Server 
* Input from STDIN:
    * Hint and who it should be sent to
    * "Update" requesting the server send a game update
    * "Print" which requests all the data be printed 
* Output To STDOUT: 
    * Current game statistics 
    * Current location and status of each player on Guide’s team
    * Current secret string 
    * List of known clues 
    * More Details in Print Function Description 
* Receive From Server:
    * Updates about current locations and status of players on Guide’s team
    * Clues about the location of krags 
    * Receive updates about the hacker’s secret 
    * Receive updates about game status 
* Send To Server: 
    * Hints to go to Field Agents 
    * Status Request 
* Logfile:
    * Log all activity to logfile in specific format 

### Structs:
* GameStruct
    * Variables: Hex GameID, Hex GuideID, char* TeamName, int TotalKrags, char* Secret, int ClaimedKrags, int LastContacted, int Update, Bag char* Hints, Bag Clue Clues, Bag Agent Agent, char* PlayerName 
    * Methods:
        * new_GameStruct(GameID, GuideID, Teamname, #Krags)
        * getAllHints(GameStruct)
        * NewHint(GameStruct, Hint)
        *  GetAllClaimedKrags(GameStruct) 
        * IncrementClaimedKrags(GameStruct)
        * GetLastContactTime(GameStruct)
        * ClueIterate(GameStruct, IterateMethod)
        * AddAClue(GameStruct, Char*, Lat, Long) 
        * GetSecret(GameStruct) 
        * UpdateSecret(GameStruct, Secret)
        * PlayersIterate*GameStruct, IterateMethod)
        * PlayerUpdate(GameStruct, Name, ID, Lat, Long) 
* Player Struct
    * Variables: char* name, hex pebbleID, Int Latitude, Int Longitude 
    * Methods: 
        * new_player(name, pebID, Lat, Long) 
        * GetName(player)
        * GetID(player)
        * GetLat(player)
        * GetLong(player)
        * SetLat(player, latitude)	
        * SetLong(player, longitude)
*Clue Struct 
    * Variables: char* clue, int latitude, int longitude 
    * Methods:
        * new_clue(clue, lat, long)
        * getClue(clue)
        * getLat(clue)
        * getLong(clue)
        * SetLat(clue, lat)
        * SetLong(clue, long) 
* Function Struct
    * Variables: char* command, coid (*func)(Array Of Tokens) 
     * arrayOfFuncs[] = { {OpCode, FunctionName} {OpCode, FunctionName} } 

### Functions as Modules
* ValidateArguments(), validates command lines arguments * ConnectSocket(), creates and connects the Game Agent to the Game Server
* CreateGameStruct(), creates game struct, creates log file
* SendStatus(), sends the server the current game status in a specified way 
* SendHint(), sends a hint to a specific field agent 
* DealWithInfo(), takes in recieved line, prints to file, send arguments next function
* StatusUpdate(), updates the game with opCode status update information 
* AgentUpdate(), updates the agent info with opCode agent information
* Clue(), updates the clue bag
* Secret(), updates the secret 
* Response(), looks at response code from server 
* TeamRecordGameOver(), prints end of game information 
* WriteToLog(), writes the char* and the time stamp to the log file
* PrintAgentUpdate(), prints when an agent is updated
* PrintClue(), prints when the clue is updated
* PrintSecret(), prints when the secret is updated
* MegaPrint(), prints when the user requests all information reprinted 
 

### Psuedo Code:
Validate Arguments ()
* Check for 6 Arguments
* Put arguments into variables 
* Make sure none are NULL
Connect Socket()
* Use socket given code 
Create Game Struct() 
* GameID = 0
* GuideID = Args given 
* TeamName = Args Given 
* PlayerName = Args Given 
* #krags = 0
* Call new_GameStruct() w args 
* Check Log Dirr & GALog is Readable 
SendStatus()
* Get: GameID, GuideID, team, PlayerName
* Get: Struct-> update 
* If(update == 1): Update = 0
* Format Message via Requirement spec 
* Send to server 
While(true){
* if(timer time has been reached)
** SendStatus()
* if(stdin is ready)
** if(stdn == "update") struct-> update = 1 
** else if(stdin == "print") Megaprint() 
** else SendHint() 
* if(message Received) 
** writeToLog() 
** DealWithInfo() 
} 

SendHint()
* Get: GameID, GuideID, team, PlayerName
* ReadLine to get STDIN
** Ex line: To:Name Hint:WalkLeft 
* Lookup name’s pebble Id with bag
* The rest of the line is the hint 
* Format Data into Messages 
* Send to the Server 

Deal With InFo() 
* Takes the whole received message 
* Prints it to the log with Log Print 
* Break into Tokens Via Pipes 
* Using Function Table, match the command code with the function 
* If Op Code doesnt exist, do nthing 
* Specific Functions per OpCodes 

StatusUpdate()
* Parse token to get GameID, GuideID, #Claimed, #Krag 
* Check all Info 
* If Valid
** If #claimed > curr #claimed - curr # claimed = claimed 
* If not valid, ignore 

AgentUpdate()
* Parse tokens 
* Check info and make sure it is all valid 
* If Player Exists, use update to update player info 
* If player doesnt exist, create a new player 
* Printer Agent Update 

Clue()
* Parse Tokesn 
* Check Info
* If Valid
** If bag already has clue, ignore 
* Else 
** new_clue(clue, lat, long)
** Add Clue to Bag of Clues 
** Print Clue() 
* If not valid, ignore 

Secret()
* Parse tokens
* Validate arguments 
* If Valid
** Curr secret = secret 
** printSecret()

Response()
* Optionally take recovery action
* Print issue to STDOUT for debugging 

TeamRecord()
* Parse tokes
* Check Variables 
* Print info to STDOUt

GameOver()
* Check Variables 
* Prints Secret to STDOUT 

Print Methods
* Print Agent Update() -Print Agent Info to STDOUT
* Print Clue() - Print new clue to STDOUt
* Print Secret() -Print updated Secret to STDOUT 
* MegaPrint() - Print all info in the following Layout with get/set methods 
Team Name: __ GudieAgent: __ Krags Found: __
Field Agent Name: __ Latitude: __ Longitude: __ 
Field Agent Name: __ Latitude: __ Longitude: __ 
Field Agent Name: __ Latitude: __ Longitude: __ 
Secret Message Currently: ___
Past Clues: Clue1, Latitude, Longitude 
Clue2, Latitude, Longitude 
Given Hints: Hint:__ To:__ 
Hint:__ To:__

### Testing
* Test Program with command line inputs that are wrong 
* Test Program with a bad Write Log File Directory 
* Test Program with bad STDIN
* Test Program with just end for STDIN
* Test Program with bad Strings in various forms from the server 




##Game Server
####UI	
N/A 

####Inputs and Outputs
Command-line 
./gameserver gameID=... kiff=... sf=... port=...
Where:
- gmaeId provides the hexadecimal ID number of this game
- kiff provides the path to the krag file, which contains coordinates and clues for each krag
- sf provides the path to the secret file, which contains a single line of text representing the secret.
- Port provides the port number of the Game Server
Guide Agent
Inputs
- Messages (See below)
Outputs
- Messages (See below)
Field Agent
Inputs
- Messages (See below)
Outputs
- Messages (See below)

####Functional decomposition into modules
Main
Handle message

####Dataflow through modules;
*Handle message* takes char *opCode and char *message(without the opCode). By using the dispatch table, it performs what opCode and message specifies. Returns nothing.

####Major data structures;
Local Global Variable
Is_game_ongoing
- 1: Yes
- 0: No

Struct game_info
- Time the game has started (indicates the elapsed time since start of the game)
- Number of krags
- Number of agents (#FA + #GA)
- Number of teams (# of Struct Team)
- Secret file name
- Bag of krags

Struct Team
- Name of guide (char *)
- Name and location of each field agent (list of FA, struct FA **)
- Number of krags claimed (int)
- Partly revealed secret string (char *)

Struct FA (FA_t)
- Name (char *)
- pebbleId (int)
- Longitude (float)
- Latitude (float)
- Last-contact-time (time_t)

Struct GA (GA_t)
- Name (char *)
- guideId (int)
- Last-contact-time (time_t)

Struct krag
- Latitude (float)
- Longitude (float)
- kragId (int)
- Clue (char *)

####Pseudo code
1. Parse command line argument ()
2. While the game is not over, perform the following
1. Listen to message
2. If successfully received message:
1. Write the message in the log file (logs/gameserver.log) () 
2. Parse message ()
3. Handle message ()
4. Present summary
3. Otherwise ignore
4. If game over:
1. Send a game summary (TEAM_RECORD) to all players
2. Send a message indicating the end-of-game (GAME_OVER) to all players
3. Break
5. Otherwise continue loop
3. Free all memory
4. Exit with 0 status

**Functions**
Parse command line argument(){
If any error:
Print message
Exit with non-zero status
Otherwise:
Create a DGRAM socket
Bind it to the given port number
Build bag of krags
}

handle_message(){ // Use dispatch table and common function
If opCode=
FA_LOCATION
Validate the message fields; ignore the invalid messages
If gameId == 0
If team is not known, register team
If pebbleId is not known, register pebbleId and associate it with given player and team
If there is already a player with the same name in the team, ignore
Respond with GAME_STATUS
If gameId != 0
Validate gameId, pabbleId, team, and player
Is gameId the same with current gameId
Is pebbleId associated with given team and player
If valid input:
Update records regarding location and last-contact-time of this FA
If statusReq==1, send a GAME_STATUS to FA
Otherwise; ignore

FA_CLAIM
Validate the message fields; ignore the invalid messages
If kragId is not known; ignore
Confirm the given latitude|longitude is within 10 meters of the known position of the identified krag
If the krag has not been claimed by this team:
Mark it as ‘claimed’ and send a SH_CLAIMED response to the FA
Send two (may be one or zero) randomly chosen clues, in the form of GS_CLUE messages to the GA on same team
Update this team’s copy of the secret so as to reveal characters of the string
Send the updated secret, via a GS_SECRET message to the GA on same team
Else if the krag has already has been claimed by this team, send a SH_CLAIMED_ALREADY response to the FA

FA_LOG  
Ignore

GA_STATUS
Validate the message fields; ignore the invalid messaegs
If gameId == 0
If team is not known, register team
If guideId is not known, register guideId and associate it with given player and team names
Else, verify that team matches a known team and associate guideId with given player name
If guide is already in the team; ignore
If no error respond with GAME_STATUS and possibly GS_AGENT
If gameId != 0
Validate the gameId, guideId, team, and player name
Is gameId the same with current gameId
Is guideId associated with given team and player
If valid message:
Update records regarding last-contact-time of this GA
If statusReq==1, send a GAME_STATUS and GS_AGENT message back to GA

GA_HINT
Validate the gameId, guideId, team and player
Is gameId the same with current gameId
Is guideId associated with given team and player
If valid:
If pebbleId is *, send the message to all FA in the team
Else if pebbleId is known player in the team, forward the message to that palyer
Update records regarding last-contact-time of this GA
Otherwise, respond with response code and ignore

FA_LOG
Write the message in the log file (logs/fieldagents.log)

NULL
Respond with SH_ERROR_INVALID_MESSAGE

not found
Respond with SH_ERROR_INVALID_OPCODE
}

void send_ga_status(to who){
Send the GA_STATUS and GS_AGENT to the given person
}

bool validate_agent (int gameId, char *agentId, char *team, char *playername){
Is gameId the same with current gameId
Is pebbleId/guideId associated with given team and player name
}

####Messages to be sent (Use dispatch table)

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

####Testing plan, including unit tests, integration tests, system tests.

**Unit testing**
* When unit testing, do not send message to anyone
* Test each functions
* Check the logfile and see if the output is as expected
* Test various edge cases
* When there are no Krag (empty krag file)
* When the format of the files are incorrect
* Invalid messages

**Integrated testing**
* Handle all messages
* Send proper messages depending on input message
* When the team start without the GA (must work correctly)
* If the krag is found, it must be recorded
* If the GA joins after some krags are revealed, it should be reflected.





## Field Agent
### UI
- Dropdown to choose a name
- Join Game? Yes/No (GS port hard coded)
- In game screen
- Name
- Team
- Time since game started
- Latitude
- Longitude
- KRAGS Left
- Decoded as of now
- Hints you’ve received (NEW SCREEN)
- Clues you’ve received (NEW SCREEN)
- Input box to enter 4-digit hex KRAG code (NEW SCREEN)
- When you receive a hint/clue: buzzes and shows hint/clue for 10 seconds
- Game over screen


### Inputs & Outputs
#### Inputs
- Choose a name from a presented list
- Join the game with the selected name
- Get hints from the GA (through the GS) and display them
- Enter a KRAG code when they find one by entering its 4-digit hex code
- Receive a game status update from the GS (upon request)
- Receive messages from the GS


#### Outputs
- Send the Game Server the player’s current location four times per minute
- The first location-update message is how it introduces itself to the GS
- Inform the Game Server when this player claims a krag (by providing a 4-digit hex code)
- Write to the logs with all activities


### Functional Decomposition Into Modules
- Main
    - setup, attempt to connect to server, choose name screen, join game screen, and game over screen
- Base Screen
    - main screen user will interact with, as well as the 5 other minor screens (hints, clues, KRAG input)
- Message Handler
    - deals with messages it receives from GS, sends messages to GS
- Pebble Subscriptions: Timer
 

### Dataflow Through Modules
1. The pebble displays the UI, has the user choose a name and join the game
2. The game tries to send the FA_LOCATION to the server to join (if it can’t contact the server the game is not joined)
3. The user now sees the Base Screen where they can see information about themselves, see clues and hints they’ve received, and enter a KRAG code
    1. Receive and display hints and clues
    2. Enter KRAG codes and show whether it was accepted or if it failed
4. Upon receiving a GAME_OVER, clean up and exit

### Major Data Structures
#### Struct FA
- `hex PebbleID`
- `char *name`
- `char *team`
- `signed double latitude`
- `signed double longitude`
- `int num_claimed`
- `int num_left`
- `char *known_chars` (from entering KRAG codes)
- `Bag *clues_received`
- `Bag *hints_received`

### Pseudo Code for Logic/Algorithmic Flow
- Subscribe to Pebble funcs
- Setup the windows
- Start a timer
- Setup the bluetooth inbox and outboxes
- App Event Loop
    - Choose a name from 2-4 options
    - Team name hard coded (view6)
    - Join the game by sending a (FA_LOCATION) indirectly to the GS over the proxy with the GS info (server and port name)
    - Start displaying game data
    - While it hasn’t received a GAME_OVER
        - If it’s a quarter of a minute
            - send an FA_LOCATION to the GS
            - send an FA_LOG to the GS
        - If the GS is sending a GA_HINT
            - alert the user of the notification (maybe a vibration)
            - display the hint
            - send an FA_LOG to the GS
        - If the GS is sending a GS_CLUE
            - alert the user of the notification (maybe a vibration)
            - display the clue
            - send an FA_LOG to the GS
        - If the GS is sending a GS_SECRET
            - alert the user of the notification (maybe a vibration)
            - display the secret with some characters revealed
            - send an FA_LOG to the GS
        - If the GS is sending a GAME_OVER
            - alert the user of the notification (maybe a vibration)
            - display the game status at the end and that the game is ending
            - send an FA_LOG to the GS
            - exit the loop and end the game
       - If the GS is sending a GS_RESPONSE
            - If the GS is sending an SH_CLAIMED
                - alert the user of the notification (maybe a vibration)
                - display a success message
                - send an FA_LOG to the GS
            - If the GS is sending an SH_CLAIMED_ALREADY
                - alert the user of the notification (maybe a vibration)
                - display a failed message
                - send an FA_LOG to the GS
            - If the GS is sending any type of SH_ERROR
                - Call a function errorHandle() to deal with errors
            - If the user enters a KRAG code
                - send a FA_CLAIM to the GS
                - send an FA_LOG to the GS

### Test Plan
- Test on the emulator and the pebble
- Test when bluetooth connection cannot be established
- Test with various user entered KRAG codes
- Test with various GS messages, including ill formatted and non-existant ones
- Test individual methods separately with bad input
- Check messages sent
- Check the logs to make sure logging is done correctly
- Test the UI going through all combinations of buttons and selections

