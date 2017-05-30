/*
 * gsstruct.c - file with structs used in game server
 *
 * Kazuma Honjo, May 2017
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>
#include "set.h"
#include "file.h"
#include "common.h"
#include "gsstruct.h"

#define PI 3.14159265


/**************** file-local global variables ****************/
static const int MESSAGE_LENGTH = 8192;
static const int CLUE_LENGTH = 140;

/**************** Struct ****************/
// struct for game information
typedef struct game_info{
    time_t start;            // Time the game started
    int num_krags;           // Number of krags
    int num_agents;          // Number of agents (#FA + #GA)
    int num_teams;           // Number of teams (# of Struct Team)
    char *secret_code;       // Secret file string
    set_t *krags;            // Set of krags (clue will be the key)
    set_t *team;             // Set of Team (team name will be the key)
    unsigned int gameID;     // ID of the game
    int game_status;         // 0 if the game is continuing, 1 if ended
}game_info_t;


// struct for team information
typedef struct team {
    char *team_name;        // name of the team
    int num_agents;         // Number of agents (#FA + #GA)
    struct ga *ga;          // game agent
    set_t *fa;              // field agents
    int num_claimed_krags;  // number of claimed krags
    int num_revealed;       // number of revealed krags
    char *secret_string;    // partly revealed secret string
}team_t;


// struct for field agent information
typedef struct fa {
    char *name;                 // name
    char *pebbleID;             // pebble id
    struct sockaddr_in them;    // address of the socket
    float longitude;            // longitude of the player's position
    float latitude;             // latitude of the player's position
    time_t last_contact_time;   // the time of last contact
}fa_t;


// struct for game agent information
typedef struct ga {
    char *name;               // name
    char *guideID;            // guide ID
    struct sockaddr_in them;  // address of the socket
    time_t last_contact_time; // the time of last contact
}ga_t;


// struct for krag information
typedef struct krag {
    float longitude;           // longitude of the krag
    float latitude;            // latitude of the krag
    unsigned int kragID;       // ID of the krag
    int kragnumber;            // the number that appeared in the krag file
    char *clue;                // clue
    set_t *claimed_team;       // teams that have claimed this krag
    set_t *revealed_team;      // teams that have been revealed this krag
}krag_t;

// struct for finding Id
typedef struct find_id {
    team_t *team;
    fa_t *fa;
    char *id;
    struct sockaddr_in them;
}find_id_t;

// struct for sending message
typedef struct send_message {
    char *message;
    int comm_sock;
}send_message_t;

// struct for holding gi, one fa,
// ga, team and comm_sock in the team
typedef struct gifaga{
    game_info_t *gi;
    ga_t *ga;
    fa_t *fa;
    team_t *team;
    int comm_sock;
}gifaga_t;

// struct for finding krag to be revealed
typedef struct find_krag{
    struct krag *krag;
    int current;
    int target;
    char *team_name;
}find_krag_t;


/**************** file-local functions ****************/
static int handle_kiff_message(char *left, char *right, game_info_t *gi, krag_t *new_krag);
static void set_kiff_handle_error(char *left, char *right, FILE *fp, char *message);
static void find_pebbleId(void *arg, const char *key, void *item);
static void find_fa_with_pebbleId(void *arg, const char *key, void *item);
static void find_guideId(void *arg, const char *key, void *item);
static void send_message_to_everyone(void *arg, const char *key, void *item);
static void reveal_krag_helper(void *arg, const char *key, void *item);

static krag_t *krag_new();
static void krag_delete(void *item);
static team_t *team_new();
static void team_delete(void *item);
static fa_t *fa_new();
static void fa_delete(fa_t *fa);
static ga_t *ga_new();
static void ga_delete(ga_t *ga);
static void set_char_delete(void *item);
static void set_fa_delete(void *item);

//#ifdef DEBUG_TEST
static void krag_set_print(FILE *fp, const char *key, void *item);
//#endif


/*********************************************************/
/**************** functions for team *********************/
/*********************************************************/

/**************** team_new ****************/
/* return a new team
 * if error, return NULL
 */
static team_t *
team_new(){
    team_t *new_team = malloc(sizeof(team_t));
    if (new_team == NULL){
        fprintf(stderr, "team_new failed due to not being able to malloc memory\n");
        return NULL;
    }
    
    // initialize parameters
    new_team->ga = NULL;
    new_team->fa = set_new();
    new_team->num_claimed_krags = 0;
    new_team->num_revealed = 0;
    new_team->secret_string = malloc((CLUE_LENGTH) * sizeof(char));
    new_team->team_name = malloc((MESSAGE_LENGTH) * sizeof(char));
    return new_team;
}

/**************** team_delete ****************/
/* free the memory of the given team
 */
static void
team_delete(void *item){
    team_t *team = item;
    if (team == NULL){
        return;
    }
    set_delete(team->fa, &set_fa_delete);
    ga_delete(team->ga);
    free(team->secret_string);
    free(team->team_name);
    free(team);
}

/**************** team_find_fa ****************/
/* Return the fa named player_name
 * return NULL if not found
 */
fa_t *
team_find_fa(team_t *team, char *player_name){
    // get the fa and return (can be NULL)
    fa_t *fa = set_find(team->fa, player_name);
    printf("finding fa\n\n");
    return fa;
}

