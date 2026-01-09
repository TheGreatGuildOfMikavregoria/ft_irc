#pragma once
#include <sstream>
#include <map>
#include <vector>
#include <cctype>
#include "Buffer.hpp"
#include "Utils.hpp"
#include <iostream>

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
		#if DEBUG
		void _processVector();
		#endif
		size_t _identifyNlCr(std::string &tempStr);
		void _removeParsedFromBuffer(size_t, std::string &);
	public:
		Command(Buffer &buff);
		std::vector<std::string> getTokens();
		unsigned int getStatus();
		bool stringWithinLength(std::string &, size_t );
		bool validateParamNum();
		std::string getCommand();
};
