#include "Client.hpp"

Client::Client(int fd) : _fd(fd) {
	_userName = "*";
	_nickName = "*";
	_realName = "*";
	_hostName = "*";
	_userMode = ModeNone;
	_regiStatus = false;
	_nickNameSet = false;
	_userNameSet = false;
	_passwordSet = false;
	_userModeSet = false;
	_disconnectFlag = false;
	_lastActivity = std::time(nullptr);
	_waitingPong = false;
}

Client::~Client() {}

int		Client::getFd() const {return _fd;}
Buffer&	Client::getInBuf() {return _in;}
Buffer&	Client::getOutBuf() {return _out;}
const 	std::string&	Client::getUserName() const {return _userName;}
const 	std::string&	Client::getNickName() const {return _nickName;}
const 	std::string&	Client::getRealName() const {return _realName;}
const 	std::string&	Client::getHostName() const {return _hostName;}

bool	Client::getRegiStatus() const {return _regiStatus;}
bool	Client::getNickNameStatus() const {return _nickNameSet;}
bool	Client::getUserNameStatus() const {return _userNameSet;}
bool	Client::getPasswordStatus() const {return _passwordSet;}
bool	Client::getUserModeStatus() const {return _userModeSet;}
bool	Client::getDisconnectFlag() const {return _disconnectFlag;}
bool	Client::getWaitingPong() const {return _waitingPong;}
std::set<Channel*>& Client::getUserChannels() {return _userChannels;}

void	Client::setInBuf(Buffer in) {_in = in;}
void	Client::setOutBuf(Buffer out) {_out = out;}
void	Client::setUserName(std::string& userName) {_userName = userName;}
void	Client::setNickName(std::string& nickName) {_nickName = nickName;}
void	Client::setRealName(std::string& realName) {_realName = realName;}
void	Client::setHostName(std::string hostName) {_hostName = hostName;}
void	Client::setRegiStatus(bool regiStatus) {_regiStatus = regiStatus;}
void	Client::setNickNameStatus(bool nickNameSet) {_nickNameSet = nickNameSet;}
void	Client::setUserNameStatus(bool userNameSet) {_userNameSet = userNameSet;}
void	Client::setPasswordStatus(bool passwordSet) {_passwordSet = passwordSet;}
void	Client::setUserModeStatus(bool userModeSet) {_userModeSet = userModeSet;}
void	Client::setDisconnectFlag(bool disconnectFlag) {_disconnectFlag = disconnectFlag;}
void	Client::setLastActivity(std::time_t lastActivity) {_lastActivity = lastActivity;}
void	Client::setWaitingPong(bool waitingPong) {_waitingPong = waitingPong;}
std::time_t	Client::getLastActivity() const {return _lastActivity;}

void	Client::addMode(int mask) {_userMode |= mask;}
void	Client::removeMode(int mask) {_userMode &= ~mask;}
bool	Client::hasMode(int mask) const {return (_userMode & mask);}
const 	std::string	Client::getUserMode() const {
	std::string s;
	if (_userMode) s += "+";
	if (_userMode & ModeInvi) s += 'i';
	if (_userMode & ModeOper) s += 'o';
	return s;
}
std::string Client::getSource() const
{
	std::string source;
	if (getNickNameStatus())
		source += getNickName();
	if (getUserNameStatus())
		source += "!" + getUserName();
	source += "@" + getHostName();
	return (source);
}

void Client::who(Client &source)
{
	std::string rpl;
	std::string resolvedChannelName;
	auto userChannelsIt = _userChannels.begin();
	if (userChannelsIt != _userChannels.end())
		resolvedChannelName = (*userChannelsIt)->getName();
	else
		resolvedChannelName = "*";
	
	rpl = numericRPL(RPL_WHOREPLY, source.getNickName(), resolvedChannelName, source.getUserName(),
	 getHostName(), SERVER_NAME, getNickName(), "H", 0, getRealName());
	#if DEBUG
	std::cout << rpl << std::endl;
	#endif
	source.getOutBuf().append(rpl.c_str(), rpl.length());

}

void Client::who(Client &source, Channel *channelPtr)
{
	std::string rpl;
	std::string resolvedChannelName;
	if (channelPtr != nullptr && _userChannels.count(channelPtr))
		rpl = numericRPL(RPL_WHOREPLY, source.getNickName(), channelPtr->getName(), getHostName(), SERVER_NAME, getNickName(), "H", 0, getRealName());
	source.getOutBuf().append(rpl.c_str(), rpl.length());
}

void Client::channelAdd(Channel &channel)
{
	_userChannels.insert(&channel);
}

void Client::channelRemove(Channel &channel)
{
	_userChannels.erase(&channel);
}
