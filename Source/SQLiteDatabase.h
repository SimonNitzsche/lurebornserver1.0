#pragma once

#include "SQLite\sqdb.h"

using namespace sqdb;

class SQLiteDatabase {
public:
	static Statement Query(const std::string& filename, const std::string& query);
};