#pragma once

#include <vector>
#include <cstring>
#include <string>
#include <string_view>

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
