//include statements
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Other methods 
void checkArgs(const int argc, char *argv[]); 

int main(const int argc, char *argv[]){	
	checkArgs(argc, argv[]); 



exit(0);
}


void checkArgs(const int argc, char *argv[]){
	if(argc != 3){
		exit(1); 
	}

}