#include "defs.h"

static void checkUp(searchInfoStructure* info) {

	// .. check if time up, or interrupt from GUI
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

	if ((info->nodes & 2047) == 0) {
		checkUp(info);
	}

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
	score = -INFINITEC;

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
				if (legal == 1) {
					info->fhf++;
				}
				info->fh++;
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



	if (depth <= 0) {
		return quiescence(alpha, beta, position, info);
		// return EvalPosition(position);
	}


	if ((info->nodes & 2047) == 0) {
		checkUp(info);
	}


	info->nodes++;

	if ((isRepetition(position) || position->fiftyMove >= 100) && position->ply) {
		return DRAW;
	}

	if (position->ply > MAXDEPTH - 1) {
		return evaluatePosition(position);
	}

	bool inCheck = squareAttacked(position->kingSquare[position->side], position->side ^ 1, position);
	bool enemyInCheck = squareAttacked(position->kingSquare[position->side ^ 1], position->side, position);

	if (inCheck) {
		depth++;
	}
	if (enemyInCheck) {
		depth++;
	}

	int score = -INFINITEC;
	int pvMove = NOMOVE;

	if (probeHashEntry(position, table, &pvMove, &score, alpha, beta, depth)) {
		table->cut++;
		return score;
	}

	if (doNull && !inCheck && position->ply && (position->normalPieces[position->side] > 1) && depth >= 4) {
		makeNullMove(position);
		score = -alphaBeta(-beta, -beta + 1, depth - 4, position, info, table, false);
		takeNullMove(position);
		if (info->stopped == true) {
			return 0;
		}

		if (score >= beta && abs(score) < ISMATE) {
			info->nullCut++;
			return beta;
		}
	}

	moveListStructure list[1];
	generateAllMoves(position, list);

	int moveNumber = 0;
	int legal = 0;
	int oldAlpha = alpha;
	int bestMove = NOMOVE;

	int bestScore = -INFINITEC;

	score = -INFINITEC;

	if (pvMove != NOMOVE) {
		for (moveNumber = 0; moveNumber < list->moveCount; moveNumber++) {
			if (list->moves[moveNumber].move == pvMove) {
				list->moves[moveNumber].score += 2000000;
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
					if (legal == 1) {
						info->fhf++;
					}
					info->fh++;

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
			return -INFINITEC + position->ply;
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