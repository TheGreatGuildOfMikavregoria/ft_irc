#include "../Server.hpp"



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
		return ;
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
			+ numericRPL(RPL_CREATED, nickName, getTimeCreated())
			+ numericRPL(RPL_MYINFO, nickName, SERVER_NAME, VERSION, INFO_USERMODES, INFO_CHANMODES, INFO_CHANMODEPARAM)
			+ numericRPL(RPL_ISUPPORT, nickName, ISUPPORT_TOKEN_LIST) //add other params
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
		for (char ch : userName) {
			if (!std::isalnum(static_cast<unsigned char>(ch))) {
				rpl = numericRPL(NTC_INVALIDUSRNAME, nickName);
				outBuf.append(rpl.c_str(), rpl.length());
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
// wweerasi@c2r2p11 ~ % nc irc.libera.chat 6667
// :lead.libera.chat NOTICE * :*** Checking Ident
// :lead.libera.chat NOTICE * :*** Looking up your hostname...
// :lead.libera.chat NOTICE * :*** Couldn't look up your hostname
// :lead.libera.chat NOTICE * :*** No Ident response
// NICK s
// USER @s * 0 :S
// :lead.libera.chat NOTICE s :*** Your username is invalid. Please make sure that your username contains only alphanumeric characters.
// ERROR :Closing Link: 194.136.126.52 (Invalid username [~])


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


/*
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
*/
