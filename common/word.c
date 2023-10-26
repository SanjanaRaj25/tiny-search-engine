/*
 * word.c     Sanjana Raj     April, 2023
 *
 * a re-usable module that "normalizes" a word
 *
 * refer to the requirement, design, and implementation specs for more information
 *     
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

 /**************** functions ****************/
 char* normalizeWord(char* word){
    // defensive check: make sure word isn't null
    if (word != NULL) {
        // convert word to lowercase by looping through each letter andd calling tolower() on it
        for (char *letter = word; *letter != '\0'; letter++){
            *letter = tolower(*letter);
        }
    }
    else {
        fprintf(stderr, "word was null, couldn't be normalized \n");
        exit(1);
    }
    return word;
 }