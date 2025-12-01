#include "Channel.hpp"

/*
	TODO:	list of clients in channel or list of channels in user
*/

void _userAdd(Client &user)
{
	_channelUsers.push_back(user);
}

void _userRemove(Client &user)
{

	int index = 0;
	auto it = _getUserIteratorByNickName(_channelUsers, user.getNickName());
	if (it != _channelUsers.end())
		_channelUsers.erase(it);
}


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
