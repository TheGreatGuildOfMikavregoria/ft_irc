#include "Server.hpp"

bool Server::_signal_int_quit = false;

void Server::SignalHandler(int signum)
{
	(void)signum;
	Server::_signal_int_quit = true;
	#if DEBUG
	std::cout << "Signal was clickered" << std::endl;
	#endif 
}

Server::Server(std::string port, std::string pw) : status(0), password(pw), port(port), _operName(OPER_NAME), _operPass(OPER_PASS), _clients(), _listenFd(-1)
{
// TODO:  protec?
	_timeCreated = Utils::getCurrentTimeString();
	_clients.reserve(MAX_CLIENTS);
	size_t endIdx;
	int portnum  = std::stoi(port, &endIdx);
	try
	{
		if (port[endIdx] != '\0')
			throw std::runtime_error("Invalid port character");
		if (portnum < 0 || portnum > 65535)
			throw std::runtime_error("Port out of range");
		_spareFd = open("/dev/null", O_RDONLY);
		if (_spareFd < 0)
			throw std::runtime_error("SpareFd opening failed");
	}
	catch(...) //Stoi throws here
	{
		throw std::runtime_error("Invalid port format");
	}
}

Server::~Server() {}

void Server::dropClient(std::size_t index, const std::string &reason)
{
	if (index >= _clients.size())
		return;

	Client &c = *_clients[index];
	auto channels = c.getUserChannels();
	for (Channel* channel : channels)
	{
// TODO: check if client->channelRemove needed
		if (channel)
			channel->userRemove(c);
	}
	if (!reason.empty())
		std::cout << "Dropping client (fd " << c.getFd() << "): " << reason << std::endl;

	if (c.getFd() >= 0)
		close(c.getFd());

	_clients.erase(_clients.begin() + index);
}

const std::string &Server::getTimeCreated()
{
	return _timeCreated;
}

void Server::sendToClient(int fd,const  std::string &msg)
{
	std::string payload = msg;
	if (payload.size() < 2 || payload.substr(payload.size() - 2) != "\r\n")
		payload += "\r\n";
	ssize_t sent = send(fd, payload.c_str(), payload.size(), 0);
	if (sent < 0)  // SEND check please remember me 
		throw std::runtime_error("send() failed");
}



bool Server::serviceClientRead(Client &c)
{

	char buffer[4096];
	ssize_t received = recv(c.getFd(), buffer, sizeof(buffer), 0);

	if (received <= 0)
		return false;

	c.setLastActivity(std::time(nullptr));
	c.getInBuf().append(buffer, static_cast<std::size_t>(received));

	while (1)
	{
		Command message(c.getInBuf());
		if (message.getStatus() == MESSAGE_COMPLETE)
		{
			_runCmd(c, message);
		}
		if (message.getStatus() == MESSAGE_INCOMPLETE)
			return true;
	}
}

bool Server::serviceClientWrite(Client &c)
{
	if (c.getOutBuf().empty())
		return true;

	ssize_t sent = send(c.getFd(), c.getOutBuf().data(), c.getOutBuf().size(), 0);
	if (sent <= 0)
		return false;

	c.getOutBuf().discard(static_cast<std::size_t>(sent));
	//disconnects the client after clearing the outbuf is discoonect flag is set
	if (c.getDisconnectFlag() && c.getOutBuf().empty()){
		return false;
		//this -> dropClient(c);
	}

	return true;
}

//fsf=file status flags, of the file, adds nonblocking into list
bool Server::set_nonblock(int fd)
{
	int fsf = fcntl(fd, F_GETFL, 0);
	return fsf >= 0 && fcntl(fd, F_SETFL, fsf | O_NONBLOCK) == 0;
}


void Server::handleClientEvents(std::vector<pollfd> &pfds)
{
	for (ssize_t i = pfds.size() - 1; i >=1; --i)
	{
		size_t index = i - 1;
		if (index >= _clients.size())
			continue ;
		Client *c = _clients[index].get();
		short retEvent = pfds[i].revents;
		if (retEvent & (POLLERR | POLLHUP | POLLNVAL)) //Error, Hangup, Invalid req.
		{
			dropClient(index, "Connection closed");
			continue;
		}
		if (retEvent & POLLIN) //Readale
		{
			if (!serviceClientRead(*c))
			{
				dropClient(index, "Connection closed");
				continue;
			}
		}
		if (retEvent & POLLOUT) //Writable
		{
			if (!serviceClientWrite(*c))
			{
				dropClient(index, "Connection closed");
				continue;
			}
		}
	}
}

