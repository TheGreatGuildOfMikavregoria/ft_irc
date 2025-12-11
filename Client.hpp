#pragma once

#include "Server.hpp"
#include "Buffer.hpp"
#include <ctime>
#include <set>

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
		int			_userMode;
		bool		_regiStatus;
		bool		_nickNameSet;
		bool		_userNameSet;
		bool		_passwordSet;
		bool		_userModeSet;//is this needed
		std::time_t	_lastActivity;
		std::set<Channel*> _userChannels;
		
	
	public:
	    enum mode {
        ModeNone	= 0,
        ModeInvi	= 1 << 0,
        ModeOper	= 1 << 1,
        ModeWallop	= 1 << 2, //remove later
        ModeNotice	= 1 << 3  //remove later
    	};

		Client(int fd);
		~Client();

		int			getFd() const;
		Buffer&		getInBuf();
		Buffer&		getOutBuf();
		const std::string&	getUserName() const;
		const std::string&	getNickName() const;
		const std::string&	getRealName() const;
		const std::string&	getHostName() const;
		const std::string	getUserMode() const; 
		bool	getRegiStatus() const;
		bool	getNickNameStatus() const;
		bool	getUserNameStatus() const;
		bool	getPasswordStatus() const;
		bool	getUserModeStatus() const;
		std::time_t			getLastActivity() const;
		std::set<Channel*>& getUserChannels();
		void	setInBuf(Buffer in);
		void	setOutBuf(Buffer out);
		void	setUserName(std::string& userName);
		void	setNickName(std::string& nickName);
		void	setRealName(std::string& realName);
		void	setHostName(std::string hostName);

		void	setRegiStatus(bool regiStatus);
		void	setNickNameStatus(bool nickNameSet);
		void	setUserNameStatus(bool userNameSet);
		void	setPasswordStatus(bool passwordSet);
		void	setUserModeStatus(bool userModeSet);//is this needed
		void	setLastActivity(std::time_t lastActivity);

		bool	hasMode(int mode) const;
		void	addMode(int mode);
		void	removeMode(int mode);

};