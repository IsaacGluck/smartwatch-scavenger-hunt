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
typedef struct send_message send_message_t;// struct for sending message
typedef struct gifaga gifaga_t;        // struct for holding gi, one fa, and ga in the team

/**************** functions ****************/

/**************** functions for game_info ****************/
/* get the initial state of game information*/
game_info_t *game_info_new();

/* delete the game info
 */
void game_info_delete(game_info_t *gi);

/* return a start time of game */
time_t game_info_get_start_time(game_info_t *gi);

/* return a status of game */
int game_info_get_game_status(game_info_t *gi);

/* return gameId in Hex*/
char *game_info_get_gameId(game_info_t *gi);

/* return number of krags in this game*/
int game_info_get_numKrags(game_info_t *gi);

/* return secret code of this game*/
char *game_info_get_secret(game_info_t *gi);

/* change the status of game to 1 */
void game_info_change_game_status(game_info_t *gi);

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

/* Return the fa with given pebbleId
 * Return NULL if it does not exist
 */
fa_t *game_info_find_fa_pebbleId(game_info_t *gi, char *pebbleId);

/* Add a new team to set of teams in game_info
 * Ignore if the team with same name exists
 */
void game_info_register_team(game_info_t *gi, char *team_name);

/* Return the team that has guide agent with given guideId
 * Return NULL if it does not exist
 */
team_t *game_info_find_guideId(game_info_t *gi, char *guideId);

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

/* Examine if there is the krag is within 10 meters from the
 * given latitude and longitude
 * Return 0 if the krag is located within 10m.
 * Return 1 if krag is not found, or is not located within 10m.
 */
int game_info_krag_distance(game_info_t *gi, char *kragId, char *latitude, char *longitude);

/* Reveal krag and return the krag that was revealed
 * Return NULL if error
 */
krag_t * game_info_reveal_krag(game_info_t *gi, team_t *team);

/* Return the team that has field agent with given pebbleId
 * Return NULL if it does not exist
 */
team_t *game_info_find_pebbleId(game_info_t *gi, char *pebbleId);

/* Send message to all agents in the game
 */
void game_info_send_message_to_everyone(game_info_t *gi, char *message, int comm_sock, void (*itemfunc)(void *arg, const char *key, void *item));




/**************** functions for krag ****************/

/* Return the latitude of the krag
 * return 1000 if it does not exist
 */
float krag_get_latitude(krag_t *krag);

/* Return the longitue of the krag
 * return 1000 if it does not exist
 */
float krag_get_longitude(krag_t *krag);

/* Return the kragId of the krag
 * return NULL if it does not exist
 */
char *krag_get_kragId(krag_t *krag);

// /*
//  * return 0 if it does not exist
//  */
// unsigned int krag_get_kragId(krag_t *krag);

/* Return the clue of the krag
 * return NULL if it does not exist
 */
char *krag_get_clue(krag_t *krag);

/* check if the krag has claimed by the team or not
 * Return 0 if claimed, 1 if not
 * Return -1 if error
 */
int krag_has_claimed(krag_t *krag, char *team_name);

/* check if the krag has revealed by the team or not
 * Return 0 if revealed, 1 if not
 * Return -1 if error
 */
int krag_has_revealed(krag_t *krag, char *team_name);

/* Mark the krag has claimed for the given krag and team
 * Return 1 if there are more krags to be claimed after marked
 * Return 2 if this was the last krag to be claimed
 * Return 0 if error
 */
int krag_mark_claimed(game_info_t *gi, krag_t *krag, char *team_name);

/* Print the data stored in krag
 */
void krag_print(krag_t *krag);




/**************** functions for team ****************/
/* Return the fa named player_name
 * return NULL if not found
 */
fa_t *team_find_fa(team_t *team, char *player_name);

/* Return the ga named player_name
 * return NULL if not found
 */
ga_t *team_find_ga(team_t *team, char *player_name);

/* register fa to the team
 * return 0 if successfully registered
 * return -6 if player with same name exists in the team
 * return -7 if the pebbleId is already registered
 */
int team_register_fa(game_info_t *gi, team_t *team, char *pebbleId, char *player_name, struct sockaddr_in them, char *latitude, char *longitude);

/* register ga to the team
 * return 0 if successfully registered
 * return -5 if the guide is already registered in the team
 * return -7 if the guideId is already registered
 */
int team_register_ga(game_info_t *gi, team_t *team, char *guideId, char *player_name, struct sockaddr_in them);

/* Print the data stored in team
 */
void team_print(team_t *team);

/* Send hint to all field agent in the team
 */
void team_send_hint_to_everyone(team_t *team, char *hint, int comm_sock);

/* Send GS_AGENT to guide agent
 */
void team_send_gs_agent(game_info_t *gi, team_t *team, int comm_sock, void (*itemfunc)(void *arg, const char *key, void *item));

/* Return the guide agent if exists
 */
ga_t *team_get_guide(team_t *team);

/* Return the guideId if exists
 */
char *team_get_guideId(team_t *team);

/* Return the number of claimed krags
 */
int team_get_numClaimed(team_t *team);

/* Return the number of claimed krags
 */
int team_get_numRevealed(team_t *team);

/* Return the secret string for this team
 */
char * team_get_secret(team_t *team);

/* return name of the team
 */
char *team_get_name(team_t *team);

/* Update the secret string for the given team based on
 * given krag
 */
void team_update_string(game_info_t *gi, team_t *team, krag_t *krag);

/* Send hint to all field agent in the team
 */
void team_send_message_to_everyone(team_t *team, char *message, int comm_sock);




/****************** functions for fa *********************/

/* Print the data stored in team
 */
void fa_print(fa_t *fa);

/* send message to field agent
 */
void
fa_send_to(fa_t *fa, int comm_sock, char *message);

/* return pebbleId of the fa
 */
char *fa_get_pebbleId(fa_t *fa);

/* return the time elapsed (seconds in int)
 * since last contact by the fa
 */
int fa_get_time(fa_t *fa);

/* return name of the fa
 */
char *fa_get_name(fa_t *fa);

/* return latitude of the fa
 * return 1000 if error
 */
float fa_get_latitude(fa_t *fa);

/* return longitude of the fa
 * return 1000 if error
 */
float fa_get_longitude(fa_t *fa);





/****************** functions for ga *********************/
/* return the guideId
 */
char *ga_get_id(ga_t *ga);

/* send message to guide agent
 */
void ga_send_to(ga_t *ga, int comm_sock, char *message);



/**************** functions for gifaga *******************/
/* return gi */
game_info_t *gifaga_get_gi(gifaga_t *gifaga);

/* return ga */
ga_t *gifaga_get_ga(gifaga_t *gifaga);

/* return team */
team_t *gifaga_get_team(gifaga_t *gifaga);

/* return comm_sock */
int gifaga_get_comm_sock(gifaga_t *gifaga);



/************* functions for send_message ****************/
/* return message */
char * send_message_get_message(send_message_t *send_message);

/* return comm_sock */
int send_message_get_comm_sock(send_message_t *send_message);
#endif   // __GSSTRUCT_H
