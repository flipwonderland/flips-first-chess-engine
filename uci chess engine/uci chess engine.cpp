#include <iostream>
#include <string>
#include <cstdlib>

#include "stdlib.h"
#include "util.h"



/*
enum piece {
	none = 0b00000,
	king = 0b00001,
	pawn = 0b00010,
	knight = 0b00011,
	bishop = 0b00100,
	rook = 0b00101,
	queen = 0b00110,

	black = 0b01000,
	white = 0b10000
};
*/




enum {
	empty, wK, wP, wN, wB, wR, wQ, bK, bP, bN, bB, bR, bQ
};

enum {
	fileA, fileB, fileC, fileD, fileE, fileF, fileG, fileH, fileNone
};

enum {
	rank1, rank2, rank3, rank4, rank5, rank6, rank7, rank8, rankNone
};

enum {
	white, black, none
};

enum {
	a1 = 21, b1, c1, d1, e1, f1, g1, h1,
	a2 = 31, b2, c2, d2, e2, f2, g2, h2,
	a3 = 41, b3, c3, d3, e3, f3, g3, h3,
	a4 = 51, b4, c4, d4, e4, f4, g4, h4,
	a5 = 61, b5, c5, d5, e5, f5, g5, h5,
	a6 = 71, b6, c6, d6, e6, f6, g6, h6,
	a7 = 81, b7, c7, d7, e7, f7, g7, h7,
	a8 = 91, b8, c8, d8, e8, f8, g8, h8, noSquare

};

enum {
	whiteKingCastle = 0b0001,
	whiteQueenCastle = 0b0010,
	blackKingCastle = 0b0100,
	blackQueenCastle = 0b1000
};
typedef struct {

	int move;
	int castlePerm;
	int enPassant;
	int fiftyMove;
	u64 positionKey;

} undoStructure;

//gonna try his struct before I move to classes again so I can get an idea of how this works
typedef struct {

	int pieces[BRD_SQ_NUM];

	int kingSquare[2];
	
	u64 bitBoardKings[3];
	u64 bitBoardpawns[3];
	u64 bitBoardKnights[3];
	u64 bitBoardBishops[3];
	u64 bitBoardRooks[3];
	u64 bitBoardQueens[3];

	//adding these now but will remove them after the move gen is created, I'll just use the bitboards after I figure out how that works
	int pawns[3];
	int kings[3];
	int knights[3];
	int bishops[3];
	int rooks[3];
	int queens[3];

	int material[3];

	int side;
	int enPassant;
	int fiftyMove;

	int ply;
	int historyPly;

	int castlePermission;

	u64 positionKey;

	int normalPieces[3]; //not pawns
	int majorPieces[3];
	int minorPieces[3];
	int nonPieces[3]; //pawns

	undoStructure history[MAX_GAME_MOVES];
	
	int pieceNumber[13];
	int pieceList[13][10];

} boardStructure;

/*
class gameState {
public:
	char square[64];
	u64 enPassant;
	bool whiteToMove;
	bool whiteShortCastle;
	bool whiteLongCastle;
	bool blackShortCastle;
	bool blackLongCastle;
	int movesPassed;
	int moves[17697]; //longest possible chess game, will never need to store more moves than that
	
	//might need these might not
	//int halfmoveClock;
	//int fullmoveNumber;
	
	u64 whiteKingBitBoard;
	u64 whitePawnBitBoard;
	u64 whiteKnightBitBoard;
	u64 whiteBishopBitBoard;
	u64 whiteRookBitBoard;
	u64 whiteQueenBitBoard;
	u64 blackKingBitBoard;
	u64 blackPawnBitBoard;
	u64 blackKnightBitBoard;
	u64 blackBishopBitBoard;
	u64 blackRookBitBoard;
	u64 blackQueenBitBoard;
	u64 allBitBoard;

	void move(int moveId) {
		int temp1 = moveId;
		int temp2 = moveId;
		int temp3 = moveId;
		int temp4 = moveId;
		int temp5 = moveId;

		int fromFile;
		int fromRank;
		int toFile;
		int toRank;
		int promotion;

		temp1 = temp1 & 0b000000000000111;
		temp2 = temp2 & 0b000000000111000;
		temp3 = temp3 & 0b000000111000000;
		temp4 = temp4 & 0b000111000000000;
		temp5 = temp5 & 0b111000000000000;

		temp2 = temp2 >> 3;
		temp3 = temp3 >> 6;
		temp4 = temp4 >> 9;
		temp5 = temp5 >> 12;

		fromFile = temp1;
		fromRank = temp2;
		toFile = temp3;
		toRank = temp4;
		promotion = temp5;

		int fromSquare = (fromRank * 8) + fromFile;
		int toSquare = (toRank * 8) + toFile;

		if (promotion == 0) {
			square[toSquare] = square[fromSquare];
		}
		else {
			//I'm sorta proud of this one, I was thinking how to get the side then I realized I store the side in the piece data, why not just use that
			//I was thinking about including another variable in the func that tracks the side to move but that would've been a lot more clunky and redundant
			int pieceSide;
			pieceSide = square[fromSquare] & 0b11000;
			int newPromotionPiece = pieceSide | promotion;
			square[toSquare] = newPromotionPiece;
		}
		square[fromSquare] = piece::none;
	}
};
*/



void initializeSquare120ToSquare64() {
	int i;
	int file;
	int rank;
	int square = a1;
	int square64 = 0;

	for (i = 0; i < BRD_SQ_NUM; i++) {
		square120ToSquare64[i] = 65;
	}
	for (i = 0; i < 64; i++) {
		square64ToSquare120[i] = 120;
	}

	for (rank = rank1; rank <= rank8; rank++) {
		for (file = fileA; file <= fileH; file++) {
			square = FR2SQ(file, rank);
			square64ToSquare120[square64] = square; //I've typed square so many times it really doesn't seem like it's spelled that way
			square120ToSquare64[square] = square64;
			square64++;
		}
	}
}

u64 positionKey;
u64 pieceKeys[13][64];
u64 sideKey;
u64 castleKeys[16];


void initializeHashKeys() {
	for (int i = 0; i < 13; i++) {
		for (int i2 = 0; i2 < 64; i2++) {
			pieceKeys[i][i2] = RAND_64;
		}
	}
	sideKey = RAND_64;
	for (int i = 0; i < 16; i++) {
		castleKeys[i] = RAND_64;
	}
}

u64 setMask[64];
u64 clearMask[64];

void initializeBitMasks() {
	for (int i = 0; i < 64; i++) {
		setMask[i] = 0ULL;
		clearMask[i] = 0ULL;
	}
	for (int i = 0; i < 64; i++) {
		setMask[i] |= (1ULL << i);
		clearMask[i] = ~setMask[i];
	}
}


//got these from bluefever softwares series
//it is also here https://www.chessprogramming.org/Looking_for_Magics
const int bitTable[64] = {
	63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2, 51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35,
	62, 31, 40, 4, 49, 5, 52, 26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28, 58, 20, 37, 17, 36, 8
};

int popFirstBit(u64* bb) {
	u64 b = *bb ^ (*bb - 1);
	u32 fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
	*bb &= (*bb - 1);
	return bitTable[(fold * 0x783a9b23) >> 26]; //I have no idea what the point of this is
}

int countBits(u64 b) {
	int r;
	for (r = 0; b; r++, b &= b - 1);
	return r;
}

u64 generatePositionKey(const boardStructure* pos) {

	int square = 0;
	u64 finalKey = 0ULL;
	int piece = empty;

	//pieces
	for (; square < BRD_SQ_NUM; square++) {
		piece = pos->pieces[square];
		if (piece != noSquare && piece != empty) {
			if (piece >= wK && piece <= bQ) {
				finalKey ^= pieceKeys[piece][square];
			}
			else {
				std::cout << "error, invalid piece in the position key generator \n";
			}
		}
	}

	if (pos->side == white) {
		finalKey ^= sideKey;
	}

	if (pos->enPassant != noSquare) {
		if (pos->enPassant >= 0 && pos->enPassant < BRD_SQ_NUM) {
			finalKey ^= pieceKeys[empty][pos->enPassant];
		}
	}

	if (pos->castlePermission >= 0 && pos->castlePermission <= 15) {
		finalKey ^= castleKeys[pos->castlePermission];
	}
	else {
		std::cout << "error, invalid castling permissions given to the position key generator \n";
	}

	return finalKey;
}


/*
gameState clearBoard;
gameState currentBoard;
*/

