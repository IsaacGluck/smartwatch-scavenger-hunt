/*****************************************************************/
/* This program          */
/*****************************************************************/

#include "message_handler.h"

















// void double_to_string(double to_str, char *str)
// {
// 	// Get before the .
//   int before = (int)to_str;

//   // Get after the .
//   float after = to_str - (float)before;

//   // make part a string
//   int_to_string(before, str);
//   int len = strlen(str);

//   str[len] = '.';  // add the .

//   // Get the value of fraction part upto given no.
//   // of points after dot. The third parameter is needed
//   // to handle cases like 233.007
//   fpart = fpart * pow(10, afterpoint);

//   intToStr((int)fpart, res + i + 1, afterpoint);

// }







// void int_to_string(int to_str, char str[])
// {
//     char temp[strlen(str)];

//     // move the int backwards to temp
//     int i = 0;
//     while (to_str) { // while to_str is not 0    
//         temp[i] = (to_str % 10) + '0'; // add '0' to get ascii code of number
//         to_str = to_str / 10; // get rid of last digit
//         i++;
//     }

//     // reverse temp
//     int j = i-1;
//     for (int k = 0; k < i; k++) {
//         str[k] = temp[j];
//       	j--;
//     }
//     str[i] = '\0';
// }