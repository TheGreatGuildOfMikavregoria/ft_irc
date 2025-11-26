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

void	Server::numericRPL(Client& c, char* format,  ...)
{
	std::string result = "";
	std::va_list args;
	va_start(args, format);
	for (const char* p = format; *p != '\0'; ++p)
	{
		if (*p == '%')
		{
			p++;
			if (*p == 's')
				result += (std::string)va_arg(args, const char*);
			
		} else if (*p)
		{
			result += *p;
		}
		
	}
	va_end(args);
	c.getOutBuf().append((char *)result.c_str(), result.length());
}

// void Server::pass(Client& c, Command& cmd) {
//     if (c.getPasswordStatus() == true) {
//         c.getOutBuf().append()
//     }
// }