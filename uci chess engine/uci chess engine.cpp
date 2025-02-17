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
	//moveListStructure list[1];

	do {
		std::string input;
		std::getline(cin, input);
		//std::getline(cin >> std::ws, input);
		std::string command = inputParser(input, 0);


		if (command == "uci") {//should turn this into a switch
			uci = true;
			//clearGameState();
			cout << "id name flipgine" << "\n";
			cout << "id author flip! (duh)" << "\n";
			cout << "uciok" << "\n";
		}
		else if (command == "debug") {

		}
		else if (command == "isready") {
			//see if it's ready to run and then
			if (inputParser(input, 1) == "hi") cout << "I see you! \n";
			cout << "readyok" << "\n";
		}
		else if (command == "setoption")  {//gotta fix this, this is 2 tokens in one
			if (inputParser(input, 3) == "hash") {
				hashTableMegabytes = std::stoi(inputParser(input, 3)); //watch this
				if (hashTableMegabytes < 4)
					hashTableMegabytes = 4;

				hashTable->pTable = NULL;
				initializeHashTable(hashTable, hashTableMegabytes);

			}
		}
		else if (command == "register") {
			//idk if I'm gonna need this one actually
		}
		else if (command == "ucinewgame") {
			clearHashTable(hashTable); //this might create unexpected behavior if the gui does not send this every time
			parsePosition("position startpos\n", currentBoard);
			cout << "new game ready to be loaded!" << "\n";
		}
		else if (command == "position") { // position [fen | startpos]  moves  .... 
			parsePosition(input, currentBoard);
		}
		else if (command == "go") {
			parseGo(input, info, currentBoard, hashTable);
		}
		else if (command == "run") {
			parseFen(STARTFEN, currentBoard);
			parseGo("go infinite", info, currentBoard, hashTable);
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

