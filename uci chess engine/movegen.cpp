#include "defs.h"

#include <iostream>

bool squareAttacked(const int square, const int side, const boardStructure* position) { //marker here to hunt for bugs

	int piece;
	int i;
	int tempSquare;
	int direction;

#ifdef DEBUG
	if (!squareOnBoard(square)) {
		std::cout << "square is not on board\n";
		return false;
	}
	else if (!sideValid(side)) {
		std::cout << "side is not valid\n";
		return false;
	}
	else if (!checkBoard(position)) {
		std::cout << "check board failed\n";
		return false;
	}
#endif

	if (side == white) { //will have to change this when I go to a 64 board square, otherwise the pawns with wrap around the board
		if (position->pieces[square - 11] == wP || position->pieces[square - 9] == wP) {
			return true;
		}
	}
	else {
		if (position->pieces[square + 11] == bP || position->pieces[square + 9] == bP) {
			return true;
		}
	}

	for (i = 0; i < 8; i++) {
		piece = position->pieces[square + knightDirection[i]];
		if (piece != offBoard && IsKn(piece) && pieceColor[piece] == side) {
			return true;
		}
	}

	for (i = 0; i < 4; i++) {
		direction = rookDirection[i];
		tempSquare = square + direction; //this is a lot more elegant than what I was trying
		piece = position->pieces[tempSquare];
		while (piece != offBoard) {
			if (piece != empty) {
				if (IsRQ(piece) && pieceColor[piece] == side) {
					return true;
				}
				break;
			}
			tempSquare += direction;
			piece = position->pieces[tempSquare];
		}
	}

	for (i = 0; i < 4; i++) {
		direction = bishopDirection[i];
		tempSquare = square + direction;
		piece = position->pieces[tempSquare];
		while (piece != offBoard) {
			if (piece != empty) {
				if (IsBQ(piece) && pieceColor[piece] == side) {
					return true;
				}
				break;
			}
			tempSquare += direction;
			piece = position->pieces[tempSquare];
		}
	}

	for (i = 0; i < 8; i++) {
		piece = position->pieces[square + kingDirection[i]];
		if (piece != offBoard && IsKi(piece) && pieceColor[piece] == side) {
			return true;
		}
	}

	return false;

}

static void addQuietMove(const boardStructure* position, int move, moveListStructure* list) {

#ifdef DEBUG
	if (!squareOnBoard(FROMSQ(move))) {
		std::cout << "quiet move from not on board\n";
	}
	if (!squareOnBoard(TOSQ(move))) {
		std::cout << "quiet move to not on board\n";
	}
	if (!checkBoard(position)) {
		std::cout << "check board failed on quiet move\n";
	}
	if (position->ply < 0 || position->ply > MAXDEPTH) {
		std::cout << "quiet move ply less than 0 or greater than " << MAXDEPTH << "\n";
	}
#endif

	list->moves[list->moveCount].move = move;

	if (position->searchKillers[0][position->ply] == move) {
		list->moves[list->moveCount].score = 900000;
	}
	else if (position->searchKillers[1][position->ply] == move) {
		list->moves[list->moveCount].score = 800000;
	}
	else {
		list->moves[list->moveCount].score = position->searchHistory[position->pieces[FROMSQ(move)]][TOSQ(move)];
	}

	list->moveCount++;

}

static void addCaptureMove(const boardStructure* position, int move, moveListStructure* list) {

#ifdef DEBUG
	if (!squareOnBoard(FROMSQ(move))) {
		std::cout << "capture move from not on board\n";
	}
	if (!squareOnBoard(TOSQ(move))) {
		std::cout << "capture move to not on board\n";
	}
	if (!checkBoard(position)) {
		std::cout << "check board failed on capture move\n";
	}
	if (position->ply < 0 || position->ply > MAXDEPTH) {
		std::cout << "capture move ply less than 0 or greater than " << MAXDEPTH << "\n";
	}
#endif

	list->moves[list->moveCount].move = move;
	list->moves[list->moveCount].score = mvvLvaScores[CAPTURED(move)][position->pieces[FROMSQ(move)]] + 1000000;
	list->moveCount++;

}

