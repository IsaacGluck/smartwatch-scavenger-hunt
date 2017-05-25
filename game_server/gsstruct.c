/*
 * gsstruct.c - file with structs used in game server
 *
 * Kazuma Honjo, May 2017
 *
 */

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "set.h"
#include "file.h"

/**************** file-local global variables ****************/
static const int MESSAGE_LENGTH = 8192;
static const int CLUE_LENGTH = 140;

/**************** Struct ****************/
// struct for game information
typedef struct game_info{
    time_t start;     // Time the game started
    int num_krags;    // Number of krags
    int num_agents;   // Number of agents (#FA + #GA)
    int num_teams;    // Number of teams (# of Struct Team)
    char *secret_code;// Secret file string
    char *kiff;       // Krag file path name
    set_t *krags;     // Set of krags (clue will be the key)
    set_t *team;      // Set of Team (team name will be the key)
    int gameID;       // ID of the game
    int game_status;  // 0 if the game is continuing, 1 if ended
}game_info_t;


// struct for team information
typedef struct team {
    struct ga *ga;          // game agent
    set_t *fa;              // field agents
    int num_claimed_krags;  // number of claimed krags
    set_t *revealed_krags;  // revealed krags
    char *secret_string;    // partly revealed secret string
}team_t;


// struct for field agent information
typedef struct fa {
    char *name;          // name
    int pebbleID;        // pebble id
    sockaddr_in them;    // address of the socket
    float longitude;     // longitude of the player's position
    float latitude;      // latitude of the player's position
    time_t last_contact_time;// the time of last contact
}fa_t;


// struct for game agent information
typedef struct ga {
    char *name;               // name
    int guideID;              // guide ID
    sockaddr_in them;         // address of the socket
    time_t last_contact_time; // the time of last contact
}ga_t;


// struct for krag information
typedef struct krag {
    float longitude;    // longitude of the krag
    float latitude;     // latitude of the krag
    int kragID;         // ID of the krag
    char *clue;         // clue
    set_t *claimed_team;// teams that have claimed this krag
}krag_t;

// struct for finding Id
struct find_id {
    team_t *team;
    fa_t *fa;
    int id;
}find_id_t;

/**************** file-local functions ****************/
static int handle_kiff_message(char *left, char *right, game_info_t *gi, krag_t *new_krag);
static void set_kiff_handle_error(char *left, char *right, FILE *fp, char *message);
static team_t *game_info_find_pebbleId(game_info_t *gi, char *pebbleId);
static void find_pebbleId(void *arg, const char *key, void *item);
static krag_t *krag_new();
static void krag_delete(krag_t *krag);
static team_t *team_new();
static void team_delete(team_t *team);
static void set_char_delete(void *item);


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
    return gi;
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

/**************** game_info_set_gameID ****************/
/* set the gameID 
 * return 0 if success, 1 if error
 */
