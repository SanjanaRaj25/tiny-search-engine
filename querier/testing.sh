#!/bin/bash
#
# testing: 

# usage: ./querier pageDirectory indexFilename

# Sanjana Raj, May 2023

# make querier
make querier

# test the program with various forms of incorrect command-line arguments to ensure that its command-line parsing, and validation of those parameters, works correctly.

# test 1: no arguments
./querier
if [ $? -ne 0 ]; then
    echo "not enough arguments, returned non-zero"
    echo "test 1 passed!"
else
    echo "test 1 failed :("
fi

# test 2: one argument
./querier ../crawler/data/letters/depth3
if [ $? -ne 0 ]; then
    echo "not enough arguments, returned non-zero"
    echo "test 2 passed!"
else
    echo "test 2 failed :("
fi

# test 3: 3+ arguments
./querier ../crawler/data/letters/depth3 ../crawler/data/letters/depth3/index dsfgag
if [ $? -ne 0 ]; then
    echo "too many arguments, returned non-zero"
    echo "test 3 passed!"
else
    echo "test 3 failed :("
fi

# test 4: invalid pageDirectory (non-existent path)
./querier ../crawler/data/hgfhjjvgh ../crawler/data/letters/depth3/index 
if [ $? -ne 0 ]; then
    echo "returned non-zero"
    echo "test 4 passed!"
else
    echo "test 4 failed :("
fi

# test 5: invalid pageDirectory (not a crawler directory)
./querier ../common ../crawler/data/letters/depth3/index 
if [ $? -ne 0 ]; then
    echo "returned non-zero"
    echo "test 5 passed!"
else
    echo "test 5 failed :("
fi

# test 6: invalid indexFile (non-existent path)
./querier ../crawler/data/letters/depth3/sdvqwdfqwe indices/letters_3
if [ $? -ne 0 ]; then
    echo "returned non-zero"
    echo "test 6 passed!"
else
    echo "test 6 failed :("
fi

# test the program with various forms of syntactically incorrect queries, ensuring that they are handled correctly.

echo "and home" | ./querier ../crawler/data/letters/depth3 ../crawler/data/letters/depth3/index
echo "home and" | ./querier ../crawler/data/letters/depth3 ../crawler/data/letters/depth3/index
echo "home or and algorithm" | ./querier ../crawler/data/letters/depth3 ../crawler/data/letters/depth3/index
echo "or the" | ./querier ../crawler/data/letters/depth3 ../crawler/data/letters/depth3/index
echo "breadth first or and search" | ./querier ../crawler/data/letters/depth3 ../crawler/data/letters/depth3/index
echo "search or" | ./querier ../crawler/data/letters/depth3 ../crawler/data/letters/depth3/index
echo "3245234543" | ./querier ../crawler/data/letters/depth3 ../crawler/data/letters/depth3/index

# test a variety of queries, pagedirectories, use fuzzquery to validate

echo "home" | ./querier ../crawler/data/letters/depth3 ../crawler/data/letters/depth3/index
echo "concatenating and menu or trick" | ./querier ../crawler/data/wikipedia/depth2 ../indexer/indices/wikipedia_2_index

./fuzzquery ../crawler/data/letters/depth3/index 5 1 | ./querier ../crawler/data/letters/depth3 ../crawler/data/letters/depth3/index 
./fuzzquery ../../shared/tse/indices/toscrape-1.ndx 50 3 | ./querier ../../shared/tse/crawldata/toscrape-1 ../../shared/tse/indices/toscrape-1.ndx