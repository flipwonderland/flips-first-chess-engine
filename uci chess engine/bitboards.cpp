#include "defs.h"

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