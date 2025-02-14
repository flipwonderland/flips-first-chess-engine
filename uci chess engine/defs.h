#define DEFS_H

#include "stdlib.h"
#include "stdio.h"
#include "util.h"

#include <iostream>
#include <string>

#define NAME "flipgine v1 :D"

#define INPUTBUFFER 400 * 6

#define BRD_SQ_NUM 120
#define MAX_GAME_MOVES 2048
//#define MAX_GAME_MOVES 17697
//this would be better, I'd run into issues if someone played a longer game which is possible but I'm gonna save on ram and do this instead
#define MAXPOSITIONMOVES 218 // this is the max number of moves that we would expect to be in a single position
#define MAXDEPTH 64
#define DRAW 0
#define INFINITEC 30000
#define ISMATE (INFINITEC - MAXDEPTH)
#define INTFINITE 2147483647

#define FR2SQ(f, r) ((21 + (f)) + ((r) * 10))
#define SQ64(sq120) (sq120ToSq64[(sq120)])
#define SQ120(sq64) (sq64ToSq120[(sq64)])
#define POP(b) popFirstBit(b)
#define CNT(b) countBits(b)
#define CLRBIT(bb,sq) ((bb) &= clearMask[(sq)])
#define SETBIT(bb,sq) ((bb) |= setMask[(sq)])

#define IsBQ(p) (isPieceBishopQueen[(p)])
#define IsRQ(p) (isPieceRookQueen[(p)])
#define IsKn(p) (isPieceKnight[(p)])
#define IsKi(p) (isPieceKing[(p)])

/*
0000 0000 0000 0000 0000 0111 1111 -> From 0x7F
0000 0000 0000 0011 1111 1000 0000 -> To >> 7, 0x7F
0000 0000 0011 1100 0000 0000 0000 -> Captured >> 14, 0xF
0000 0000 0100 0000 0000 0000 0000 -> EP 0x40000
0000 0000 1000 0000 0000 0000 0000 -> Pawn Start 0x80000
0000 1111 0000 0000 0000 0000 0000 -> Promoted Piece >> 20, 0xF
0001 0000 0000 0000 0000 0000 0000 -> Castle 0x1000000
*/

#define FROMSQ(m) ((m) & 0x7F)
#define TOSQ(m) (((m)>>7) & 0x7F)
#define CAPTURED(m) (((m)>>14) & 0xF)
#define PROMOTED(m) (((m)>>20) & 0xF)

#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000

#define MFLAGCAP 0x7C000
#define MFLAGPROM 0xF00000

#define MOVE_MOVEGEN(f,t,ca,pro,fl) ( (f) | ((t) << 7) | ( (ca) << 14 ) | ( (pro) << 20 ) | (fl))
#define SQOFFBOARD_MOVEGEN(sq) (filesBoard[(sq)]==offBoard)

#define NOMOVE 0

#define HASH_PCE(piece,square) (position->positionKey ^= (pieceKeys[(piece)][(square)]))
#define HASH_CA (position->positionKey ^= (castleKeys[(position->castlePermission)]))
#define HASH_SIDE (position->positionKey ^= (sideKey))
#define HASH_EP (position->positionKey ^= (pieceKeys[empty][(position->enPassant)]))



#define STARTFEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define TESTFEN "rnbqkbnr/1ppppppp/8/pP6/8/8/P1PPPPPP/RNBQKBNR w KQkq a6 0 1"
#define TESTFEN2 "rnbqkbnr/p1pppppp/8/8/P7/1p6/1PPPPPPP/RNBQKBNR w KQkq - 0 1"
#define TESTFEN3 "rnbqkbnr/pppppp1p/8/8/6pP/8/PPPPPPP1/RNBQKBNR b KQkq h3 0 1"
#define PAWNMOVETESTW "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define PAWNMOVETESTB "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b - e3 0 1"
#define KNIGHTSKINGS "5k2/1n6/4n3/6N1/8/3N4/8/5K2 w - - 0 1"
#define ROOKSFEN "6k1/8/5r2/8/1nR5/5N2/8/6K1 b - - 0 1"
#define QUEENSFEN "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 w - - 0 1"
#define BISHOPSFEN "6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 b - - 0 1" 
#define CASTLETESTFEN "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
#define CASTLETEST2FEN "3rk2r/8/8/8/8/8/6p1/R3K2R w KQk - 0 1"
#define PERFORMANCETESTFEN "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define WAC1 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -"
#define WAC2 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"
#define CURRENTTESTFEN "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/P1N2Q1p/1PPBBPPP/R3K2R b KQkq - 0 1"

enum { HFNONE, HFALPHA, HFBETA, HFEXACT };

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

	//PV = principal variation, the best move as seen so far along with the line
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

extern int filesBoard[BRD_SQ_NUM];
extern int ranksBoard[BRD_SQ_NUM];

