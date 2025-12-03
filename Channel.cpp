
#include "Utils.hpp"
#include "Channel.hpp"

/*
	TODO:	list of clients in channel or list of channels in user
*/

// TODO: modify to work with pointers
void Channel::_userAdd(Client *user)
{
	_channelUsers.push_back(user);
}

void Channel::_userRemove(Client &user)
{
	auto it = Utils::getUserIteratorByNickName(_channelUsers, user.getNickName());
	if (it != _channelUsers.end())
		_channelUsers.erase(it);
}

void Channel::_chanOperatorAdd(Client &futureOperator)
{
	_operators.push_back(futureOperator.getNickName());
}

void Channel::_chanOperatorRemove(Client &client)
{
	auto it = Utils::getStringIteratorByString(_operators, client.getNickName());
	if (it != _operators.end())
		_operators.erase(it);
}

void Channel::_inviteListAdd(std::string &toAdd)
{
	_inviteList.push_back(toAdd);
}
void Channel::_inviteListRemove(std::string &toRemove)
{
	auto it = Utils::getStringIteratorByString(_inviteList, toRemove);
	if (it != _inviteList.end())
		_inviteList.erase(it);
}



Channel::Channel(const std::string &name)
{
	_name = name;
// TODO:  protec?
	_timeCreated = Utils::getCurrentTimeString();
}

void Channel::setKey(std::string &key)
{
	_key = key;
}

void Channel::setTopic(std::string &topic)
{
	_topic = topic;
}

const std::string &Channel::getKey() const
{
	return _key;
}

const std::string &Channel::getTopic() const
{
	return _topic;
}

const std::string &Channel::getName() const
{
	return _name;
}

bool Channel::getInviteOnlyMode() const
{
	return _inviteOnlyMode;
}

bool Channel::getClientLimitMode() const
{
	return _clientLimitMode;
}

bool Channel::getKeyMode() const
{
	return _keyMode;
}

bool Channel::getProtectedTopicMode() const
{
	return _protectedTopicMode;
}

std::vector<Client> &Channel::getChannelUsers()
{
	return _channelUsers;
}

std::vector<std::string> &Channel::getInviteList()
{
	return _inviteList;
}

int Channel::join(Client &client)
{
	(void)client;
	if ()
	return 0;
}

int Channel::join(Client &client, std::string &key)
{
	(void)client;
	(void)key;
	return 0;
}

int Channel::part(Client &client)
{
	(void)client;
	return 0;
}

int Channel::kick(Client &source, std::string &nick)
{
	(void)source;
	(void)nick;
	return 0;
}

int invite(Client &source, std::string &nick)
{
	(void)source;
	(void)nick;
	return (0);
}

bool Channel::validateName(std::string &name)
{
	if (!name.length())
		return (false);
	if (name[0] != '#' && name[0] != '&')
		return (false);
	return (true);
}
/*
bool Channel::validateModes(std::string &mode)
{
	
}
*/
