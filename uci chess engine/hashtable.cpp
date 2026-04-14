#include "defs.h"
#include "util.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>


#define FOLD_DATA(sc, de, fl, mv) ( (sc + INF_BOUND) | (de << 16) | (fl << 23) | ((u64)mv << 25))

#define EXTRACT_SCORE(x) ((x & 0xFFFF) - INF_BOUND)
#define EXTRACT_DEPTH(x) ((x >> 16) & 0x3F)
#define EXTRACT_FLAGS(x) ((x >> 23) & 0x3)
#define EXTRACT_MOVE(x) ((int)(x >> 25))

/*
void dataCheck(const int move) {
	int depth = rand() % MAXDEPTH;
	int flag = rand() % 3;
	int score = rand() % AB_BOUND;

	u64 data = FOLD_DATA(score, depth, flag, move);
	
	printf("orig: move:%s d:%d fl:%d sc:%d data:%llx\n", printMove(move), depth, flag, score, data);
	printf("check: move:%s d:%d fl:%d sc:%d\n\n",
	printMove(EXTRACT_MOVE(data)),
	EXTRACT_DEPTH(data),
	EXTRACT_FLAGS(data),
	EXTRACT_SCORE(data));

}

void tempHashTest(const char* fen) {
	boardStructure b[1];
	parseFen(fen, b);

	moveListStructure list[1];
	generateAllMoves(b, list);

	int moveNumber = 0;
	for (moveNumber = 0; moveNumber < list->moveCount; moveNumber++) {

		if (!makeMove(b, list->moves[moveNumber].move)) {
			continue;
		}
		takeMove(b);
		dataCheck(list->moves[moveNumber].move);
	}
}
*/


void clearHashTable(hashTableStructure* table) {

	hashEntryStructure* tableEntry;

	for (tableEntry = table->pTable; tableEntry < table->pTable + table->numberOfEntries; tableEntry++) {
		
		/*
		tableEntry->positionKey = 0ULL;
		tableEntry->move = NOMOVE;
		tableEntry->depth = 0;
		tableEntry->score = 0;
		tableEntry->flags = 0;
		*/

		tableEntry->age = 0;

		tableEntry->smp_key = 0ULL;
		tableEntry->smp_data = 0ULL;
	}
	table->newWrite = 0;
	table->currentAge = 0;
}



void verifyEntrySMP(hashEntryStructure* entry) {
	
	/*
	u64 data = FOLD_DATA(entry->score, entry->depth, entry->flags, entry->move);
	u64 key = entry->positionKey ^ data;

	if (data != entry->smp_data) { printf("data error"); exit(1); }
	if (key != entry->smp_key) { printf("smp_key error"); exit(1); }

	int move = EXTRACT_MOVE(data);
	int flag = EXTRACT_FLAGS(data);
	int score = EXTRACT_SCORE(data);
	int depth = EXTRACT_DEPTH(data);

	if (move != entry->move) { printf("move error\n");  }
	if (flag != entry->flags) { printf("flags error\n");  }
	if (score != entry->score) { printf("score error\n");  }
	if (depth != entry->depth) { printf("depth error\n"); }
	*/
}

int probePVMove(const boardStructure* position, const hashTableStructure* table) {

	int index = position->positionKey % table->numberOfEntries;
	
	u64 test_key = position->positionKey ^ table->pTable[index].smp_data;

	if (table->pTable[index].smp_key == test_key) {
		return EXTRACT_MOVE(table->pTable[index].smp_data);
	}

	return NOMOVE;
}

int getPVLine(const int depth, boardStructure* position, const hashTableStructure* table) {

#ifdef DEBUG
	if (!(depth < MAXDEPTH) || !(depth >= 1)) {
		std::cout << "invalid depth given to pv line\n";
	}
#endif
	int move = probePVMove(position, table);
	int count = 0;

	while (move != NOMOVE && count < depth) {

#ifdef DEBUG
		if (!(count < MAXDEPTH)) {
			std::cout << "count outside pv array\n";
		}
#endif
		if (moveExists(position, move)) {
			makeMove(position, move);
			position->PVArray[count] = move;
			count++;
		}
		else {
			break;
		}
		move = probePVMove(position, table);
	}

	while (position->ply > 0) {
		takeMove(position);
	}

	return count;

}

 bool probeHashEntry(boardStructure* position, hashTableStructure* table, int* move, int* score, int alpha, int beta, int depth) {

	int index = position->positionKey % table->numberOfEntries;

	/*
		ASSERT(index >= 0 && index <= table->numEntries - 1);
		ASSERT(depth >= 1 && depth < MAXDEPTH);
		ASSERT(alpha < beta);
		ASSERT(alpha >= -INFINITE && alpha <= INFINITE);
		ASSERT(beta >= -INFINITE && beta <= INFINITE);
		ASSERT(position->ply >= 0 && position->ply < MAXDEPTH);
	*/

	u64 test_key = position->positionKey ^ table->pTable[index].smp_data;
	if (table->pTable[index].smp_key == test_key) {

		int smp_move = EXTRACT_MOVE(table->pTable[index].smp_data);
		int smp_depth = EXTRACT_DEPTH(table->pTable[index].smp_data);
		int smp_score = EXTRACT_SCORE(table->pTable[index].smp_data);
		int smp_flags = EXTRACT_FLAGS(table->pTable[index].smp_data);

		*move = smp_move;

		if (smp_depth >= depth) {
			table->hit++;
			
			*score = smp_score;
			if (*score > ISMATE) *score -= position->ply;
			else if (*score < -ISMATE) *score += position->ply;

			switch (smp_flags) {

				//ASSERT(*score >= -INFINITE && *score <= INFINITE);

			case HFALPHA: if (*score <= alpha) {
				*score = alpha;
				return true;
			}
						break;
			case HFBETA: if (*score >= beta) {
				*score = beta;
				return true;
			}
					   break;
			case HFEXACT:
				return true;
				break;
			default:
				//ASSERT(false);
				break;
			}
		}
	}

	return false;
}

void storeHashEntry(boardStructure* position, hashTableStructure* table, int move, int score, int flags, int depth) {

	int index = position->positionKey % table->numberOfEntries;

	/*
	ASSERT(index >= 0 && index <= table->numEntries - 1);
	ASSERT(depth >= 1 && depth < MAXDEPTH);
	ASSERT(flags >= HFALPHA && flags <= HFEXACT);
	ASSERT(score >= -INFINITE && score <= INFINITE);
	ASSERT(position->ply >= 0 && position->ply < MAXDEPTH);
	*/

	bool replace = false;

	if (table->pTable[index].smp_key == 0) {
		table->newWrite++;
		replace = true;
	}
	else {
		if (table->pTable[index].age < table->currentAge) {
			replace = true;
		} else if (EXTRACT_DEPTH(table->pTable[index].smp_data) <= depth) {
			replace = true;
		}

	}

	if (!replace)
		return;

	if (score > ISMATE) score += position->ply;
	else if (score < -ISMATE) score -= position->ply;
	
	u64 smp_data = FOLD_DATA(score, depth, flags, move);
	//u64 smp_key = position->positionKey ^ smp_data;
	

	/*
	table->pTable[index].positionKey = position->positionKey;
	table->pTable[index].move = move;
	table->pTable[index].flags = flags;
	table->pTable[index].depth = depth;
	table->pTable[index].score = score;
	*/
	table->pTable[index].age = table->currentAge;

	table->pTable[index].smp_data = smp_data;
	table->pTable[index].smp_key = position->positionKey ^ smp_data;



}