/*
void clearGameState() {
	for (int i = 0; i <= 63; i++) {
		clearBoard.square[i] = piece::none;
	}
	clearBoard.enPassant = 0ULL;

	clearBoard.whiteToMove = true;
	clearBoard.whiteShortCastle = false;
	clearBoard.whiteLongCastle = false;
	clearBoard.blackLongCastle = false;
	clearBoard.blackShortCastle = false;
	clearBoard.movesPassed = 0;

	clearBoard.allBitBoard = 0ULL;
	clearBoard.whiteKingBitBoard = clearBoard.allBitBoard;
	clearBoard.whitePawnBitBoard = clearBoard.allBitBoard;
	clearBoard.whiteKnightBitBoard = clearBoard.allBitBoard;
	clearBoard.whiteBishopBitBoard = clearBoard.allBitBoard;
	clearBoard.whiteRookBitBoard = clearBoard.allBitBoard;
	clearBoard.whiteQueenBitBoard = clearBoard.allBitBoard;
	clearBoard.blackKingBitBoard = clearBoard.allBitBoard;
	clearBoard.blackPawnBitBoard = clearBoard.allBitBoard;
	clearBoard.blackKnightBitBoard = clearBoard.allBitBoard;
	clearBoard.blackBishopBitBoard = clearBoard.allBitBoard;
	clearBoard.blackRookBitBoard = clearBoard.allBitBoard;
	clearBoard.blackQueenBitBoard = clearBoard.allBitBoard;
	

	for (int i = 0; i <= 17696; i++) {
		clearBoard.moves[i] = 0;
	}
	currentBoard = clearBoard;
}
*/




std::string inputParser(std::string input, const int desiredToken) {
	std::string temp = input;
	std::string token;
	size_t stopTokenPlace;
	int tokenCount = 0;
	bool stop = false;
	int loopBreaker = 10000;
	int allowedTokensWithoutInfo = 2;

	while (!stop) {
		if (desiredToken != 0) stopTokenPlace = temp.find(' ');
		else stopTokenPlace = 0;

		if (stopTokenPlace == std::string::npos)/* end of the input string*/ {
			stop = true;
		}
		if (loopBreaker == 0) {
			std::cout << "infinite loop caused in input parser! input: " << input << "\ntoken: " << desiredToken << "\n";
			break;
		}
		if (allowedTokensWithoutInfo == 0)
			break;
		//might be able to change this to !=
		if (tokenCount < desiredToken) /*if it's less than the desired token, we don't care what it is, just delete it*/{
			temp.erase(0, stopTokenPlace + 1); //+1 because it's the array version that starts with 0, and this needs it to start with 1
			tokenCount++;
			allowedTokensWithoutInfo++;
		}
		else {
			int deleteAllAfterDesiredToken = temp.find(' '); //also don't care about what's after our token
			if (deleteAllAfterDesiredToken != std::string::npos)
				temp.erase(deleteAllAfterDesiredToken, temp.length()); //it would probably not be good if we deleted all after npos
			token = temp;
			return token;
		}
		loopBreaker--;
		allowedTokensWithoutInfo--;

	}
	return "endOfTheLinePal."; //as long as the input is never this it shouldn't be an issue
}




std::string startingFenString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
std::string inputFenString{};

/*
void fenToGamestate(std::string fenString) {
	int stringPlace = 0;
	int position;
	bool boardSet = false;
	int rank = 0;
	int file = 7;
	int piecesLeftInRank = 8;
	int skips = 0;
	int piecesSkipped = 0;
	std::string boardPieces = inputParser(fenString, 0);
	std::cout << "board pieces: " << boardPieces << "\n";
	int charactersToGoThrough = boardPieces.length();

	while (!boardSet) {
		position = (file * 8) + rank;
		int fenPart = fenString[stringPlace];
		switch (fenPart) {
		case('1'):
		case('2'):
		case('3'):
		case('4'):
		case('5'):
		case('6'):
		case('7'):
		case('8'): //this is to rectify the char int being the same value as an int
			switch (fenPart)gonna change this to stoi when I figure out how that works {
			case('1'):
				skips = 1;
				break;
			case('2'):
				skips = 2;
				break;
			case('3'):
				skips = 3;
				break;
			case('4'):
				skips = 4;
				break;
			case('5'):
				skips = 5;
				break;
			case('6'):
				skips = 6;
				break;
			case('7'):
				skips = 7;
				break;
			case('8'):
				skips = 8;
				break;
			}
			for (; skips >= 0; skips--) {
				currentBoard.square[position] = piece::none;
				rank++;
				piecesSkipped++;
				std::cout << piecesSkipped << " pieces skipped\n";
			}
			break;
		case('/'):
			rank = 0;
			file--;
			break;
		case('K'):
			currentBoard.whiteKingBitBoard |= (1ULL << position);
			currentBoard.square[position] = piece::white + piece::king;
			rank++;
			break;
		case('P'):
			currentBoard.whitePawnBitBoard |= (1ULL << position);
			currentBoard.square[position] = piece::white + piece::pawn;
			rank++;
			std::cout << "white pawn added on position " << position << "\n";
			break;
		case('N'):
			currentBoard.whiteKnightBitBoard |= (1ULL << position);
			currentBoard.square[position] = piece::white + piece::knight;
			rank++;
			break;
		case('B'):
			currentBoard.whiteBishopBitBoard |= (1ULL << position);
			currentBoard.square[position] = piece::white + piece::bishop;
			rank++;
			break;
		case('R'):
			currentBoard.whiteRookBitBoard |= (1ULL << position);
			currentBoard.square[position] = piece::white + piece::rook;
			rank++;
			break;
		case('Q'):
			currentBoard.whiteQueenBitBoard |= (1ULL << position);
			currentBoard.square[position] = piece::white + piece::queen;
			rank++;
			break;
		case('k'):
			currentBoard.blackKingBitBoard |= (1ULL << position);
			currentBoard.square[position] = piece::black + piece::king;
			rank++;
			break;
		case('p'):
			currentBoard.blackPawnBitBoard |= (1ULL << position);
			currentBoard.square[position] = piece::black + piece::pawn;
			rank++;
			break;
		case('n'):
			currentBoard.blackKnightBitBoard |= (1ULL << position);
			currentBoard.square[position] = piece::black + piece::knight;
			rank++;
			break;
		case('b'):
			currentBoard.blackBishopBitBoard |= (1ULL << position);
			currentBoard.square[position] = piece::black + piece::bishop;
			rank++;
			break;
		case('r'):
			currentBoard.blackRookBitBoard |= (1ULL << position);
			currentBoard.square[position] = piece::black + piece::rook;
			rank++;
			break;
		case('q'):
			currentBoard.blackQueenBitBoard |= (1ULL << position);
			currentBoard.square[position] = piece::black + piece::queen;
			rank++;
			break;
		default:
			break;
		}
		charactersToGoThrough--;
		if (charactersToGoThrough == 0)
			boardSet = true;
		stringPlace++;
	}
	std::cout << "board set!" << "\n";

	fenString[stringPlace] == 'w' ? currentBoard.whiteToMove = true : currentBoard.whiteToMove = false;
	stringPlace++;

	std::cout << "color to move set!" << "\n";


	std::string castles = inputParser(fenString, 2);
	int castlesLength = castles.length();

	stringPlace++;
	for (; castlesLength != 0; castlesLength--) {
		switch (fenString[stringPlace]) {
		case 'K':
			currentBoard.whiteShortCastle = true;
		case 'Q':
			currentBoard.whiteShortCastle = true;
		case 'k':
			currentBoard.blackShortCastle = true;
		case 'q':
			currentBoard.blackLongCastle = true;
		case '-':
			break;
		default:
			break;
		}
		stringPlace++;
	}

	std::cout << "castling set!" << "\n";


	int fileLetterAdd = 0;
	int rankNumberMultiply = 0;

	stringPlace++;
	if (fenString[stringPlace] != '-') {
		switch (fenString[stringPlace]) {
		case 'a':
			fileLetterAdd = 0;
			break;
		case 'b':
			fileLetterAdd = 1;
			break;
		case 'c':
			fileLetterAdd = 2;
			break;
		case 'd':
			fileLetterAdd = 3;
			break;
		case 'e':
			fileLetterAdd = 4;
			break;
		case 'f':
			fileLetterAdd = 5;
			break;
		case 'g':
			fileLetterAdd = 6;
			break;
		case 'h':
			fileLetterAdd = 7;
			break;
		}
		stringPlace++;
		switch (fenString[stringPlace]) again the char ints aren't the same as *int* ints  {
		case('1'):
			rankNumberMultiply = 0;
			break;
		case('2'):
			rankNumberMultiply = 1;
			break;
		case('3'):
			rankNumberMultiply = 2;
			break;
		case('4'):
			rankNumberMultiply = 3;
			break;
		case('5'):
			rankNumberMultiply = 4;
			break;
		case('6'):
			rankNumberMultiply = 5;
			break;
		case('7'):
			rankNumberMultiply = 6;
			break;
		case('8'):
			rankNumberMultiply = 7;
			break;
		}



	}

	int enPassantSquare = (rankNumberMultiply * 8) + fileLetterAdd;
	currentBoard.enPassant |= (1ULL << enPassantSquare);
	std::cout << "ready to go!!!" << "\n";
	// there's more for the half clock and full move counters but I don't think the engine has to worry about those (the gui deals with that)
}
*/

