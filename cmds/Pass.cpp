#include "../Server.hpp"

#define USERLEN 8
#define SERVER_NAME "ircserv"
#define NETWORK_NAME "42Net"
#define VERSION "1.0"

// void	Server::numericRPL(Client& c, const char* format,  ...) {
// 	std::string result = "ircserv";
// 	std::va_list args;
// 	va_start(args, format);
// 	for (const char* p = format; *p != '\0'; ++p)
// 	{
// 		if (*p == '%')
// 		{
// 			p++;
// 			if (*p == 's')
// 				result += (std::string)va_arg(args, const char*);
			
// 		} else if (*p)
// 		{
// 			result += *p;
// 		}
// 	}
// 	va_end(args);
// 	c.getOutBuf().append(result.c_str(), result.length());
// }

// Client* Server::clientLookUp(const std::string& nickName) {
// 	std::vector<Client>::iterator it;
// 	for (it = _clients.begin(); it != _clients.end(); ++it) {
// 		if (it->getNickName() == nickName) {
// 			return &(*it);
// 		}
// 	}
// 	return nullptr;
// }

Client* Server::clientLookUp(const std::string& nickName) {

	for (auto& clientPtr : _clients) {
		if (clientPtr->getNickName() == nickName) {
			return clientPtr.get();
		}
	}
	return nullptr;
}

bool Server::isValidNickName(const std::string& nickName) {
	if (nickName.empty()) {
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

// void Server::serverBroadcast(const std::string& msg) {
// 	std::vector<Client>::iterator it;
// 	for (it = _clients.begin(); it != _clients.end(); ++it)
// 		if ((*it).getRegiStatus())
// 			it->getOutBuf().append(msg.c_str(), msg.length());
// }

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
		return ;//Asuming ther's time out for registration
	else {
		if (!c.getPasswordStatus()) {
			rpl = numericRPL(ERR_PASSWDMISMATCH, nickName);
			outBuf.append(rpl.c_str(), rpl.length());
			// std::cout << "Dropping client (fd " << c.getFd() << "): password mismatch" << std::endl; //client doesn't get to print the meassage before onnection is closed.
			// close(c.getFd()); //check if drop client can be used here intead of repeating these lines
			c.setDisconnectFlag(true);
			return;
		}
		else {
			c.setRegiStatus(true);
			rpl = numericRPL(RPL_WELCOME, nickName, NETWORK_NAME, nickName, c.getUserName(), c.getHostName()) //change macros later
			+ numericRPL(RPL_YOURHOST, nickName, SERVER_NAME, VERSION) //change macros later 
			+ numericRPL(RPL_CREATED, nickName) //add datetime
			+ numericRPL(RPL_MYINFO, nickName) //add other params
			+ numericRPL(RPL_ISUPPORT, nickName); //add other params
		}
		outBuf.append(rpl.c_str(), rpl.length());
	}	
}

bool Server::isValidOperHost(const std::string &clientIP, int clientFD)
{
	struct sockaddr_in local_addr;
	socklen_t len = sizeof(local_addr);
	getsockname(clientFD, (struct sockaddr *)&local_addr, &len);
	std::string serverIP = inet_ntoa(local_addr.sin_addr);
	return (clientIP == "127.0.0.1" || clientIP == serverIP);
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
		else if (this -> clientLookUp(newNickName.substr(0,9)))
			rpl = numericRPL(ERR_NICKNAMEINUSE, nickName, newNickName);
		else {
			newNickName = newNickName.substr(0,9);
			c.setNickName(newNickName);
			c.setNickNameStatus(true);
			if (!c.getRegiStatus())
					this -> registerClient(c);
			else {
				rpl = ":" + nickName + " NICK " + newNickName + "\r\n";//might have to change  the format of this later
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
	else {
		std::string userName = cmd.getTokens().at(1);
		for (char c : userName) {
			if (c == '@')//ignores command silently. decide the behaviour.//what happens if user send USER again?
				return;
		}
		userName = "~" + userName.substr(0,USERLEN);//check if ~ should be included in USERLEN
		c.setUserName(userName);
		c.setUserNameStatus(true);
		c.setRealName(cmd.getTokens().at(4));
		this -> registerClient(c);
		return;
	}
	outBuf.append(rpl.c_str(), rpl.length());
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
}

// void Server::oper(Client& c, Command& cmd) {
// 	const std::string nickName = c.getNickName();
// 	Buffer& outBuf = c.getOutBuf();
// 	std::string rpl;
// 	if (cmd.getTokens().size() < 3)
// 		rpl = numericRPL(ERR_NEEDMOREPARAMS, nickName, cmd.getTokens().at(0));
// 	else if (cmd.getTokens().at(1) != OPER_NAME || cmd.getTokens().at(2) != OPER_PASS)
// 		rpl = numericRPL(ERR_PASSWDMISMATCH, nickName);
// 	else if (!this->isValidOperHost(c.getHostName(), c.getFd()))
// 		rpl = numericRPL(ERR_NOOPERHOST, nickName);
// 	else {
// 		rpl = numericRPL(RPL_YOUREOPER, nickName);
// 		outBuf.append(rpl.c_str(), rpl.length());
// 		if (c.getUserMode().find('o') == std::string::npos) {
// 			std::string strMode = c.getUserMode() + "o";
// 			c.setUserMode(strMode);
// 			rpl = ":" + nickName + " MODE " + nickName + " +o\r\n";
// 			serverBroadcast(rpl);
// 		}
// 		return;
// 	}
// 	outBuf.append(rpl.c_str(), rpl.length());
// }

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

void Server::quit(Client& c, Command& cmd) {
	const std::string nickName = c.getNickName();
	std::string reason;
	if (!(cmd.getTokens().size() < 2))
		reason = cmd.getTokens().at(1);
	// Buffer& outBuf = c.getOutBuf();
 	std::string rpl;
 	rpl = "Closing Link: " + nickName +  " (Quit: " + reason + "!)";
 	this -> error(c,rpl);
 	rpl = ":" + nickName + " QUIT :" +  reason + "\r\n";
 	std::set<Channel*>::iterator it;
 	for (it = c.getUserChannels().begin(); it != c.getUserChannels().end(); ++it) {
		Channel* chan = *it;
		chan->userRemove(c);
 		chan->chanBroadcast(c, rpl);
 	}
	c.setDisconnectFlag(true);
}

void Server::error(Client& c, const std::string& msg) {
	std::string err_msg;
	Buffer& outBuf = c.getOutBuf();
	err_msg = "ERROR :" + msg + "\r\n";
	outBuf.append(err_msg.c_str(), err_msg.length());
}

void Server::dropClient(Client& c)
{
	if (c.getFd() >= 0) //is this check needed? it is contructed with valid fd right?
		close(c.getFd());

	_clients.erase( 
		std::remove_if(_clients.begin(), _clients.end(),
		[&](const std::unique_ptr<Client>& p){
			return p.get() == &c;
		}),
		_clients.end());
}
