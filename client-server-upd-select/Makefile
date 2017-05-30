# Makefile for UDP chat-server examples
#
# David Kotz - May 2016, May 2017

PROGS = chatclient1 chatserver1 chatclient2 chatserver2
LIBS = 
LLIBS = 

# added  -Wno-braced-scalar-init because Linux and MacOS disagree
FLAGS = # 
CFLAGS = -Wall -Wno-braced-scalar-init -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

all: $(PROGS)

########### chatclient1 ##################
chatclient1: chatclient1.o file.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

chatclient1.o: file.h

########### chatserver1 #################
chatserver1: chatserver1.o file.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

chatserver1.o: 

########### chatclient2 ##################
chatclient2: chatclient2.o file.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

chatclient2.o: file.h

########### chatserver2 #################
chatserver2: chatserver2.o file.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

chatserver2.o: 

clean:
	rm -rf *~ *.o *.dSYM
	rm -f $(PROGS) 
