

L = ../libcs50


# object files and the target library
OBJS = shared.o common.o
LIB = common.a


# build the library by archiving object files
$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

shared.o: shared.h
common.o: common.h

.PHONY: clean sourcelist

sourcelist: Makefile *.md *.c *.h
	@echo $^

clean:
	rm -f *~ *.o *.dSYM
	rm *.a
