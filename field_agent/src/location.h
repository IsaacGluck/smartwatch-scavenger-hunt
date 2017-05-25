/*****************************************************************/
/* This program          */
/*****************************************************************/
#include <pebble.h>


typedef struct location_struct location_t;

location_t *parse_location(char* location_s);

double string_to_double(char* number_s);