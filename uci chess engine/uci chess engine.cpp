#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

#define _VECTOR_

#include "stdlib.h"
#include "stdio.h"
#include "util.h"
#include "math.h"
#include "string.h"
#include "tinycthread.h"

#ifdef _WIN32
#include "windows.h"
#include "io.h"
#else
#include "time.h"
#endif

int getTimeMs() {
#ifdef _WIN32
	return GetTickCount();
#else
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec * 1000 + t.tv_usec / 1000;
#endif

}

//PV = principal variation, the best move as seen so far along with the line

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
	a8 = 91, b8, c8, d8, e8, f8, g8, h8, noSquare, offBoard

};

enum {
	whiteKingCastle = 0b0001,
	whiteQueenCastle = 0b0010,
	blackKingCastle = 0b0100,
	blackQueenCastle = 0b1000
};

typedef struct {
	
	int move; //this move int is exactly what I was doing with my engine before I started following this series
	int score;//not exactly eval from what I'm aware

} moveStructure;


typedef struct {

	moveStructure moves[MAXPOSITIONMOVES];
	int moveCount;

} moveListStructure;



typedef struct {

	u64 positionKey;
	int move;
	int score;
	int depth;
	int flags;
	int age;

} hashEntryStructure;

typedef struct {
	
	hashEntryStructure* pTable;
	int numberOfEntries;
	int newWrite;
	int overWrite;
	int hit;
	int cut;
	int currentAge;

} hashTableStructure;

typedef struct {

	int move;
	int castlePermission;
	int enPassant;
	int fiftyMove;
	u64 positionKey;

} undoStructure;

//gonna try his struct before I move to classes again so I can get an idea of how this works
typedef struct {

	int pieces[BRD_SQ_NUM];

	int kingSquare[2];
	
	u64 bitBoardKings[3];
	u64 bitBoardPawns[3];
	u64 bitBoardKnights[3];
	u64 bitBoardBishops[3];
	u64 bitBoardRooks[3];
	u64 bitBoardQueens[3];

	int material[2];

	int side;
	int enPassant;
	int fiftyMove;

	int ply;
	int historyPly;

	int castlePermission;

	u64 positionKey;

	int normalPieces[2]; //not pawns
	int majorPieces[2];
	int minorPieces[2];
	int nonPieces[2]; //pawns

	undoStructure history[MAX_GAME_MOVES];
	
	int pieceNumber[13];
	int pieceList[13][10];

	int PVArray[MAXDEPTH];

	int searchHistory[13][BRD_SQ_NUM];
	int searchKillers[2][MAXDEPTH]; //this one is the move that beat the alpha number

} boardStructure;

typedef struct {

	int startTime;
	int stopTime;
	int depth;
	int depthset;
	int movesToGo;
	bool timeSet;

	bool infinite;
	long nodes;

	bool quit;
	bool stopped;

	float fh;
	float fhf;
	int nullCut;


} searchInfoStructure;

typedef struct {

	searchInfoStructure* info;
	boardStructure* originalPosition;
	hashTableStructure* transpositionTable;

} searchThreadDataStructure;

//IMPORTANT do not lose lol
hashTableStructure hashTable[1];

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

int sq120ToSq64[BRD_SQ_NUM];
int sq64ToSq120[64];

