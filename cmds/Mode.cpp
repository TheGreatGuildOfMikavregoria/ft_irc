#include "../Server.hpp"

void	Server::mode(Client& c, Command& cmd)
{
	const std::string nickName = c.getNickName();
	const std::string target = cmd.getTokens().at(1);
	Buffer& outBuf = c.getOutBuf();
	std::string rpl;
	if (cmd.getTokens().size() < 2)
		rpl = numericRPL(ERR_NEEDMOREPARAMS, nickName, cmd.getTokens().at(0));
	if ()
}