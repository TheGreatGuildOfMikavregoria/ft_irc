#include "Utils.hpp"

std::string stringToLowercase(std::string text)
{
	int i = 0;
	for (char &c : text)
		c = std::tolower(text[i]);
	return text;
}
