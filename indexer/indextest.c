/*
 * indextest.c     Sanjana Raj     April, 2023
 *
 * unit test for the index module
 *
 * refer to the requirement, design, and implementation specs for more information
 *     
 */

#include "../common/index.h"
#include "../libcs50/file.h"

int main(const int argc, char* argv[]){
  if (argc != 3){
		fprintf(stderr, "incorrect number of arguments, usage: ./indextest oldIndexFile, newIndexFile" );
		exit(1);
	}
	char* oldIndexFile = argv[1];
	char* newIndexFile = argv[2];

	// reads input file, loads information into index object, writes them out to file
	index_t* newIndex = index_load(oldIndexFile, newIndexFile);
	index_save(newIndexFile, newIndex);
	index_delete(newIndex);
	exit(0);
}


