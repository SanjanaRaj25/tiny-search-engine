/*
 * querier.c     Sanjana Raj     May, 2023
 *
 * The TSE querier is a standalone program that reads the index file produced by the TSE Indexer,
  and page files produced by the TSE Querier, and answers search queries submitted via stdin.
 *
 * refer to the requirement, design, and implementation specs for more information
 *     
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#include "../libcs50/file.h"
#include "../libcs50/bag.h"
#include "../libcs50/hash.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/webpage.h"
#include "../libcs50/mem.h"
#include "../common/pagedir.h"
#include "../common/index.h"
#include "../common/word.h"

/**************** global variables  ****************/
char* pageDir;
counters_t* counterC;

/**************** local structs  ****************/
struct twocounters {
    counters_t* counterA;
    counters_t* counterB;
};

/**************** function prototypes  ****************/
char** tokenizeQuery(char* currLine);
bool validateQuery(char** tokenList, char* currLine);
counters_t* counters_intersect(counters_t* counterA, counters_t* counterB);
counters_t* accumulateQuery(char** wordList, int start, int end, index_t* newIndex);
void handleQuery(char* currLine, char* pageDir, index_t* newIndex);
int main(const int argc, char* argv[]);

/**************** static functions  ****************/
static void parseArgs(const int argc, char* argv[],
                      char* pageDirectory, char* indexFilename);
static void counters_merge_helper(void* arg, const int key, const int count);
static void counters_merge(counters_t* counterA, counters_t* counterB);
static void counters_intersect_helper(void* arg, const int key, const int count);
static void extractMax(void* arg, const int key, int item);
static void rankDocs(void* arg, const int key, int item);

/**************** functions  ****************/

/**************** parseArgs ****************/
/* given arguments from the command line, validate them, return only if successsful.*/
// argc = # of arguments, arg[1] = pageDirectory, arg[2] = indexFilename
static void parseArgs(const int argc, char* argv[],
                      char* pageDirectory, char* indexFilename)
{
    // make sure there are exactly 2 arguments
    if (argc != 3){
        fprintf(stderr, "Incorrect number of arguments. Call using the format: querier pageDirectory indexFilename\n");
		exit(1);
	}

    // make sure that pageDirectory is the pathname for a directory produced by the Crawler, by validating the .crawler file in the directory
    char* fileName = "/.crawler";
    char* pathName = malloc((strlen(pageDirectory) + strlen(fileName) + 1) * sizeof(char));
    sprintf(pathName, "%s%s", pageDirectory, fileName);

    // open the .crawler file 
    FILE* crawlerFile = NULL;
    crawlerFile = fopen(pathName, "r");
  
    // defensive check: the .crawler file can be opened
    if (crawlerFile == NULL) {
        fprintf(stderr, "the .crawler file in the given pageDirectory could not be opened :-(\n");
        exit(2);
    } else {
        fclose(crawlerFile);
    }

     // make sure that pageDirectory/1 is a file that can be read
     char* firstFileName = "/1";
     char* firstPathName = malloc((strlen(pageDirectory) + strlen(firstFileName) + 1) * sizeof(char));
     sprintf(firstPathName, "%s%s", pageDirectory, firstFileName);

    // open the pageDirectory/1 file 
    FILE* firstFile = NULL;
    firstFile = fopen(firstPathName, "r");
    
    // defensive check: the pageDirectory/1 file can be opened for reading 
    if (firstFile == NULL) {
        fprintf(stderr, "the first file in the given pageDirectory could not be opened :-(\n");
        exit(3);
    } else {
        fclose(firstFile);
    }

    // make sure indexFilename is the pathname of a file that can be read
    FILE* indexFile;
    indexFile = fopen(indexFilename, "r");
    if (indexFile == NULL){
        fprintf(stderr, "indexFilename is not the pathname of a file that can be read :-(\n" );
		exit(4);
    }
    else {
        fclose(indexFile);
        free(pathName);
        free(firstPathName);
    }
}

/* helper function that converts a query into a list of words and returns it*/
char** tokenizeQuery(char* currLine) {
    int len = strlen(currLine);
    char** wordList = malloc(sizeof(char*) * len/2 + 1);
    bool space = true; // denote if there was just a space and a new word is about to start
    int numWords = 0;

    for (int i = 0; i < len; i++){
        if (!(isalpha(currLine[i]))){
            if(!space){
               currLine[i]='\0';
               space = true;
            }
        }
        else {
            if (space){
                wordList[numWords]=&currLine[i];
                numWords++;
                space = false;
            }
        }
    }
    // set the last entry to null
    wordList[numWords] = NULL;
    return wordList;
}


