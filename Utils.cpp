#include "Utils.hpp"

std::string stringToLowercase(std::string text)
{
	int i = 0;
	for (char &c : text)
		c = std::tolower(text[i]);
	return text;
}

auto getUserIteratorByNickName(const std::vector<Client> &clientVector, const std::string &nickName)
{
	auto it = clientVector.begin();
	int index = 0;
	for (;it != clientVector.end(); ++it)
	{
		if (clientVector.at(index).getNickName() == nickName)
			break;
		index++;
	}
	return (it);
}

auto getChannelIteratorByChannelName(const std::vector<Channel> &channelVector, const std::string &channelName)
{
	auto it = clientVector.begin();
	int index = 0;
	for (;it != clientVector.end(); ++it)
	{
		if (clientVector.at(index).getName() == channelName)
			break;
		index++;
	}
	return (it);
}
