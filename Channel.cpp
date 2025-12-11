
#include "Utils.hpp"
#include "Channel.hpp"

/*
	TODO:	list of clients in channel or list of channels in user
*/

// TODO: modify to work with pointers
void Channel::userAdd(Client *user)
{
	_channelUsers.insert(user);
	user->getUserChannels().insert(this);
}

void Channel::userRemove(Client &user)
{
	std::set<Channel *>	&userChannels = user.getUserChannels();
	auto			usChanIt = userChannels.find(this);

	if (usChanIt != userChannels.end())
		userChannels.erase(usChanIt);
	_channelUsers.erase(&user);
}

void Channel::_chanOperatorAdd(Client &futureOperator)
{
	//_operators.push_back(futureOperator.getNickName());
	_operators.insert(futureOperator.getNickName());
}

void Channel::_chanOperatorRemove(Client &client)
{
	_operators.erase(client.getNickName());
/*
	auto it = Utils::getStringIteratorByString(_operators, client.getNickName());
	if (it != _operators.end())
		_operators.erase(it);
*/
}

void Channel::_inviteListAdd(std::string &toAdd)
{
	_inviteList.insert(toAdd);
//	_inviteList.push_back(toAdd);
}
void Channel::_inviteListRemove(std::string &toRemove)
{
	_inviteList.erase(toRemove);
/*
	auto it = Utils::getStringIteratorByString(_inviteList, toRemove);
	if (it != _inviteList.end())
		_inviteList.erase(it);
*/
}

Channel::Channel(const std::string &name)
{
	_name = name;
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

std::set<Client *> &Channel::getChannelUsers()
{
	return _channelUsers;
}

std::set<std::string> &Channel::getInviteList()
{
	return _inviteList;
}

int Channel::join(Client &client)
{
	std::string rpl;
	Buffer &outBuf = client.getOutBuf();
	//TODO: think through repeated join
	if (_keyMode)
		return 475;
	if (_clientLimitMode && _channelUsers.size() == _clientLimit)
		return 471;
	if (_inviteOnlyMode && !_channelUsers.count(&client))
		return 473;
	userAdd(&client);
	if (_operators.size() == 0)
		_operators.insert(client.getNickName());
	std::string prefix = ":" + client.getNickName();
	std::string message = prefix + " JOIN " + _name + "\r\n";
	this->chanBroadcast(message);
//TODO: errcode for internal error?
	if (_topic.size())
		rpl = numericRPL(RPL_TOPIC, client.getNickName(), _name, _topic);
	else
		rpl = numericRPL(RPL_NOTOPIC, client.getNickName(), _name);
	outBuf.append(rpl.c_str(), rpl.length());
	names(client);
	return 1;
}

int Channel::join(Client &client, std::string &key)
{
	if (_keyMode && key != _key)
		return 475;
	return join(client);
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

int Channel::invite(Client &source, std::string &nick)
{
	(void)source;
	(void)nick;
	return (0);
}

int Channel::names(Client &source)
{
	std::string rpl;
	Buffer &outBuf = source.getOutBuf();
	std::string namesToList;
	for (Client *client : _channelUsers)
	{
		namesToList += client->getNickName() + " ";
	}
	// TODO: check mode??
	rpl = numericRPL(RPL_NAMREPLY, source.getNickName(), "=", _name, namesToList);
	rpl += numericRPL(RPL_ENDOFNAMES, source.getNickName(), _name);
	outBuf.append(rpl.c_str(), rpl.length());
	return 1;
}
/*
const std::string &Channel::getTopic() const
{
	return _topic;
}
*/
bool Channel::validateName(std::string &name)
{
	if (!name.length())
		return (false);
	if (name[0] != '#' && name[0] != '&')
		return (false);
	return (name.length() >= 2);
}

void Channel::chanBroadcast(std::string &message)
{
	for (Client *userPtr : _channelUsers)
	{
		userPtr->getOutBuf().append(message.c_str(), message.length());
	}
}

void Channel::chanBroadcast(Client &client, std::string &message)
{
	for (Client *userPtr : _channelUsers)
	{
		if (!(userPtr == &client))
			userPtr->getOutBuf().append(message.c_str(), message.length());
	}
}
/*
bool Channel::validateModes(std::string &mode)
{
	
}
*/
