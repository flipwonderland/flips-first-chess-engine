#include "defs.h"

void clearHashTable(hashTableStructure* table) {

	hashEntryStructure* tableEntry;

	for (tableEntry = table->pTable; tableEntry < table->pTable + table->numberOfEntries; tableEntry++) {
		tableEntry->positionKey = 0ULL;
		tableEntry->move = NOMOVE;
		tableEntry->depth = 0;
		tableEntry->score = 0;
		tableEntry->flags = 0;
		tableEntry->age = 0;
	}
	table->newWrite = 0;
	table->currentAge = 0;
}


int probePVMove(const boardStructure* position, const hashTableStructure* table) {

	int index = position->positionKey % table->numberOfEntries;
	//ASSERT(index >= 0 && index <= pos->HashTable->numEntries - 1);

	if (table->pTable[index].positionKey == position->positionKey) {
		return table->pTable[index].move;
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

	if (table->pTable[index].positionKey == position->positionKey) {
		*move = table->pTable[index].move;
		if (table->pTable[index].depth >= depth) {
			table->hit++;
			/*
			ASSERT(table->pTable[index].depth >= 1 && table->pTable[index].depth < MAXDEPTH);
			ASSERT(table->pTable[index].flags >= HFALPHA && table->pTable[index].flags <= HFEXACT);
			*/

			*score = table->pTable[index].score;
			if (*score > ISMATE) *score -= position->ply;
			else if (*score < -ISMATE) *score += position->ply;

			switch (table->pTable[index].flags) {

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

void storeHashEntry(boardStructure* position, hashTableStructure* table, const int move, int score, const int flags, const int depth) {

	int index = position->positionKey % table->numberOfEntries;

	/*
	ASSERT(index >= 0 && index <= table->numEntries - 1);
	ASSERT(depth >= 1 && depth < MAXDEPTH);
	ASSERT(flags >= HFALPHA && flags <= HFEXACT);
	ASSERT(score >= -INFINITE && score <= INFINITE);
	ASSERT(position->ply >= 0 && position->ply < MAXDEPTH);
	*/

	bool replace = false;

	if (table->pTable[index].positionKey == 0) {
		table->newWrite++;
		replace = true;
	}
	else {
		if (table->pTable[index].age < table->currentAge || table->pTable[index].depth <= depth) {
			replace = true;
			table->overWrite++;
		}

	}

	if (!replace)
		return;

	if (score > ISMATE) score += position->ply;
	else if (score < -ISMATE) score -= position->ply;

	table->pTable[index].move = move;
	table->pTable[index].positionKey = position->positionKey;
	table->pTable[index].flags = flags;
	table->pTable[index].score = score;
	table->pTable[index].depth = depth;
	table->pTable[index].age = table->currentAge;
}