void Server::serverAcceptClients()
{
	for (;;)
	{
		sockaddr_in c_addr;
		socklen_t c_len = sizeof(c_addr);
		std::memset(&c_addr, 0, sizeof(c_addr));

		int sock_fd = accept(_listenFd, reinterpret_cast<sockaddr *>(&c_addr), &c_len);
		if (sock_fd < 0)
		{

			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break ;
			if (errno == ECONNABORTED)
				continue ;
			if (errno == EMFILE || errno == ENFILE) //Testing some failsafes in case running out of fd's tldr should just keep under os ulimit
			{
				if (_spareFd >= 0)
				{
					close(_spareFd);
					_spareFd = -1;
					int temp = accept(_listenFd, reinterpret_cast<sockaddr *>(&c_addr), &c_len);
					if (temp >= 0)
						close(temp);
					_spareFd = open("/dev/null", O_RDONLY);
					if (_spareFd < 0)
						std::cout << "WARNING: spareFd failed to open\n";
					continue;
				}
				else if (!_clients.empty())
				{
					dropClient(0, "Emergency fd failsafe 2?");
					continue;
				}
			}
			perror("accept");
			break;
		}
		if (_clients.size() == MAX_CLIENTS)
		{
			sendToClient(sock_fd, "ERROR :Server full");
			close(sock_fd);
			continue ;
		}
		if (!set_nonblock(sock_fd))
		{
			close(sock_fd);
			continue ;
		}
		
		std::unique_ptr<Client> newClient(new Client(sock_fd));
		newClient->setHostName(inet_ntoa(c_addr.sin_addr));
		_clients.push_back(std::move(newClient));

		std::cout << "Accepted new client: " << sock_fd << std::endl;
	}
}


void Server::buildPollList(std::vector<pollfd> &pfds)
{
	pfds.clear();
	pfds.reserve(_clients.size() + 1);

	pfds.push_back(pollfd{_listenFd, POLLIN, 0});

	for (size_t i = 0; i < _clients.size(); ++i)
	{

		short event = POLLIN;
		if (!_clients[i]->getOutBuf().empty())
			event |= POLLOUT;
		pfds.push_back(pollfd{ _clients[i]->getFd(), event, 0});
	}
}

ssize_t Server::indexForFd(int fd)
{
	for (size_t i = 0; i < _clients.size(); ++i)
		if (_clients[i]->getFd() == fd)
			return static_cast<ssize_t>(i);
	return -1;
}

void Server::_runLoop()
{
	std::vector<pollfd> pfds;

	auto nextClientTimeout = [&]() -> int
	{
		if (_clients.empty())
			return -1;
		std::time_t now = std::time(nullptr);
		int timeout = -1;
		for (const auto &client : _clients)
		{
			int idleTime = client->getWaitingPong() ? CLIENT_TIMEOUT : CLIENT_PONG_WAITTIME;

			int remainingTime = idleTime -  static_cast<int>(std::difftime(now, client->getLastActivity()));;

			if (remainingTime <= 0)
				return 1000;
			if (timeout == -1 || remainingTime < timeout)
				timeout = remainingTime;
		}
		return timeout * 1000;
	};

	auto n{0};

	for (;;)
	{
		
		std::cout <<"------------------------ Loop: " <<n++ << "------------------------" <<std::endl;
		buildPollList(pfds);

		int pRes = poll(pfds.data(), pfds.size(), nextClientTimeout());
		if (pRes < 0)
		{
			if (errno == EINTR)
			{
				if (Server::_signal_int_quit)
					break;
				continue;
			}
			throw std::runtime_error("poll() failure");
		}

		for (ssize_t i = _clients.size() - 1; i >= 0; --i)
		{
			auto &client = _clients[i];
			if (client->getWaitingPong() == false && std::difftime(std::time(nullptr), client->getLastActivity()) > CLIENT_PONG_WAITTIME)
			{
				client->setWaitingPong(true);
				sendToClient(client->getFd(), "PING");
				std::cout << "Server has sent a ping request to: "<< client->getNickName() << std::endl;
			}
			else if (client->getWaitingPong() == true && std::difftime(std::time(nullptr), client->getLastActivity()) > CLIENT_TIMEOUT)
			{	
				sendToClient(client->getFd(), "Server: Client timed out (PONG check failed).");
				dropClient(static_cast<size_t>(i), "Client timeout");
			}

		//	std::cout << "Client " << client->getFd() << " timediff " << difftime(std::time(nullptr),client->getLastActivity())  << std::endl;
		}
		if (pfds[0].revents & POLLIN)
			serverAcceptClients();

		handleClientEvents(pfds);

		if (Server::_signal_int_quit)
			break;
	}

}


void Server::_startServerListener()
{

	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
		throw std::runtime_error("socket() failed");

	int opt = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");

	#if DEBUG
	std::cout << "Socket id:"<< socket_fd << std::endl;
	#endif

	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(std::stoi(port));
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(socket_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
	{
		close(socket_fd);
		throw std::runtime_error("bind() failed");
	}

	if (listen(socket_fd, 32000) < 0)
	{
		close(socket_fd);
		throw std::runtime_error("listen() failed");
	}

	if (!set_nonblock(socket_fd))
	{
		close(socket_fd);
		throw std::runtime_error("fcntl() failed to set non-blocking");
	}

	_listenFd = socket_fd;
	std::cout << "Server is listening" << std::endl;
}

void Server::start_server()
{
	try
	{
		#if DEBUG
		std::cout << "Trying to start Server\n";
		#endif

		_clients.reserve(MAX_CLIENTS);
		_startServerListener();
		_runLoop();

		#if DEBUG
		std::cout << "Serever is closing down" << std::endl;
		#endif

		for (ssize_t x = _clients.size() -1; x >= 0 ; --x)
		{
			sendToClient(_clients[x]->getFd(), "ERROR :Server is closing down");
			dropClient(x, "Server closing");
		}
		close(_spareFd);
		close(_listenFd);
	}
	catch (std::exception &e)
	{
		std::cout << "Server error: " << e.what() << std::endl;
	}
}

