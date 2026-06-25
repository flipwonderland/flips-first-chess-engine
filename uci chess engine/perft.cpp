#include "defs.h"
#include <iostream>

//perft testing here
u64 leafNodes;

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
	std::cout << std::dec << "\n Starting Test To Depth:" << depth << "\n";
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
		u64 cumNodes = leafNodes; //what nodes???
		perft(depth - 1, position);
		takeMove(position);
		u64 oldNodes = leafNodes - cumNodes;
		std::cout << std::dec << "move " << moveNumber + 1 << " : " << printMove(move) << " : " << oldNodes << "\n";
		//printf("move %d : %s : %ld\n", moveNumber + 1, printMove(move), oldNodes);
	}
	int timeDifferential = getTimeMs() - start;
	u64 nodesPerSecond = (leafNodes / timeDifferential) * 1000;
	std::cout << std::dec << "\ntest complete : " << leafNodes << " nodes visited in " << timeDifferential << " ms, NPS: " << nodesPerSecond << "\n";
	//printf("\nTest Complete : %ld nodes visited in %dms, NPS: %d\n", leafNodes, timeDifferential, nodesPerSecond);
}

u64 singlePerftTest(int depth, boardStructure* position) {

	/*
	if (!checkBoard(position)) {
		std::cout << "checkboard failed at the start of perft testing func\n";
	}
	*/
	/*
	int start = getTimeMs();
	*/

	
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
		perft(depth - 1, position);
		takeMove(position);
	}
	/*
	int timeDifferential = getTimeMs() - start;
	int nodesPerSecond = (leafNodes / timeDifferential) * 1000;
	*/

	return leafNodes;
	
}

