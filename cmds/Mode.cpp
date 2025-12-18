#include "../Server.hpp"

void	Server::mode(Client& c, Command& cmd)
{
	const std::string nickName = c.getNickName();
	std::string target = cmd.getTokens().at(1);
	Buffer& outBuf = c.getOutBuf();
	std::string rpl;
	if (cmd.getTokens().size() < 2)
		rpl = numericRPL(ERR_NEEDMOREPARAMS, nickName, cmd.getTokens().at(0));
	if (Channel::hasChanPrefix(target)) {
		auto it = Utils::getChannelIteratorByChannelName( _channels, target);
		if (it == _channels.end())
			rpl = numericRPL(ERR_NOSUCHCHANNEL, nickName, target);
		if (cmd.getTokens().size() < 3) {
			rpl = numericRPL(RPL_CHANNELMODEIS, target, (*it).getChanMode(), (*it).getChanModeParams());
		}
	}
}