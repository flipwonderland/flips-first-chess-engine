#include "defs.h"

static bool isRepetition(const boardStructure* position) {

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

static void generateAllCaptures(const boardStructure* position, moveListStructure* list) {

	//ASSERT(CheckBoard(position));

	list->moveCount = 0;

	int piece = empty;
	int side = position->side;
	int square = 0; int tempSquare = 0;
	int pieceNumber = 0;
	int direction = 0;
	int index = 0;
	int pieceIndex = 0;

	if (side == white) {

		for (pieceNumber = 0; pieceNumber < position->pieceNumber[wP]; ++pieceNumber) {
			square = position->pieceList[wP][pieceNumber];
			//ASSERT(squareOnBoard(square));

			if (!SQOFFBOARD_MOVEGEN(square + 9) && pieceColor[position->pieces[square + 9]] == black) {
				addWhitePawnCaptureMove(position, square, square + 9, position->pieces[square + 9], list);
			}
			if (!SQOFFBOARD_MOVEGEN(square + 11) && pieceColor[position->pieces[square + 11]] == black) {
				addWhitePawnCaptureMove(position, square, square + 11, position->pieces[square + 11], list);
			}

			if (position->enPassant != noSquare) {
				if (square + 9 == position->enPassant) {
					addEnPassantMove(position, MOVE_MOVEGEN(square, square + 9, empty, empty, MFLAGEP), list);
				}
				if (square + 11 == position->enPassant) {
					addEnPassantMove(position, MOVE_MOVEGEN(square, square + 11, empty, empty, MFLAGEP), list);
				}
			}
		}

	}
	else {

		for (pieceNumber = 0; pieceNumber < position->pieceNumber[bP]; ++pieceNumber) {
			square = position->pieceList[bP][pieceNumber];
			//ASSERT(squareOnBoard(square));

			if (!SQOFFBOARD_MOVEGEN(square - 9) && pieceColor[position->pieces[square - 9]] == white) {
				addBlackPawnCaptureMove(position, square, square - 9, position->pieces[square - 9], list);
			}

			if (!SQOFFBOARD_MOVEGEN(square - 11) && pieceColor[position->pieces[square - 11]] == white) {
				addBlackPawnCaptureMove(position, square, square - 11, position->pieces[square - 11], list);
			}
			if (position->enPassant != noSquare) {
				if (square - 9 == position->enPassant) {
					addEnPassantMove(position, MOVE_MOVEGEN(square, square - 9, empty, empty, MFLAGEP), list);
				}
				if (square - 11 == position->enPassant) {
					addEnPassantMove(position, MOVE_MOVEGEN(square, square - 11, empty, empty, MFLAGEP), list);
				}
			}
		}
	}

	/* Loop for slide pieces */
	pieceIndex = loopSlidingIndex[side];
	piece = loopSlidingPiece[pieceIndex++];
	while (piece != 0) {
		//ASSERT(PieceValid(piece));

		for (pieceNumber = 0; pieceNumber < position->pieceNumber[piece]; ++pieceNumber) {
			square = position->pieceList[piece][pieceNumber];
			//ASSERT(squareOnBoard(square));

			for (index = 0; index < numberOfDirections[piece]; ++index) {
				direction = pieceDirection[piece][index];
				tempSquare = square + direction;

				while (!SQOFFBOARD_MOVEGEN(tempSquare)) {
					// BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
					if (position->pieces[tempSquare] != empty) {
						if (pieceColor[position->pieces[tempSquare]] == (side ^ 1)) {
							addCaptureMove(position, MOVE_MOVEGEN(square, tempSquare, position->pieces[tempSquare], empty, 0), list);
						}
						break;
					}
					tempSquare += direction;
				}
			}
		}

		piece = loopSlidingPiece[pieceIndex++];
	}

	/* Loop for non slide */
	pieceIndex = loopNonSlidingIndex[side];
	piece = loopNonSlidingPiece[pieceIndex++];

	while (piece != 0) {
		//ASSERT(pieceValid(piece));

		for (pieceNumber = 0; pieceNumber < position->pieceNumber[piece]; ++pieceNumber) {
			square = position->pieceList[piece][pieceNumber];
			//ASSERT(squareOnBoard(square));

			for (index = 0; index < numberOfDirections[piece]; ++index) {
				direction = pieceDirection[piece][index];
				tempSquare = square + direction;

				if (SQOFFBOARD_MOVEGEN(tempSquare)) {
					continue;
				}

				// BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
				if (position->pieces[tempSquare] != empty) {
					if (pieceColor[position->pieces[tempSquare]] == (side ^ 1)) {
						addCaptureMove(position, MOVE_MOVEGEN(square, tempSquare, position->pieces[tempSquare], empty, 0), list);
					}
					continue;
				}
			}
		}

		piece = loopNonSlidingPiece[pieceIndex++];
	}
	//ASSERT(MoveListOk(list, position));
}

static void searchPosition(boardStructure* position, searchInfoStructure* info, hashTableStructure* table) {

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

