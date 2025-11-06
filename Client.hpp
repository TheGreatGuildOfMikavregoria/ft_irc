#pragma once

#include "Server.hpp"
#include "Buffer.hpp"

class Client{
	private:
        int fd;
	    Buffer in;
	    Buffer out;
        std::string nick;
        std::string user;
        
};