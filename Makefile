CXX = c++
NAME = ircserv

SRCS = main.cpp Server.cpp Buffer.cpp Command.cpp Utils.cpp Client.cpp ServerRunCmd.cpp cmds/Pass.cpp
OBJS = $(SRCS:.cpp=.o)

INCLUDES =
LDFLAGS =
CXXFLAGS = -g $(INCLUDES) -std=c++17

DEBUG ?= 0
ifeq ($(DEBUG),1)
	CXXFLAGS += -O0 -DDEBUG=1 

else
	CXXFLAGS += -O2 -DNDEBUG -Wall -Wextra -Werror
endif

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug:
	$(MAKE) fclean
	$(MAKE) DEBUG=1 all
release:
	$(MAKE) clean
	$(MAKE) DEBUG=0 all

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re debug release
