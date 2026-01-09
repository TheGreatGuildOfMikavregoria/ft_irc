#include "../Server.hpp"

void Server::join(Client& c, Command& cmd) {
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
	if (cmd.getTokens().size() == 2 && cmd.getTokens()[1] == "0")
	{
		std::string reason;
		for (Channel &channel : _channels)
		{
			channel.part(c, reason);
		}
		return ;
	}
	std::vector <std::string> channels = Utils::ft_split(cmd.getTokens()[1], ',');
	std::vector <std::string> keys;
	if (cmd.getTokens().size() == 3)
		keys = Utils::ft_split(cmd.getTokens()[2], ',');

	auto chanIterStart = channels.begin();
	auto keyIterStart = keys.begin();
	auto chanIterEnd = channels.end();
	auto keyIterEnd = keys.end();
	for (; chanIterStart != chanIterEnd; ++chanIterStart, ++keyIterStart)
	{
		auto it = Utils::getChannelIteratorByChannelName( _channels, *chanIterStart);
		if (it == _channels.end())
		{
			if (!Channel::validateName(*chanIterStart))
			{
				rpl = numericRPL(ERR_BADCHANMASK, nickName, *chanIterStart);
				outBuf.append(rpl.c_str(), rpl.length());
				continue ;
			}
			Channel newChan(*chanIterStart);
			_channels.push_back(newChan);
			it = Utils::getChannelIteratorByChannelName(_channels, *chanIterStart);
			it->join(c, false);
			continue;
		}
		else
		{

			if (keyIterStart < keyIterEnd)
				it->join(c, *keyIterStart);
			else
				it->join(c, false);
		}
	}
	return ;
}
