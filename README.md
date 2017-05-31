# README for Final Project
### Views, May 2017

## Team name

Views


## Team members

- Morgan Sorbaro
- Isaac Gluck
- Kazuma Honjo


### To Run: type make 

#### Game_Agent Subdirectory: 
##### Usage: 
* guideagent.c -program to control and connect the guide agent to the game server 
* where:
    * given parameters, checks them
    * creates a socket and connects to server
    * messages are passed between server and socket to communicate all parts of the game and update information
* output:
    * guideagent.log full of all messages sent and recieved
    * prints user interface from stdout of all the information relevant to the guide agent 
 * stdin: guideagentID guideagentName team port host
    * inputs can come in any order to run the game 
    * also once game has begun, can type hint,name,hinttext to send hint
    * can also type update to get a status update
    * can type print to print all current game info
* stdout: 
    * prints when different messages are recieved
    * ex prints secret when updated and clues when krags are recieved
    * prints the whole game when "print" is typed
* stderr: error messages 

##### Example command line:
guideagent guideId=0134 team=aqua player=morgan host=flume port=2872

##### Exit status: 
* Exit 0 if all runs smoothly 
* Exit 1 if not the right amount of args
* Exit 2 if gameID is not the right length or has a space
* Exit 3 if the team name has spaces or is not hte right length 
* Exit 4 if the player id has spaces or is not the right length 
* Exit 5 if the server is unable to connect properly 
* Exit 6 if an argument in the command line is NULL
* Exit 7 if a duplicate argument is found 
* Exit 8 if the first status is not sent right 
* Exit 9 if an error is gotten while selecting 
* Exit 10 if error sending stdin 

##### Assumptions:
* User will input hint correctly in the right hint,<name>,<hint text> format 
* User will attempt to enter valid names 
* Log file created from bash script 

##### Non Assumptions:
* User can enter command line in any order
* Server could give a bad string 
* Arguments could be null or duplicated 

##### Limitations:
* Can only deal with some messages
* Hint has to be in the right order 

#### Feild_Agent subdirectory: 
##### Usage: 
* pebble program -pebble is coded to control the watch which communicates with the server and gives information on the feild agents 
* where:
    * lets the user know information about the game status 
    * allows the user to input krag ideas on the pebble watch 
    * communicates game state with the game server about game state and parses and updates data
* inputs:
    * recieves messages from server about game state 
* output:
    * prints the to server messages that relate to the field agents 
 * stdin: none 
* stdout: none

#### Game Server 

##### Example command line:
To build, pebble build
To run emulator, pebble install --emulator basalt --logs;
To run pebble (the file in dev may be different on different systems), pebble install --serial /dev/cu.PebbleTimeAAA3-SerialPo --logs 

##### Exit status: 
* no exit statuses on the pebble 

##### Assumptions:
* server is running on the correct port
* proxy is running on the correct port 
* no 0000 guide agent ID 

##### Non Assumptions:
* if location is not connected, it remains at 00
* if there are duplicate names you must choose another

##### Limitations:
* teams, port, and name choices are hard coded in 
 
##### Usage: 
The Game Server application coordinates one and only one game each time it runs.

- inputs:
    - recieves messages from guide agent and field agent about game state 
- output:
    - interprets message and responds depending on the message
    - all the inputs and outputs are recorded in ../logs/gameserver.log
- stdin: 
    - **GAME OVER** ends the game
- stdout: 
    - prints message when 

To turn on the debugging mode, take away the '#' from Makefile, DEBUG = -DDEBUG

##### Example command line:
To build, run `make`
To run emulator, pebble install --emulator basalt --logs;
To run the program, run `./gameserver gameId kff sf port`

##### Exit status: 
* 0: Successfully finished
* 1: Error during initializing parameters
* 2: Wrong nubmer of command line arguments
* 3: Wrong gameID
* 4: Wrong kff
* 5: Wrong sf
* 6: Wrong port
* 7: Error during setting up UDP/DGRAM
* 8: Error during receiving message through socket
* 99: Malloc failed

##### Assumptions:
* no 0000 game ID
* no 0000 guide agent ID or pebble ID

##### Non Assumptions:
* Once game starts, unless there are malloc error, the game continues.

##### Limitations:
* Because it does not fork for each guide or field agent, once many player connect, the server may not be able to handle all messages.





