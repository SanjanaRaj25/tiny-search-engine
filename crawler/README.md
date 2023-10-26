# CS50 TSE CRAWLER
## Sanjana Raj (SanjanaRaj25)

## Lab 3 Data Structures
I used my implementation of the set, counters, and hashtable data structures from lab 3. 

The assignment and Specs are in a [public repo](https://github.com/CS50Spring2023/labs/tse).

## Usage

To build `crawler`, run `make`. 

To test, run `make test`. 

To get a valgrind report, run `make valgrind`. 

To clean up, run `make clean`.

## Overview

 * `Makefile` - compilation procedure
 * `crawler.c` - the implementation
 * `testing.sh` - test driver
 * `testing.sh` - output from testing
 * `data` - repository containing output from testing crawls (letters, toscrape, and wiki)


## Known bugs

Valgrind produces the following leak summary:

==1233535== 1,600 bytes in 1 blocks are definitely lost in loss record 1 of 1
==1233535==    at 0x484DA83: calloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==1233535==    by 0x10A4B6: hashtable_new (hashtable.c:36)
==1233535==    by 0x1098B7: crawl (crawler.c:111)
==1233535==    by 0x109694: main (crawler.c:43)

==1233535== LEAK SUMMARY:
==1233535==    definitely lost: 1,600 bytes in 1 blocks
==1233535==    indirectly lost: 0 bytes in 0 blocks
==1233535==      possibly lost: 0 bytes in 0 blocks
==1233535==    still reachable: 0 bytes in 0 blocks
==1233535==         suppressed: 0 bytes in 0 blocks

That is, main calls crawl, crawl uses hashtable_new, and 1600 bytes are lost. I've tried fixing this by freeing the hashtable in different ways, as well as using the "given" implementation. 

## Assumptions

No assumptions other than those than are outlined in the specs

## Style Notes

Variable and functions are defined via camel case.

