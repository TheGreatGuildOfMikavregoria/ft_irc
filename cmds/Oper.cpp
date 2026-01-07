#include "../Server.hpp"

bool Server::isValidOperHost(const std::string &clientIP, int clientFD)
{
	struct sockaddr_in local_addr;
	socklen_t len = sizeof(local_addr);
	getsockname(clientFD, (struct sockaddr *)&local_addr, &len);
	std::string serverIP = inet_ntoa(local_addr.sin_addr);
	return (clientIP == "127.0.0.1" || clientIP == serverIP);
}

void Server::oper(Client& c, Command& cmd) {
	const std::string nickName = c.getNickName();
	Buffer& outBuf = c.getOutBuf();
	(void)outBuf;
	std::string rpl;
	if (cmd.getTokens().size() < 3)
		rpl = numericRPL(ERR_NEEDMOREPARAMS, nickName, cmd.getTokens().at(0));
	else if (cmd.getTokens().at(1) != OPER_NAME || cmd.getTokens().at(2) != OPER_PASS)
		rpl = numericRPL(ERR_PASSWDMISMATCH, nickName);
	else if (!this->isValidOperHost(c.getHostName(), c.getFd()))
		rpl = numericRPL(ERR_NOOPERHOST, nickName);
	else {
		rpl = numericRPL(RPL_YOUREOPER, nickName);
		outBuf.append(rpl.c_str(), rpl.length());
		if (!c.hasMode(Client::ModeOper)) {
			c.addMode(Client::ModeOper);
			rpl = ":" + nickName + " MODE " + nickName + " +o\r\n";
			serverBroadcast(rpl);
		}
		return;
	}
	outBuf.append(rpl.c_str(), rpl.length());
}