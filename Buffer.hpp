#pragma once

// #include <cstddef>
#include <vector>
// #include <cerrno>
// #include <cstdlib>
#include <cstring>
// #include <iostream>
// #include <stdexcept>
#include <string>
#include <string_view>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <poll.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <fcntl.h>

 class Buffer
{
	private:
		std::vector<char> _buffer;

	public:
		Buffer();
		~Buffer();

		void append(const char *data, size_t len);
		void append(std::string_view s);
		bool empty();
		char *data();
		size_t size();
		void discard(size_t len);
};
