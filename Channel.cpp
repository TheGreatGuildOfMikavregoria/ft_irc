
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

void Channel::join(Client &client, bool keyValidated)
{
	std::string rpl;
	Buffer &outBuf = client.getOutBuf();
	//TODO: think through repeated join
	if (!keyValidated && _keyMode)
	{
		rpl = numericRPL(ERR_BADCHANNELKEY, client.getNickName(), _name);
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	if (_clientLimitMode && _channelUsers.size() == _clientLimit)
	{
		rpl = numericRPL(ERR_CHANNELISFULL, client.getNickName(), _name);
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	if (_inviteOnlyMode && !_channelUsers.count(&client))
	{
		rpl = numericRPL(ERR_INVITEONLYCHAN, client.getNickName(), _name);
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
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
}

void Channel::join(Client &client, std::string &key)
{
	if (key != _key)
	{
		join(client, false);
		return ;
	}
	join(client, true);
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
bool Channel::hasChanPrefix(std::string &name)
{
	if (name[0] != '#' && name[0] != '&' && name[0] != '!' && name[0] != '+' )
		return (false);
	return true;
}
bool Channel::validateName(std::string &name)
{
	if (!name.length())
		return (false);
	if (!hasChanPrefix(name))
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
		if (userPtr != &client)
			userPtr->getOutBuf().append(message.c_str(), message.length());
	}
}
/*
bool Channel::validateModes(std::string &mode)
{
	
}
*/

const std::string	Channel::getChanMode() const {

}

void	Channel::addMode(int mask) {_chanMode |= mask;}
void	Channel::removeMode(int mask) {_chanMode &= ~mask;}
bool	Channel::hasMode(int mask) const {return (_chanMode & mask);}
const std::string	Channel::getChanModeParams() const {
	std::string s = "+";//change apropriately
	if (_chanMode & ModeInviteOnly) s += 'i';
	if (_chanMode & ModeProtectedTopic) s += 't';
	if (_chanMode & ModeKeyOn) s += 'k';
	if (_chanMode & ModeOperator) s += 'o';
	if (_chanMode & ModeClientLim) s += 'l';
	return s;
}
