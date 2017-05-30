/*
 * gs_response_handler - a function tables used for gameserver
 *
 * Functions for response based on received message from agent or guide agent
 *
 * If result = integer: Command
 *
 * Common Returning value
 * 0: Successfully handled message, and nothing to be done
 * -1: SH_ERROR_INVALID_MESSAGE
 * -2: SH_ERROR_INVALID_OPCODE
 * -3: SH_ERROR_INVALID_FIELD
 * -4: SH_ERROR_INVALID_GAME_ID
 * -5: SH_ERROR_INVALID_TEAMNAME
 * -6: SH_ERROR_INVALID_PLAYERNAME
 * -7: SH_ERROR_INVALID_ID
 * -8: SH_DUPLICATE_PLAYERNAME
 * -9: SH_ERROR_DUPLICATE_FIELD
 * -99: malloc error
 *
 * FA_LOCATION specific returning value
 * 1: Respond with GAME_STATUS
 *
 * FA_CLAIM specific returning value
 * 1: Respond with SH_CLAIMED, GS_CLUE, and GS_SECRET
 * 2: Respond with SH_CLAIMED and finish the game
 * 3: Respond with SH_CLAIMED_ALREADY
 *
 * GA_STATUS specific returning value
 * 1: Respond with GAME_STATUS and GS_AGENT
 * 2: Respond with GAME_STATUS, GS_AGENT, and reveal two clue
 *
 * GA_HINT specific returning value
 * 1: Send the message to all FA in the team
 * 2: Forward the message to the specified player
 *
 * Kazuma Honjo, May 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#include "file.h"
#include "set.h"
#include "common.h"
#include "shared.h"
#include "gsstruct.h"
#include "gs_response_handler.h"

/**************** file-local global variables ****************/
static const int MESSAGE_LENGTH = 8192;

static void handle_result_message(int result, int comm_sock, struct sockaddr_in them,
                                  game_info_t *gi, char *message_from);
static void respond_with_game_status(int comm_sock, struct sockaddr_in them,
                                     game_info_t *gi, char *message_from);
static void respond_with_gs_clue(int comm_sock, struct sockaddr_in them,
                                 game_info_t *gi, char *message_from);
static void respond_with_gs_secret(int comm_sock, struct sockaddr_in them,
                                   game_info_t *gi, char *message_from);
static void respond_with_gs_agent(int comm_sock, struct sockaddr_in them,
                                  game_info_t *gi, char *message_from);
static void respond_with_gs_claimed(int comm_sock, struct sockaddr_in them,
                                    game_info_t *gi, char *message_from);
static void send_gs_agent(void *arg, const char *key, void *item);
static void send_hint_to_everyone_in_team(int comm_sock, struct sockaddr_in them,
                                          game_info_t *gi, char *message_from);
static void send_gs_clue(int comm_sock, game_info_t *gi, char *message_from, krag_t *krag);
static void respond_with_ga_hint(int comm_sock, struct sockaddr_in them,
                                 char *message_from, game_info_t *gi);
static void send_game_over_to_everyone(void *arg, const char *key, void *item);
static void send_team_record_to_everyone(void *arg, const char *key, void *item);
static team_t *get_team(char *message_from, game_info_t *gi);
static char *get_token(char *message, char *left_hand_side);


/**************** respond ****************/
/* respond based on the resulting value of message handler function
 */
