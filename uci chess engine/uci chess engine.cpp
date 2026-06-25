#include <string>
#include <iostream>

#include "util.h"
#include "defs.h"


bool uci = false;
bool keepRunning = true;
bool boardLoaded = false;

hashTableStructure hashTable[1];

//what would be cool is if I could somehow make it learn every time you play it so I can set it up to learn against other engines with cutechess
//I think I could just do that by making a nn that changes the move order possibly, because with ab pruning if the best move is first the search will be extremely fast
int main(int argc, char* argv[])
{
	using std::cout;
	using std::cin;
	cout << "gamer engine made by flipwonderland" << "\n";
	
	initializeAll();

	int hashTableMegabytes = 1024;


	boardStructure currentBoard[1];
	hashTable->pTable = NULL;
	initializeHashTable(hashTable, hashTableMegabytes);
	//currentBoard->PVTable->pTable = NULL;

	searchInfoStructure info[1];
	info->quit = false;
	info->stopped = false;
	info->threadNumber = 1;
	int threadsSet = 1;
	//moveListStructure list[1];

	parsePosition("position startpos\n", currentBoard);

	/*
	tempHashTest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
	tempHashTest("r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1");
	exit(0);
	*/

	do {
		std::string input;
		std::getline(cin, input);
		//std::getline(cin >> std::ws, input);
		std::string command = inputParser(input, 0);


		if (command == "uci") {
			uci = true;
			//clearGameState();
			cout << "id name flipgine" << "\n";
			cout << "id author flip! (duh)" << "\n";
			cout << "uciok" << "\n";
		}
		else if (command == "debug") {

		}
		else if (command == "isready") {
			if (inputParser(input, 1) == "hi") cout << "I see you! \n";
			cout << "readyok" << "\n";
		}
		else if (command == "setoption")  {
			if (inputParser(input, 1) == "hash") {
				hashTableMegabytes = std::stoi(inputParser(input, 2));
				if (hashTableMegabytes < 4)
					hashTableMegabytes = 4;

				hashTable->pTable = NULL;
				initializeHashTable(hashTable, hashTableMegabytes);

			}
			if (inputParser(input, 1) == "threads") {
				int threadsSet = std::stoi(inputParser(input, 2));
				info->threadNumber = threadsSet;
				cout << "set threads to: " << threadsSet << "\n";
			}
		}
		else if (command == "register") {
			//idk if I'm gonna need this one actually
		}
		else if (command == "ucinewgame") {
			clearHashTable(hashTable); //this might create unexpected behavior if the gui does not send this every time (what?)
			parsePosition("position startpos\n", currentBoard);
			//cout << "new game ready to be loaded!" << "\n";
		}
		else if (command == "position") { // position [fen | startpos]  moves  .... 
			parsePosition(input, currentBoard);
		}
		else if (command == "go") {
			parseGo(input, info, currentBoard, hashTable);
		}
		else if (command == "run") {
			parseFen(LCT_1, currentBoard);
			parseGo("go depth 12", info, currentBoard, hashTable);
		}
		else if (command == "ponderhit") {

		}
		else if (command == "stop") {
			joinSearchThread(info);
		}
		else if (command == "quit") {
			joinSearchThread(info);
			keepRunning = false;
			info->quit = true;
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
		else if (command == "perftQ") {
			quickPerftLib(currentBoard);
		}
		else if (command == "perftF") {
			fullPerftLib(currentBoard);
		}
		else if (command == "d") {
			printSquareBoard(currentBoard);
		}
		else if (command == "mirror") {
			int firstEval = evaluatePosition(currentBoard);
			mirrorBoard(currentBoard);
			if (firstEval != evaluatePosition(currentBoard)) {
				cout << "mirror test failed\n";
				cout << "start eval: " << firstEval << "\n";
				cout << "second eval: " << evaluatePosition(currentBoard) << "\n";
			}
			mirrorBoard(currentBoard);
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

