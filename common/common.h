/*
 * common.h - header file for common functions
 *
 * Kazuma Honjo, May 2017
 */

#ifndef __COMMON_H_
#define __COMMON_H_

/* return the integer value of the given hex */
int stringHexToDec(char* hex);

/* return the char * of the given unsigned decimal number 
 * has to be freed by caller
 */
char *decToStringHex(unsigned int dec);

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


#endif