static void addEnPassantMove(const boardStructure* position, int move, moveListStructure* list) {

#ifdef DEBUG
	if (!squareOnBoard(FROMSQ(move))) {
		std::cout << "en passant move from not on board\n";
	}
	if (!squareOnBoard(TOSQ(move))) {
		std::cout << "en passant move to not on board\n";
	}
	if (!checkBoard(position)) {
		std::cout << "check board failed on en passant move\n";
	}
	if (position->ply < 0 || position->ply > MAXDEPTH) {
		std::cout << "en passant move ply less than 0 or greater than " << MAXDEPTH << "\n";
	}
#endif

	list->moves[list->moveCount].move = move;
	list->moves[list->moveCount].score = 105 + 1000000;
	list->moveCount++;

}

static void addWhitePawnMove(const boardStructure* position, const int from, const int to, moveListStructure* list) {

#ifdef DEBUG
	if (!squareOnBoard(from)) {
		std::cout << "pawn from move not on board\n";
	}
	if (!squareOnBoard(to)) {
		std::cout << "pawn to move not on board\n";
	}
	if (!checkBoard(position)) {
		std::cout << "checkboard failed on pawn move\n";
	}
#endif

	if (ranksBoard[from] == rank7) {
		addQuietMove(position, MOVE_MOVEGEN(from, to, empty, wQ, 0), list);
		addQuietMove(position, MOVE_MOVEGEN(from, to, empty, wR, 0), list);
		addQuietMove(position, MOVE_MOVEGEN(from, to, empty, wB, 0), list);
		addQuietMove(position, MOVE_MOVEGEN(from, to, empty, wN, 0), list);
	}
	else {
		addQuietMove(position, MOVE_MOVEGEN(from, to, empty, empty, 0), list);
	}
}

static void addBlackPawnMove(const boardStructure* position, const int from, const int to, moveListStructure* list) {

#ifdef DEBUG
	if (!squareOnBoard(from)) {
		std::cout << "pawn from move not on board\n";
	}
	if (!squareOnBoard(to)) {
		std::cout << "pawn to move not on board\n";
	}
	if (!checkBoard(position)) {
		std::cout << "checkboard failed on pawn move\n";
	}
#endif

	if (ranksBoard[from] == rank2) {
		addQuietMove(position, MOVE_MOVEGEN(from, to, empty, bQ, 0), list);
		addQuietMove(position, MOVE_MOVEGEN(from, to, empty, bR, 0), list);
		addQuietMove(position, MOVE_MOVEGEN(from, to, empty, bB, 0), list);
		addQuietMove(position, MOVE_MOVEGEN(from, to, empty, bN, 0), list);
	}
	else {
		addQuietMove(position, MOVE_MOVEGEN(from, to, empty, empty, 0), list);
	}
}


static void addWhitePawnCaptureMove(const boardStructure* position, const int from, const int to, const int capture, moveListStructure* list) {

#ifdef DEBUG
	if (!pieceValidEmpty(capture)) {
		std::cout << "pawn from move not on board\n";
	}
	if (!squareOnBoard(from)) {
		std::cout << "pawn from move not on board\n";
	}
	if (!squareOnBoard(to)) {
		std::cout << "pawn to move not on board\n";
	}
	if (!checkBoard(position)) {
		std::cout << "checkboard failed on pawn move\n";
	}
#endif

	if (ranksBoard[from] == rank7) {
		addCaptureMove(position, MOVE_MOVEGEN(from, to, capture, wQ, 0), list);
		addCaptureMove(position, MOVE_MOVEGEN(from, to, capture, wR, 0), list);
		addCaptureMove(position, MOVE_MOVEGEN(from, to, capture, wB, 0), list);
		addCaptureMove(position, MOVE_MOVEGEN(from, to, capture, wN, 0), list);
	}
	else {
		addCaptureMove(position, MOVE_MOVEGEN(from, to, capture, empty, 0), list);
	}
}

static void addBlackPawnCaptureMove(const boardStructure* position, const int from, const int to, const int capture, moveListStructure* list) {

#ifdef DEBUG
	if (!pieceValidEmpty(capture)) {
		std::cout << "pawn from move not on board\n";
	}
	if (!squareOnBoard(from)) {
		std::cout << "pawn from move not on board\n";
	}
	if (!squareOnBoard(to)) {
		std::cout << "pawn to move not on board\n";
	}
	if (!checkBoard(position)) {
		std::cout << "checkboard failed on pawn move\n";
	}
#endif

	if (ranksBoard[from] == rank2) {
		addCaptureMove(position, MOVE_MOVEGEN(from, to, capture, bQ, 0), list);
		addCaptureMove(position, MOVE_MOVEGEN(from, to, capture, bR, 0), list);
		addCaptureMove(position, MOVE_MOVEGEN(from, to, capture, bB, 0), list);
		addCaptureMove(position, MOVE_MOVEGEN(from, to, capture, bN, 0), list);
	}
	else {
		addCaptureMove(position, MOVE_MOVEGEN(from, to, capture, empty, 0), list);
	}
}