void
respond(char *opCode, int result, int comm_sock, struct sockaddr_in them,
        game_info_t *gi, char *message_from){
    if (opCode == NULL || gi == NULL || message_from == NULL){
        return;
    }
    
    #ifdef DEBUG
    printf("Result value is: %d\n", result);
    #endif
    
    if (result < 0){
        handle_result_message(result, comm_sock, them, gi, message_from);
    }
    
    if (strcmp(opCode, "FA_LOCATION") == 0){
        if (result == 1){
            respond_with_game_status(comm_sock, them, gi, message_from);
        }
    }
    else if (strcmp(opCode, "FA_CLAIM") == 0){
        if (result == 1){
            handle_result_message(-10, comm_sock, them, gi, message_from);
            respond_with_gs_clue(comm_sock, them, gi, message_from);
            respond_with_gs_claimed(comm_sock, them, gi, message_from);
            respond_with_gs_secret(comm_sock, them, gi, message_from);
        }
        else if (result == 2){
            handle_result_message(-10, comm_sock, them, gi, message_from);
            respond_with_gs_claimed(comm_sock, them, gi, message_from);
            game_info_change_game_status(gi);
        }
        else if (result == 3){
            handle_result_message(-11, comm_sock, them, gi, message_from);
        }
    }
    else if (strcmp(opCode, "GA_STATUS") == 0){
        if (result == 1){
            respond_with_game_status(comm_sock, them, gi, message_from);
            respond_with_gs_agent(comm_sock, them, gi, message_from);
        }
        else if (result == 2){
            respond_with_game_status(comm_sock, them, gi, message_from);
            respond_with_gs_agent(comm_sock, them, gi, message_from);
            respond_with_gs_clue(comm_sock, them, gi, message_from);
        }
    }
    else if (strcmp(opCode, "GA_HINT") == 0){
        if (result == 1){
            send_hint_to_everyone_in_team(comm_sock, them, gi, message_from);
        }
        else if (result == 2){
            respond_with_ga_hint(comm_sock, them, message_from, gi);
        }
    }
    else if (strcmp(opCode, "FA_LOG") == 0){
        if (result == 0){
            // write to log file
            char *ipaddress = getIP(comm_sock, them); //get the IP Adress
            print_log(message_from, "fieldagents.log", ipaddress, "FA"); //print it to the log file
            free(ipaddress);
        }
    }
}


/**************** handle_result_message ****************/
/* handle the result messages for result < 0
 */
static void
handle_result_message(int result, int comm_sock, struct sockaddr_in them,
                      game_info_t *gi, char *message_from){
    if (gi == NULL || message_from == NULL){
        return;
    }
    // if malloc error, exit
    if (result == -99){
        game_info_delete(gi);
        exit(99);
    }
    
    // initialize parameters
    char *message = malloc(MESSAGE_LENGTH);
    char *gameId = game_info_get_gameId(gi);
    if (message == NULL || gameId == NULL){
        if (message != NULL)free(message);
        if (gameId != NULL)free(gameId);
    }
    
    
    strcpy(message,"opCode=GS_RESPONSE|gameId=");
    int i = strlen(message);
    strcpy(&(message[i]), gameId);
    i = strlen(message);
    strcpy(&(message[i]), "|respCode=");
    i = strlen(message);
    if (result == -1){
        strcpy(&(message[i]), "SH_ERROR_INVALID_MESSAGE|text=");
        i = strlen(message);
        strcpy(&(message[i]), message_from);
    }
    else if (result == -2){
        strcpy(&(message[i]), "SH_ERROR_INVALID_OPCODE|text=opCode: ");
        i = strlen(message);
        char *token = get_token(message_from, "opCode");
        strcpy(&(message[i]), token);
        free(token);
    }
    else if (result == -3){
        strcpy(&(message[i]), "SH_ERROR_INVALID_FIELD|text=Invalid field exists.");
    }
    else if (result == -4){
        strcpy(&(message[i]), "SH_ERROR_INVALID_GAME_ID|text=gameId: ");
        i = strlen(message);
        char *token = get_token(message_from, "gameId");
        strcpy(&(message[i]), token);
        free(token);
    }
    else if (result == -5){
        strcpy(&(message[i]), "SH_ERROR_INVALID_TEAMNAME|text=team name:");
        i = strlen(message);
        char *token = get_token(message_from, "team");
        strcpy(&(message[i]), token);
        free(token);
    }
    else if (result == -6){
        strcpy(&(message[i]), "SH_ERROR_INVALID_PLAYERNAME|text=player name: ");
        i = strlen(message);
        char *token = get_token(message_from, "player");
        strcpy(&(message[i]), token);
        free(token);
    }
    else if (result == -7){
        strcpy(&(message[i]), "SH_ERROR_INVALID_ID|text=");
        i = strlen(message);
        strcpy(&(message[i]), message_from);
        i = strlen(message);
        strcpy(&(message[i]), " has invalid id");
    }
    else if (result == -8){
        strcpy(&(message[i]), "SH_DUPLICATE_PLAYERNAME|text=player name: ");
        i = strlen(message);
        char *token = get_token(message_from, "player");
        strcpy(&(message[i]), token);
        free(token);
    }
    else if (result == -9){
        strcpy(&(message[i]), "SH_ERROR_DUPLICATE_FIELD|text=");
        i = strlen(message);
        strcpy(&(message[i]), message_from);
        i = strlen(message);
        strcpy(&(message[i]), " has invalid field");
    }
    else if (result == -10){
        strcpy(&(message[i]), "SH_CLAIMED|text=Successfully claimed krag");
    }
    else if (result == -11){
        strcpy(&(message[i]), "SH_CLAIMED_ALREADY|text=This krag has already been claimed");
    }
    
    if (sendto(comm_sock, message, strlen(message), 0,
               (struct sockaddr *) &them, sizeof(them)) < 0) {
        perror("sending in datagram socket");
        exit(6);
    }
    // write to log file
    char *ipaddress = getIP(comm_sock, them); //get the IP Adress
    print_log(message, "gameserver.log", ipaddress, "TO"); //print it to the log file
    free(ipaddress);
    
    #ifdef DEBUG
    printf("Out message: %s\n", message);
    #endif
    
    free(gameId);
    free(message);
}


