#include "defs.h"
#include "tinycthread.h"
#include <malloc.h>
#include "iostream"
#include <cstdlib>


thrd_t workerThreads[MAXTHREADS];
int rootDepth;

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

	info->printDepth = true;
}


//change this lol this is dumb
static void internalClearForSearch(boardStructure* position, searchInfoStructure* info, hashTableStructure* table) {

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

static void checkUp(searchInfoStructure* info) {

	// check if time is up
	if (info->timeSet == true && getTimeMs() > info->stopTime) {
		info->stopped = true;
	}

}


static void pickNextMove(int moveNumber, moveListStructure* list) {

	moveStructure temp;
	int i;
	int bestScore = 0;
	int bestNumber = moveNumber;

	for (i = moveNumber; i < list->moveCount; i++) { //this could probably be optimized greatly, but that could be said for everything here probably
		if (list->moves[i].score > bestScore) {
			bestScore = list->moves[i].score;
			bestNumber = i;
		}
	}
#ifdef DEBUG
	if (moveNumber >= 0 && moveNumber < list->moveCount) {}
	else
		std::cout << "move number invalid in next move picker";
	if (bestNumber >= 0 && bestNumber < list->moveCount) {}
	else
		std::cout << "best number invalid in next move picker";
	if (bestNumber >= moveNumber) {}
	else
		std::cout << "best number >= move number (I am so lazy) in next move picker";

#endif

	temp = list->moves[moveNumber];
	list->moves[moveNumber] = list->moves[bestNumber];
	list->moves[bestNumber] = temp;
}

static int quiescence(int alpha, int beta, boardStructure* position, searchInfoStructure* info) {

	/*
	ASSERT(CheckBoard(position));
	ASSERT(beta > alpha);
	*/

	/*
	if ((info->nodes & 2047) == 0) {
		checkUp(info);
	}
	*/

	info->nodes++;

	if (isRepetition(position) || position->fiftyMove >= 100) {
		return 0;
	}

	if (position->ply > MAXDEPTH - 1) {
		return evaluatePosition(position);
	}

	int score = evaluatePosition(position);

	//ASSERT(Score > -INFINITE && Score < INFINITE);

	if (score >= beta) {
		return beta;
	}

	if (score > alpha) {
		alpha = score;
	}

	moveListStructure list[1];
	generateAllCaptures(position, list);

	int moveNumber = 0;
	int legal = 0;
	score = -AB_BOUND;

	for (moveNumber = 0; moveNumber < list->moveCount; moveNumber++) {

		pickNextMove(moveNumber, list);

		if (!makeMove(position, list->moves[moveNumber].move)) {
			continue;
		}

		legal++;
		score = -quiescence(-beta, -alpha, position, info);
		takeMove(position);

		if (info->stopped == true) {
			return 0;
		}

		if (score > alpha) {
			if (score >= beta) {
#ifdef VERBOSE_OUTPUT
				if (legal == 1) {
					info->fhf++;
				}
				info->fh++;
#endif
				return beta;
			}
			alpha = score;
		}
	}

	//ASSERT(alpha >= OldAlpha);

	return alpha;
}

static int alphaBeta(int alpha, int beta, int depth, boardStructure* position, searchInfoStructure* info, hashTableStructure* table, int doNull) {

	/*
	ASSERT(CheckBoard(position));
	ASSERT(beta > alpha);
	ASSERT(depth >= 0);
	*/

	if ((info->nodes & 2047) == 0) {
		checkUp(info);
	}

	info->nodes++;

	if (depth <= 0) {
		return quiescence(alpha, beta, position, info);
	}

	if ((isRepetition(position) || position->fiftyMove >= 100) && position->ply) {
		return DRAW;
	}

	if (position->ply > MAXDEPTH - 1) {
		return evaluatePosition(position);
	}
	
	int score = -AB_BOUND;
	int pvMove = NOMOVE;

	if (probeHashEntry(position, table, &pvMove, &score, alpha, beta, depth)) {
#ifdef VERBOSE_OUTPUT
		table->cut++;
#endif
		return score;
	}

	bool inCheck = squareAttacked(position->kingSquare[position->side], position->side ^ 1, position);

	if (inCheck) {
		depth++;
	}

	if (doNull && !inCheck && position->ply && (position->normalPieces[position->side] > 1) && depth >= 4) {
		makeNullMove(position);
		score = -alphaBeta(-beta, -beta + 1, depth - 4, position, info, table, false);
		takeNullMove(position);

		if (info->stopped == true) {
			return 0;
		}

		if (score >= beta && abs(score) < ISMATE) {
#ifdef VERBOSE_OUTPUT
			info->nullCut++;
#endif
			return beta;
		}
	}

	moveListStructure list[1];
	generateAllMoves(position, list);

	int moveNumber = 0;
	int legal = 0;
	int oldAlpha = alpha;
	int bestMove = NOMOVE;

	int bestScore = -AB_BOUND;

	score = -AB_BOUND;

	if (pvMove != NOMOVE) {
		for (moveNumber = 0; moveNumber < list->moveCount; moveNumber++) {
			if (list->moves[moveNumber].move == pvMove) {
				list->moves[moveNumber].score += 9999999; //so it checks the last best move first as it's likely the best move for the next depth as well
				break;
			}
		}
	}

	for (moveNumber = 0; moveNumber < list->moveCount; moveNumber++) {

		pickNextMove(moveNumber, list);

		if (!makeMove(position, list->moves[moveNumber].move)) {
			continue;
		}

		legal++;
		score = -alphaBeta(-beta, -alpha, depth - 1, position, info, table, true);
		takeMove(position);

		if (info->stopped == true) {
			return 0;
		}

		if (score > bestScore) {
			bestScore = score;
			bestMove = list->moves[moveNumber].move;
			if (score > alpha) {
				if (score >= beta) {
#ifdef VERBOSE_OUTPUT
					if (legal == 1) {
						info->fhf++;
					}
					info->fh++;
#endif
					if (!(list->moves[moveNumber].move & MFLAGCAP)) {
						position->searchKillers[1][position->ply] = position->searchKillers[0][position->ply];
						position->searchKillers[0][position->ply] = list->moves[moveNumber].move;
					}

					storeHashEntry(position, table, bestMove, beta, HFBETA, depth);

					return beta;
				}
				alpha = score;

				if (!(list->moves[moveNumber].move & MFLAGCAP)) {
					position->searchHistory[position->pieces[FROMSQ(bestMove)]][TOSQ(bestMove)] += depth;
				}
			}
		}
	}

	if (legal == 0) {
		if (inCheck) {
			return -AB_BOUND + position->ply;
		}
		else {
			return 0;
		}
	}


	//ASSERT(alpha >= OldAlpha);

	if (alpha != oldAlpha) {
		storeHashEntry(position, table, bestMove, bestScore, HFEXACT, depth);
	}
	else {
		storeHashEntry(position, table, bestMove, alpha, HFALPHA, depth);
	}

	return alpha;
}


static void iterativeDeepen(searchWorkerDataStructure* workerData) {

	workerData->bestMove = NOMOVE;
	workerData->ponderMove = NOMOVE;
	int bestScore = -AB_BOUND;
	int currentDepth = 0;
	int pvMoves = 0;
	int pvNumber = 0;

	for (currentDepth = 1; currentDepth <= workerData->info->depth; ++currentDepth) {
		rootDepth = currentDepth;
		bestScore = alphaBeta(-AB_BOUND, AB_BOUND, currentDepth, workerData->position, workerData->info, workerData->ttable, true);

		if (workerData->info->stopped == true) {
			break;
		}

		if (workerData->threadNumber == 0) {
			pvMoves = getPVLine(currentDepth, workerData->position, workerData->ttable);
			workerData->bestMove = workerData->position->PVArray[0];
			workerData->ponderMove = workerData->position->PVArray[1];

			bool mate = false;
			bool myMate = false;
			int untilMate = 0;
			if (bestScore > (AB_BOUND - MAXDEPTH)) {
				mate = true;
				myMate = true;
			}
			else if (bestScore < (-AB_BOUND + MAXDEPTH)) {
				mate = true;
				myMate = false;

			}
				std::cout << "info";
				std::cout << " depth " << currentDepth;
				if (!mate)
					std::cout << " score cp " << bestScore;
				else if (myMate) {
				/*
					if ((AB_BOUND - bestScore) > 1) {
						untilMate = (AB_BOUND - bestScore) / 4;
					}
					else {
						untilMate = AB_BOUND - bestScore;
					}
				*/
				std::cout << " score mate " << (AB_BOUND - bestScore);
				}
				else {
					std::cout << " score mate -" << (AB_BOUND + bestScore); //for some reason the mate it gives is (m * 4) + 1 for white and (m * 4) for black, gotta figure that out later
				}
				std::cout << " nodes " << workerData->info->nodes;

#ifdef VERBOSE_OUTPUT
				int secondsPassed = (getTimeMs() - workerData->info->startTime) * 1000;
				int nodesPerSecond = workerData->info->nodes / secondsPassed;
				std::cout << " nps " << nodesPerSecond;
#endif
				std::cout << " time " << getTimeMs() - workerData->info->startTime;
				
				std::cout << " pv";
				for (pvNumber = 0; pvNumber < pvMoves; pvNumber++) {
					std::cout << " " << printMove(workerData->position->PVArray[pvNumber]);
				}
				std::cout << std::endl;
#ifdef VERBOSE_OUTPUT
			printf("Hits:%d Overwrite:%d NewWrite:%d Cut:%d Ordering %.2f NullCut:%d\n", workerData->ttable->hit, workerData->ttable->overWrite, workerData->ttable->newWrite, workerData->ttable->cut, (workerData->info->fhf / workerData->info->fh) * 100, workerData->info->nullCut);
#endif
			
		}
	}

}

int startWorkerThread(void* data) {

	searchWorkerDataStructure* workerData = (searchWorkerDataStructure*)data;
	//printf("Thread:%d Starts\n", workerData->threadNumber);
	/*
	int startTime = getTimeMs();
	*/

	iterativeDeepen(workerData);

	//printf("Thread:%d Ends depth %d\n", workerData->threadNumber, workerData->depth);
		if (workerData->threadNumber == 0) {	
			std::cout << "bestmove " << printMove(workerData->bestMove);
			std::cout << " ponder " << printMove(workerData->ponderMove) << std::endl; //the way the printmove is set up you can't change the move in the same line because it's const
		}
	delete(data);
	return 0;
}

void setupWorker(int threadNumber, thrd_t* workerThread, boardStructure* position, searchInfoStructure* info, hashTableStructure* table) {
	
	/*
	u64 searchWorkerDataSize = sizeof(searchWorkerDataStructure);
	u64 boardStructureDataSize = sizeof(boardStructure);
	*/

	searchWorkerDataStructure* pWorkerData = new(searchWorkerDataStructure);
	pWorkerData->position = new(boardStructure);
	memcpy(pWorkerData->position, position, sizeof(boardStructure));
	pWorkerData->info = info;
	pWorkerData->ttable = table;
	pWorkerData->threadNumber = threadNumber;
	thrd_create(workerThread, &startWorkerThread, (void*)pWorkerData);
}

void createSearchWorkers(boardStructure* position, searchInfoStructure* info, hashTableStructure* table) {

	//printf ("CreateSearchWorkers:%d\n", info->threadNumber);
	for (int i = 0; i < info->threadNumber; i++) {
		setupWorker(i, &workerThreads[i], position, info, table);
	}
}

static void searchPosition(boardStructure* position, searchInfoStructure* info, hashTableStructure* table) {

	
	int bestMove = NOMOVE;
	/*
	int bestScore = -AB_BOUND;
	int currentDepth = 0;
	int pvMoves = 0;
	int pvNumber = 0;
	*/

	clearForSearch(position, info, table);

	/*
	if (engineOptions->useBook == true) {
		bestMove = getBookMove(position);
	}
	*/

	//printf("search depth:%d\n",info->depth);

	if (bestMove == NOMOVE) {
		createSearchWorkers(position, info, table);
	}
	
	for (int i = 0; i < info->threadNumber; i++) {
		thrd_join(workerThreads[i], NULL);
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
	int offSetTime = 10;
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
		time -= offSetTime;
		info->stopTime = info->startTime + time + inc;
	}

	if (depth == -1) {
		info->depth = MAXDEPTH;
	}
	/*
	printf("time:%u start:%u stop:%u depth:%u timeset:%u\n",
		time, info->startTime, info->stopTime, info->depth, info->timeSet);
	*/

	//searchPosition(position, info, table);
	mainSearchThread = launchSearchThread(position, info, table);

}

static int internalBenchmarkSearch(int depth, searchInfoStructure* info, boardStructure* position, hashTableStructure* table, int hashTableMegabytes) {

	internalClearForSearch(position, info, table);
	//clear hash table so it doesn't already have the answer
	table->pTable = NULL;
	initializeHashTable(hashTable, hashTableMegabytes);

	searchWorkerDataStructure workerData[1];
	workerData->position = position;
	workerData->info = info;
	workerData->ttable = table;

	workerData->threadNumber = 1;
	workerData->depth = 0;

	workerData->info->depth = depth;
	workerData->info->printDepth = false;

	int timeStarted = getTimeMs();

	iterativeDeepen(workerData);

	int timeTaken = getTimeMs() - timeStarted;
	
	//clear before and after
	table->pTable = NULL;
	initializeHashTable(hashTable, hashTableMegabytes);

	if ((getTimeMs() - timeStarted) > 0) {
		return timeTaken;
	}
	else {
		return -9999;
	}
	
	
}

void quickBenchmark(boardStructure* position, searchInfoStructure* info, hashTableStructure* table, int hashTableMegabytes) {

	int depth = 7;
	int timesToSample = 5;
	int timeToComplete = 0;

	parseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ", position);

	for (int i = 0; i < timesToSample; i++) {
		int singleTest = 0;
		singleTest = internalBenchmarkSearch(depth, info, position, table, hashTableMegabytes);
		std::cout << "test " << i + 1 << " completed in " << singleTest << " ms\n";
		timeToComplete += singleTest;

	}
	std::cout << "the average time to get to depth " << depth << " for startpos is " << timeToComplete / timesToSample << " ms\n";

	depth = 7;
	timesToSample = 5;
	timeToComplete = 0;
	parseFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", position);
	
	for (int i = 0; i < timesToSample; i++) {
		int singleTest = 0;
		singleTest = internalBenchmarkSearch(depth, info, position, table, hashTableMegabytes);
		std::cout << "test " << i + 1 << " completed in " << singleTest << " ms\n";
		timeToComplete += singleTest;

	}
	std::cout << "the average time to get to depth " << depth << " for kiwipete is " << timeToComplete / timesToSample << " ms\n";


}