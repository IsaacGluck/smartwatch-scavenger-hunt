/*
 * gs_message_handler - a function tables used for gameserver
 *
 * Handles message received from field agent or guide agent
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
#include "gsstruct.h"

/**************** file local type ****************/
// struct for holding messages
typedef struct token{
    char *left;              // left hand side of the equation
    char *right;             // right hand side of the equation
    char *rest_of_line;      // rest of the line
}token_t;

/**************** file-local global variables ****************/
static const int MESSAGE_LENGTH = 8192;

/**************** functions ****************/
static token_t *token_new();
static void token_next(token_t *token);
static void token_delete(token_t *token);
static char **allocate_message_fields(int number_of_slots);
static void delete_message_fields(char **message_fields, int number_of_slots);



/**************** fn_fa_location ****************/
//int fn_fa_location(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
int fn_fa_location(char *rest_of_line, game_info_t *gi){
    token_t *token = token_new(rest_of_line);
    if (token == NULL){
        return -99;
    }
    char **message_fields = allocate_message_fields(7);
    if (message_fields == NULL){
        return -99;
    }
    
    char *gameId;
    char *pebbleId;
    char *team_name;
    char *player_name;
    char *latitude;
    char *longitude;
    char *statusReq;
    // while there are something in the rest of line
    // create the array of the message fields
    do {
        token_next(token);
        if (strcmp(token->left,"gameId") == 0){
            message_fields[0] = token->right;
            gameId = message_fields[0];
        }
        else if (strcmp(token->left,"pebbleId") == 0){
            message_fields[1] = token->right;
            pebbleId = message_fields[1];
        }
        else if (strcmp(token->left,"team") == 0){
            message_fields[2] = token->right;
            team_name = message_fields[2];
        }
        else if (strcmp(token->left,"player") == 0){
            message_fields[3] = token->right;
            player_name = message_fields[3];
        }
        else if (strcmp(token->left,"latitude") == 0){
            message_fields[4] = token->right;
            latitude = message_fields[4];
        }
        else if (strcmp(token->left,"longitude") == 0){
            message_fields[5] = token->right;
            longitude = message_fields[5];
        }
        else if (strcmp(token->left,"statusReq") == 0){
            message_fields[6] = token->right;
            statusReq = message_fields[6];
        }
        else {
            delete_message_fields(message_fields, 7);
            return -3;
        }
    } while(token->rest_of_line != '\0');
    
    // if there are missing part, invalid message
    for (int i = 0; i < 7; i++){
        if (message_fields[i] == '\0'){
            return -1;
        }
    }
    
    int status;
    // if gameId = 0
    if (strcmp(gameId,"0") == 0){
        // if team is not known, register team
        team_t *team = game_info_find_team(gi, team_name);
        if (team == NULL){
            game_info_register_team(gi, team_name);
            team_t *team = game_info_find_team(gi, team_name);
            // if team cannot be registered, error
            if (team == NULL){
                delete_message_fields(message_fields, 7);
                token_delete(token);
                return -5;
            }
        }
        
        // If there is already a player with the same name in the team,
        // ignore and return -9
        fa_t *fa = team_find_fa(team, player_name);
        if (fa != NULL){
            delete_message_fields(message_fields, 7);
            token_delete(token);
            return -9;
        }
        
        // if pebbleId is not known, register pebbleId
        // and associate it with given player and team
        if ((status = team_register_fa(gi, team, pebbleId, player_name, them, latitude, longitude)) != 0){
            delete_message_fields(message_fields, 7);
            token_delete(token);
            return status;
        }
        else{
            delete_message_fields(message_fields, 7);
            token_delete(token);
            return 1;
        }
    }
    // if gameId != 0
    else{
        // validate gameId, pebbleId, team, and player
        // if correct, update
        if ((status = game_info_validate(gi, gameId, pebbleId, team_name, player_name, latitude, longitude)) != 0){
            delete_message_fields(message_fields, 7);
            token_delete(token);
            return status;
        }
        
        if (strcmp(statusReq, "1") == 0){
            delete_message_fields(message_fields, 7);
            token_delete(token);
            return 1;
        }
        delete_message_fields(message_fields, 7);
        token_delete(token);
        return 0;
    }
}