int
game_info_set_gameID(game_info_t *gi, char *gameID_in_hex){
    if (gi == NULL || gameID_in_hex == NULL){
        return 1;
    }
    gi->gameID = intToHex(gameID_in_hex);
    return 0;
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
        
        // loop over the line
        for (int i = 0; i < strlen(line); i++){
            if (isspace(line[i])){
                set_kiff_handle_error(left, right, fp, "Error: space found in the kiff\n");
                return 1;
            }
            
            if (line[i] == '='){
                if (state == 0){
                    state = 1;
                    left[j] = '\0';
                    j = 0;
                }
                // if currently reading right hand side,
                // the next non-alphabet/number character must be '|'
                else {
                    et_kiff_handle_error(left, right, fp, "Error: '=' found consecutively in the kiff\n");
                    return 1;
                }
            }
            if (line[i] == '|'){
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
        //set_insert(gi->krags, int_to_hex(new_krag->kragID), new_krag);
        free(left);
        free(right);
        free(line);
    }
    
    fclose(fp);
    gi->kiff = kiff;
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
        ///new_krag->kragID = hex_to_int(right);
        // oaiu 0-uwnaf pawh09 na[=9w u\ca\ 0    uawpycpw4e y[a weifypacw fyp awo4y 8pofy pw8cfapw pcy aweyf aowy pfy pway fo pwf9 yw90 fyy w390fy w9eh fhfuw 0f9awunv9nnf
    }
    else if ((strcmp(left, "clue") == 0) && (strlen(right) <= (CLUE_LENGTH-1))){
        new_krag->clue = left;
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
    strcpy(gi->secret_code,line);
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
    if (game_info_find_team(gi->team, team_name) == NULL){
        team_t *new_team = team_new();
        set_insert(gi->team, team_name, new_team);
    }
}

/**************** game_info_find_pebbleId ****************/
/* Return the team that has field agent with given pebbleId
 * Return NULL if it does not exist
 */
static team_t *
game_info_find_pebbleId(game_info_t *gi, char *pebbleId){
    find_id_t *find_id = malloc(sizeof(find_id_t));
    find_id->id = intToHex(pebbleId);
    find_id->fa = NULL;
    find_id->team = NULL;
    
    // interate over all the teams and find the fa
    // associated with the given pebbleId
    set_iterate(gi->team, find_id, &find_pebbleId);
    team_t *team = gi->team;
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
    const char *team_name = key;
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
    const char *fa_name = key;
    fa_t *fa = item;
    
    // if the input is NULL, do nothing
    if (find_id == NULL || fa == NULL){
        return;
    }
    
    if (strcmp(fa->pebbleId, find_id->id) == 0){
        find_id->fa = fa;
    }
}

/**************** game_info_validate ****************/
/* Validate gameId, pebbleId, team name and player name.
 * Return 0 if all correct.
 * Return -4 if gameId is incorrect
 * Return -5 if team name not found
 * Return -6 if player not found in the team
 * Return -7 if pebbleId is incorrect
 */
int
game_info_validate(game_info *gi, char *gameId, char *pebbleId, char *team_name, char *player_name, char *latitude, char *longitude){
    if (gi->gameId != intToHex(gameId)) return -4;
    
    team_t *team = game_info_find_team(gi, team_name);
    if (team == NULL) return -5;
    
    fa_t * fa = team_find_fa(team, player_name);
    if (fa == NULL) return -6;
    
    if (fa->pebbleID != intToHex(pebbleId)) return -7;
    
    fa->latitude = atof(latitude);
    fa->longitude = atof(longitude);
    fa->last_contact_time = time();
    return 0;
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
    krag->clue = malloc((CLUE_LENGTH) * sizeof(char)); // maximum length for clue is 140
    krag->claimed_team = set_new();
    return krag;
}

/**************** krag_delete ****************/
/* free the memory of the given krag
 */
static void
krag_delete(krag_t *krag){
    if (krag == NULL){
        return;
    }
    set_delete(krag->claimed_team, &set_char_delete);
    free(krag->clue);
    free(krag);
}



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
    new_team->revealed_krags = set_new();
    new_team->secret_string = malloc((CLUE_LENGTH) * sizeof(char));
    return new_team;
}

/**************** team_delete ****************/
/* free the memory of the given team
 */
static void
team_delete(team_t *team){
    if (team == NULL){
        return;
    }
    set_delete(team->fa, &set_fa_delete);
    set_delete(team->revealed_krags, &set_char_delete);
    free(krag->clue);
    free(krag);
}

/**************** team_find_fa ****************/
/* Return the fa named player_name
 * return NULL if not found
 */
fa_t *
team_find_fa(team_t *team, char *player_name){
    // get the fa and return (can be NULL)
    fa_t *fa = set_find(team->fa, player_name);
    return fa;
}

/**************** team_register_fa ****************/
/* register fa to the team
 * return 0 if successfully registered
 * return -7 if the pebbleId is already registered
 */
int
team_register_fa(game_info_t *gi, team_t *team, char *pebbleId, char *player_name, sockaddr_in them, char *latitude, char *longitude){
    // if the player with the given pebbleId is not found
    if (game_info_find_pebbleId(gi, pebbleId) == NULL){
        fa_t *fa = fa_new();
        fa->pebbleID = intToHex(pebbleId);
        fa->name = player_name;
        fa->them = them;
        fa->latitude = atof(latitude);
        fa->longitude = atof(longitude);
        fa->last_contact_time = time();
        set_insert(team, player_name, fa);
    }
    // if the player with the given pebbleId is found, it will not be registered
    // invalid id
    else {
        return -7;
    }
}


/*********************************************************/
/****************** functions for fa *********************/
/*********************************************************/

typedef struct fa {
    char *name;          // name
    int pebbleID;        // pebble id
    sockaddr_in them;    // address of the socket
    float longitude;     // longitude of the player's position
    float latitude;      // latitude of the player's position
    time_t last_contact_time;// the time of last contact
}fa_t;
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
    fa->name = malloc(MESSAGE_LENGTH * sizeof(char));
    return new_fa;
}

/**************** fa_delete ****************/
/* free the memory of the given fa
 */
static void
team_delete(team_t *team){
    if (team == NULL){
        return;
    }
    set_delete(team->fa, &set_fa_delete);
    set_delete(team->revealed_krags, &set_char_delete);
    free(krag->clue);
    free(krag);
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
    if (fa != NULL){
        free(fa->name);
        free(fa);
    }
}
