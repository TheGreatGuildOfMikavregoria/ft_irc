#include "../Server.hpp"

void Server::who(Client& c, Command& cmd)
{
	const std::string nickName = c.getNickName();
	std::string rpl;
	Buffer &outBuf = c.getOutBuf();
	std::string mask;
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
	mask = cmd.getTokens()[1];
	if (Channel::hasChanPrefix(cmd.getTokens()[1]))
	{
		auto it = std::find_if(_channels.begin(), _channels.end(),
			[&mask](Channel &chan)
			{
				return (mask == chan.getName());
			});
		if (it != _channels.end())
			it->who(c);
	}
	else
	{
		int found = 0;
		for (std::unique_ptr<Client> &client : _clients)
		{
			if (client->getNickName() == mask)
			{
				client->who(c);
				found = 1;
			}
		}
		if (!found)
		{
			for (std::unique_ptr<Client> &client : _clients)
			{
				if (client->getNickName().substr(0, mask.length()) == mask && !client->hasMode(Client::ModeInvi))
					client->who(c);
			}
		}
	}
	rpl = numericRPL(RPL_ENDOFWHO, nickName, cmd.getTokens()[1]);
	outBuf.append(rpl.c_str(), rpl.length());
}