/**************** respond_with_game_status ****************/
/* respond with game status
 * opCode=GAME_STATUS|gameId=FEED|guideId=0707|numClaimed=5|numKrags=8
 */
static void
respond_with_game_status(int comm_sock, struct sockaddr_in them, game_info_t *gi, char *message_from){
    // initialize parameters
    team_t *team = get_team(message_from, gi);
    if (team == NULL) {
        return;
    }
    
    char *message = malloc(MESSAGE_LENGTH);
    char *gameId = game_info_get_gameId(gi);
    char *numClaimed = malloc(15);
    char *numKrags = malloc(15);
    if (message == NULL || gameId == NULL || numClaimed == NULL || numKrags == NULL){
        if (message != NULL)free(message);
        if (gameId != NULL)free(gameId);
        if (numClaimed != NULL)free(numClaimed);
        if (numKrags != NULL)free(numKrags);
        return;
    }
    char *guideId = team_get_guideId(team);
    if (guideId == NULL){
        guideId = malloc(MESSAGE_LENGTH);
        strcpy(guideId, "none");
    }
    
    strcpy(message,"opCode=GAME_STATUS|gameId=");
    int i = strlen(message);
    strcpy(&(message[i]), gameId);
    i = strlen(message);
    
    strcpy(&(message[i]), "|guideId=");
    i = strlen(message);
    strcpy(&(message[i]), guideId);
    i = strlen(message);
    
    strcpy(&(message[i]), "|numClaimed=");
    i = strlen(message);
    sprintf(numClaimed, "%d", team_get_numClaimed(team));
    strcpy(&(message[i]), numClaimed);
    i = strlen(message);
    strcpy(&(message[i]), "|numKrags=");
    i = strlen(message);
    sprintf(numKrags, "%d", game_info_get_numKrags(gi));
    strcpy(&(message[i]), numKrags);
    
    if (sendto(comm_sock, message, strlen(message), 0, (struct sockaddr *) &them, sizeof(them)) < 0) {
        perror("sending in datagram socket");
        exit(6);
    }
    // write to log file
    char *ipaddress = getIP(comm_sock, them); //get the IP Adress
    print_log(message, "gameserver.log", ipaddress, "FROM"); //print it to the log file
    free(ipaddress);
    
    #ifdef DEBUG
    printf("Out message: %s\n", message);
    #endif
    
    free(gameId);
    free(guideId);
    free(numClaimed);
    free(numKrags);
    free(message);
}


