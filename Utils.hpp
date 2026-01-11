#pragma once
#include <string>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <list>
#include "Client.hpp"
#include "Channel.hpp"
class Client;
class Channel;

class Utils {
	public:
		static std::string stringToUppercase(std::string text);
		static std::vector<std::string> ft_split(std::string param, char sep);
		static auto getChannelIteratorByChannelName(std::list<Channel> &clientVector, const std::string &name) -> std::list<Channel>::iterator;
		static std::string getCurrentTimeString();
		static std::string longToString(long someLong);
};
