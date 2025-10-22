#include "Server.hpp"


Server::Server() : status(0), _clients(), _listenFd(-1) {}

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
// 10:46 Waiting for CAP LS response...
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

	conn.in.append(buffer, static_cast<std::size_t>(received));
	conn.out.append(buffer, static_cast<std::size_t>(received));// for debugging
	//processInput();

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

	conn.out.disgard(static_cast<std::size_t>(sent));
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

       #define _GNU_SOURCE         /* See feature_test_macros(7) 
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
               int   fd;         /* file descriptor 
               short events;     /* requested events 
               short revents;    /* returned events 
           };
		   On success, poll() returns a nonnegative value
		   which is the number
       of elements in the pollfds whose revents fields have been set to a
       nonzero value (indicating an event or an error).  A return value
       of zero indicates that the system call timed out before any file
       descriptors became ready.
*/

void Server::_runLoop()
{
	std::vector<pollfd> pfds;
	//std::vector<int> clients;
	//const int listen_fd = _ListenFd;

	for (;;)
	{
		//Building poll list
		pfds.clear();
		pfds.push_back(pollfd{_listenFd, POLLIN, 0}); // Adds server listener to the list

		for (size_t i = 0; i < _clients.size(); ++i) //Iter through clients
		{
			//tldr, part that checks if (in) from server perspctive, 
			//do we have data to read from client and out do we have data to send to
			//client e.g. in = client prompt, out = channel msgs
			short event = POLLIN; //Requested event set, so if data is written we will read/bewoken up to read, so we call recv maybe?
			if (!_clients[i].out.empty()) 
				event |= POLLOUT; // If out has queued data, so when there is smth to write, it will exist for that moment
			pfds.push_back(pollfd{ _clients[i].fd, event, 0}); //pushes to pfds so poll can handle client appropriatelty
		}

		DBG("pfds=" << pfds.size()
			<< " clients=" << _clients.size());
		
		int pRes = poll(pfds.data(), pfds.size(), 10000);
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

		//Accepting new clientss, building Conn for them and adding to the list to check in next run
		if (pfds[0].revents & POLLIN)
		{
			sockaddr_in c_addr;
			socklen_t c_len = sizeof(c_addr);
			std::memset(&c_addr, 0, sizeof(c_addr));
			int sock_fd = accept(_listenFd, reinterpret_cast<sockaddr *>(&c_addr), &c_len);
			if (sock_fd >= 0)
			{
				if (!set_nonblock(sock_fd))
					close(sock_fd);
				else
				{
					Conn c;
					c.fd = sock_fd;
					_clients.push_back(c);
					std::cout << "Accepted new client: " << sock_fd << std::endl;
				}
			}
			 for (size_t i = 1; i < pfds.size(); ++i) {
			 DBG("fd=" << pfds[i].fd << " revents=" << pollMaskStr(pfds[i].revents));}
		}

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
			if (_retEvent & POLLIN) //Reabale
			{
				serviceClientRead(index);
			}
			if (_retEvent & POLLOUT) //Writable
			{
				serviceClientWrite(index);
			}
		}
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
	}
}


void Server::_startServerListener()
{
	// socket() creates an endpoint for communication and returns a file
	//    descriptor that refers to that endpoint.        int socket(int domain, int type, int protocol);

	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
		throw std::runtime_error("socket() failed");
	//setsockopt ?
	#if DEBUG
	std::cout << "Socket id:"<< socket_fd << std::endl;
	#endif
	
	// struct sockaddr_in {
	// 	sa_family_t     sin_family;     /* AF_INET */
	// 	in_port_t       sin_port;       /* Port number */
	// 	struct in_addr  sin_addr;       /* IPv4 address */
	// };

	// struct in_addr {
	// 	in_addr_t s_addr;
	// };

	sockaddr_in addr;	// Holds the IPv4 address/port configuration for this socket.
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;	// Use the IPv4 protocol family.
	addr.sin_port = htons(6667);	// Listen on port 6667 (default IRC port), convert to network byte order.
	addr.sin_addr.s_addr = INADDR_ANY;	// Accept connections on any local network interface.

	//        bind - bind a name to a socket
	//  When a socket is created with socket(2), it exists in a name space
	//        (address family) but has no address assigned to it.  bind()
	//        assigns the address specified by addr to the socket referred to by
	//        the file descriptor sockfd.  addrlen specifies the size, in bytes,
	//        of the address structure pointed to by addr.  Traditionally, this
	//        operation is called “assigning a name to a socket”.
	//        int bind(int sockfd, const struct sockaddr *addr,
	//                 socklen_t addrlen);

	if (bind(socket_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
	{
		close(socket_fd);
		throw std::runtime_error("bind() failed");
	}

	//    listen - listen for connections on a socket
	//    int listen(int sockfd, int backlog);
	//    listen() marks the socket referred to by sockfd as a passive
	//    socket, that is, as a socket that will be used to accept incoming
	//    connection requests using accept(2).

	if (listen(socket_fd, 42) < 0)
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
	}
	catch (std::exception &e)
	{
		std::cout << "Server error: " << e.what() << std::endl;
		std::exit(1);
	}
}

