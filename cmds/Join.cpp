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
	if (cmd.length() == 1)
	{
		//some error
		return ;
	}
	std::vector <std::string> channels = getSeparatedParams(cmd[1]);
	if (cmd.length() == 2)
	{
		for (auto channelName : channels)
		{
			auto it = Utils::getChannelIteratorByChannelName( _channels, channelName);
			if (it == _channels.end)
			{
				_channels.push_back(Channel(*it));
				int status = _channels[_channels.size() - 1].join(c);
				if (status ==2324234234)
				{
					//todo: rpl something
				}
			}
		}
		//handle join with channel names only
		return ;
	}
	if (cmd.length() == 3)
	{	
		std::vector <std::string> keys = getSeparatedParams(cmd[2]);
		
		for (auto channelName : channels)
		{
			auto it = Utils::getChannelIteratorByChannelName( _channels, channelName);
			if (it == _channels.end)
			{
				_channels.push_back(Channel(*it));
				int status = _channels[_channels.size() - 1].join(c);
				if (status ==2324234234)
				{
					//todo: rpl something
				}
			}
		}
		//handle join with channel names only
		return ;
	}
}
