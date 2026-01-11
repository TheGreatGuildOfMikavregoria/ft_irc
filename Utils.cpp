#include "Utils.hpp"

std::string Utils::stringToUppercase(std::string text)
{
	for (char &c : text)
		c = std::toupper(c);
	return text;
}

std::vector<std::string> Utils::ft_split(std::string param, char sep)
{
	std::vector<std::string> params;
	
	size_t index =  param.find(sep);
	if (index == std::string::npos)
	{
		params.push_back(param);
		return (params);
	}
	while (index != std::string::npos)
	{
		params.push_back(param.substr(0, index));
		param = param.substr(index + 1);
		index = param.find(sep);
	}
	params.push_back(param.substr(0, index));
	return params;
}

std::string Utils::longToString(long someLong)
{
	std::ostringstream stream;
	stream << someLong;
	return stream.str();
}

auto Utils::getChannelIteratorByChannelName(std::list<Channel> &channelVector, const std::string &name) -> std::list<Channel>::iterator
{
	auto itStart = channelVector.begin();
	auto itEnd = channelVector.end();
	return std::find_if(itStart, itEnd, [&name](const Channel &channel) { return (channel.getName() == name); });
}

std::string Utils::getCurrentTimeString()
{
	time_t currentTime = time(NULL);
	if (currentTime == -1)
		return "Could not resolve Time";
	struct tm *localTime = localtime(&currentTime);
	try {
		std::string result(asctime(localTime));
		result = result.substr(0, result.size() - 1);
		return (result);
	}
	catch (...)
	{}
	return "Could not resolve Time";
}