void quickPerftLib(boardStructure* position) {

	std::cout << "\nquick perft started!\n";
	int start = getTimeMs();
	bool failed = false;
	u64 totalNodes = 0;
	u64 totalStartposNodes = 206603;
	//lots of magic numbers here, they are just the correct perft results for that position
	//initial position
	parseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ", position);
	if (singlePerftTest(1, position ) != 20) {
		std::cout << "initial position depth 1 failed\n";
		std::cout << std::dec << singlePerftTest(1, position);
	} else if (singlePerftTest(2, position) != 400) {
		std::cout << "initial position depth 2 failed\n";
		std::cout << std::dec << singlePerftTest(2, position);
	}
	else if (singlePerftTest(3, position) != 8902) {
		std::cout << "initial position depth 3 failed\n";
		std::cout << std::dec << singlePerftTest(3, position);
	}
	else if (singlePerftTest(4, position) != 197281) {
		std::cout << "initial position depth 4 failed\n";
		std::cout << std::dec << singlePerftTest(4, position);
	}
	else {
		std::cout << "\ninitial position tests passed to depth 4!\n";
		totalNodes += totalStartposNodes;
	}

	// kiwipete
	u64 totalKiwipeteNodes = 99949;
	parseFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", position);
	if (singlePerftTest(1, position) != 48) {
		std::cout << "kiwipete depth 1 failed\n";
		std::cout << std::dec << singlePerftTest(1, position);
	}
	else if (singlePerftTest(2, position) != 2039) {
		std::cout << "kiwipete depth 2 failed\n";
		std::cout << std::dec << singlePerftTest(2, position);
	}
	else if (singlePerftTest(3, position) != 97862) {
		std::cout << "kiwipete depth 3 failed\n";
		std::cout << std::dec << singlePerftTest(3, position);
	}
	else {
		std::cout << "\nkiwipete tests passed to depth 3!\n";
		totalNodes += totalKiwipeteNodes;
	}
	
	// tricky en passant one
	u64 totalNumber3Nodes = 46255;
	parseFen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", position);
	if (singlePerftTest(1, position) != 14) {
		std::cout << "#3 depth 1 failed\n";
		std::cout << std::dec << singlePerftTest(1, position);
	}
	else if (singlePerftTest(2, position) != 191) {
		std::cout << "#3 depth 2 failed\n";
		std::cout << std::dec << singlePerftTest(2, position);
	}
	else if (singlePerftTest(3, position) != 2812) {
		std::cout << "#3 depth 3 failed\n";
		std::cout << std::dec << singlePerftTest(3, position);
	}
	else if (singlePerftTest(4, position) != 43238) {
		std::cout << "#3 depth 4 failed\n";
		std::cout << std::dec << singlePerftTest(4, position);
	}
	else {
		std::cout << "\n#3 tests passed to depth 4!\n";
		totalNodes += totalNumber3Nodes;
	}

	u64 totalNumber4Nodes = 9737;
	parseFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", position);
	if (singlePerftTest(1, position) != 6) {
		std::cout << "#4 depth 1 failed\n";
		std::cout << std::dec << singlePerftTest(1, position);
	}
	else if (singlePerftTest(2, position) != 264) {
		std::cout << "#4 depth 2 failed\n";
		std::cout << std::dec << singlePerftTest(2, position);
	}
	else if (singlePerftTest(3, position) != 9467) {
		std::cout << "#4 depth 3 failed\n";
		std::cout << std::dec << singlePerftTest(3, position);
	}
	else {
		std::cout << "\n#4 tests passed to depth 3!\n";
		totalNodes += totalNumber4Nodes;
	}

	u64 totalNumber5Nodes = 63909;
	parseFen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", position);
	if (singlePerftTest(1, position) != 44) {
		std::cout << "#5 depth 1 failed\n";
		std::cout << std::dec << singlePerftTest(1, position);
	}
	else if (singlePerftTest(2, position) != 1486) {
		std::cout << "#5 depth 2 failed\n";
		std::cout << std::dec << singlePerftTest(2, position);
	}
	else if (singlePerftTest(3, position) != 62379) {
		std::cout << "#5 depth 3 failed\n";
		std::cout << std::dec << singlePerftTest(3, position);
	}
	else {
		std::cout << "\n#5 tests passed to depth 3!\n";
		totalNodes += totalNumber4Nodes;
	}

	u64 totalNumber6Nodes = 92015;
	parseFen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", position);
	if (singlePerftTest(1, position) != 46) {
		std::cout << "#6 depth 1 failed\n";
		std::cout << std::dec << singlePerftTest(1, position);
	}
	else if (singlePerftTest(2, position) != 2079) {
		std::cout << "#6 depth 2 failed\n";
		std::cout << std::dec << singlePerftTest(2, position);
	}
	else if (singlePerftTest(3, position) != 89890) {
		std::cout << "#6 depth 3 failed\n";
		std::cout << std::dec << singlePerftTest(3, position);
	}
	else {
		std::cout << "\n#6 tests passed to depth 3!\n";
		totalNodes += totalNumber4Nodes;
	}

	int timeDif = getTimeMs() - start;
	u64 nodesPerSecond = (leafNodes / timeDif) * 1000;
	
	int seconds = 0;
	int minutes = 0;
	int hours = 0;

	if (timeDif > 999){
		seconds = timeDif / 1000;
	}
	if (seconds > 59) {
		minutes = seconds / 60;
	}
	if (minutes > 59) {
		hours = minutes / 60;
	}

	if (hours > 0) {
		std::cout << "\nquick test completed in " << hours << ":" << minutes << ":" << seconds << "." << timeDif % 1000 << "\n";
	} else if (minutes > 0) {
		std::cout << "\nquick test completed in " << minutes << ":" << seconds << "." << timeDif % 1000 << "\n";
	} else if (seconds > 0) {
		std::cout << "\nquick test completed in " << seconds << "." << timeDif % 1000 << "\n";
	} else {
		std::cout << "\nquick test completed in " << timeDif << "ms" << "\n";
	}
	std::cout << totalNodes << " nodes searched, " << nodesPerSecond << " nps\n";

}

