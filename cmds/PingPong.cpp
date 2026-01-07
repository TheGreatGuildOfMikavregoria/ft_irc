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

void Server::ping(Client& c, Command& cmd) {
	const std::string nickName = c.getNickName();
	Buffer& outBuf = c.getOutBuf();
	std::string rpl;
	if (cmd.getTokens().size() < 2)
		rpl = numericRPL(ERR_NOORIGIN, nickName, cmd.getTokens().at(0));
	else
		rpl = ":" SERVER_NAME " PONG "  SERVER_NAME  " :" +  cmd.getTokens().at(1) + "\r\n";
	outBuf.append(rpl.c_str(), rpl.length());
	std::cout << "----Ping RPL ----" << std::endl; //is this required? //TODO: debug stuff remove later
	std::cout << c.getNickName() << " "<< rpl; //TODO: debug stuff remove later
	std::cout << "----Ping RPL END----" << std::endl; //TODO: debug stuff remove later
}