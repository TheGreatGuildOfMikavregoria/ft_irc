#include "Test.hpp"

Test::Test() : _some(0) {}

Test::Test(int num) : _some(num)
{
	
}
/*
Test::Test(int num) : _some(num)
{
}
*/

int Test::getSome()
{
	return _some;
}

auto getUserIteratorByNickName(std::vector<int> &clientVector, int num)
{
	auto it = clientVector.begin();
	int index = 0;
	for (;it != clientVector.end(); ++it)
	{
		if (clientVector.at(index) == num)
			break;
		index++;
	}
	return (it);
}

int main()
{
//	Test test1(5);
//	Test test2(6);
	std::vector<int> tv;
	tv.push_back(5);
	tv.push_back(6);
	
	auto iterator = getUserIteratorByNickName(tv, 6);
	tv.erase(iterator);
	for (int  &num : tv)
	{
		std::cout << num << std::endl;
	}
}