void generateAllCaptures(const boardStructure* position, moveListStructure* list) {

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

void generateAllMoves(const boardStructure* position, moveListStructure* list) {

#ifdef DEBUG
	if (!checkBoard(position)) {
		std::cout << "board check failed in generate moves function\n";
	}
#endif

	list->moveCount = 0;

	int piece = empty;
	int side = position->side;
	int square = 0;
	int targetSquare = 0;
	int pieceNumber = 0;

	int direction = 0;
	int index; //wow I'm actually spelling it out I'm ashamed
	int pieceIndex;

	if (side == white) {

		for (pieceNumber = 0; pieceNumber < position->pieceNumber[wP]; pieceNumber++) {
			square = position->pieceList[wP][pieceNumber];


#ifdef DEBUG
			if (!squareOnBoard(square)) {
				std::cout << "pawn was off board in generate moves function\n";
				break;
			}
#endif 

			if (position->pieces[square + 10] == empty) {
				addWhitePawnMove(position, square, square + 10, list);
				if (ranksBoard[square] == rank2 && position->pieces[square + 20] == empty) {
					addQuietMove(position, MOVE_MOVEGEN(square, (square + 20), empty, empty, MFLAGPS), list);
				}
			}

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

		if (position->castlePermission & whiteKingCastle) {
			if (position->pieces[f1] == empty && position->pieces[g1] == empty) {
				if (!squareAttacked(e1, black, position) && !squareAttacked(f1, black, position)) {
					addQuietMove(position, MOVE_MOVEGEN(e1, g1, empty, empty, MFLAGCA), list);
				}
			}
		}

		if (position->castlePermission & whiteQueenCastle) {
			if (position->pieces[d1] == empty && position->pieces[c1] == empty && position->pieces[b1] == empty) {
				if (!squareAttacked(e1, black, position) && !squareAttacked(d1, black, position)) {
					addQuietMove(position, MOVE_MOVEGEN(e1, c1, empty, empty, MFLAGCA), list);
				}
			}
		}

	}
	else {
		for (pieceNumber = 0; pieceNumber < position->pieceNumber[bP]; pieceNumber++) {
			square = position->pieceList[bP][pieceNumber];

#ifdef DEBUG
			if (!squareOnBoard(square)) {
				std::cout << "pawn was off board in generate moves function\n";
				break;
			}
#endif 

			if (position->pieces[square - 10] == empty) {
				addBlackPawnMove(position, square, square - 10, list);
				if (ranksBoard[square] == rank7 && position->pieces[square - 20] == empty) {
					addQuietMove(position, MOVE_MOVEGEN(square, (square - 20), empty, empty, MFLAGPS), list);
				}
			}

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

		if (position->castlePermission & blackKingCastle) {
			if (position->pieces[f8] == empty && position->pieces[g8] == empty) {
				if (!squareAttacked(e8, white, position) && !squareAttacked(f8, white, position)) {
					addQuietMove(position, MOVE_MOVEGEN(e8, g8, empty, empty, MFLAGCA), list);
				}
			}
		}

		if (position->castlePermission & blackQueenCastle) {
			if (position->pieces[d8] == empty && position->pieces[c8] == empty && position->pieces[b8] == empty) {
				if (!squareAttacked(e8, white, position) && !squareAttacked(d8, white, position)) {
					addQuietMove(position, MOVE_MOVEGEN(e8, c8, empty, empty, MFLAGCA), list);
				}
			}
		}
	}

	pieceIndex = loopSlidingIndex[side];
	piece = loopSlidingPiece[pieceIndex];
	pieceIndex++;

	while (piece != 0) {
#ifdef DEBUG
		if (!pieceValid(piece)) {
			std::cout << "piece invalid on move list generator\n";
			break;
		}
#endif 
		for (pieceNumber = 0; pieceNumber < position->pieceNumber[piece]; pieceNumber++) {
			square = position->pieceList[piece][pieceNumber];
#ifdef DEBUG
			if (!squareOnBoard(square)) {
				std::cout << "non sliding piece not on board\n";
			}
#endif
			for (index = 0; index < numberOfDirections[piece]; index++) {
				direction = pieceDirection[piece][index];
				targetSquare = square + direction;

				while (!SQOFFBOARD_MOVEGEN(targetSquare)) {
					if (position->pieces[targetSquare] != empty) {
						if (pieceColor[position->pieces[targetSquare]] == (side ^ 1)) { // black ^ 1 == white and vice reversa
							addCaptureMove(position, MOVE_MOVEGEN(square, targetSquare, position->pieces[targetSquare], empty, 0), list);
						}
						break;
					}
					addQuietMove(position, MOVE_MOVEGEN(square, targetSquare, empty, empty, 0), list);
					targetSquare += direction; //this code really is remarkably similar to what I made, maybe I have some talent? probably just got lucky
				}
			}

		}
		piece = loopSlidingPiece[pieceIndex];
		pieceIndex++;

	}

	pieceIndex = loopNonSlidingIndex[side];
	piece = loopNonSlidingPiece[pieceIndex];
	pieceIndex++;

	while (piece != 0) {
#ifdef DEBUG
		if (!pieceValid(piece)) {
			std::cout << "piece invalid on move list generator\n";
			break;
		}
#endif
		for (pieceNumber = 0; pieceNumber < position->pieceNumber[piece]; pieceNumber++) {
			square = position->pieceList[piece][pieceNumber];
#ifdef DEBUG
			if (!squareOnBoard(square)) {
				std::cout << "non sliding piece not on board\n";
			}
#endif
			for (index = 0; index < numberOfDirections[piece]; index++) {
				direction = pieceDirection[piece][index];
				targetSquare = square + direction;

				if (SQOFFBOARD_MOVEGEN(targetSquare)) {
					continue;
				}
				if (position->pieces[targetSquare] != empty) {
					if (pieceColor[position->pieces[targetSquare]] == (side ^ 1)) { // black ^ 1 == white and vice reversa
						addCaptureMove(position, MOVE_MOVEGEN(square, targetSquare, position->pieces[targetSquare], empty, 0), list);
					}
					continue;
				}
				addQuietMove(position, MOVE_MOVEGEN(square, targetSquare, empty, empty, 0), list);
			}

		}

		piece = loopNonSlidingPiece[pieceIndex];
		pieceIndex++;

	}


}



static void clearPiece(const int square, boardStructure* position) {
#ifdef DEBUG
	if (!squareOnBoard(square)) {
		std::cout << "clear piece called on a square that's not on the board\n";
	}
#endif
	int piece = position->pieces[square];

#ifdef DEBUG
	if (!pieceValid(piece)) {
		std::cout << "clear piece called on a piece that is not valid\n";
	}
#endif

	int color = pieceColor[piece];
	int i = 0;
	int tempPieceNumber = -1;

	HASH_PCE(piece, square);

	position->pieces[square] = empty;
	position->material[color] -= pieceValue[piece];

	if (normalPiece[piece]) {
		position->normalPieces[color]--;
		if (majorPiece[piece]) {
			position->majorPieces[color]--;
		}
		else {
			position->minorPieces[color]--;
		}
	}
	else {
		CLRBIT(position->bitBoardPawns[color], SQ64(square));
		CLRBIT(position->bitBoardPawns[none], SQ64(square));
		//need to add the other bitboards when I start using them
	}

	for (i = 0; i < position->pieceNumber[piece]; i++) {
		if (position->pieceList[piece][i] == square) {
			tempPieceNumber = i;
			break;
		}
	}
#ifdef DEBUG
	if (tempPieceNumber == -1) {
		std::cout << "no\n";
	}
	else {
		position->pieceNumber[piece]--;
		position->pieceList[piece][tempPieceNumber] = position->pieceList[piece][position->pieceNumber[piece]];
	}
#endif
#ifdef PERFORMANCEMODE
	position->pieceNumber[piece]--;
	position->pieceList[piece][tempPieceNumber] = position->pieceList[piece][position->pieceNumber[piece]];
#endif
}

static void addPiece(const int square, boardStructure* position, const int piece) {
#ifdef DEBUG
	if (!squareOnBoard(square)) {
		std::cout << "clear piece called on a square that's not on the board\n";
	}
	if (!pieceValid(piece)) {
		std::cout << "add piece called on a piece that is not valid\n";
	}
#endif

	int color = pieceColor[piece];

#ifdef DEBUG
	if (!sideValid(color)) {
		std::cout << "side given to add piece is not valid\n";
	}
#endif

	HASH_PCE(piece, square);

	position->pieces[square] = piece;

	if (normalPiece[piece]) {
		position->normalPieces[color]++;
		if (majorPiece[piece]) {
			position->majorPieces[color]++;
		}
		else {
			position->minorPieces[color]++;
		}
	}
	else {
		SETBIT(position->bitBoardPawns[color], SQ64(square));
		SETBIT(position->bitBoardPawns[none], SQ64(square));
		//need to add the other bitboards when I start using them part doux
	}

	position->material[color] += pieceValue[piece];
	position->pieceList[piece][position->pieceNumber[piece]] = square;
	position->pieceNumber[piece]++;

}

static void movePiece(const int from, const int to, boardStructure* position) {

#ifdef DEBUG
	if (!squareOnBoard(from)) {
		std::cout << "move from is invalid\n";
		std::cout << "move from > " << from << "\n";
	}
	if (!squareOnBoard(to)) {
		std::cout << "move to is invalid\n";
		std::cout << "move to > " << to << "\n";
	}
#endif

	int i = 0;
	int piece = position->pieces[from];
	int color = pieceColor[piece];

#ifdef DEBUG
	if (!sideValid(color)) {
		std::cout << "side given to move piece is not valid\n";
	}
	if (!pieceValid(piece)) {
		std::cout << "move piece called on a piece that is not valid\n";
	}

	int t_PieceNum = false;
#endif

	HASH_PCE(piece, from);
	position->pieces[from] = empty;

	HASH_PCE(piece, to);
	position->pieces[to] = piece;

	if (!normalPiece[piece]) {
		CLRBIT(position->bitBoardPawns[color], SQ64(from));
		CLRBIT(position->bitBoardPawns[none], SQ64(from));
		SETBIT(position->bitBoardPawns[color], SQ64(to));
		SETBIT(position->bitBoardPawns[none], SQ64(to));
	} //need to add the other bitboards when I start using them part tres

	for (i = 0; i < position->pieceNumber[piece]; i++) {
		if (position->pieceList[piece][i] == from) {
			position->pieceList[piece][i] = to;
#ifdef DEBUG
			t_PieceNum = true;
#endif
			break;
		}
	}
#ifdef DEBUG
	if (!t_PieceNum) {
		std::cout << "piece not moved in move piece func\n";
	}
#endif
}
void takeMove(boardStructure* position) {
#ifdef DEBUG
	if (!checkBoard(position)) {
		std::cout << "check board failed at the start of take move\n";
	}
#endif

	position->historyPly--;
	position->ply--;

#ifdef DEBUG
	if (position->historyPly < 0 || position->historyPly > MAX_GAME_MOVES) {
		std::cout << "history went further than longest possible game (or under 0), consider upping the limit or if it's already the longest game possible theres a bug\n";
		std::cout << "history: " << position->historyPly << "\n";
	}
	if (position->ply < 0 || position->ply > MAXDEPTH) {
		std::cout << "ply invalid, either under 0 or above " << MAXDEPTH << "\n";
		std::cout << "ply: " << position->ply << "\n";
	}
#endif

	int move = position->history[position->historyPly].move;
	int from = FROMSQ(move);
	int to = TOSQ(move);

#ifdef DEBUG
	if (!squareOnBoard(from)) {
		std::cout << "move from is invalid\n";
		std::cout << "move from > " << from << "\n";

	}
	if (!squareOnBoard(to)) {
		std::cout << "move to is invalid\n";
		std::cout << "move to > " << to << "\n";

	}
#endif

	if (position->enPassant != noSquare)
		HASH_EP;

	HASH_CA;

	position->castlePermission = position->history[position->historyPly].castlePermission;
	position->fiftyMove = position->history[position->historyPly].fiftyMove;
	position->enPassant = position->history[position->historyPly].enPassant;

	if (position->enPassant != noSquare)
		HASH_EP;

	HASH_CA;

	position->side ^= 1;
	HASH_SIDE;

	if (MFLAGEP & move) {
		if (position->side == white) {
			addPiece(to - 10, position, bP);
		}
		else {
			addPiece(to + 10, position, wP);
		}
	}
	else if (MFLAGCA & move) {
		switch (to) {
		case c1:
			movePiece(d1, a1, position);
			break;
		case c8:
			movePiece(d8, a8, position);
			break;
		case g1:
			movePiece(f1, h1, position);
			break;
		case g8:
			movePiece(f8, h8, position);
			break;
		default:
			std::cout << "invalid castling move on take move\n";
			break;
		}
	}

	movePiece(to, from, position);

	if (isPieceKing[position->pieces[from]]) {
		position->kingSquare[position->side] = from;
	}

	int captured = CAPTURED(move);
	if (captured != empty) {
#ifdef DEBUG
		if (!pieceValid(captured)) {
			std::cout << "piece captured was not valid\n";
		}
#endif
		addPiece(to, position, captured);
	}

	if (PROMOTED(move) != empty) {
#ifdef DEBUG
		if (!pieceValid(PROMOTED(move)) || isPiecePawn[PROMOTED(move)]) {
			std::cout << "promoted piece invalid\n";
		}
#endif
		clearPiece(from, position);
		addPiece(from, position, (pieceColor[PROMOTED(move)] == white ? wP : bP));
	}
#ifdef DEBUG
	if (!checkBoard(position)) {
		std::cout << "check board failed at the end of take move\n";
	}
#endif
}

bool makeMove(boardStructure* position, int move) {

#ifdef DEBUG
	if (!checkBoard(position)) {
		std::cout << "invalid position given to make move func\n";
	}
#endif

	int from = FROMSQ(move);
	int to = TOSQ(move);
	int side = position->side;

#ifdef DEBUG
	if (!squareOnBoard(from)) {
		std::cout << "move from is invalid\n";
		std::cout << "move from > " << from << "\n";
	}
	if (!squareOnBoard(to)) {
		std::cout << "move to is invalid\n";
		std::cout << "move to > " << from << "\n";
	}
	if (!sideValid(side)) {
		std::cout << "side given to make move is not valid\n";
	}
	if (!pieceValid(position->pieces[from])) {
		std::cout << "make move called on a piece that is not valid\n";
	}
	if (position->historyPly < 0 || position->historyPly > MAX_GAME_MOVES) {
		std::cout << "history went further than longest possible game (or under 0), consider upping the limit or if it's already the longest game possible theres a bug\n";
		std::cout << "history: " << position->historyPly << "\n";
	}
	if (position->ply < 0 || position->ply > MAXDEPTH) {
		std::cout << "ply invalid, either under 0 or above " << MAXDEPTH << "\n";
		std::cout << "ply: " << position->ply << "\n";
	}
#endif

	position->history[position->historyPly].positionKey = position->positionKey;

	if (move & MFLAGEP) { //en passant
		if (side == white) {
			clearPiece(to - 10, position);
		}
		else {
			clearPiece(to + 10, position);
		} //gotta be careful, if a secret 3rd player enters the match they will be treated as black
	}
	else if (move & MFLAGCA) { //castling
		switch (to) {
		case c1:
			movePiece(a1, d1, position);
			break;
		case c8:
			movePiece(a8, d8, position);
			break;
		case g1:
			movePiece(h1, f1, position);
			break;
		case g8:
			movePiece(h8, f8, position);
			break;
		default:
			std::cout << "illegal castle in make move\n";
			break;
		}
	}

	if (position->enPassant != noSquare) {
		HASH_EP;
	}

	HASH_CA;

	position->history[position->historyPly].move = move;
	position->history[position->historyPly].fiftyMove = position->fiftyMove;
	position->history[position->historyPly].enPassant = position->enPassant;
	position->history[position->historyPly].castlePermission = position->castlePermission;

	position->castlePermission &= castlePermSheet[from];
	position->castlePermission &= castlePermSheet[to];
	position->enPassant = noSquare;

	HASH_CA;

	int captured = CAPTURED(move);
	position->fiftyMove++;

	if (captured != empty) {
#ifdef DEBUG
		if (!pieceValid(captured)) {
			std::cout << "piece captured was not valid\n";
		}
#endif
		clearPiece(to, position);
		position->fiftyMove = 0;
	}

	position->historyPly++;
	position->ply++;

#ifdef DEBUG
	if (position->historyPly < 0 || position->historyPly > MAX_GAME_MOVES) {
		std::cout << "history went further than longest possible game (or under 0), consider upping the limit or if it's already the longest game possible theres a bug\n";
		std::cout << "history: " << position->historyPly << "\n";
	}
	if (position->ply < 0 || position->ply > MAXDEPTH) {
		std::cout << "ply invalid, either under 0 or above " << MAXDEPTH << "\n";
		std::cout << "ply: " << position->ply << "\n";
	}
#endif

	if (isPiecePawn[position->pieces[from]]) {
		position->fiftyMove = 0;
		if (move & MFLAGPS) {
			if (side == white) {
				position->enPassant = from + 10;
#ifdef DEBUG
				if (ranksBoard[position->enPassant] != rank3) {
					std::cout << "en passant on an invalid rank\n";
				}
#endif
			}
			else {
				position->enPassant = from - 10;
#ifdef DEBUG
				if (ranksBoard[position->enPassant] != rank6) {
					std::cout << "en passant on an invalid rank\n";
				}
#endif
			}
			HASH_EP;
		}
	}

	movePiece(from, to, position);

	int promotedPiece = PROMOTED(move);
	if (promotedPiece != empty) {
#ifdef DEBUG
		if (!pieceValid(promotedPiece) || isPiecePawn[promotedPiece]) {
			std::cout << "promoted piece invalid\n";
		}
#endif
		clearPiece(to, position);
		addPiece(to, position, promotedPiece);
	}

	if (isPieceKing[position->pieces[to]]) {
		position->kingSquare[position->side] = to;
	}

	position->side ^= 1;
	HASH_SIDE;

#ifdef DEBUG
	if (!checkBoard(position)) {
		std::cout << "checkboard failed at the end of make move func\n";
	}
#endif


	if (squareAttacked(position->kingSquare[side], position->side, position)) {
		takeMove(position);
		return false;
	}

	return true;
}

void makeNullMove(boardStructure* position) {

#ifdef DEBUG
	if (checkBoard(position)) {}
	else {
		std::cout << "checkboard failed at make null move\n";
	}
	if (!squareAttacked(position->kingSquare[position->side], position->side ^ 1, position)) {}
	else {
		std::cout << "king in check in null move\n";
	}
#endif

	position->ply++;
	position->history[position->historyPly].positionKey = position->positionKey;

	if (position->enPassant != noSquare) HASH_EP;

	position->history[position->historyPly].move = NOMOVE;
	position->history[position->historyPly].fiftyMove = position->fiftyMove;
	position->history[position->historyPly].enPassant = position->enPassant;
	position->history[position->historyPly].castlePermission = position->castlePermission;
	position->enPassant = noSquare;

	position->side ^= 1;
	position->historyPly++;
	HASH_SIDE;

#ifdef DEBUG
	if (checkBoard(position)) {}
	else {
		std::cout << "checkboard failed at make null move\n";
	}
	if (position->historyPly >= 0 && position->historyPly < MAX_GAME_MOVES) {}
	else {
		std::cout << "placeholder\n";
	}
	if (position->ply >= 0 && position->ply < MAXDEPTH) {}
	else {
		std::cout << "placeholder\n";
	}
#endif
	return;
} // MakeNullMove idk wtf a null move is

void takeNullMove(boardStructure* position) {

#ifdef DEBUG
	if (checkBoard(position)) {}
	else {
		std::cout << "checkboard failed at take null move\n";
	}
#endif

	position->historyPly--;
	position->ply--;

	if (position->enPassant != noSquare) HASH_EP;

	position->castlePermission = position->history[position->historyPly].castlePermission;
	position->fiftyMove = position->history[position->historyPly].fiftyMove;
	position->enPassant = position->history[position->historyPly].enPassant;

	if (position->enPassant != noSquare) HASH_EP;
	position->side ^= 1;
	HASH_SIDE;

#ifdef DEBUG
	if (checkBoard(position)) {}
	else {
		std::cout << "checkboard failed at make null move\n";
	}
	if (position->historyPly >= 0 && position->historyPly < MAX_GAME_MOVES) {}
	else {
		std::cout << "placeholder\n";
	}
	if (position->ply >= 0 && position->ply < MAXDEPTH) {}
	else {
		std::cout << "placeholder\n";
	}
#endif
}

int moveExists(boardStructure* position, const int move) {

	moveListStructure list[1];
	generateAllMoves(position, list);

	int moveNumber;
	for (moveNumber = 0; moveNumber < list->moveCount; moveNumber++) {

		if (!makeMove(position, list->moves[moveNumber].move)) {
			continue;
		}
		takeMove(position);
		if (list->moves[moveNumber].move == move) {
			return true;
		}
	}
	return false;
}