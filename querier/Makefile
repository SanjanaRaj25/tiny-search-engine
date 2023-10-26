# Makefile for CS50 Indexer
#
# Sanjana Raj, April 2023


############## default: build the indexer executable program ##########

OBJS_A = querier.o ../libcs50/file.o ../common/pagedir.o ../common/index.o ../common/word.o
OBJS_B = fuzzquery.o ../libcs50/file.o ../common/pagedir.o ../common/index.o ../common/word.o
LIBS = 
# define local library that will call make on  libcs50
LL = $L/libcs50.a
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
all: querier
querier: $(OBJS_A) $(LL)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@
fuzzquery: $(OBJS_B) $(LL)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@


# test: $make test
test:
	bash ./testing.sh &> testing.out

# valgrind: $make valgrind
valgrind: querier
	$(VALGRIND) echo "home" | ./querier ../crawler/data/letters/depth3 ../crawler/data/letters/depth3/index


# valgrind: $make clean
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f querier
	rm -f core
