#pragma once

#include <algorithm> //for remove_if
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
#include "Channel.hpp"
#include "NumericRPL.hpp"
#include <unordered_map>
#include <memory>
#include <csignal>
#include <ctime>
#include <list>
#define MAX_CLIENTS 512
#define CLIENT_TIMEOUT 60
#define CLIENT_PONG_WAITTIME CLIENT_TIMEOUT / 10 * 9
#define SERVER_NAME "ircserv"
#define OPER_NAME   "ircAdmin"
#define OPER_PASS	"admin@IRC42"

class Client;
class Command;
class Channel;
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
		{"QUIT", &Server::quit},
		{"JOIN", &Server::join},
		{"PART", &Server::part},
		{"TOPIC", &Server::topic},
		{"INVITE", &Server::invite},
		{"MODE", &Server::mode},
		{"KICK", &Server::kick},
		{"WHO", &Server::who},
		{"PRIVMSG", &Server::privmsg},
		{"PONG", &Server::pong},
	};

	int status; //I believed i needed at somepoint now i dont remember
	std::string password; 
	std::string port;
	std::string _operName;
	std::string _operPass;
	static bool _signal_int_quit;
	std::vector<std::unique_ptr<Client>> _clients;
	std::list<Channel> _channels;
	int _listenFd;
	int _spareFd;
	std::string _timeCreated;

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
	ssize_t indexForFd(int fd);
public:
	Server(std::string port, std::string pw);
	~Server();
	static void SignalHandler(int signum);
	void start_server();
	bool set_nonblock(int fd);
	// void	numericRPL(Client& c, const char* format,  ...);

	void	pass(Client& c, Command& cmd);
	std::string &getTimeCreated();
	void	nick(Client& c, Command& cmd);
	void	user(Client& c, Command& cmd);
	void	ping(Client& c, Command& cmd);
	void	oper(Client& c, Command& cmd);
	void	quit(Client& c, Command& cmd);
	void	error(Client& c, const std::string& msg);
	void	join(Client& c, Command& cmd);
	void	part(Client& c, Command& cmd);
	void	topic(Client &c, Command &cmd);
	void	invite(Client &c, Command &cmd);
	void	mode(Client& c, Command& cmd);
	void	kick(Client& c, Command& cmd);
	void	who(Client& c, Command& cmd);

	Client*	clientLookUp(const std::string& nickName);
	bool	isValidNickName(const std::string& nickName);
	void	registerClient(Client& c);
	void	serverBroadcast(const std::string& msg);
	bool	isValidOperHost(const std::string &clientIP, int clientFD);
	void	dropClient(Client& c);
	void	privmsg(Client &c, Command &cmd);
	Channel* getChannelByName(std::string &ch);
	void pong(Client &c, Command &cmd);


};
