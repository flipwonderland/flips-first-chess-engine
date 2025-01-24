#include "stdlib.h"

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


//these are from the bluefever software series
#define RAND_64 (	(u64)rand() | \
					(u64)rand() << 15 | \
					(u64)rand() << 30 | \
					(u64)rand() << 45 | \
					((u64)rand() & 0xf) << 60	)

#define POP(b) popBit(b)
#define CNT(b) countBits(b)
#define CLRBIT(bb,sq) ((bb) &= clearMask(sq))
#define SETBIT(bb,sq) ((bb) |= setMask(sq))
#define FR2SQ(f, r) ((21 + (f)) + ((r) * 10))
#define SQ64 (sq120) (square120ToSquare64[(sq120)])
#define SQ120(sq64) (square64ToSquare120[(sq64)])

internal int square120ToSquare64[BRD_SQ_NUM];
internal int square64ToSquare120[64];

#define STARTFEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define TESTFEN "rnbqkbnr/1ppppppp/8/pP6/8/8/P1PPPPPP/RNBQKBNR w KQkq a6 0 1"
#define TESTFEN2 "rnbqkbnr/p1pppppp/8/8/P7/1p6/1PPPPPPP/RNBQKBNR w KQkq - 0 1"
#define TESTFEN3 "rnbqkbnr/pppppp1p/8/8/6pP/8/PPPPPPP1/RNBQKBNR b KQkq h3 0 1"

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