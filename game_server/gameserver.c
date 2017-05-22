/*
 * gameserver.c - game server for final project
 *
 * usage: ./gameserver gameID kiff sf port
 *
 * inputs:
 *  gameID - the hexadecimal ID number of this game
 *  kiff - the path to the krag file, which contains coordinates and clues for each krag
 *  sf - the path to the secret file, which contains a single line of text representing the secret
 *  port - the port number of the Game Server
 *
 * output:
 *  All the activities will be written in the log file located at logs/gameserver.log
 *  When there is an error or when the game finishes, message will be printed to stdout.
 *
 * exit:
 *  0 - Successfully finished
 *  1 - Error during initializing parameters
 *  2 - Wrong nubmer of command line arguments
 *  3 - Wrong gameID
 *  4 - Wrong kiff
 *
 * Kazuma Honjo, May 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "set.h"
#include "gsstruct.h"


/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/


/**************** global types ****************/
/* none */

/**************** local functions ****************/
/* not visible outside this file */
static void parse_command_line_arguments(const int argc, char *argv[], game_info_t *gi);


/**************** main ****************/
int
main(const int argc, char *argv[]){
    // initialize parameters
    game_info_t *gi = game_info_new();
    if (gi == NULL){
        fprintf(stderr, "failed to initialize gi\n");
        exit(1);
    }
    
    
    parse_command_line_arguments(argc, argv, gi);
}

static void
parse_command_line_arguments(const int argc, char *argv[], game_info_t *gi){
    // command line arguments must be 4 (excluding ./gameserver)
    if (argc != 5){
        fprintf(stderr, "./gameserver gameID kiff sf port\n");
        exit(2);
    }
    
    // set gameID
    char *gameID = argv[1];
    if (game_info_set_gameID(gi, gameID) != 0){
        fprintf(stderr, "Wrong gameID\n");
        fprintf(stderr, "./gameserver gameID kiff sf port\n");
        exit(3);
    }
    
    // validate and set kiff
    char *kiff = argv[2];
    if (game_info_set_kiff(gi, kiff) != 0){
        fprintf(stderr, "Wrong kiff\n");
        fprintf(stderr, "./gameserver gameID kiff sf port\n");
        exit(4);
    }
    
    // validate and set sf
    char *sf = argv[3];
    // validate port
}