/**************** team_print ****************/
/* Print the data stored in team
 */
void
team_print(team_t *team){
    if (team == NULL){
        printf("No team to print\n");
        return;
    }
    printf("Team Name: %s\n", team->team_name);
    if (team->ga != NULL){
        printf("\tGuide: %s\n", team->ga->name);
    }
    printf("\tNumber of claimed crags: %d\n", team->num_claimed_krags);
    printf("\tSecret: %s\n", team->secret_string);
}

/**************** team_send_message_to_everyone ****************/
/* Send hint to all field agent in the team
 */
void
team_send_message_to_everyone(team_t *team, char *message, int comm_sock){
    if (team == NULL || message == NULL){
        return;
    }
    send_message_t *send_message = malloc(sizeof(send_message_t));
    if (send_message == NULL){
        fprintf(stderr, "team_send_message_to_everyone failed due to not being able to malloc memory\n");
        return;
    }
    send_message->message = message;
    send_message->comm_sock = comm_sock;
    set_iterate(team->fa, send_message, &send_message_to_everyone);
    free(send_message);
}

/**************** send_message_to_everyone ****************/
/* Helper function for team_send_message_to_everyone
 */
static void
send_message_to_everyone(void *arg, const char *key, void *item){
    send_message_t *send_message = arg;
    fa_t *fa = item;
    
    if (send_message == NULL || fa == NULL){
        return;
    }
    
    fa_send_to(fa, send_message->comm_sock, send_message->message);
}

/**************** team_send_gs_agent ****************/
/* Send GS_AGENT to guide agent
 */
void
team_send_gs_agent(game_info_t *gi, team_t *team, int comm_sock, void (*itemfunc)(void *arg, const char *key, void *item)){
    if (gi == NULL || team == NULL || itemfunc == NULL) return;
    
    // initialize struct for holding gi, ga
    gifaga_t *gifaga = malloc(sizeof(gifaga_t));
    gifaga->gi = gi;
    gifaga->ga = team->ga;
    gifaga->team = team;
    gifaga->comm_sock = comm_sock;
    
    // iterate the set and send gs_agent to ga
    set_iterate(team->fa, gifaga, itemfunc);
    
    // free memory
    free(gifaga);
}

/**************** team_get_guide ****************/
/* Return the guide agent if exists
 */
ga_t *
team_get_guide(team_t *team){
    if (team == NULL) return NULL;
    return team->ga;
}

/**************** team_get_guideID ****************/
/* Return the guideId if exists
 */
char *team_get_guideId(team_t *team){
    if (team == NULL) return NULL;
    if (team->ga == NULL) return NULL;
    char *guideId = malloc(MESSAGE_LENGTH);
    strcpy(guideId, team->ga->guideID);
    return guideId;
}

/**************** team_get_numClaimed ****************/
/* Return the number of claimed krags
 */
int
team_get_numClaimed(team_t *team){
    if (team == NULL) return 0;
    return team->num_claimed_krags;
}

/**************** team_get_numRevealed ****************/
/* Return the number of claimed krags
 */
int
team_get_numRevealed(team_t *team){
    if (team == NULL) return 0;
    return team->num_revealed;
}

/**************** team_get_secret ****************/
/* Return the secret string for this team
 */
char *
team_get_secret(team_t *team){
    if (team == NULL) return NULL;
    char *secret = malloc(CLUE_LENGTH);
    strcpy(secret, team->secret_string);
    return secret;
}

/**************** team_get_name ****************/
/* return name of the team
 */
char *
team_get_name(team_t *team){
    if (team == NULL) return NULL;
    char *team_name = malloc(MESSAGE_LENGTH);
    strcpy(team_name, team->team_name);
    return team_name;
}

/**************** team_update_string ****************/
/* Update the secret string for the given team based on
 * given krag
 */
void
team_update_string(game_info_t *gi, team_t *team, krag_t *krag){
    if (gi == NULL || team == NULL || krag == NULL) return;
    int k = gi->num_krags;
    int i = krag->kragnumber;
    for (int j = 0; j < strlen(gi->secret_code); j++){
        if (j%k == i){
            team->secret_string[j] = gi->secret_code[j];
        }
    }
}

/*********************************************************/
/**************** functions for game_info ****************/
/*********************************************************/

/**************** game_info_new ****************/
/* return a new game info 
 * if error, return NULL
 */
game_info_t *
game_info_new(){
    game_info_t *gi = malloc(sizeof(game_info_t));
    if (gi == NULL){
        fprintf(stderr, "game_info_new failed due to not being able to malloc memory\n");
        return NULL;
    }
    
    // initialize parameters
    gi->start = time(NULL);
    gi->num_krags = 0;
    gi->num_agents = 0;
    gi->num_teams = 0;
    gi->krags = set_new();
    gi->team = set_new();
    gi->gameID = 0;
    gi->game_status = 0;
    gi->secret_code = malloc(CLUE_LENGTH);
    return gi;
}

/**************** game_info_delete ****************/
/* delete the game info
 */
void
game_info_delete(game_info_t *gi){
    if (gi == NULL){
        return;
    }
    
    // free all memory
    free(gi->secret_code);
    set_delete(gi->krags, &krag_delete);
    set_delete(gi->team, &team_delete);
    free(gi);
}

