/* 
 * index.h - header file for index.c file 
 *
 * a module that implements the abstract index_t type 
 * an index is a wrapper for a hashtable, with counters as items
 *
 * Sanjana Raj, April 2023
 *
 */

#ifndef __INDEX_H
#define __INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"
#include "word.h"

/**************** global types  ****************/

typedef struct index index_t;

/**************** functions ****************/

void* index_get(index_t* index);

/*************** index_new***************/
/* allows a user to create a new index object 
* input: the size of the index they want to create
* output: pointer to newly created index object
*/
index_t* index_new(int numSlots);


/*************** index_find ***************/
/* allows a user to "look up" a word in the index
* input: the index they are searching and the word they are searching for
* output: pointer to counter object associated with the word, NULL if the word is not in the index
*/
void* index_find(index_t* index, const char* word);

/**************** index_save ****************/
/* index_save() writes the index's information to a file that the user passes in
*
* input: pointer to file path name, pointer to index
* output: none
*
* exit: non-zero if the passed in file or index is null
*/
bool index_save(const char* file, index_t* index);

/*************** index_delete ***************/
/* deletes existing index
*
* input: index and itemdelete function
* output: none
*/
void index_delete(index_t* index);


/*************** index_insert ***************/
/* insert word into index structure
*
* input: index, word, counter
* output: false on failure, true on success
*/
bool index_insert(index_t* index, const char* key, void* item);

/*************** index_load ***************/
/* loads an index from a file into an index structure, then save to another file
*
* input: index, word, counter
* output: false on failure, true on success
*/
index_t* index_load(char* indexFilename, char* outFilename);

#endif // __INDEX_H