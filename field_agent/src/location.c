/*****************************************************************/
/* This program parses the location messages received by the field agent and exports them as a location struct*/
/*****************************************************************/
#include "location.h"


location_t* parse_location(char* location_s)
{

	char *latitude_string = location_s; // pointer to the beginning of the string
	char *longitude_string = NULL;
	int location_s_length = (int)strlen(location_s);

	// Tokenize the string
	for (int i = 0; i < location_s_length; i++) {
		if (location_s[i] == '|') {
			location_s[i] = '\0';
			longitude_string = &location_s[i+1];
			break;
		}
	}

	if (longitude_string == NULL) {
		return NULL; // could not tokenize the location_s
	}

	// double latitude_d = string_to_double(latitude_string);
	// double longitude_d = string_to_double(longitude_string);

	location_t* location = malloc(sizeof(location_t)); // must free later
	if (location == NULL){return NULL;}
	location->latitude = latitude_string;
	location->longitude = longitude_string;

	return location;
}


// /*
//  * This code converts a string to a double.
//  * Useful for the location functions.
//  * Code ideas from https://crackprogramming.blogspot.com/2012/10/implement-atof.html
//  */
// double string_to_double(char* number_s)
// {
// 	if (!number_s || !*number_s) {
// 		return 0; // validate input
// 	}

// 	char* num_p = number_s; // don't manipulate the parameter pointer

// 	// set defaults
// 	double intSection =  0;
// 	double fracSection =  0;
// 	int divisor = 1;
// 	int sign = 1;
// 	int in_fraction = 0; // 0 is false, 1 is true

// 	// check the sign
// 	if (*num_p == '-') {
// 		num_p++;
// 		sign = -1;
// 	}

// 	// loop through the string until the end, \0
// 	while (*num_p != '\0') {
// 		if (*num_p >= '0' && *num_p <= '9') { // make sure the character is a number
// 			if (in_fraction != 0)  { // fraction section
// 				fracSection = (fracSection * 10) + (*num_p - '0');
// 				divisor *= 10;
// 			} else { // int section
// 				intSection = (intSection * 10) + (*num_p - '0');
// 			}
// 		} else if (*num_p == '.') {
// 			if (in_fraction != 0) { // already in fraction section -> abort
// 				return sign * (intSection + fracSection/divisor);
// 			} else { // move to the fraction section
// 				in_fraction++;
// 			}
// 		} else { // not a number or dot -> abort
// 			return sign * (intSection + fracSection/divisor);
// 		}
// 		num_p++;
// 	}

// 	// return if you haven't already
// 	return sign * (intSection + fracSection/divisor);
// }