/**************** respond_with_gs_clue ****************/
/* respond with gs clue
 * opCode=GS_CLUE|gameId=|guideId=|kragId=|clue=
 */
static void respond_with_gs_clue(int comm_sock, struct sockaddr_in them, game_info_t *gi, char *message_from){
    team_t *team = get_team(message_from, gi);
    // reveal two and send both clues
    // First time is if number of reveled krags is less than number of total krags
    #ifdef DEBUG
    printf("num_krags: %d\tnum_revealed: %d\n",game_info_get_numKrags(gi),team_get_numRevealed(team));
    #endif
    
    if (team_get_numRevealed(team) < game_info_get_numKrags(gi)){
        krag_t *krag = game_info_reveal_krag(gi, team);
        if (krag != NULL) send_gs_clue(comm_sock, gi, message_from, krag);
    }
    
    #ifdef DEBUG
    printf("num_krags: %d\tnum_revealed: %d\n",game_info_get_numKrags(gi),team_get_numRevealed(team));
    #endif
    
    // Second time is if number of revealed krags is less than number of total krags
    if (team_get_numRevealed(team) < game_info_get_numKrags(gi)){
        krag_t *krag = game_info_reveal_krag(gi, get_team(message_from, gi));
        if (krag != NULL) send_gs_clue(comm_sock, gi, message_from, krag);
    }
}

/**************** send_gs_clue ****************/
/* Send with gs clue to the guide agent
 * opCode=GS_CLUE|gameId=|guideId=|kragId=|clue=
 */
static void
send_gs_clue(int comm_sock, game_info_t *gi, char *message_from, krag_t *krag){
    #ifdef DEBUG
    krag_print(krag);
    #endif
    
    team_t *team = get_team(message_from, gi);
    if (team == NULL) return;
    
    // initialize parameters
    char *message = malloc(MESSAGE_LENGTH);
    char *gameId = game_info_get_gameId(gi);
    char *guideId = team_get_guideId(team);
    unsigned int kragid = krag_get_kragId(krag);
    char *kragId = decToStringHex(kragid);
    char *clue = krag_get_clue(krag);
    if (message == NULL || gameId == NULL || guideId == NULL
        || kragId == NULL || clue == NULL){
        if (gameId != NULL)free(gameId);
        if (guideId != NULL)free(guideId);
        if (kragId != NULL)free(kragId);
        if (clue != NULL)free(clue);
        if (message != NULL)free(message);
        return;
    }
    
    strcpy(message,"opCode=GS_CLUE|gameId=");
    int i = strlen(message);
    
    strcpy(&(message[i]), gameId);
    i = strlen(message);
    
    strcpy(&(message[i]), "|guideId=");
    i = strlen(message);
    strcpy(&(message[i]), guideId);
    i = strlen(message);
    
    strcpy(&(message[i]), "|kragId=");
    i = strlen(message);
    strcpy(&(message[i]), kragId);
    i = strlen(message);
    
    strcpy(&(message[i]), "|clue=");
    i = strlen(message);
    strcpy(&(message[i]), clue);
    
    ga_send_to(team_get_guide(team), comm_sock, message);
    
    #ifdef DEBUG
    printf("Out message: %s\n", message);
    #endif
    
    free(gameId);
    free(guideId);
    free(kragId);
    free(clue);
    free(message);
}

/**************** respond_with_gs_claimed ****************/
/* respond with gs claimed
 * opCode=GS_CLAIMED|gameId=|guideId=|pebbleId=|kragId=|latitude=|longitude=
 */
