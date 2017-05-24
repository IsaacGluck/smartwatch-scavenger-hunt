L = ../libcs50

CC=gcc
CFLAGS=-Wall -pedantic -std=c11 -ggdb -I$L
PROG = guideagent
OBJS = guideagent.o
LIBS = -lm 
LLIBS =  $L/libcs50.a-Linux 

.PHONY: all clean

all: $(PROG)

# executable depends on object files
$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LLIBS) $(LIBS) -o $(PROG)

# object files depend on include files
guideagent.o: $L/bag.h $L/file.h $L/hashtable.h $L/webpage.h 

clean:
	rm -f $(PROG)
	rm -f *~ *.o
	rm -rf *.dSYM
