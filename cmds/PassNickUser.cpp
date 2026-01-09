#include "../Server.hpp"

Client* Server::clientLookUp(const std::string& nickName) {

	for (auto& clientPtr : _clients) {
		if (clientPtr->getNickName() == nickName) {
			return clientPtr.get();
		}
	}
	return nullptr;
}

bool Server::isValidNickName(const std::string& nickName) {
	if (nickName.empty() || nickName.length() > NICKLEN) {
		return false;
	}
	if (std::isdigit(nickName[0])) {
		return false;
	}
	if (nickName[0] == '#' || nickName[0] == '&' || nickName[0] == ':') {
		return false;
	}
	for (size_t i = 0; i < nickName.length(); ++i) {
		char c = nickName[i];
		if (std::isspace(c)) {
			return false;
		}
		if (!std::isalnum(c) && c != '[' && c != ']' && c != '{' && c != '}' && 
			c != '\\' && c != '|' && c != '-' && c != '_' && c != '^') {
			return false;
		}
	}
	return true;
}

void Server::serverBroadcast(const std::string& msg) {
	for (auto& clientPtr : _clients) {
		if (clientPtr && clientPtr->getRegiStatus()) {
			clientPtr->getOutBuf().append(msg.c_str(), msg.length());
		}
	}
}

void Server::registerClient(Client& c) {
	const std::string nickName = c.getNickName();
	Buffer& outBuf = c.getOutBuf();
	std::string rpl;
	if (!c.getNickNameStatus() || !c.getUserNameStatus())
		return ;
	else {
		if (!c.getPasswordStatus()) {
			rpl = numericRPL(ERR_PASSWDMISMATCH, nickName);
			outBuf.append(rpl.c_str(), rpl.length());
			rpl = "Closing Link: " + c.getHostName() +  " (Bad password)";
			this -> error(c,rpl);
			c.setDisconnectFlag(true);
			return;
		}
		else {
			c.setRegiStatus(true);
			rpl = numericRPL(RPL_WELCOME, nickName, NETWORK_NAME, nickName, c.getUserName(), c.getHostName())
			+ numericRPL(RPL_YOURHOST, nickName, SERVER_NAME, VERSION)
			+ numericRPL(RPL_CREATED, nickName, getTimeCreated())
			+ numericRPL(RPL_MYINFO, nickName, SERVER_NAME, VERSION, INFO_USERMODES, INFO_CHANMODES, INFO_CHANMODEPARAM)
			+ numericRPL(RPL_ISUPPORT, nickName, ISUPPORT_TOKEN_LIST)
			+ numericRPL(ERR_NOMOTD, nickName);
		}
		outBuf.append(rpl.c_str(), rpl.length());
	}	
}

void Server::pass(Client& c, Command& cmd) {
	const std::string nickName = c.getNickName();
	Buffer& outBuf = c.getOutBuf();
	std::string rpl;
	if (cmd.getTokens().size() < 2)
		rpl = numericRPL(ERR_NEEDMOREPARAMS, nickName, cmd.getTokens().at(0));
	else if (c.getPasswordStatus() && c.getRegiStatus())
		rpl = numericRPL(ERR_ALREADYREGISTERED, nickName);
	else if (c.getPasswordStatus() && cmd.getTokens().at(1) != password)
		c.setPasswordStatus(false);
	else if (cmd.getTokens().at(1) == password)
		c.setPasswordStatus(true);
	outBuf.append(rpl.c_str(), rpl.length());
}

void Server::nick(Client& c, Command& cmd) {
	const std::string nickName = c.getNickName();
	Buffer& outBuf = c.getOutBuf();
	std::string rpl;
	if (cmd.getTokens().size() < 2)
		rpl = numericRPL(ERR_NONICKNAMEGIVEN, nickName);
	else {
		std::string newNickName = cmd.getTokens().at(1);
		if (!this -> isValidNickName(newNickName))
			rpl = numericRPL(ERR_ERRONEUSNICKNAME, nickName, newNickName);
		else if (this -> clientLookUp(newNickName))
			rpl = numericRPL(ERR_NICKNAMEINUSE, nickName, newNickName);
		else {
			c.setNickName(newNickName);
			c.setNickNameStatus(true);
			if (!c.getRegiStatus())
					this -> registerClient(c);
			else {
				//rpl = ":" + nickName + " NICK " + newNickName + "\r\n";//might have to change  the format of this later
				rpl = ":" + c.getSource() + " NICK " + newNickName + "\r\n";//might have to change  the format of this later
				serverBroadcast(rpl);
			}
			return;
		}
	}
	outBuf.append(rpl.c_str(), rpl.length());
}

void Server::user(Client& c, Command& cmd) {
	const std::string nickName = c.getNickName();
	Buffer& outBuf = c.getOutBuf();
	std::string rpl;
	if (cmd.getTokens().size() < 5 || cmd.getTokens().at(0).empty())
		rpl = numericRPL(ERR_NEEDMOREPARAMS, nickName, cmd.getTokens().at(0));
	else if (c.getRegiStatus())
		rpl = numericRPL(ERR_ALREADYREGISTERED, nickName);
	else if (c.getUserNameStatus())
		return;
	else {
		std::string userName = cmd.getTokens().at(1);
		for (char ch : userName) {
			if (!std::isalnum(static_cast<unsigned char>(ch))) {
				rpl = numericRPL(NTC_INVALIDUSRNAME, nickName);
				outBuf.append(rpl.c_str(), rpl.length());
				rpl = "Closing Link: " + c.getHostName() +  " (Invalid username)";
				this -> error(c,rpl);
				c.setDisconnectFlag(true);
				return;
			}
		}
		userName = "~" + userName.substr(0,USERLEN);
		c.setUserName(userName);
		c.setUserNameStatus(true);
		c.setRealName(cmd.getTokens().at(4));
		this -> registerClient(c);
		return;
	}
	outBuf.append(rpl.c_str(), rpl.length());
}

