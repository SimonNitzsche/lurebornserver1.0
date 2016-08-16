#include "WebAPIService.h"
#include <thread>
#include "Logger.h"
#include "LiveUpdateDB.h"
#include <windows.h>
#include "GameMessages.h"
#include "CharactersDB.h"
#include "AccountsDB.h"
#include "Characters.h"
#include "Social.h"

void parseUpdate(std::vector<std::string> cmdV) {
	LiveUpdateTable::applyCommand(cmdV.at(0)); //id;cmd;params...
	std::string command = cmdV.at(1);
	if (command == "NameAccepted") {
		std::string charId = cmdV.at(2);
		std::string playerName = cmdV.at(3);
		GameMSG::setName(std::stoll(charId), std::wstring(playerName.begin(), playerName.end()));
		Chat::sendMythranInfo(std::stoll(charId), "Your name has been approved! Your Friends will now see your new name.", "Congratulations!");
		return;
	}
}

void startup() {
	LiveUpdateTable::createIfNotExists();
}

void WebAPIService::class_thread() {
	//Logger::log("WAPI", "THRD", "Checking LiveUpdate-Database...");

	auto cmdL = LiveUpdateTable::getCommands();
	if (cmdL.size() > 0)
		for (int i = 0; i < cmdL.size(); i++)
			parseUpdate(cmdL.at(i));
}
