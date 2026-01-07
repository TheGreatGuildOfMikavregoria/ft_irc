CXX = c++
NAME = ircserv
BOT = bot
vpath %.cpp cmds botsrcs

#SRCS = main.cpp Server.cpp Buffer.cpp Command.cpp Utils.cpp Client.cpp \
		Msg.cpp ServerRunCmd.cpp Channel.cpp Pass.cpp Join.cpp Pong.cpp

SRCS = main.cpp Server.cpp Buffer.cpp Command.cpp Utils.cpp Client.cpp ServerRunCmd.cpp Channel.cpp \
	Pass.cpp \
	Join.cpp \
	Part.cpp \
	Topic.cpp \
	Invite.cpp \
	Mode.cpp \
	Kick.cpp \
	Who.cpp \
	Pong.cpp \
	Msg.cpp \
	Names.cpp \

OBJS = $(SRCS:.cpp=.o)

SRCS_B = bot.cpp Buffer.cpp
OBJS_B = $(SRCS_B:.cpp=.o)

INCLUDES =
LDFLAGS =
CXXFLAGS = -g $(INCLUDES) -std=c++17

DEBUG ?= 0
ifeq ($(DEBUG),1)
	CXXFLAGS += -O0 -DDEBUG=1 

else
	CXXFLAGS += -O2 -DNDEBUG -Wall -Wextra -Werror
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
