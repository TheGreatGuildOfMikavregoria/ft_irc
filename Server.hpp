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
#include <stdint.h>
#include <cstdarg>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "Buffer.hpp"
#include "Command.hpp"
#include "Client.hpp"
#include "NumericRPL.hpp"
#include <unordered_map>
#include <memory>
#include <csignal>
#include <ctime>
#define MAX_CLIENTS 512
#define CLIENT_TIMEOUT 600
#define OPER_NAME   "ircAdmin"
#define OPER_PASS	"admin@IRC42"

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

class Client;

class Server
{
private:
	
	const std::unordered_map<std::string, void (Server::*)(Client &, Command &)> _commandMap = {
	/*	{"SOME", &Server::_some},
		{"ASD", &Server::_asd},
		{"WASD", &Server::_wasd},
*/
		{"TEST", &Server::_testComm},
		{"PASS", &Server::pass},
		{"NICK", &Server::nick},
		{"USER", &Server::user},
		{"PING", &Server::ping},
		{"OPER", &Server::oper},
		{"QUIT", &Server::quit}
	};
	int status; //I believed i needed at somepoint now i dont remember
	std::string password; 
	std::string port;
	std::string _operName;
	std::string _operPass;
	static bool _signal;
	std::vector<std::unique_ptr<Client>> _clients;
	//std::vector<Channel> _channels;
	int _listenFd;
	int _spareFd;

	void _startServerListener();
	void _runLoop();
	void dropClient(std::size_t index, const std::string &reason);
	bool serviceClientRead(Client &c);
	bool serviceClientWrite(Client &c);
	// void processInput(std::string &buff, Client &conn);
	void sendToClient(int fd, const std::string &msg);
	void buildPollList(std::vector<pollfd> &pfds);
	void serverAcceptClients();
	void handleClientEvents(std::vector<pollfd> &pfds);
	void _runCmd(Client &, Command &);
	void _testComm(Client &cl, Command &message);

public:
	Server(std::string port, std::string pw);
	~Server();
	static void SignalHandler(int signum);
	void start_server();
	bool set_nonblock(int fd);
	// void	numericRPL(Client& c, const char* format,  ...);

	void	pass(Client& c, Command& cmd);
	void	nick(Client& c, Command& cmd);
	void	user(Client& c, Command& cmd);
	void	ping(Client& c, Command& cmd);
	void	oper(Client& c, Command& cmd);
	void	quit(Client& c, Command& cmd);

	Client*	clientLookUp(const std::string& nickName);
	bool	isValidNickName(const std::string& nickName);
	void	registerClient(Client& c);
	void	serverBroadcast(const std::string& msg);
	bool	isValidOperHost(const std::string &clientIP, int clientFD);
};
