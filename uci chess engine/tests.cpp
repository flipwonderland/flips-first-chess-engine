#include "defs.h"
#include <iostream>

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