/*
 * pagedir.c     Sanjana Raj     April, 2023
 *
 * a re-usable module that writes a page to the pageDirectoryy and marks it as Crawler-produced
 *
 * refer to the requirement, design, and implementation specs for more information
 *     
 * function descriptions were taken from the implementation spec
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "pagedir.h"
#include "../libcs50/mem.h"
#include "../libcs50/webpage.h"
#include "../libcs50/file.h"

/**************** pagedir_init ****************/
/* refer to pagedir.h for description */
bool pagedir_init(const char* pageDirectory){

    // construct the pathname for the .crawler file in that directory
    char* pathName = mem_malloc(strlen(pageDirectory) + 10);
    sprintf(pathName, "%s/.crawler", pageDirectory);
    // open the file for writing; on error, return false.
    FILE* openFile = fopen(pathName, "w");
    if (openFile == NULL){
        mem_free(pathName);
        return false;
    }
     // close the file and return true.
    else {
        fclose(openFile);
        mem_free(pathName);
        return true;
    }
}

/**************** pagedir_validate(dir) ****************/
/* refer to pagedir.h for description */
bool pagedir_validate(const char* pageDirectory){
    // construct the pathname for the page file in pageDirectory
    char* pathName = mem_malloc(strlen(pageDirectory) + 15);
    sprintf(pathName, "%s/.crawler", pageDirectory);
    // validate by seeing if you can open the file using the pathname with .crawler appended
    FILE* dirFile = fopen(pathName, "r");
    if (dirFile == NULL){
        // dir was invalidated
        mem_free(pathName);
        return false;
    }
    else {
        // dir was validated
        mem_free(pathName);
        fclose(dirFile);
        return true;
    }
}

/**************** pagedir_load ****************/
/* load a page from a file in that directory */

webpage_t* pagedir_load(char* fileName){
    // make a webpage: open file, get URL, depth, HTML
    webpage_t* temp;
    char* URL = URL;
    char* depth = 0;
    char* html;
   
    // open file
    FILE* file = fopen(fileName, "r");

    // check that you can open file
    if (file != NULL) {

         // check at least 3 lines
         if (file_numLines(file)>=3) {
            // get URL
                // if memory leaks, make a copy of strings
            URL = file_readLine(file);
            // get depth
            depth = file_readLine(file);
            html = file_readFile(file);

            // make webpage, with NULL html at this point
            temp = webpage_new(URL, atoi(depth), html);
            mem_free(depth);
            // defensive check: could make webpage
            if (temp == NULL) {
                fprintf(stderr, "there was an issue loading the webpage\n");
                mem_free(temp);
            }
         }
         else {
            fprintf(stderr, "file does not contain a line for URL, depth, and HTML \n");
         }
    }  
    // close file   
    fclose(file);
    return temp; 
}

/**************** pagedir_save ****************/
/* refer to pagedir.h for description */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID){

    // construct the pathname for the page file in pageDirectory
    char* pathName = mem_malloc(strlen(pageDirectory) + 10);
    sprintf(pathName, "%s/%d", pageDirectory, docID);

    // open that file for writing
    FILE* openFile = fopen(pathName, "w");
    if (openFile == NULL){
        fprintf(stderr, "there was an issue opening the file for writing \n");
        mem_free(pathName);
    }

    else {
        // webpage_fetch(*page);
        // print the URL
        fprintf(openFile, "%s\n", webpage_getURL(page));

        // print the depth
        fprintf(openFile, "%d\n", webpage_getDepth(page));

        // print the contents of the webpage
        fprintf(openFile, "%s\n", webpage_getHTML(page));

        // close the file
        fclose(openFile);
        mem_free(pathName);
    }
}