void initializeSquare120ToSquare64() {
	int i;
	int file;
	int rank;
	int square = a1;
	int square64 = 0;

	for (i = 0; i < BRD_SQ_NUM; i++) {
		sq120ToSq64[i] = 65;
	}
	for (i = 0; i < 64; i++) {
		sq64ToSq120[i] = 120;
	}

	for (rank = rank1; rank <= rank8; rank++) {
		for (file = fileA; file <= fileH; file++) {
			square = FR2SQ(file, rank);
			sq64ToSq120[square64] = square; //I've typed square so many times it really doesn't seem like it's spelled that way
			sq120ToSq64[square] = square64;
			square64++;
		}
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

u64 pieceKeys[13][BRD_SQ_NUM];
u64 sideKey;
u64 castleKeys[16];


void initializeHashKeys() {
	for (int i = 0; i < 13; i++) {
		for (int i2 = 0; i2 < BRD_SQ_NUM; i2++) {
			pieceKeys[i][i2] = RAND_64;
		}
	}

	sideKey = RAND_64;

	for (int i = 0; i < 16; i++) {
		castleKeys[i] = RAND_64;
	}
}


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

void initializeHashTable(hashTableStructure* table, const int megaBytes) {

	int hashSize = 0x100000 * megaBytes;
	table->numberOfEntries = hashSize / sizeof(hashEntryStructure);
	table->numberOfEntries -= 2;

	if (table->pTable != NULL) {
		free(table->pTable);
	}

	table->pTable = (hashEntryStructure *) malloc(table->numberOfEntries * sizeof(hashEntryStructure));
	if (table->pTable == NULL) {
		printf("Hash Allocation Failed, trying %d mega bytes...\n", megaBytes / 2);
		initializeHashTable(table, megaBytes / 2);
	}
	else {
		clearHashTable(table);
		printf("HashTable init complete with %d entries\n", table->numberOfEntries);
	}

}
//                              ".PNBRQKpnbrqk"
// const int victimScore[13] = { 0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };

const int victimScore[13] = { 0, 600, 100, 300, 200, 400, 500, 600, 100, 300, 200, 400, 500 };
static int mvvLvaScores[13][13];
//const char m_pieceCharacter[] = ".KPNBRQkpnbrq";


void initilizeMvvLva() { //most valuable victim, least valuable attacker
	int attacker;
	int victim;
	for (attacker = wK; attacker <= bQ; attacker++) {
		for (victim = wK; victim <= bQ; victim++) {
			mvvLvaScores[victim][attacker] = victimScore[victim] + 6 - (victimScore[attacker] / 100);
		}
	}
/*
	for (attacker = wK; attacker <= bQ; attacker++) {
		for (victim = wK; victim <= bQ; victim++) {
			printf("%c x %c = %d\n", m_pieceCharacter[attacker], m_pieceCharacter[victim], mvvLvaScores[victim][attacker]);
		}
	}
*/

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


int filesBoard[BRD_SQ_NUM];
int ranksBoard[BRD_SQ_NUM];

static void initializeFilesAndRanksBoard() {

	int i;
	int file;
	int rank;
	int square = a1;
	int square64 = 0;

	for (i = 0; i < BRD_SQ_NUM; i++) {
		filesBoard[i] = offBoard;
		ranksBoard[i] = offBoard;
	}

	for (rank = rank1; rank <= rank8; rank++) {
		for (file = fileA; file <= fileH; file++) {
			square = FR2SQ(file, rank);
			filesBoard[square] = file;
			ranksBoard[square] = rank;
		}
	}
}

static bool squareIs120(const int square) {
	return (square >= 0 && square < 120);
}

static bool squareOnBoard(const int square) {
	return filesBoard[square] == offBoard ? 0 : 1;
}

static bool sideValid(const int side) {
	return (side == white || side == black) ? 1 : 0;
}

static bool fileRankValid(const int fromRank) {
	return (fromRank >= 0 && fromRank <= 7) ? 1 : 0;
}

static bool pieceValidEmpty(const int piece) {
	return (piece >= empty && piece <= bQ) ? 1 : 0;
}

static bool pieceValidEmptyOffboard(const int piece) {
	return (pieceValidEmpty(piece) || piece == offBoard);
}

static bool pieceValid(const int piece) {
	return (piece >= wK && piece <= bQ) ? 1 : 0;
}

u64 generatePositionKey(const boardStructure* position) {

	int square = 0;
	u64 finalKey = 0ULL;
	int piece = empty;

	//pieces
	for (; square < BRD_SQ_NUM; square++) {
		piece = position->pieces[square];
		if (piece != noSquare && piece != empty && piece != offBoard) {
#ifdef PERFORMANCEMODE
			finalKey ^= pieceKeys[piece][square];
#endif
#ifdef DEBUG
			if (piece >= wK && piece <= bQ) {
				finalKey ^= pieceKeys[piece][square];
			}
			else {
				std::cout << "error, invalid piece in the position key generator \n";
			}
#endif
		}
	}

	if (position->side == white) {
		finalKey ^= sideKey;
	}

	if (position->enPassant != noSquare) {
#ifdef DEBUG
		if (position->enPassant >= 0 && position->enPassant < BRD_SQ_NUM) {
			if (!squareOnBoard(position->enPassant)) {
				std::cout << "en passant position not on board in keygen\n";
			}
			else if (ranksBoard[position->enPassant] != rank3 && ranksBoard[position->enPassant] != rank6) {
				std::cout << "en passant not on the 3rd or 6th ranks in keygen\n";
			}
			else {
				finalKey ^= pieceKeys[empty][position->enPassant];
			}

		}
#endif
#ifdef PERFORMANCEMODE
		finalKey ^= pieceKeys[empty][position->enPassant];
#endif
	}


#ifdef DEBUG
	if (position->castlePermission >= 0 && position->castlePermission <= 15) {
		finalKey ^= castleKeys[position->castlePermission];
	}
	else {
		std::cout << "error, invalid castling permissions given to the position key generator \n";
	}
#endif
#ifdef PERFORMANCEMODE
	finalKey ^= castleKeys[position->castlePermission];
#endif

	return finalKey;
}




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
			size_t deleteAllAfterDesiredToken = temp.find(' '); //also don't care about what's after our token
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

bool normalPiece[13] = { true, false, false, true, true, true, true, true, false, true, true, true, true };
bool majorPiece[13] = { false, true, false, false, false, true, true, true, false, false, false, true, true };
bool minorPiece[13] = { false, false, false, true, true, false, false, false, false, true, true, false, false };
int pieceColor[13] = { none, white, white, white, white, white, white, black, black, black, black, black, black };
int pieceValue[13] = { 0, 100000, 100, 300, 315, 500, 900, 100000, 100, 300, 315, 500, 900 };


static void updateListsMaterial(boardStructure* position) {
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

static bool checkBoard(const boardStructure* position) {

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
		std::cout << "true pos key: " << truePosKey <<"\n";
		
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


static void resetBoard(boardStructure* position) {
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



const int knightDirection[8] = { -8, -19,	-21, -12, 8, 19, 21, 12 };
const int rookDirection[4] = { -1, -10,	1, 10 };
const int bishopDirection[8] = { -9, -11, 11, 9 };
const int kingDirection[8] = { -1, -10,	1, 10, -9, -11, 11, 9 };
//									  empty  wk    wp     wn     wb     wr     wq     bk     bp    bn     bb    br      bq
const bool isPiecePawn[13] =        { false, false, true, false, false, false, false, false, true, false, false, false, false };
const bool isPieceKnight[13] =      { false, false, false, true, false, false, false, false, false, true, false, false, false };
const bool isPieceKing[13] =        { false, true, false, false, false, false, false, true, false, false, false, false, false };
const bool isPieceRookQueen[13] =   { false, false, false, false, false, true, true, false, false, false, false, true, true };
const bool isPieceBishopQueen[13] = { false, false, false, false, true, false, true, false, false, false, true, false, true };
const bool pieceSlides[13] =        { false, false, false, false, true, true, true, false, false, false, true, true, true };




static bool squareAttacked(const int square, const int side, const boardStructure *position) { //marker here to hunt for bugs
	
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


char* printSquare(const int square) {

	static char squareString[3];

	int file = filesBoard[square];
	int rank = ranksBoard[square];

	sprintf_s(squareString, "%c%c", ('a' + file), ('1' + rank));

	return squareString;

}

const char pieceCharacter[] = ".KPNBRQkpnbrq";
const char sideCharacter[] = "wb-";
const char rankCharacter[] = "12345678";
const char fileCharacter[] = "abcdefgh";
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





const int loopSlidingPiece[8] = { wB, wR, wQ, 0, bB, bR, bQ, 0 };
const int loopSlidingIndex[2] = { 0, 4 };

const int loopNonSlidingPiece[6] = { wN, wK, 0, bN, bK };
const int loopNonSlidingIndex[2] = { 0, 3 };

const int pieceDirection[13][8] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0 }, //empty
	{ -1, -10,	1, 10, -9, -11, 11, 9 }, //wk
	{ 0, 0, 0, 0, 0, 0, 0, 0 } , //wp
	{ -8, -19,	-21, -12, 8, 19, 21, 12 }, //wn
	{ -9, -11, 11, 9, 0, 0, 0, 0 } ,//wb
	{ -1, -10,	1, 10, 0, 0, 0, 0 },//wr
	{ -1, -10,	1, 10, -9, -11, 11, 9 },//wq
	{ -1, -10,	1, 10, -9, -11, 11, 9 },//bk
	{ 0, 0, 0, 0, 0, 0, 0, 0 },//bp
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },//bn
	{ -9, -11, 11, 9, 0, 0, 0, 0 },//bb
	{ -1, -10,	1, 10, 0, 0, 0, 0 },//br
	{ -1, -10,	1, 10, -9, -11, 11, 9 }//bq
};

/*
{ -8, -19,	-21, -12, 8, 19, 21, 12 } n
{ -9, -11, 11, 9, 0, 0, 0, 0 } b
{ -1, -10,	1, 10, 0, 0, 0, 0 } r
*/
const int numberOfDirections[13] = { 0, 8, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8 };

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
	list->moves[list->moveCount].score = mvvLvaScores[CAPTURED(move)] [position->pieces[FROMSQ(move)]] + 1000000;
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



static void generateAllMoves(const boardStructure* position, moveListStructure* list) {

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

static void takeMove(boardStructure* position) {
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



const int castlePermSheet[120] = {
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

static bool makeMove(boardStructure* position, int move) {

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

static void makeNullMove(boardStructure* position) {

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

static int moveExists(boardStructure* position, const int move) {

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

const int pawnIsolated = -10;
const int pawnPassed[8] = { 0, 5, 10, 20, 35, 60, 100, 200 };
const int rookOpenFile = 10;
const int rookSemiOpenFile = 5;
const int queenOpenFile = 5;
const int queenSemiOpenFile = 3;
const int bishopPair = 30;

u64 blackPassedMask[64];
u64 whitePassedMask[64];
u64 isolatedMask[64];

u64 fileBBMask[8];
u64 rankBBMask[8];


const int pawnTable[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
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
	0	,	5	,	5	,	-10	,	-10	,	0	,	10	,	5	,
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

void initializeEvaluationMasks() {
	int square;
	int tempSquare;
	int rank;
	int file;

	for (square = 0; square < 8; square++) {
		fileBBMask[square] = 0ULL;
		rankBBMask[square] = 0ULL;
	}

	for (rank = rank8; rank >= rank1; rank--) {
		for (file = fileA; file <= fileH; file++) {
			square = rank * 8 + file;
			fileBBMask[file] |= (1ULL << square);
			rankBBMask[rank] |= (1ULL << square);
		}
	}

	for (square = 0; square < 64; square++) {
		isolatedMask[square] = 0ULL;
		whitePassedMask[square] = 0ULL;
		blackPassedMask[square] = 0ULL;
	}

	for (square = 0; square < 64; square++) {
		tempSquare = square + 8;

		while (tempSquare < 64) {
			whitePassedMask[square] |= (1ULL << tempSquare);
			tempSquare += 8;
		}

		tempSquare = square - 8;
		while (tempSquare >= 0) {
			blackPassedMask[square] |= (1ULL << tempSquare);
			tempSquare -= 8;
		}

		if (filesBoard[SQ120(square)] > fileA) {
			isolatedMask[square] |= fileBBMask[filesBoard[SQ120(square)] - 1];

			tempSquare = square + 7;
			while (tempSquare < 64) {
				whitePassedMask[square] |= (1ULL << tempSquare);
				tempSquare += 8;
			}

			tempSquare = square - 9;
			while (tempSquare >= 0) {
				blackPassedMask[square] |= (1ULL << tempSquare);
				tempSquare -= 8;
			}
		}

		if (filesBoard[SQ120(square)] < fileH) {
			isolatedMask[square] |= fileBBMask[filesBoard[SQ120(square)] + 1];

			tempSquare = square + 9;
			while (tempSquare < 64) {
				whitePassedMask[square] |= (1ULL << tempSquare);
				tempSquare += 8;
			}

			tempSquare = square - 7;
			while (tempSquare >= 0) {
				blackPassedMask[square] |= (1ULL << tempSquare);
				tempSquare -= 8;
			}
		}
	}
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

static int evaluatePosition(const boardStructure *position) {

	int piece;
	int pieceNumber;
	int square;
	int score = position->material[white] - position->material[black];
	
	if (!position->pieceNumber[wP] && !position->pieceNumber[bP] && materialDraw(position) == true) {
		return 0;
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
		score += pawnTable[SQ64(square)];
		
		if ((isolatedMask[SQ64(square)] & position->bitBoardPawns[white]) == 0) {
			//printf("wP Iso:%s\n",PrSq(sq));
			score += pawnIsolated;
		}

		if ((whitePassedMask[SQ64(square)] & position->bitBoardPawns[black]) == 0) {
			//printf("wP Passed:%s\n",PrSq(sq));
			score += pawnPassed[ranksBoard[square]];
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
		score -= pawnTable[MIRROR64(SQ64(square))];

		
		if ((isolatedMask[SQ64(square)] & position->bitBoardPawns[black]) == 0) {
			//printf("bP Iso:%s\n",PrSq(sq));
			score -= pawnIsolated;
		}

		if ((blackPassedMask[SQ64(square)] & position->bitBoardPawns[white]) == 0) {
			//printf("bP Passed:%s\n",PrSq(sq));
			score -= pawnPassed[7 - ranksBoard[square]];
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
		if  (!(position->bitBoardPawns[none] & fileBBMask[filesBoard[square]])) {
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
	
	if ((position->material[black] <= ENDGAME_MAT)) {
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
	
	if ((position->material[white] <= ENDGAME_MAT)) {
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

static int probePVMove(const boardStructure* position, const hashTableStructure* table) {

	int index = position->positionKey % table->numberOfEntries;
	//ASSERT(index >= 0 && index <= pos->HashTable->numEntries - 1);

	if (table->pTable[index].positionKey == position->positionKey) {
		return table->pTable[index].move;
	}

	return NOMOVE;
}

/*
static int probePVMove(const boardStructure* position) {

	int index = position->positionKey % position->PVTable->numberOfEntries;
#ifdef DEBUG
	if (index < 0 && index > position->hashTable->numberOfEntries - 1) {
		std::cout << "invalid index in probe pv move\n";
	}
#endif
	if (position->PVTable->pTable[index].positionKey == position->positionKey) {
		return position->PVTable->pTable[index].move;
	}

	return NOMOVE;
}
*/


static int getPVLine(const int depth, boardStructure* position, const hashTableStructure *table) {

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

/*
static bool probeHashEntry(boardStructure* position, int* move, int* score, int alpha, int beta, int depth) {

	int index = position->positionKey % position->hashTable->numberOfEntries;

#ifdef DEBUG
	if (index >= 0 && index <= position->hashTable->numberOfEntries - 1) {}// I got lazy part 2
	else {
		std::cout << "hash table entries mismatched\n";
		std::cout << "hash table entires: " << position->hashTable->numberOfEntries << "\n";
		std::cout << "index: " << index << "\n";
	}
	if (depth >= 1 && depth < MAXDEPTH) {}
	else {
		std::cout << "depth is incorrect\n";
	}
	if (alpha >= -INFINITEC && alpha <= INFINITEC) {}
	else {
		std::cout << "alpha is greater than you mating or less than your opponent mating\n";
		std::cout << "alpha: " << score << "\n";
	}
	if (beta >= -INFINITEC && beta <= INFINITEC) {}
	else {
		std::cout << "beta is greater than you mating or less than your opponent mating\n";
		std::cout << "beta: " << score << "\n";
	}
	if (position->ply >= 0 && position->ply < MAXDEPTH) {}
	else {
		std::cout << "ply is less than 0 or greater than max depth\n";
	}
	if (alpha < beta) {}
	else {
		std::cout << "beta is greater than alpha\n";
	}
#endif
	

	if (position->hashTable->pTable[index].positionKey == position->positionKey) {
		*move = position->hashTable->pTable[index].move;
		if (position->hashTable->pTable[index].depth >= depth) {
			position->hashTable->hit++;
#ifdef DEBUG
			if (position->hashTable->pTable[index].depth >= 1 && position->hashTable->pTable[index].depth < MAXDEPTH) {}
			else {
				std::cout << "depth invalid in hash entry prober\n";
			}
			if (position->hashTable->pTable[index].flags >= HFALPHA && position->hashTable->pTable[index].flags <= HFEXACT) {}
			else {
				std::cout << "flags invalid in hash entry prober\n";
			}
#endif
			*score = position->hashTable->pTable[index].score;
			if (*score > ISMATE) 
				*score -= position->ply;
			else if (*score < -ISMATE) 
				*score += position->ply;

			switch (position->hashTable->pTable[index].flags) {
#ifdef DEBUG
				if (*score >= -INFINITEC && *score <= INFINITEC) {}
				else {
					std::cout << "score invalid in hash entry prober\n";
				}
#endif
			case HFALPHA: 
				if (*score <= alpha) {
				*score = alpha;
				return true;
			}
				break;
			case HFBETA: 
			if (*score >= beta) {
				*score = beta;
				return true;
			}
				break;
			case HFEXACT:
				return true;
				break;
			default: 
				std::cout << "invalid flag in prober in the switch thingy I'm getting sick of writing these you'll probably never read this one\n";
				break;
			}
		}
	}

	return false;
}
*/

static bool probeHashEntry(boardStructure* position, hashTableStructure* table, int* move, int* score, int alpha, int beta, int depth) {

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

/*
void storePVMove(const boardStructure* position, const int move) {

	int index = position->positionKey % position->PVTable->numberOfEntries;

	position->PVTable->pTable[index].move = move;
	position->PVTable->pTable[index].positionKey = position->positionKey;
}
*/

/*
static void storeHashEntry (boardStructure* position, const int move, int score, const int flags, const int depth) {

	int index = position->positionKey % position->hashTable->numberOfEntries;
	
#ifdef DEBUG
	if (index >= 0 && index <= position->hashTable->numberOfEntries - 1) {
	}// I got lazy
	else {
		std::cout << "hash table entries mismatched\n";
		std::cout << "hash table entires: " << position->hashTable->numberOfEntries << "\n";
		std::cout << "index: " << index << "\n";
	}
	if (depth >= 1 && depth < MAXDEPTH) {
	} else {
		std::cout << "depth is incorrect\n";
	}
	if (flags >= HFALPHA && flags <= HFEXACT) {
	} else {
		std::cout << "flags are incorrect\n";
	}
	if (score >= -INFINITEC && score <= INFINITEC) {
	} else {
		std::cout << "score is greater than you mating or less than your opponent mating\n";
		std::cout << "score: " << score << "\n";
	}
	if (position->ply >= 0 && position->ply < MAXDEPTH) {
	} else {
		std::cout << "ply is less than 0 or greater than max depth\n";
	}
#endif

	if ( position->hashTable->pTable[index].positionKey == 0) {
		position->hashTable->newWrite++;
	}
	else {
		position->hashTable->overWrite++;
	}

	
	if (score > ISMATE)
		score += position->ply;
	else if (score < -ISMATE)
		score -= position->ply;
	

	position->hashTable->pTable[index].move = move;
	position->hashTable->pTable[index].positionKey = position->positionKey;
	position->hashTable->pTable[index].flags = flags;
	position->hashTable->pTable[index].score = score;
	position->hashTable->pTable[index].depth = depth;
}
*/

static void storeHashEntry(boardStructure* position, hashTableStructure* table, const int move, int score, const int flags, const int depth) {

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

/*
bool parseMove(std::string pointerCharacter, boardStructure* position) {

#ifdef DEBUG
	if (!checkBoard(position)) {
		std::cout << "check move failed on parsemove\n";
	}
#endif

	if (pointerCharacter[1] > '8' || pointerCharacter[1] < '1') 
		return NOMOVE;
	if (pointerCharacter[3] > '8' || pointerCharacter[3] < '1') 
		return NOMOVE;
	if (pointerCharacter[0] > 'h' || pointerCharacter[0] < 'a') 
		return NOMOVE;
	if (pointerCharacter[2] > 'h' || pointerCharacter[2] < 'a') 
		return NOMOVE;

	int from = FR2SQ(pointerCharacter[0] - 'a', pointerCharacter[1] - '1');
	int to = FR2SQ(pointerCharacter[2] - 'a', pointerCharacter[3] - '1');

#ifdef DEBUG
	if (!squareOnBoard(from) || !squareOnBoard(to)) {
		std::cout << "to or from not on board in parsemove\n";
	}

	std::cout << "parsemove from > " << from << "\n";
	std::cout << "parsemove to > " << to << "\n";
#endif

	moveListStructure list[1];
	generateAllMoves(position, list);
	int moveNumber = 0;
	int move = 0;
	int promotedPiece = empty;

	for (moveNumber = 0; moveNumber < list->moveCount; moveNumber++) {
		move = list->moves[moveNumber].move;
		if (FROMSQ(move) == from && TOSQ(move) == to) {
			promotedPiece = PROMOTED(move);
			if (promotedPiece != empty) {
				if (IsRQ(promotedPiece) && !IsBQ(promotedPiece) && pointerCharacter[4] == 'r') {
					return move;
				}
				else if (!IsRQ(promotedPiece) && IsBQ(promotedPiece) && pointerCharacter[4] == 'b') {
					return move;
				}
				else if (IsRQ(promotedPiece) && IsBQ(promotedPiece) && pointerCharacter[4] == 'q') {
					return move;
				}
				else if (IsKn(promotedPiece) && pointerCharacter[4] == 'n') {
					return move;
				}
				continue;
			}
			return move;
		}
	}

	return NOMOVE;
}
*/


//perft testing here
long leafNodes;

static void perft(int depth, boardStructure* position) {

#ifdef DEBUG
	if (!checkBoard(position)) {
		std::cout << "checkboard failed at the start of perft func\n";
	}
#endif

	if (depth == 0) {
		leafNodes++;
		return;
	}

	moveListStructure list[1];
	generateAllMoves(position, list);

	int moveNumber = 0;
	for (moveNumber = 0; moveNumber < list->moveCount; moveNumber++) {

		if (!makeMove(position, list->moves[moveNumber].move)) { //essentially if the move is legal
			continue;
		}
		perft(depth - 1, position); //ahh! recursion! this is probably nessecary in this case but usually you want to avoid it
		takeMove(position);
	}

	return;
}


static void perftTest(int depth, boardStructure* position) {

#ifdef DEBUG
	if (!checkBoard(position)) {
		std::cout << "checkboard failed at the start of perft testing func\n";
	}
#endif

	int start = getTimeMs();

	printSquareBoard(position);
	printf("\nStarting Test To Depth:%d \n", depth);
	leafNodes = 0;
	moveListStructure list[1];
	generateAllMoves(position, list);

	int move;
	int moveNumber = 0;
	for (moveNumber = 0; moveNumber < list->moveCount; moveNumber++) {
		move = list->moves[moveNumber].move;
		if (!makeMove(position, move)) {
			continue;
		}
		long cumNodes = leafNodes; //what nodes???
		perft(depth - 1, position);
		takeMove(position);
		long oldNodes = leafNodes - cumNodes;
		printf("move %d : %s : %ld\n", moveNumber + 1, printMove(move), oldNodes); //cumulative nodes I think, odd choice of word to shorten
	}
	int timeDifferential = getTimeMs() - start;
	int nodesPerSecond = (leafNodes / timeDifferential) * 1000;
	printf("\nTest Complete : %ld nodes visited in %dms, NPS: %d\n", leafNodes, timeDifferential, nodesPerSecond);
}

/*
static void checkUp (searchInfoStructure* info) {
 
	// .. check if time up, or interrupt from GUI
	if (info->timeSet == true && getTimeMs() > info->stopTime) {
		info->stopped = true;
	}

}
*/

static void pickNextMove(int moveNumber, moveListStructure* list) {

	moveStructure temp;
	int i = 0;
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

static bool isRepetition(const boardStructure *position) {

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
	info->stopped = 0;
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


static int quiescence(int alpha, int beta, boardStructure* position, searchInfoStructure* info) {
	/*
	ASSERT(CheckBoard(position));
	ASSERT(beta > alpha);
	*/
	/*
	if ((info->nodes & 2047) == 0) {
		checkUp(info);
	}
	*/
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
/*
static int alphaBeta(int alpha, int beta, int depth, boardStructure* position, searchInfoStructure* info, bool doNull) {
	
#ifdef DEBUG
	if (!checkBoard(position)) {
		std::cout << "check board failed in alpha beta func\n";
	}
	if (beta < alpha) {
		std::cout << "beta less than alpha\n";
	}
	if (depth < 0) {
		std::cout << "depth less than zero in alpha beta func\n";
	}
#endif

	if (depth <= 0) {
		return quiescence(alpha, beta, position, info);
		// return evaluatePosition(position);
		
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

	
	if (inCheck == true) {
		depth++;
	}
	
	int score = -INFINITEC;
	int PVMove = NOMOVE;
	
	
	bool gamer = probeHashEntry(position, &PVMove, &score, alpha, beta, depth);
	
	if (gamer == true) {
		position->hashTable->cut++;
		return score;
	}
	
	
	if (doNull && !inCheck && position->ply && (position->normalPieces[position->side] > 0) && depth >= 4) {
		makeNullMove(position);
		score = -alphaBeta(-beta, -beta + 1, depth - 4, position, info, false);
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
	
	
	if (PVMove != NOMOVE) {
		for (moveNumber = 0; moveNumber < list->moveCount; moveNumber++) {
			if (list->moves[moveNumber].move == PVMove) {
				list->moves[moveNumber].score = 2000000;
				printf("PV move found \n");
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
		score = -alphaBeta(-beta, -alpha, depth - 1, position, info, true);
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

					storeHashEntry(position, bestMove, beta, HFBETA, depth);
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

#ifdef DEBUG
	if (alpha < oldAlpha) {
		std::cout << "new alpha worse than old alpha??????\n";
	}
#endif
	
	if (alpha != oldAlpha) {
		storeHashEntry(position, bestMove, bestScore, HFEXACT, depth);
	}
	else {
		storeHashEntry(position, bestMove, alpha, HFALPHA, depth);
	}
	

	return alpha;
}
*/

static int alphaBeta(int alpha, int beta, int depth, boardStructure* position, searchInfoStructure* info, hashTableStructure *table, int doNull) {

	/*
	ASSERT(CheckBoard(position));
	ASSERT(beta > alpha);
	ASSERT(depth >= 0);
	*/

	if (depth <= 0) {
		return quiescence(alpha, beta, position, info);
		// return EvalPosition(position);
	}

	/*
	if ((info->nodes & 2047) == 0) {
		checkUp(info);
	}
	*/

	info->nodes++;

	if ((isRepetition(position) || position->fiftyMove >= 100) && position->ply) {
		return DRAW;
	}

	if (position->ply > MAXDEPTH - 1) {
		return evaluatePosition(position);
	}

	int inCheck = squareAttacked(position->kingSquare[position->side], position->side ^ 1, position);

	if (inCheck == true) {
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
				list->moves[moveNumber].score = 2000000;
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
			//printf("info depth %d score cp %d nodes %ld nps %d time %d ", currentDepth, bestScore, info->nodes, nodesPerSecond, getTimeMs() - info->startTime);
			std::cout << "info ";
			std::cout << "depth " << currentDepth;
			std::cout << " score cp " << bestScore;
			std::cout << " nodes " << info->nodes;
			std::cout << " nps " << nodesPerSecond;
			std::cout << " time " << getTimeMs() - info->startTime;
			std::cout << " pv";
			for (pvNumber = 0; pvNumber < pvMoves; pvNumber++) {
				std::cout << " " << printMove(position->PVArray[pvNumber]);
			}
			std::cout << std::endl;

			printf("Hits:%d Overwrite:%d NewWrite:%d Cut:%d Ordering %.2f NullCut:%d\n", table->hit, table->overWrite, table->newWrite, table->cut, (info->fhf/info->fh)*100,info->nullCut);
		}
	}

	std::cout << "bestmove " << printMove(bestMove) << std::endl;
	
}

int searchPositionThread(void* data) {
	
	searchThreadDataStructure* searchData = (searchThreadDataStructure*)data;
	//boardStructure* position = malloc(sizeof(boardStructure));
	boardStructure* position = new(boardStructure);
	memcpy(position, searchData->originalPosition, sizeof(boardStructure));
	
	searchPosition(position, searchData->info, searchData->transpositionTable);
	
	delete(position);
	//std::cout << "freed\n";

	return 0;

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

thrd_t mainSearchThread;

thrd_t launchSearchThread(boardStructure* position, searchInfoStructure* info, hashTableStructure* table) {
	
	searchThreadDataStructure* pSearchData = new(searchThreadDataStructure);

	pSearchData->originalPosition = position;
	pSearchData->info = info;
	pSearchData->transpositionTable = table;


	thrd_t th;
	thrd_create(&th, &searchPositionThread, (void*)pSearchData);

	return th;

}

void joinSearchThread(searchInfoStructure* info) {

	info->stopped = true;

	thrd_join(mainSearchThread, NULL);

}


static void parseGo( std::string line3, searchInfoStructure* info, boardStructure* position, hashTableStructure *table) {

	int depth = -1, movestogo = 30, movetime = -1;
	int time = -1, inc = 0;
	const char* ptr = NULL;
	info->timeSet = false;
	
	const char* line2 = line3.c_str();
	char line[sizeof(line3)];
	for (int i = 0; i < sizeof(line3); i++) {
		line[i] = line2[i];
	}
	

	if ((ptr = strstr(line, "infinite"))) {
		;
	}

	if ((ptr = strstr(line, "binc")) && position->side == black) {
		inc = atoi(ptr + 5);
	}

	if ((ptr = strstr(line, "winc")) && position->side == white) {
		inc = atoi(ptr + 5);
	}

	if ((ptr = strstr(line, "wtime")) && position->side == white) {
		time = atoi(ptr + 6);
	}

	if ((ptr = strstr(line, "btime")) && position->side == black) {
		time = atoi(ptr + 6);
	}

	if ((ptr = strstr(line, "movestogo"))) {
		movestogo = atoi(ptr + 10);
	}

	if ((ptr = strstr(line, "movetime"))) {
		movetime = atoi(ptr + 9);
	}

	if ((ptr = strstr(line, "depth"))) {
		depth = atoi(ptr + 6);
	}

	if (movetime != -1) {
		time = movetime;
		movestogo = 1;
	}

	info->startTime = getTimeMs();
	info->depth = depth;

	if (time != -1) {
		info->timeSet = true;
		time /= movestogo;
		time -= 50;
		info->stopTime = info->startTime + time + inc;
	}

	if (depth == -1) {
		info->depth = MAXDEPTH;
	}
	
	//printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
	//	time, info->startTime, info->stopTime, info->depth, info->timeSet); 
	
	//searchPosition(position, info, table);
	mainSearchThread = launchSearchThread(position, info, table);

}

/*

void uciLoop(boardStructure* currentBoard, searchInfoStructure* info) {
	
	info->GAME_MODE = UCIMODE;
	
	std::string cmd;
	
	int hashTableSizeMB = 64;

	std::cout << "uciok" << "\n";

	while (true) {
		memset(&line[0], 0, sizeof(line));
		fflush(stdout);
		if (!fgets(line, INPUTBUFFER, stdin))
			

		if (line[0] == '\n')
			continue;

		if (!strncmp(line, "isready", 7)) {
			printf("readyok\n");
			continue;
		}
		else if (!strncmp(line, "position", 8)) {
			parsePosition(line, currentBoard);
		}
		else if (!strncmp(line, "ucinewgame", 10)) {
			//parsePosition("position startpos\n", currentBoard);
		}
		else if (!strncmp(line, "go", 2)) {
			printf("Seen Go..\n");
			parseGo(line, info, currentBoard);
		}
		else if (!strncmp(line, "quit", 4)) {
			info->quit = true;
			break;
		}
		else if (!strncmp(line, "uci", 3)) {
			printf("id name %s\n", NAME);
			printf("id author flipwonderland (duh) w/ help from Bluefever!\n");
			printf("uciok\n");
		}
		else if (!strncmp(line, "debug", 4)) {
			//DebugAnalysisTest(pos, info);
			break;
		}
		else if (!strncmp(line, "setoption name Hash value ", 26)) {
			sscanf_s(line, "%*s %*s %*s %*s %d", &hashTableSizeMB);
			if (hashTableSizeMB < 4) hashTableSizeMB = 4;
			printf("Set Hash to %d MB\n", hashTableSizeMB);
			initializeHashTable(currentBoard->hashTable, hashTableSizeMB);
		}
		else if (!strncmp(line, "setoption name Book value ", 26)) {
			
			char* ptrTrue = NULL;
			ptrTrue = strstr(line, "true");
			if (ptrTrue != NULL) {
				EngineOptions->UseBook = TRUE;
			}
			else {
				EngineOptions->UseBook = FALSE;
			}
			
		}
		if (info->quit)
			break;
	}
}
*/

bool saveStringToFile(const std::string& text, const std::string& filename) {
	std::ofstream outFile(filename);
	if (!outFile.is_open()) {
		std::cout << "Error opening file!" << "\n";
		return false;
	}
	outFile << text;
	outFile.close();
	return true;
}


void initializeAll() {

	//computeMoveBoards();
	initializeFilesAndRanksBoard();
	initializeEvaluationMasks();
	initializeSquare120ToSquare64();
	initializeBitMasks();
	initializeHashKeys();
	initilizeMvvLva();

}

bool uci = false;
bool keepRunning = true;
bool boardLoaded = false;
//what would be cool is if I could somehow make it learn every time you play it so I can set it up to learn against other engines with cutechess
//I think I could just do that by making a nn that changes the move order possibly, because with ab pruning if the best move is first the search will be extremely fast
int main(int argc, char* argv[])
{
	using std::cout;
	using std::cin;
	cout << "gamer engine made by flipwonderland" << "\n";
	
	
	/*
	boardStructure currentBoard[1];
	searchInfoStructure info[1];
	info->quit = false;
	currentBoard->hashTable->pTable = NULL;
	initializeHashTable(currentBoard->hashTable, 64);
	
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

	int ArgNum = 0;

	char line[256];
	
	while (TRUE) {
		memset(&line[0], 0, sizeof(line));

		fflush(stdout);
		if (!fgets(line, 256, stdin))
			continue;
		if (line[0] == '\n')
			continue;
		if (!strncmp(line, "uci", 3)) {
			uciLoop(currentBoard, info);
			if (info->quit == TRUE) break;
			continue;
		}
	}
	
	free(currentBoard->hashTable->pTable);
	return 0;
	*/
	
	
	initializeAll();

	int hashTableMegaBytes = 1024;


	boardStructure currentBoard[1];
	hashTable->pTable = NULL;
	initializeHashTable(hashTable, hashTableMegaBytes);
	//currentBoard->PVTable->pTable = NULL;

	searchInfoStructure info[1];
	info->quit = false;
	//moveListStructure list[1];

	do {
		std::string input;
		std::getline(cin, input);
		//std::getline(cin >> std::ws, input);
		std::string command = inputParser(input, 0);


		if (command == "uci") {//should turn this into a switch
			uci = true;
			//clearGameState();
			cout << "id name flipgine" << "\n";
			cout << "id author flip! (duh)" << "\n";
			cout << "uciok" << "\n";
		}
		else if (command == "debug") {

		}
		else if (command == "isready") {
			//see if it's ready to run and then
			if (inputParser(input, 1) == "hi") cout << "I see you! \n";
			cout << "readyok" << "\n";
		}
		else if (command == "setoption")  {//gotta fix this, this is 2 tokens in one
			if (inputParser(input, 3) == "hash") {
				hashTableMegaBytes = stoi(inputParser(input, 3));
				if (hashTableMegaBytes < 4)
					hashTableMegaBytes = 4;

				hashTable->pTable = NULL;
				initializeHashTable(hashTable, hashTableMegaBytes);

			}
		}
		else if (command == "register") {
			//idk if I'm gonna need this one actually
		}
		else if (command == "ucinewgame") {
			clearHashTable(hashTable); //this might create unexpected behavior if the gui does not send this every time
			parsePosition("position startpos\n", currentBoard);
			cout << "new game ready to be loaded!" << "\n";
		}
		else if (command == "position") { // position [fen | startpos]  moves  .... 
			parsePosition(input, currentBoard);
		}
		else if (command == "go") {
			parseGo(input, info, currentBoard, hashTable);
		}
		else if (command == "run") {
			parseFen(STARTFEN, currentBoard);
			parseGo("go infinite", info, currentBoard, hashTable);
		}
		else if (command == "stop") {
			info->stopped = true; //this shouldn't have to be here but it does have to be, figure out why 
			joinSearchThread(info);
		}
		else if (command == "ponderhit") {

		}
		else if (command == "quit") {
			info->stopped = true; //this shouldn't have to be here but it does have to be, figure out why 
			joinSearchThread(info);
			keepRunning = false;
			//info->quit = true;
		}
		else if (command == "legalCheck") {
			if (boardLoaded) {
			}
			else {
				cout << "board is not loaded!" << "\n";
			}
		}
		else if (command == "perft") {
			int perftAmount = std::stoi((inputParser(input, 1)));
			perftTest(perftAmount, currentBoard);
		}
		else {
			/*
			cout << "unknown command, try again. command: " << command << "\n";
			cout << "second command: " << inputParser(input, 1) << "\n";
			cout << "full input: " << input << "\n";
			*/
		}
	} while (keepRunning);

	free(hashTable->pTable);
	return 0;
	
}

