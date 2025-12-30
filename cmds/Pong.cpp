#include "../Server.hpp"

void Server::pong(Client &c, Command &cmd)
{
	(void)cmd;
	if (c.getWaitingPong() == false)
		return ;
	std::cout << c.getNickName() << " Ponged" << std::endl;
	c.setLastActivity(std::time(nullptr));
	c.setWaitingPong(false);
}

