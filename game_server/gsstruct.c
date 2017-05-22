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
/* none */

/**************** Struct ****************/
// struct for game information
typedef struct game_info{
    time_t start;     // Time the game started
    int num_krags;    // Number of krags
    int num_agents;   // Number of agents (#FA + #GA)
    int num_teams;    // Number of teams (# of Struct Team)
    char *sf;         // Secret file path name
    char *kiff;       // Krag file path name
    set_t *krags;     // Set of krags (clue will be the key)
    set_t *team;      // Set of Team (team name will be the key)
    int gameID;       // ID of the game
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
    return gi;
}

/**************** game_info_get_start ****************/
/* return a start time of game */
time_t
game_info_get_start_time(game_info_t *gi){
    return gi->start;
}

/**************** game_info_set_gameID ****************/
/* set the gameID 
 * return 0 if success, 1 if error
 */
int
game_info_set_gameID(game_info_t *gi, char *gameID_in_hex){
    // convert gameID from hex to int
    // duncitoianen oanwoin wiaenf a
    // afbnoiwa pnawf
    // gi->gameID = ........
    return 0;
}

/**************** game_info_set_kiff ****************/
/* set the kiff
 * return 0 if success, 1 if error
 */
int
game_info_set_kiff(game_info_t *gi, char *kiff){
    // Open the file. If it does not exist or cannot be read,
    // wrong file path name. Return 1
    FILE *fp = fopen(kiff, "r");
    if (fp == NULL){
        return 1;
    }
    
    char *line;
    // read all lines in the file
    while ((line = readlinep(fp)) != NULL){
        // allocate memory
        // each word must be shorter than the length of line
        char *left = malloc(strlen(line)+1);
        char *right = malloc(strlen(line)+1);
        int j = 0;
        int state = 0; // 0 indiactes currently reading the left-hand-side
                       // 1 indicates currently reading the right-hand-side
        krag_t *new_krag = krag_new();
        
        // loop over the line
        for (int i = 0; i < strlen(line); i++){
            if (isspace(line[i])){
                free(left);
                free(right);
                fclose(fp);
                fprintf(stderr, "Error: space found in the kiff\n");
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
    }
    
    fclose(fp);
    gi->kiff = kiff;
    return 0;
}

/**************** handle_kiff_message ****************/
/* helper function for game_info_set_kiff
 * handle the message properly
 * return 0 if success, 1 if malloc error, 2 if other error
 */
static int
handle_kiff_message(char *left, char *right, game_info_t *gi, krag_t *new_krag){
    if ((left == NULL) || (right == NULL) || (gi == NULL) || (new_krag == NULL)){
        return 2;
    }
    if (new_krag == NULL){
        fprintf(stderr, "krag_new failed due to not being able to malloc memory\n");
        return 1;
    }
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
    else if ((strcmp(left, "clue") == 0) && (strlen(right) <= 140)){
        new_krag->clue = left;
    }
    else{
        fprintf(stderr, "Error: krag format error\n");
        krag_delete(new_krag);
        return 2;
    }
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
        return NULL;
    }
    
    // initialize parameters
    krag->longitude = 0;
    krag->latitude = 0;
    krag->kragID = 0;
    krag->clue = malloc(sizeof(char) * 141); // maximum length for clue is 140
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
