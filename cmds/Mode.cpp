#include "../Server.hpp"

void	Server::mode(Client& c, Command& cmd)
{
	const std::string nickName = c.getNickName();
	Buffer& outBuf = c.getOutBuf();
	std::string rpl;
	if (cmd.getTokens().size() < 2)
	rpl = numericRPL(ERR_NEEDMOREPARAMS, nickName, cmd.getTokens().at(0));
	
}