//int fn_fa_claim(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
int fn_fa_claim(char *rest_of_line, game_info_t *gi);
// initialize token and parse the message
token_t *token = token_new(rest_of_line);
if (token == NULL){
    return -99;
}
char **message_fields = allocate_message_fields(7);
if (message_fields == NULL){
    return -99;
}

char *gameId;
char *pebbleId;
char *team_name;
char *player_name;
char *latitude;
char *longitude;
char *kragId;
// while there are something in the rest of line
// create the array of the message fields
do {
    token_next(token);
    if (strcmp(token->left,"gameId") == 0){
        message_fields[0] = token->right;
        gameId = message_fields[0];
    }
    else if (strcmp(token->left,"pebbleId") == 0){
        message_fields[1] = token->right;
        pebbleId = message_fields[1];
    }
    else if (strcmp(token->left,"team") == 0){
        message_fields[2] = token->right;
        team_name = message_fields[2];
    }
    else if (strcmp(token->left,"player") == 0){
        message_fields[3] = token->right;
        player_name = message_fields[3];
    }
    else if (strcmp(token->left,"latitude") == 0){
        message_fields[4] = token->right;
        latitude = message_fields[4];
    }
    else if (strcmp(token->left,"longitude") == 0){
        message_fields[5] = token->right;
        longitude = message_fields[5];
    }
    else if (strcmp(token->left,"kragId") == 0){
        message_fields[6] = token->right;
        kragId = message_fields[6];
    }
    // if the left hand side is not found, error
    else {
        delete_message_fields(message_fields, 7);
        return -3;
    }
} while(token->rest_of_line != '\0');

// if there are missing part, invalid message
for (int i = 0; i < 7; i++){
    if (message_fields[i] == '\0'){
        delete_message_fields(message_fields, 7);
        token_delete(token);
        return -1;
    }
}

int status;
// validate gameId, pebbleId, team, and player
// if correct, update, if error return the error value
if ((status = game_info_validate(gi, gameId, pebbleId, team_name, player_name, latitude, longitude)) != 0){
    delete_message_fields(message_fields, 7);
    token_delete(token);
    return status;
}

krag_t *krag;
// if krag is not found, return -7
if ( (krag = game_info_find_krag(gi, kragId)) == NULL){
    delete_message_fields(message_fields, 7);
    token_delete(token);
    return -7;
}

// Confirm the given latitude|longitude is within 10 meters
// of the known position of the identified krag
if ((status = game_info_krag_distance(gi, kragId, latitude, longitude)) != 0){
    delete_message_fields(message_fields, 7);
    token_delete(token);
    return -1;
}

// If the krag has not been claimed by this team
if (krag_has_claimed(krag, team_name) != 0){
    // Mark it as ‘claimed’ and send a SH_CLAIMED response to the FA
    status = krag_mark_claimed(gi, krag, team_name);
    delete_message_fields(message_fields, 7);
    token_delete(token);
    // If this is not the last krag to be claimed, return 1
    if (status == 1){
        return 1;
    }
    // If this is the last krag to be claimed, return 2
    else if (status == 2){
        return 2;
    }
    // otherwise ignore
    else {
        return 0;
    }
}
// Else if the krag has already has been claimed by this team, return 3
delete_message_fields(message_fields, 7);
token_delete(token);
return 3;
}


//int fn_fa_log(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
int fn_fa_log(char *rest_of_line, game_info_t *gi){
    return 0;
}


