# CS50 TSE QUERIER
## Design Spec

In this document we reference the [Requirements Specification](REQUIREMENTS.md) and focus on the implementation-independent design decisions.

Here we focus on:

- User interface
- Inputs and outputs
- Functional decomposition into modules
- Pseudo code (plain English-like language) for logic/algorithmic flow
- Major data structures
- Testing plan

## User interface

As described in the [Requirements Spec](REQUIREMENTS.md), the querier has two forms of interface with the user.

It initially executes by receiving 2 arguments via the command-line.

```bash
$ querier pageDirectory indexFilename
```

For example, to query a crawler-produced pageDirectory named 'data/letters/depth2' that's located in the crawler subdirectory, and to reference an indexer produced file that is in the same subdirectory, use this command line: 

``` bash
$ ./querier ../crawler/data/letters/depth2 ../crawler/data/letters/depth2/index
```
Then, the user can enter search queries through stdin, and recceive a ranked set of documents, in decreasing order, each with a listed score, document ID, and URL.

## Inputs and outputs

*Input:* there are no file inputs; there are command-line parameters described above.

*Output:* Per the requirements spec, Querier will print a set of douments in dereasing rank order. For example, if we consider the three documents (D1, D2, D3) and three queries:

|query             | D1                  | D2                   | D3                  |
|:-----------------|:--------------------|:---------------------|:--------------------|
*matches*          | 0 cat, 5 dog, 7 emu | 3 cat, 2 dog, 1 emu  | 3 cat, 4 dog, 0 emu |
cat and dog        | score = 0           | score = 2            | score = 3*          |
cat or  dog        | score = 5           | score = 5            | score = 7*          |
dog or  dog        | score = 10*         | score = 4            | score = 8           |
cat and dog or emu | score = 7*          | score = 3            | score = 3           |

where * indicates the highest rank for each query.

If the user enters the single-word query "emu", then the program should output 

"7 D1 URL"
"1 D2 URL"
"0 D3 URL

If the user enters the query "cat or dog", then the program should output

"7 D3 URL"
"5 D2 URL"
"5 D1 URL

## Functional decomposition into modules

We anticipate the following modules or functions:

 1. *main*, which parses arguments and initializes other modules
 2. *parseArgs*, which validates arguments from the command line
 3. *querier*, which reads all search queries from stdin and handles them, until EOF
 4. *handleQuery*, which handles an individual query by cleaning and validating them

## Pseudo code for logic/algorithmic flow

The queries will run as follows:

    parse the command line, validate parameters, initialize other modules
    load the index file into a new index structure
    while there are search queries in stdin
      read the line
      handle the query
        clean/validate the query by ignoring blank lines
        translate upper-case letters to lowercase
        split lines into words and sequences
        print the cleaned up query
        identify a set of documents that satisfy the query
        rank them based on score
        print them in order of decreasing rank: 
    free any extraneous memory
    exit 0

## Major data structures

Helper modules provide all the data structures we need:

- *index* extended from common subdirectory, index.h contains more information
- *counters* extended from libcs50 subdirectory, counters.h contains more information
- *hashtable* extended from libcs50 subdirectory, hashtable.h contains more information
- *file* extended from libcs50 subdirectory, file.h contains more information
- *word* extended from libcs50 subdirectory, word.h contains more information
- *twocounters* struct that contains pointers to two counter objects, can be used 

## Testing plan

A sampling of tests that should be run:

1. Test the program with various forms of incorrect command-line arguments to ensure that its command-line parsing, and validation of those parameters, works correctly.

2. Test the program with various forms of syntactically incorrect queries, ensuring that they are handled correctly.

3. Test the program using fuzzquery and by piping in crawler produced directories

