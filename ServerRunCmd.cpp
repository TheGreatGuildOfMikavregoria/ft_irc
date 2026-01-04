#include "Server.hpp"



void Server::_runCmd(Client &currentClient, Command &message)
{
//	//lazy
	removeEmptyChannels();
	auto cmd = _commandMap.find(Utils::stringToUppercase(message.getCommand()));
	if (cmd == _commandMap.end())
	{
		if (currentClient.getRegiStatus())
		{
			std::string rpl = numericRPL(ERR_UNKNOWNCOMMAND, currentClient.getNickName(), message.getCommand());
			currentClient.getOutBuf().append(rpl.c_str(), rpl.length());
		}
		return;
	}
	(this->*(cmd->second))(currentClient, message);
//	removeEmptyChannels();
}

void Server::removeEmptyChannels()
{
	_channels.erase(remove_if(_channels.begin(), _channels.end(), [](Channel &channel)
		{ return (channel.isEmpty()); } ), _channels.end());
}
/*
void Server::_testComm(Client &c, Command &message)
{
	(void)c;
	(void)message;
	std::cout << "this is a test command" << std::endl;
	std::string rpl = numericRPL(RPL_WELCOME, "nick", "networkname", "nick", "user", "host");
	c.getOutBuf().append(rpl.c_str(), rpl.length());
}
*/
