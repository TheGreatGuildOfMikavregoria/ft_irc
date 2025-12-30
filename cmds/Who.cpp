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
		std::cout << "who is channel" << std::endl;
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
		std::cout << "who is user" << std::endl;
		for (std::unique_ptr<Client> &client : _clients)
		{
			if (client->getNickName().substr(0, mask.length()) == mask)
			{
				std::cout << "mask match" << std::endl;
				client->who(c);
			}
		}
	}
	rpl = numericRPL(RPL_ENDOFWHO, nickName, cmd.getTokens()[1]);
	outBuf.append(rpl.c_str(), rpl.length());
}

