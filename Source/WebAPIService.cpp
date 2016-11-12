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
#include "Worlds.h"

void WebAPIService::parseUpdate(std::vector<std::string> cmdV) {
	LiveUpdateTable::applyCommand(cmdV.at(0)); //id;cmd;param1;param2;param3
	std::string command = cmdV.at(1);
	std::string param1 = cmdV.at(2);
	std::string param2 = cmdV.at(3);
	std::string param3 = cmdV.at(4);

	if (command == "NameAccepted") { // Name moderation
		std::string charId = param1;
		std::string playerName = param2;
		GameMSG::setName(std::stoll(charId), std::wstring(playerName.begin(), playerName.end()));
		Chat::sendMythranInfo(std::stoll(charId), "Your name has been approved! Your Friends will now see your new name.", "Congratulations!");
		return;
	}
	else if (command == "ModerationInformation") { // Announcement from the Moderator
		std::vector<long long> AffectedClients;
		AffectedClients.clear(); // Make writable
		
		std::string receiver = param1;
		std::string title = param2;
		std::string message = param3;

		if (receiver.length() == 4) { // Everyone in World
			for each (SessionInfo ses in SessionsTable::getClientsInWorld(std::stoi(receiver)))
				AffectedClients.push_back(ses.activeCharId);
		}
		else if (CharactersTable::getAccountFromObjid(std::stoll(receiver))>0) { // Only affected client
			if(SessionsTable::findCharacter(std::stoll(receiver))!=UNASSIGNED_SYSTEM_ADDRESS)
				AffectedClients.push_back(std::stoll(receiver));
		}
		else { // Everyone on Server
			for each (SessionInfo ses in SessionsTable::getClientsInInstance(Instances::currentInstance))
				AffectedClients.push_back(ses.activeCharId);
		}

		for each (long long cid in AffectedClients)
			Chat::sendMythranInfo(cid, message, title);
	}
}

void WebAPIService::startup() {
	LiveUpdateTable::createIfNotExists();
}

void WebAPIService::class_thread() {
	//Logger::log("WAPI", "THRD", "Checking LiveUpdate-Database...");

	auto cmdL = LiveUpdateTable::getCommands();
	if (cmdL.size() > 0)
		for (int i = 0; i < cmdL.size(); i++)
			WebAPIService::parseUpdate(cmdL.at(i));
}
