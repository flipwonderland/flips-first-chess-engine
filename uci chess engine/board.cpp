#include "defs.h"
#include "util.h"

bool squareIs120(const int square) {
	return (square >= 0 && square < 120);
}

bool squareOnBoard(const int square) {
	return filesBoard[square] == offBoard ? 0 : 1;
}

bool sideValid(const int side) {
	return (side == white || side == black) ? 1 : 0;
}

bool fileRankValid(const int fromRank) {
	return (fromRank >= 0 && fromRank <= 7) ? 1 : 0;
}

bool pieceValidEmpty(const int piece) {
	return (piece >= empty && piece <= bQ) ? 1 : 0;
}

bool pieceValidEmptyOffboard(const int piece) {
	return (pieceValidEmpty(piece) || piece == offBoard);
}

bool pieceValid(const int piece) {
	return (piece >= wK && piece <= bQ) ? 1 : 0;
}

void updateListsMaterial(boardStructure* position) {
	int piece;
	int square;
	int color;
	int i;

	for (i = 0; i < BRD_SQ_NUM; i++) {
		square = i;
		piece = position->pieces[i];
		if (piece != offBoard && piece != empty) {

			color = pieceColor[piece];

			if (normalPiece[piece])
				position->normalPieces[color]++;
			if (minorPiece[piece])
				position->minorPieces[color]++;
			if (majorPiece[piece])
				position->majorPieces[color]++;

			position->material[color] += pieceValue[piece];


			position->pieceList[piece][position->pieceNumber[piece]] = square;
			position->pieceNumber[piece]++;

			if (piece == wK) //redundant apparently
				position->kingSquare[color] = square;
			if (piece == bK)
				position->kingSquare[color] = square;

			if (piece == wP) {
				SETBIT(position->bitBoardPawns[color], SQ64(square));
				SETBIT(position->bitBoardPawns[none], SQ64(square));
			}
			else if (piece == bP) {
				SETBIT(position->bitBoardPawns[color], SQ64(square));
				SETBIT(position->bitBoardPawns[none], SQ64(square));
			}
			/*
			if (piece == wK) {
				SETBIT(position->bitBoardKnights[color], SQ64(square));
				SETBIT(position->bitBoardKnights[none], SQ64(square));
			}
			if (piece == bK) {
				SETBIT(position->bitBoardKnights[color], SQ64(square));
				SETBIT(position->bitBoardKnights[none], SQ64(square));
			}
			if (piece == wB) {
				SETBIT(position->bitBoardBishops[color], SQ64(square));
				SETBIT(position->bitBoardBishops[none], SQ64(square));
			}
			if (piece == bB) {
				SETBIT(position->bitBoardBishops[color], SQ64(square));
				SETBIT(position->bitBoardBishops[none], SQ64(square));
			}
			if (piece == wR) {
				SETBIT(position->bitBoardRooks[color], SQ64(square));
				SETBIT(position->bitBoardRooks[none], SQ64(square));
			}
			if (piece == bR) {
				SETBIT(position->bitBoardRooks[color], SQ64(square));
				SETBIT(position->bitBoardRooks[none], SQ64(square));
			}
			if (piece == wQ) {
				SETBIT(position->bitBoardQueens[color], SQ64(square));
				SETBIT(position->bitBoardQueens[none], SQ64(square));
			}
			if (piece == bQ) {
				SETBIT(position->bitBoardQueens[color], SQ64(square));
				SETBIT(position->bitBoardQueens[none], SQ64(square));
			}
			*/
		}
	}
}

