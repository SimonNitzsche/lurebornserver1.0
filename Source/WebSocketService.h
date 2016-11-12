#pragma once
#include "Logger.h"
#include "easywsclient.hpp"
class WebSocketService {
private:
	static std::vector<std::vector<std::string>> sendTasks;
public:
	static void WSService();
};