/**************** game_info_get_start_time ****************/
/* return a start time of game */
time_t
game_info_get_start_time(game_info_t *gi){
    if (gi == NULL){
        return -1;
    }
    return gi->start;
}

/**************** game_info_get_game_status ****************/
/* return a status of game */
int
game_info_get_game_status(game_info_t *gi){
    if (gi == NULL){
        return -1;
    }
    return gi->game_status;
}

/**************** game_info_get_game_gameId ****************/
/* return gameId in Hex*/
char *
game_info_get_gameId(game_info_t *gi){
    if (gi == NULL){
        return NULL;
    }
    char *gameid = decToStringHex(gi->gameID);
    return gameid;
}

/**************** game_info_get_numKrags ****************/
/* return number of krags in this game*/
int
game_info_get_numKrags(game_info_t *gi){
    if (gi == NULL) return 0;
    return gi->num_krags;
}

/**************** game_info_set_gameID ****************/
/* set the gameID 
 * return 0 if success, 1 if error
 */
int
game_info_set_gameID(game_info_t *gi, char *gameID_in_hex){
    if (gi == NULL || gameID_in_hex == NULL){
        return 1;
    }
    gi->gameID = stringHexToDec(gameID_in_hex);
    return 0;
}

/**************** game_info_get_secret ****************/
/* return secret code of this game*/
char *
game_info_get_secret(game_info_t *gi){
    if (gi == NULL) return NULL;
    char *secret = malloc(MESSAGE_LENGTH);
    strcpy(secret, gi->secret_code);
    return secret;
}


/**************** game_info_change_game_status ****************/
/* change the status of game to 1 */
void
game_info_change_game_status(game_info_t *gi){
    if (gi == NULL){
        return;
    }
    gi->game_status = 1;
}

/**************** game_info_set_kiff ****************/
/* set the kiff and builds the set of krags
 * return 0 if success, 1 if error
 */
int
game_info_set_kiff(game_info_t *gi, char *kiff){
    if (gi == NULL || kiff == NULL){
        return 1;
    }
    // Open the file. If it does not exist or cannot be read,
    // wrong file path name. Return 1
    FILE *fp = fopen(kiff, "r");
    if (fp == NULL){
        fprintf(stderr, "kiff cannot be opened or does not exist\n");
        return 1;
    }
    
    int num_krags = 0;
    char *line;
    // read all lines in the file
    while ((line = readlinep(fp)) != NULL){
        // allocate memory
        // each word must be shorter than the length of line
        char *left = malloc(strlen(line)+1);
        char *right = malloc(strlen(line)+1);
        if (left == NULL || right == NULL){
            fprintf(stderr, "Error while allocating memory in game_info_set_kiff\n");
            return 1;
        }
        int j = 0;
        int state = 0; // 0 indiactes currently reading the left-hand-side
                       // 1 indicates currently reading the right-hand-side
        
        krag_t *new_krag = krag_new();
        if (new_krag == NULL){
            return 1;
        }
        new_krag->kragnumber = num_krags;
        
        // loop over the line
        for (int i = 0; i < strlen(line); i++){
            if (line[i] == '='){
                if (state == 0){
                    state = 1;
                    left[j] = '\0';
                    j = 0;
                }
                // if currently reading right hand side,
                // the next non-alphabet/number character must be '|'
                else {
                    set_kiff_handle_error(left, right, fp, "Error: '=' found consecutively in the kiff\n");
                    return 1;
                }
            }
            else if (line[i] == '|'){
                if (state == 1){
                    state = 0;
                    right[j] = '\0';
                    j = 0;
                    if (handle_kiff_message(left, right, gi, new_krag) != 0){
                        set_kiff_handle_error(left, right, fp, "Error: krag format error\n");
                        return 1;
                    }
                }
                // if currently reading left hand side,
                // the next non-alphabet/number character must be '='
                else {
                    set_kiff_handle_error(left, right, fp, "Error: '|' found consecutively in the kiff\n");
                    return 1;
                }
            }
            // when i reaches the end of the line
            else if (i == strlen(line)-1){
                // it should be reading the right hand side
                if (state == 1){
                    right[j] = '\0';
                    if (handle_kiff_message(left, right, gi, new_krag) != 0){
                        set_kiff_handle_error(left, right, fp, "Error: krag format error\n");
                        return 1;
                    }
                }
                else {
                    set_kiff_handle_error(left, right, fp, "Error: krag format error\n");
                    return 1;
                }
            }
            else {
                // if currently reading the left-hand-side
                if (state == 0){
                    left[j] = line[i];
                }
                else{
                    right[j] = line[i];
                }
                j++;
            }
        }
        
        char *val = decToStringHex(new_krag->kragID);
        set_insert(gi->krags, val, new_krag);
        num_krags += 1;
        free(val);
        free(left);
        free(line);
    }
    
    fclose(fp);
    gi->num_krags = num_krags;
    return 0;
}

/**************** set_kiff_handle_error ****************/
/* handle the error recieved for game_info_set_kiff
 */
static void
set_kiff_handle_error(char *left, char *right, FILE *fp, char *message){
    if (left != NULL) free(left);
    if (right != NULL) free(right);
    if (fp != NULL) fclose(fp);
    if (message != NULL) fprintf(stderr, message);
}

/**************** handle_kiff_message ****************/
/* helper function for game_info_set_kiff
 * handle the message properly
 * return 0 if success, 1 if error
 */
