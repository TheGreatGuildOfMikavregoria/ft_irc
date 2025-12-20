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
	it->kick(c, cmd.getTokens()[2], reason);
/*
	std::set<Channel *> &userChannels = c.getUserChannels();
	Channel &chanRef = *it;
	if (userChannels.count(&chanRef))
		it->kick(c, reason);
	else
	{
		std::string rpl = numericRPL(ERR_NOTONCHANNEL, c.getNickName(), *chanIterStart);
		outBuf.append(rpl.c_str(), rpl.length());
	}
*/
}
