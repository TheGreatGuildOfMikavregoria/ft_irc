#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <cstddef>
#include <vector>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "Buffer.hpp"




///////////////////////
#if defined(DEBUG) && DEBUG
	#include <iostream>
	#define DBG(msg) do { std::cerr << "[DBG] " << msg << std::endl; } while(0)
#else
	#define DBG(msg) do {} while(0)
#endif
static inline const char* pollMaskStr(short ev) {
	static thread_local std::string s;
	s.clear();
	if (ev & POLLIN) s += "IN|";
	if (ev & POLLOUT) s += "OUT|";
	if (ev & POLLERR) s += "ERR|";
	if (ev & POLLHUP) s += "HUP|";
	if (ev & POLLNVAL) s += "NVAL|";
	if (!s.empty()) s.pop_back();
	return s.empty() ? "-" : s.c_str();
}
//////////////////////////

struct Conn
{
	int fd;
	Buffer in;
	Buffer out;

	std::string nice;
	std::string user;
};

class Server
{
private:
	int status; //I believed i needed at somepoint now i dont remember
	//TO be implemented:
	// std::string password; 
	// std::string port;

	std::vector<Conn> _clients;
	//std::vector<Channel> _channels;
	int _listenFd;

	void _startServerListener();
	void _runLoop();
	void dropClient(std::size_t index, const std::string &reason);
	void serviceClientRead(std::size_t index);
	void serviceClientWrite(std::size_t index);

public:
	Server();
	~Server();

	void start_server();
	bool set_nonblock(int fd);
};
