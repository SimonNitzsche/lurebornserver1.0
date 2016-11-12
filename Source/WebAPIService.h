#pragma once
#include <vector>
class WebAPIService
{
private:
	static void parseUpdate(std::vector<std::string> cmdV);
public:
	static void startup();
	static void class_thread();
};

