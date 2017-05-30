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