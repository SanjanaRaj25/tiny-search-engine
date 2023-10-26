/* 
 * pagedir.h - header file for pagedir.c file 
 *
 * pagedir includes methods to write a page to a pageDirectory and mark it as crawler produced
 *
 * Sanjana Raj, April 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../libcs50/mem.h"
#include "../libcs50/webpage.h"
#include "../libcs50/file.h"

/**************** functions ****************/

/**************** pagedir_init ****************/
/* opens a file in the provided pageDirectory to write into
* input: character array representing the directory folder name
* output: boolean value true if creating the file worked, false and error to stderr if creating the file failed
*/
bool pagedir_init(const char* pageDirectory);

/**************** pagedir_save ****************/
/* opens a file and prints out the provided page's URL, depth, and contents
* input: webpage object representing page, 
* output: nothing, unless there's an error in which case an error is printed/it exits non-zero 
*/
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/**************** pagedir_validate ****************/
/* verify whether dir is a crawler produced directory 
* input: 'pageDirectory', which is the directory that should be checked for Crawler-produced output
* output: true if the directory is crawler produced, false otherwise
*/
bool pagedir_validate(const char* pageDirectory);

/**************** pagedir_load ****************/
/* loads a webpage object from a file in the provided directory
* input: ptr to character array representing the file that needs to be pulled from
* output: webpage object
*/
extern webpage_t* pagedir_load(char* fileName);