/* helper function that validates a tokenized query .*/
bool validateQuery(char** tokenList, char* currLine) {
    // go through every word and: normalize it, make sure that there are no adjacent operators
    int len = strlen(currLine);
    int lastOperator=100;
    int i = 0;
    char** wordList = malloc(sizeof(char*)*len/2+1);
    wordList = tokenList;

    while (wordList[i] != NULL) {
        // normalize the word in the passed in array, save it to new array
        // if the current word is an operator
        if ((strcmp(wordList[i], "and") == 0) || (strcmp(wordList[i], "or") == 0)){
            // check if the last word was an operator
            if (i == lastOperator+1) {
                fprintf(stderr, "Error: two operators cannot be adjacent");
                return false;
            }
            // set this word as the last operator
            lastOperator = i;
        }
        i++;
    }


    // check if the first word is "or"
    if (strcmp(wordList[0], "or") == 0) {
        fprintf(stderr, "Error: 'or' cannot be first\n");
        return false;
    }

    // check if the first word is "and"
    if (strcmp(wordList[0], "and") == 0) {
        fprintf(stderr, "Error: 'and' cannot be first\n");
        return false;
    }

    // check if the last word is "or"
    if (strcmp(wordList[i-1], "or") == 0) {
        fprintf(stderr, "Error: 'or' cannot be last\n");
        return false;
    }

    // check if the last word is "and"
    if (strcmp(wordList[i-1], "and") == 0) {
        fprintf(stderr, "Error: 'and' cannot be last\n");
        return false;
    }

    // validated!
    return true;
}

/**************** counters_merge_helper ****************/
/* Consider one item for insertion into the other counter.
 * If the other counter does not contain the item, insert it;
 * otherwise, update the other counter's item with sum of item values.
 */
static void counters_merge_helper(void* arg, const int key, const int count){
    counters_t* counterA = (counters_t*)arg;
    int itemB = count;
    // find the same key in countersA
    int itemA = counters_get(counterA, key);
    if (itemA == 0) {
        // not found: insert it
        counters_set(counterA, key, itemB);
    } else {
        // add to the existing value
        counters_set(counterA, key, itemA+itemB);
    }
}

/**************** counters_merge ****************/
/* merges the second counter into the first counter, second counter is unchanged */
static void counters_merge(counters_t* counterA, counters_t* counterB) {
    counters_iterate(counterB, counterA, counters_merge_helper);
}

/**************** counters_intersect_helper ****************/
/* helper function for counters_intersect */
static void counters_intersect_helper(void* arg, const int key, const int count) {
    struct twocounters* counters = (struct twocounters*)arg;
    counters_t* counterB = counters->counterB;
    counters_t* result = counters->counterA;
    
    // If the key is found in counterB
    int itemB = counters_get(counterB, key);
    if (itemB != 0) {
        // Add (key, item) to result
        counters_set(result, key, itemB);
    }
}

/**************** counters_intersect ****************/
/* finds intersection of two counters and returns it */
counters_t* counters_intersect(counters_t* counterA, counters_t* counterB) {
    counters_t* result = counters_new();
    struct twocounters counters = { result, counterB };
    // for each (key,item) in counterA
    counters_iterate(counterA, &counters, counters_intersect_helper);
    return result;
}

/**************** accumulateQuery ****************/
/* helper function that loops over word list and create final counter set of documents according to precedence rules.*/
counters_t* accumulateQuery(char** wordList, int start, int end, index_t* newIndex) {

    int i = start;
    counters_t* result = counters_new();
    counters_t* temp = NULL;

    // loop through every word
    while (i < end){
        // if not an AND or OR

        if ((strcmp(wordList[i], "and") != 0) && (strcmp(wordList[i], "or") !=0)){
            // find the counter for this word in the index
            counters_t* word = index_find(newIndex, wordList[i]);
         
            // if temp is null, make it the counter for this word
            if (temp == NULL){
                temp = word;
            }
            // intersect this word into the temp counter
            else {
                temp = counters_intersect(temp, word);
            }
        }
        // if it IS an or
        else if (strcmp(wordList[i], "or") == 0) {
            // union this word with words seen so far, and reset temp
            counters_t* word = index_find(newIndex, wordList[i]);
            counters_merge(result, word);
            temp = NULL;
        }
        i++;
    }
    counters_merge(result, temp);
    return result;
}

