#include "defs.h"
#include <iostream>

char* printSquare(const int square) {

	static char squareString[3];

	int file = filesBoard[square];
	int rank = ranksBoard[square];

	sprintf_s(squareString, "%c%c", ('a' + file), ('1' + rank));

	return squareString;

}

void printSquareBoard(const boardStructure* position) {
	int sq;
	int file;
	int rank;
	int piece;

	std::cout << "\ngame board: \n" << "-----------------------\n";

	for (rank = rank8; rank >= rank1; rank--) {
		std::cout << " " << rank + 1 << " ";
		for (file = fileA; file <= fileH; file++) {
			sq = FR2SQ(file, rank);
			piece = position->pieces[sq];
			std::cout << " " << pieceCharacter[piece];
		}
		std::cout << "\n";
	}

	std::cout << "\n    ";
	for (file = fileA; file <= fileH; file++) {
		printf("%c", 'a' + file);
		printf(" ");
		//std::cout << " " << "a" + file;
	}
	std::cout << "\n-----------------------\n";
	std::cout << "\n";

	std::cout << "side: " << sideCharacter[position->side] << "\n";
	printf("en Passant:%d\n", position->enPassant);
	//std::cout << "en Passant: " << position->enPassant << "\n";
	std::cout << "castle permission:\n";
	position->castlePermission& whiteKingCastle ? printCastle = 'K' : printCastle = '-';
	std::cout << "K: " << printCastle << "\n";
	position->castlePermission& whiteQueenCastle ? printCastle = 'Q' : printCastle = '-';
	std::cout << "Q: " << printCastle << "\n";
	position->castlePermission& blackKingCastle ? printCastle = 'k' : printCastle = '-';
	std::cout << "k: " << printCastle << "\n";
	position->castlePermission& blackQueenCastle ? printCastle = 'q' : printCastle = '-';
	std::cout << "q: " << printCastle << "\n"; //this is really ugly

	std::cout << "position key:\n" << std::hex << position->positionKey;


	std::cout << "\n-----------------------\n";
}

void printBitBoard(u64 bitBoardToPrint) {

	u64 shift = 1ULL;

	int rank;
	int file;
	int square = 0;

	std::cout << "\n";
	for (rank = 7; rank >= 0; rank--) {
		for (file = 0; file <= 7; file++) {
			square = (rank * 8) + file;

			if ((shift << square) & bitBoardToPrint) {
				std::cout << "X";
			}
			else {
				std::cout << "-";
			}
		}

		std::cout << " " << square;
		std::cout << "\n";
	}

}

char* printMove(const int move) {

	static char moveString[6];

	int fileFrom = filesBoard[FROMSQ(move)];
	int rankFrom = ranksBoard[FROMSQ(move)];
	int fileTo = filesBoard[TOSQ(move)];
	int rankTo = ranksBoard[TOSQ(move)];

	int promoted = PROMOTED(move);

	if (promoted > 1) {
		char promotedChar = 'q'; //note to self, remake this so I can tell when a piece is a queen and throw an error when it's none of the pieces
		if (IsKn(promoted)) {
			promotedChar = 'n';
		}
		else if (IsRQ(promoted) && !IsBQ(promoted)) {
			promotedChar = 'r';
		}
		else if (!IsRQ(promoted) && IsBQ(promoted)) {
			promotedChar = 'b';
		}
		sprintf_s(moveString, "%c%c%c%c%c", ('a' + fileFrom), ('1' + rankFrom), ('a' + fileTo), ('1' + rankTo), promotedChar);
	}
	else {
		sprintf_s(moveString, "%c%c%c%c", ('a' + fileFrom), ('1' + rankFrom), ('a' + fileTo), ('1' + rankTo));
	}

	return moveString;
}

void printMoveList(const moveListStructure* list) {

	int i;
	int score = 0;
	int move = 0;
	std::cout << "move list:" << list->moveCount << "\n";

	for (i = 0; i < list->moveCount; i++) {

		move = list->moves[i].move;
		score = list->moves[i].score;

		printf("move:%d > %s (score:%d)\n", i + 1, printMove(move), score);
	}
	printf("move list total %d moves:\n\n", list->moveCount);

}