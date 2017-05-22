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
 *  1 -
 *
 * Kazuma Honjo, May 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "index.h"
#include "pagedir.h"
#include "file.h"
#include "counters.h"


/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/


/**************** global types ****************/
/* none */

/**************** local functions ****************/
/* not visible outside this file */


/**************** main ****************/
int
main()
