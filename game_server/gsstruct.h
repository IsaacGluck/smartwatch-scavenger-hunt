/*
 * gsstruct.h - header file for game server struct module
 * 
 * Kazuma Honjo, May 2017
 *
 */

#ifndef __GSSTRUCT_H
#define __GSSTRUCT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <arpa/inet.h>
#include <unistd.h>

/**************** global types ****************/
/* opaque to users of the module */
typedef struct game_info game_info_t;  // struct for game information
typedef struct team team_t;            // struct for team information
typedef struct fa fa_t;                // struct for field agent information
typedef struct ga ga_t;                // struct for game agent information
typedef struct krag krag_t;            // struct for krag information


/**************** functions ****************/

/**************** functions for game_info ****************/
/* get the initial state of game information*/
game_info_t *game_info_new();

/* return a start time of game */
time_t game_info_get_start_time(game_info_t *gi);

/* return a status of game */
int game_info_get_game_status(game_info_t *gi);

/* set the gameID
 * return 0 if success, 1 if error
 */
int game_info_set_gameID(game_info_t *gi, char *gameID_in_hex);

/* set the kiff and builds the set of krags
 * return 0 if success, 1 if error
 */
int game_info_set_kiff(game_info_t *gi, char *kiff);

/* set the secret code
 * return 0 if success, 1 if error
 */
int game_info_set_secret_code(game_info_t *gi, char *sf);

/* Return the struct team_t associated with the given team name;
 * return NULL if not found.
 */
team_t *game_info_find_team(game_info_t *gi, char *team_name);

/* Add a new team to set of teams in game_info
 * Ignore if the team with same name exists
 */
void game_info_register_team(game_info_t *gi, char *team_name);

/* Validate gameId, pebbleId, team name and player name.
 * Return 0 if all correct.
 * Return -4 if gameId is incorrect
 * Return -5 if team name not found
 * Return -6 if player not found in the team
 * Return -7 if pebbleId is incorrect
 */
int game_info_validate(game_info_t *gi, char *gameId, char *pebbleId, char *team_name, char *player_name, char *latitude, char *longitude);

/* Examine if there is a krag in the set of krag or not
 * Return the krag_t * if exists.
 * Return NULL if not found
 */
krag_t *game_info_find_krag(game_info_t *gi, char *kragId);


/**************** functions for krag ****************/
/* check if the krag has claimed by the team or not
 * Return 0 if claimed, 1 if not
 * Return -1 if error
 */
int krag_has_claimed(krag_t *krag, char *team_name);

/* Mark the krag has claimed for the given krag and team
 * Return 1 if there are more krags to be claimed after marked
 * Return 2 if this was the last krag to be claimed
 * Return 0 if error
 */
int krag_mark_claimed(game_info_t *gi, krag_t *krag, char *team_name);


/**************** functions for team ****************/
/* Return the fa named player_name
 * return NULL if not found
 */
fa_t *team_find_fa(team_t *team, char *player_name);

/* Return the ga named player_name
 * return NULL if not found
 */
ga_t *team_find_ga(team_t *team, char *player_name);
#endif   // __GSSTRUCT_H
