/*
 * gs_message_handler - a function tables used for gameserver
 *
 * Handles message received from field agent or guide agent
 *
 * Returning value: Required response (Not performed by this method)
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
static void free_message_fields(char *gameId, char *pebbleId,
        char *team_name, char *player_name, char *latitude, char *longitude,
        char *statusReq, char *kragId, char *guideId, char *hint);



/**************** fn_fa_location ****************/
int fn_fa_location(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
    token_t *token = token_new(rest_of_line);
    if (token == NULL){
        return -99;
    }

    // allocate memory and return -99 if error
    char *gameId = malloc(MESSAGE_LENGTH);
    char *pebbleId = malloc(MESSAGE_LENGTH);
    char *team_name = malloc(MESSAGE_LENGTH);
    char *player_name = malloc(MESSAGE_LENGTH);
    char *latitude = malloc(MESSAGE_LENGTH);
    char *longitude = malloc(MESSAGE_LENGTH);
    char *statusReq = malloc(MESSAGE_LENGTH);
    if (gameId == NULL || pebbleId == NULL || team_name == NULL || player_name == NULL || latitude == NULL || longitude == NULL || statusReq == NULL){
        if(gameId == NULL) free(gameId);
        if(pebbleId == NULL) free(pebbleId);
        if(team_name == NULL) free(team_name);
        if(player_name == NULL) free(player_name);
        if(latitude == NULL) free(latitude);
        if(longitude == NULL) free(longitude);
        if(statusReq == NULL) free(statusReq);
        return -99;
    }
    
    // while there are something in the rest of line
    // create the array of the message fields
    do {
        token_next(token);
        printf("%s %s \n\t%s\n", token->left, token->right, token->rest_of_line);
        if (strcmp(token->left,"gameId") == 0){
            strcpy(gameId, token->right);
        }
        else if (strcmp(token->left,"pebbleId") == 0){
            strcpy(pebbleId, token->right);
        }
        else if (strcmp(token->left,"team") == 0){
            strcpy(team_name, token->right);
        }
        else if (strcmp(token->left,"player") == 0){
            strcpy(player_name, token->right);
        }
        else if (strcmp(token->left,"latitude") == 0){
            strcpy(latitude, token->right);
        }
        else if (strcmp(token->left,"longitude") == 0){
            strcpy(longitude, token->right);
        }
        else if (strcmp(token->left,"statusReq") == 0){
            strcpy(statusReq, token->right);
        }
        else {
            free_message_fields(gameId, pebbleId, team_name, player_name, latitude, longitude, statusReq, NULL, NULL, NULL);
            return -3;
        }
    } while(token->rest_of_line != '\0');
    
    
    int status;
    // if gameId = 0
    if (strcmp(gameId,"0") == 0){
        // if team is not known, register team
        team_t *team = game_info_find_team(gi, team_name);
        if (team == NULL){
            
            // register the team and get the team
            game_info_register_team(gi, team_name);
            team = game_info_find_team(gi, team_name);
            team_print(team);
            // if team cannot be registered, error
            if (team == NULL){
                free_message_fields(gameId, pebbleId, team_name, player_name, latitude, longitude, statusReq, NULL, NULL, NULL);
                token_delete(token);
                return -5;
            }
        }
        
        #ifdef DEBUG
        printf("%s\n",player_name);
        #endif
        
        // If there is already a player with the same name in the team,
        // ignore and return -8
        fa_t *fa = team_find_fa(team, player_name);
        if (fa != NULL){
            free_message_fields(gameId, pebbleId, team_name, player_name, latitude, longitude, statusReq, NULL, NULL, NULL);
            token_delete(token);
            return -8;
        }
        fa_print(fa);
        
        // if pebbleId is not known, register pebbleId
        // and associate it with given player and team
        if ((status = team_register_fa(gi, team, pebbleId, player_name, them, latitude, longitude)) != 0){
            free_message_fields(gameId, pebbleId, team_name, player_name, latitude, longitude, statusReq, NULL, NULL, NULL);
            token_delete(token);
            return status;
        }
        else{
            token_delete(token);
            free_message_fields(gameId, pebbleId, team_name, player_name, latitude, longitude, statusReq, NULL, NULL, NULL);
            return 1;
        }
    }
    // if gameId != 0
    else{
        // validate gameId, pebbleId, team, and player
        // if correct, update
        if ((status = game_info_validate(gi, gameId, pebbleId, team_name, player_name, latitude, longitude)) != 0){
            free_message_fields(gameId, pebbleId, team_name, player_name, latitude, longitude, statusReq, NULL, NULL, NULL);
            token_delete(token);
            return status;
        }
        
        if (strcmp(statusReq, "1") == 0){
            free_message_fields(gameId, pebbleId, team_name, player_name, latitude, longitude, statusReq, NULL, NULL, NULL);
            token_delete(token);
            return 1;
        }
        free_message_fields(gameId, pebbleId, team_name, player_name, latitude, longitude, statusReq, NULL, NULL, NULL);
        token_delete(token);
        return 0;
    }
}


