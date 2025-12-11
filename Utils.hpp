#pragma once
#include <string>
#include <algorithm>
#include <ctime>
#include "Client.hpp"
#include "Channel.hpp"
#include <list>
class Client;
class Channel;

class Utils {
	public:
		static std::string stringToLowercase(std::string text);

		static std::string stringToUppercase(std::string text);
		static std::vector<Client *>::iterator getUserIteratorByNickName(std::vector<Client *> &clientVector, const std::string &nickName); 
//		static Client *getClientPtrByNickName(std::vector<Client *> &clientVector, const std::string &nickName); 

		static auto getChannelIteratorByChannelName(std::list<Channel> &clientVector, const std::string &name) -> std::list<Channel>::iterator;
		static auto getStringIteratorByString(std::vector<std::string> &stringVector, const std::string &str) -> std::vector<std::string>::iterator;
		static std::string getCurrentTimeString();
};
