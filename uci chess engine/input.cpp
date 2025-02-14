#include "defs.h"
#include <string>
#include <iostream>


std::string inputParser(std::string input, const int desiredToken) {
	std::string temp = input;
	std::string token;
	size_t stopTokenPlace;
	int tokenCount = 0;
	bool stop = false;
	int loopBreaker = 10000;
	int allowedTokensWithoutInfo = 2;

	while (!stop) {
		if (desiredToken != 0) stopTokenPlace = temp.find(' ');
		else stopTokenPlace = 0;

		if (stopTokenPlace == std::string::npos)/* end of the input string*/ {
			stop = true;
		}
		if (loopBreaker == 0) {
			std::cout << "infinite loop caused in input parser! input: " << input << "\ntoken: " << desiredToken << "\n";
			break;
		}
		if (allowedTokensWithoutInfo == 0)
			break;
		//might be able to change this to !=
		if (tokenCount < desiredToken) /*if it's less than the desired token, we don't care what it is, just delete it*/ {
			temp.erase(0, stopTokenPlace + 1); //+1 because it's the array version that starts with 0, and this needs it to start with 1
			tokenCount++;
			allowedTokensWithoutInfo++;
		}
		else {
			size_t deleteAllAfterDesiredToken = temp.find(' '); //also don't care about what's after our token
			if (deleteAllAfterDesiredToken != std::string::npos)
				temp.erase(deleteAllAfterDesiredToken, temp.length()); //it would probably not be good if we deleted all after npos
			token = temp;
			return token;
		}
		loopBreaker--;
		allowedTokensWithoutInfo--;

	}
	return "endOfTheLinePal."; //as long as the input is never this it shouldn't be an issue
}

static void parseGo(std::string line3, searchInfoStructure* info, boardStructure* position, hashTableStructure* table) {

	int depth = -1, movestogo = 30, movetime = -1;
	int time = -1, inc = 0;
	const char* ptr = NULL;
	info->timeSet = false;

	const char* line2 = line3.c_str();
	char line[sizeof(line3)];
	for (int i = 0; i < sizeof(line3); i++) {
		line[i] = line2[i];
	}


	if ((ptr = strstr(line, "infinite"))) {
		;
	}

	if ((ptr = strstr(line, "binc")) && position->side == black) {
		inc = atoi(ptr + 5);
	}

	if ((ptr = strstr(line, "winc")) && position->side == white) {
		inc = atoi(ptr + 5);
	}

	if ((ptr = strstr(line, "wtime")) && position->side == white) {
		time = atoi(ptr + 6);
	}

	if ((ptr = strstr(line, "btime")) && position->side == black) {
		time = atoi(ptr + 6);
	}

	if ((ptr = strstr(line, "movestogo"))) {
		movestogo = atoi(ptr + 10);
	}

	if ((ptr = strstr(line, "movetime"))) {
		movetime = atoi(ptr + 9);
	}

	if ((ptr = strstr(line, "depth"))) {
		depth = atoi(ptr + 6);
	}

	if (movetime != -1) {
		time = movetime;
		movestogo = 1;
	}

	info->startTime = getTimeMs();
	info->depth = depth;

	if (time != -1) {
		info->timeSet = true;
		time /= movestogo;
		time -= 50;
		info->stopTime = info->startTime + time + inc;
	}

	if (depth == -1) {
		info->depth = MAXDEPTH;
	}

	printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
		time, info->startTime, info->stopTime, info->depth, info->timeSet);

	//searchPosition(position, info, table);
	mainSearchThread = launchSearchThread(position, info, table);

}