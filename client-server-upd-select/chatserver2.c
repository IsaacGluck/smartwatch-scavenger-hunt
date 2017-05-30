/* 
 * chatserver2 - an example chat server using UDP and select().
 *
 * Read messages from a UDP socket and print them to stdout;
 * read messages from stdin and send them back to client.
 * Use select() so we don't block on either stdin or the socket.
 * 
 * usage: ./chatserver2
 *  and ctrl-D to exit from a prompt, or ctrl-C if necessary.
 *
 * David Kotz - May 2016, May 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>	      // read, write, close
#include <string.h>	      // strlen
#include <strings.h>	      // bcopy, bzero
#include <arpa/inet.h>	      // socket-related calls
#include <sys/select.h>	      // select-related stuff 
#include "file.h"	      // readlinep

/**************** file-local constants and functions ****************/
static const int BUFSIZE = 1024;     // read/write buffer size
static int socket_setup();
static int handle_stdin(int comm_sock, struct sockaddr_in *themp);
static void handle_socket(int comm_sock, struct sockaddr_in *themp);

/**************** main() ****************/
int
main(const int argc, char *argv[])
{
  // no arguments to parse

  // set up a socket on which to receive messages
  int comm_sock = socket_setup();
  
  // the client's address, filled in by recvfrom
  struct sockaddr_in them = {0, 0, {0}};  // sender's address
  
  // read from either the socket or stdin, whichever is ready first;
  // if stdin, read a line and send it to the socket;
  // if socket, receive message from socket and write to stdout.
  while (true) {	      // loop exits on EOF from stdin
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
	if (handle_stdin(comm_sock, &them) == EOF) {
	  break; // exit loop if EOF on stdin
	}
      }
      if (FD_ISSET(comm_sock, &rfds)) {
	handle_socket(comm_sock, &them); // may update 'them'
      }

      // print a fresh prompt
      printf(": ");
      fflush(stdout);
    }
  }

  close(comm_sock);
  putchar('\n');
  return 0;
}

/**************** socket_setup ****************/
/* All the ugly work of preparing the datagram socket;
 * exit program on any error.
 */
static int
socket_setup()
{
  // Create socket on which to listen (file descriptor)
  int comm_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (comm_sock < 0) {
    perror("opening datagram socket");
    exit(1);
  }

  // Name socket using wildcards
  struct sockaddr_in server;  // server address
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = 0;
  if (bind(comm_sock, (struct sockaddr *) &server, sizeof(server))) {
    perror("binding socket name");
    exit(2);
  }

  // Print our assigned port number
  socklen_t serverlen = sizeof(server);	// length of the server address
  if (getsockname(comm_sock, (struct sockaddr *) &server, &serverlen)) {
    perror("getting socket name");
    exit(3);
  }
  printf("Ready at port %d\n", ntohs(server.sin_port));

  return (comm_sock);
}

/**************** handle_stdin ****************/
/* stdin has input ready; read a line and send it to the client.
 * return EOF if EOF was encountered on stdin;
 * return 0 if there is no client to whom we can send;
 * return 1 if message sent successfully.
 * exit on any socket error.
 */
static int
handle_stdin(int comm_sock, struct sockaddr_in *themp)
{
  char *response = readlinep(stdin);
  if (response == NULL) 
    return EOF;

  if (themp->sin_family != AF_INET) {
    printf("You have yet to hear from any client.\n");
    fflush(stdout);
    return 0;
  } 

  if (sendto(comm_sock, response, strlen(response), 0, 
	     (struct sockaddr *) themp, sizeof(*themp)) < 0) {
    perror("sending in datagram socket");
    exit(6);
  }
  free(response);

  return 1;
}

/**************** handle_socket ****************/
/* Socket has input ready; receive a datagram and print it.
 * If 'themp' is a valid client, leave it unchanged.
 * If 'themp' is not a valid client, update *themp to this sender.
 * Exit on any socket error.
 */
static void
handle_socket(int comm_sock, struct sockaddr_in *themp)
{
  // socket has input ready
  struct sockaddr_in sender;		 // sender of this message
  struct sockaddr *senderp = (struct sockaddr *) &sender;
  socklen_t senderlen = sizeof(sender);  // must pass address to length
  char buf[BUFSIZE];	      // buffer for reading data from socket
  int nbytes = recvfrom(comm_sock, buf, BUFSIZE-1, 0, senderp, &senderlen);

  if (nbytes < 0) {
    perror("receiving from socket");
    exit(1);
  } else {
    buf[nbytes] = '\0';     // null terminate string

    // where was it from?
    if (sender.sin_family != AF_INET) {
      printf("From non-Internet address: Family %d\n", sender.sin_family);
    } else {
      // if we don't yet have a client, use this sender as our client.
      if (themp->sin_family != AF_INET)
	*themp = sender;
      
      // was it from the expected client?
      if (sender.sin_addr.s_addr == themp->sin_addr.s_addr && 
	  sender.sin_port == themp->sin_port) {
	// print the message
	printf("[%s@%05d]: %s\n", 
	       inet_ntoa(sender.sin_addr),
	       ntohs(sender.sin_port), 
	       buf);
      } else {
	printf("[%s@%05d]: INTERLOPER! %s\n",
	       inet_ntoa(sender.sin_addr),
	       ntohs(sender.sin_port),
	       buf);
      }
    }
    fflush(stdout);
  }
}