static int
handle_kiff_message(char *left, char *right, game_info_t *gi, krag_t *new_krag){
    if ((left == NULL) || (right == NULL) || (gi == NULL) || (new_krag == NULL)){
        return 1;
    }

    // check the left-hand-side word and set the value of it
    // to the value of right-hand-side
    if (strcmp(left, "latitude") == 0){
        new_krag->latitude = atof(right);
    }
    else if (strcmp(left, "longitude") == 0){
        new_krag->longitude = atof(right);
    }
    else if (strcmp(left, "kragId") == 0){
        new_krag->kragID = stringHexToDec(right);
    }
    else if ((strcmp(left, "clue") == 0) && (strlen(right) <= (CLUE_LENGTH-1))){
        new_krag->clue = right;
    }
    // if the left-hand-side word does not fall in to the words above,
    // the krag file format is not correct
    else{
        krag_delete(new_krag);
        return 1;
    }
    return 0;
}

/**************** game_info_set_secret_code ****************/
/* set the secret code
 * return 0 if success, 1 if error
 */
int
game_info_set_secret_code(game_info_t *gi, char *sf){
    if (gi == NULL || sf == NULL){
        return 1;
    }
    
    // Open the file. If it does not exist or cannot be read,
    // wrong file path name. Return 1
    FILE *fp = fopen(sf, "r");
    if (fp == NULL){
        fprintf(stderr, "sf cannot be opened or does not exist\n");
        return 1;
    }
    char *line = readlinep(fp);
    if (line == NULL || strlen(line) > (CLUE_LENGTH-1)){
        fprintf(stderr, "Error: the format of sf is wrong\n");
        return 1;
    }
    printf("%s\n\n", line);
    strcpy(gi->secret_code, line);
    free(line);
    fclose(fp);
    return 0;
}

/**************** game_info_find_team ****************/
/* Return the struct team_t associated with the given team name;
 * return NULL if not found.
 */
team_t *
game_info_find_team(game_info_t *gi, char *team_name){
    // get the team and return it (can be NULL)
    team_t *team = set_find(gi->team, team_name);
    return team;
}

/**************** game_info_register_team ****************/
/* Add a new team to set of teams in game_info
 * Ignore if the team with same name exists or any error
 */
void
game_info_register_team(game_info_t *gi, char *team_name){
    // if the team is not found in the game, add the team
    if (game_info_find_team(gi, team_name) == NULL){
        team_t *new_team = team_new();
        strcpy(new_team->team_name, team_name);
        set_insert(gi->team, team_name, new_team);
        
        // represent the secret code by '_'
        char *buf = malloc(CLUE_LENGTH);
        for (int i = 0; i < strlen(gi->secret_code); i++){
            buf[i] = '_';
        }
        buf[strlen(gi->secret_code)] = '\0';
        strcpy(new_team->secret_string, buf);
        free(buf);
        gi->num_teams += 1;
    }
}

/**************** game_info_find_fa_pebbleId ****************/
/* Return the fa with given pebbleId
 * Return NULL if it does not exist
 */
fa_t *
game_info_find_fa_pebbleId(game_info_t *gi, char *pebbleId){
    find_id_t *find_id = malloc(sizeof(find_id_t));
    find_id->id = malloc(MESSAGE_LENGTH);
    strcpy(find_id->id, pebbleId);
    find_id->fa = NULL;
    find_id->team = NULL;
    
    // interate over all the teams and find the fa
    // associated with the given pebbleId
    set_iterate(gi->team, find_id, &find_pebbleId);
    fa_t *fa = find_id->fa;
    free(find_id->id);
    free(find_id);
    
    // return the fa (can be NULL)
    return fa;
}

/**************** game_info_find_pebbleId ****************/
/* Return the team that has field agent with given pebbleId
 * Return NULL if it does not exist
 */
team_t *
game_info_find_pebbleId(game_info_t *gi, char *pebbleId){
    find_id_t *find_id = malloc(sizeof(find_id_t));
    find_id->id = malloc(MESSAGE_LENGTH);
    strcpy(find_id->id, pebbleId);
    find_id->fa = NULL;
    find_id->team = NULL;
    
    // interate over all the teams and find the fa
    // associated with the given pebbleId
    set_iterate(gi->team, find_id, &find_pebbleId);
    team_t *team = find_id->team;
    free(find_id->id);
    free(find_id);
    
    // return the team (can be NULL)
    return team;
}

/**************** find_pebbleId ****************/
/* Helper function for game_info_find_pebbleId
 */
static void
find_pebbleId(void *arg, const char *key, void *item){
    // initialize parameters
    find_id_t *find_id = arg;
    team_t *team = item;
    
    // if the input is NULL, do nothing
    if (find_id == NULL || team == NULL){
        return;
    }
    
    // iterate over the fa in this team to find the player
    // if it is still not found
    if (find_id->fa == NULL){
        set_iterate(team->fa, find_id, &find_fa_with_pebbleId);
        // if it was found, set the team
        if (find_id->fa != NULL){
            find_id->team = team;
        }
    }
}

/**************** find_fa_with_pebbleId ****************/
/* Helper function for find_pebbleId
 */
