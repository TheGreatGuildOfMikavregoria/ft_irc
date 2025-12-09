#include <iostream>
#include <vector>

std::vector<std::string> getSeparatedParams(std::string param);
int main()
{

	std::string test = "uiashdashdjksadh,VCXKXKVCXMVCXMV,asdsad,MCXV";
	std::vector<std::string> resVec = getSeparatedParams(test);
	for (auto str : resVec)
	{
		std::cout << str << std::endl;
	}

}

std::vector<std::string> getSeparatedParams(std::string param)
{
	std::vector<std::string> params;
	
	size_t index =  param.find(',');
	if (index == std::string::npos)
	{
		params.push_back(param);
		return (params);
	}
	while (index != std::string::npos)
	{
		params.push_back(param.substr(0, index));
		param = param.substr(index + 1);
		index = param.find(',');
	}
	return params;
}
