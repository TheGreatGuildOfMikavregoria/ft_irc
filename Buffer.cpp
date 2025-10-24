#include "Buffer.hpp"

Buffer::Buffer() : _buffer() {}

Buffer::~Buffer() {}

void Buffer::append(char *data, size_t len)
{
	if (!data || len == 0)
		return ;
	_buffer.insert(_buffer.end(), data, data + len);
}

bool Buffer::empty()
{
	return _buffer.empty();
}

char *Buffer::data()
{
	return _buffer.empty() ? nullptr : &_buffer[0];
}

size_t Buffer::size()
{
	return _buffer.size();
}

void Buffer::discard(size_t len)
{
	if (len == 0)
		return;
	if (len >= _buffer.size())
	{
		_buffer.clear();
		return ;
	}
	_buffer.erase(_buffer.begin(), _buffer.begin() + len);
}
