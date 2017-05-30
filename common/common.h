#ifndef __COMMON_H_
#define __COMMON_H_

int stringHexToDec(char* hex);
char *decToStringHex(int dec);

char **getOpCode(char *message);
void deleteOpCode(char **token);

#endif
