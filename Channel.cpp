
#include "Utils.hpp"
#include "Channel.hpp"

/*
	TODO:	list of clients in channel or list of channels in user
*/

// TODO: modify to work with pointers
// TODO: change
void Channel::userAdd(Client *user)
{
	_channelUsers.insert(user);
	user->getUserChannels().insert(this);
}

// TODO change
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
	std::time_t topicChange;
	std::string rpl;
	Buffer &outBuf = client.getOutBuf();
	std::string nickname = client.getNickName();
	//TODO: think through repeated join
	if (!keyValidated && getKeyMode())
	{
		rpl = numericRPL(ERR_BADCHANNELKEY, nickname, _name);
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	if (getClientLimitMode() && _channelUsers.size() == _clientLimit)
	{
		rpl = numericRPL(ERR_CHANNELISFULL, nickname, _name);
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	if (getInviteOnlyMode() && !_channelUsers.count(&client))
	{
		rpl = numericRPL(ERR_INVITEONLYCHAN, nickname, _name);
		outBuf.append(rpl.c_str(), rpl.length());
		return ;
	}
	userAdd(&client);
	_inviteListRemove(nickname);
	if (_operators.size() == 0)
	{
		_operators.insert(nickname);
		_topicUpdatedWho = nickname;
	}
	std::string prefix = ":" + nickname;
	std::string message = prefix + " JOIN " + _name + "\r\n";
	this->chanBroadcast(message);
	topicChange = std::time(nullptr);
	if (topicChange != static_cast<time_t>(-1))
		_topicUpdatedTime = topicChange;
	topic(client, false);
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

void Channel::part(Client &client, std::string &reason)
{
	std::string response;
	if (reason.length())
		response = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName()  + " PART " + _name + " :" + reason +  "\r\n";
	else
		response = ":" + client.getNickName() + "!" + client.getUserName()  + "@" + client.getHostName() + " PART " + _name + "\r\n";
	chanBroadcast(response);
	userRemove(client);
}

void Channel::topic(Client &c, bool broadcast)
{
	std::string rpl;
	Buffer &outBuff = c.getOutBuf();
	if (_topic.size())
		rpl = numericRPL(RPL_TOPIC, c.getNickName(), _name, _topic);
	else
		rpl = numericRPL(RPL_NOTOPIC, c.getNickName(), _name);
	rpl += numericRPL(RPL_TOPICWHOTIME, c.getNickName(), _name, _topicUpdatedWho, Utils::longToString(_topicUpdatedTime));
	if (broadcast)
		chanBroadcast(rpl);
	else
		outBuff.append(rpl.c_str(), rpl.length());
	
// RPL_TOPICWHOTIME
}

void Channel::topic(Client &c, std::string &newTopic)
{
	std::string rpl;
	Buffer &outBuff = c.getOutBuf();
	std::time_t topicChange;
	if (getProtectedTopicMode() && !_operators.count(c.getNickName()))
	{
		rpl = numericRPL(ERR_CHANOPRIVSNEEDED, c.getNickName(), _name);
		outBuff.append(rpl.c_str(), rpl.length());
		return;
	}
//		it->topic(c, );
	topicChange = std::time(nullptr);
	if (topicChange != static_cast<time_t>(-1))
		_topicUpdatedTime = topicChange;
	_topicUpdatedWho = c.getNickName();
	_topic = newTopic;
	topic(c, true);
// try set new topic, reply??
}

int Channel::kick(Client &source, std::string &nick)
{
	(void)source;
	(void)nick;
	return 0;
}

bool Channel::invite(Client &source, std::string &nick)
{
	std::string rpl;
	Buffer &outBuf = source.getOutBuf();
	if (!_channelUsers.count(&source))
	{
		rpl = numericRPL(ERR_NOTONCHANNEL, source.getNickName(), _name);
		outBuf.append(rpl.c_str(), rpl.length());
		return false;
	}
//TODO : new mode considerations
	if (getInviteOnlyMode() && !_operators.count(source.getNickName()))
	{
		rpl = numericRPL(ERR_CHANOPRIVSNEEDED, source.getNickName(), _name);
		outBuf.append(rpl.c_str(), rpl.length());
		return false;
	}
	auto userIt = std::find_if(_channelUsers.begin(), _channelUsers.end(), [&nick](Client *client){ return client->getNickName() == nick; });
	if (userIt != _channelUsers.end())
	{
		rpl = numericRPL(ERR_USERONCHANNEL, source.getNickName(), nick, _name);
		outBuf.append(rpl.c_str(), rpl.length());
		return false;
	}
//TODO: somehow not shown for the inviter
	rpl = numericRPL(RPL_INVITING, source.getNickName(), nick, _name);
	outBuf.append(rpl.c_str(), rpl.length());
	_inviteListAdd(nick);
	return (true);
}

void Channel::names(Client &source)
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
		if (userPtr != &client)
			userPtr->getOutBuf().append(message.c_str(), message.length());
	}
}
/*
bool Channel::validateModes(std::string &mode)
{
	
}
*/
