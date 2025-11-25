#include "Test.hpp"

//void Test::_ircFunc(Command &message, Client &current)

void Test::_some(std::string param)
{
	std::cout << "Some" << std::endl << "Internal data: " << _localData << std::endl << "Param: " << param << std::endl;
}
void Test::_asd(std::string param)
{
	std::cout << "Asd" << std::endl << "Internal data: " << _localData << std::endl << "Param: " << param << std::endl;
}
void Test::_wasd(std::string param)
{
	std::cout << "Wasd" << std::endl << "Internal data: " << _localData << std::endl << "Param: " << param << std::endl;
}

Test::Test()
{
	
}

void Test::runCmd(std::string &cmdName)
{
	auto cmd = _command_map.find(cmdName);
	if (cmd == _command_map.end())
	{
		std::cout << "Command not found" << std::endl;
	}
	else
		(this->*(cmd->second))("I am param");
}

/*
void Test::testRun()
{
	auto cmd = _command_map.find("SOME");
	if (cmd == _command_map.end())
	{
		std::cout << "Command not found" << std::endl;
	}
	else
		(this->*(cmd->second))("I am param");
}
*/
