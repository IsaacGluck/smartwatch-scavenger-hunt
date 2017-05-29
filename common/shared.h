#ifndef __SHARED_TEST_H
#define __SHARED_TEST_H
#include <stdio.h>
extern const char *version_str;

void print_shared();
int validate_message(char* message);
int print_log(char* message, char* filename, char* IPport, char* tofrom); 


#endif // __SHARED_TEST_H