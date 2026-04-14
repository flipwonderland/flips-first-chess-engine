#include "defs.h"
#include <iostream>

//perft testing here
long leafNodes;

static void perft(int depth, boardStructure* position) {


	if (!checkBoard(position)) {
		std::cout << "checkboard failed at the start of perft func\n";
	}


	if (depth == 0) {
		leafNodes++;
		return;
	}

	moveListStructure list[1];
	generateAllMoves(position, list);

	int moveNumber = 0;
	for (moveNumber = 0; moveNumber < list->moveCount; moveNumber++) {

		if (!makeMove(position, list->moves[moveNumber].move)) { 
			continue;
		}
		perft(depth - 1, position); 
		takeMove(position);
	}

	return;
}


void perftTest(int depth, boardStructure* position) {

#ifdef DEBUG
	if (!checkBoard(position)) {
		std::cout << "checkboard failed at the start of perft testing func\n";
	}
#endif

	int start = getTimeMs();

	printSquareBoard(position);
	std::cout << "\n Starting Test To Depth:" << depth << "\n";
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
		printf("move %d : %s : %ld\n", moveNumber + 1, printMove(move), oldNodes); //cummulative nodes I think, odd choice of word to shorten
	}
	int timeDifferential = getTimeMs() - start;
	int nodesPerSecond = (leafNodes / timeDifferential) * 1000;
	printf("\nTest Complete : %ld nodes visited in %dms, NPS: %d\n", leafNodes, timeDifferential, nodesPerSecond);
}

bool singlePerftTest(int depth, int expectedNodes, boardStructure* position) {

	/*
	if (!checkBoard(position)) {
		std::cout << "checkboard failed at the start of perft testing func\n";
	}
	*/

	int start = getTimeMs();

	printSquareBoard(position);
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
		long cumNodes = leafNodes; 
		perft(depth - 1, position);
		takeMove(position);
		long oldNodes = leafNodes - cumNodes;
	}
	int timeDifferential = getTimeMs() - start;
	int nodesPerSecond = (leafNodes / timeDifferential) * 1000;

	return false;
	
}

void fullPerftTest(boardStructure* position) {

	parsePosition("", position);

}