/**************** fn_fa_claim ****************/
int fn_fa_claim(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
    // initialize token and parse the message
    token_t *token = token_new(rest_of_line);
    if (token == NULL){
        return -99;
    }
    
    char *gameId = malloc(MESSAGE_LENGTH);
    char *pebbleId = malloc(MESSAGE_LENGTH);
    char *team_name = malloc(MESSAGE_LENGTH);
    char *player_name = malloc(MESSAGE_LENGTH);
    char *latitude = malloc(MESSAGE_LENGTH);
    char *longitude = malloc(MESSAGE_LENGTH);
    char *kragId = malloc(MESSAGE_LENGTH);
    if (gameId == NULL || pebbleId == NULL || team_name == NULL || player_name == NULL || latitude == NULL || longitude == NULL || kragId == NULL){
        return -99;
    }
    // while there are something in the rest of line
    // create the array of the message fields
    do {
        token_next(token);
        if (strcmp(token->left,"gameId") == 0){
            strcpy(gameId, token->right);
        }
        else if (strcmp(token->left,"pebbleId") == 0){
            strcpy(pebbleId, token->right);
        }
        else if (strcmp(token->left,"team") == 0){
            strcpy(team_name, token->right);
        }
        else if (strcmp(token->left,"player") == 0){
            strcpy(player_name, token->right);
        }
        else if (strcmp(token->left,"latitude") == 0){
            strcpy(latitude, token->right);
        }
        else if (strcmp(token->left,"longitude") == 0){
            strcpy(longitude, token->right);
        }
        else if (strcmp(token->left,"kragId") == 0){
            strcpy(kragId, token->right);
        }
        // if the left hand side is not found, error
        else {
            free_message_fields(gameId, pebbleId, team_name, player_name, latitude, longitude, NULL, kragId, NULL, NULL);
            return -3;
        }
    } while(token->rest_of_line != '\0');
    
    
    int status;
    // validate gameId, pebbleId, team, and player
    // if correct, update, if error return the error value
    if ((status = game_info_validate(gi, gameId, pebbleId, team_name, player_name, latitude, longitude)) != 0){
        free_message_fields(gameId, pebbleId, team_name, player_name, latitude, longitude, NULL, kragId, NULL, NULL);
        token_delete(token);
        return status;
    }
    
    krag_t *krag;
    // if krag is not found, return -7
    if ( (krag = game_info_find_krag(gi, kragId)) == NULL){
        free_message_fields(gameId, pebbleId, team_name, player_name, latitude, longitude, NULL, kragId, NULL, NULL);
        return -7;
    }
    
    // Confirm the given latitude|longitude is within 10 meters
    // of the known position of the identified krag
    if ((status = game_info_krag_distance(gi, kragId, latitude, longitude)) != 0){
        free_message_fields(gameId, pebbleId, team_name, player_name, latitude, longitude, NULL, kragId, NULL, NULL);
        token_delete(token);
        return -1;
    }
    
#ifdef DEBUG
    printf("latitude|longitude confirmed\n");
    krag_print(krag);
    printf("team_name: %s\n",team_name);
#endif
    
    // If the krag has not been claimed by this team
    if ((status = krag_has_claimed(krag, team_name)) == 1){
        printf("status: %d", status);
        // Mark it as ‘claimed’ and send a SH_CLAIMED response to the FA
        status = krag_mark_claimed(gi, krag, team_name);
        free_message_fields(gameId, pebbleId, team_name, player_name, latitude, longitude, NULL, kragId, NULL, NULL);
        token_delete(token);
        printf("marked CLAIMED\n");
        return status;
    }
    else if (status == 0){
        // Else if the krag has already has been claimed by this team, return 3
        free_message_fields(gameId, pebbleId, team_name, player_name, latitude, longitude, NULL, kragId, NULL, NULL);
        token_delete(token);
        return 3;
    }
    else{
        return 0;
    }
}


