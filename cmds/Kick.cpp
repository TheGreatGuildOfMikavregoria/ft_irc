#include "../Server.hpp"

void Server::kick(Client &c, Command &cmd)
{
	const std::string nickName = c.getNickName();
	std::string rpl;
	Buffer &outBuf = c.getOutBuf();
	std::string reason;
	if (!c.getRegiStatus())
	{
		rpl = numericRPL(ERR_NOTREGISTERED, c.getNickNameStatus() ? c.getNickName() : c.getUserName());
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	if (cmd.getTokens().size() < 3)
	{
		rpl = numericRPL(ERR_NEEDMOREPARAMS, nickName, cmd.getTokens()[0]);
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	if (cmd.getTokens().size() > 3)
		reason = cmd.getTokens()[3];
	auto it = Utils::getChannelIteratorByChannelName( _channels, cmd.getTokens()[1]);
	if (it == _channels.end())
	{
		rpl = numericRPL(ERR_NOSUCHCHANNEL, nickName, cmd.getTokens()[1]);
		outBuf.append(rpl.c_str(), rpl.length());
		return;
	}
	#if DEBUG
	std::cout << "kick channel found" << std::endl;
	#endif
	it->kick(c, cmd.getTokens()[2], reason);
}