static void
find_fa_with_pebbleId(void *arg, const char *key, void *item){
    find_id_t *find_id = arg;
    fa_t *fa = item;
    
    // if the input is NULL, do nothing
    if (find_id == NULL || fa == NULL){
        return;
    }
    
    if ( strcmp(fa->pebbleID,find_id->id) == 0 ){
        find_id->fa = fa;
    }
}

/**************** game_info_find_guideId ****************/
/* Return the team that has guide agent with given guideId
 * Return NULL if it does not exist
 */
team_t *
game_info_find_guideId(game_info_t *gi, char *guideId){
    find_id_t *find_id = malloc(sizeof(find_id_t));
    find_id->id = guideId;
    find_id->fa = NULL;
    find_id->team = NULL;
    
    // interate over all the teams and find the ga
    // associated with the given guideId
    set_iterate(gi->team, find_id, &find_guideId);
    team_t *team = find_id->team;
    free(find_id);
    
    // return the team (can be NULL)
    return team;
}

/**************** find_guideId ****************/
/* Helper function for game_info_find_guideId
 */
static void
find_guideId(void *arg, const char *key, void *item){
    // initialize parameters
    find_id_t *find_id = arg;
    team_t *team = item;
    
    // if the input is NULL, do nothing
    if (find_id == NULL || team == NULL){
        return;
    }
    
    // iterate over the fa in this team to find the player
    // if it is still not found
    if (find_id->team == NULL){
        if (team->ga != NULL){
            if (strcmp(team->ga->guideID,find_id->id) == 0){
                find_id->team = team;
            }
        }
    }
}

/**************** team_register_ga ****************/
/* register ga to the team
 * return 0 if successfully registered
 * return -5 if the guide is already registered in the team
 * return -7 if the guideId is already registered
 */
int
team_register_ga(game_info_t *gi, team_t *team, char *guideId, char *player_name, struct sockaddr_in them){
    // two guide agent in one team is not allowed
    team_print(team);
    if (team->ga != NULL){
        return -8;
    }
    
    // if the player with the given guideId is not found
    if (game_info_find_guideId(gi,guideId) == NULL){
        ga_t *ga = ga_new();
        if (ga == NULL){
            return -99;
        }
        strcpy(ga->name, player_name);
        strcpy(ga->guideID, guideId);
        ga->them = them;
        ga->last_contact_time = time(NULL);
        team->ga = ga;
        team->num_agents += 1;
        gi->num_agents += 1;
        return 0;
    }
    // if the player with the given pebbleId is found, it will not be registered
    // invalid id
    else {
        return -7;
    }
}

/**************** team_register_fa ****************/
/* register fa to the team
 * return 0 if successfully registered
 * return -6 if player with same name exists in the team
 * return -7 if the pebbleId is already registered
 */
int
team_register_fa(game_info_t *gi, team_t *team, char *pebbleId, char *player_name, struct sockaddr_in them, char *latitude, char *longitude){
    // same name in any given team is not allowed
    if (team_find_fa(team, player_name) != NULL){
        return -6;
    }
    
    // if the player with the given pebbleId is not found
    if (game_info_find_pebbleId(gi, pebbleId) == NULL){
        fa_t *fa = fa_new();
        if (fa == NULL){
            return -99;
        }
        strcpy(fa->pebbleID, pebbleId);
        strcpy(fa->name, player_name);
        fa->them = them;
        fa->latitude = atof(latitude);
        fa->longitude = atof(longitude);
        fa->last_contact_time = time(NULL);
        set_insert(team->fa, fa->name, fa);
        team->num_agents += 1;
        gi->num_agents += 1;
        return 0;
    }
    // if the player with the given pebbleId is found, it will not be registered
    // invalid id
    else {
        return -7;
    }
}

/**************** game_info_validate ****************/
/* Validate gameId, pebbleId, team name and player name.
 * pebbleId is interchangable with guideId
 * latitude and longitude can be NULL if it is validating 
 * guide agent
 * Return 0 if all correct.
 * Return -4 if gameId is incorrect
 * Return -5 if team name not found
 * Return -6 if player not found in the team
 * Return -7 if pebbleId is incorrect
 */
int
game_info_validate(game_info_t *gi, char *gameId, char *pebbleId, char *team_name, char *player_name, char *latitude, char *longitude){
    // if any input is NULL, wrong message; return -1
    if (gi == NULL || gameId == NULL || pebbleId == NULL || team_name == NULL || player_name == NULL){
        return -1;
    }
    
    // if the gameId does not match, return -4
    if (gi->gameID != stringHexToDec(gameId)) return -4;
    
    // if team does not exist, return -5
    team_t *team = game_info_find_team(gi, team_name);
    if (team == NULL) return -5;
    
    ga_t *ga = NULL;
    fa_t *fa = NULL;
    // get ga is the name is the same
    if (team->ga != NULL){
        if (strcmp(team->ga->name, player_name) == 0){
            ga = team->ga;
        }
    }
    // if ga's name is not the same with the given name,
    // find the fa with the same name
    if (ga == NULL){
        fa = team_find_fa(team, player_name);
    }
    // if player is not found, return -6
    if ((fa == NULL) && (ga == NULL)) return -6;
    
    // if the fa is found
    else if (fa != NULL){
        // if the pebbleId does not match, return -7
        if (strcmp(fa->pebbleID,pebbleId) != 0) return -7;
        
        // if any input is NULL, wrong message; return -1
        if (latitude == NULL || longitude == NULL) return -1;
        
        // update
        fa->latitude = atof(latitude);
        fa->longitude = atof(longitude);
        fa->last_contact_time = time(NULL);
        return 0;
    }
    
    // if the ga is found
    else{
        // if the guideId does not match, return -7
        if (strcmp(ga->guideID,pebbleId) != 0) return -7;
        
        // update
        ga->last_contact_time = time(NULL);
        return 0;
    }
}