/*
void moveCollector(std::string input, int movePlace) {
	std::string moveString;

	do {
		moveString = inputParser(input, movePlace);
		//have to take the moves, decode the string, and encode them into an int that are in an array 
		//I think I can read the rank and file and put them into 3 bits each and have an extra 2 for promotions
		int readLength = moveString.length() - 1;
		int readNumber = 0;
		while (readNumber <= readLength) {
			char toRead = moveString[readNumber];
			if (readNumber != 4) {
				switch (toRead) {
				case 'a':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000000000 : currentBoard.moves[currentBoard.movesPassed] += 0b000000000000000;
					break;
				case 'b':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000000001 : currentBoard.moves[currentBoard.movesPassed] += 0b000000001000000;
					break;
				case 'c':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000000010 : currentBoard.moves[currentBoard.movesPassed] += 0b000000010000000;
					break;
				case 'd':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000000011 : currentBoard.moves[currentBoard.movesPassed] += 0b000000011000000;
					break;
				case 'e':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000000100 : currentBoard.moves[currentBoard.movesPassed] += 0b000000100000000;
					break;
				case 'f':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000000101 : currentBoard.moves[currentBoard.movesPassed] += 0b000000101000000;
					break;
				case 'g':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000000110 : currentBoard.moves[currentBoard.movesPassed] += 0b000000110000000;
					break;
				case 'h':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000000111 : currentBoard.moves[currentBoard.movesPassed] += 0b000000111000000;
					break;
				case '1':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000000000 : currentBoard.moves[currentBoard.movesPassed] += 0b000000000000000;
					break;
				case '2':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000001000 : currentBoard.moves[currentBoard.movesPassed] += 0b000001000000000;
					break;
				case '3':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000010000 : currentBoard.moves[currentBoard.movesPassed] += 0b000010000000000;
					break;
				case '4':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000011000 : currentBoard.moves[currentBoard.movesPassed] += 0b000011000000000;
					break;
				case '5':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000100000 : currentBoard.moves[currentBoard.movesPassed] += 0b000100000000000;
					break;
				case '6':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000101000 : currentBoard.moves[currentBoard.movesPassed] += 0b000101000000000;
					break;
				case '7':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000110000 : currentBoard.moves[currentBoard.movesPassed] += 0b000110000000000;
					break;
				case '8':
					readNumber <= 1 ? currentBoard.moves[currentBoard.movesPassed] += 0b000000000111000 : currentBoard.moves[currentBoard.movesPassed] += 0b000111000000000;
					break;
				}
			}
			else {
				switch (toRead) this will just be promotions {
				case 'q':
					currentBoard.moves[currentBoard.movesPassed] += 0b110000000000000;
					break;
				case 'r':
					currentBoard.moves[currentBoard.movesPassed] += 0b101000000000000;
					break;
				case 'b':
					currentBoard.moves[currentBoard.movesPassed] += 0b100000000000000;
					break;
				case 'n':
					currentBoard.moves[currentBoard.movesPassed] += 0b011000000000000;
					break;
				}
			}
			readNumber++;
		}
		movePlace++;
		currentBoard.movesPassed++;
	} while (moveString != "endOfTheLinePal.");

}
*/

bool normalPiece[13] = { false, true, false, true, true, true, true, true, false, true, true, true, true };
bool majorPiece[13] = { false, true, false, false, false, true, true, true, false, false, false, true, true };
bool minorPiece[13] = { false, false, false, true, true, false, false, false, false, true, true, false, false };
int pieceColor[13] = { none, white, white, white, white, white, white, black, black, black, black , black, black };
int pieceValue[13] = { 0, 2147483648, 100, 300, 315, 500, 900, 2147483648, 100, 300, 315, 500, 900 };

void updateListsMaterial(boardStructure* position) {
	int piece;
	int square;
	int color;
	int i;

	for (i = 0; i < BRD_SQ_NUM; i++) {
		square = i;
		piece = position->pieces[i];
		if (piece != noSquare && piece != empty) {
			
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
		
		}
	}
}

/*
//got these from sebastian lagues vid :D
int pawnValue = 100;
int bishopValue = 300;
int knightValue = 300;
int rookValue = 500;
int queenValue = 900;


int countPieceMaterial() {
	int material = 0;
	int	piece = 0;
	int side = 0;
	for (int i = 0; i <= 63; i++) {
		piece = currentBoard.square[i] & 0b00111;
		side = currentBoard.square[i] & 0b11000;
		switch (piece) {
		case piece::none:
			break;
		case piece::pawn:
			side == 1 ? material += pawnValue : material += -pawnValue;
			break;
		case piece::bishop:
			side == 1 ? material += bishopValue : material += -bishopValue;
			break;
		case piece::knight:
			side == 1 ? material += knightValue : material += -knightValue;
			break;
		case piece::rook:
			side == 1 ? material += rookValue : material += -rookValue;
			break;
		case piece::queen:
			side == 1 ? material += queenValue : material += -queenValue;
			break;
		}
	}
	return material;
}
*/