static void respond_with_gs_claimed(int comm_sock, struct sockaddr_in them,
                                    game_info_t *gi, char *message_from){
    team_t *team = get_team(message_from, gi);
    char *kragId = get_token(message_from, "kragId");
    krag_t *krag = game_info_find_krag(gi, kragId);
    ga_t *ga = team_get_guide(team);
    
    if (team == NULL || krag == NULL || ga == NULL){
        free(kragId);
        return;
    }
    
    // initialize parameters
    char *message = malloc(MESSAGE_LENGTH);
    char *gameId = game_info_get_gameId(gi);
    char *pebbleId = get_token(message_from, "pebbleId");
    char *latitude = malloc(15);
    char *longitude = malloc(15);
    if (message == NULL || gameId == NULL || pebbleId == NULL
        || kragId == NULL || longitude == NULL || latitude == NULL){
        if (gameId != NULL) free(gameId);
        if (pebbleId != NULL) free(pebbleId);
        if (kragId != NULL) free(kragId);
        if (longitude != NULL) free(longitude);
        if (latitude != NULL) free(latitude);
        if (gameId != NULL) free(message);
        return;
    }
    
    // update the secret string of the given team
    team_update_string(gi, team, krag);
    
    // create message
    strcpy(message,"opCode=GS_CLAIMED|gameId=");
    int i = strlen(message);
    strcpy(&(message[i]), gameId);
    i = strlen(message);
    
    strcpy(&(message[i]), "|guideId=");
    i = strlen(message);
    strcpy(&(message[i]), ga_get_id(ga));
    i = strlen(message);
    
    strcpy(&(message[i]), "|pebbleId=");
    i = strlen(message);
    strcpy(&(message[i]), pebbleId);
    i = strlen(message);
    
    strcpy(&(message[i]), "|kragId=");
    i = strlen(message);
    strcpy(&(message[i]), kragId);
    i = strlen(message);
    
    strcpy(&(message[i]), "|latitude=");
    i = strlen(message);
    sprintf(latitude, "%f", krag_get_latitude(krag));
    if (strcmp(latitude,"1000") != 0) strcpy(&(message[i]), latitude);
    i = strlen(message);
    
    strcpy(&(message[i]), "|longitude=");
    i = strlen(message);
    sprintf(longitude, "%f", krag_get_longitude(krag));
    if (strcmp(longitude,"1000") != 0) strcpy(&(message[i]), longitude);
    
    ga_send_to(ga, comm_sock, message);
    
    #ifdef DEBUG
    printf("Out message: %s\n", message);
    #endif
    
    free(gameId);
    free(pebbleId);
    free(kragId);
    free(longitude);
    free(latitude);
    free(message);
}


/**************** respond_with_gs_secret ****************/
/* respond with gs secret
 * opCode=GS_SECRET|gameId=|guideId=|secret=
 */
static void respond_with_gs_secret(int comm_sock, struct sockaddr_in them,
                                   game_info_t *gi, char *message_from){
    team_t *team = get_team(message_from, gi);
    ga_t *ga = team_get_guide(team);
    if (team == NULL || ga == NULL){
        return;
    }
    // initialize parameters
    char *message = malloc(MESSAGE_LENGTH);
    char *gameId = game_info_get_gameId(gi);
    char *guideId = team_get_guideId(team);
    char *secret = team_get_secret(team);
    if (message == NULL || gameId == NULL || guideId == NULL || secret == NULL ){
        if (gameId != NULL)free(gameId);
        if (guideId != NULL)free(guideId);
        if (secret != NULL)free(secret);
        if (message != NULL)free(message);
        return;
    }
    
    strcpy(message,"opCode=GS_SECRET|gameId=");
    int i = strlen(message);
    strcpy(&(message[i]), gameId);
    i = strlen(message);
    
    strcpy(&(message[i]), "|guideId=");
    i = strlen(message);
    strcpy(&(message[i]), guideId);
    i = strlen(message);
    
    strcpy(&(message[i]), "|secret=");
    i = strlen(message);
    strcpy(&(message[i]), secret);
    
    ga_send_to(ga, comm_sock, message);
    
    #ifdef DEBUG
    printf("Out message: %s\n", message);
    #endif
    
    free(gameId);
    free(guideId);
    free(secret);
    free(message);
}

