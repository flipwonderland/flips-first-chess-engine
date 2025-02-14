#include "defs.h"
#include "util.h"

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

//precomputed move table
//got this idea from sebastian lauges vid cause I had no idea what to do


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

		whiteConnectedMask[square] = 0ULL;
		blackConnectedMask[square] = 0ULL;
		whiteDoubledMask[square] = 0ULL;
		blackDoubledMask[square] = 0ULL;
	}

	for (square = 0; square < 64; square++) {
		tempSquare = square + 8;

		while (tempSquare < 64) {
			whitePassedMask[square] |= (1ULL << tempSquare);
			whiteDoubledMask[square] |= (1ULL << tempSquare);
			tempSquare += 8;
		}

		tempSquare = square - 8;
		while (tempSquare >= 0) {
			blackPassedMask[square] |= (1ULL << tempSquare);
			blackDoubledMask[square] |= (1ULL << tempSquare);
			tempSquare -= 8;
		}

		if (filesBoard[SQ120(square)] > fileA) {
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


		file = square % 8;
		rank = square / 8;

		if (file != 7) {
			tempSquare = square + 9;
			blackConnectedMask[square] |= (1ULL << tempSquare);
			tempSquare = square - 7;
			whiteConnectedMask[square] |= (1ULL << tempSquare);

		}

		if (file != 0) {
			tempSquare = square + 7;
			blackConnectedMask[square] |= (1ULL << tempSquare);
			tempSquare = square - 9;
			whiteConnectedMask[square] |= (1ULL << tempSquare);

		}
	}

	for (square = 0; square < 64; square++) {
		file = square % 8;
		rank = square / 8;
		if (file != 0) {
			tempSquare = square - 1;
			isolatedMask[square] |= (1ULL << tempSquare);
			while (tempSquare < 63) {
				tempSquare += 8;
				isolatedMask[square] |= (1ULL << tempSquare);
			}
			while (tempSquare > 0) {
				tempSquare -= 8;
				isolatedMask[square] |= (1ULL << tempSquare);
			}
		}
		if (file != 7) {
			tempSquare = square + 1;
			isolatedMask[square] |= (1ULL << tempSquare);
			while (tempSquare < 63) {
				tempSquare += 8;
				isolatedMask[square] |= (1ULL << tempSquare);
			}
			while (tempSquare > 0) {
				tempSquare -= 8;
				isolatedMask[square] |= (1ULL << tempSquare);
			}
		}
	}



	for (square = 0; square < 64; square++) {

		//std::cout << "isolated mask for square " << square << "\n";
		//printBitBoard(isolatedMask[square]);
		//blackConnectedMask[square] |= (1ULL << square);
		//std::cout << "black connected mask for square " << square << "\n";
		//printBitBoard(blackConnectedMask[square]);
		//std::cout << "white doubled mask for square " << square << "\n";
		//printBitBoard(whiteDoubledMask[square]);
		//std::cout << "black doubled mask for square " << square << "\n";
		//printBitBoard(blackDoubledMask[square]);
	}
}

void initializeHashTable(hashTableStructure* table, const int megabytes) {

	int hashSize = 0x100000 * megabytes;
	table->numberOfEntries = hashSize / sizeof(hashEntryStructure);
	table->numberOfEntries -= 2;

	if (table->pTable != NULL) {
		free(table->pTable);
	}

	table->pTable = (hashEntryStructure*)malloc(table->numberOfEntries * sizeof(hashEntryStructure));
	if (table->pTable == NULL) {
		printf("Hash Allocation Failed, trying %d mega bytes...\n", megabytes / 2);
		initializeHashTable(table, megabytes / 2);
	}
	else {
		clearHashTable(table);
		printf("HashTable init complete with %d entries\n", table->numberOfEntries);
	}

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