#include "../Server.hpp"

#define USERLEN 8
/*
PASS message
	Command: PASS
	Parameters: <password>
The PASS command is used to set a ‘connection password’. If set, the password must be set before any 
attempt to register the connection is made. This requires that clients send a PASS command before 
sending the NICK / USER combination.

The password supplied must match the one defined in the server configuration. It is possible to send 
multiple PASS commands before registering but only the last one sent is used for verification and it 
may not be changed once the client has been registered.

If the password supplied does not match the password expected by the server, then the server SHOULD 
send ERR_PASSWDMISMATCH (464) and MAY then close the connection with ERROR. Servers MUST send at least 
one of these two messages.

Servers may also consider requiring SASL authentication upon connection as an alternative to this, when 
more information or an alternate form of identity verification is desired.

Numeric replies:

	ERR_NEEDMOREPARAMS (461)
	ERR_ALREADYREGISTERED (462)
	ERR_PASSWDMISMATCH (464)

Command Example:
	PASS secretpasswordhere

*/

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

Client* Server::clientLookUp(const std::string& nickName) {
	std::vector<Client>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->getNickName() == nickName) {
			return &(*it);
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

void Server::registerClient(Client& c) {
	if ()
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
			else 
				rpl = nickName + " NICK " + newNickName + "\r\n";
			//pass rpl to all users and maybe return here
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
			if (c == '@')//cancel the 
				return;
		}
		userName = ("~" + userName).substr(0,USERLEN);
		c.setUserName(userName);
		c.setUserModeStatus(true);
		c.setRealName(cmd.getTokens().at(4));
		this -> registerClient(c);
		return;
	}
	outBuf.append(rpl.c_str(), rpl.length());
}