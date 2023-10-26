# CS50 TSE Querier
## Implementation Spec

In this document we reference the [Requirements Specification](REQUIREMENTS.md) and [Design Specification](DESIGN.md) and focus on the implementation-specific decisions.
Here we focus on\:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 
We use 1 new data structures: `twocounters`is a local structure that contains pointers to two counter objects and is used to intersect counters.

## Control flow

The Querier is implemented in one file `querier.c`, with X functions.

### main

The `main` function simply calls `parseArgs`, stores the page directory as aa global variable, loads the index file into an index object, continually reads and handles queries from stdin, then cleans up and exits 0.

### parseArgs

Given arguments from the command line, extract them into the function parameters; return only if successful.

* for `pageDirectory`, make sure that the directory contains a crawler file that can be opened, and the first file in the page directory can be opened
* for `indexName`, make sure it's a pathname of a file that can be read
* if any trouble is found, print an error to stderr and exit non-zero.

### tokenizeQuery

Convert a query into a list of words with the last entry in the list being set to NULL, and return it.

### validateQuery

Go through every word in the list and normalize it, make sure that there are no syntax errors in the query.

### counters_merge

Given two counter objects, add all elements in the second that are not already in the first, to the first. This functionally is a "union" operation with countersets.

### counters_intersect

Given two counter objects, return a pointer to a third counterset that contains all items that are in BOTH the first and second counters. This functionally is an "intersect" operation with countersets.

### accumulateQuery
Given a tokenized and validated list of words, create a counterset of all documents that satisfy the query represented in the list. 

### rankDocs
Given a set of documents, continually extract and print the best scoring document, along with its score and URL.

### handleQuery
Handle an individual query by tokenizing and validating it, printing the cleaned query if both succeed, creating the counterset of satisying documents, and then printing according to the requirements and design specs/

### libcs50

We leverage the modules of libcs50, most notably `counters`, `hashtable`, and `file`.
See that directory for module interfaces.


## Function prototypes

### crawler

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `crawler.c` and is not repeated here.

```c
char** tokenizeQuery(char* currLine);
bool validateQuery(char** tokenList, char* currLine);
counters_t* counters_intersect(counters_t* counterA, counters_t* counterB);
counters_t* accumulateQuery(char** wordList, int start, int end, index_t* newIndex);
void handleQuery(char* currLine, char* pageDir, index_t* newIndex);
int main(const int argc, char* argv[]);
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status. All code uses defensive-programming tactics to catch and exit. If a query fails due to syntax errors, an error message will be printed and the user will be able to ask another query. If the calling of querier itself fails, the users will have to call it again.

## Testing plan

The testing plan described in `DESIGN.md`is implemented in testing.sh, with output written to testing.out. There are three components to the testing plan:

1. testing the commandline parsing, and its handling of errors
2. testing the query parsing, and its handling of errors
3. test the querier with a variety of subdirectories, using fuzzquery

