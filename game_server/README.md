### README for fieldagent.c
#### CS50 Krag Final Project 

To build, run `make`
To run the program, run `./gameserver gameId kff sf port`
Where
    - `gameId` provides the hexadecimal ID number of this game
    - `kff` provides the path to the krag file, which contains coordinates and clues for each krag
    - `sf` provides the path to the secret file, which contains a single line of text representing the secret
    - `port` provides the port number of the Game Server

The Project link is: http://www.cs.dartmouth.edu/~cs50/Labs/Project/

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

