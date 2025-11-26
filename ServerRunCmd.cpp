#include "Server.hpp"



void Server::_runCmd(Client &currentClient, Command &message)
{
// TODO: all cmds in uppercase?
	auto cmd = _commandMap.find(message.getCommand());
	if (cmd == _commandMap.end())
	{
// TODO:  appropriate response?
		std::cout << "Command not found" << std::endl;
		return;
	}
//	else
	(this->*(cmd->second))(currentClient, message);
}

void Server::_testComm(Client &cl, Command &message)
{
	(void)cl;
	(void)message;
	std::cout << "this is a test command" << std::endl;
	numericRPL(cl, (char *)RPL_WELCOME, "ircserv", "nick", "networkname", "nick", "user", "host");
}
