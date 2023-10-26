/*
 * indexer.c     Sanjana Raj     April, 2023
 *
 * The TSE indexer is a standalone program that reads the document 
 * files produced by the TSE crawler, builds an index, and writes that
 * index to a file. Its companion, the index tester, loads an index
 * file produced by the indexer and saves it to another file.
 *
 * refer to the requirement, design, and implementation specs for more information
 *     
 * pseudocode for logic and algorithmic flow was taken from the provided design spec
 *
 * indexer executes from a command line with the syntax ./indexer pageDirectory indexFilename
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "../libcs50/hashtable.h"
#include "../libcs50/webpage.h"
#include "../libcs50/mem.h"
#include "../libcs50/file.h"
#include "../libcs50/counters.h"

#include "../common/pagedir.h"
#include "../common/index.h"
#include "../common/word.h"

/**************** function prototypes ****************/
void indexPage(char* pageDirID, index_t* index, int documentID);
index_t* indexBuild(char* pageDirectory)
int main(const int argc, char* argv[]);

/**************** indexPage ****************/
/* scans a webpage document to add its words to the index */
void indexPage(char* pageDirID, index_t* index, int documentID) {

    // pagedir load
    webpage_t*  webpage = pagedir_load(pageDirID);
    int pos = 0;
	char* currWord;

//  steps through each word of the webpage,
    while ((currWord = webpage_getNextWord(webpage, &pos)) != NULL) {

 //    skips trivial words (less than length 3),
        if (strlen(currWord) >= 3) {

//          normalizes the word (converts to lower case),
            currWord = normalizeWord(currWord);

//          adding the word to the index if needed
            if (index_find(index, currWord) == NULL) {
                counters_t* wordCount = counters_new();
                counters_add(wordCount, documentID);
                index_insert(index, currWord, wordCount);
            }
//          increments the count of occurrences of this word in this docID
            else {
                counters_t* count = index_find(index, currWord);
                counters_add(count, documentID);
            }
        }   
        free(currWord);  
    }
     webpage_delete(webpage);
}

/**************** indexBuild ****************/
/* builds an in-memory index from webpage files found in the pageDirectory */
index_t* indexBuild(char* pageDirectory)
{

// defensive check: make sure pageDirectory is valid
	if (!pagedir_validate(pageDirectory)){
		fprintf(stderr, "passed in pageDirectory is not valid \n");
		exit(1);
	}
//    creates a new 'index' object
    index_t* newIndex = index_new(200);

//   loops over document ID numbers, counting from 1
    int documentID = 1;
    // webpage_t* currPage;
    char* pageDirID = mem_malloc((strlen(pageDirectory)) + 15);
    sprintf(pageDirID, "%s/%d", pageDirectory, documentID);
    FILE* buildPage;

    while ((buildPage = fopen(pageDirID, "r")) != NULL) {

        indexPage(pageDirID, newIndex, documentID);

//      increment values before next loop
        documentID++;
        fclose(buildPage);
        sprintf(pageDirID, "%s/%d", pageDirectory, documentID);

    }
    mem_free(pageDirID);
    return newIndex;
}

/**************** main ****************/
/* The main function parses arguments and initializes other modules */
int main(const int argc, char* argv[])
{
    // make sure there are exactly 2 arguments
    if (argc != 3){
		fprintf(stderr, "indexer requires exactly 2 arguments, usage = ./indexer pageDirectory indexFilename" );
		exit(1);
	}
    // parse the command line
    char* pageDirectory = argv[1];
    char* indexFilename = argv[2];
    FILE* argFile;

    // validate parameters:

    // confirm that a .crawler file exists in page directory and can be read
    char* fileName = "/.crawler";
    char * pathName = mem_malloc((strlen(pageDirectory) + strlen(fileName) + 1) * sizeof(char));
    sprintf(pathName, "%s%s", pageDirectory, fileName);

    // open the .crawler file 
    FILE* crawlerFile = NULL;
    crawlerFile = fopen(pathName, "r");
    free(pathName);
  
    // defensive check: the .crawler file can be opened

    if (crawlerFile == NULL) {
        fprintf(stderr, "the .crawler file in the given pageDirectory could not be opened\n");
        exit(3);
    } else {
        fclose(crawlerFile);
    }

    // make sure it's possible to open & write into the indexFilename file
    argFile = fopen(indexFilename, "w");
    if (argFile == NULL){
        fprintf(stderr, "file could not be written into" );
		exit(1);
    }
    else {
        fclose(argFile);
    }
    // initialize other modules
    // call indexBuild, with pageDirectory
    index_t* newIndex = indexBuild(pageDirectory);
    index_save(indexFilename, newIndex);
    // index_delete(newIndex);
    exit(0);
}


