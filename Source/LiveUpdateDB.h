#pragma once
#include "Database.h"

class LiveUpdateTable : public MySQLTable {
public:
	static void createIfNotExists();
	static std::vector<std::vector<std::string>> getCommands();
	static void instance();
	static void applyCommand(std::string dbID);
	static void startup();
};