void resetBoard(boardStructure* position) {
	int i;

	for (i = 0; i < BRD_SQ_NUM; i++) {
		position->pieces[i] = noSquare;
	}

	for (i = 0; i < 64; i++) {
		position->pieces[SQ120(i)] = empty; // this converts the index into the actual play board, so the play board is reset to empty
	}

	for (i = 0; i < 3; i++) {
		position->pieces[i] = 0;
		position->majorPieces[i] = 0;
		position->minorPieces[i] = 0;
		position->nonPieces[i] = 0;

		position->kings[i] = 0ULL;
		position->pawns[i] = 0ULL;
		position->bishops[i] = 0ULL;
		position->knights[i] = 0ULL;
		position->rooks[i] = 0ULL;
		position->queens[i] = 0ULL;

		position->kings[i] = 0;
		position->bishops[i] = 0;
		position->knights[i] = 0;
		position->rooks[i] = 0;
		position->queens[i] = 0;


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

void parseFen(const char* fen, boardStructure* position) {
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
		if (position->castlePermission <= 0 && position->castlePermission >= 15) {
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

//precomputed move table
//got this idea from sebastian lauges vid cause I had no idea what to do
bool moveTableDiagonal[64][64];
bool moveTableCardinal[64][64];

//doesn't matter if this is slow, it'll only be ran once at startup
void computeMoveBoards() {
	int currentRank = 0;
	int currentFile = 0;
	int testRank = 0;
	int testFile = 0;
	int testPosition = 0;
	bool testComplete = false;

	for (int boardArray = 0; boardArray <= 63; boardArray++) {

		testRank = currentRank;
		testFile = currentFile;
		testPosition = 0;
		testComplete = false;


		for (int diagonalArray = 0; diagonalArray <= 7; diagonalArray++) {

			//nw
			while (testComplete != true) {
				testRank += 1;
				testFile -= 1;
				testPosition = (testRank * 8) + testFile;
				if (testRank <= 7 && testRank >= 0 && testFile <= 7 && testFile >= 0) {
					moveTableDiagonal[boardArray][testPosition] = true;
				}
				else {
					moveTableDiagonal[boardArray][testPosition] = false;
					testRank = currentRank;
					testFile = currentFile;
					testComplete = true;
				}
			}

			//ne
			testComplete = false;
			while (testComplete != true) {
				testRank += 1;
				testFile += 1;
				testPosition = (testRank * 8) + testFile;
				if (testRank <= 7 && testRank >= 0 && testFile <= 7 && testFile >= 0) {
					moveTableDiagonal[boardArray][testPosition] = true;
				}
				else {
					moveTableDiagonal[boardArray][testPosition] = false;
					testRank = currentRank;
					testFile = currentFile;
					testComplete = true;
				}
			}
			//se
			testComplete = false;
			while (testComplete != true) {
				testRank -= 1;
				testFile += 1;
				testPosition = (testRank * 8) + testFile;
				if (testRank <= 7 && testRank >= 0 && testFile <= 7 && testFile >= 0) {
					moveTableDiagonal[boardArray][testPosition] = true;
				}
				else {
					moveTableDiagonal[boardArray][testPosition] = false;
					testRank = currentRank;
					testFile = currentFile;
					testComplete = true;
				}
			}
			//sw
			testComplete = false;
			while (testComplete != true) {
				testRank -= 1;
				testFile -= 1;
				testPosition = (testRank * 8) + testFile;
				if (testRank <= 7 && testRank >= 0 && testFile <= 7 && testFile >= 0) {
					moveTableDiagonal[boardArray][testPosition] = true;
				}
				else {
					moveTableDiagonal[boardArray][testPosition] = false;
					testRank = currentRank;
					testFile = currentFile;
					testComplete = true;
				}
			}

		}

		for (int cardinalArray = 0; cardinalArray <= 7; cardinalArray++) {
			//n
			testRank = currentRank;
			testFile = currentFile;
			testComplete = false;

			while (testComplete != true) {
				testRank += 1;
				testPosition = (testRank * 8) + testFile;
				if (testRank <= 7 && testRank >= 0 && testFile <= 7 && testFile >= 0) {
					moveTableCardinal[boardArray][testPosition] = true;
				}
				else {
					moveTableCardinal[boardArray][testPosition] = false;
					testRank = currentRank;
					testFile = currentFile;
					testComplete = true;
				}
			}

			//e
			testComplete = false;
			while (testComplete != true) {
				testFile += 1;
				testPosition = (testRank * 8) + testFile;
				if (testRank <= 7 && testRank >= 0 && testFile <= 7 && testFile >= 0) {
					moveTableCardinal[boardArray][testPosition] = true;
				}
				else {
					moveTableCardinal[boardArray][testPosition] = false;
					testRank = currentRank;
					testFile = currentFile;
					testComplete = true;
				}
			}
			//s
			testComplete = false;
			while (testComplete != true) {
				testRank -= 1;
				testPosition = (testRank * 8) + testFile;
				if (testRank <= 7 && testRank >= 0 && testFile <= 7 && testFile >= 0) {
					moveTableCardinal[boardArray][testPosition] = true;
				}
				else {
					moveTableCardinal[boardArray][testPosition] = false;
					testRank = currentRank;
					testFile = currentFile;
					testComplete = true;
				}
			}
			//w
			testComplete = false;
			while (testComplete != true) {
				testFile -= 1;
				testPosition = (testRank * 8) + testFile;
				if (testRank <= 7 && testRank >= 0 && testFile <= 7 && testFile >= 0) {
					moveTableCardinal[boardArray][testPosition] = true;
				}
				else {
					moveTableCardinal[boardArray][testPosition] = false;
					testRank = currentRank;
					testFile = currentFile;
					testComplete = true;
				}
			}
		}

		if (currentFile <= 7) {
			currentFile++;
		}
		else {
			currentRank++;
			currentFile = 0;
		}
	}
}

void testMoveBoard(int moveTable, int boardArray) {
	int place = 0;
	if (moveTable == 0) {
		for (int rank = 7; rank >= 0; rank--) {
			for (int file = 7; file >= 0; file--) {
				place = (rank * 8) + file; //what I think is happening is I keep switching between the bottom left and the top left being 0
				if (moveTableDiagonal[boardArray][place])
					std::cout << "1";
				else
					std::cout << "0";
			}
			std::cout << "\n";
		}
	}
	else if (moveTable == 1) {
		for (int rank = 7; rank >= 0; rank--) {
			for (int file = 7; file >= 0; file--) {
				place = (rank * 8) + file;
				if (moveTableCardinal[boardArray][place])
					std::cout << "1";
				else
					std::cout << "0";
			}
			std::cout << "\n";
		}
	}

}

/*
int moveTable;
cout << "what move table do you want to look at ";
cin >> moveTable;
int boardArray;
cout << "what array do you want to look at ";
cin >> boardArray;
testMoveBoard(moveTable, boardArray);
*/

/*
bool pseudoLegalChecker(int from, int to, bool whitesTurn, bool enPassant[]) {

	bool onEvenSquare = from % 2;
	bool toEvenSquare = to % 2;
	bool slidingMovePossible = false;
	int slidingCheck = 0;

	if (whitesTurn) {
		switch (currentBoard.square[from]) {
		case 9:  //piece::white && piece::king
			switch (from - to) {
			case -1:
			case -7:
			case -8:
			case -9:
			case 1:
			case 7:
			case 8:
			case 9:
				switch (currentBoard.square[to]) {
				case 0:
				case 17:
				case 18:
				case 19:
				case 20:
				case 21:
				case 22:
					return true;
				default:
					return false;
				}
			default:
				return false;
			}

		case 10: //piece::white && piece::pawn
			switch (to - from) {
			case 8:
				switch (currentBoard.square[to]) {
				case 0:
					return true;
				default:
					return false;
				}
			case 16: // this is the first double move
				switch (from) { // for the first row of pawns
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
					switch (currentBoard.square[to]) {
					case 0:
						return true;
					default:
						return false;
					}
				default:
					return false;
				}

			case 7: //captures here
			case 9:
				switch (currentBoard.square[to]) {
				case 17:
				case 18:
				case 19:
				case 20:
				case 21:
				case 22:
					return true;
				default:
					if (enPassant[to])
						return true;
					else
						return false;
				}
			default:
				return false;
			}
		case 11: //piece::white && piece::knight:
			switch (from - to) {
			case -17:
			case -15:
			case -10:
			case -6:
			case 6:
			case 10:
			case 15:
			case 17:
				switch (currentBoard.square[to]) {
				case 0:
				case 17:
				case 18:
				case 19:
				case 20:
				case 21:
				case 22:
					return true;
				default:
					return false;
				}
			default:
				return false;
			}
		case 12://piece::white && piece::bishop:

			if (!onEvenSquare == toEvenSquare) //I think this is a pretty cool optimization, because bishops can only move to a square that is the same color, I'll have to include this in the actual legal move function
				return false;
			else {
				if (moveTableDiagonal[from][to])
					slidingMovePossible;
			}
			if (slidingMovePossible) {
				bool pieceInTheWayNW = false;
				bool pieceInTheWayNE = false;
				bool pieceInTheWaySW = false;
				bool pieceInTheWaySE = false;
				bool legalMove = false;
				int targetSquare = to;
				int toGoToTest = from; // might be able to remove this and just test the from, but will test that after I can confirm it by tying commands

				bool directionFound = false;
				int direction = 0; //nw will be 1, ne 2, sw 3, and se 4
				int infiniteLoopBreaker = 10000;
				while (!directionFound) {
					int NWtest = from;
					int NEtest = from;
					int SWtest = from;
					int SEtest = from;
					NWtest += 7;
					NEtest += 9;
					SWtest -= 9;
					SEtest -= 7;
					infiniteLoopBreaker--;

					if (NWtest == to) {
						directionFound = true;
						direction = 1;
					}
					if (NEtest == to) {
						directionFound = true;
						direction = 2;
					}
					if (SWtest == to) {
						directionFound = true;
						direction = 3;
					}
					if (SEtest == to) {
						directionFound = true;
						direction = 4;
					}
					if (infiniteLoopBreaker == 0) {
						std::cout << "oops, something caused an infinite loop in the bishop legal move checker :P" << "\n";
					}
				}
				//later I'll optimize this, it'll run the second branch even if the solution was on the first branch
				//this function is important to be optimal because it'll be run for every piece on every board
				if (direction == 1) {
					while (!pieceInTheWayNW) {
						toGoToTest += 7;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayNW = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayNW = true;
						}

					}
				}
				if (direction == 2) {
					while (!pieceInTheWayNE) {
						toGoToTest += 9;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayNE = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayNE = true;
						}

					}
				}
				if (direction == 3) {
					while (!pieceInTheWaySW) {
						toGoToTest -= 9;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWaySW = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWaySW = true;
						}
					}
				}
				if (direction == 4) {
					while (!pieceInTheWaySE) {
						toGoToTest -= 7;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWaySE = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWaySE = true;
						}

					}
				}
				if (!legalMove) captures {
					switch (currentBoard.square[to]) {
					case 17:
					case 18: //don't have to put the none because it'll already be 'legalMove' if it is none, from the part above
					case 19:
					case 20:
					case 21:
					case 22:
						return true;
					default:
						return false;
					}
				}
				else {
					return true;
				}
			}
			else {
				return false;
			}
		case 13://white + rook
			if (moveTableCardinal[from][to])
				slidingMovePossible;

			if (slidingMovePossible) {
				bool pieceInTheWayN = false;
				bool pieceInTheWayE = false;
				bool pieceInTheWayS = false;
				bool pieceInTheWayW = false;
				bool legalMove = false;
				int targetSquare = to;
				int toGoToTest = from;
				if (from < to) {
					while (!pieceInTheWayN) {
						toGoToTest += 8;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayN = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayN = true;
						}

					}
					while (!pieceInTheWayE) {
						toGoToTest += 1;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayE = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayE = true;
						}

					}

				}
				if (from > to) {
					while (!pieceInTheWayS) {
						toGoToTest -= 8;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayS = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayS = true;
						}
					}
					while (!pieceInTheWayW) {
						toGoToTest -= 1;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayW = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayW = true;
						}

					}
				}
				if (!legalMove) captures {
					switch (currentBoard.square[to]) {
					case 17:
					case 18: //don't have to put the none because it'll already be 'legalMove' if it is none, from the part above
					case 19:
					case 20:
					case 21:
					case 22:
						return true;
					default:
						return false;
					}
				}
				else {
					return true;
				}
			}
			else {
				return false;
			}
		case 14://white + queen
			if (moveTableCardinal[from][to] || moveTableDiagonal) //this is just bishop + rook 
				slidingMovePossible;

			if (slidingMovePossible) {
				bool pieceInTheWayN = false;
				bool pieceInTheWayE = false;
				bool pieceInTheWayS = false;
				bool pieceInTheWayW = false;
				bool pieceInTheWayNW = false;
				bool pieceInTheWayNE = false;
				bool pieceInTheWaySW = false;
				bool pieceInTheWaySE = false;
				bool legalMove = false;
				int targetSquare = to;
				int toGoToTest = from;
				if (from < to) {
					while (!pieceInTheWayE) {
						toGoToTest += 1;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayE = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayE = true;
						}

					}
					while (!pieceInTheWayNW) {
						toGoToTest += 7;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayNW = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayNW = true;
						}

					}
					while (!pieceInTheWayN) {
						toGoToTest += 8;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayN = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayN = true;
						}

					}
					while (!pieceInTheWayNE) {
						toGoToTest += 9;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayNE = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayNE = true;
						}

					}


				}
				if (from > to) {
					while (!pieceInTheWayW) {
						toGoToTest -= 1;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayW = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayW = true;
						}

					}
					while (!pieceInTheWaySE) {
						toGoToTest -= 7;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWaySE = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWaySE = true;
						}

					}
					while (!pieceInTheWayS) {
						toGoToTest -= 8;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayS = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayS = true;
						}
					}
					while (!pieceInTheWaySW) {
						toGoToTest -= 9;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWaySW = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWaySW = true;
						}
					}
				}
				if (!legalMove) captures {
					switch (currentBoard.square[to]) {
					case 17:
					case 18: //don't have to put the none because it'll already be 'legalMove' if it is none, from the part above
					case 19:
					case 20:
					case 21:
					case 22:
						return true;
					default:
						return false;
					}
				}
				else {
					return true;
				}
			}
			else {
				return false;
			}
		default:
			return false;
		}
	}
	else {
		switch (currentBoard.square[from]) {
		case 17: //piece::black && piece::king:
			switch (from - to) {
			case -1:
			case -7:
			case -8:
			case -9:
			case 1:
			case 7:
			case 8:
			case 9:
				switch (currentBoard.square[to]) {
				case 0:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
					return true;
				default:
					return false;
				}
			default:
				return false;
			}
		case 18: //piece::black && piece::pawn:
			switch (to - from) {
			case -8:
				switch (currentBoard.square[to]) {
				case 0:
					return true;
				default:
					return false;
				}
			case -16: // this is the first double move
				switch (from) { // for the first row of pawns
				case 48:
				case 49:
				case 50:
				case 51:
				case 52:
				case 53:
				case 54:
				case 55:
					switch (currentBoard.square[to]) {
					case 0:
						return true;
					default:
						return false;
					}
				default:
					return false;
				}
			case -7: //captures here
			case -9:
				switch (currentBoard.square[to]) {
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
					return true;
				default:
					if (enPassant[to])
						return true;
					else
						return false;
				}

			default:
				return false;
			}
		case 19: //piece::black && piece::knight:
			switch (from - to) {
			case -17:
			case -15:
			case -10:
			case -6:
			case 6:
			case 10:
			case 15:
			case 17:
				switch (currentBoard.square[to]) {
				case 0:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
					return true;
				default:
					return false;
				}
			default:
				return false;
			}
		case 20: //piece::black && piece::bishop:
			if (!onEvenSquare == toEvenSquare) //I think this is a pretty cool optimization, because bishops can only move to a square that is the same color, I'll have to include this in the actual legal move function
				return false;
			else {
				if (moveTableDiagonal[from][to])
					slidingMovePossible;
			}
			if (slidingMovePossible) {
				bool pieceInTheWayNW = false;
				bool pieceInTheWayNE = false;
				bool pieceInTheWaySW = false;
				bool pieceInTheWaySE = false;
				bool legalMove = false;
				int targetSquare = to;
				int toGoToTest = from;
				if (from < to) {
					while (!pieceInTheWayNW) {
						toGoToTest += 7;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayNW = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayNW = true;
						}

					}
					while (!pieceInTheWayNE) {
						toGoToTest += 9;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayNE = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayNE = true;
						}

					}

				}
				if (from > to) {
					while (!pieceInTheWaySW) {
						toGoToTest -= 9;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWaySW = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWaySW = true;
						}
					}
					while (!pieceInTheWaySE) {
						toGoToTest -= 7;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWaySE = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWaySE = true;
						}

					}
				}
				if (!legalMove) captures {
					switch (currentBoard.square[to]) {
					case 9: //don't have to put the none because it'll already be 'legalMove' if it is none, from the part above
					case 10:
					case 11:
					case 12:
					case 13:
					case 14:
						return true;
					default:
						return false;
					}
				}
				else {
					return true;
				}
			}
			else {
				return false;
			}
		case 21: //piece::black && piece::rook:
			if (moveTableCardinal[from][to])
				slidingMovePossible;

			if (slidingMovePossible) {
				bool pieceInTheWayN = false;
				bool pieceInTheWayE = false;
				bool pieceInTheWayS = false;
				bool pieceInTheWayW = false;
				bool legalMove = false;
				int targetSquare = to;
				int toGoToTest = from;
				if (from < to) {
					while (!pieceInTheWayN) {
						toGoToTest += 8;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayN = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayN = true;
						}

					}
					while (!pieceInTheWayE) {
						toGoToTest += 1;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayE = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayE = true;
						}

					}

				}
				if (from > to) {
					while (!pieceInTheWayS) {
						toGoToTest -= 8;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayS = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayS = true;
						}
					}
					while (!pieceInTheWayW) {
						toGoToTest -= 1;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayW = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayW = true;
						}

					}
				}
				if (!legalMove) captures {
					switch (currentBoard.square[to]) {
					case 9: //don't have to put the none because it'll already be 'legalMove' if it is none, from the part above
					case 10:
					case 11:
					case 12:
					case 13:
					case 14:
						return true;
					default:
						return false;
					}
				}
				else {
					return true;
				}
			}
			else {
				return false;
			}
		case 22: //piece::black && piece::queen:
			if (moveTableCardinal[from][to] || moveTableDiagonal) //this is just bishop + rook 
				slidingMovePossible;

			if (slidingMovePossible) {
				bool pieceInTheWayN = false;
				bool pieceInTheWayE = false;
				bool pieceInTheWayS = false;
				bool pieceInTheWayW = false;
				bool pieceInTheWayNW = false;
				bool pieceInTheWayNE = false;
				bool pieceInTheWaySW = false;
				bool pieceInTheWaySE = false;
				bool legalMove = false;
				int targetSquare = to;
				int toGoToTest = from;
				if (from < to) {
					while (!pieceInTheWayE) {
						toGoToTest += 1;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayE = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayE = true;
						}

					}
					while (!pieceInTheWayNW) {
						toGoToTest += 7;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayNW = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayNW = true;
						}

					}
					while (!pieceInTheWayN) {
						toGoToTest += 8;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayN = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayN = true;
						}

					}
					while (!pieceInTheWayNE) {
						toGoToTest += 9;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayNE = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayNE = true;
						}

					}


				}
				if (from > to) {
					while (!pieceInTheWayW) {
						toGoToTest -= 1;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayW = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayW = true;
						}

					}
					while (!pieceInTheWaySE) {
						toGoToTest -= 7;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWaySE = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWaySE = true;
						}

					}
					while (!pieceInTheWayS) {
						toGoToTest -= 8;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWayS = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWayS = true;
						}
					}
					while (!pieceInTheWaySW) {
						toGoToTest -= 9;
						if (!currentBoard.square[toGoToTest] == piece::none) {
							pieceInTheWaySW = false;
							if (toGoToTest == to) {
								legalMove = true;
								return true;
							}
						}
						else {
							pieceInTheWaySW = true;
						}
					}
				}
				if (!legalMove) captures {
					switch (currentBoard.square[to]) {
					case 9: //don't have to put the none because it'll already be 'legalMove' if it is none, from the part above
					case 10:
					case 11:
					case 12:
					case 13:
					case 14:
						return true;
					default:
						return false;
					}
				}
				else {
					return true;
				}
			}
			else {
				return false;
			}
		}


	}

	return false;


}
*/

bool checkLegalMove(int board[], int moveId) {
	//uuuuuuuuuuuuuuuh idk how to do this
	return false;
}

//here I'll make a thing that prints a screen for the legal moves that a piece can make, later I'll have a thing to print the board
/*
void printMovesForPiece(int from, bool whitesTurn, bool enPassant[]) {
	int place = 0;
	for (int rank = 7; rank >= 0; rank--) {
		for (int file = 7; file >= 0; file--) {
			place = (rank * 8) + file;
			if (pseudoLegalChecker(from, place, whitesTurn, enPassant))
				std::cout << "1";
			else
				std::cout << "0";
		}
		std::cout << "\n";
	}
}
*/

/*	int pieceToTest;
cout << "what piece do you want to see the moves for ";
cin >> pieceToTest;
printMovesForPiece(pieceToTest, currentBoard.whiteToMove, currentBoard.enPassant);
*/

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
			} else {
				std::cout << "-";
			}
		}
		std::cout << "\n";
	}

}

