#include "stdlib.h"

#define DEBUG
//#define PERFORMANCEMODE

typedef char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;

#define global_variable static
#define internal static

#define BRD_SQ_NUM 120
#define MAX_GAME_MOVES 2048
//#define MAX_GAME_MOVES 17697
//this would be better, I'd run into issues if someone played a longer game which is possible but I'm gonna save on ram and do this instead
#define MAXPOSITIONMOVES 218 // this is the max number of moves that we would expect to be in a single position
#define MAXDEPTH 512

//these are from the bluefever software series
#define RAND_64 (	(u64)rand() | \
					(u64)rand() << 15 | \
					(u64)rand() << 30 | \
					(u64)rand() << 45 | \
					((u64)rand() & 0xf) << 60	)

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

#define NOMOVE false

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
#define CURRENTTESTFEN "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"


inline int
clamp(int min, int val, int max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

inline float
clampf(float min, float val, float max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

//this was taken from the pong game that I followed a tutorial for