/**************** respond_with_gs_agent ****************/
/* respond with gs agent for each fa in the team
 * opCode=GS_AGENT|gameId=|pebbleId=|team=|player=|latitude=|longitude=|lastContact=
 */
static void
respond_with_gs_agent(int comm_sock, struct sockaddr_in them, game_info_t *gi, char *message_from){
    team_t *team = get_team(message_from, gi);
    if (team == NULL) return;
    team_send_gs_agent(gi, team, comm_sock, &send_gs_agent);
}

/**************** send_gs_agent ****************/
/* Helper function for respond_with_gs_agent
 */
static void
send_gs_agent(void *arg, const char *key, void *item){
    gifaga_t *gifaga = arg;
    fa_t *fa = item;
    if (gifaga == NULL || fa == NULL) return;
    game_info_t *gi = gifaga_get_gi(gifaga);
    ga_t *ga = gifaga_get_ga(gifaga);
    
    // initialize parameters
    char *message = malloc(MESSAGE_LENGTH);
    char *gameId = game_info_get_gameId(gi);
    char *pebbleId = fa_get_pebbleId(fa);
    team_t *team = gifaga_get_team(gifaga);
    char *team_name = team_get_name(team);
    char *player = fa_get_name(fa);
    char *latitude = malloc(15);
    char *longitude = malloc(15);
    char *lastContact = malloc(15);
    if (message == NULL || gameId == NULL || pebbleId == NULL
        || team_name == NULL || player == NULL || latitude == NULL
        || longitude == NULL || lastContact == NULL ){
        if (gameId != NULL)free(gameId);
        if (pebbleId != NULL)free(pebbleId);
        if (team_name != NULL)free(team_name);
        if (player != NULL)free(player);
        if (latitude != NULL)free(latitude);
        if (longitude != NULL)free(longitude);
        if (lastContact != NULL)free(lastContact);
        if (message != NULL)free(message);
        return;
    }
    
    strcpy(message,"opCode=GS_AGENT|gameId=");
    int i = strlen(message);
    strcpy(&(message[i]), gameId);
    i = strlen(message);
    
    strcpy(&(message[i]), "|pebbleId=");
    i = strlen(message);
    strcpy(&(message[i]), pebbleId);
    i = strlen(message);
    
    strcpy(&(message[i]), "|team=");
    i = strlen(message);
    strcpy(&(message[i]), team_name);
    i = strlen(message);
    
    strcpy(&(message[i]), "|player=");
    i = strlen(message);
    strcpy(&(message[i]), player);
    i = strlen(message);
    
    strcpy(&(message[i]), "|latitude=");
    i = strlen(message);
    sprintf(latitude, "%f", fa_get_latitude(fa));
    if (strcmp(latitude,"1000") != 0) strcpy(&(message[i]), latitude);
    i = strlen(message);
    
    strcpy(&(message[i]), "|longitude=");
    i = strlen(message);
    sprintf(longitude, "%f", fa_get_longitude(fa));
    if (strcmp(longitude,"1000") != 0) strcpy(&(message[i]), longitude);
    i = strlen(message);
    
    strcpy(&(message[i]), "|lastContact=");
    i = strlen(message);
    sprintf(lastContact, "%d", fa_get_time(fa));
    if (strcmp(lastContact,"-1") != 0)strcpy(&(message[i]), lastContact);
    
    ga_send_to(ga, gifaga_get_comm_sock(gifaga), message);
    
    #ifdef DEBUG
    printf("Out message: %s\n", message);
    #endif
    
    free(gameId);
    free(pebbleId);
    free(team_name);
    free(player);
    free(latitude);
    free(longitude);
    free(lastContact);
    free(message);
}

