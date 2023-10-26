/*
 * index.c     Sanjana Raj     April, 2023
 *
 * a re-usable module that implements an abstract index_t type that represents an index in memory
 *
 * refer to the requirement, design, and implementation specs for more information
 *     
 * function descriptions were taken from the implementation spec
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libcs50/counters.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/file.h"
#include "index.h"
#include "word.h"

 /* index_t: structure to represent a index, and its contents.
 * hashtable keyed by word, stores counters as items
 * counters are keyed by docID, stores count of the number of occurrences of that word in the doc with that ID
 */

/**************** global types  ****************/

static void index_print_helper(void* file, const char* word, void* item);
static void counters_print_helper(void* file, const int key, int item);
static void index_delete_helper(void* item);

typedef struct index {
    hashtable_t* indexTable;
} index_t;

/**************** index_new ****************/
/* see index.h for description */
index_t* index_new(int numSlots){

    if (numSlots > 0){

        index_t* newIndex = malloc(sizeof(index_t));
        // defensive check: allocating memory for index worked
        if (newIndex == NULL) {
            return NULL;
        }
        // create the hashtable-- hashtable_new checks whether this worked
        newIndex->indexTable = hashtable_new(numSlots);
        if (newIndex -> indexTable == NULL){
            free(newIndex);
            return NULL;
        }
        return newIndex;
    }
    return NULL;
}

/**************** index_get ****************/
/* see index.h for description */
void* index_get(index_t* index){
    return index->indexTable;
}

/**************** index_find ****************/
/* see index.h for description */
void* index_find(index_t* index, const char* word){
    // check if any parameters are null
    if (index == NULL || word == NULL) {
        fprintf(stderr, "provided key or index was null");
        return NULL;
    }

    // look up the word and return it
    return hashtable_find(index->indexTable, word);
}

/**************** index_save ****************/
/* see index.h for description */
bool index_save(const char* file, index_t* index){
    FILE *fp;
     // defensive check: provided file and index exist
     if (file != NULL) {
        if (index != NULL) {

            fp = fopen(file, "w");
            // write the index file: iterate over the hashtable, call helper functions
            hashtable_iterate(index->indexTable, fp, index_print_helper);
            // hashtable_print(index->indexTable, fp, counters_print_helper);
            fclose(fp);
            return true; 
        }
        else{
            fprintf(stderr, "provided index was null :(\n");
            return false;
        }
     }
     else {
        fprintf(stderr, "provided file was null :(\n");
        return false;
     }
}

/**************** index_print_helper ****************/
/* see index.h for description */
static void index_print_helper(void* file, const char* word, void* item){
    
    FILE *fp = file;
    counters_t* counters = item;
    fprintf(fp, "%s", word);
    // fprintf(fp, " ");
    counters_iterate(counters, fp, counters_print_helper);
    fprintf(fp, "\n");
}

/**************** counters_print_helper ****************/
/* see index.h for description */
static void counters_print_helper(void* file, const int key, int item){
    FILE *fp = file;
    fprintf(fp, " %d %d", key, item);
    // fprintf(fp, " ");
    // fprintf(fp, "%d", item);
    // fprintf(fp, " ");
}

/*************** index_delete ***************/
/* see index.h for description */
void index_delete(index_t* index){
    hashtable_delete(index->indexTable, index_delete_helper);
    free(index);
}

/*************** index_delete_helper ***************/
/* see index.h for description */
static void index_delete_helper(void* item){
    counters_t* counters = item;
    counters_delete(counters);
}

/*************** index_insert ***************/
/* see index.h for description */
bool index_insert(index_t* index, const char* key, void* item){
    return hashtable_insert(index->indexTable, key, item);
}

/*************** index_load ***************/
/* see index.h for description */
index_t* index_load(char* indexFilename, char* outFilename) {
    // Declare variables and pointers
    int docID;
    int num;
    char* currWord;
    index_t* ind;
    FILE* inFile;

    // Open the index file for reading
    inFile = fopen(indexFilename, "r");
    if (inFile == NULL) {
        // Error handling: failed to open the index file
        fprintf(stderr, "Failed to open the index file");
        exit(1);
    }

    // Create a new index with appropriate number of slots
    ind = index_new(200);

    // Check if the index was created properly
    if (ind == NULL) {
        fprintf(stderr, "Failed to create the index");
        exit(1);
    }
   
    // Read the first word from the file
    // Iterate through the file, word by word
    while ((currWord = file_readWord(inFile)) != NULL) {
        if (strlen(currWord) >= 3) {
            // for all the doc id pairs in the line of the word
            while(fscanf(inFile, " %d %d ", &docID, &num) == 2) {
                if (index_find(ind, currWord) == NULL) {
                    // make a new counter, add the doc ID to it, and insert it into the index
                    counters_t* counter = counters_new();

                        if (counter == NULL) {
                            fprintf(stderr, "Failed to create the counter");
                            exit(1);
                        }

                    counters_add(counter, docID);
                    index_insert(ind, currWord, counter);
                }
                else {
                     // set the value of the counter
                    counters_t* foundCounter = index_find(ind, currWord);

                    if (foundCounter == NULL) {
                        fprintf(stderr, "Failed to find and store the counter");
                        exit(1);
                    }

                    counters_set(foundCounter, docID, num);
                    index_insert(ind, currWord, foundCounter);
                }
            }
        }
        // free currWord, will be read again in next iteration of while loop
        free(currWord);
    }
    // Save the index structure to the specified output file
    index_save(outFilename, ind);
    // Close the index file
    fclose(inFile);
    // Return the index structure (ind)
    return ind;
}