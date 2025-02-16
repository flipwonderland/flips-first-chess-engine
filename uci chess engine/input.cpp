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

