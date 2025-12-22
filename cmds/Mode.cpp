#include "../Server.hpp"

std::string	Server::applyChanMode(Client& c, Command& cmd) {

}

void	Server::mode(Client& c, Command& cmd)
{
	const std::string nickName = c.getNickName();
	std::string target = cmd.getTokens().at(1);
	Buffer& outBuf = c.getOutBuf();
	std::string rpl;
	if (cmd.getTokens().size() < 2)
		rpl = numericRPL(ERR_NEEDMOREPARAMS, nickName, cmd.getTokens().at(0));
	else if (Channel::hasChanPrefix(target)) {
		auto it = Utils::getChannelIteratorByChannelName( _channels, target);
		if (it == _channels.end())
			rpl = numericRPL(ERR_NOSUCHCHANNEL, nickName, target);
		else if (cmd.getTokens().size() < 3) {
			rpl = numericRPL(RPL_CHANNELMODEIS, nickName, target, (*it).getChanMode(), (*it).getClientLimit());
			rpl += numericRPL(RPL_CREATIONTIME, nickName, target, (*it).getTimeCreated());
		}
		else {
			rpl = applyChanMode(c, Command& cmd)
		}
	}
}
