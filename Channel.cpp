#include "Channel.hpp"


Channel::Channel(const std::string &name, const std::string &modes)
{
	_name = name;
	
	

}

bool Channel::validateName(std::string &name)
{
	if (!name.length())
		return (false);
	if (name[0] != '#' && name[0] != '&')
		return (false);
	return (true);
}

bool Channel::validateModes(std::string &mode)
{

}
