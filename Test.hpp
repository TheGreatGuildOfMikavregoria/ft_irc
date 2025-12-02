#ifndef TEST_HPP
# define TEST_HPP

#include <vector>
#include <iostream>

class Test {
	public:
		Test();
		Test(int num);
		Test(Test &) = default;
		Test	&operator=(Test&) = default;
		~Test() = default;
		int getSome();
	private:
		int _some;
	protected:
};

#endif