/**************** fn_fa_log ****************/
int fn_fa_log(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
    return 0;
}


/**************** fn_ga_status ****************/
int fn_ga_status(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
    // initialize token and parse the message
    token_t *token = token_new(rest_of_line);
    if (token == NULL){
        return -99;
    }
    
    char *gameId = malloc(MESSAGE_LENGTH);
    char *guideId = malloc(MESSAGE_LENGTH);
    char *team_name = malloc(MESSAGE_LENGTH);
    char *player_name = malloc(MESSAGE_LENGTH);
    char *statusReq = malloc(MESSAGE_LENGTH);
    if (gameId == NULL || guideId == NULL || team_name == NULL || player_name == NULL || statusReq == NULL){
        return -99;
    }
    // while there are something in the rest of line
    // create the array of the message fields
    do {
        token_next(token);
        if (strcmp(token->left,"gameId") == 0){
            strcpy(gameId, token->right);
        }
        else if (strcmp(token->left,"guideId") == 0){
            strcpy(guideId, token->right);
        }
        else if (strcmp(token->left,"team") == 0){
            strcpy(team_name, token->right);
        }
        else if (strcmp(token->left,"player") == 0){
            strcpy(player_name, token->right);
        }
        else if (strcmp(token->left,"statusReq") == 0){
            strcpy(statusReq, token->right);
        }
        // if the left hand side is not found, error
        else {
            free_message_fields(gameId, guideId, team_name, player_name, NULL, NULL, statusReq, NULL, NULL, NULL);
            return -3;
        }
    } while(token->rest_of_line != '\0');
    
    
    int status;
    // If gameId == 0
    if (strcmp(gameId,"0") == 0){
        // If team is not known, register team
        team_t *team = game_info_find_team( gi, team_name);
        if (team == NULL){
            game_info_register_team( gi, team_name);
            team = game_info_find_team(gi, team_name);
            // if team cannot be registered, error
            if (team == NULL){
                free_message_fields(gameId, guideId, team_name, player_name, NULL, NULL, statusReq, NULL, NULL, NULL);
                token_delete(token);
                return -5;
            }
        }
        // If guideId is not known, register guideId
        // and associate it with given player and team names
        if ((status = team_register_ga(gi, team, guideId, player_name, them)) != 0){
            free_message_fields(gameId, guideId, team_name, player_name, NULL, NULL, statusReq, NULL, NULL, NULL);
            token_delete(token);
            return status;
        }
        else{
            free_message_fields(gameId, guideId, team_name, player_name, NULL, NULL, statusReq, NULL, NULL, NULL);
            token_delete(token);
            return 2;
        }
    }
    // If gameId != 0
    else{
        // validate gameId, guideId, team, and player
        // if correct, update, if error return the error value
        if ((status = game_info_validate(gi, gameId, guideId, team_name, player_name, NULL, NULL)) != 0){
            free_message_fields(gameId, guideId, team_name, player_name, NULL, NULL, statusReq, NULL, NULL, NULL);
            token_delete(token);
            return status;
        }
        
        // If statusReq==1, return 1
        if (strcmp(statusReq, "1") == 0){
            free_message_fields(gameId, guideId, team_name, player_name, NULL, NULL, statusReq, NULL, NULL, NULL);
            token_delete(token);
            return 1;
        }
        // Otherwise return 0
        free_message_fields(gameId, guideId, team_name, player_name, NULL, NULL, statusReq, NULL, NULL, NULL);
        token_delete(token);
        return 0;
        
    }
}


