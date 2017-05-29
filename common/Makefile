# Makefile for common library
#
# Kazuma Honjo, May 2017

L = ../libcs50


# object files and the target library
OBJS = common.o
LIB = common.a

CFLAGS = -Wall -I$L -std=gnu11 -pedantic -ggdb $(TESTING)


# build the library by archiving object files
$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

# clean
clean:
	rm -f *~ *.o *.dSYM
	rm *.a
