#pragma once

#include <unordered_map>
#include <string>
#include <iostream>

class Test {

	private:
		const std::unordered_map<std::string, void (Test::*)(std::string)> _command_map = {
			{"SOME", &Test::_some},
			{"ASD", &Test::_asd},
			{"WASD", &Test::_wasd},
		};
		int _localData = -1;
		void _some(std::string param);
		void _asd(std::string param);
		void _wasd(std::string param);
	public:
		Test();
		void runCmd(std::string &);
		void testRun();
		
};
