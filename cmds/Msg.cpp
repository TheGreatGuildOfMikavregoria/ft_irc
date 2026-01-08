#include "../Server.hpp"

Channel* Server::getChannelByName(std::string &ch)
{
	for (auto &channel : _channels)
	{
		if (channel.getName() == ch)
			return &channel;
	}
	return NULL;
}

void Server::privmsg(Client &c, Command &cmd)
{
	auto tokens = cmd.getTokens();
	const std::string nickName = c.getNickName();
	Buffer &outBuf = c.getOutBuf();
	if (!c.getRegiStatus())
	{
		std::string rpl = numericRPL(ERR_NOTREGISTERED, nickName);
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}

	if (tokens.size() < 2)
	{
		std::string rpl = numericRPL(ERR_NORECIPIENT, nickName, "PRIVMSG");
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	auto &receiver = tokens[1];
	
	if (tokens.size() < 3)
	{
		std::string rpl = numericRPL(ERR_NOTEXTTOSEND, nickName);
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	auto &msg = tokens[2];

	if (tokens[1].find(',') != std::string::npos)
	{
		std::string rpl = numericRPL(ERR_TOOMANYTARGETS, nickName, "PRIVMSG");
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}

	#if DEBUG
	std::cout << "----MSG----"<< std::endl;
	auto debug{0};
	for (auto &token : tokens)
	{
		std::cout << debug++ << " "<< token << std::endl;
	}
	std::cout << "----MSG-END----"<< std::endl;
	#endif
	
	if (receiver.at(0) == '#')
	{
		Channel *channelPtr = getChannelByName(receiver);
		if (!channelPtr)
		{
			std::string rpl = numericRPL(ERR_NOSUCHCHANNEL, nickName, receiver);
			outBuf.append(rpl.c_str(), rpl.length());
			return ;
		}
		std::set<Channel*> &userChannels = c.getUserChannels();
		if (userChannels.find(channelPtr) == userChannels.end())
		{
			std::string rpl = numericRPL(ERR_CANNOTSENDTOCHAN, nickName, receiver);
			outBuf.append(rpl.c_str(), rpl.length());
			return ;
		}

		std::string payload = ":" + c.getNickName() + " PRIVMSG " + receiver + " :" + msg + "\r\n";
		channelPtr->chanBroadcast(c,payload);
	}
	else
	{
		auto target = clientLookUp(receiver);
		if (!target)
		{
			std::string rpl = numericRPL(ERR_NOSUCHNICK, nickName, target->getNickName());
			outBuf.append(rpl.c_str(), rpl.length());
			return ;
		}

		std::string payload = ":" + nickName + " PRIVMSG " + receiver + " :" + msg + "\r\n";
		target->getOutBuf().append(payload.c_str(), payload.size());
	}
	return ;
}
