#include "defs.h"

thrd_t mainSearchThread;

thrd_t launchSearchThread(boardStructure* position, searchInfoStructure* info, hashTableStructure* table) {
	
	searchThreadDataStructure* pSearchData = new(searchThreadDataStructure);

	pSearchData->originalPosition = position;
	pSearchData->info = info;
	pSearchData->transpositionTable = table;


	thrd_t th;
	thrd_create(&th, &searchPositionThread, (void*)pSearchData);

	return th;

}


int searchPositionThread(void* data) {

	searchThreadDataStructure* searchData = (searchThreadDataStructure*)data;
	//boardStructure* position = malloc(sizeof(boardStructure));
	boardStructure* position = new(boardStructure);
	memcpy(position, searchData->originalPosition, sizeof(boardStructure));

	searchPosition(position, searchData->info, searchData->transpositionTable);

	delete(position);
	//std::cout << "freed\n";

	return 0;

}