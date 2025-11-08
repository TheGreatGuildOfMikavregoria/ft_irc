#include "Server.hpp"

bool Server::_signal = false;

void Server::SignalHandler(int signum)
{
	(void)signum;
	Server::_signal = true;
	#if DEBUG
	std::cout << "Signal was clickered" << std::endl;
	#endif 
}

Server::Server(std::string port, std::string pw) : status(0), password(pw), port(port), _clients(), _listenFd(-1)
{
	try
	{
		if (std::stoi(port) < 0 || std::stoi(port) > 65535)
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

// Irssi test log: 
// 0:46 -!- Irssi: The following settings were initialized    
// 10:46                        real_name
// 10:46                        user_name gtb
// 10:46                             nick gtb
// 10:46 -!- Irssi: Looking up localhost:6667
// 10:46 -!- Irssi: Unable to connect server localhost:6667    
//           port 6667 [Name or service not known]
// 10:46 -!- Irssi: Looking up localhost
// 10:46 -!- Irssi: Connecting to localhost [127.0.0.1] port   
//           6667
// // 10:46 Waiting for CAP LS response...


// Advertisement of sasl capability when authentication is unavailable
// Servers MUST NOT advertise the sasl capability if the authentication layer is unavailable.
// Servers MUST NAK any sasl capability request if the authentication layer is unavailable.
// If a client requires pre-authentication and is unable to obtain the sasl capability, then the client MUST disconnect and MAY retry the connection.


// 10:46 -!- Irssi: Connection to localhost established        
// 10:46 -!- Irssi: (default) warning Unhandled CAP subcommand           302
// 10:46 -!- Irssi: (GLib) critical g_ascii_strcasecmp:        
//           assertion 's1 != NULL' failed
// 10:46 -!- Irssi: (default) critical netsplit_find:
//           assertion 'nick != NULL' failed
// 10:46 -!- Irssi: (default) critical netjoin_find: assertion           'nick != NULL' failed
// 10:46 -!- Irssi: (default) critical sig_message_join:       
//           assertion 'nick != NULL' failed
// 10:46 -!- Irssi: (default) critical event_join: assertion   
//           'nick != NULL' failed
// 10:46 -!- Irssi: (GLib) critical g_ascii_strcasecmp:        
//           assertion 's1 != NULL' failed


//Messages management is not done
//And channels i guess

//  SOLID?
// Possible Decomposition

// Server (Server.hpp:17)
// Acts as the high-level coordinator: bootstraps listening socket, starts the event loop, owns the Accept/Loop components. Keeps lifecycle concerns in one place.

// Listener/Acceptor
// Encapsulates socket creation, bind, listen, non-block setup, and accept(). Server asks it for a ready client fd. Keeps low-level socket API isolated and testable.

// Poller/EventLoop
// Wraps poll() state (std::vector<pollfd>) plus helpers like registerFd, updateInterest, and wait(). Exposes results as readable enums so the rest of the code avoids bit-masks.

// ClientSession (Conn replacement)
// Owns a single peer’s fd, inbound/outbound Buffers, metadata (user, etc.), and operations like queueSend, consumeInput, close(). Server stores these and delegates read/write handling.

// Buffer (Buffer.hpp:9)
// Already split. Expand with protocol-aware helpers (peek line, trim) if needed; keeps byte juggling out of sessions.

// Protocol/Command Handler
// Parses ClientSession input, applies business rules, and uses session/Server APIs to respond or broadcast. Keeps application logic away from networking.

// Utility Modules
// Small helpers for logging, error propagation, configuration (port, backlog, timeout).


void Server::dropClient(std::size_t index, const std::string &reason)
{
	if (index >= _clients.size())
		return;

	Conn &conn = _clients[index];
	if (!reason.empty())
		std::cout << "Dropping client (fd " << conn.fd << "): " << reason << std::endl;

	if (conn.fd >= 0)
		close(conn.fd);

	_clients.erase(_clients.begin() + index);
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

// void Server::processInput(std::string &buff, Conn &conn)
// {
// 	if (buff.empty())
// 		return;

// 	const std::string::size_type pos = buff.find(' ');
// 	if (pos == std::string::npos)
// 		return ;
	
// 	std::string pre = buff.substr(0, pos - 1);
// 	std::string post = buff.substr(pos + 1);
// 	std::cout << "To make sense of this : "<< buff << " " << pre << " " << post << std::endl;
// 	if (pre == "CAP")
// 		return;
// 	else if (pre == "JOIN")
// 	{
// 		std::cout << "FML";
// 		sendToClient(conn.fd, "Join test");
// 		return;
// 	}
// 	else if (pre == "NICK")
// 		return;
// 	else if (pre == "USER")
// 		return;

// }



void Server::serviceClientRead(std::size_t index)
{
	if (index >= _clients.size())
		return;

	Conn &conn = _clients[index];
	char buffer[4096];
	ssize_t received = recv(conn.fd, buffer, sizeof(buffer), 0);

	if (received <= 0)
	{
		dropClient(index, received == 0 ? "Peer disconnected" : "Read error");
		return;
	}

	//conn.out.append(buffer, static_cast<std::size_t>(received));// for debugging
	conn.in.append(buffer, static_cast<std::size_t>(received));
	// while (!conn.in.empty())
	// {
	// 	std::string data = conn.in.str();
	// 	std::size_t pos = data.find('\n');
	// 	if (pos == std::string::npos)
	// 		break ;

	// 	std::string line = data.substr(0, pos);
	// 	if (!line.empty() && line.back() == '\r')
	// 		line.pop_back();

	// 	conn.in.discard(pos + 1); 

	// 	if (!line.empty())
	// 		processInput(line, conn);
	// }

	//
}

void Server::serviceClientWrite(std::size_t index)
{
	if (index >= _clients.size())
		return;

	Conn &conn = _clients[index];
	if (conn.out.empty())
		return;

	ssize_t sent = send(conn.fd, conn.out.data(), conn.out.size(), 0);
	if (sent <= 0)
	{
		dropClient(index, "Write error");
		return;
	}

	conn.out.discard(static_cast<std::size_t>(sent));
}

//fsf=file status flags, of the file, adds nonblocking into list
bool Server::set_nonblock(int fd)
{
	int fsf = fcntl(fd, F_GETFL, 0);
	return fsf >= 0 && fcntl(fd, F_SETFL, fsf | O_NONBLOCK) == 0;
}

/*NAME         top
       poll, ppoll - wait for some event on a file descriptor
LIBRARY         top
       Standard C library (libc, -lc)
SYNOPSIS         top
       #include <poll.h>

       int poll(struct pollfd *fds, nfds_t nfds, int timeout);

       #define _GNU_SOURCE          See feature_test_macros(7) 
       #include <poll.h>
DESCRIPTION         top
       poll() performs a similar task to select(2): it waits for one of a
       set of file descriptors to become ready to perform I/O.  The
       Linux-specific epoll(7) API performs a similar task, but offers
       features beyond those found in poll().

       The set of file descriptors to be monitored is specified in the
       fds argument, which is an array of structures of the following
       form:

           struct pollfd {
               int   fd;          file descriptor 
               short events;      requested events 
               short revents;     returned events 
           };
		   On success, poll() returns a nonnegative value
		   which is the number
       of elements in the pollfds whose revents fields have been set to a
       nonzero value (indicating an event or an error).  A return value
       of zero indicates that the system call timed out before any file
       descriptors became ready.
*/
		// POLLIN

		// listener: at least one pending connection → call accept().
		
		// client: readable or remote closed (you’ll see recv() == 0 for EOF).
		
		// POLLOUT
		
		// client is writable (send buffer has space). try send().
		
		// because TCP sockets are often writable, you only request this when you have bytes queued to avoid wakeups.
		
		// POLLHUP
		
		// hangup: peer closed the connection. often comes with/after POLLIN (where recv()==0). treat as “close it.”
		
		// POLLERR
		
		// error occurred (e.g., ICMP error surfaced). you can getsockopt(SO_ERROR) for details, but in a simple server just drop the client.
		
		// POLLNVAL
		
		// invalid fd (not open or never valid). programming bug: close it and remove.
		
		// POLLPRI
		
		// “out-of-band”/urgent data (rare for TCP in modern apps). usually ignore.
		
		// linux has POLLRDHUP (peer closed its write end). handy if you want to detect half-closes; not strictly required.

		// Checks the revents of clients and acts according to the returned event.
void Server::handleClientEvents(std::vector<pollfd> &pfds)
{
	for (ssize_t i = pfds.size() - 1; i >=1; --i)
	{
		size_t index = i - 1;
		if (index >= _clients.size())
			continue ;

		short _retEvent = pfds[i].revents;
		if (_retEvent & (POLLERR | POLLHUP | POLLNVAL)) //Error, Hangup, Invalid req.
		{
			dropClient(index, "Connection closed");
			continue;
		}
		if (_retEvent & POLLIN) //Readale
		{
			serviceClientRead(index);
		}
		if (_retEvent & POLLOUT) //Writable
		{
			serviceClientWrite(index);
		}
	}
}
#include <string.h>
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
			#if DEBUG
			std::cout << std::strerror(errno);
			std::cout << " ;;;too much bs\n";
			#endif
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break ;
			if (errno == ECONNABORTED)
				continue ;
			if (errno == EMFILE || errno == ENFILE) //Testing some failsafes in case running out of fd's tldr should just keep under os ulimit
			{

				//usleep(10);
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
		Conn c;
		c.fd = sock_fd;
		_clients.push_back(c);

		std::cout << "Accepted new client: " << sock_fd << std::endl;
	}
}

//tldr, part that checks if (in) from server perspctive, 
//do we have data to read from client and out do we have data to send to
//client e.g. in = client prompt, out = channel msgs
void Server::buildPollList(std::vector<pollfd> &pfds)
{
	pfds.clear();
	pfds.reserve(_clients.size() + 1);

	pfds.push_back(pollfd{_listenFd, POLLIN, 0});

	for (size_t i = 0; i < _clients.size(); ++i)
	{

		short event = POLLIN;
		if (!_clients[i].out.empty()) 
			event |= POLLOUT;
		pfds.push_back(pollfd{ _clients[i].fd, event, 0});
	}
}

void Server::_runLoop()
{
	std::vector<pollfd> pfds;

	for (;;)
	{
		buildPollList(pfds);

		DBG("pfds=" << pfds.size()
			<< " clients=" << _clients.size());
		
		int pRes = poll(pfds.data(), pfds.size(), 0);
		if (pRes < 0)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error("poll() failure");
		}
		//if (pRes == 0) // noevents = goes straight to timeout 
		//	continue;  

		DBG("poll() ready=" << pRes
			<< " listen.revents=" << pollMaskStr(pfds[0].revents));

		if (pfds[0].revents & POLLIN)
			serverAcceptClients();

		handleClientEvents(pfds);

		#if DEBUG
		for(int i = 0; i < _clients.size(); i++)
		{
			std::cout << "Out: [" << i << "] ";
			Buffer &temp = _clients[i].out;
			if (temp.empty())
				continue;
			std::string a(temp.data(), temp.size());
			std::cout << a;
			std::cout <<std::endl;
		}
		for(int i = 0; i < _clients.size(); i++)
		{
			std::cout << "In: [" << i << "] ";
			Buffer &temp = _clients[i].in;
			if (temp.empty())
				continue;
			std::string a(temp.data(), temp.size());
			std::cout  << a;
			std::cout <<std::endl;
		}
		#endif

		if (Server::_signal == true)
			break;
	}

}

	// socket() creates an endpoint for communication and returns a file
	//    descriptor that refers to that endpoint.        int socket(int domain, int type, int protocol);


// 	int setsockopt(int socket, int level, int option_name,
//        const void *option_value, socklen_t option_len);
// 		The setsockopt() function shall set the option specified by the option_name argument, at the protocol level specified by the level argument, to the value pointed to by the option_value argument for the socket associated with the file descriptor specified by the socket argument.

// The level argument specifies the protocol level at which the option resides.
//  To set options at the socket level, specify the level argument as SOL_SOCKET.
//   To set options at other levels, supply the appropriate level identifier for 
//   the protocol controlling the option. For example, to indicate that an option
//    is interpreted by the TCP (Transport Control Protocol), set level to 
//    IPPROTO_TCP as defined in the <netinet/in.h> header.

	// struct sockaddr_in {
	// 	sa_family_t     sin_family;     /* AF_INET */
	// 	in_port_t       sin_port;       /* Port number */
	// 	struct in_addr  sin_addr;       /* IPv4 address */
	// };

	// struct in_addr {
	// 	in_addr_t s_addr;
	// };

	//        bind - bind a name to a socket
	//  When a socket is created with socket(2), it exists in a name space
	//        (address family) but has no address assigned to it.  bind()
	//        assigns the address specified by addr to the socket referred to by
	//        the file descriptor sockfd.  addrlen specifies the size, in bytes,
	//        of the address structure pointed to by addr.  Traditionally, this
	//        operation is called “assigning a name to a socket”.
	//        int bind(int sockfd, const struct sockaddr *addr,
	//                 socklen_t addrlen);

// SO_REUSEADDR
// Specifies that the rules used in validating 
// addresses supplied to bind() should allow reuse of 
// local addresses, if this is supported by the protocol. 
// This option takes an int value. This is a Boolean option.


	//    listen - listen for connections on a socket
	//    int listen(int sockfd, int backlog);
	//    listen() marks the socket referred to by sockfd as a passive
	//    socket, that is, as a socket that will be used to accept incoming
	//    connection requests using accept(2).

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

		_startServerListener();
		_runLoop();

		#if DEBUG
		std::cout << "Serever is closing down" << std::endl;
		#endif

		for (ssize_t x = _clients.size() -1; x >= 0 ; --x)
		{
			sendToClient(_clients[x].fd, "ERROR :Server is closing down");
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

