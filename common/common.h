/*
 * common.h - header file for common functions
 *
 * Kazuma Honjo, May 2017
 */

#ifndef __COMMON_H_
#define __COMMON_H_

#ifdef NOPEBBLE // we are *not* building for pebble
#include <arpa/inet.h>
#include <sys/select.h>


/* return the integer value of the given hex */
int stringHexToDec(char* hex);

/* return the char * of the given unsigned decimal number 
 * has to be freed by caller
 */
char *decToStringHex(unsigned int dec);

/* This method gets the IP address of the server */
char *getIP(int comm_sock, struct sockaddr_in them);

#endif // NOPEBBLE

/* return the array created from the message
 * array[0] contains opCode
 * array[1] contains rest of the line (opCode excluded)
 *
 * the message must be valid, required to use validate function
 * beforehand.
 *
 * has to be freed by caller
 */
char **getOpCode(char *message);


#endif //__COMMON_H_
