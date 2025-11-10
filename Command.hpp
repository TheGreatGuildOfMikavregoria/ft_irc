#pragma once
#include <sstream>
#include <map>
#include <vector>
#include <cctype>
#include "Buffer.hpp"
// TODO: debug purposes, remove later
#include <iostream>

//TODO: implement bit flags for different states
#define MESSAGE_COMPLETE 1
#define MESSAGE_INCOMPLETE 2
#define MESSAGE_TO_VECTOR_FAIL 3

class Command {
	private:
		Buffer &_buffer;
		std::vector<std::string> _tokens;
		unsigned int	_status;
		bool _validateCommand(std::string &command);
		void _commandStringToVector(std::string &);
		void _processVector();
		size_t _identifyNlCr(std::string &tempStr);
		void _removeParsedFromBuffer(size_t, std::string &);
	public:
		Command(std::string &buff);
		Command(Buffer &buff);
		std::vector<std::string> getTokens();
		unsigned int getStatus();
};
