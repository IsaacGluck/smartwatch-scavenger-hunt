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
 *  99 - Malloc failed
 *
 * Kazuma Honjo, May 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#include "file.h"
#include "set.h"
#include "common.h"
#include "gsstruct.h"
#include "gs_message_handler.h"
#include "gs_response_handler.h"

/**************** file-local global variables ****************/
static const int MESSAGE_LENGTH = 8192;

/**************** local types ****************/
/* nonde */

/**************** global types ****************/
/* none */

/**************** local functions ****************/
/* not visible outside this file */
static void parse_command_line_arguments(const int argc, char *argv[], game_info_t *gi, int *comm_sock, struct sockaddr_in *server);
static int set_up_udp(int port, int *comm_sock, struct sockaddr_in *server);
static void handle_stdin(int comm_sock, struct sockaddr_in *themp, game_info_t *gi);
static void handle_socket(int comm_sock, struct sockaddr_in them, game_info_t *gi);


/**************** main ****************/
/* Referenced chatserver2
 * Made by David Kotz - May 2016, May 2017
 * Dartmouth College cs50
 */
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
    
    // the client's address, filled in by recvfrom
    struct sockaddr_in them = {0, 0, {0}};  // sender's address
    
    // read from either the socket or stdin, whichever is ready first;
    // if stdin, read a line and send it to the socket;
    // if socket, receive message from socket and write to stdout.
    // loop while game status is 0
    while(game_info_get_game_status(gi) == 0){
        // for use with select()
        fd_set rfds;	      // set of file descriptors we want to read
        
        // Watch stdin (fd 0) and the UDP socket to see when either has input.
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);	      // stdin
        FD_SET(comm_sock, &rfds); // the UDP socket
        int nfds = comm_sock+1;   // highest-numbered fd in rfds
        
        // Wait for input on either source
        int select_response = select(nfds, &rfds, NULL, NULL, NULL);
        // note: 'rfds' updated
        
        if (select_response < 0) {
            // some error occurred
            perror("select()");
            exit(9);
        } else if (select_response == 0) {
            // timeout occurred; this should not happen
        } else if (select_response > 0) {
            // some data is ready on either source, or both
            
            if (FD_ISSET(0, &rfds)) {
                handle_stdin(comm_sock, &them, gi);
            }
            if (FD_ISSET(comm_sock, &rfds)) {
                handle_socket(comm_sock, them, gi);
            }
            
            // print a fresh prompt
            printf(": ");
            fflush(stdout);
        }
    }
    
    send_game_over(comm_sock, gi);
    send_team_record(comm_sock, gi);
    
    close(comm_sock);
    game_info_delete(gi);
    putchar('\n');
    return 0;
}


/**************** parse_command_line_arguments ****************/
/* parse the command line arguments and set up the game
 * if there are any error, exit with proper exit status
 */
static void parse_command_line_arguments(const int argc, char *argv[], game_info_t *gi, int *comm_sock, struct sockaddr_in *server){
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
        game_info_delete(gi);
        exit(3);
    }
    
    // validate and set kiff
    char *kiff = argv[2];
    if (game_info_set_kiff(gi, kiff) != 0){
        fprintf(stderr, "Wrong kiff\n");
        fprintf(stderr, "./gameserver gameID kiff sf port\n");
        game_info_delete(gi);
        exit(4);
    }
    
    // validate and set sf
    char *sf = argv[3];
    if (game_info_set_secret_code(gi, sf) != 0){
        fprintf(stderr, "Wrong sf\n");
        fprintf(stderr, "./gameserver gameID kiff sf port\n");
        game_info_delete(gi);
        exit(5);
    }
    
    // validate port
    for (int i = 0; i < strlen(argv[4]); i++){
        if (!isdigit(argv[4][i])){
            fprintf(stderr, "Port is not a number\n");
            fprintf(stderr, "./gameserver gameID kiff sf port\n");
            game_info_delete(gi);
            exit(6);
        }
    }
    int port = atoi(argv[4]);
    if (set_up_udp(port, comm_sock, server) != 0){
        game_info_delete(gi);
        exit(7);
    }
}


/**************** set_up_udp ****************/
/* set up the dgram socket and bint it to given port number
 * if there are any error, exit with proper exit status
 *
 * Reference: udpserver.c
 * Made by David Kotz - May 2016, May 2017
 * Dartmouth College cs50
 */
static int
set_up_udp(int port, int *comm_sock, struct sockaddr_in *server){
    // create socket on which to listen
    *comm_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (*comm_sock < 0){
        perror("opening datagram socket");
        return 1;
    }
    
    // name socket using wildcards
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = INADDR_ANY;
    server->sin_port = htons(port);
    if (bind(*comm_sock, (struct sockaddr *)server, sizeof(*server))){
        perror("binding socket name");
        return 1;
    }
    return 0;
}

/**************** handle_stdin ****************/
/* handle the case input is from stdin */
static void handle_stdin(int comm_sock, struct sockaddr_in *themp, game_info_t *gi){
    char* line = readlinep(stdin);
    if (line != NULL){
       printf("input from stdin: %s\n", line);
    
        if (strcmp(line, "GAME OVER") == 0){
            game_info_change_game_status(gi);
      }
    }
    free(line);
}

/**************** handle_socket ****************/
/* handle the case input is from stdin */
static void handle_socket(int comm_sock, struct sockaddr_in them, game_info_t *gi){
    if (gi == NULL) return;
    
    char *buf = malloc(MESSAGE_LENGTH);  // buffer for reading data from socket
    struct sockaddr *themp = (struct sockaddr *) &them;
    socklen_t themlen = sizeof(them);
    int nbytes = recvfrom(comm_sock, buf, MESSAGE_LENGTH-1, 0, themp, &themlen);
    
    if (nbytes < 0){
        perror("receiving from socket");
        exit(8);
    }
    else if (nbytes > 0){
        buf[nbytes] = '\0';            // null terminate string
        
        char **tokens;
        tokens = getOpCode(buf);
        char *opCode = tokens[0];
        char *rest_of_message = tokens[1];
        printf("opCode: %s\n\trest of message: %s\n\n", opCode, rest_of_message);
        
        
        
        // dispatch the appropriate function
        int fn;
        int result;
        for (fn = 0; dispatch[fn].opCode != NULL; fn++){
            if (strcmp(opCode, dispatch[fn].opCode) == 0){
                result = (*dispatch[fn].func)(rest_of_message, gi, them);
                break;
            }
        }
        if (dispatch[fn].opCode == NULL){
            printf("\n\nUnknown command\n\n");
        }
        
        respond(opCode, result, comm_sock, them, gi, buf);
        
        free(tokens[0]);
        free(tokens[1]);
        free(tokens);
    }
    free(buf);
}