/**************** game_info_find_krag ****************/
/* Examine if there is a krag in the set of krag or not
 * Return the krag_t * if exists.
 * Return NULL if not found
 */
krag_t *
game_info_find_krag(game_info_t *gi, char *kragId){
    unsigned int kragID = stringHexToDec(kragId);
    kragId = decToStringHex(kragID);
    krag_t *krag = set_find(gi->krags, kragId);
    free(kragId);
    return krag;
}

/**************** game_info_krag_distance ****************/
/* Examine if there is the krag is within 10 meters from the 
 * given latitude and longitude
 * Return 0 if the krag is located within 10m.
 * Return 1 if krag is not found, or is not located within 10m.
 */
int
game_info_krag_distance(game_info_t *gi, char *kragId, char *latitude, char *longitude){
    krag_t *krag = game_info_find_krag(gi, kragId);
    // make sure that the krag exists
    if (krag != NULL){
        // calculate the distance in meters. quoted from
        // https://stackoverflow.com/questions/837872/calculate-distance-in-meters-when-you-know-longitude-and-latitude-in-java
        // Retrieved on May 25, 2017
        double earthRadius = 6371000; //meters
        double val = PI / 180;
        double dLat = (double) (atof(latitude) - krag->latitude) * val;
        double dLng = (double) (atof(longitude) - krag->longitude) * val;
        double a = sin(dLat/2) * sin(dLat/2) + cos(atof(latitude) * val) * cos((krag->latitude) * val) * sin(dLng/2) * sin(dLng/2);
        double c = 2 * atan2(sqrt(a), sqrt(1-a));
        float dist = (float) (earthRadius * c);
        
        // if the distance is within 10m, return 0
        if (dist <= 10){
            return 0;
        }
    }
    return 1;
}

/**************** game_info_reveal_krag ****************/
/* Reveal krag and return the krag that was revealed
 * Return NULL if error
 */
krag_t *
game_info_reveal_krag(game_info_t *gi, team_t *team){
    if (gi == NULL || team == NULL) return NULL;
    
    // if all krags are revealed,
    // end the game and return NULL
    if (gi->num_krags <= team->num_revealed) {
        game_info_change_game_status(gi);
        return NULL;
    }
    

    find_krag_t *find_krag = malloc(sizeof(find_krag_t));
    find_krag->team_name = team->team_name;
    find_krag->krag = NULL;
    // loop the set of krag until it finds a krag that is not
    // revealed to the team
    do{
        find_krag->target = rand()%(game_info_get_numKrags(gi));
        find_krag->current = 0;
        set_iterate(gi->krags, find_krag, &reveal_krag_helper);
    }while (find_krag->krag == NULL);
    
    krag_t *krag;
    krag = find_krag->krag;
    team->num_revealed += 1;
    free(find_krag);
    return krag;
}

/**************** reveal_krag_helper ****************/
/* Helper function for game_info_reveal_krag
 */
static void
reveal_krag_helper(void *arg, const char *key, void *item){
    krag_t *krag = item;
    find_krag_t *find_krag = arg;
    if (krag == NULL || find_krag == NULL) return;
    
    if ((find_krag->current == find_krag->target) && (find_krag->krag == NULL)){
        if (krag_has_revealed(krag, find_krag->team_name) == 1){
            find_krag->krag = krag;
            char *team_name_to_be_inserted = malloc(MESSAGE_LENGTH);
            strcpy(team_name_to_be_inserted, find_krag->team_name);
            set_insert(krag->revealed_team, find_krag->team_name, team_name_to_be_inserted);
        }
    }
    else {
        find_krag->current++;
    }
}

/**************** game_info_send_message_to_everyone ****************/
/* Send message to all agents in the game
 */
void
game_info_send_message_to_everyone(game_info_t *gi, char *message, int comm_sock, void (*itemfunc)(void *arg, const char *key, void *item)){
    if (gi == NULL || message == NULL){
        return;
    }
    
    send_message_t *send_message = malloc(sizeof(send_message_t));
    if (send_message == NULL){
        fprintf(stderr, "team_send_message_to_everyone failed due to not being able to malloc memory\n");
        return;
    }
    send_message->message = message;
    send_message->comm_sock = comm_sock;
    
    set_iterate(gi->team, send_message, itemfunc);
    
    free(send_message);
}


/*********************************************************/
/**************** functions for krag *********************/
/*********************************************************/

/**************** krag_new ****************/
/* return a new krag
 * if error, return NULL
 */
static krag_t *
krag_new(){
    krag_t *krag = malloc(sizeof(krag_t));
    if (krag == NULL){
        fprintf(stderr, "krag_new failed due to not being able to malloc memory\n");
        return NULL;
    }
    
    // initialize parameters
    krag->longitude = 0;
    krag->latitude = 0;
    krag->kragID = 0;
    krag->claimed_team = set_new();
    krag->revealed_team = set_new();
    return krag;
}

