#pragma once
#include <string>
#include <algorithm>
#include <ctime>
#include "Client.hpp"
#include "Channel.hpp"

class Client;
class Channel;

class Utils {
	public:
		static std::string stringToLowercase(std::string text);

		static auto getUserIteratorByNickName(std::vector<Client> &clientVector, const std::string &nickName) -> std::vector<Client>::iterator;

		static auto getChannelIteratorByChannelName(std::vector<Channel> &clientVector, const std::string &name) -> std::vector<Channel>::iterator;
		static auto getStringIteratorByString(std::vector<std::string> &stringVector, const std::string &str) -> std::vector<std::string>::iterator;
		static std::string getCurrentTimeString();
};
