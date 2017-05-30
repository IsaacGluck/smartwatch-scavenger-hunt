# Makefile for gameserver
#
# Kazuma Honjo, May 2017

C = ../common
L = ../libcs50
#DEBUG = -DEBUG

PROG = gameserver
OBJS = gameserver.o gsstruct.o gs_message_handler.o gs_response_handler.o
LLIBS = $L/libcs50.a $C/common.a


CFLAGS = -Wall -pedantic -std=gnu11 -ggdb -I$C -I$L -lm $(DEBUG)
CC = gcc
MAKE = make


# executable depends on object files
all: $(PROG)

$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LLIBS) -o $@


# object files depend on include files
gsstruct.o: $L/set.h $L/file.h gsstruct.h $C/common.h
gs_message_handler.o: gsstruct.h
gs_response_handler.o: gsstruct.h
gameserver.o: $L/set.h $C/common.h


# phony testing
.PHONY: test testclean clean



test: $(PROG)


testclean:


# valgrind
valgrind: 

clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f $(TPROG)