/**************** krag_delete ****************/
/* free the memory of the given krag
 */
static void
krag_delete(void *item){
    krag_t *krag = item;
    if (krag == NULL){
        return;
    }
    set_delete(krag->claimed_team, &set_char_delete);
    set_delete(krag->revealed_team, &set_char_delete);
    free(krag->clue);
    free(krag);
}

/**************** krag_get_longitude ****************/
/* Return the longitue of the krag
 * return 1000 if it does not exist
 */
float
krag_get_longitude(krag_t *krag){
    if (krag == NULL) return 1000;
    return krag->longitude;
}

/**************** krag_get_latitude ****************/
/* Return the latitude of the krag
 * return 1000 if it does not exist
 */
float
krag_get_latitude(krag_t *krag){
    if (krag == NULL) return 1000;
    return krag->latitude;
}

/**************** krag_get_kragId ****************/
/* Return the kragId of the krag
 * return 0 if it does not exist
 */
unsigned int
krag_get_kragId(krag_t *krag){
    if (krag == NULL) return 0;
    return krag->kragID;
}

/**************** krag_get_clue ****************/
/* Return the clue of the krag
 * return NULL if it does not exist
 */
char *
krag_get_clue(krag_t *krag){
    if (krag == NULL) return NULL;
    char *clue = malloc(CLUE_LENGTH);
    strcpy(clue, krag->clue);
    return clue;
}


/**************** krag_has_claimed ****************/
/* check if the krag has claimed by the team or not
 * Return 0 if claimed, 1 if not
 * Return -1 if error
 */
int
krag_has_claimed(krag_t *krag, char *team_name){
    if (krag == NULL || team_name == NULL){
        return -1;
    }
    
    // if team not found in the set of claimed_teams,
    // return NULL
    if (set_find(krag->claimed_team, team_name) == NULL){
        return 1;
    }
    return 0;
}

/**************** krag_has_revealed ****************/
/* check if the krag has revealed by the team or not
 * Return 0 if revealed, 1 if not
 * Return -1 if error
 */
int
krag_has_revealed(krag_t *krag, char *team_name){
    if (krag == NULL || team_name == NULL){
        return -1;
    }
    
    // if team not found in the set of claimed_teams,
    // return NULL
    if (set_find(krag->revealed_team, team_name) == NULL){
        return 1;
    }
    return 0;
}


/**************** krag_mark_claimed ****************/
/* Mark the krag has claimed for the given krag and team
 * Return 1 if there are more krags to be claimed after marked
 * Return 2 if this was the last krag to be claimed
 * Return 0 if error
 */
int
krag_mark_claimed(game_info_t *gi, krag_t *krag, char *team_name){
    if (krag_has_claimed(krag, team_name) == 1){
        team_t *team = game_info_find_team(gi, team_name);
        if (set_find(krag->claimed_team, team_name) == NULL){
            char *team_name_to_be_inserted = malloc(MESSAGE_LENGTH);
            strcpy(team_name_to_be_inserted,team_name);
            set_insert(krag->claimed_team, team_name, team_name_to_be_inserted);
            team->num_claimed_krags += 1;
        }
        if (set_find(krag->revealed_team, team_name) == NULL){
            char *team_name_to_be_inserted2 = malloc(MESSAGE_LENGTH);
            strcpy(team_name_to_be_inserted2,team_name);
            set_insert(krag->revealed_team, team_name, team_name_to_be_inserted2);
            team->num_revealed += 1;
        }
        
        team_print(team);
        printf("%d %d",team->num_claimed_krags ,gi->num_krags);
        
        // If there are more krags to be claimed
        if (team->num_claimed_krags < gi->num_krags){
            return 1;
        }
        // If all krags are claimed
        return 2;
    }
    
    return 0;
}

/*********************************************************/
/****************** functions for fa *********************/
/*********************************************************/

/**************** fa_new ****************/
/* return a new fa
 * if error, return NULL
 */
static fa_t *
fa_new(){
    fa_t *new_fa = malloc(sizeof(fa_t));
    if (new_fa == NULL){
        fprintf(stderr, "fa_new failed due to not being able to malloc memory\n");
        return NULL;
    }
    
    // initialize parameters
    new_fa->name = malloc(MESSAGE_LENGTH * sizeof(char));
    new_fa->pebbleID = malloc(MESSAGE_LENGTH * sizeof(char));
    return new_fa;
}

/**************** fa_delete ****************/
/* free the memory of the given fa
 */
static void
fa_delete(fa_t *fa){
    if (fa == NULL){
        return;
    }
    free(fa->name);
    free(fa->pebbleID);
    free(fa);
}

/**************** fa_print ****************/
/* Print the data stored in team
 */
void
fa_print(fa_t *fa){
    if (fa == NULL){
        printf("No fa to print\n");
        return;
    }
    printf("fa name: %s\n", fa->name);
    printf("\tlongitude: %lf\n\tlatitude: %lf", fa->longitude, fa->latitude);
}

/**************** fa_send_to ****************/
/* send message to field agent
 */
void
fa_send_to(fa_t *fa, int comm_sock, char *message){
    if (fa == NULL || message == NULL) return;
    if (sendto(comm_sock, message, strlen(message), 0, (struct sockaddr *) &(fa->them), sizeof(fa->them)) < 0) {
        perror("sending in datagram socket");
        exit(6);
    }
}

