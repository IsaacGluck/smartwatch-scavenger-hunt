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
 *  5 - Wrong sf
 *  6 - Wrong port
 *  7 - Error during setting up UDP/DGRAM
 *  8 - Error during receiving message through socket
 *
 * Kazuma Honjo, May 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "set.h"
#include "gsstruct.h"


/**************** file-local global variables ****************/
static const int MESSAGE_LENGTH = 8192;

/**************** local types ****************/


/**************** global types ****************/
/* none */

/**************** local functions ****************/
/* not visible outside this file */
static void parse_command_line_arguments(const int argc, char *argv[], game_info_t *gi, int *comm_sock, struct sockaddr_in *server);
static void set_up_udp(int port, int *comm_sock, struct sockaddr_in *server);


/**************** main ****************/
int
main(const int argc, char *argv[]){
    // initialize parameters
    game_info_t *gi = game_info_new();
    if (gi == NULL){
        fprintf(stderr, "failed to initialize gi\n");
        exit(1);
    }
    int comm_sock;
    struct sockaddr_in server;
    
    // parse the command line arguments and set up the game
    parse_command_line_arguments(argc, argv, gi, &comm_sock, &server);
    
    // receive datagrams and handle the message
    while(game_info_get_game_status(gi) == 0){
        char buf[MESSAGE_LENGTH];         // buffer for reading data from socket
        struct sockaddr_in them;          // sender's address
        struct sockaddr *themp = (struct sockaddr *) &them;
        socklen_t themlen = sizeof(them);
        int nbytes = recvfrom(comm_sock, buf, MESSAGE_LENGTH-1, 0, themp, &themlen);
        
        if (nbytes < 0){
            perror("receiving from socket");
            exit(8);
        }
        else if (nbytes > 0){
            buf[nbytes] = '\0';            // null terminate string
            
            do something with this message!!!!!
        }
    }
}


/**************** parse_command_line_arguments ****************/
/* parse the command line arguments and set up the game
 * if there are any error, exit with proper exit status
 */
static void
parse_command_line_arguments(const int argc, char *argv[], game_info_t *gi, int *comm_sock, struct sockaddr_in *server){
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
    if (game_info_set_secret_code(gi, sf) != 0){
        fprintf(stderr, "Wrong sf\n");
        fprintf(stderr, "./gameserver gameID kiff sf port\n");
        exit(5);
    }
    
    // validate port
    for (int i = 0; i < strlen(argv[4]); i++){
        if (!isdigit(argv[4][i])){
            fprintf(stderr, "Port is not a number\n");
            fprintf(stderr, "./gameserver gameID kiff sf port\n");
            exit(6);
        }
    }
    int port = atoi(argv[4]);
    set_up_udp(port, comm_sock, server);
}


/**************** set_up_udp ****************/
/* set up the dgram socket and bint it to given port number
 * if there are any error, exit with proper exit status
 *
 * reference: udpserver.c from dartmouth college cs50
 */
static void
set_up_udp(int port, int *comm_sock, struct sockaddr_in *server){
    // create socket on which to listen
    *comm_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (*comm_sock < 0){
        perror("opening datagram socket");
        exit(7);
    }
    
    // name socket using wildcards
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = INADDR_ANY;
    server->sin_port = htons(port);
    if (bind(*comm_sock, (struct sockaddr *)server, sizeof(*server))){
        perror("binding socket name");
        exit(7);
    }
}