/**************** extractMax ****************/
/* helper function to find the best scoring document */
static void extractMax(void* arg, const int key, int item) {
    int* maxKey = arg;
    if (item > counters_get(counterC, *maxKey)) {
        *maxKey = key;
    }
}

/**************** rankDocs ****************/
/* helper function to rank documents*/
static void rankDocs(void* arg, const int key, int item) {

    counters_t* docs = arg;
    counterC = docs;
    char* URL; 

    // continually find the max, print it
    int* maxKey = malloc(sizeof(int));
    *maxKey = -1;

    counters_iterate(docs, maxKey, extractMax);

    while (counters_get(docs, *maxKey) != 0){
        counters_iterate(docs, maxKey, extractMax);
        printf("score: %d ", (counters_get(docs, *maxKey)));
        printf("docID: %d ", *maxKey);

        // get the URL for this document!
        char* pathName = malloc((strlen(pageDir) + 50) * sizeof(char));
        sprintf(pathName, "%s/%d", pageDir, *maxKey);

        // open the pageDirectory/1 file 
        FILE* fp;
        fp = fopen(pathName, "r");
        URL = file_readLine(fp);
        printf("URL: %s\n", URL);
        fclose(fp);

        // set the count to 0
        counters_set(docs, *maxKey, 0);
        free(pathName);
        free(URL);
    }
    // if there's nothing in the counter
    free(maxKey);
}

/**************** handleQuery ****************/
/* helper function that handles an individual query.*/
void handleQuery(char* currLine, char* pageDir, index_t* newIndex) {

    char** wordList = tokenizeQuery(currLine);
    bool validated = validateQuery(wordList, currLine);
    // means tokenizing and validating was successful
    if (validated == true && wordList != NULL){
        // print the cleaned up query 
        int i = 0;
        printf("Query: ");
        while (wordList[i] != NULL) {
            printf("%s ", wordList[i]);
            i++;
        }
         printf("\n");

        // process the query
        int start = 0;
        int end = i;
        counters_t* query = accumulateQuery(wordList, start, end, newIndex);

        if (query == NULL){
            printf("no matches for query.");
        }

        // iterate through the counterset and print scores in decreasing order of scores
        counters_iterate(query, query, rankDocs);
        }
    
   // was not validated/failed
   else {
        return;
   }
}

/**************** main ****************/
/* The main function calls parseArgs, load_index, querier, then exits 0 */

int main(const int argc, char* argv[])
{
    char* query;
    int end;

    // call parseArgs to validate command-line arguments
    parseArgs(argc, argv, argv[1], argv[2]);

    // store the page directory as a global variable
    pageDir = argv[1];

    // call load_index -- returns an index object with words as keys, and counters for each doc in the subdirectory as item
    // my index_load function writes the index out to a new file (for indextest), but that isn't necessary for the purposes of querier, I just write it out to a secondary testing file
    char* outfile = "indexout";
    index_t* newIndex = index_load(argv[2], outfile);

    // read from stdin until end of file
    // while (!feof(stdin)) {
    //     query = file_readLine(stdin);
    //     end = strlen(query);
    //     // make sure query has at least one letter
    //     if (end == 0) {
    //         fprintf(stderr, "query contains invalid input\n");
    //         continue;
    //     }
    //     // make sure the query only contains letters and spaces
    //     for (int i = 0; i < end; i++){
    //         if (!(isalpha(query[i])) && !(isspace(query[i]))){
    //             fprintf(stderr, "query contains invalid input: only enter letters and spaces!\n");
    //             exit(1);
    //         }
    //     }
    //     // make everything lowercase
    //     query = normalizeWord(query);

    //     // run querier
    //     handleQuery(query, argv[1], newIndex);
    // }

     while ((query = file_readLine(stdin)) != NULL) {
        end = strlen(query);
        // make sure query has at least one letter
        if (end == 0) {
            fprintf(stderr, "query contains invalid input\n");
            continue;
        }
        // make sure the query only contains letters and spaces
        for (int i = 0; i < end; i++){
            if (!(isalpha(query[i])) && !(isspace(query[i]))){
                fprintf(stderr, "query contains invalid input: only enter letters and spaces!\n");
                exit(1);
            }
        }
        // make everything lowercase
        query = normalizeWord(query);

        // run querier
        handleQuery(query, argv[1], newIndex);
    }
   
    // cleanup
    free(query);
    index_delete(newIndex);
}