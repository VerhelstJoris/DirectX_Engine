#pragma once

#include <string>
#include <vector>


struct ContextData
{
	bool useContext;
	std::string context;
	int contextLength;
};

static int FindClosingBracket(std::string text, int openPos)
{
	int closePos = openPos;
	int counter = 1;

	int textLength = text.length();

	while (counter > 0) 
	{
		char c = text[++closePos];
		if (c == '[') {
			counter++;
		}
		else if (c == ']') {
			counter--;
		}
	}

	return closePos;
}