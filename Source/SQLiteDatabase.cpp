#include "SQLiteDatabase.h"

#include <iostream>
#include <Windows.h>
#include <string>

using namespace sqdb;

Statement SQLiteDatabase::Query(const std::string& filename, const std::string& query) {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	//std::cout << "Current Directory Is: " << std::string(buffer).substr(0, pos) << std::endl;

	std::string path = std::string(buffer).substr(0, pos);
	path += "\\";
	path += filename;

	Db db(path.c_str());

	Statement s = db.Query(query.c_str());
	return s;
}
