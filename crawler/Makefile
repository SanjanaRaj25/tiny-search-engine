# Makefile for CS50 Crawler
#
# Sanjana Raj, April 2023


############## default: build the crawler executable program ##########

OBJS = crawler.o ../libcs50/file.o ../common/pagedir.o
LIBS = 
# define local library that will call make on  libcs50
LL = $L/libcs50-given.a
L = ../libcs50
# MM = ..$M/common.a
# M = ../common
.PHONY: all test valgrind clean

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -g -ggdb $(TESTING) -I$L -I../common
MAKE = make

# for memory-leak tests
VALGRIND = valgrind -s --leak-check=full --show-leak-kinds=all

# compile: $make
crawler: $(OBJS) $(LL)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

## test cases: $make test
# test: testing.sh
# 	./testing.sh &> testing.out
test:
	bash -v testing.sh &> testing.out
# valgrind: $make valgrind
valgrind: crawler
	$(VALGRIND) ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html data/valgrind 0

# valgrind: $make clean
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f crawler
	rm -f core
