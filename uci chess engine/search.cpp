#include "defs.h"
#include "tinycthread.h"
#include "iostream"
#include <cstdlib>

bool isRepetition(const boardStructure* position) {

	int i;

	for (i = position->historyPly - position->fiftyMove; i < position->historyPly - 1; i++) {
#ifdef DEBUG
		if (i < 0 || i > MAX_GAME_MOVES) {
			std::cout << "error in the repitition func, index is less than 0 or greater than max game moves\n";
		}
#endif
		if (position->positionKey == position->history[i].positionKey) {
			return true;
		}
	}

	return false;

}

static void clearForSearch(boardStructure* position, searchInfoStructure* info, hashTableStructure* table) {

	int i = 0;
	int i2 = 0;

	for (i = 0; i < 13; i++) {
		for (i2 = 0; i2 < BRD_SQ_NUM; i2++) {
			position->searchHistory[i][i2] = 0;
		}
	}

	for (i = 0; i < 2; i++) {
		for (i2 = 0; i2 < MAXDEPTH; i2++) {
			position->searchKillers[i][i2] = 0;
		}
	}

	table->overWrite = 0;
	table->hit = 0;
	table->cut = 0;
	position->ply = 0;
	table->currentAge++;

	info->nullCut = 0;
	info->stopped = false;
	info->nodes = 0;
	info->fh = 0;
	info->fhf = 0;

}

void searchPosition(boardStructure* position, searchInfoStructure* info, hashTableStructure* table) {

	int rootDepth;
	int bestMove = NOMOVE;
	int bestScore = -INFINITEC;
	int currentDepth = 0;
	int pvMoves = 0;
	int pvNumber = 0;

	clearForSearch(position, info, table);

	/*
	if (engineOptions->useBook == true) {
		bestMove = getBookMove(position);
	}
	*/


	//printf("search depth:%d\n",info->depth);

	if (bestMove == NOMOVE) {
		for (currentDepth = 1; currentDepth <= info->depth; currentDepth++) {
			rootDepth = currentDepth;
			//                     alpha      beta
			bestScore = alphaBeta(-INFINITEC, INFINITEC, currentDepth, position, info, table, true);

			if (info->stopped == true) {
				break;
			}

			pvMoves = getPVLine(currentDepth, position, table);
			bestMove = position->PVArray[0];

			int nodesPerSecond = 0;
			if ((getTimeMs() - info->startTime) != 0) {
				nodesPerSecond = info->nodes / ((getTimeMs() - info->startTime) + 1) * 1000;
			}
			bool mate = false;
			bool myMate = false;
			if (bestScore > (INFINITEC - MAXDEPTH)) {
				mate = true;
				myMate = true;
			}
			else if (bestScore < (-INFINITEC + MAXDEPTH)) {
				mate = true;
			}
			//printf("info depth %d score cp %d nodes %ld nps %d time %d ", currentDepth, bestScore, info->nodes, nodesPerSecond, getTimeMs() - info->startTime);
			std::cout << "info";
			std::cout << " depth " << currentDepth;
			if (!mate)
				std::cout << " score cp " << bestScore;
			else if (myMate)
				std::cout << " score mate " << INFINITEC - bestScore;
			else
				std::cout << " score mate -" << INFINITEC + bestScore;
			std::cout << " nodes " << info->nodes;
			std::cout << " nps " << nodesPerSecond;
			std::cout << " time " << getTimeMs() - info->startTime;
			std::cout << " pv";
			for (pvNumber = 0; pvNumber < pvMoves; pvNumber++) {
				std::cout << " " << printMove(position->PVArray[pvNumber]);
			}
			std::cout << std::endl;

			printf("Hits:%d Overwrite:%d NewWrite:%d Cut:%d Ordering %.2f NullCut:%d\n", table->hit, table->overWrite, table->newWrite, table->cut, (info->fhf / info->fh) * 100, info->nullCut);
		}
	}

	if (bestMove != NOMOVE) {
		std::string bestMoveStr = printMove(bestMove);
		std::cout << "bestmove " << bestMoveStr << std::endl;

	}

}

thrd_t mainSearchThread;

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

thrd_t launchSearchThread(boardStructure* position, searchInfoStructure* info, hashTableStructure* table) {

	searchThreadDataStructure* pSearchData = new(searchThreadDataStructure);

	pSearchData->originalPosition = position;
	pSearchData->info = info;
	pSearchData->transpositionTable = table;


	thrd_t th;
	thrd_create(&th, &searchPositionThread, (void*)pSearchData);

	return th;

}


void joinSearchThread(searchInfoStructure* info) {
	
	info->stopped = true;
	thrd_join(mainSearchThread, NULL);

}

void parseGo(std::string line3, searchInfoStructure* info, boardStructure* position, hashTableStructure* table) {

	int depth = -1, movestogo = 30, movetime = -1;
	int time = -1, inc = 0;
	const char* ptr = NULL;
	info->timeSet = false;

	const char* line2 = line3.c_str();
	char line[sizeof(line3)];
	for (int i = 0; i < sizeof(line3); i++) {
		line[i] = line2[i];
	}


	if ((ptr = strstr(line, "infinite"))) {
		;
	}

	if ((ptr = strstr(line, "binc")) && position->side == black) {
		inc = atoi(ptr + 5);
	}

	if ((ptr = strstr(line, "winc")) && position->side == white) {
		inc = atoi(ptr + 5);
	}

	if ((ptr = strstr(line, "wtime")) && position->side == white) {
		time = atoi(ptr + 6);
	}

	if ((ptr = strstr(line, "btime")) && position->side == black) {
		time = atoi(ptr + 6);
	}

	if ((ptr = strstr(line, "movestogo"))) {
		movestogo = atoi(ptr + 10);
	}

	if ((ptr = strstr(line, "movetime"))) {
		movetime = atoi(ptr + 9);
	}

	if ((ptr = strstr(line, "depth"))) {
		depth = atoi(ptr + 6);
	}

	if (movetime != -1) {
		time = movetime;
		movestogo = 1;
	}

	info->startTime = getTimeMs();
	info->depth = depth;

	if (time != -1) {
		info->timeSet = true;
		time /= movestogo;
		time -= 50;
		info->stopTime = info->startTime + time + inc;
	}

	if (depth == -1) {
		info->depth = MAXDEPTH;
	}

	printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
		time, info->startTime, info->stopTime, info->depth, info->timeSet);

	//searchPosition(position, info, table);
	mainSearchThread = launchSearchThread(position, info, table);

}