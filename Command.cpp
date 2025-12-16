#include "Command.hpp"

bool Command::_validateCommand(std::string &command)
{
	size_t i = 0;
	if (std::isalpha(command[0]))
	{
		while (i < command.length())
		{
			if (!std::isalpha(command[i]))
				return (false);
			i++;
		}
	}
	else if (std::isdigit(command[0]))
	{
		if (command.length() != 3)
			return (false);
		while (i < command.length())
		{
			if (!std::isdigit(command[i]))
				return (false);
			i++;
		}
	}
	return (true);	
}

size_t Command::_identifyNlCr(std::string &tempStr)
{

	size_t nlPosition = tempStr.find('\n');
	size_t crPosition = tempStr.find('\r');
	if (nlPosition == std::string::npos && crPosition == std::string::npos)
		return std::string::npos;
	if (nlPosition == std::string::npos)
		return crPosition;
	if (crPosition == std::string::npos)
		return nlPosition;
	if (crPosition < nlPosition)
		return crPosition;
	return nlPosition;
}

unsigned int Command::getStatus() { return _status; }

void Command::_removeParsedFromBuffer(size_t nlCrPosition, std::string &tempStr)
{
	size_t trailingNlCr = 0;
	while (nlCrPosition + trailingNlCr < tempStr.size() && (tempStr[nlCrPosition + trailingNlCr] == '\r' || tempStr[nlCrPosition + trailingNlCr] == '\n'))
	{
		trailingNlCr++;
	}
	_buffer.discard(nlCrPosition + trailingNlCr);

}

Command::Command(Buffer &buffer) : _buffer(buffer)
{
	std::string tempStr(_buffer.data(), _buffer.size());
	size_t nlCrPosition = _identifyNlCr(tempStr);
	if (nlCrPosition == std::string::npos)	
	{
		_status = MESSAGE_INCOMPLETE;
		return ;
	}
	std::string commandLine = tempStr.substr(0, nlCrPosition);
	// TODO: think through the behaviour if the message is invalid
	_commandStringToVector(commandLine);
	_removeParsedFromBuffer(nlCrPosition, tempStr);
	_status = MESSAGE_COMPLETE;
	_processVector();
}

void Command::_processVector()
{
	std::cout << "Command Vector:" << std::endl;
	for (std::string asd : _tokens)
	{
		std::cout << asd << std::endl;
	}
	std::cout << "Command Vector END" << std::endl;
}

void Command::_commandStringToVector(std::string &buffer)
{
	if (buffer.empty())
	{
		_status = MESSAGE_TO_VECTOR_FAIL;
		return ;
	}
	std::stringstream ss(buffer);
	std::string token;
	std::string trailingParam;
	int counter = 0;
	int trailing = 0;
	while (ss)
	{
		token = "";
		ss >> token;
		if (!token.length())
			break;
		if (!counter)
		{
			if (!_validateCommand(token))
			{
				_status = MESSAGE_TO_VECTOR_FAIL;
				return ;
			}
			_tokens.push_back(token);
		}
		if (counter)
		{
			if (token[0] == ':' && !trailing)
			{
				trailing = 1;
// TODO: consider different token sizes, 
				token = token.substr(1, token.length() - 1);
			}
			if (trailing)
			{
				if (trailingParam.length())
					trailingParam += " ";
				trailingParam += token;
			}
			else
				_tokens.push_back(token);
		}
		counter++;
	}
	if (trailing)
		_tokens.push_back(trailingParam);
}

std::vector<std::string> Command::getTokens()
{
	return _tokens;
}

std::string Command::getCommand()
{
	if (_tokens.size())
		return (_tokens[0]);
	return ("");
}

bool Command::stringWithinLength(std::string &str, size_t length)
{
	return (str.length() <= length);
}

bool Command::validateParamNum()
{
	if (Utils::stringToLowercase(this->getCommand()) == "nick")
	{
		return (_tokens.size() == 2);
	}
	if (Utils::stringToLowercase(this->getCommand()) == "ping")
	{
		return (_tokens.size() == 2);
	}
	return false;
}
