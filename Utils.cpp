#include "Utils.hpp"

std::string Utils::stringToLowercase(std::string text)
{
	int i = 0;
	for (char &c : text)
		c = std::tolower(text[i]);
	return text;
}
// TODO: add one to work with pointers
std::vector<Client *>::iterator Utils::getUserIteratorByNickName(std::vector<Client *> &clientVector, const std::string &nickname)
{
	auto itStart = clientVector.begin();
	auto itEnd = clientVector.end();
	return std::find_if(itStart, itEnd, [&nickname](const Client *client) { return (client->getNickName() == nickname); });
}
/*
static Client *getClientPtrByNickName(std::vector<Client *> &clientVector, const std::string &nickName)
{
	auto itStart = clientVector.begin();
	auto itEnd = clientVector.end();
	return std::find_if(itStart, itEnd, [&nickname](const Client *client) { return (client->getNickName() == nickname); });
}
*/

auto Utils::getChannelIteratorByChannelName(std::vector<Channel> &channelVector, const std::string &name) -> std::vector<Channel>::iterator
{
	auto itStart = channelVector.begin();
	auto itEnd = channelVector.end();
	return std::find_if(itStart, itEnd, [&name](const Channel &channel) { return (channel.getName() == name); });
}

auto Utils::getStringIteratorByString(std::vector<std::string> &stringVector, const std::string &str) -> std::vector<std::string>::iterator
{
	auto itStart = stringVector.begin();
	auto itEnd = stringVector.end();
	return std::find_if(itStart, itEnd, [&str](std::string &strVector) { return (strVector == str); });
}

std::string Utils::getCurrentTimeString()
{
	time_t currentTime = time(NULL);
	if (currentTime == -1)
		return "Could not resolve Time";
	struct tm *localTime = localtime(&currentTime);
	try {
		std::string result(asctime(localTime));
		return (result);
	}
	catch (...)
	{}
	return "Could not resolve Time";
}
