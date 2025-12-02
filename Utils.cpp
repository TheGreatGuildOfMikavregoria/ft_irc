#include "Utils.hpp"

std::string stringToLowercase(std::string text)
{
	int i = 0;
	for (char &c : text)
		c = std::tolower(text[i]);
	return text;
}

auto getUserIteratorByNickName(std::vector<Client> &clientVector, const std::string &nickname) -> std::vector<Client>::iterator
{
	auto itStart = clientVector.begin();
	auto itEnd = clientVector.end();
	return std::find_if(itStart, itEnd, [&nickname](const Client &client) { return (client.getNickName() == nickname); });
}

auto getChannelIteratorByChannelName(std::vector<Channel> &channelVector, const std::string &name) -> std::vector<Channel>::iterator
{
	auto itStart = channelVector.begin();
	auto itEnd = channelVector.end();
	return std::find_if(itStart, itEnd, [&name](const Channel &channel) { return (channel.getName() == name); });
}

auto getStringIteratorByString(std::vector<std::string> &stringVector, const std::string &str) -> std::vector<std::string>::iterator
{
	auto itStart = stringVector.begin();
	auto itEnd = stringVector.end();
	return std::find_if(itStart, itEnd, [&str](std::string &strVector) { return (strVector == str); });
}