/**************** fa_get_pebbleId ****************/
/* return pebbleId of the fa
 */
char *
fa_get_pebbleId(fa_t *fa){
    if (fa == NULL) return NULL;
    char *pebbleId = malloc(MESSAGE_LENGTH);
    strcpy(pebbleId, fa->pebbleID);
    return pebbleId;
}

/**************** fa_get_name ****************/
/* return name of the fa
 */
char *
fa_get_name(fa_t *fa){
    if (fa == NULL) return NULL;
    char *name = malloc(MESSAGE_LENGTH);
    strcpy(name, fa->name);
    return name;
}

/**************** fa_get_latitude ****************/
/* return latitude of the fa
 * return 1000 if error
 */
float
fa_get_latitude(fa_t *fa){
    if (fa == NULL) return 1000;
    return fa->latitude;
}

/**************** fa_get_longitude ****************/
/* return longitude of the fa
 * return 1000 if error
 */
float
fa_get_longitude(fa_t *fa){
    if (fa == NULL) return 1000;
    return fa->longitude;
}

/**************** fa_get_time ****************/
/* return the time elapsed (seconds in int)
 * since last contact by the fa
 */
int
fa_get_time(fa_t *fa){
    if (fa == NULL) return -1;
    time_t last = fa->last_contact_time;
    time_t now = time(NULL);
    return (int)difftime(now , last);
}


/*********************************************************/
/****************** functions for ga *********************/
/*********************************************************/

/**************** ga_new ****************/
/* return a new ga
 * if error, return NULL
 */
static ga_t *
ga_new(){
    ga_t *new_ga = malloc(sizeof(ga_t));
    if (new_ga == NULL){
        fprintf(stderr, "ga_new failed due to not being able to malloc memory\n");
        return NULL;
    }
    
    // initialize parameters
    new_ga->name = malloc(MESSAGE_LENGTH * sizeof(char));
    new_ga->guideID = malloc(MESSAGE_LENGTH * sizeof(char));
    return new_ga;
}

/**************** ga_delete ****************/
/* free the memory of the given fa
 */
static void
ga_delete(ga_t *ga){
    if (ga == NULL){
        return;
    }
    free(ga->name);
    free(ga->guideID);
    free(ga);
}

/**************** ga_get_id ****************/
/* return the guideId
 */
char *
ga_get_id(ga_t *ga){
    if (ga == NULL) return NULL;
    return ga->guideID;
}

/**************** ga_send_to ****************/
/* send message to guide agent
 */
void
ga_send_to(ga_t *ga, int comm_sock, char *message){
    if (ga == NULL || message == NULL) return;
    if (sendto(comm_sock, message, strlen(message), 0, (struct sockaddr *) &(ga->them), sizeof(ga->them)) < 0) {
        perror("sending in datagram socket");
        exit(6);
    }
}

/*********************************************************/
/**************** functions for gifaga *******************/
/*********************************************************/
/* return gi */
game_info_t *
gifaga_get_gi(gifaga_t *gifaga){
    if (gifaga == NULL) return NULL;
    return gifaga->gi;
}

/* return ga */
ga_t *
gifaga_get_ga(gifaga_t *gifaga){
    if (gifaga == NULL) return NULL;
    return gifaga->ga;
}

/* return team */
team_t *
gifaga_get_team(gifaga_t *gifaga){
    if (gifaga == NULL) return NULL;
    return gifaga->team;
}

/* return comm_sock */
int
gifaga_get_comm_sock(gifaga_t *gifaga){
    if (gifaga == NULL) return -1;
    return gifaga->comm_sock;
}

/*********************************************************/
/************* functions for send_message ****************/
/*********************************************************/
/* return message */
char *
send_message_get_message(send_message_t *send_message){
    if (send_message == NULL) return NULL;
    char *message = malloc(MESSAGE_LENGTH);
    strcpy(message, send_message->message);
    return message;
}

/* return comm_sock */
int
send_message_get_comm_sock(send_message_t *send_message){
    if (send_message == NULL) return -1;
    return send_message->comm_sock;
}



/*********************************************************/
/********** helper functions for set_delete **************/
/*********************************************************/

/**************** set_char_delete ****************/
/* helper function for deleting char item in set
 */
static void
set_char_delete(void *item){
    char *string = item;
    if (string != NULL){
        free(string);
    }
}

/**************** set_fa_delete ****************/
/* helper function for deleting fa item in set
 */
static void
set_fa_delete(void *item){
    fa_t *fa = item;
    fa_delete(fa);
}


/*********************************************************/
/*********** helper functions for debugging **************/
/*********************************************************/

/**************** krag_print ****************/
/* Print the data stored in krag
 */
void
krag_print(krag_t *krag){
    if (krag == NULL) {
        printf("NULL KRAG");
        return;
    }
    char *val = decToStringHex(krag->kragID);
    printf("kragID: %s\n\tlongitude: %f\n\tlatitude: %f\n\tclue: %s\n\n", val, krag->longitude, krag->latitude, krag->clue);
    free(val);
}

static void
krag_set_print(FILE *fp, const char *key, void *item){
    krag_t *krag = item;
    printf("key: %s\n", key);
    krag_print(krag);
}


