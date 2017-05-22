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
    float longitude;     // longitude of the player's position
    float latitude;      // latitude of the player's position
    time_t last_contact_time;// the time of last contact
}fa_t;


// struct for game agent information
typedef struct ga {
    char *name;               // name
    int guideID;              // guide ID
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

/**************** file-local functions ****************/
static int handle_kiff_message(char *left, char *right, game_info_t *gi, krag_t *new_krag);
static void set_kiff_handle_error(char *left, char *right, FILE *fp, char *message);
static krag_t *krag_new();
static void krag_delete(krag_t *krag);
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
    // convert gameID from hex to int
    // duncitoianen oanwoin wiaenf a
    // afbnoiwa pnawf
    // gi->gameID = ........
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
                    free(left);
                    free(right);
                    fclose(fp);
                    fprintf(stderr, "Error: '=' found consecutively in the kiff\n");
                    return 1;
                }
            }
            if (line[i] == '|'){
                if (state == 1){
                    state = 0;
                    right[j] = '\0';
                    j = 0;
                    if (handle_kiff_message(left, right, gi, new_krag) != 0){
                        free(left);
                        free(right);
                        fclose(fp);
                        fprintf(stderr, "Error: krag format error\n");
                        return 1;
                    }
                }
                // if currently reading left hand side,
                // the next non-alphabet/number character must be '='
                else {
                    free(left);
                    free(right);
                    fclose(fp);
                    fprintf(stderr, "Error: '|' found consecutively in the kiff\n");
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


/*********************************************************/
/**************** functions for krag *********************/
/*********************************************************/

/**************** krag_new ****************/
/* return a new game info
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
    krag->clue = malloc(sizeof(char) * (CLUE_LENGTH)); // maximum length for clue is 140
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

/**************** set_char_delete ****************/
/* delete helper function for deleting char item in set
 */
static void
set_char_delete(void *item){
    char *string = item;
    if (string != NULL){
        free(string);
    }
}