char pieceCharacter[] = ".KPNBRQkpnbrq";
char sideCharacter[] = "wb-";
char rankCharacter[] = "12345678";
char fileCharacter[] = "abcdefgh";
char printCastle;

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
		printf("%c",'a' + file);
		printf(" ");
		//std::cout << " " << "a" + file;
	}
	std::cout << "\n-----------------------\n";
	std::cout << "\n";

	std::cout << "side: " << sideCharacter[position->side] << "\n";
	printf("en Passant:%d\n", position->enPassant);
	//std::cout << "en Passant: " << position->enPassant << "\n";
	std::cout << "castle permission:\n";
	position->castlePermission & whiteKingCastle ? printCastle = 'K' : printCastle = '-';
	std::cout << "K: " << printCastle << "\n";
	position->castlePermission & whiteQueenCastle ? printCastle = 'Q' : printCastle = '-';
	std::cout << "Q: " << printCastle << "\n";
	position->castlePermission & blackKingCastle ? printCastle = 'k' : printCastle = '-';
	std::cout << "k: " << printCastle << "\n";
	position->castlePermission & blackQueenCastle ? printCastle = 'q' : printCastle = '-';
	std::cout << "q: " << printCastle << "\n"; //this is really ugly

	//this is the only one that I'm gonna do printf because idk how to make character out do hex
	std::cout << "position key:\n" << std::hex << position->positionKey;
	
	
	std::cout << "\n-----------------------\n";
}

