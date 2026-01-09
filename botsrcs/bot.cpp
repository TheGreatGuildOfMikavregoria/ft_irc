#include "bot.hpp"

static volatile std::sig_atomic_t g_quit = 0;

static void SignalHandler(int)
{
	g_quit = 1;
}

static ssize_t findBrk( Buffer &b)
{
	const char *data = b.data();
	size_t n = b.size();

	for (size_t i = 0; i + 1 < n; ++i)
	{
		if (data[i] == '\r' && data[i + 1] == '\n')
			return i;
	}
	return -1;
}

static std::string pop_line(Buffer &b)
{
	auto pos = findBrk(b);
	if (pos < 0)
		return "";
	
	std::string line(b.data(), b.data() + pos);
	b.discard(pos + 2);
	return line;
}


void Bot::sendToClient(int fd,const  std::string &msg)
{
	std::string payload = msg;
	if (payload.size() < 2 || payload.substr(payload.size() - 2) != "\r\n")
		payload += "\r\n";
	ssize_t sent = send(fd, payload.c_str(), payload.size(), 0);
	if (sent < 0)  // SEND check please remember me 
		throw std::runtime_error("send() failed");
}

Bot::~Bot() {}

void Bot::run()
{
	struct sigaction sa;
	std::memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SignalHandler;
	sigaction(SIGINT, &sa, nullptr);
	sigaction(SIGQUIT, &sa, nullptr);


	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
		throw std::runtime_error("socket() failed");

	sockaddr_in c_addr;
	std::memset(&c_addr, 0, sizeof(c_addr));
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(std::stoi(_port));

	int ret = inet_pton(AF_INET, _host.c_str(), &c_addr.sin_addr);
	if (ret < 0)
		throw std::runtime_error("conversion failed");

	int status = connect(socket_fd, (struct sockaddr*)&c_addr, sizeof(c_addr));
	if (status < 0)
		throw std::runtime_error("connect fail");
	
	sendToClient(socket_fd, std::string("PASS ") + _password);
	sendToClient(socket_fd, std::string("NICK ") + _nick);
	sendToClient(socket_fd, std::string("USER ") + _nick + " 0 * :" + _nick);

	for(;;)
	{
		char buffer[4096];
		ssize_t received = recv(socket_fd, buffer, sizeof(buffer), 0);
		if (received <= 0)
		{
			if (g_quit)
				break;
			if (errno == EINTR)
				continue;
			throw std::runtime_error("client recv");
		}

		c.append(buffer, static_cast<std::size_t>(received));


		while (1)
		{
			if (g_quit)
				break;
			std::string line = pop_line(c);
			if (line.empty())
				break ;
			#if DEBUG
			std::cout << "LINE: " << line << std::endl;
			#endif
			if (line.rfind("PING", 0) == 0)
			{
				std::string token = line.size() > 5 ? line.substr(5) : "";
				sendToClient(socket_fd, "PONG " + token);
				continue ;
			}
			if (line.find(" PRIVMSG ") != std::string::npos)
			{
			
				size_t targetEnd = line.find(' ', 0) - 1;

				if (targetEnd != std::string::npos)
				{
					std::string target = line.substr(1, targetEnd);

					size_t msgPos = line.find(" :", targetEnd);
					if (msgPos != std::string::npos)
					{
						std::string msg = line.substr(msgPos + 2);
						if (msg == "!help" )
						{
							sendToClient(socket_fd, "PRIVMSG " + target + " :Available commands:");
							sendToClient(socket_fd, "PRIVMSG " + target + " : ∗ KICK - Eject a client from the channel");
							sendToClient(socket_fd, "PRIVMSG " + target + " : ∗ INVITE - Invite a client to a channel");
							sendToClient(socket_fd, "PRIVMSG " + target + " : ∗ TOPIC - Change or view the channel topic");
							sendToClient(socket_fd, "PRIVMSG " + target + " : ∗ MODE - Change the channel’s mode:");
							sendToClient(socket_fd, "PRIVMSG " + target + " : - i: Set/remove Invite-only channel");
							sendToClient(socket_fd, "PRIVMSG " + target + " : - t: Set/remove the restrictions of the TOPIC command to channel operators");
							sendToClient(socket_fd, "PRIVMSG " + target + " : - k: Set/remove the channel key (password)");
							sendToClient(socket_fd, "PRIVMSG " + target + " : - o: Give/take channel operator privilege");
						}
					}
				}
			}

			if (line.find(" 433 ") != std::string::npos || line.rfind("433 ", 0) == 0)
			{
				++_nick_attempt;
				_nick = _nick_base + "_" + std::to_string(_nick_attempt);
				sendToClient(socket_fd, std::string("NICK ") + _nick);
				continue ;
			}

		}
		
	}
	close(socket_fd);
}

int main(int ac, char **av)
{
	if (ac != 4){
		std::cout << "Usage: ./bot <host> <port> <password>"<< std::endl;
		return 0;
	}
	try
	{
		Bot b(av);
		b.run();
	}
	catch (...)
	{
		std::cout << "Bot gave up" << std::endl;
	}
}
