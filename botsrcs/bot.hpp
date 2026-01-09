
#pragma once

#include <iostream>
#include "../Buffer.hpp"
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <stdexcept>
#include <cerrno>
#include <csignal>

class Bot
{
	public:
		std::string _host;
		std::string _port;
		std::string _password;
		std::string _nick_base;
		std::string _nick;
		size_t _nick_attempt;
		Buffer c;
		Bot(char **argv) : _host(argv[1]), _port(argv[2]), _password(argv[3]), _nick_base("bot"), _nick("bot"), _nick_attempt(0) {}

		~Bot();
		void run();
		void sendToClient(int fd,const  std::string &msg);
};