#include "stdlib.h"

//#define DEBUG
#define PERFORMANCEMODE

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


//these are from the bluefever software series
#define RAND_64 (	(u64)rand() | \
					(u64)rand() << 15 | \
					(u64)rand() << 30 | \
					(u64)rand() << 45 | \
					((u64)rand() & 0xf) << 60	)



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