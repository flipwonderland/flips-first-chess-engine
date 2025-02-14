#include "util.h"
#include "defs.h"


const int pawnTableOpening[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

const int pawnTableEndgame[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
-40	,	-40	,	-40	,	-40	,	-40	,	-40	,	-40	,	-40	,
-40	,	-40	,	-40	,	-40	,   -40	,	-40	,	-40	,	-40	,
-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,
-10	,	-10	,	-10	,	-10	,	-10	,	-10	,	-10	,	-10	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
20	,	20	,	20  ,	20	,	20	,	20	,	20	,	20	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

const int knightTable[64] = {
0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

const int bishopTable[64] = {
0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

const int rookTable[64] = {
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0
};

const int kingEndGame[64] = {
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50
};

const int kingOpening[64] = {
	0	,	5	,	10	,	-10	,	-10	,	0	,	20	,	10	,
	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,
	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70
};

int mirror64[64] = {
56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	,
40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	,
32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	,
24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	,
16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	,
8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	,
0	,	1	,	2	,	3	,	4	,	5	,	6	,	7
};

#define MIRROR64(sq) (mirror64[(sq)])

static void mirrorBoard(boardStructure* position) {

	int tempPiecesArray[64];
	int tempSide = position->side ^ 1;
	int SwapPiece[13] = { empty, bK, bP, bN, bB, bR, bQ, wK, wP, wN, wB, wR, wQ };
	int tempCastlePerm = 0;
	int tempEnPas = noSquare;

	int sq;
	int tp;

	if (position->castlePermission & whiteKingCastle) tempCastlePerm |= blackKingCastle;
	if (position->castlePermission & whiteQueenCastle) tempCastlePerm |= blackQueenCastle;

	if (position->castlePermission & blackKingCastle) tempCastlePerm |= whiteKingCastle;
	if (position->castlePermission & blackQueenCastle) tempCastlePerm |= whiteQueenCastle;

	if (position->enPassant != noSquare) {
		tempEnPas = SQ120(mirror64[SQ64(position->enPassant)]);
	}

	for (sq = 0; sq < 64; sq++) {
		tempPiecesArray[sq] = position->pieces[SQ120(mirror64[sq])];
	}

	resetBoard(position);

	for (sq = 0; sq < 64; sq++) {
		tp = SwapPiece[tempPiecesArray[sq]];
		position->pieces[SQ120(sq)] = tp;
	}

	position->side = tempSide;
	position->castlePermission = tempCastlePerm;
	position->enPassant = tempEnPas;

	position->positionKey = generatePositionKey(position);

	updateListsMaterial(position);

	//ASSERT(CheckBoard(pos));
}

// sjeng 11.2
//8/6R1/2k5/6P1/8/8/4nP2/6K1 w - - 1 41 
static bool materialDraw(const boardStructure* position) {

	//ASSERT(CheckBoard(position));

	if (!position->pieceNumber[wR] && !position->pieceNumber[bR] && !position->pieceNumber[wQ] && !position->pieceNumber[bQ]) {
		if (!position->pieceNumber[bB] && !position->pieceNumber[wB]) {
			if (position->pieceNumber[wN] < 3 && position->pieceNumber[bN] < 3) { return true; }
		}
		else if (!position->pieceNumber[wN] && !position->pieceNumber[bN]) {
			if (abs(position->pieceNumber[wB] - position->pieceNumber[bB]) < 2) { return true; }
		}
		else if ((position->pieceNumber[wN] < 3 && !position->pieceNumber[wB]) || (position->pieceNumber[wB] == 1 && !position->pieceNumber[wN])) {
			if ((position->pieceNumber[bN] < 3 && !position->pieceNumber[bB]) || (position->pieceNumber[bB] == 1 && !position->pieceNumber[bN])) { return true; }
		}
	}
	else if (!position->pieceNumber[wQ] && !position->pieceNumber[bQ]) {
		if (position->pieceNumber[wR] == 1 && position->pieceNumber[bR] == 1) {
			if ((position->pieceNumber[wN] + position->pieceNumber[wB]) < 2 && (position->pieceNumber[bN] + position->pieceNumber[bB]) < 2) { return true; }
		}
		else if (position->pieceNumber[wR] == 1 && !position->pieceNumber[bR]) {
			if ((position->pieceNumber[wN] + position->pieceNumber[wB] == 0) && (((position->pieceNumber[bN] + position->pieceNumber[bB]) == 1) || ((position->pieceNumber[bN] + position->pieceNumber[bB]) == 2))) { return true; }
		}
		else if (position->pieceNumber[bR] == 1 && !position->pieceNumber[wR]) {
			if ((position->pieceNumber[bN] + position->pieceNumber[bB] == 0) && (((position->pieceNumber[wN] + position->pieceNumber[wB]) == 1) || ((position->pieceNumber[wN] + position->pieceNumber[wB]) == 2))) { return true; }
		}
	}
	return false;
}

#define ENDGAME_MAT (1 * pieceValue[wR] + 2 * pieceValue[wN] + 2 * pieceValue[wP] + pieceValue[wK])

int evaluatePosition(const boardStructure* position) {

	int piece;
	int pieceNumber;
	int square;
	int score = position->material[white] - position->material[black];

	if (!position->pieceNumber[wP] && !position->pieceNumber[bP] && materialDraw(position) == true) {
		return 0;
	}

	bool whiteIsolated = false;
	bool blackIsolated = false;

	bool whitePassed = false;
	bool blackPassed = false;

	bool endGame;

	if (position->material[black] + position->material[white] <= ENDGAME_MAT * 2) {
		endGame = true;
	}
	else {
		endGame = false;
	}

	piece = wP;
	for (pieceNumber = 0; pieceNumber < position->pieceNumber[piece]; pieceNumber++) {
		square = position->pieceList[piece][pieceNumber];
#ifdef DEBUG
		if (!squareOnBoard(square)) {
			std::cout << "square not on board in eval position func\n";
		}
		if (SQ64(square) < 0 || SQ64(square) > 63) {
			std::cout << "square in invalid position in eval func\n";
		}
#endif	

		if (endGame) {
			score += pawnTableEndgame[SQ64(square)];
		}
		else if (!endGame) {
			score += pawnTableOpening[SQ64(square)];
		}


		//score += pawnTableOpening[SQ64(square)];



		if ((isolatedMask[SQ64(square)] & position->bitBoardPawns[white]) == 0) {
			//printf("wP Iso:%s\n",printSquare(square));
			//score += pawnIsolated;
			whiteIsolated = true;
		}

		if ((whiteDoubledMask[SQ64(square)] & position->bitBoardPawns[white]) != 0) {
			if (whiteIsolated) {
				//printf("wP Doubled Isolated:%s\n", printSquare(square));
				score += pawnDoubledIsolated;
				whiteIsolated = false;
			}
			else {
				//printf("wP Doubled:%s\n", printSquare(square));
				score += pawnDoubled;
			}
		}

		if (whiteIsolated) {
			score += pawnIsolated;
			whiteIsolated = false;
		}

		if ((whitePassedMask[SQ64(square)] & position->bitBoardPawns[black]) == 0) {
			//printf("wP Passed:%s\n",printSquare(square));
			//score += pawnPassed[ranksBoard[square]];
			whitePassed = true;
		}

		if ((whiteConnectedMask[SQ64(square)] & position->bitBoardPawns[white]) != 0) {
			if (whitePassed) {
				//printf("wP Connected Passed:%s\n",printSquare(square));
				score += pawnConnectedPassed[ranksBoard[square]];
				whitePassed = false;
			}
			else {
				//printf("wP Connected:%s\n", printSquare(square));
				score += pawnConnected[ranksBoard[square]];
			}
		}

		if (whitePassed) {
			//printf("wP Passed:%s\n",printSquare(square));
			score += pawnPassed[ranksBoard[square]];
			whitePassed = false;
		}

	}

	piece = bP;
	for (pieceNumber = 0; pieceNumber < position->pieceNumber[piece]; ++pieceNumber) {
		square = position->pieceList[piece][pieceNumber];
#ifdef DEBUG
		if (!squareOnBoard(square)) {
			std::cout << "square not on board in eval func\n";
		}
		if (MIRROR64(SQ64(square)) < 0 || MIRROR64(SQ64(square)) > 63) {
			std::cout << "square in invalid position in eval func\n";
		}
#endif	

		if (endGame) {
			score -= pawnTableEndgame[MIRROR64(SQ64(square))];
		}
		else {
			score -= pawnTableOpening[MIRROR64(SQ64(square))];
		}

		//score -= pawnTableOpening[SQ64(square)];


		if ((isolatedMask[SQ64(square)] & position->bitBoardPawns[black]) == 0) {
			//printf("bP Iso:%s\n",printSquare(square));
			//score -= pawnIsolated;
			blackIsolated = true;
		}

		if ((blackDoubledMask[SQ64(square)] & position->bitBoardPawns[black]) != 0) {
			if (blackIsolated) {
				//printf("bP Doubled Isolated:%s\n", printSquare(square));
				score -= pawnDoubledIsolated;
				blackIsolated = false;
			}
			else {
				score -= pawnDoubled;
			}
		}

		if (blackIsolated) {
			score -= pawnIsolated;
			blackIsolated = false;
		}


		if ((blackPassedMask[SQ64(square)] & position->bitBoardPawns[white]) == 0) {
			//printf("bP Passed:%s\n",printSquare(square));
			//score -= pawnPassed[7 - ranksBoard[square]];
			blackPassed = true;
		}

		if ((blackConnectedMask[SQ64(square)] & position->bitBoardPawns[black]) != 0) {
			if (blackPassed) {
				//printf("bP Connected Passed:%s\n", printSquare(square));
				score -= pawnConnectedPassed[7 - ranksBoard[square]];
				blackPassed = false;
			}
			else {
				//printf("bP Connected:%s\n", printSquare(square));
				score -= pawnConnected[7 - ranksBoard[square]];
			}
		}

		if (blackPassed) {
			//printf("bP Passed:%s\n",printSquare(square));
			score -= pawnPassed[7 - ranksBoard[square]];
			blackPassed = false;
		}

	}

	piece = wN;
	for (pieceNumber = 0; pieceNumber < position->pieceNumber[piece]; ++pieceNumber) {
		square = position->pieceList[piece][pieceNumber];
#ifdef DEBUG
		if (!squareOnBoard(square)) {
			std::cout << "square not on board in eval func\n";
		}
		if (MIRROR64(SQ64(square)) < 0 || MIRROR64(SQ64(square)) > 63) {
			std::cout << "square in invalid position in eval func\n";
		}
#endif
		score += knightTable[SQ64(square)];
	}

	piece = bN;
	for (pieceNumber = 0; pieceNumber < position->pieceNumber[piece]; ++pieceNumber) {
		square = position->pieceList[piece][pieceNumber];
#ifdef DEBUG
		if (!squareOnBoard(square)) {
			std::cout << "square not on board in eval func\n";
		}
		if (MIRROR64(SQ64(square)) < 0 || MIRROR64(SQ64(square)) > 63) {
			std::cout << "square in invalid position in eval func\n";
		}
#endif
		score -= knightTable[MIRROR64(SQ64(square))];
	}

	piece = wB;
	for (pieceNumber = 0; pieceNumber < position->pieceNumber[piece]; ++pieceNumber) {
		square = position->pieceList[piece][pieceNumber];
#ifdef DEBUG
		if (!squareOnBoard(square)) {
			std::cout << "square not on board in eval func\n";
		}
		if (MIRROR64(SQ64(square)) < 0 || MIRROR64(SQ64(square)) > 63) {
			std::cout << "square in invalid position in eval func\n";
		}
#endif
		score += bishopTable[SQ64(square)];
	}

	piece = bB;
	for (pieceNumber = 0; pieceNumber < position->pieceNumber[piece]; ++pieceNumber) {
		square = position->pieceList[piece][pieceNumber];
#ifdef DEBUG
		if (!squareOnBoard(square)) {
			std::cout << "square not on board in eval func\n";
		}
		if (MIRROR64(SQ64(square)) < 0 || MIRROR64(SQ64(square)) > 63) {
			std::cout << "square in invalid position in eval func\n";
		}
#endif
		score -= bishopTable[MIRROR64(SQ64(square))];
	}

	piece = wR;
	for (pieceNumber = 0; pieceNumber < position->pieceNumber[piece]; ++pieceNumber) {
		square = position->pieceList[piece][pieceNumber];
#ifdef DEBUG
		if (!squareOnBoard(square)) {
			std::cout << "square not on board in eval func\n";
		}
		if (MIRROR64(SQ64(square)) < 0 || MIRROR64(SQ64(square)) > 63) {
			std::cout << "square in invalid position in eval func\n";
		}
#endif
		score += rookTable[SQ64(square)];
		/*
		ASSERT(FileRankValid(FilesBrd[square]));
		*/

		if (!(position->bitBoardPawns[none] & fileBBMask[filesBoard[square]])) {
			score += rookOpenFile;
		}
		else if (!(position->bitBoardPawns[white] & fileBBMask[filesBoard[square]])) {
			score += rookSemiOpenFile;
		}

	}

	piece = bR;
	for (pieceNumber = 0; pieceNumber < position->pieceNumber[piece]; ++pieceNumber) {
		square = position->pieceList[piece][pieceNumber];
#ifdef DEBUG
		if (!squareOnBoard(square)) {
			std::cout << "square not on board in eval func\n";
		}
		if (MIRROR64(SQ64(square)) < 0 || MIRROR64(SQ64(square)) > 63) {
			std::cout << "square in invalid position in eval func\n";
		}
#endif
		score -= rookTable[MIRROR64(SQ64(square))];
		/*
		ASSERT(FileRankValid(FilesBrd[square]));
		*/
		if (!(position->bitBoardPawns[none] & fileBBMask[filesBoard[square]])) {
			score -= rookOpenFile;
		}
		else if (!(position->bitBoardPawns[black] & fileBBMask[filesBoard[square]])) {
			score -= rookSemiOpenFile;
		}
	}

	piece = wQ;
	for (pieceNumber = 0; pieceNumber < position->pieceNumber[piece]; ++pieceNumber) {
		square = position->pieceList[piece][pieceNumber];
#ifdef DEBUG
		if (!squareOnBoard(square)) {
			std::cout << "square not on board in eval func\n";
		}
		if (MIRROR64(SQ64(square)) < 0 || MIRROR64(SQ64(square)) > 63) {
			std::cout << "square in invalid position in eval func\n";
		}
#endif
		/*
		ASSERT(FileRankValid(FilesBrd[square]));
		*/
		if (!(position->bitBoardPawns[none] & fileBBMask[filesBoard[square]])) {
			score += queenOpenFile;
		}
		else if (!(position->bitBoardPawns[white] & fileBBMask[filesBoard[square]])) {
			score += queenSemiOpenFile;
		}
	}

	piece = bQ;
	for (pieceNumber = 0; pieceNumber < position->pieceNumber[piece]; ++pieceNumber) {
		square = position->pieceList[piece][pieceNumber];
#ifdef DEBUG
		if (!squareOnBoard(square)) {
			std::cout << "square not on board in eval func\n";
		}
		if (MIRROR64(SQ64(square)) < 0 || MIRROR64(SQ64(square)) > 63) {
			std::cout << "square in invalid position in eval func\n";
		}
#endif
		/*
		ASSERT(FileRankValid(FilesBrd[square]));
		*/
		if (!(position->bitBoardPawns[none] & fileBBMask[filesBoard[square]])) {
			score -= queenOpenFile;
		}
		else if (!(position->bitBoardPawns[black] & fileBBMask[filesBoard[square]])) {
			score -= queenSemiOpenFile;
		}
	}
	//8/p6k/6p1/5p2/P4K2/8/5pB1/8 b - - 2 62 
	piece = wK;
	square = position->pieceList[piece][0];
#ifdef DEBUG
	if (!squareOnBoard(square)) {
		std::cout << "square not on board in eval func\n";
	}
	if (MIRROR64(SQ64(square)) < 0 || MIRROR64(SQ64(square)) > 63) {
		std::cout << "square in invalid position in eval func\n";
	}
#endif

	if (endGame) {
		score += kingEndGame[SQ64(square)];
	}
	else {
		score += kingOpening[SQ64(square)];
	}


	piece = bK;
	square = position->pieceList[piece][0];
#ifdef DEBUG
	if (!squareOnBoard(square)) {
		std::cout << "square not on board in eval func\n";
	}
	if (MIRROR64(SQ64(square)) < 0 || MIRROR64(SQ64(square)) > 63) {
		std::cout << "square in invalid position in eval func\n";
	}
#endif

	if (endGame) {
		score -= kingEndGame[MIRROR64(SQ64(square))];
	}
	else {
		score -= kingOpening[MIRROR64(SQ64(square))];
	}


	if (position->pieceNumber[wB] >= 2) score += bishopPair;
	if (position->pieceNumber[bB] >= 2) score -= bishopPair;

	if (position->side == white) {
		return score;
	}
	else {
		return -score;
	}


	return 0;
}