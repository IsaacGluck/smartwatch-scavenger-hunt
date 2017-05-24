# Makefile for gameserver
#
# Kazuma Honjo, May 2017

C = ../common
L = ../libcs50

PROG = gameserver
OBJS = gameserver.o gsstruct.o
LLIBS = $L/libcs50.a #$C/common.a


CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$C -I$L
CC = gcc
MAKE = make


# executable depends on object files
all: $(PROG)

$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LLIBS) -o $@


# object files depend on include files
gameserver.o: $L/set.h
gsstruct.o: $L/set.h gsstruct.h #$C/


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