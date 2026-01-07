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
//      Command: PRIVMSG
//   Parameters: <target>{,<target>} <text to be sent>
void Server::privmsg(Client &c, Command &cmd)
{
	auto tokens = cmd.getTokens();
	Buffer &outBuf = c.getOutBuf();
	if (!c.getRegiStatus())
	{
		std::string rpl = numericRPL(ERR_NOTREGISTERED, c.getNickNameStatus() ? c.getNickName() : c.getUserName());
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}

	if (tokens.size() < 2)
	{
		std::string rpl = numericRPL(ERR_NORECIPIENT, c.getNickNameStatus() ? c.getNickName() : c.getUserName());
		outBuf.append(rpl.c_str(), rpl.length());
		return ; //411 ERR_NORECIPIENT (411)
	}
	auto &receiver = tokens[1];
	
	if (tokens.size() < 3)
	{
		std::string rpl = numericRPL(ERR_NOTEXTTOSEND, c.getNickNameStatus() ? c.getNickName() : c.getUserName());
		outBuf.append(rpl.c_str(), rpl.length());
		return ;//412 ERR_NOTEXTTOSEND (412)
	}
	auto &msg = tokens[2];

	if (tokens[1].find(',') != std::string::npos)
	{
		//std::string rpl = numericRPL(ERR_TOOMANYTARGETS, c.getNickNameStatus() ? c.getNickName() : c.getUserName());
//				outBuf.append(rpl.c_str(), rpl.length());
		return ;//407 ERR_TOOMANYTARGETS (407)
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
		//Send to channel
		Channel *channelPtr = getChannelByName(receiver);
		if (!channelPtr)
		{
			std::string rpl = numericRPL(ERR_NOSUCHCHANNEL, c.getNickNameStatus() ? c.getNickName() : c.getUserName());
			outBuf.append(rpl.c_str(), rpl.length());
			return ; //403
		}
		std::set<Channel*> &userChannels = c.getUserChannels();
		if (userChannels.find(channelPtr) == userChannels.end())
		{
			std::string rpl = numericRPL(ERR_CANNOTSENDTOCHAN, c.getNickNameStatus() ? c.getNickName() : c.getUserName());
			outBuf.append(rpl.c_str(), rpl.length());

			return ;//404; ERR_CANNOTSENDTOCHAN (404)
		}

		std::string payload = ":" + c.getNickName() + " PRIVMSG " + receiver + " :" + msg + "\r\n";
		channelPtr->chanBroadcast(c,payload);
	}
	else
	{
		auto target = clientLookUp(receiver);
		if (!target)
		{
			std::string rpl = numericRPL(ERR_NOSUCHNICK, c.getNickNameStatus() ? c.getNickName() : c.getUserName());
			outBuf.append(rpl.c_str(), rpl.length());
			return ;//401; ERR_NOSUCHNICK (401)
		}

		std::string payload = ":" + c.getNickName() + " PRIVMSG " + receiver + " :" + msg + "\r\n";
		target->getOutBuf().append(payload.c_str(), payload.size());
	}
	return ;
}