void fullPerftLib(boardStructure* position) {

	std::cout << "\nfull perft started!\n";
	int start = getTimeMs();
	bool failed = false;
	u64 totalNodes = 0;
	u64 totalStartposNodes = 124132536;
	//lots of magic numbers here, they are just the correct perft results for that position
	//initial position
	parseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ", position);
	if (singlePerftTest(1, position) != 20) {
		std::cout << "initial position depth 1 failed\n";
		std::cout << std::dec << singlePerftTest(1, position);
	}
	else if (singlePerftTest(2, position) != 400) {
		std::cout << "initial position depth 2 failed\n";
		std::cout << std::dec << singlePerftTest(2, position);
	}
	else if (singlePerftTest(3, position) != 8902) {
		std::cout << "initial position depth 3 failed\n";
		std::cout << std::dec << singlePerftTest(3, position);
	}
	else if (singlePerftTest(4, position) != 197281) {
		std::cout << "initial position depth 4 failed\n";
		std::cout << std::dec << singlePerftTest(4, position);
	}
	else if (singlePerftTest(5, position) != 4865609) {
		std::cout << "initial position depth 5 failed\n";
		std::cout << std::dec << singlePerftTest(5, position);
	}
	else if (singlePerftTest(6, position) != 119060324) {
		std::cout << "initial position depth 6 failed\n";
		std::cout << std::dec << singlePerftTest(6, position);
	}
	else {
		std::cout << "\ninitial position tests passed to depth 6!\n";
		totalNodes += totalStartposNodes;
	}

	// kiwipete
	u64 totalKiwipeteNodes = 8229523927;
	parseFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", position);
	if (singlePerftTest(1, position) != 48) {
		std::cout << "kiwipete depth 1 failed\n";
		std::cout << std::dec << singlePerftTest(1, position);
	}
	else if (singlePerftTest(2, position) != 2039) {
		std::cout << "kiwipete depth 2 failed\n";
		std::cout << std::dec << singlePerftTest(2, position);
	}
	else if (singlePerftTest(3, position) != 97862) {
		std::cout << "kiwipete depth 3 failed\n";
		std::cout << std::dec << singlePerftTest(3, position);
	}
	else if (singlePerftTest(4, position) != 4085603) {
		std::cout << "kiwipete depth 4 failed\n";
		std::cout << std::dec << singlePerftTest(4, position);
	}
	else if (singlePerftTest(5, position) != 193690690) {
		std::cout << "kiwipete depth 5 failed\n";
		std::cout << std::dec << singlePerftTest(5, position);
	}
	else if (singlePerftTest(6, position) != 8031647685) {
		std::cout << "kiwipete depth 6 failed\n";
		std::cout << std::dec << singlePerftTest(6, position);
	}
	else {
		std::cout << "\nkiwipete tests passed to depth 6!\n";
		totalNodes += totalKiwipeteNodes;
	}

	// tricky en passant one
	u64 totalNumber3Nodes = 190384623;
	parseFen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", position);
	if (singlePerftTest(1, position) != 14) {
		std::cout << "#3 depth 1 failed\n";
		std::cout << std::dec << singlePerftTest(1, position);
	}
	else if (singlePerftTest(2, position) != 191) {
		std::cout << "#3 depth 2 failed\n";
		std::cout << std::dec << singlePerftTest(2, position);
	}
	else if (singlePerftTest(3, position) != 2812) {
		std::cout << "#3 depth 3 failed\n";
		std::cout << std::dec << singlePerftTest(3, position);
	}
	else if (singlePerftTest(4, position) != 43238) {
		std::cout << "#3 depth 4 failed\n";
		std::cout << std::dec << singlePerftTest(4, position);
	}
	else if (singlePerftTest(5, position) != 674624) {
		std::cout << "#3 depth 5 failed\n";
		std::cout << std::dec << singlePerftTest(5, position);
	}
	else if (singlePerftTest(6, position) != 11030083) {
		std::cout << "#3 depth 6 failed\n";
		std::cout << std::dec << singlePerftTest(6, position);
	}
	else if (singlePerftTest(7, position) != 178633661) {
		std::cout << "#3 depth 7 failed\n";
		std::cout << std::dec << singlePerftTest(7, position);
	}
	else {
		std::cout << "\n#3 tests passed to depth 7!\n";
		totalNodes += totalNumber3Nodes;
	}

	u64 totalNumber4Nodes = 722310395;
	parseFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", position);
	if (singlePerftTest(1, position) != 6) {
		std::cout << "#4 depth 1 failed\n";
		std::cout << std::dec << singlePerftTest(1, position);
	}
	else if (singlePerftTest(2, position) != 264) {
		std::cout << "#4 depth 2 failed\n";
		std::cout << std::dec << singlePerftTest(2, position);
	}
	else if (singlePerftTest(3, position) != 9467) {
		std::cout << "#4 depth 3 failed\n";
		std::cout << std::dec << singlePerftTest(3, position);
	}
	else if (singlePerftTest(4, position) != 422333) {
		std::cout << "#4 depth 4 failed\n";
		std::cout << std::dec << singlePerftTest(4, position);
	}
	else if (singlePerftTest(5, position) != 15833292) {
		std::cout << "#4 depth 5 failed\n";
		std::cout << std::dec << singlePerftTest(5, position);
	}
	else if (singlePerftTest(6, position) != 706045033) {
		std::cout << "#4 depth 6 failed\n";
		std::cout << std::dec << singlePerftTest(6, position);
	}
	else {
		std::cout << "\n#4 tests passed to depth 6!\n";
		totalNodes += totalNumber4Nodes;
	}

	u64 totalNumber5Nodes = 92108590;
	parseFen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", position);
	if (singlePerftTest(1, position) != 44) {
		std::cout << "#5 depth 1 failed\n";
		std::cout << std::dec << singlePerftTest(1, position);
	}
	else if (singlePerftTest(2, position) != 1486) {
		std::cout << "#5 depth 2 failed\n";
		std::cout << std::dec << singlePerftTest(2, position);
	}
	else if (singlePerftTest(3, position) != 62379) {
		std::cout << "#5 depth 3 failed\n";
		std::cout << std::dec << singlePerftTest(3, position);
	}
	else if (singlePerftTest(4, position) != 2103487) {
		std::cout << "#5 depth 4 failed\n";
		std::cout << std::dec << singlePerftTest(4, position);
	}
	else if (singlePerftTest(5, position) != 89941194) {
		std::cout << "#5 depth 5 failed\n";
		std::cout << std::dec << singlePerftTest(5, position);

	}
	else {
		std::cout << "\n#5 tests passed to depth 5!\n";
		totalNodes += totalNumber5Nodes;
	}

	u64 totalNumber6Nodes = 168062160;
	parseFen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", position);
	if (singlePerftTest(1, position) != 46) {
		std::cout << "#6 depth 1 failed\n";
		std::cout << std::dec << singlePerftTest(1, position);
	}
	else if (singlePerftTest(2, position) != 2079) {
		std::cout << "#6 depth 2 failed\n";
		std::cout << std::dec << singlePerftTest(2, position);
	}
	else if (singlePerftTest(3, position) != 89890) {
		std::cout << "#6 depth 3 failed\n";
		std::cout << std::dec << singlePerftTest(3, position);
	}
	else if (singlePerftTest(4, position) != 3894594) {
		std::cout << "#6 depth 4 failed\n";
		std::cout << std::dec << singlePerftTest(4, position);
	}
	else if (singlePerftTest(5, position) != 164075551) {
		std::cout << "#6 depth 5 failed\n";
		std::cout << std::dec << singlePerftTest(5, position);
	}
	else {
		std::cout << "\n#6 tests passed to depth 5!\n";
		totalNodes += totalNumber6Nodes;
	}

	int timeDif = getTimeMs() - start;
	u64 nodesPerSecond = (totalNodes / timeDif) * 1000;

	int seconds = 0;
	int minutes = 0;
	int hours = 0;

	if (timeDif > 999) {
		seconds = timeDif / 1000;
	}
	if (seconds > 59) {
		minutes = seconds / 60;
	}
	if (minutes > 59) {
		hours = minutes / 60;
	}

	if (hours > 0) {
		if (minutes < 10) {
			if (seconds < 10) {
				std::cout << "\nfull test completed in " << hours << ":0" << minutes % 60 << ":0" << seconds % 60 << "." << timeDif % 1000 << "\n";
			}
			std::cout << "\nfull test completed in " << hours << ":0" << minutes % 60 << ":" << seconds % 60 << "." << timeDif % 1000 << "\n";
		}
		if (seconds < 10) {
			std::cout << "\nfull test completed in " << hours << ":" << minutes % 60 << ":0" << seconds % 60 << "." << timeDif % 1000 << "\n";
		}
		std::cout << "\nfull test completed in " << hours << ":" << minutes % 60 << ":" << seconds % 60 << "." << timeDif % 1000 << "\n";
	}
	else if (minutes > 0) {
		if (seconds < 10) {
			std::cout << "\nfull test completed in " << minutes << ":0" << seconds % 60 << "." << timeDif % 1000 << "\n";
		}
		std::cout << "\nfull test completed in " << minutes << ":" << seconds % 60 << "." << timeDif % 1000 << "\n";
	}
	else if (seconds > 0) {
		std::cout << "\nfull test completed in " << seconds << "." << timeDif % 1000 << "\n";
	}
	else {
		std::cout << "\nfull test completed in " << timeDif << "ms" << "\n";
	}
	std::cout << totalNodes << " nodes searched, " << nodesPerSecond << " nps\n";

}