int fn_ga_hint(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
    // initialize token and parse the message
    token_t *token = token_new(rest_of_line);
    if (token == NULL){
        return -99;
    }
    
    char *gameId = malloc(MESSAGE_LENGTH);
    char *guideId = malloc(MESSAGE_LENGTH);
    char *team_name = malloc(MESSAGE_LENGTH);
    char *player_name = malloc(MESSAGE_LENGTH);
    char *pebbleId = malloc(MESSAGE_LENGTH);
    char *hint = malloc(MESSAGE_LENGTH);
    if (gameId == NULL || guideId == NULL || team_name == NULL || player_name == NULL || pebbleId == NULL || hint == NULL){
        return -99;
    }
    // while there are something in the rest of line
    // create the array of the message fields
    do {
        token_next(token);
        if (strcmp(token->left,"gameId") == 0){
            strcpy(gameId, token->right);
        }
        else if (strcmp(token->left,"guideId") == 0){
            strcpy(guideId, token->right);
        }
        else if (strcmp(token->left,"team") == 0){
            strcpy(team_name, token->right);
        }
        else if (strcmp(token->left,"player") == 0){
            strcpy(player_name, token->right);
        }
        else if (strcmp(token->left,"pebbleId") == 0){
            strcpy(pebbleId, token->right);
        }
        else if (strcmp(token->left,"hint") == 0){
            strcpy(hint, token->right);
        }
        // if the left hand side is not found, error
        else {
            free_message_fields(gameId, pebbleId, team_name, player_name, NULL, NULL, NULL, NULL, guideId, hint);
            return -3;
        }
    } while(token->rest_of_line != '\0');
    
    
    int status;
    // validate gameId, pebbleId, team, and player
    // if correct, update, if error return the error value
    if ((status = game_info_validate(gi, gameId, guideId, team_name, player_name, NULL, NULL)) != 0){
        free_message_fields(gameId, pebbleId, team_name, player_name, NULL, NULL, NULL, NULL, guideId, hint);
        token_delete(token);
        return status;
    }
    
    // If pebbleId is *, return 1
    if (strcmp(pebbleId, "*") == 0){
        free_message_fields(gameId, pebbleId, team_name, player_name, NULL, NULL, NULL, NULL, guideId, hint);
        token_delete(token);
        return 1;
    }
    // Else if pebbleId is known player in the team, return 2
    else if (game_info_find_pebbleId(gi, pebbleId) != NULL){
        free_message_fields(gameId, pebbleId, team_name, player_name, NULL, NULL, NULL, NULL, guideId, hint);
        token_delete(token);
        return 2;
    }
    // Else if pebbleId is not known player in the team, return -7
    else {
        free_message_fields(gameId, pebbleId, team_name, player_name, NULL, NULL, NULL, NULL, guideId, hint);
        token_delete(token);
        return -7;
    }
}


int fn_game_status(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
    return -2;
}
int fn_gs_agent(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
    return -2;
}
int fn_gs_clue(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
    return -2;
}
int fn_gs_claimed(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
    return -2;
}
int fn_gs_secret(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
    return -2;
}
int fn_gs_response(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
    return -2;
}
int fn_game_over(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
    return -2;
}
int fn_team_record(char *rest_of_line, game_info_t *gi, struct sockaddr_in them){
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
    new_token->rest_of_line = rest_of_line;
    if (new_token->left == NULL || new_token->right == NULL){
        fprintf(stderr, "token_new failed due to not being able to malloc memory\n");
        return NULL;
    }
    
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
        token->rest_of_line = token->rest_of_line+i+1;
    }
    else{
        token->rest_of_line = '\0';
    }
}


static void token_delete(token_t *token){
    if (token == NULL){
        return;
    }
    free(token->rest_of_line);
    free(token->left);
    free(token->right);
    free(token);
}


static void
free_message_fields(char *gameId, char *pebbleId, char *team_name, char *player_name, char *latitude, char *longitude, char *statusReq, char *kragId, char *guideId, char *hint){
    if (gameId != NULL) free(gameId);
    if (pebbleId != NULL) free(pebbleId);
    if (team_name != NULL) free(team_name);
    if (player_name != NULL) free(player_name);
    if (latitude != NULL) free(latitude);
    if (longitude != NULL) free(longitude);
    if (statusReq != NULL) free(statusReq);
    if (kragId != NULL) free(kragId);
    if (guideId != NULL) free(guideId);
    if (hint != NULL) free(hint);
}
