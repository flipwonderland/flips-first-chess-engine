#include "defs.h"

static void parseFen(const char* fen, boardStructure* position) {
	int i;
	int rank = rank8;
	int file = fileA;
	int piece = 0;
	int count = 0;
	int sq64 = 0;
	int sq120 = 0;

	resetBoard(position);

	if (fen == NULL) {
		std::cout << "no pointer given to fen parser\n";
	}
	else if (position == NULL) {
		std::cout << "no pointer given to fen parser\n";
	}
	else {
		//this was basically my solution as well that's cool to see my idea had merit
		while ((rank >= rank1) && fen) {
			count = 1;
			switch (*fen) {
			case 'p':
				piece = bP;
				break;
			case 'r':
				piece = bR;
				break;
			case 'n':
				piece = bN;
				break;
			case 'b':
				piece = bB;
				break;
			case 'k':
				piece = bK;
				break;
			case 'q':
				piece = bQ;
				break;
			case 'P':
				piece = wP;
				break;
			case 'R':
				piece = wR;
				break;
			case 'N':
				piece = wN;
				break;
			case 'B':
				piece = wB;
				break;
			case 'K':
				piece = wK;
				break;
			case 'Q':
				piece = wQ;
				break;

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				piece = empty;
				count = *fen - '0'; //this is super cool, the ascii numbers are inline so when you do this, it gives you the number :D
				break;

			case '/':
			case ' ':
				rank--;
				file = fileA;
				fen++;
				continue; //I didn't know this existed but it is super useful and I will be using it a lot in the future

			default:
				std::cout << "fen pieces error\n";
				break;
			}

			for (i = 0; i < count; i++) { //here I need a switch to change the correct bitboard, and shift it to the position
				sq64 = (rank * 8) + file;
				sq120 = SQ120(sq64);
				if (piece != empty) {
					position->pieces[sq120] = piece;
				}
				file++;
			}
			fen++;
		}

		if (*fen == 'w' || *fen == 'b') {
			position->side = (*fen == 'w') ? white : black;
			fen += 2; //lmao he's really doing exactly what I did, I thought mine was bad design
		}
		else {
			std::cout << "error, side to move invalid\n";
		}

		for (i = 0; i < 4; i++) {
			if (*fen == ' ') {
				break;
			}
			switch (*fen) {
			case 'K': position->castlePermission |= whiteKingCastle;
				break;
			case 'Q': position->castlePermission |= whiteQueenCastle;
				break;
			case 'k': position->castlePermission |= blackKingCastle;
				break;
			case 'q': position->castlePermission |= blackQueenCastle;
				break;
			default:
				break;
			}
			fen++;
		}
		if (position->castlePermission < 0 && position->castlePermission > 15) {
			std::cout << "error, castle permission out of range in fen\n";
		}

		fen++;
		if (*fen != '-') {
			file = fen[0] - 'a';
			rank = fen[1] - '1';

			position->enPassant = FR2SQ(file, rank);

		}

		position->positionKey = generatePositionKey(position);

	}

	updateListsMaterial(position);

}

static int parseMove(const char* ptrChar, boardStructure* position) {

	//ASSERT(CheckBoard(position));

	if (ptrChar[1] > '8' || ptrChar[1] < '1')
		return NOMOVE;
	if (ptrChar[3] > '8' || ptrChar[3] < '1')
		return NOMOVE;
	if (ptrChar[0] > 'h' || ptrChar[0] < 'a')
		return NOMOVE;
	if (ptrChar[2] > 'h' || ptrChar[2] < 'a')
		return NOMOVE;

	int from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
	int to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');

	//ASSERT(SqOnBoard(from) && SqOnBoard(to));

	moveListStructure list[1];
	generateAllMoves(position, list);
	int MoveNum = 0;
	int Move = 0;
	int PromPce = empty;

	for (MoveNum = 0; MoveNum < list->moveCount; ++MoveNum) {
		Move = list->moves[MoveNum].move;
		if (FROMSQ(Move) == from && TOSQ(Move) == to) {
			PromPce = PROMOTED(Move);
			if (PromPce != empty) {
				if (IsRQ(PromPce) && !IsBQ(PromPce) && ptrChar[4] == 'r') {
					return Move;
				}
				else if (!IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4] == 'b') {
					return Move;
				}
				else if (IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4] == 'q') {
					return Move;
				}
				else if (IsKn(PromPce) && ptrChar[4] == 'n') {
					return Move;
				}
				continue;
			}
			return Move;
		}
	}

	return NOMOVE;
}

static void parsePosition(std::string lineInStr, boardStructure* position) {

	lineInStr += 9;

	const char* ptrChar = lineInStr.c_str();
	const char* lineIn = lineInStr.c_str();

	if (strncmp(lineIn, "startpos", 8) == 0) {
		parseFen(STARTFEN, position);
	}
	else {
		ptrChar = strstr(lineIn, "fen");
		if (ptrChar == NULL) {
			parseFen(STARTFEN, position);
		}
		else {
			ptrChar += 4;
			parseFen(ptrChar, position);
		}
	}

	ptrChar = strstr(lineIn, "moves");
	int move;

	if (ptrChar != NULL) {
		ptrChar += 6;
		while (*ptrChar) {
			move = parseMove(ptrChar, position);
			if (move == NOMOVE) break;
			makeMove(position, move);
			position->ply = 0;
			while (*ptrChar && *ptrChar != ' ') ptrChar++;
			ptrChar++;
		}
	}
#ifdef debug
	printSquareBoard(position);
#endif
}