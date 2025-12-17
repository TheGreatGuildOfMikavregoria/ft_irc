#include "../Server.hpp"

void Server::part(Client &c, Command &cmd)
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
	std::vector <std::string> channels = Utils::ft_split(cmd.getTokens()[1], ',');
	std::string reason;
	if (cmd.getTokens().size() == 3)
		reason = cmd.getTokens()[2];
	auto chanIterStart = channels.begin();
	auto keyIterStart = keys.begin();
	auto chanIterEnd = channels.end();
	auto keyIterEnd = keys.end();
	for (; chanIterStart != chanIterEnd; ++chanIterStart, ++keyIterStart)
	{
		auto it = Utils::getChannelIteratorByChannelName( _channels, *chanIterStart);
		if (it == _channels.end())
		{
			rpl = numericRPL(ERR_NOSUCHCHANNEL, nickName, *chanIterStart);
			outBuf.append(rpl.c_str(), rpl.length());
		}
		else
		{
			std::set<Channel *> &userChannels = c.getUserChannels();
			if (userChannels.count(channelPtr))
				it->part(c, reason);
			else
			{
				std::string rpl = numericRPL(ERR_NOTONCHANNEL, c.getNickName(), chanIterStart);
				outBuf.append(rpl.c_str(), rpl.length());
			}
		}
	}
}
