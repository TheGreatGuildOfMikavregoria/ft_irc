#include "../Server.hpp"

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
		if (this -> clientLookUp(newNickName))
			rpl = numericRPL(ERR_NICKNAMEINUSE, nickName, newNickName);
		else if (!this -> isValidNickName(newNickName))
			rpl = numericRPL(ERR_ERRONEUSNICKNAME, nickName, newNickName);
		else {
			c.setNickName(newNickName);
			c.setNickNameStatus(true);
			rpl = nickName + " NICK " + newNickName + "\r\n";
			//pass rpl to channel and maybe return here
		}
	}
	outBuf.append(rpl.c_str(), rpl.length());
}