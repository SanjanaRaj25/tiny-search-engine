#!/bin/bash
#
# testing: invokes the crawler several times, with a variety of command-line arguments to demonstrate that crawler.c works with various cases

# usage: ./crawler seedURL pageDirectory maxDepth

# Sanjana Raj, April 2023


# make crawler
make crawler

# define vars: output directory, urls
seedURL=http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
pageDirectory=data

# First, a sequence of invocations with erroneous arguments, testing each of the possible mistakes that can be made. 

#  not enough arguments [test 1]
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html 

./crawler $seedURL $pageDirectory 
if [ $? -ne 0 ]; then
    echo "not enough many arguments, returned non-zero"
    echo "test 1 passed!"
else
    echo "test 1 failed :("
fi

# #  invalid URL: null [test 2]
./crawler NULL data 2 
if [ $? -ne 0 ]; then
    echo "invalid url, returned non-zero"
    echo "test 2 passed!"
else
    echo "test 2 failed :("
fi

# #  invalid URL: invalid extension [test 3]
./crawler https://www.ducksters.com/jokes/silly.php data 3
if [ $? -ne 0 ]; then
    echo "invalid extension, returned non-zero"
    echo "test 3 passed!"
else
    echo "test 3 failed :("
fi

#  invalid URL: not internal [test 4]
./crawler https://www.semrush.com/blog/external-links/ data 3
if [ $? -ne 0 ]; then
    echo "external URL, returned non-zero"
    echo "test 4 passed!"
else
    echo "test 4 failed :("
fi

# #  invalid URL: not a URL [test 5]
./crawler NOTAWEBSITE data 2
if [ $? -ne 0 ]; then
    echo "invalid URL, returned non-zero"
    echo "test 5 passed!"
else
    echo "test 5 failed :("
fi

# #  invalid page directory: null file [test 6]
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html NULL 2
if [ $? -ne 0 ]; then
    echo "invalid page directory"
    echo "test 6 passed!"
else
    echo "test 6 failed :("
fi

# #  invald maxDepth: negative [test 7]
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html data -10
if [ $? -ne 0 ]; then
    echo "negative maxDepth, returned non-zero"
    echo "test 7 passed!"
else
    echo "test 7 failed :("
fi

# #  invald maxDepth: too big [test 8]
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html data 15
if [ $? -ne 0 ]; then
    echo "too big maxDepth, returned non-zero"
    echo "test 8 passed!"
else
    echo "test 8 failed :("
fi


# # Second, a run with valgrind over a moderate-sized test case (such as toscrape at depth 1). 

valgrind --leak-check=full --show-leak-kinds=all ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html $pageDirectory/valgrind 1

# Third, runs over all three CS50 websites

./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $pageDirectory/letters/depth1 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $pageDirectory/letters/depth2 2
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $pageDirectory/letters/depth3 3
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $pageDirectory/letters/depth10 10

if [ $? -eq 0 ]
then
  echo "letters test worked!"
else
  echo "letters test failed!"
fi

seedURL=http://cs50tse.cs.dartmouth.edu/tse/toscrape/
./crawler $seedURL $pageDirectory/toscrape/depth0 0
./crawler $seedURL $pageDirectory/toscrape/depth2 2
./crawler $seedURL $pageDirectory/toscrape/depth3 3
if [ $? -eq 0 ]
then
  echo "toscrape test worked! look in the toscrape subdirectory of data to see the output!"
else
  echo "letters test failed!"
fi

seedURL=http://cs50tse.cs.dartmouth.edu/tse/wikipedia/
./crawler $seedURL $pageDirectory/wikipedia/depth0 0
./crawler $seedURL $pageDirectory/wikipedia/depth1 1
./crawler $seedURL $pageDirectory/wikipedia/depth2 2

