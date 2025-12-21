#include "Server.hpp"



void Server::_runCmd(Client &currentClient, Command &message)
{
// TODO: all cmds in uppercase?
	auto cmd = _commandMap.find(Utils::stringToUppercase(message.getCommand()));
	if (cmd == _commandMap.end())
	{
// TODO:  appropriate response?
		std::cout << "Command not found" << std::endl;
		return;
	}
//	else
	(this->*(cmd->second))(currentClient, message);
}

void Server::_testComm(Client &c, Command &message)
{
	(void)c;
	(void)message;
	std::cout << "this is a test command" << std::endl;
	std::string rpl = numericRPL(RPL_WELCOME, "nick", "networkname", "nick", "user", "host");
	c.getOutBuf().append(rpl.c_str(), rpl.length());
}