void initializeAll() {
	computeMoveBoards();
	initializeSquare120ToSquare64();
	initializeBitMasks();
	initializeHashKeys();
}

bool uci = false;
bool keepRunning = true;
bool boardLoaded = false;
//what would be cool is if I could somehow make it learn every time you play it so I can set it up to learn against other engines with cutechess
//I think I could just do that by making a nn that changes the move order possibly, because with ab pruning if the best move is first the search will be extremely fast
int main()
{
	using std::cout;
	using std::cin;
	cout << "gamer engine made by flipwonderland" << "\n";

	initializeAll();
	boardStructure currentBoard[1];

	do {
		std::string input;
		std::getline(cin, input);
		//std::getline(cin >> std::ws, input);
		std::string command = inputParser(input, 0);

		if (command == "uci")/*should turn this into a switch*/ {
			uci = true;
			//clearGameState();
			cout << "id name flipgine" << "\n";
			cout << "id author flip! (duh)" << "\n";
			cout << "uciok" << "\n";
		}
		else if (command == "debug") {
			//printBitBoard(currentBoard.whitePawnBitBoard);
			parseFen(STARTFEN, currentBoard);
			printSquareBoard(currentBoard);

			parseFen(TESTFEN, currentBoard);
			printSquareBoard(currentBoard);

			parseFen(TESTFEN2, currentBoard);
			printSquareBoard(currentBoard);

			parseFen(TESTFEN3, currentBoard);
			printSquareBoard(currentBoard);
		}
		else if (command == "isready") {
			//see if it's ready to run and then
			if (inputParser(input, 1) == "hi") cout << "I see you! \n";
			cout << "readyok" << "\n";
		}
		else if (command == "setoption name") /*gotta fix this, this is 2 tokens in one*/ {
			// if I figure out how to do multithreading I'll put a command here
		}
		else if (command == "register") {
			//idk if I'm gonna need this one actually
		}
		else if (command == "ucinewgame") {
			//clearGameState(); //this might create unexpected behavior if the gui does not send this every time
			//boardLoaded = false;
			//cout << "new game ready to be loaded!" << "\n";
		}
		else if (command == "position") /*position [fen | startpos]  moves  ....*/ {
			/*if (inputParser(input, 1) == "startpos") {
				cout << "startpos chosen \n";
				fenToGamestate(startingFenString);
				boardLoaded = true;
			}
			else {
				std::string positionChosen = inputParser(input, 1);
				cout << "custom FEN chosen, FEN is: " << positionChosen << "\n";
				fenToGamestate(input);
				int movePlace = 7; //7 is the end of the fen string, so if there's moves this will be the first one
				moveCollector(input, movePlace); //moveplace is the start of the move tokens\

				for (int i = 0; i <= currentBoard.movesPassed; i++) {
					currentBoard.move(currentBoard.moves[i]);
				}
				boardLoaded = true;
			}
			if (boardLoaded) {
				cout << "board loaded!" << "\n";
			}
			*/
		}
		else if (command == "go") {

		}
		else if (command == "stop") {

		}
		else if (command == "ponderhit") {

		}
		else if (command == "quit")
			keepRunning = false;
		else if (command == "legalCheck") {
			if (boardLoaded) {
			}
			else {
				cout << "board is not loaded!" << "\n";
			}
		}
		else {
			cout << "unknown command, try again. command: " << command << "\n";
			cout << "second command: " << inputParser(input, 1) << "\n";
			cout << "full input: " << input << "\n";
		}
	} while (keepRunning);
	return 0;
}


