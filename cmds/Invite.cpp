#include "../Server.hpp"

void Server::invite(Client& c, Command& cmd) {
	
	const std::string nickName = c.getNickName();
	std::string rpl;
	Buffer &outBuf = c.getOutBuf();
	Client *invitee = nullptr;
	if (!c.getRegiStatus())
	{
		rpl = numericRPL(ERR_NOTREGISTERED, c.getNickNameStatus() ? c.getNickName() : c.getUserName());
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	if (cmd.getTokens().size() < 3)
	{
		rpl = numericRPL(ERR_NEEDMOREPARAMS, nickName, cmd.getTokens()[0]);
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}

	auto it = Utils::getChannelIteratorByChannelName( _channels, cmd.getTokens()[2]);
	if (it == _channels.end())
	{
		rpl = numericRPL(ERR_NOSUCHCHANNEL, nickName, cmd.getTokens()[2]);
		outBuf.append(rpl.c_str(), rpl.length());
		return;
	}
// TODO: think through caps
// invite-notify capability
	if (it->invite(c, cmd.getTokens()[1]))
	{
		invitee = clientLookUp(cmd.getTokens()[1]);
		if (invitee != nullptr)
		{
			std::cout << "found client" << std::endl;
			outBuf = invitee->getOutBuf();
			rpl = ":" + c.getNickName() + " INVITE " + cmd.getTokens()[1] + " " + cmd.getTokens()[2] + "\r\n";
			outBuf.append(rpl.c_str(), rpl.length());
			return;
		}
	}
/*	
	if (cmd.getTokens().size() == 2 && cmd.getTokens()[1] == "0")
	{
		std::string reason;
		//TODO: PART all user joined channels
		for (Channel &channel : _channels)
		{
			channel.part(c, reason);
		}
		return ;
	}
*/
}
