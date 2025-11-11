#pragma once

#include "Server.hpp"
#include "Buffer.hpp"

class Client{
	private:
		int			_fd;
		Buffer		_in;
		Buffer		_out;
		std::string	_userName;
		std::string	_nickName;
		std::string	_realName;
		std::string	_hostName;
		bool		_authSuccess;
		bool		_nickNameSet;
		bool		_userNameSet;
		bool		_passwordSet;
		bool		_userModeSet;
	
	public:
		
};