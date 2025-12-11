

#include "Server.hpp"

#include <string>
#include "Utils.hpp"

int main(int ac, char **av)
{
	if (ac == 3)
	{
		try
		{
			std::string port(av[1]);
			std::string pw(av[2]);
			Server srv(port, pw);
			signal(SIGINT, Server::SignalHandler);
			signal(SIGQUIT, Server::SignalHandler);
			srv.start_server();
		}
		catch (std::exception &e)
		{
			std::cout << "Error: " << e.what() << std::endl;
		}
	}
	else
		std::cout << "./ircserv <port> <password>" << std::endl;
	return 0;
}