//int fn_ga_status(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
int fn_ga_status(char *rest_of_line, game_info_t *gi){
    // initialize token and parse the message
    token_t *token = token_new(rest_of_line);
    if (token == NULL){
        return -99;
    }
    char **message_fields = allocate_message_fields(5);
    if (message_fields == NULL){
        return -99;
    }
    
    char *gameId;
    char *guideId;
    char *team_name;
    char *player_name;
    char *statusReq;
    // while there are something in the rest of line
    // create the array of the message fields
    do {
        token_next(token);
        if (strcmp(token->left,"gameId") == 0){
            message_fields[0] = token->right;
            gameId = message_fields[0];
        }
        else if (strcmp(token->left,"guideId") == 0){
            message_fields[1] = token->right;
            guideId = message_fields[1];
        }
        else if (strcmp(token->left,"team") == 0){
            message_fields[2] = token->right;
            team_name = message_fields[2];
        }
        else if (strcmp(token->left,"player") == 0){
            message_fields[3] = token->right;
            player_name = message_fields[3];
        }
        else if (strcmp(token->left,"statusReq") == 0){
            message_fields[4] = token->right;
            statusReq = message_fields[4];
        }
        // if the left hand side is not found, error
        else {
            delete_message_fields(message_fields, 5);
            return -3;
        }
    } while(token->rest_of_line != '\0');
    
    // if there are missing part, invalid message
    for (int i = 0; i < 5; i++){
        if (message_fields[i] == '\0'){
            delete_message_fields(message_fields, 5);
            token_delete(token);
            return -1;
        }
    }
    
    int status;
    // If gameId == 0
    if (strcmp(gameId,"0") == 0){
        // If team is not known, register team
        team_t *team = game_info_find_team( gi, team_name);
        if (team == NULL){
            game_info_register_team( gi, team_name);
            team_t *team = game_info_find_team(gi, team_name);
            // if team cannot be registered, error
            if (team == NULL){
                delete_message_fields(message_fields, 5);
                token_delete(token);
                return -5;
            }
        }
        // If guideId is not known, register guideId
        // and associate it with given player and team names
        if ((status = team_register_ga(gi, team, guideId, player_name, them)) != 0){
            delete_message_fields(message_fields, 5);
            token_delete(token);
            return status;
        }
        else{
            delete_message_fields(message_fields, 5);
            token_delete(token);
            return 1;
        }
    }
    // If gameId != 0
    else{
        // validate gameId, guideId, team, and player
        // if correct, update, if error return the error value
        if ((status = game_info_validate(gi, gameId, guideId, team_name, player_name, NULL, NULL)) != 0){
            delete_message_fields(message_fields, 5);
            token_delete(token);
            return status;
        }
        
        // If statusReq==1, return 1
        if (strcmp(statusReq, "1") == 0){
            delete_message_fields(message_fields, 5);
            token_delete(token);
            return 1;
        }
        // Otherwise return 0
        delete_message_fields(message_fields, 5);
        token_delete(token);
        return 0;
        
    }
}


//int fn_ga_hint(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
int fn_ga_hint(char *rest_of_line, game_info_t *gi){
    // initialize token and parse the message
    token_t *token = token_new(rest_of_line);
    if (token == NULL){
        return -99;
    }
    char **message_fields = allocate_message_fields(6);
    if (message_fields == NULL){
        return -99;
    }
    
    char *gameId;
    char *guideId;
    char *team_name;
    char *player_name;
    char *pebbleId;
    // while there are something in the rest of line
    // create the array of the message fields
    do {
        token_next(token);
        if (strcmp(token->left,"gameId") == 0){
            message_fields[0] = token->right;
            gameId = message_fields[0];
        }
        else if (strcmp(token->left,"guideId") == 0){
            message_fields[1] = token->right;
            guideId = message_fields[1];
        }
        else if (strcmp(token->left,"team") == 0){
            message_fields[2] = token->right;
            team_name = message_fields[2];
        }
        else if (strcmp(token->left,"player") == 0){
            message_fields[3] = token->right;
            player_name = message_fields[3];
        }
        else if (strcmp(token->left,"pebbleId") == 0){
            message_fields[4] =
            pebbleId = message_fields[4];
        }
        else if (strcmp(token->left,"hint") == 0){
            message_fields[5] = token->right;
        }
        // if the left hand side is not found, error
        else {
            delete_message_fields(message_fields, 6);
            return -3;
        }
    } while(token->rest_of_line != '\0');
    
    // if there are missing part, invalid message
    for (int i = 0; i < 6; i++){
        if (message_fields[i] == '\0'){
            return -1;
        }
    }
    
    int status;
    // validate gameId, pebbleId, team, and player
    // if correct, update, if error return the error value
    if ((status = game_info_validate(gi, gameId, guideId, team_name, player_name, NULL, NULL)) != 0){
        delete_message_fields(message_fields, 6);
        token_delete(token);
        return status;
    }
    
    // If pebbleId is *, return 1
    if (strcmp(pebbleId, "*") == 0){
        delete_message_fields(message_fields, 6);
        token_delete(token);
        return 1;
    }
    // Else if pebbleId is known player in the team, return 2
    else if (game_info_find_pebbleId(gi, pebbleId) != NULL){
        delete_message_fields(message_fields, 6);
        token_delete(token);
        return 2;
    }
    // Else if pebbleId is not known player in the team, return -7
    else {
        delete_message_fields(message_fields, 6);
        token_delete(token);
        return -7;
    }
}


