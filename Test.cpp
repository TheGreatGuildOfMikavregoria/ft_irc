#include "Test.hpp"

Test::Test() : _some(0) {}

Test::Test(int num) : _some(num)
{
	
}

int Test::getSome()
{
	return _some;
}

auto getUserIteratorByNickName(std::vector<Test> &clientVector, const int num)
{
	auto it = clientVector.begin();
	int index = 0;
	for (;it != clientVector.end(); ++it)
	{
		if (clientVector.at(index).getSome() == num)
			break;
		index++;
	}
	return (it);
}

int main()
{
	const Test test1(5);
	const Test test2(6);
	std::vector<Test> tv;
	tv.push_back(test1);
	tv.push_back(test2);
	
	auto iterator = getUserIteratorByNickName(tv, 6);
	tv.erase(iterator);
	for (Test  &test : tv)
	{
		std::cout << test.getSome() << std::endl;
	}
}
