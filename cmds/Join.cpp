#include "../Server.hpp"

std::vector<std::string> getSeparatedParams(std::string param)
{
	std::vector<std::string> params;
	
	size_t index =  param.find(',');
	if (index == std::string::npos)
	{
		params.push_back(param);
		return (params);
	}
	while (index != std::string::npos)
	{
		params.push_back(param.substr(0, index));
		param = param.substr(index + 1);
		index = param.find(',');
	}
	return params;
}

void Server::join(Client& c, Command& cmd) {
	const std::string nickName = c.getNickName();
	std::string rpl;
	auto outBuf = c.getOutBuf();
	if (!c.getRegiStatus())
	{
		rpl = numericRPL(ERR_NOTREGISTERED, c.getNickNameStatus() ? c.getNickName() : c.getUserName());
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	if (cmd.getTokens().size() == 1)
	{
		//some error
		//ERR_NEEDMOREPARAMS (461)
		rpl = numericRPL(ERR_NEEDMOREPARAMS, nickName, cmd.getTokens()[0]);
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	if (cmd.getTokens().size() == 2 && cmd.getTokens()[1] == "0")
	{
		//TODO: PART all user joined channels
		return ;
	}
	std::vector <std::string> channels = getSeparatedParams(cmd.getTokens()[1]);
	std::vector <std::string> keys;
	if (cmd.getTokens().size() == 3)
		keys = getSeparatedParams(cmd.getTokens()[2]);

	auto chanIterStart = channels.begin();
	auto keyIterStart = keys.begin();
	auto chanIterEnd = channels.end();
	auto keyIterEnd = keys.end();
	int status;
	for (; chanIterStart != chanIterEnd; ++chanIterStart, ++keyIterStart)
	{
		auto it = Utils::getChannelIteratorByChannelName( _channels, *chanIterStart);
		if (it == _channels.end())
		{
			//create and join
			if (!Channel::validateName(*chanIterStart))
			{
				//ERR_BADCHANMASK (476)
				rpl = numericRPL(ERR_BADCHANMASK, nickName, *chanIterStart);
				outBuf.append(rpl.c_str(), rpl.length());
				continue ;
			}
			// TODO: check chan limit
			//TODO: internall error check
			Channel newChan(*chanIterStart);
			_channels.push_back(newChan);
			it = Utils::getChannelIteratorByChannelName(_channels, *chanIterStart);
			status = it->join(c);
			continue;
		}
		else
		{

			if (keyIterStart < keyIterEnd)
				status = it->join(c, *keyIterStart);
			else
				status = it->join(c);
			// TODO: move to chan join
			if (status == 475)
			{
				rpl = numericRPL(ERR_BADCHANNELKEY, nickName, *chanIterStart);
				outBuf.append(rpl.c_str(), rpl.length());
				continue;
			}
			else if (status == 471)
			{
				rpl = numericRPL(ERR_CHANNELISFULL, nickName, *chanIterStart);
				outBuf.append(rpl.c_str(), rpl.length());
				continue;
			}
			else if (status == 473)
			{
				rpl = numericRPL(ERR_INVITEONLYCHAN, nickName, *chanIterStart);
				outBuf.append(rpl.c_str(), rpl.length());
				continue;
			}
			// TODO: check chan limit
			else if (status == 1)
			{
				//SUCCESS and join handled everything
			}
		}
	}
	return ;
}