extern std::string startingFenString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
extern std::string inputFenString{};

extern bool normalPiece[13] = { true, false, false, true, true, true, true, true, false, true, true, true, true };
extern bool majorPiece[13] = { false, true, false, false, false, true, true, true, false, false, false, true, true };
extern bool minorPiece[13] = { false, false, false, true, true, false, false, false, false, true, true, false, false };
extern int pieceColor[13] = { none, white, white, white, white, white, white, black, black, black, black, black, black };
extern int pieceValue[13] = { 0, 100000, 100, 300, 315, 500, 900, 100000, 100, 300, 315, 500, 900 };

extern const int knightDirection[8] = { -8, -19,	-21, -12, 8, 19, 21, 12 };
extern const int rookDirection[4] = { -1, -10,	1, 10 };
extern const int bishopDirection[8] = { -9, -11, 11, 9 };
extern const int kingDirection[8] = { -1, -10,	1, 10, -9, -11, 11, 9 };
//									  empty  wk    wp     wn     wb     wr     wq     bk     bp    bn     bb    br      bq
extern const bool isPiecePawn[13] = { false, false, true, false, false, false, false, false, true, false, false, false, false };
extern const bool isPieceKnight[13] = { false, false, false, true, false, false, false, false, false, true, false, false, false };
extern const bool isPieceKing[13] = { false, true, false, false, false, false, false, true, false, false, false, false, false };
extern const bool isPieceRookQueen[13] = { false, false, false, false, false, true, true, false, false, false, false, true, true };
extern const bool isPieceBishopQueen[13] = { false, false, false, false, true, false, true, false, false, false, true, false, true };
extern const bool pieceSlides[13] = { false, false, false, false, true, true, true, false, false, false, true, true, true };

extern const char pieceCharacter[] = ".KPNBRQkpnbrq";
extern const char sideCharacter[] = "wb-";
extern const char rankCharacter[] = "12345678";
extern const char fileCharacter[] = "abcdefgh";
extern char printCastle;

extern const int loopSlidingPiece[8] = { wB, wR, wQ, 0, bB, bR, bQ, 0 };
extern const int loopSlidingIndex[2] = { 0, 4 };

extern const int loopNonSlidingPiece[6] = { wN, wK, 0, bN, bK };
extern const int loopNonSlidingIndex[2] = { 0, 3 };

extern const int pieceDirection[13][8] = {
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
extern const int numberOfDirections[13] = { 0, 8, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8 };

extern const int castlePermSheet[120] = {
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

extern const int pawnIsolated = -5;
extern const int pawnDoubled = -10;
extern const int pawnDoubledIsolated = -25;
extern const int pawnPassed[8] = { 0, 5, 10, 20, 35, 60, 100, 200 };
extern const int pawnConnected[8] = { 0, 0, 10, 15, 35, 40, 60, 200 };
extern const int pawnConnectedPassed[8] = { 0, 15, 20, 40, 100, 150, 250, 600 };
extern const int rookOpenFile = 15;
extern const int rookSemiOpenFile = 10;
extern const int queenOpenFile = 5;
extern const int queenSemiOpenFile = 3;
extern const int bishopPair = 30;

extern u64 blackPassedMask[64];
extern u64 whitePassedMask[64];
extern u64 isolatedMask[64];

extern u64 whiteConnectedMask[64];
extern u64 blackConnectedMask[64];
extern u64 whiteDoubledMask[64];
extern u64 blackDoubledMask[64];

extern u64 fileBBMask[8];
extern u64 rankBBMask[8];

extern bool moveTableDiagonal[64][64];
extern bool moveTableCardinal[64][64];

extern const int victimScore[13] = { 0, 600, 100, 300, 200, 400, 500, 600, 100, 300, 200, 400, 500 };
extern int mvvLvaScores[13][13];

extern u64 pieceKeys[13][BRD_SQ_NUM];
extern u64 sideKey;
extern u64 castleKeys[16];

extern u64 setMask[64];
extern u64 clearMask[64];

extern int sq120ToSq64[BRD_SQ_NUM];
extern int sq64ToSq120[64];

//board
extern bool squareIs120(const int square);
extern bool squareOnBoard(const int square);
extern bool sideValid(const int side);
extern bool fileRankValid(const int fromRank);
extern bool pieceValidEmpty(const int piece);
extern bool pieceValidEmptyOffboard(const int piece);
extern bool pieceValid(const int piece);
extern void updateListsMaterial(boardStructure* position);
extern bool checkBoard(const boardStructure* position);
extern void resetBoard(boardStructure* position);

//eval
extern int evaluatePosition(const boardStructure* position);

//fen

//minmax

//perft

//hashkey
extern u64 generatePositionKey(const boardStructure* position);

//hashtable

//init
extern void initializeAll();

//input

//movegen

//output

//search

//tests

//threads