/**************** send_hint_to_everyone_in_team ****************/
/* send ga hint to all fa in team
 * opCode=GA_HINT|gameId=|guideId=|team=|player=|pebbleId=|hint=
 */
static void
send_hint_to_everyone_in_team(int comm_sock, struct sockaddr_in them, game_info_t *gi, char *message_from){
    team_t *team = get_team(message_from, gi);
    if (team == NULL){
        return;
    }
    team_send_message_to_everyone(team, message_from, comm_sock);
    
    #ifdef DEBUG
    printf("Out message to all fa in team: %s\n", message_from);
    #endif
}

/**************** respond_with_ga_hint ****************/
/* respond with ga hint
 * opCode=GA_HINT|gameId=|guideId=|team=|player=|pebbleId=|hint=
 */
static void
respond_with_ga_hint(int comm_sock, struct sockaddr_in them, char *message_from, game_info_t *gi){
    char *pebbleId = get_token(message_from, "pebbleId");
    fa_t *fa = game_info_find_fa_pebbleId(gi, pebbleId);
    fa_send_to(fa, comm_sock, message_from);
    
    #ifdef DEBUG
    printf("Out message: %s\n", message_from);
    #endif
    
    free(pebbleId);
}


/**************** send_game_over ****************/
/* Send GAME OVER to all agents
 * opCode=GAME_OVER|gameId=|secret=
 */
void
send_game_over(int comm_sock, game_info_t *gi){
    // initialize parameters
    char *message = malloc(MESSAGE_LENGTH);
    char *gameId = game_info_get_gameId(gi);
    char *secret = game_info_get_secret(gi);
    if (message == NULL || gameId == NULL || secret == NULL){
        if (message != NULL) free(message);
        if (gameId != NULL) free(gameId);
        if (secret != NULL) free(secret);
        return;
    }
    
    // create message
    strcpy(message,"opCode=GAME_OVER|gameId=");
    int i = strlen(message);
    strcpy(&(message[i]), gameId);
    i = strlen(message);
    
    strcpy(&(message[i]), "|secret=");
    i = strlen(message);
    strcpy(&(message[i]), secret);
    
    //send message to everyone in the game
    game_info_send_message_to_everyone(gi, message, comm_sock, &send_game_over_to_everyone);
    
    free(secret);
    free(message);
    free(gameId);
}

/**************** send_game_over_to_everyone ****************/
/* Helper function for game_info_send_message_to_everyone
 */
static void
send_game_over_to_everyone(void *arg, const char *key, void *item){
    send_message_t *send_message = arg;
    team_t *team = item;
    if (send_message == NULL || team == NULL) return;
    
    // initialize parameters
    char *message = send_message_get_message(send_message);
    int comm_sock = send_message_get_comm_sock(send_message);
    if (message == NULL) return;
    
    // send to the ga and all fa in team
    ga_send_to(team_get_guide(team), comm_sock, message);
    team_send_message_to_everyone(team, message, comm_sock);
    
    #ifdef DEBUG
    printf("Out message: %s\n", message);
    #endif
    
    free(message);
}

/**************** send_team_record ****************/
/* Send TEAM RECORD to all agentsResult value i
 * opCode=TEAM_RECORD|gameId=|team=|numClaimed=|numPlayers=
 */
