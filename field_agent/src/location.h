/*****************************************************************/
/* This program          */
/*****************************************************************/
#ifndef __LOCATION_H
#define __LOCATION_H

#include <pebble.h>


typedef struct location_struct {
	char* latitude;
	char* longitude;
} location_t;

location_t *parse_location(char* location_s);

#endif