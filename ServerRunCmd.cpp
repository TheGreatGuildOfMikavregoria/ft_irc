#include "Server.hpp"



void Server::runCmd(Client &currentClient, Command &message)
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
