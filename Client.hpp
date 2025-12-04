#pragma once

#include "Server.hpp"
#include "Buffer.hpp"
#include "Channel.hpp"

class Channel;

class Client{
	private:
		int			_fd;
		Buffer		_in;
		Buffer		_out;
		std::string	_userName;
		std::string	_nickName;
		std::string	_realName;
		std::string	_hostName;
		bool		_authStatus;
		bool		_regiStatus;
		bool		_nickNameSet;
		bool		_userNameSet;
		bool		_passwordSet;
		bool		_userModeSet;
		std::vector<Channel *> _clientChannels;
	
	public:
		Client(int fd);
		~Client();

		int	getFd() const;
		Buffer&		getInBuf();
		Buffer&		getOutBuf();
		const std::string&	getUserName() const;
		const std::string&	getNickName() const;
		const std::string&	getRealName() const;
		const std::string&	getHostName() const;
		bool	getAuthStatus() const;
		bool	getRegiStatus() const;
		bool	getNickNameStatus() const;
		bool	getUserNameStatus() const;
		bool	getPasswordStatus() const;
		bool	getUserModeStatus() const;

		void	setInBuf(Buffer in);
		void	setOutBuf(Buffer out);
		void	setUserName(std::string& userName);
		void	setNickName(std::string& nickName);
		void	setRealName(std::string& realName);
		void	setHostName(std::string& hostName);
		void	setAuthStatus(bool authStatus);
		void	setRegiStatus(bool regiStatus);
		void	setNickNameStatus(bool nickNameSet);
		void	setUserNameStatus(bool userNameSet);
		void	setPasswordStatus(bool passwordSet);
		void	setUserModeStatus(bool userModeSet);
};
