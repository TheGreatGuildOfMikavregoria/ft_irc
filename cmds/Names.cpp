#include "../Server.hpp"

void Server::names(Client& c, Command& cmd) {
	const std::string nickName = c.getNickName();
	std::string rpl;
	Buffer &outBuf = c.getOutBuf();
	if (!c.getRegiStatus())
	{
		rpl = numericRPL(ERR_NOTREGISTERED, c.getNickNameStatus() ? c.getNickName() : c.getUserName());
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	if (cmd.getTokens().size() < 2)
	{
		rpl = numericRPL(ERR_NEEDMOREPARAMS, nickName, cmd.getTokens()[0]);
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	std::vector <std::string> channels = Utils::ft_split(cmd.getTokens()[1], ',');
//	auto chanIterStart = channels.begin();
//	auto chanIterEnd = channels.end();
	for (std::string &channelName : channels)
	{
		auto it = Utils::getChannelIteratorByChannelName( _channels, channelName);
		if (it == _channels.end())
		{
			rpl = numericRPL(RPL_ENDOFNAMES, c.getNickName(), channelName);
			outBuf.append(rpl.c_str(), rpl.length());
		}
		else
			it->names(c);
	}
	return ;
}