void
send_team_record(int comm_sock, game_info_t *gi){
    // initialize
    char *message = malloc(MESSAGE_LENGTH);
    char *gameId = game_info_get_gameId(gi);
    char *numKrags = malloc(15);
    if (message == NULL || gameId == NULL || numKrags == NULL){
        if (message != NULL) free(message);
        if (gameId != NULL) free(gameId);
        if (numKrags != NULL) free(numKrags);
        return;
    }
    
    // create message
    strcpy(message,"opCode=TEAM_RECORD|gameId=");
    int i = strlen(message);
    
    strcpy(&(message[i]), gameId);
    i = strlen(message);
    strcpy(&(message[i]), "|numKrags=");
    i = strlen(message);
    
    sprintf(numKrags, "%d", game_info_get_numKrags(gi));
    strcpy(&(message[i]), numKrags);
    i = strlen(message);
    strcpy(&(message[i]), "|team=");
    
    // send the message to everyone in the game
    game_info_send_message_to_everyone(gi, message, comm_sock, &send_team_record_to_everyone);
    
    free(numKrags);
    free(message);
    free(gameId);
}
/**************** send_team_record_to_everyone ****************/
/* Helper function for game_info_send_message_to_everyone
 */
static void
send_team_record_to_everyone(void *arg, const char *key, void *item){
    send_message_t *send_message = arg;
    team_t *team = item;
    if (send_message == NULL || team == NULL) return;
    
    // initialize parameters
    char *message = send_message_get_message(send_message);
    char *team_name = team_get_name(team);
    char *numClaimed = malloc(15);
    if (message == NULL || team_name == NULL || numClaimed == NULL){
        if (message != NULL) free(message);
        if (team_name != NULL) free(team_name);
        if (numClaimed != NULL) free(numClaimed);
        return;
    }
    
    int i = strlen(message);
    strcpy(&(message[i]), team_name);
    i = strlen(message);
    
    strcpy(&(message[i]), "|numClaimed=");
    i = strlen(message);
    sprintf(numClaimed, "%d", team_get_numClaimed(team));
    strcpy(&(message[i]), numClaimed);
    
    int comm_sock = send_message_get_comm_sock(send_message);
    ga_send_to(team_get_guide(team), comm_sock, message);
    team_send_message_to_everyone(team, message, comm_sock);
    
    #ifdef DEBUG
    printf("Out message: %s\n", message);
    #endif
    
    free(numClaimed);
    free(team_name);
    free(message);
}

/**************** get_team ****************/
/* parse the message and get the team, which the guide agent belongs to
 */
static team_t *
get_team(char *message_from, game_info_t *gi){
    char *guideId = get_token(message_from, "guideId");
    if (guideId != NULL){
        team_t *team = game_info_find_guideId(gi, guideId);
        free(guideId);
        return team;
    }
    char *pebbleId = get_token(message_from, "pebbleId");
    if (pebbleId != NULL){
        team_t *team = game_info_find_team_pebbleId(gi, pebbleId);
        free(pebbleId);
        return team;
    }
    return NULL;
}

/**************** get_token ****************/
/* parse the message and get the appropriate right hand side token
 */
static char *
get_token(char *message, char *left_hand_side){
    char *buf = malloc(MESSAGE_LENGTH);
    if (buf == NULL) return NULL;
    int j = 0;
    int text = 0;
    
    // loop over the message and find the
    // right hand side of the given left hand side
    for (int i = 0; i < strlen(message); i++){
        if (message[i] == '='){
            buf[j] = '\0';
            // if the buf match with left hand side,
            // start reading the right hand side
            if (strcmp(buf, left_hand_side) == 0){
                text = 1;
            }
            j = 0;
        }
        else if (message[i] == '|'){
            buf[j] = '\0';
            j = 0;
            // break the loop if buf finds the right hand side
            if (text == 1){
                break;
            }
        }
        else{
            buf[j] = message[i];
            j++;
        }
        
        if (i == strlen(message)-1){
            buf[j] = '\0';
        }
    }
    
    // return the buf if the right hand side is found
    if (text == 1){
        return buf;
    }
    // otherwise return NULL
    else {
        free(buf);
        return NULL;
    }
}
