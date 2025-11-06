# DISCLAIMER: This is a work in progress, set public to test Github Organization features.

# 💬 ft_irc

A fully functional Internet Relay Chat (IRC) server written in **C++**, compliant with the **RFC 1459** standard.  
This project was completed as part of the **42 curriculum**, focusing on socket programming, networking, and asynchronous I/O handling.

---

## 🎯 Goal

Implement an IRC server capable of handling multiple clients simultaneously, supporting the main IRC commands, and managing user and channel states.

---

## 🧩 Features

- TCP server handling multiple clients  
- Non-blocking sockets with `poll()`  
- Command parsing and response system  
- User and channel management  
- Password authentication (`PASS` command)  
- Support for essential IRC commands: NICK, USER, JOIN, PART, PRIVMSG, NOTICE, KICK, MODE, TOPIC, QUIT
- Graceful client disconnection and cleanup  
- Configurable port and password at runtime  

---

## ⚙️ Build & Run

### Compile and run
```bash
make
./ircserv <port> <password>

Example:
./ircserv 6667 mysecret

or with netcat for manual testing:
nc localhost 6667

🧱 Architecture Overview
src/
 ├── main.cpp        # entry point, server setup
 ├── Server.cpp/.hpp # core loop, client handling
 ├── Client.cpp/.hpp # client state, nickname, user info
 ├── Command.cpp/.hpp# command parsing & execution
 ├── Channel.cpp/.hpp# channel logic (join, topic, users)
 └── utils/          # helper functions, string handling, etc.

The server runs a main event loop using poll() to handle input/output across all sockets.

Each command is parsed and dispatched to the appropriate handler class.

Channels maintain their own user lists and topic data.

🧵 Collaboration Workflow
To maintain clean development and easy review:

Create a new branch for each feature:

git checkout -b feature/command-parsing
Commit frequently with meaningful messages:

git commit -m "Implement parsing for JOIN and PART"
Push your branch and open a Pull Request (PR) into main.

Review, test, and merge only after approval.

Branch naming convention:

feature/<name>     # new features
fix/<name>         # bug fixes
refactor/<name>    # improvements

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
