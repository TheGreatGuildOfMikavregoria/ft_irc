#include "../Server.hpp"

void Server::topic(Client &c, Command &cmd)
{
	const std::string nickName = c.getNickName();
	std::string rpl;
	Buffer &outBuf = c.getOutBuf();
	if (!c.getRegiStatus())
	{
		rpl = numericRPL(ERR_NOTREGISTERED, c.getNickNameStatus() ? c.getNickName() : c.getUserName());
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	if (cmd.getTokens().size() == 1)
	{
		rpl = numericRPL(ERR_NEEDMOREPARAMS, nickName, cmd.getTokens()[0]);
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	auto it = Utils::getChannelIteratorByChannelName( _channels, cmd.getTokens()[1]);
	if (it == _channels.end())
	{
		rpl = numericRPL(ERR_NOSUCHCHANNEL, c.getNickName(), cmd.getTokens()[1]);
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	std::set<Channel *> &userChannels = c.getUserChannels();
	Channel &chanRef = *it;
	if (!userChannels.count(&chanRef))
	{
		std::string rpl = numericRPL(ERR_NOTONCHANNEL, c.getNickName(), cmd.getTokens()[1]);
		outBuf.append(rpl.c_str(), rpl.length());
	}
	else
	{
		if (cmd.getTokens().size() > 2)
			it->topic(c, cmd.getTokens()[2]);
		else
			it->topic(c, false);
	}
	
}
