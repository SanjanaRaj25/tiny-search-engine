# Makefile for CS50 Common
#
# Sanjana Raj, April 2023


############## default: build the common.a library ##########

OBJS = pagedir.o index.o word.o
LIBS = common.a

.PHONY: all test valgrind clean

CFLAGS = -Wall -pedantic -std=c11 -g -ggdb $(TESTING)
CC = gcc
MAKE = make

# for memory-leak tests
VALGRIND = valgrind -s --leak-check=full --show-leak-kinds=all

# build
$(LIBS): $(OBJS)
	ar cr $(LIBS) $(OBJS)

# dependencies
pagedir.o: pagedir.h
index.o: index.h
word.o: word.h

# clean 
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f crawler
	rm -f core
	rm -f $(LIBS) *~ *.o
