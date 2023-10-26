/* 
 * word.h - header file for word.c file 
 *
 * module that implements normalizeWord
 *
 * Sanjana Raj, April 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**************** functions ****************/
/* input: character pointer to word
* output: word converted to lowercase
*
* exit: non-zero if word is null and cannot be normalized
*/ 

char* normalizeWord(char* word);