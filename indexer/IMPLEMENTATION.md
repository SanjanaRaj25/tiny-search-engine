# CS50 TSE Indexer
## Implementation Spec

In this document I reference the Requirements and Design Specs, and focus on implementation-specific decisions. The format and some on the phrasing in this document was based heavily off of the Implementation Spec provided for Crawler.

Here we focus on the core subset:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 

We use 3 new data structures: an 'index' that contains word keys and associated counters in each page, and a 'word' object that implements the "normalizeWord" function, and extend pagedir to load and validate pages.

Other key data structures were reused from past sections of the lab: webpage, file, hashtable, and counters are all located in libcs50, with detailed header files.

## Control flow

The Indexer is implemented in one file `index.c`, with three functions.

### main

The `main` function simply validates parameters by performing a series of defensive checks, calls a function to build the index, saves the index, then returns 0.

### indexBuild

Given a pagedirectory from the command line, build an in-memory index

Pseudocode:

	make sure the pageDirectory is valid
	create a new index object
	loop over all the document ID numbers, starting at 1
	create a pagedirID path for each document
	open a file and call index page
	return the index

### indexPage

Given a pageDirectoryID, index, and documentID, scan a webpage document to add its words to the index

Pseudocode:

	load the webpage
	step through each word of the webpage
		skip trivial words
		normalize the word
		add the word to the index if necessary
		if not, incrememnt the count of this word in this document
	free the word and delete the webpage


## Other modules

### pagedir

We extend the re-usable module `pagedir.c` to confirm that a pageDirectory was in fact Crawler-produced, and then to load a file from a Crawler-produced pageDirectory.

We chose to write this as a separate module, in `../common`, to encapsulate all the knowledge about how to initialize, save, validate, and load page files.


### libcs50

We leverage the modules of libcs50, most notably `counters`, `hashtable`, and `webpage`.
See that directory for module interfaces.

## Function prototypes

### indexer

Detailed descriptions of each function's interface is provided through commenting in each indexer function and is not repeated here. 

```c
void indexPage(char* pageDirID, index_t* index, int documentID);
index_t* indexBuild(char* pageDirectory)
int main(const int argc, char* argv[]);
```

### pagedir

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `pagedir.h` and is not repeated here.

```c
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
webpage_t* pagedir_load(char* fileName);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

```
### word

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `word.h` and is not repeated here.

```c
normalizeWord(char* word);

```

### index

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `index.h` and is not repeated here.

```c
index_t* index_new(int numSlots);
void* index_get(index_t* index);
void* index_find(index_t* index, const char* word);
bool index_save(const char* file, index_t* index);
void index_delete(index_t* index);
bool index_insert(index_t* index, const char* key, void* item);
index_t* index_load(char* indexFilename, char* outFilename);

```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status. All code uses defensive-programming tactics to catch and exit.


## Testing plan

Here is an implementation-specific testing plan.

### Unit testing

There are four units (indexer, pagedir, index, and word).
The indexer represents the whole system and is covered below.
Each of the other units can be tested by observing their behavior during the system test.

### indextest
We write a file `indextest.c` that can be used to validate the accuracy of our indexer module, and act as a unit test. indextest receives an indexfile, then runs index_load on it, which turns it into an index object (basically reverse engineering it). This object is turned back into a file using index_save. Later, we run indexcmp on this file and the original file, to validate the accuracy of the index files that indexer produces.

### Integration/system testing

We write a script `testing.sh` that invokes the indexer several times: first with all permutations of erroneous arguments, then on a variety of pagedirectories (both produced via my crawler in the last lab and from the shared crawler output), then indextest is called on different indexfiles.

Then, indexcmp is called to validate the output of some of these index files and calls. If it prints nothing when comparing two files, it means that they are the same.

Verify correct behavior by studying the output, and by sampling the files created.