/*
The UCI protocol as publiced by Stefan-Meyer Kahlen (ShredderChess):

Description of the universal chess interface (UCI)    April 2004
================================================================

* all communication is done via standard input and output with text commands,

* The engine should boot and wait for input from the GUI,
  the engine should wait for the "isready" or "setoption" command to set up its internal parameters
  as the boot process should be as quick as possible.

* the engine must always be able to process input from stdin, even while thinking.

* all command strings the engine receives will end with '\n',
  also all commands the GUI receives should end with '\n',
  Note: '\n' can be 0x0c or 0x0a0c or any combination depending on your OS.
  If you use Engine und GUI in the same OS this should be no problem if you cummunicate in text mode,
  but be aware of this when for example running a Linux engine in a Windows GUI.

* The engine will always be in forced mode which means it should never start calculating
  or pondering without receiving a "go" command first.

* Before the engine is asked to search on a position, there will always be a position command
  to tell the engine about the current position.

* by default all the opening book handling is done by the GUI,
  but there is an option for the engine to use its own book ("OwnBook" option, see below)

* if the engine or the GUI receives an unknown command or token it should just ignore it and try to
  parse the rest of the string.

* if the engine receives a command which is not supposed to come, for example "stop" when the engine is
  not calculating, it should also just ignore it.


Move format:
------------

The move format is in long algebraic notation.
A nullmove from the Engine to the GUI should be send as 0000.
Examples:  e2e4, e7e5, e1g1 (white short castling), e7e8q (for promotion)



GUI to engine:
--------------

These are all the command the engine gets from the interface.

* uci
	tell engine to use the uci (universal chess interface),
	this will be send once as a first command after program boot
	to tell the engine to switch to uci mode.
	After receiving the uci command the engine must identify itself with the "id" command
	and sent the "option" commands to tell the GUI which engine settings the engine supports if any.
	After that the engine should sent "uciok" to acknowledge the uci mode.
	If no uciok is sent within a certain time period, the engine task will be killed by the GUI.

* debug [ on | off ]
	switch the debug mode of the engine on and off.
	In debug mode the engine should sent additional infos to the GUI, e.g. with the "info string" command,
	to help debugging, e.g. the commands that the engine has received etc.
	This mode should be switched off by default and this command can be sent
	any time, also when the engine is thinking.

* isready
	this is used to synchronize the engine with the GUI. When the GUI has sent a command or
	multiple commands that can take some time to complete,
	this command can be used to wait for the engine to be ready again or
	to ping the engine to find out if it is still alive.
	E.g. this should be sent after setting the path to the tablebases as this can take some time.
	This command is also required once before the engine is asked to do any search
	to wait for the engine to finish initializing.
	This command must always be answered with "readyok" and can be sent also when the engine is calculating
	in which case the engine should also immediately answer with "readyok" without stopping the search.

* setoption name  [value ]
	this is sent to the engine when the user wants to change the internal parameters
	of the engine. For the "button" type no value is needed.
	One string will be sent for each parameter and this will only be sent when the engine is waiting.
	The name of the option in  should not be case sensitive and can inludes spaces like also the value.
	The substrings "value" and "name" should be avoided in  and  to allow unambiguous parsing,
	for example do not use  = "draw value".
	Here are some strings for the example below:
	   "setoption name Nullmove value true\n"
	  "setoption name Selectivity value 3\n"
	   "setoption name Style value Risky\n"
	   "setoption name Clear Hash\n"
	   "setoption name NalimovPath value c:\chess\tb\4;c:\chess\tb\5\n"

* register
	this is the command to try to register an engine or to tell the engine that registration
	will be done later. This command should always be sent if the engine	has send "registration error"
	at program startup.
	The following tokens are allowed:
	* later
	   the user doesn't want to register the engine now.
	* name
	   the engine should be registered with the name
	* code
	   the engine should be registered with the code
	Example:
	   "register later"
	   "register name Stefan MK code 4359874324"

* ucinewgame
   this is sent to the engine when the next search (started with "position" and "go") will be from
   a different game. This can be a new game the engine should play or a new game it should analyse but
   also the next position from a testsuite with positions only.
   If the GUI hasn't sent a "ucinewgame" before the first "position" command, the engine shouldn't
   expect any further ucinewgame commands as the GUI is probably not supporting the ucinewgame command.
   So the engine should not rely on this command even though all new GUIs should support it.
   As the engine's reaction to "ucinewgame" can take some time the GUI should always send "isready"
   after "ucinewgame" to wait for the engine to finish its operation.

* position [fen  | startpos ]  moves  ....
	set up the position described in fenstring on the internal board and
	play the moves on the internal chess board.
	if the game was played  from the start position the string "startpos" will be sent
	Note: no "new" command is needed. However, if this position is from a different game than
	the last position sent to the engine, the GUI should have sent a "ucinewgame" inbetween.

* go
	start calculating on the current position set up with the "position" command.
	There are a number of commands that can follow this command, all will be sent in the same string.
	If one command is not send its value should be interpreted as it would not influence the search.
	* searchmoves  ....
		restrict search to this moves only
		Example: After "position startpos" and "go infinite searchmoves e2e4 d2d4"
		the engine should only search the two moves e2e4 and d2d4 in the initial position.
	* ponder
		start searching in pondering mode.
		Do not exit the search in ponder mode, even if it's mate!
		This means that the last move sent in in the position string is the ponder move.
		The engine can do what it wants to do, but after a "ponderhit" command
		it should execute the suggested move to ponder on. This means that the ponder move sent by
		the GUI can be interpreted as a recommendation about which move to ponder. However, if the
		engine decides to ponder on a different move, it should not display any mainlines as they are
		likely to be misinterpreted by the GUI because the GUI expects the engine to ponder
	   on the suggested move.
	* wtime
		white has x msec left on the clock
	* btime
		black has x msec left on the clock
	* winc
		white increment per move in mseconds if x > 0
	* binc
		black increment per move in mseconds if x > 0
	* movestogo
	  there are x moves to the next time control,
		this will only be sent if x > 0,
		if you don't get this and get the wtime and btime it's sudden death
	* depth
		search x plies only.
	* nodes
	   search x nodes only,
	* mate
		search for a mate in x moves
	* movetime
		search exactly x mseconds
	* infinite
		search until the "stop" command. Do not exit the search without being told so in this mode!

* stop
	stop calculating as soon as possible,
	don't forget the "bestmove" and possibly the "ponder" token when finishing the search

* ponderhit
	the user has played the expected move. This will be sent if the engine was told to ponder on the same move
	the user has played. The engine should continue searching but switch from pondering to normal search.

* quit
	quit the program as soon as possible


Engine to GUI:
--------------

* id
	* name
		this must be sent after receiving the "uci" command to identify the engine,
		e.g. "id name Shredder X.Y\n"
	* author
		this must be sent after receiving the "uci" command to identify the engine,
		e.g. "id author Stefan MK\n"

* uciok
	Must be sent after the id and optional options to tell the GUI that the engine
	has sent all infos and is ready in uci mode.

* readyok
	This must be sent when the engine has received an "isready" command and has
	processed all input and is ready to accept new commands now.
	It is usually sent after a command that can take some time to be able to wait for the engine,
	but it can be used anytime, even when the engine is searching,
	and must always be answered with "isready".

* bestmove  [ ponder  ]
	the engine has stopped searching and found the move  best in this position.
	the engine can send the move it likes to ponder on. The engine must not start pondering automatically.
	this command must always be sent if the engine stops searching, also in pondering mode if there is a
	"stop" command, so for every "go" command a "bestmove" command is needed!
	Directly before that the engine should send a final "info" command with the final search information,
	the the GUI has the complete statistics about the last search.

* copyprotection
	this is needed for copyprotected engines. After the uciok command the engine can tell the GUI,
	that it will check the copy protection now. This is done by "copyprotection checking".
	If the check is ok the engine should sent "copyprotection ok", otherwise "copyprotection error".
	If there is an error the engine should not function properly but should not quit alone.
	If the engine reports "copyprotection error" the GUI should not use this engine
	and display an error message instead!
	The code in the engine can look like this
	  TellGUI("copyprotection checking\n");
	   // ... check the copy protection here ...
	   if(ok)
		  TellGUI("copyprotection ok\n");
	  else
		 TellGUI("copyprotection error\n");

* registration
	this is needed for engines that need a username and/or a code to function with all features.
	Analog to the "copyprotection" command the engine can send "registration checking"
	after the uciok command followed by either "registration ok" or "registration error".
	Also after every attempt to register the engine it should answer with "registration checking"
	and then either "registration ok" or "registration error".
	In contrast to the "copyprotection" command, the GUI can use the engine after the engine has
	reported an error, but should inform the user that the engine is not properly registered
	and might not use all its features.
	In addition the GUI should offer to open a dialog to
	enable registration of the engine. To try to register an engine the GUI can send
	the "register" command.
	The GUI has to always answer with the "register" command	if the engine sends "registration error"
	at engine startup (this can also be done with "register later")
	and tell the user somehow that the engine is not registered.
	This way the engine knows that the GUI can deal with the registration procedure and the user
	will be informed that the engine is not properly registered.

* info
	the engine wants to send infos to the GUI. This should be done whenever one of the info has changed.
	The engine can send only selected infos and multiple infos can be send with one info command,
	e.g. "info currmove e2e4 currmovenumber 1" or
		 "info depth 12 nodes 123456 nps 100000".
	Also all infos belonging to the pv should be sent together
	e.g. "info depth 2 score cp 214 time 1242 nodes 2124 nps 34928 pv e2e4 e7e5 g1f3"
	I suggest to start sending "currmove", "currmovenumber", "currline" and "refutation" only after one second
	to avoid too much traffic.
	Additional info:
	* depth
		search depth in plies
	* seldepth
		selective search depth in plies,
		if the engine sends seldepth there must also a "depth" be present in the same string.
	* time
		the time searched in ms, this should be sent together with the pv.
	* nodes
		x nodes searched, the engine should send this info regularly
	* pv  ...
		the best line found
	* multipv
		this for the multi pv mode.
		for the best move/pv add "multipv 1" in the string when you send the pv.
		in k-best mode always send all k variants in k strings together.
	* score
		* cp
			the score from the engine's point of view in centipawns.
		* mate
			mate in y moves, not plies.
			If the engine is getting mated use negativ values for y.
		* lowerbound
		  the score is just a lower bound.
		* upperbound
		   the score is just an upper bound.
	* currmove
		currently searching this move
	* currmovenumber
		currently searching move number x, for the first move x should be 1 not 0.
	* hashfull
		the hash is x permill full, the engine should send this info regularly
	* nps
		x nodes per second searched, the engine should send this info regularly
	* tbhits
		x positions were found in the endgame table bases
	* cpuload
		the cpu usage of the engine is x permill.
	* string
		any string str which will be displayed be the engine,
		if there is a string command the rest of the line will be interpreted as .
	* refutation   ...
	   move  is refuted by the line  ... , i can be any number >= 1.
	   Example: after move d1h5 is searched, the engine can send
	   "info refutation d1h5 g6h5"
	   if g6h5 is the best answer after d1h5 or if g6h5 refutes the move d1h5.
	   if there is norefutation for d1h5 found, the engine should just send
	   "info refutation d1h5"
		The engine should only send this if the option "UCI_ShowRefutations" is set to true.
	* currline   ...
	   this is the current line the engine is calculating.  is the number of the cpu if
	   the engine is running on more than one cpu.  = 1,2,3....
	   if the engine is just using one cpu,  can be omitted.
	   If  is greater than 1, always send all k lines in k strings together.
		The engine should only send this if the option "UCI_ShowCurrLine" is set to true.


* option
	This command tells the GUI which parameters can be changed in the engine.
	This should be sent once at engine startup after the "uci" and the "id" commands
	if any parameter can be changed in the engine.
	The GUI should parse this and build a dialog for the user to change the settings.
	Note that not every option needs to appear in this dialog as some options like
	"Ponder", "UCI_AnalyseMode", etc. are better handled elsewhere or are set automatically.
	If the user wants to change some settings, the GUI will send a "setoption" command to the engine.
	Note that the GUI need not send the setoption command when starting the engine for every option if
	it doesn't want to change the default value.
	For all allowed combinations see the example below,
	as some combinations of this tokens don't make sense.
	One string will be sent for each parameter.
	* name

		The option has the name id.
		Certain options have a fixed value for , which means that the semantics of this option is fixed.
		Usually those options should not be displayed in the normal engine options window of the GUI but
		get a special treatment. "Pondering" for example should be set automatically when pondering is
		enabled or disabled in the GUI options. The same for "UCI_AnalyseMode" which should also be set
		automatically by the GUI. All those certain options have the prefix "UCI_" except for the
		first 6 options below. If the GUI get an unknown Option with the prefix "UCI_", it should just
		ignore it and not display it in the engine's options dialog.
		*  = Hash, type is spin
			the value in MB for memory for hash tables can be changed,
			this should be answered with the first "setoptions" command at program boot
			if the engine has sent the appropriate "option name Hash" command,
			which should be supported by all engines!
			So the engine should use a very small hash first as default.
		*  = NalimovPath, type string
			this is the path on the hard disk to the Nalimov compressed format.
			Multiple directories can be concatenated with ";"
		*  = NalimovCache, type spin
			this is the size in MB for the cache for the nalimov table bases
			These last two options should also be present in the initial options exchange dialog
			when the engine is booted if the engine supports it
		*  = Ponder, type check
			this means that the engine is able to ponder.
			The GUI will send this whenever pondering is possible or not.
			Note: The engine should not start pondering on its own if this is enabled, this option is only
			needed because the engine might change its time management algorithm when pondering is allowed.
		*  = OwnBook, type check
			this means that the engine has its own book which is accessed by the engine itself.
			if this is set, the engine takes care of the opening book and the GUI will never
			execute a move out of its book for the engine. If this is set to false by the GUI,
			the engine should not access its own book.
		*  = MultiPV, type spin
			the engine supports multi best line or k-best mode. the default value is 1
		*  = UCI_ShowCurrLine, type check, should be false by default,
			the engine can show the current line it is calculating. see "info currline" above.
		*  = UCI_ShowRefutations, type check, should be false by default,
			the engine can show a move and its refutation in a line. see "info refutations" above.
		*  = UCI_LimitStrength, type check, should be false by default,
			The engine is able to limit its strength to a specific Elo number,
		   This should always be implemented together with "UCI_Elo".
		*  = UCI_Elo, type spin
			The engine can limit its strength in Elo within this interval.
			If UCI_LimitStrength is set to false, this value should be ignored.
			If UCI_LimitStrength is set to true, the engine should play with this specific strength.
		   This should always be implemented together with "UCI_LimitStrength".
		*  = UCI_AnalyseMode, type check
		   The engine wants to behave differently when analysing or playing a game.
		   For example when playing it can use some kind of learning.
		   This is set to false if the engine is playing a game, otherwise it is true.
		 *  = UCI_Opponent, type string
		   With this command the GUI can send the name, title, elo and if the engine is playing a human
		   or computer to the engine.
		   The format of the string has to be [GM|IM|FM|WGM|WIM|none] [|none] [computer|human]
		   Example:
		   "setoption name UCI_Opponent value GM 2800 human Gary Kasparow"
		   "setoption name UCI_Opponent value none none computer Shredder"


	* type
		The option has type t.
		There are 5 different types of options the engine can send
		* check
			a checkbox that can either be true or false
		* spin
			a spin wheel that can be an integer in a certain range
		* combo
			a combo box that can have different predefined strings as a value
		* button
			a button that can be pressed to send a command to the engine
		* string
			a text field that has a string as a value,
			an empty string has the value ""
	* default
		the default value of this parameter is x
	* min
		the minimum value of this parameter is x
	* max
		the maximum value of this parameter is x
	* var
		a predefined value of this parameter is x
	Example:
	Here are 5 strings for each of the 5 possible types of options
	   "option name Nullmove type check default true\n"
	  "option name Selectivity type spin default 2 min 0 max 4\n"
	   "option name Style type combo default Normal var Solid var Normal var Risky\n"
	   "option name NalimovPath type string default c:\\n"
	   "option name Clear Hash type button\n"



Example:
--------

This is how the communication when the engine boots can look like:

GUI     engine

// tell the engine to switch to UCI mode
uci

// engine identify
	  id name Shredder
		id author Stefan MK

// engine sends the options it can change
// the engine can change the hash size from 1 to 128 MB
		option name Hash type spin default 1 min 1 max 128

// the engine supports Nalimov endgame tablebases
		option name NalimovPath type string default
		option name NalimovCache type spin default 1 min 1 max 32

// the engine can switch off Nullmove and set the playing style
	   option name Nullmove type check default true
		option name Style type combo default Normal var Solid var Normal var Risky

// the engine has sent all parameters and is ready
		uciok

// Note: here the GUI can already send a "quit" command if it just wants to find out
//       details about the engine, so the engine should not initialize its internal
//       parameters before here.
// now the GUI sets some values in the engine
// set hash to 32 MB
setoption name Hash value 32

// init tbs
setoption name NalimovCache value 1
setoption name NalimovPath value d:\tb;c\tb

// waiting for the engine to finish initializing
// this command and the answer is required here!
isready

// engine has finished setting up the internal values
		readyok

// now we are ready to go

// if the GUI is supporting it, tell the engine that is is
// searching on a game that is hasn't searched on before
ucinewgame

// if the engine supports the "UCI_AnalyseMode" option and the next search is supposted to
// be an analysis, the GUI should set "UCI_AnalyseMode" to true if it is currently
// set to false with this engine
setoption name UCI_AnalyseMode value true

// tell the engine to search infinite from the start position after 1.e4 e5
position startpos moves e2e4 e7e5
go infinite

// the engine starts sending infos about the search to the GUI
// (only some examples are given)


		info depth 1 seldepth 0
		info score cp 13  depth 1 nodes 13 time 15 pv f1b5
		info depth 2 seldepth 2
		info nps 15937
		info score cp 14  depth 2 nodes 255 time 15 pv f1c4 f8c5
		info depth 2 seldepth 7 nodes 255
		info depth 3 seldepth 7
		info nps 26437
		info score cp 20  depth 3 nodes 423 time 15 pv f1c4 g8f6 b1c3
		info nps 41562
		....


// here the user has seen enough and asks to stop the searching
stop

// the engine has finished searching and is sending the bestmove command
// which is needed for every "go" command sent to tell the GUI
// that the engine is ready again
		bestmove g1f3 ponder d8f6
*/