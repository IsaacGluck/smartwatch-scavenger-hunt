/*****************************************************************/
/* This program          */
/*****************************************************************/
#ifndef __LOCATION_H
#define __LOCATION_H

#include <pebble.h>


typedef struct location_struct {
	double latitude;
	double longitude;
} location_t;

location_t *parse_location(char* location_s);

double string_to_double(char* number_s);

#endif