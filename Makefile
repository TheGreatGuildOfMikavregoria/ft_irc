CXX = c++
NAME = ircserv
BOT = bot
vpath %.cpp cmds botsrcs

SRCS = main.cpp Server.cpp Buffer.cpp Command.cpp Utils.cpp Client.cpp ServerRunCmd.cpp Channel.cpp \
	PassNickUser.cpp \
	Join.cpp \
	Part.cpp \
	Topic.cpp \
	Invite.cpp \
	Mode.cpp \
	Kick.cpp \
	Who.cpp \
	PingPong.cpp \
	Msg.cpp \
	Oper.cpp \
	Quit.cpp \
	Names.cpp \

OBJS = $(SRCS:.cpp=.o)

SRCS_B = bot.cpp Buffer.cpp
OBJS_B = $(SRCS_B:.cpp=.o)

INCLUDES =
LDFLAGS =
CXXFLAGS = $(INCLUDES) -std=c++17

DEBUG ?= 0
ifeq ($(DEBUG),1)
	CXXFLAGS += -g -O0 -DDEBUG=1 

else
	CXXFLAGS += -O2 -DNDEBUG -Wall -Wextra -Werror -pedantic-errors
endif

all: $(NAME) $(BOT)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

$(BOT): $(OBJS_B)
	$(CXX) $(CXXFLAGS) $(OBJS_B) -o $(BOT) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug:
	$(MAKE) fclean
	$(MAKE) DEBUG=1 all
release:
	$(MAKE) clean
	$(MAKE) DEBUG=0 all


clean:
	rm -f $(OBJS) $(OBJS_B)

fclean: clean
	rm -f $(NAME) $(BOT)

re: fclean all

.PHONY: all clean fclean re debug release
