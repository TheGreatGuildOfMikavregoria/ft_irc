#include "Client.hpp"

Client::Client(int fd) : _fd(fd) {
	_userName = "*";
	_nickName = "*";
	_realName = "*";
	_hostName = "*";
	_authStatus = false;
	_regiStatus = false;
	_nickNameSet = false;
	_userNameSet = false;
	_passwordSet = false;
	_userModeSet = false;
	_lastActivity = std::time(nullptr);
}

Client::~Client() {}
int	Client::getFd() const {return _fd;}
Buffer&		Client::getInBuf() {return _in;}
Buffer&		Client::getOutBuf() {return _out;}
const std::string&	Client::getUserName() const {return _userName;}
const std::string&	Client::getNickName() const {return _nickName;}
const std::string&	Client::getRealName() const {return _realName;}
const std::string&	Client::getHostName() const {return _hostName;}
bool	Client::getAuthStatus() const {return _authStatus;}
bool	Client::getRegiStatus() const {return _regiStatus;}
bool	Client::getNickNameStatus() const {return _nickNameSet;}
bool	Client::getUserNameStatus() const {return _userNameSet;}
bool	Client::getPasswordStatus() const {return _passwordSet;}
bool	Client::getUserModeStatus() const {return _userModeSet;}
std::set<Channel*>& Client::getUserChannels() {return _userChannels;}

void	Client::setInBuf(Buffer in) {_in = in;}
void	Client::setOutBuf(Buffer out) {_out = out;}
void	Client::setUserName(std::string& userName) {_userName = userName;}
void	Client::setNickName(std::string& nickName) {_nickName = nickName;}
void	Client::setRealName(std::string& realName) {_realName = realName;}
void	Client::setHostName(std::string hostName) {_hostName = hostName;}
void	Client::setAuthStatus(bool authStatus) {_authStatus = authStatus;}
void	Client::setRegiStatus(bool regiStatus) {_regiStatus = regiStatus;}
void	Client::setNickNameStatus(bool nickNameSet) {_nickNameSet = nickNameSet;}
void	Client::setUserNameStatus(bool userNameSet) {_userNameSet = userNameSet;}
void	Client::setPasswordStatus(bool passwordSet) {_passwordSet = passwordSet;}
void	Client::setUserModeStatus(bool userModeSet) {_userModeSet = userModeSet;}
void	Client::setLastActivity(std::time_t lastActivity) {_lastActivity = lastActivity;}
std::time_t	Client::getLastActivity() const {return _lastActivity;}
