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
	(void)cmd;
	auto tokens = cmd.getTokens();
	if (tokens.size() < 3)
		return ;
	auto &receiver = tokens[1];
	auto &msg = tokens[2];
	//std::string &receiver = tokens[1];
	//std::cout << receiver<< std::endl;
	//std::string &msg = tokens[2];
	//for (auto token : tokens)
	//	std::cout << ":"<<token << std::endl;

	if (receiver.at(0) == '#')
	{
		//Send to channel
		std::set<Channel*> &userChannels = c.getUserChannels();
		Channel *channelPtr = getChannelByName(tokens[1]);
		//std::cout << "he2re"<< std::endl;

		if (userChannels.find(channelPtr) != userChannels.end())
		{
			std::string msgBuild = ":" + c.getNickName() + " PRIVMSG " + receiver + " :" + msg + "\r\n";
		//	std::cout << msg << std::endl;
			channelPtr->chanBroadcast(c,msgBuild);
//std::cout << "here"<< std::endl;
			return ;
		}
		else if (tokens[1].find(','))
			return ;//407
		else
			return ;//404;
	}
	else
	{
//std::cout << "wtf";
		if (auto target = clientLookUp(receiver))
		{
		//	std::cout << "wtf";

			std::string msgBuild = ":" + c.getNickName() + " PRIVMSG " + receiver + " :" + msg + "\r\n";

			target->getOutBuf().append(msgBuild.c_str(), msgBuild.size());
		}
		else
			return ;//401;
	}

	std::cout << tokens[1] << std::endl;
	auto inbuf = c.getInBuf();
}