//int fn_game_status(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
int fn_game_status(char *rest_of_line, game_info_t *gi){
    return -2;
}
//int fn_gs_agent(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
int fn_gs_agent(char *rest_of_line, game_info_t *gi){
    return -2;
}
//int fn_gs_clue(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
int fn_gs_clue(char *rest_of_line, game_info_t *gi){
    return -2;
}
//int fn_gs_claimed(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
int fn_gs_claimed(char *rest_of_line, game_info_t *gi){
    return -2;
}
//int fn_gs_secret(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
int fn_gs_secret(char *rest_of_line, game_info_t *gi){
    return -2;
}
//int fn_gs_response(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
int fn_gs_response(char *rest_of_line, game_info_t *gi){
    return -2;
}
//int fn_game_over(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
int fn_game_over(char *rest_of_line, game_info_t *gi){
    return -2;
}
//int fn_team_record(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
int fn_team_record(char *rest_of_line, game_info_t *gi){
    return -2;
}


static token_t *token_new(char *rest_of_line){
    if (rest_of_line == NULL){
        return NULL;
    }
    
    token_t *new_token = malloc(sizeof(token_t));
    if (new_token == NULL){
        fprintf(stderr, "token_new failed due to not being able to malloc memory\n");
        return NULL;
    }
    
    // initialize parameters
    new_token->left = malloc(MESSAGE_LENGTH);
    new_token->right = malloc(MESSAGE_LENGTH);
    new_token->rest_of_line = malloc(MESSAGE_LENGTH);
    if (new_token->left == NULL || new_token->right == NULL || new_token->rest_of_line == NULL){
        fprintf(stderr, "token_new failed due to not being able to malloc memory\n");
        return NULL;
    }
    
    strcpy(new_token->rest_of_line, rest_of_line);
    return new_token;
}


static void token_next(token_t *token){
    // initialize parameters
    int i = 0;
    int j = 0;
    int state = 0; // 0: currently reading left hand side 1: right hand side
    
    while (i < strlen(token->rest_of_line)){
        if (*(token->rest_of_line+i) == '|'){
            (token->right)[j] = '\0';
            break;
        }
        else if (*(token->rest_of_line+i) == '='){
            (token->left)[j] = '\0';
            j = 0;
            state = 1;
        }
        else{
            if (state == 0){
                (token->left)[j] = (token->rest_of_line)[i];
            }
            else {
                (token->right)[j] = (token->rest_of_line)[i];
                if (i == strlen(token->rest_of_line)-1){
                    (token->right)[j+1] = '\0';
                }
            }
            j++;
        }
        i++;
    }
    if (i != strlen(token->rest_of_line)){
        token->rest_of_line = token->rest_of_line+i;
    }
    else{
        token->rest_of_line = '\0';
    }
}


static void token_delete(token_t *token){
    if (token == NULL){
        return;
    }
    if (token->left != NULL) free(token->left);
    if (token->right != NULL) free(token->right);
    if (token->rest_of_line != NULL) free(token->rest_of_line);
    free(token);
}


static char **allocate_message_fields(int number_of_slots){
    // create a 2D array with given number of slots
    // return NULL if malloc error
    char **message_fields = (char **) malloc( number_of_slots * sizeof(char *) );
    if (message_fields == NULL){
        return NULL;
    }
    for (int i = 0; i<number_of_slots; i++){
        message_fields[i] = (char *)malloc(MESSAGE_LENGTH * sizeof(char));
        message_fields[i] = '\0';
        // if malloc error, return NULL after freeing all the memory
        if (message_fields[i] == NULL){
            delete_message_fields(message_fields, i);
            return NULL;
        }
    }
    return message_fields;
}
static void delete_message_fields(char **message_fields, int number_of_slots){
    if (message_fields != NULL);
    for (int i = 0; i < number_of_slots; i++){
        free(message_fields[i]);
    }
    free(message_fields);
}
