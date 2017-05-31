## TESTING.md 
### Unit Testing
#### Guide Agent Unit Testing : 
 
* *Testing input in any order* 
    [msorbaro@moose ~/cs50/views/guide_agent]$ guideagent guideId=0125 team=aqua 
player=morgan host=flume port=2872
    Hello morgan! Welcome to the Game!
    Type 'hint,personHintIsGoingTo,hintText to send a hint to the specified person
    Type 'print' to print out all the information currently associated with the game
 
    [msorbaro@moose ~/cs50/views/guide_agent]$ guideagent guideId=0125 port=1324 team=aqua player=morgan host=flume 
    Hello morgan! Welcome to the Game!
    Type 'hint,personHintIsGoingTo,hintText to send a hint to the specified person
    Type 'print' to print out all the information currently associated with the game
    
    *No matter what order still runs in the same way 
 
2. *Testing bad inputs* 
    [msorbaro@moose ~/cs50/views/guide_agent]$ guideagent guideId=0125 team=aqua 
player=morgan host=flume cow=2872
    Hello morgan! Welcome to the Game!
    Type 'hint,personHintIsGoingTo,hintText to send a hint to the specified person
    Type 'print' to print out all the information currently associated with the game
    To Begin, type 'start' 
    start
    sending in datagram socket: Invalid argument
    *Prints invalid argument instead of seg fauling when cow isnt an argument 
 
3. Testing for Each op code
    * Testing for each op code started by being typed into STDIN 
    * Eventually used the chat server to send methods 
	* Example from each message getting sent shown bellow:
	
	[msorbaro@moose ~/cs50/views/guide_agent]$ guideagent guideId=0707 team=aqua 
player=Alice host=flume port=56307
 
    Hello Alice! Welcome to the Game!
    Type 'hint,personHintIsGoingTo,hintText to send a hint to the specified person
    Type 'print' to print out all the information currently associated with the game
 
    To Begin, type 'start'
    *Start*
    **By typing start I then sent this message**
    **opCode=GAME_STATUS|gameId=FEED|guideId=0707|numClaimed=5|numKrags=8** 

    The gameID is: FEED , The guideID is 0707 , the player name is: Alice,  the Team name is aqua 
    The total num of krags are 8, the total num collected is 5, and the secret is 
    Agents:
    Unfound Krags: 
    Found Krags 
    **Prints the game information so far but not much yet**

    **Using this message added an agent** 
    **opCode=GS_AGENT|gameId=FEED|pebbleId=8080477D|team=aqua|player=Kelsey|latitude=85.706552|longitude=-37.287418|lastContact=98** 
    : agent Kelsey has been added and is at Latitude: 35.706551 Longitude: -67.287415 Last Contact: 58: 
    **prints that kelsey the new agent was added** 
    
    **add two new clues** 
    **opCode=GS_CLUE|gameId=FEED|guideId=0707|kragId=F041|clue=This is another clue** 
    **opCode=GS_CLUE|gameId=FEED|guideId=0707|kragId=07E1|clue=A stone building for religious services, under the third archway.**
    The new clue is: 'This is another clue' 
    : The new clue is: 'A stone building for religious services, under the third archway.' 
    **prints that there are new clues**  
    
    **claim a clue**
    **opCode=GS_CLAIMED|gameId=FEED|guideId=0707|pebbleId=8080477D|kragId=07E1|latitude=43.706552|longitude=-72.287418** 
    : The krag coorsponding to this clue: A stone building for religious services, under the third archway. was claimed at Lat: 43.706551 Long: -72.287415
    **prints that the clue was claimed**
    
    **update the secret because a new clue** 
    **opCode=GS_SECRET|gameId=FEED|guideId=0707|secret=com_____ _cie____50** 
    : The updated secret is now: com_____ _cie____50: : 
    **prints new secret**
    
    **update the agent's location** 
    agent Kelsey has been updated to Latitude: 85.706551 Longitude: -37.287418 Last Contact: 98: : 
    **prints that it was updated**
    
    **accept team ending information** 
    **opCode=TEAM_RECORD|gameId=FEED|team=aqua|numPlayers=3|numClaimed=8**
    **game over**
    **opCode=GAME_OVER|gameId=FEED|secret=computer science 50 rocks!**
    
    GAME OVER
    The final secret is computer science 50 rocks!
    **prints the final output** 
    
    **ask for an update in another example** 
    : *update*
    The gameID is: feed , The guideID is 0125 , the player name is: morgan,  the Team name is aqua 
    The total num of krags are 1, the total num collected is 0, and the secret is 
    Agents:
    Unfound Krags: 
    There is this clue: Hello for an unfound krag
    Found Krags 
    **updates will be sent and the whole thing printed**
    
    **also can type hint which will send a hint to game. More in system testing** 
4. Inputs that just are not a thing 
    Server sends: 
    : [129.170.213.207@34998]: start
    : dog
    : at
    :
    : asdfsdf
    : nothign
    : not valid code
    And nothing happnes with the guide agent, even with just a blank line 
    
#### Field Agent Unit Testing : 
* For each of the windows I tested the different buttons on how they affected the windows.
* For each window with input (choose name, input krag, hints received), I tested clicking on all the different inputs to make sure they are functional.
* I also tested by sending manually different messages, including invalid ones from the game server.
* There are a few cases that are triggered by responses from the Game Server flipping a boolean. I tested these by flipping the booleans internally and seeing the results.
    Code:
    **Test Game Over**
    char secret_buff[200];
    snprintf(secret_buff, sizeof(secret_buff), "Game over!\n The secret was: qwertyuiop");
    strcpy(FA_INFO->known_chars, secret_buff);
    FA_INFO->game_over_received = true;
 
    **Test wrong name**
    FA_INFO->wrong_name = true;
 
    **Test claimed already**
    FA_INFO->krag_claimed_already = true;
 
    **Test claimed**
    FA_INFO->krag_claimed = true;
 

### Integratin Testing: 
* We had two major parts of intergration testing 
* *Testing the guide agent with the server*
    * Had the server and guide agent send eachother the proper methods that were suppose to be send and saw how they responded
    * when the Guide agent joined the game, the server would send a status and the clues
    * When the guide agent sent a hint, the server would receive that hint and get ready to send it to the field agent 
    * when the guide agent sent update, the game server would respond by sending the whole game update again. 
    * When the server sent any message to the guide agent, it would output the correct output and update its internal structures. We tested this by printing the outputs 
* *Testing the field gent with the server* 


### System Testing: 
* We had the guide agent, server, and feild agents all connect together 
* When they all joined, the proper information was sent to the field agents and the guide agent
* We specifically tested sending a hint from the guide agent and making sure that it got to the correct field agent(S) which it did 
* We also tested the feild agent claiming a krag and saw the output displayed on the game agent screen. 
* Overall the systems worked very well together 