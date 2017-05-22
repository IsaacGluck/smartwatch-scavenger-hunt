/*
 * gsstruct.h - header file for game server struct module
 * 
 * Kazuma Honjo, May 2017
 *
 */

#ifndef __GSSTRUCT_H
#define __GSSTRUCT_H

#include <time.h>

/**************** global types ****************/
/* opaque to users of the module */
typedef struct game_info game_info_t;  // struct for game information
typedef struct team team_t;            // struct for team information
typedef struct fa fa_t;                // struct for field agent information
typedef struct ga ga_t;                // struct for game agent information
typedef struct krag krag_t;            // struct for krag information


/**************** functions ****************/
/* get the initial state of game information
 */
game_info_t *game_info_new();

time_t game_info_get_start_time(game_info_t *gi);

int game_info_set_gameID(game_info_t *gi, char *gameID_in_hex);

int game_info_set_kiff(game_info_t *gi, char *kiff);
#endif   // __GSSTRUCT_H
