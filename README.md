# 💬 ft_irc

A fully functional Internet Relay Chat (IRC) server written in **C++**.  

---

## 🧩 Features

- TCP server handling multiple clients  
- Non-blocking sockets with `poll()`  
- Command parsing and response system  
- User and channel management  
- Password authentication (`PASS` command)  
- Support for essential IRC commands
- Graceful client disconnection and cleanup  
- Helper bot that returns information about commands

---

## ⚙️ Build & Run

### Compile and run
```bash
make
./ircserv <port> <password>

Example:
./ircserv 6667 mysecret


Clients can connect e.g. via netcat:
nc localhost 6667

Or preferrably using reference client irssi.


💡 Example Commands

PASS <password>
NICK <nickname>
USER <username> 0 * :<realname>
JOIN #channel
PRIVMSG #channel :Hello everyone!
PART #channel
QUIT :Goodbye

⚡ Signals
Signals to close down server:
Ctrl + C
Ctrl + D
