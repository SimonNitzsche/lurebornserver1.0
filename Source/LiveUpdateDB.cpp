#include "LiveUpdateDB.h"
#include "Logger.h"
#include "Database.h"
#include <sstream>
#include <stdlib.h>


void LiveUpdateTable::createIfNotExists() {
	std::stringstream qrss;
	qrss << "CREATE TABLE IF NOT EXISTS `liveupdates` ( `id` BIGINT(255) NOT NULL AUTO_INCREMENT , ";
	qrss << "`command` TEXT NOT NULL COMMENT 'commandName' , `param1` TEXT NOT NULL COMMENT ";
	qrss << "'Argument #1' , `param2` TEXT NOT NULL COMMENT 'Argument #2' , `param3` TEXT NOT ";
	qrss << "NULL COMMENT 'Argument #3' , PRIMARY KEY (`id`)) ENGINE = InnoDB ";
	qrss <<	"COMMENT = 'Interface for Live Updates';";

	auto qr = Database::Query(qrss.str());
}

std::vector<std::vector<std::string>> LiveUpdateTable::getCommands() {
	std::string qrs = "SELECT * FROM `liveupdates`;";
	auto qr = Database::Query(qrs);
	std::vector<std::vector<std::string>> commandList = { { "" } };
	commandList.clear();
	if (mysql_num_rows(qr) > 0) {
		Logger::log("LUDB", "GCMD", "Updating "+std::to_string(mysql_num_rows(qr))+" items...");
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(qr)) != NULL) {
			std::vector<std::string> cmdV = {""};
			cmdV.clear();
			for (int i = 0; i < mysql_num_fields(qr); i++)
				if (row[i] != NULL&&row[i] != "")
					cmdV.insert(std::end(cmdV), row[i]);
			commandList.insert(std::end(commandList), cmdV);
		}
	}
	return commandList;
}

void LiveUpdateTable::applyCommand(std::string dbID) {
	std::string qrs = "DELETE FROM `liveupdates` WHERE `id` = " + dbID;
	auto qr = Database::Query(qrs);
}