bool checkBoard(const boardStructure* position) {

	int tempPieceNumber[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int tempNormalPiece[2] = { 0, 0 };
	int tempMajorPiece[2] = { 0, 0 };
	int tempMinorPiece[2] = { 0, 0 };

	int tempMaterial[2] = { 0, 0 };

	int tempPiece;
	int tempPieceCount;
	int square64;
	int square120;
	int color;
	int pieceCount;

	bool testFailed = false; //implement this later, just put it to true on all the character out lines

	u64 tempPawns[3] = { 0ULL, 0ULL, 0ULL };

	tempPawns[white] = position->bitBoardPawns[white];
	tempPawns[black] = position->bitBoardPawns[black];
	tempPawns[none] = position->bitBoardPawns[none];

	for (tempPiece = wK; tempPiece <= bQ; tempPiece++) {
		for (tempPieceCount = 0; tempPieceCount < position->pieceNumber[tempPiece]; tempPieceCount++) {
			square120 = position->pieceList[tempPiece][tempPieceCount];
			if (position->pieces[square120] != tempPiece) {
				std::cout << "error: board incorrect, piece list not aligned with board array\n";
			}
			else {
				continue;
			}
		}
	}

	for (square64 = 0; square64 < 64; square64++) {

		square120 = SQ120(square64);
		tempPiece = position->pieces[square120];
		tempPieceNumber[tempPiece]++;
		color = pieceColor[tempPiece];

		if (normalPiece[tempPiece])
			tempNormalPiece[color]++;
		if (minorPiece[tempPiece])
			tempMinorPiece[color]++;
		if (majorPiece[tempPiece])
			tempMajorPiece[color]++;

		if (color == white || color == black)
			tempMaterial[color] += pieceValue[tempPiece];
	}


	for (tempPiece = wK; tempPiece <= bQ; tempPiece++) {
		if (tempPieceNumber[tempPiece] != position->pieceNumber[tempPiece]) {
			std::cout << "error: temp piece does not equal real piece\n";
		}
	}

	//this is where the bitboard checks go, I'm just adding the pawns for now but later I'm gonna add all of the bitboards
	pieceCount = CNT(tempPawns[white]);
	if (pieceCount != position->pieceNumber[wP])
		std::cout << "white pawn bitboard does not match\n";
	pieceCount = CNT(tempPawns[black]);
	if (pieceCount != position->pieceNumber[bP])
		std::cout << "black pawn bitboard does not match\n";
	pieceCount = CNT(tempPawns[none]);
	if (pieceCount != (position->pieceNumber[wP] + position->pieceNumber[bP]))
		std::cout << "white and/or black pawn bitboard does not match\n";
	/*
	other bitboards etc etc
	...
	...
	...
	*/

	while (tempPawns[white]) {
		square64 = POP(&tempPawns[white]);
		if (position->pieces[SQ120(square64)] != wP)
			std::cout << "bitboard (white) pawn was in the wrong position, or is it vice reversa?\n";
	}

	while (tempPawns[black]) {
		square64 = POP(&tempPawns[black]);
		if (position->pieces[SQ120(square64)] != bP)
			std::cout << "bitboard (black) pawn was in the wrong position, or is it vice reversa?\n";
	}

	while (tempPawns[none]) {
		square64 = POP(&tempPawns[none]);
		if ((position->pieces[SQ120(square64)] != wP) && (position->pieces[SQ120(square64)] != bP))
			std::cout << "bitboard (none) pawn was in the wrong position, or is it vice reversa?\n";
	}
	/*
	other bitboards etc etc
	...
	...
	...
	*/

	if (tempMaterial[white] != position->material[white])
		std::cout << "white material does not match\n" << "white material: " << tempMaterial[white] << "\n";
	if (tempMaterial[black] != position->material[black])
		std::cout << "black material does not match\n" << "black material: " << tempMaterial[black] << "\n";

	if (tempMinorPiece[white] != position->minorPieces[white])
		std::cout << "white minor pieces do not match\n";
	if (tempMinorPiece[black] != position->minorPieces[black])
		std::cout << "black minor pieces do not match\n";

	if (tempMajorPiece[white] != position->majorPieces[white])
		std::cout << "white major pieces do not match\n";
	if (tempMajorPiece[black] != position->majorPieces[black])
		std::cout << "black major pieces do not match\n";

	if (tempNormalPiece[white] != position->normalPieces[white])
		std::cout << "white pieces (not pawns) do not match\n";
	if (tempNormalPiece[black] != position->normalPieces[black])
		std::cout << "black pieces (not pawns) do not match\n";

	if (position->side != white && position->side != black)
		std::cout << "position side is neither white nor black\n";
	if (generatePositionKey(position) != position->positionKey) {
		std::cout << "position keys do not match\n";
		std::cout << "temp pos key: " << position->positionKey << "\n";
		u64 truePosKey = generatePositionKey(position);
		std::cout << "true pos key: " << truePosKey << "\n";

		testFailed = true;
	}
	if (position->enPassant != noSquare && ((ranksBoard[position->enPassant] != rank6 && position->side == white)
		|| (ranksBoard[position->enPassant] != rank3 && position->side == black))) //I'm not super confident in this one but looks right
		std::cout << "en passant square in invalid position\n";


	if (position->pieces[position->kingSquare[white]] != wK)
		std::cout << "the white 'king square' square does not have the white king on it\n";
	if (position->pieces[position->kingSquare[black]] != bK)
		std::cout << "the black 'king square' square does not have the black king on it\n";

	if (position->castlePermission < 0 || position->castlePermission > 15)
		std::cout << "castle permission is invalid\n";

	if (!testFailed)
		return true;
	else
		return false;
}


void resetBoard(boardStructure* position) {
	int i;

	for (i = 0; i < BRD_SQ_NUM; i++) {
		position->pieces[i] = offBoard;
	}

	for (i = 0; i < 64; i++) {
		position->pieces[sq64ToSq120[i]] = empty; // this converts the index into the actual play board, so the play board is reset to empty
	}

	for (i = 0; i < 2; i++) {
		position->normalPieces[i] = 0;
		position->majorPieces[i] = 0;
		position->minorPieces[i] = 0;
		position->nonPieces[i] = 0;

		position->material[i] = 0;
	}
	for (i = 0; i < 3; i++) {

		position->bitBoardKings[i] = 0ULL;
		position->bitBoardPawns[i] = 0ULL;
		position->bitBoardBishops[i] = 0ULL;
		position->bitBoardKnights[i] = 0ULL;
		position->bitBoardRooks[i] = 0ULL;
		position->bitBoardQueens[i] = 0ULL;



	}

	for (i = 0; i < 13; i++) {
		position->pieceNumber[i] = 0;
	}

	position->kingSquare[white] = noSquare;
	position->kingSquare[black] = noSquare;

	position->side = none;
	position->enPassant = noSquare;
	position->fiftyMove = 0;

	position->ply = 0;
	position->historyPly = 0;

	position->castlePermission = 0;

	position->positionKey = 0ULL;


}
