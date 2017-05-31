### README for fieldagent.c
#### CS50 Krag Final Project 

To build, run 'pebble build'

The Project link is: http://www.cs.dartmouth.edu/~cs50/Labs/Project/

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
* hints and text dont have equals or pipes 

##### Non Assumptions:
* if location is not connected, it remains at 00
* if there are duplicate names you must choose another

##### Limitations:
* teams, port, and name choices are hard coded in 


