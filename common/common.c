#ifdef NOPEBBLE // we are *not* building for pebble
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



/**************** file-local global variables ****************/
static const int MESSAGE_LENGTH = 8192;

unsigned int stringHexToDec(char* hex){
    unsigned int decimalNumber;
    sscanf(hex,"%x", &decimalNumber);

    return decimalNumber;
}

char *decToStringHex(unsigned int dec){
    // initialize
    char *hex = malloc(5);
    
    sprintf(hex, "%x", dec);
    
    return hex;
}



// not robust
// no error check and handling
char **getOpCode(char *message){
    char **tokens = malloc(2 * sizeof(char *));
    for (int i = 0; i < 2; i ++){
        tokens[i] = malloc(MESSAGE_LENGTH);
    }
    
    char *buf = malloc(MESSAGE_LENGTH);
    int j = 0;
    int k = 0;
    int m = 0;
    int state = 0;     // currently reading the opCode?
                       // 0: no     1: yes
    
    for(int i = 0; i < strlen(message); i++){
        if (state == 0){
            tokens[1][k] = message[i];
            k++;
        }
        else {
            tokens[0][m] = message[i];
            m++;
        }
        
        if (message[i] == '='){
            buf[j] = '\0';
            if (strcmp(buf,"opCode") == 0){
                state = 1;
                k -= 7;
            }
            j = 0;
        }
        else if (message[i] == '|'){
            buf[j] = '\0';
            j = 0;
            if (state == 1){
                state = 0;
                tokens[0][m-1] = '\0';
            }
        }
        else{
            buf[j] = message[i];
            j++;
        }
        
        if (i == strlen(message)-1){
            tokens[1][k] = '\0';
        }
    }
    free(buf);
    return tokens;
}

void deleteopCode(char **token){
    free(token[0]);
    free(token[1]);
    free(token);
}


#else // we are building for pebble
#include <pebble.h>
#endif