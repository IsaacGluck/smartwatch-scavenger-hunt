/*****************************************************************/
/* This program parses the location messages received by the field agent and exports them as a location struct*/
/*****************************************************************/
#include "location.h"


typedef struct location_struct{
	double latitude;
	double longitude_string;
} location_t;


location_t* parse_location(char* location_s)
{
	char *latitude_string = location_s; // pointer to the beginning of the string
	char *longitude_string_string = NULL;
	// Tokenize the string
	for (int i = 0; i < strlen(location_s); i++) {
		if (location_s[i] == '|') {
			location_s[i] = '\0'; 
			longitude_string = location_s[i+1];
			break;
		}
	}

	if (longitude_string == NULL) {
		return NULL; // could not tokenize the location_s
	}

	double latitude = string_to_double(latitude_string);
	double longitude = string_to_double(longitude_string);

	location_t* location = malloc(sizeof(location_t)); // must free later
	location.latitude = latitude;
	location.longitude = longitude;
}


/*
 * This code converts a string to a double.
 * Useful for the location functions.
 * Code ideas from https://crackprogramming.blogspot.com/2012/10/implement-atof.html
 */
double string_to_double(char* number_s)
{
	if (!number_s || !*number_s) {
		return 0; // validate input
	}

	char* num_p = number_s; // don't manipulate the parameter pointer

	// set defaults
	double intSection =  0;
	double fracSection =  0;
	int divisor = 1;
	int sign = 1;
	int in_fraction = 0; // 0 is false, 1 is true

	// check the sign
	if (*num_p == '-') {
		num_p++;
		sign = -1;
	}

	// loop through the string until the end, \0
	while (*num_p != '\0') {
		if (*num_p >= '0' && *num_p <= '9') { // make sure the character is a number
			if (in_fraction == 1)  { // fraction section
				fracSection = (fracSection * 10) + (*num_p - '0');
				divisor *= 10;
			} else { // int section
				intSection = (intSection * 10) + (*num_p - '0');
			}
		} else if (*num_p == '.') {
			if (in_fraction == 1) { // already in fraction section -> abort
				return sign * (intSection + fracSection/divisor);
			} else { // move to the fraction section
				in_fraction == 1;
			}
		} else { // not a number or dot -> abort
			return sign * (intSection + fracSection/divisor);
		}
		num_p++;
	}

	// return if you haven't already
	return sign * (intSection + fracSection/divisor);
}