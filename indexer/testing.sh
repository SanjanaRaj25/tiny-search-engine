#!/bin/bash
#
# testing: invokes the indexer several times, with a variety of command-line arguments to demonstrate that indexer.c works with various cases, output is written to testing.out

# usage: ./indexer pageDirectory indexFilename

# Sanjana Raj, April 2023

# make indexer and indextest
make indexer
make indextest


# Test indexer with various invalid arguments:

# test 1: no arguments
./indexer
if [ $? -ne 0 ]; then
    echo "not enough arguments, returned non-zero"
    echo "test 1 passed!"
else
    echo "test 1 failed :("
fi

# test 2: one argument
./indexer ../crawler/data/letters/depth3
if [ $? -ne 0 ]; then
    echo "not enough arguments, returned non-zero"
    echo "test 2 passed!"
else
    echo "test 2 failed :("
fi

# test 3: 3+ arguments
./indexer ../crawler/data/letters/depth3 ../crawler/data/letters/depth3/index dsfgag
if [ $? -ne 0 ]; then
    echo "too many arguments, returned non-zero"
    echo "test 3 passed!"
else
    echo "test 3 failed :("
fi

# test 4: invalid pageDirectory (non-existent path)
./indexer ../crawler/data/hgfhjjvgh ../crawler/data/letters/depth3/index 
if [ $? -ne 0 ]; then
    echo "returned non-zero"
    echo "test 4 passed!"
else
    echo "test 4 failed :("
fi

# test 5: invalid pageDirectory (not a crawler directory)
./indexer ../common ../crawler/data/letters/depth3/index 
if [ $? -ne 0 ]; then
    echo "returned non-zero"
    echo "test 5 passed!"
else
    echo "test 5 failed :("
fi

# test 6: invalid indexFile (non-existent path)
./indexer ../crawler/data/letters/depth3/sdvqwdfqwe indices/letters_3
if [ $? -ne 0 ]; then
    echo "returned non-zero"
    echo "test 6 passed!"
else
    echo "test 6 failed :("
fi

# test 7: invalid indexFile (read-only directory, located in my indexer directory, used chmod a=r)
./indexer readonly indices/letters_10_loaded_index
if [ $? -ne 0 ]; then
    echo "returned non-zero"
    echo "test 7 passed!"
else
    echo "test 7 failed :("
fi

# # test 8: invalid indexFile (existing, read-only file)
./indexer ../crawler/data/letters/depth3 readonlyfile
if [ $? -ne 0 ]; then
    echo "returned non-zero"
    echo "test 8 passed!"
else
    echo "test 8 failed :("
fi

# Run indexer on a variety of pageDirectories and use indextest as one means of validating the resulting index.
./indexer ../crawler/data/letters/depth3 indices/letters_3_index
if [ $? -ne 0 ]; then
    echo "test failed :("
else
    echo "test passed on letters, depth 3"
fi

./indexer ../crawler/data/letters/depth10 indices/letters_10_index
if [ $? -ne 0 ]; then
    echo "test failed :("
else
    echo "test passed on letters, depth 10"
fi

# run on a couple different crawler produced directories

./indexer ../crawler/data/wikipedia/depth1 indices/wikipedia_1_index
if [ $? -ne 0 ]; then
    echo "test failed :("
else
    echo "test passed on wikipedia, depth 1"
fi

./indexer ../../shared/tse/crawldata/wikipedia_2 indices/wikipedia_2_index
if [ $? -ne 0 ]; then
    echo "test failed :("
else
    echo "test passed on wikipedia, depth 2"
fi

./indexer ../../shared/tse/crawldata/toscrape-1 indices/toscrape_1_index
if [ $? -ne 0 ]; then
    echo "test failed :("
else
    echo "test passed on to scrape, depth 1"
fi

 #./indextest indices/letters_10_index indices/letters_10_loaded_index
# run indextest on some different index files
./indextest ../../shared/tse/indices/toscrape-1.ndx indices/toscrape_1_loaded_index
./indextest indices/letters_10_index indices/letters_10_loaded_index

# use indexcmp to validate output-- if nothing is printed, then files were the same

../../shared/tse/indexcmp indices/toscrape_1_index ../../shared/tse/indices/toscrape-1.ndx
../../shared/tse/indexcmp indices/wikipedia_2_index ../../shared/tse/indices/wikipedia_2.ndx
../../shared/tse/indexcmp indices/letters_10_index indices/letters_10_loaded_index

# Run valgrind on both indexer and indextest to ensure no memory leaks or errors.
# runs on indexer
make valgrind

# # runs on indextest
valgrind ./indextest ../../shared/tse/indices/toscrape-1.ndx indices/toscrape_1_loaded_index

