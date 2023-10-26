/*
 * crawler.c     Sanjana Raj     April, 2023
 *
 * The TSE crawler is a standalone program that crawls the web and 
 retrieves webpages starting from a "seed" URL. It parses the seed 
 webpage, extracts any embedded URLs, then retrieves each of those pages,
  recursively, but limiting its exploration to a given "depth".
 *
 * refer to the requirement, design, and implementation specs for more information
 *     
 * function descriptions were taken from the implementation spec
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "../libcs50/bag.h"
#include "../libcs50/hash.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/webpage.h"
#include "../libcs50/mem.h"
#include "../common/pagedir.h"

/**************** function prototypes ****************/
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);


/**************** main ****************/
/* The main function simply calls parseArgs and crawl, then exits zero. */ 
int main(const int argc, char* argv[])
{
    char* seedURL = NULL;
	char*  pageDirectory = NULL; 
	int maxDepth = 0;
    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
    crawl(seedURL, pageDirectory, maxDepth);
    mem_free(seedURL);
    mem_free(pageDirectory);
    exit(0);
}

/**************** parseArgs ****************/
/* given arguments from the command line, extract them into the function parameters; return only if successful.*/
// argc = # of arguments, arg[1] = seedURL, arg[2] = directory to write out to, arg[3] = maxDepth
static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth)
{

    // make sure there are exactly 3 arguments
    if (argc != 4){
        fprintf(stderr, "Incorrect number of arguments. Call using the format: crawler seedURL pageDirectory maxDepth\n");
		exit(1);
	}

    // allocate space to store parameters
    *seedURL = mem_malloc((strlen(argv[1]) + 1) * sizeof(char));
    strcpy(*seedURL, argv[1]);

    *pageDirectory = mem_malloc((strlen(argv[2]) + 1) * sizeof(char));
    strcpy(*pageDirectory, argv[2]);

    *maxDepth = atoi(argv[3]);

    // char** extractedSeedURL = mem_malloc((strlen(argv[1]) + 1) * sizeof(char));
    // // issues with liine below
    // strcpy(**extractedSeedURL, argv[1]);

	// char** extractedPageDirectory = mem_malloc((strlen(argv[2]) + 1) * sizeof(char));
    // strcpy(**pageDirectory, argv[2]);

	// *maxDepth = atoi(argv[3]);

    // for seedURL, normalize the URL and validate that it is an internal URL
    char* normURL = normalizeURL(*seedURL);
    if (!isInternalURL(normURL)){
		fprintf(stderr, "seedURL is not a valid internal URL.\n");
		exit(1);
	}
    free(normURL);

    // construct the file for pageDirectory, make sure it can be opened and closed
    
    if (!pagedir_init(*pageDirectory)){
        fprintf(stderr, "pagedir_init failed, couldn't create valid .crawler file\n");
        exit(2);
    }

    // make sure that maxDepth is an int, in the specified range

    if(*maxDepth < 0 || *maxDepth > 10){
        fprintf(stderr, " the provided maxDepth is not an integer in range [0..10] \n");
        exit(3);
    }
    // if the program reaches this point then, all the arguments must be valid and stored in the function parameters
}


/**************** crawl ****************/
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth) 
{
     // initialize the hashtable of URLs we've seen so far
    int numSlots = 200;

    hashtable_t* seenURLS = hashtable_new(numSlots);

    // add the seedURL to the hashtable
    if (seenURLS == NULL){
        fprintf(stderr, "there was an error creating the hashtable\n"); 
        exit(1);
    }

    // normalize the seedURL and add it to hashtable
    char* normURL = normalizeURL(seedURL);
    char* hashURL = mem_malloc(strlen(seedURL)+10);
    strcpy(hashURL, normURL);
    hashtable_insert(seenURLS, hashURL, "");
    
    // initialize the bag of pages we have yet to explore
    bag_t* pagesToExplore = bag_new();
    if (pagesToExplore == NULL){
        fprintf(stderr, "there was an error creating the bag\n"); 
        exit(1);
    }

    // add a webpage representing the seedURL at depth 0
    char* bagURL = mem_malloc(strlen(seedURL)+10);
    strcpy(bagURL, normURL);
    free(normURL);
    
    // webpage_new has 3 parameters: url, depth, and html
    webpage_t* page = webpage_new(bagURL, 0, NULL);
    //make sure objects were created, then insert
    if (page == NULL){
        fprintf(stderr, "there was an error creating the webpage\n"); 
        exit(4);
    }
     bag_insert(pagesToExplore, page);


    // create a variable to store a unique document ID for each fie
    int documentID = 1;
    webpage_t* currPage;

    // while bag is not empty
    while ((currPage = (webpage_t*) bag_extract(pagesToExplore)) != NULL) {
        // fetch the HTML for that webpage -- will return true if successful, false if not
        // if fetch was successful,
        if (webpage_fetch(currPage)){
            //print the detched URL
            printf("%d Fetched: %s\n", webpage_getDepth(currPage), webpage_getURL(currPage));
            // save the webpage to pageDirectory
            pagedir_save(currPage, pageDirectory, documentID);
            // if the webpage is not at maxDepth,
            if (webpage_getDepth(currPage) < maxDepth) {
                // pageScan that HTML
                printf("%d Scanning: %s\n", webpage_getDepth(currPage), webpage_getURL(currPage));
                pageScan(currPage, pagesToExplore, seenURLS);
            }
        }
                 
        // delete that webpage
        webpage_delete(currPage);

        // increment the ID
        documentID ++;
    }
    // delete the hashtable
    hashtable_delete(seenURLS, NULL);
    
    // delete the bag
	bag_delete(pagesToExplore, NULL);

    // free hashURL
    free(hashURL);
    }



/**************** pageScan ****************/
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen)
{
    // check if any parameter is null
    if (pagesToCrawl == NULL){
        fprintf(stderr, "provided bag was invalid\n");  
        return;
    }

    if (pagesSeen == NULL){
        fprintf(stderr, "provided hashtable was invalid.\n");  
        return;
    }

    // store position, nextURL, and current depth
    int pos = 0;
    char* nextURL;

    // while there is another URL in the page
    while ((nextURL = webpage_getNextURL(page, &pos)) != NULL) {
    
        // normalize the URL, and store it in a char pointer
        char* normURL = normalizeURL(nextURL);
        free(nextURL);

        // print the normalized URL
        printf("%d Found URL: %s\n", webpage_getDepth(page), normURL);

        // if that URL is Internal,
        if (isInternalURL(normURL)){

            // 	insert the webpage into the hashtable, if that succeeded
            if (hashtable_insert(pagesSeen, normURL, "")){

                // create a webpage_t for it / insert the webpage into the bag
                char* pageURL = mem_malloc((strlen(normURL) + 1) * sizeof(char));
                strcpy(pageURL, normURL);

                webpage_t* newPage = webpage_new(pageURL, webpage_getDepth(page) + 1, NULL);

                if (newPage != NULL){
					bag_insert(pagesToCrawl, newPage);
					printf("%d Added: %s\n", webpage_getDepth(page), pageURL);
				}
                
            }
            // hashtable_insert returned false, meaning the key passed in already existed
            else {
				printf("%d IgnDupl: %s\n", webpage_getDepth(page), normURL);
			}		
        }
        // the URL is not intrenal
        else {
            printf("%d IgnExtrn: %s\n", webpage_getDepth(page), normURL);
        }
        mem_free(normURL);
    }
}