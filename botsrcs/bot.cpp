#include <iostream>
#include "../Buffer.hpp"
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <stdexcept>


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

class Bot
{
	public:
		std::string _host;
		std::string _port;
		std::string _password;
		Buffer c;
		Bot(char **argv);
		~Bot();
		void run();
		void sendToClient(int fd,const  std::string &msg);
};

Bot::Bot(char **argv) : _host(argv[1]), _port(argv[2]), _password(argv[3]) {
	//_host = argv[1];
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
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
		throw std::runtime_error("socket() failed");

	sockaddr_in c_addr;
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(std::stoi(_port));

	int ret = inet_pton(AF_INET, _host.c_str(), &c_addr.sin_addr);
	if (ret < 0)
		throw std::runtime_error("conversion failed");

	int status = connect(socket_fd, (struct sockaddr*)&c_addr, sizeof(c_addr));
	if (status < 0)
		throw std::runtime_error("connect fail");
	
	sendToClient(socket_fd, std::string("PASS ") + _password);
	sendToClient(socket_fd, std::string("NICK bot"));
	sendToClient(socket_fd, std::string("USER bot 0 * :bot"));

	for(;;)
	{
		char buffer[4096];
		ssize_t received = recv(socket_fd, buffer, sizeof(buffer), 0);
		if (received <= 0)
			throw std::runtime_error("client recv");
		std::cout <<"BUFFER: "<< buffer << std::endl;
		c.append(buffer, static_cast<std::size_t>(received));


		while (1)
		{
			std::string line = pop_line(c);
			if (line.empty())
				break ;
			
			std::cout << "LINE: " << line << std::endl;
			if (line.rfind("PING", 0) == 0)
				sendToClient(socket_fd, "PONG");

		}
	}
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