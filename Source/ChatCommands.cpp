#include "ChatCommands.h"
#include "Social.h"
#include "WorldConnection.h"
#include "WorldServer.h"
#include "UtfConverter.h"
#include "Logger.h"
#include "World.h"
#include "GameMessages.h"
#include "ReplicaData.h"
#include "Worlds.h"
#include "Account.h"
#include "Packet.h"
#include "InventoryDB.h"
#include "PlayerObject.h"
#include "EnvironmentalObject.h"
#include "NPCObject.h"
#include "EnemyObject.h"
#include "SmashableObject.h"
#include "DroppedLootObject.h";
#include "serverLoop.h"
#include "ObjectID.h"
#include "CharactersDB.h"

// SQLite
#include "SQLiteDatabase.h"
#include "SQLite\sqdb.h"

#include <ctype.h>

std::vector<ChatCommandHandler *> ChatCommandManager::ChatCommandHandlers;
std::unordered_map<std::wstring, ChatCommandHandler *> ChatCommandManager::ChatCommands;

void ChatCommandManager::registerCommands(ChatCommandHandler * CommandHandler){
	ChatCommandManager::ChatCommandHandlers.push_back(CommandHandler);
	std::vector<std::wstring> names = CommandHandler->getCommandNames();
	for (unsigned char k = 0; k < names.size(); k++){
		ChatCommandManager::ChatCommands.insert(std::make_pair(names.at(k), CommandHandler));
	}
}

void ChatCommandManager::handleCommand(std::wstring message, SessionInfo *s){
	std::wstring command;
	std::vector<std::wstring> params;
	unsigned char state = 0;

	for (unsigned int k = 0; k < message.length(); k++){
		wchar_t chr = message.at(k);
		if (state == 0){
			//Nothing is parsed yet
			if (chr == L'/'){ //Needs to start with '/'
				state = 1; //Set state to STATE_COMMAND
			}
			else{
				break;
			}
		}
		else if (state == 1){
			//We are parsing the command name
			if (chr == L' '){
				state = 2;
			}
			else{
				command.push_back(chr);
			}
		}
		else if (state == 2){
			//expecting commands
			if (chr == L' '){
				//We could have two modes: add an empty param or ignore more spaces
				//Aditionally we can test if the last one was already an empty one
				//To be safe for now, just ignore it
			}
			else if (chr == L'"'){ //Start of a string command
				params.push_back(L"");
				state = 4;
			}
			else{
				//Start of a normal command
				params.push_back(std::wstring(1, chr));
				state = 3;
			}
		}
		else if (state == 3){
			//Normal param
			if (chr == L' '){
				state = 2;
			}
			else{
				params.at(params.size() - 1).append(1, chr);
			}
		}
		else if (state == 4){
			//String param
			if (chr == L'"'){
				state = 2;
				//what do we do with that? It should be a parsing error when not followed by a space
			}
			else if (chr == L'\\'){
				state = 5;
			}
			else{
				params.at(params.size() - 1).append(1, chr);
			}
		}
		else if (state == 5){
			//Escape sequences
			if (chr == L'n'){
				params.at(params.size() - 1).append(L"\n");
			}
			else if (chr == L't'){
				params.at(params.size() - 1).append(L"\t");
			}
			else{
				params.at(params.size() - 1).append(1, chr);
			}
		}
	}


	if (command == L"help"){
		bool flag = true;
		if (params.size() > 0){
			std::wstring com = params.at(0);
			if (com.at(0) == L'/'){
				com = com.substr(1, com.size() - 1);
			}
			std::unordered_map<std::wstring, ChatCommandHandler *>::iterator it = ChatCommandManager::ChatCommands.find(com);
			if (it != ChatCommandManager::ChatCommands.end()){
				Chat::sendChatMessage(s->addr, L"Syntax: /" + com + L" " + it->second->getSyntax());
				Chat::sendChatMessage(s->addr, it->second->getDescription());
			}
			else{
				Chat::sendChatMessage(s->addr, L"Command " + params.at(0) + L" was not found");
			}
			flag = false;
		}
		if (flag){
			for (unsigned int k = 0; k < ChatCommandManager::ChatCommandHandlers.size(); k++){
				std::wstring desc = ChatCommandManager::ChatCommandHandlers.at(k)->getShortDescription();
				std::wstring cmd = ChatCommandManager::ChatCommandHandlers.at(k)->getCommandNames().at(0);
				unsigned long stringvalue = cmd.size() * 4;
				short diff = 0;
				std::wstring space = L"\t\t";
				for (unsigned int l = 0; l < cmd.size(); l++){
					wchar_t c = cmd.at(l);
					if (c == L'm') diff = diff + 2;
					else if (c == L'w') diff = diff + 2;
					else if (c == L'r') diff = diff - 1;
					else if (c == L'f') diff = diff - 2;
					else if (c == L't') diff = diff - 2;
					else if (c == L'i') diff = diff - 3;
					else if (c == L'j') diff = diff - 3;
					else if (c == L'l') diff = diff - 3;
				}

				if ((stringvalue + diff) > 30){
					space = L"\t";
				}
				
				if (desc != L""){
					Chat::sendChatMessage(s->addr, L"/" + cmd + space + desc);
				}
			}
		}
	}
	else{
		std::unordered_map<std::wstring, ChatCommandHandler *>::iterator it = ChatCommandManager::ChatCommands.find(command);
		if (it != ChatCommandManager::ChatCommands.end()){
			it->second->handleCommand(s, &params);
		}
		else{
			Chat::sendChatMessage(s->addr, L"Command " + command + L" does not exist!");
		}
	}
}

void FlightCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	bool f2 = true;
	if (params->size() > 0){
		if (params->at(0) == L"off" || params->at(0) == L"false"){
			WorldServerPackets::SendGameMessage(s->addr, s->activeCharId, 561);
			f2 = false;
		}
	}
	if (f2){
		RakNet::BitStream *pc = WorldServerPackets::InitGameMessage(s->activeCharId, 561);
		pc->Write((unsigned long)0x70ba);
		pc->Write((unsigned short)0x8);
		pc->Write((unsigned char)0x5);
		pc->Write((unsigned char)0x2);
		pc->Write((unsigned short)0xc);
		pc->Write((unsigned char)0x3);
		pc->Write((unsigned short)0x6c1);
		pc->Write((unsigned char)0x0);
		pc->Write((unsigned char)0x1);
		pc->Write((unsigned char)0x80);
		pc->Write((unsigned char)0x7f);
		pc->Write((unsigned long)0xa7);
		WorldServer::sendPacket(pc, s->addr);
	}
}

std::vector<std::wstring> FlightCommandHandler::getCommandNames(){
	return { L"flight", L"jetpack", L"fly" };
}

std::wstring FlightCommandHandler::getDescription(){
	return L"Enables the player to fly around the world as if they were to wear a jetpack";
}
std::wstring FlightCommandHandler::getShortDescription(){
	return L"Enables flight";
}
std::wstring FlightCommandHandler::getSyntax(){
	return L"[off]";
}

void TeleportCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	RakNet::BitStream *bs = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_GAME_MSG);
	bs->Write(s->activeCharId);
	bs->Write((unsigned short)19);
	bs->Write(false);
	bs->Write(false);
	bs->Write(false);
	float x = 0.0F;
	float y = 0.0F;
	float z = 0.0F;
	PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s->activeCharId);
	if (params->size() > 2) {
		try
		{
			if (params->at(0).substr(0) == L"~") { x = player->getComponent1()->getPosition().x; }
			else { x = std::stof(params->at(0)); }

			if (params->at(1).substr(0) == L"~") { y = player->getComponent1()->getPosition().y; }
			else { y = std::stof(params->at(1)); }

			if (params->at(2).substr(0) == L"~") { z = player->getComponent1()->getPosition().z; }
			else { z = std::stof(params->at(2)); }
		}
		catch (const std::exception&)
		{
			Chat::sendChatMessage(player->objid, L"Unable to teleport!");
		}
	}
	else {
		x = player->getComponent1()->getPosition().x;
		y = player->getComponent1()->getPosition().y;
		z = player->getComponent1()->getPosition().z;
	}
	bs->Write(x);
	bs->Write(y);
	bs->Write(z);
	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s->zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++) {
		WorldServer::sendPacket(bs, sessionsz.at(k).addr);
	}
}

std::vector<std::wstring> TeleportCommandHandler::getCommandNames(){
	return{ L"teleport", L"tele", L"ot"};
}

std::wstring TeleportCommandHandler::getDescription(){
	return L"Teleports a player to the specified location or map origin";
}
std::wstring TeleportCommandHandler::getShortDescription(){
	return L"Teleport player";
}
std::wstring TeleportCommandHandler::getSyntax(){
	return L"[<x> <y> <z>]";
}

void WhisperCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	if (params->size() > 1){
		std::wstring reciever = params->at(0);
		std::wstring message = params->at(1);
		for (unsigned int k = 2; k < params->size(); k++){
			message.append(L" ");
			message.append(params->at(k));
		}
		Logger::log("WRLD", "COMMAND", "Private message to " + UtfConverter::ToUtf8(reciever));
		Logger::log("WRLD", "COMMAND", UtfConverter::ToUtf8(message));
	}
}

std::vector<std::wstring> WhisperCommandHandler::getCommandNames(){
	return{ L"whisper", L"w", L"tell" };
}

std::wstring WhisperCommandHandler::getDescription(){
	return L"Sends another player a private chat message";
}
std::wstring WhisperCommandHandler::getShortDescription(){
	return L"Private Chat";
}
std::wstring WhisperCommandHandler::getSyntax(){
	return L"<reciever> <message>";
}

void TestmapCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	if (params->size() > 0){
		std::string checkZoneID = UtfConverter::ToUtf8(params->at(0));
		bool flag = true;
		for (unsigned int i = 0; i < checkZoneID.length(); i++){
			if (!isdigit(checkZoneID.at(i)))
				flag = false;
		}
		if (flag){
			unsigned short argumentValue = std::stoi(params->at(0));
			ZoneId zone = static_cast<ZoneId>(argumentValue);
			Logger::log("WRLD", "CHAT", "Requesting teleport to " + std::to_string(argumentValue));
			bool f = false;
			if (getWorldTarget(zone).size() > 0){
				if (zone != ZoneId::NO_ZONE && zone != ZoneId::KEELHAUL_CANYON){
					COMPONENT1_POSITION pos = getZoneSpawnPoint(zone, static_cast<ZoneId>(s->zone));
					f = Worlds::loadWorld(s->addr, zone, pos, 0, 0);
					if (f){
						
						Session::leave(s->activeCharId);
						WorldPlace place;
						place.zoneID = zone;
						place.mapClone = 0;
						place.mapInstance = 0;
						place.x = pos.x;
						place.y = pos.y;
						place.z = pos.z;
						if (CharactersTable::setCharactersPlace(s->activeCharId, place))
						{
							
							
						}
						ObjectsManager::clientLeaveWorld(s->activeCharId, s->addr);

					}
					else{
						Chat::sendChatMessage(s->addr, L"Cannot teleport to this zone");
					}
				}
				else{
					Chat::sendChatMessage(s->addr, L"Cannot teleport to this zone");
				}
			}
			if (!f){
				Chat::sendChatMessage(s->addr, L"Cannot teleport to WorldID " + params->at(0));
			}
		}
		else{
			Chat::sendChatMessage(s->addr, L"Cannot teleport to WorldID " + params->at(0));
		}
	}
}

std::vector<std::wstring> TestmapCommandHandler::getCommandNames(){
	return{ L"testmap", L"tp" };
}

std::wstring TestmapCommandHandler::getDescription(){
	return L"Sends the player to another world";
}
std::wstring TestmapCommandHandler::getShortDescription(){
	return L"World Travel";
}
std::wstring TestmapCommandHandler::getSyntax(){
	return L"<ZoneId>";
}

void SwitchCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	if (params->size() > 1){
		std::string ip = UtfConverter::ToUtf8(params->at(0));
		short port = (short)std::stoi(params->at(1));

		SystemAddress target;
		target.SetBinaryAddress(ip.data());
		target.port = port;

		bool flag = Session::sendToInstance(s->addr, target);

		if (flag){
			RakNet::BitStream * redirect = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_REDIRECT);
			PacketTools::WriteToPacket(redirect, ip, 33);
			redirect->Write(port);
			redirect->Write((unsigned char)1);
			WorldServer::sendPacket(redirect, s->addr);
		}
		else{
			Chat::sendChatMessage(s->addr, L"Could not switch instance");
		}
	}
}

std::vector<std::wstring> SwitchCommandHandler::getCommandNames(){
	return{ L"switch", L"tpx" };
}

std::wstring SwitchCommandHandler::getDescription(){
	return L"Sends the player to another server";
}
std::wstring SwitchCommandHandler::getShortDescription(){
	return L"Change Servers";
}
std::wstring SwitchCommandHandler::getSyntax(){
	return L"<IP> <port>";
}

void ItemsCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	if (params->size() > 1){
		if (params->at(0) == L"create"){
			long lot = std::stol(params->at(1));
			long long objid = ObjectsTable::createObject(lot);
			Chat::sendChatMessage(s->addr, L"Object created with id: " + std::to_wstring(objid - 1152921510794154770));
		}

		if (params->at(0) == L"equip"){
			long long objid = 1152921510794154770 + std::stoll(params->at(1));
			PlayerObject *player = (PlayerObject *) ObjectsManager::getObjectByID(s->activeCharId);
			if (player != NULL){
				if (params->size() > 2){
					unsigned short slot = std::stoi(params->at(2));
					player->getComponent17()->equipItem(objid, slot);
				}
				else{
					player->getComponent17()->equipItem(objid);
				}
				ObjectsManager::serialize(player);  //player->serialize();
			}
		}
		if (params->at(0) == L"unequip"){
			long long objid = 1152921510794154770 + std::stoll(params->at(1));
			PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s->activeCharId);
			if (player != NULL){
				bool un = player->getComponent17()->unequipItem(objid);
				if (!un){
					Chat::sendChatMessage(s->addr, L"Object not found");
				}
				else{
					ObjectsManager::serialize(player); // player->serialize();
				}
			}
		}
	}
}

std::vector<std::wstring> ItemsCommandHandler::getCommandNames(){
	return{ L"inventory", L"item"};
}

std::wstring ItemsCommandHandler::getDescription(){
	return L"Manages Items for a player";
}

std::wstring ItemsCommandHandler::getShortDescription(){
	return L"Manage Items";
}

std::wstring ItemsCommandHandler::getSyntax(){
	return L"(create <LOT>|equip <num>|unequip <num>)";
}

void AddItemCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	if (params->size() == 1)
	{
		std::string checkLOT = UtfConverter::ToUtf8(params->at(0));
		std::string checkAmount = "1";
		bool flag = true;
		for (unsigned int i = 0; i < checkLOT.length(); i++){
			if (!isdigit(checkLOT.at(i)))
				flag = false;
		}
		for (unsigned int i = 0; i < checkAmount.length(); i++){
			if (!isdigit(checkAmount.at(i)))
				flag = false;
		}
		if (flag){
			long lot = std::stoi(params->at(0));
			unsigned long amount = std::stoi(params->at(1));

			unsigned long slot = -1;
			for (int i = 0; (slot == -1) && (i != 24); i++){
				if (InventoryTable::getItemFromSlot(s->activeCharId, i) == -1)
					slot = i;
			}

			if (slot == -1){
				Chat::sendChatMessage(s->addr, L"Can't add requested item to your inventory. There aren't any free slots!");
			}
			else{
				long long objid = ObjectsTable::createObject(lot);
				InventoryTable::insertItem(s->activeCharId, objid, amount, slot, false);
				//send live packet here

				RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(s->activeCharId, 227);
				ef->Write((bool)false);
				ef->Write((bool)false);
				ef->Write((bool)false);
				ef->Write((bool)false);
				ef->Write((unsigned long)0);
				ef->Write(stol(checkLOT));
				ef->Write((bool)false);
				ef->Write((bool)false);
				ef->Write((bool)true);
				ef->Write((unsigned long)stol(checkAmount));
				ef->Write((bool)true);
				ef->Write((unsigned long)0);
				ef->Write(objid);
				ef->Write((float)0);
				ef->Write((float)0);
				ef->Write((float)0);
				ef->Write((bool)true);
				ef->Write((unsigned long)slot);



				WorldServer::sendPacket(ef, s->addr);


				Chat::sendChatMessage(s->addr, L"Successfully added the requested item to your inventory!");
			}
		}
		else{
			Chat::sendChatMessage(s->addr, L"Syntax: /give " + this->getSyntax());
		}
	}
	else if (params->size() == 2){
		std::string checkLOT = UtfConverter::ToUtf8(params->at(0));
		std::string checkAmount = UtfConverter::ToUtf8(params->at(1));
		bool flag = true;
		for (unsigned int i = 0; i < checkLOT.length(); i++){
			if (!isdigit(checkLOT.at(i)))
				flag = false;
		}
		for (unsigned int i = 0; i < checkAmount.length(); i++){
			if (!isdigit(checkAmount.at(i)))
				flag = false;
		}
		if (flag){
			long lot = std::stoi(params->at(0));
			unsigned long amount = std::stoi(params->at(1));

			unsigned long slot = -1;
			for (int i = 0; (slot == -1) && (i != 24); i++){
				if (InventoryTable::getItemFromSlot(s->activeCharId, i) == -1)
					slot = i;
			}

			if (slot == -1){
				Chat::sendChatMessage(s->addr, L"Can't add requested item to your inventory. There aren't any free slots!");
			}
			else{
				long long objid = ObjectsTable::createObject(lot);
				InventoryTable::insertItem(s->activeCharId, objid, amount, slot, false);
				//send live packet here
				
				RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(s->activeCharId, 227);
				ef->Write((bool) false);
				ef->Write((bool) false);
				ef->Write((bool) false);
				ef->Write((bool) false);
				ef->Write((unsigned long)0);
				ef->Write(stol(checkLOT));
				ef->Write((bool)false);
				ef->Write((bool)false);
				ef->Write((bool)true);
				ef->Write((unsigned long)stol(checkAmount));
				ef->Write((bool)true);
				ef->Write((unsigned long)0);
				ef->Write(objid);
				ef->Write((float)0);
				ef->Write((float)0);
				ef->Write((float)0);
				ef->Write((bool) true);
				ef->Write((unsigned long)slot);
				


				WorldServer::sendPacket(ef, s->addr);


				Chat::sendChatMessage(s->addr, L"Successfully added the requested item to your inventory!");
			}
		}
		else{
			Chat::sendChatMessage(s->addr, L"Syntax: /give " + this->getSyntax());
		}
	}
	else if (params->size() == 3){
		std::string checkLOT = UtfConverter::ToUtf8(params->at(0));
		std::string checkAmount = UtfConverter::ToUtf8(params->at(1));
		bool flag = true;
		for (unsigned int i = 0; i < checkLOT.length(); i++){
			if (!isdigit(checkLOT.at(i)))
				flag = false;
		}
		for (unsigned int i = 0; i < checkAmount.length(); i++){
			if (!isdigit(checkAmount.at(i)))
				flag = false;
		}
		if (flag){
			long lot = std::stoi(params->at(0));
			unsigned long amount = std::stoi(params->at(1));
			std::string name = UtfConverter::ToUtf8(params->at(2));

			bool found = false;
			long long charid = CharactersTable::getObjidFromCharacter(name);
			if (charid > 0){
				SystemAddress addr = SessionsTable::findCharacter(charid);
				if (addr != UNASSIGNED_SYSTEM_ADDRESS){
					found = true;

					unsigned long slot = -1;
					for (int i = 0; (slot == -1) && (i != 24); i++){
						if (InventoryTable::getItemFromSlot(charid, i) == -1)
							slot = i;
					}

					if (slot == -1){
						std::stringstream ss;
						ss << "Can't add requested item to ";
						ss << name;
						ss << "'s inventory. There aren't any free slots!";

						Chat::sendChatMessage(s->addr, UtfConverter::FromUtf8(ss.str()));
					}
					else{
						long long objid = ObjectsTable::createObject(lot);
						InventoryTable::insertItem(charid, objid, amount, slot, false);
						PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(charid);
						ObjectsManager::serialize(player);
						std::stringstream ss;
						ss << "Successfully added the requested item to ";
						ss << name;
						ss << "'s inventory!";
						Chat::sendChatMessage(s->addr, UtfConverter::FromUtf8(ss.str()));

						std::string source = "UNKNOWN";

						ListCharacterInfo c = CharactersTable::getCharacterInfo(s->activeCharId);
						if (c.info.objid > 0){
							source = c.info.name;
						}

						RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(s->activeCharId, 227);
						ef->Write((bool)false);
						ef->Write((bool)false);
						ef->Write((bool)false);
						ef->Write((bool)false);
						ef->Write((unsigned long)0);
						ef->Write(stol(checkLOT));
						ef->Write((bool)false);
						ef->Write((bool)false);
						ef->Write((bool)true);
						ef->Write((unsigned long)stol(checkAmount));
						ef->Write((bool)true);
						ef->Write((unsigned long)0);
						ef->Write(objid);
						ef->Write((float)0);
						ef->Write((float)0);
						ef->Write((float)0);
						ef->Write((bool)true);
						ef->Write((unsigned long)slot);
						WorldServer::sendPacket(ef, s->addr);

						std::stringstream ss2;
						ss2 << source;
						ss2 << " added an item with LOT ";
						ss2 << std::to_string(lot);
						ss2 << " to your inventory. Please travel to another world or relog to reload your inventory.";
						Chat::sendChatMessage(addr, UtfConverter::FromUtf8(ss2.str()));
					}
				}
			}

			if (!found){
				std::stringstream ss;
				ss << "Can't add requested item to ";
				ss << name;
				ss << "'s inventory. Player not found/online!";
				Chat::sendChatMessage(s->addr, UtfConverter::FromUtf8(ss.str()));
			}
		}
		else{
			Chat::sendChatMessage(s->addr, L"Syntax: /give " + this->getSyntax());
		}
	}
	else{
		Chat::sendChatMessage(s->addr, L"Syntax: /give " + this->getSyntax());
	}
}

std::vector<std::wstring> AddItemCommandHandler::getCommandNames(){
	return{ L"give" };
}

std::wstring AddItemCommandHandler::getDescription(){
	return L"Adds an Item to a Users Inventory";
}

std::wstring AddItemCommandHandler::getShortDescription(){
	return L"Adds an Item";
}

std::wstring AddItemCommandHandler::getSyntax(){
	return L"<LOT> <amount> [<name>]";
}

void PositionCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s->activeCharId);
	std::wstringstream wstr;
	if (player != NULL){
		COMPONENT1_POSITION pos = player->getComponent1()->getPosition();
		wstr << L"Position: (" << pos.x << "|" << pos.y << "|" << pos.z << ")";
	}
	Chat::sendChatMessage(s->addr, wstr.str());
}

std::vector<std::wstring> PositionCommandHandler::getCommandNames(){
	return{ L"position", L"pos"};
}

std::wstring PositionCommandHandler::getDescription(){
	return L"Displays the position";
}

std::wstring PositionCommandHandler::getShortDescription(){
	return L"Display Position";
}

std::wstring PositionCommandHandler::getSyntax(){
	return L"";
}

void ClientCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){

}

std::vector<std::wstring> ClientCommandHandler::getCommandNames(){
	return{ L"loc"};
}

std::wstring ClientCommandHandler::getDescription(){
	return L"";
}

std::wstring ClientCommandHandler::getShortDescription(){
	return L"";
}

std::wstring ClientCommandHandler::getSyntax(){
	return L"";
}

void AttributeCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	//levels do not work yet
	std::wstringstream msg;
	if (params->size() > 1 && params->size() <= 3){
		std::wstring attr = params->at(0);
		unsigned long value = std::stoul(params->at(1));
		bool max = false;
		float maxV = 0;
		if (params->size() == 3){
			max = true;
			maxV = std::stof(params->at(2));
		}
		PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s->activeCharId);
		if (player != NULL){
			DestructibleComponent * c7 = player->getComponent7();
			CharacterComponent * c4 = player->getComponent4();
			COMPONENT7_DATA4 d4 = c7->getData4();
			if (attr == L"health"){
				d4.health = value;
				if (max) d4.maxHealth = maxV;
				msg << L"Set health to " << value;
			}
			else if (attr == L"armor"){
				d4.armor = value;
				if (max) d4.maxArmor = maxV;
				msg << L"Set armor to " << value;
			}
			else if (attr == L"imagi"){
				d4.imagination = value;
				if (max) d4.maxImagination = maxV;
				msg << L"Set imagination to " << value;
			}
			else if (attr == L"level"){
				//c4->setLevel(value);
				msg << L"This is a future feature. -Kendal";
			}
			c7->setData4(d4);
			ObjectsManager::serialize(player);
			
		}
	}
	else
	{
		msg << L"You have entered an incorrect number of parameters.";
	}
	Chat::sendChatMessage(s->addr, msg.str(), L"System");
}

std::vector<std::wstring> AttributeCommandHandler::getCommandNames(){
	return{ L"setattr" };
}

std::wstring AttributeCommandHandler::getDescription(){
	return L"Sets attributes for the player";
}

std::wstring AttributeCommandHandler::getShortDescription(){
	return L"Set Attributes";
}

std::wstring AttributeCommandHandler::getSyntax(){
	return L"(health|armor|imagi|level) <value> [<max>]";
}

void PermissionCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	std::wstringstream msg;
	ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s->activeCharId);
	msg << L"Your permission level is " << cinfo.info.gmlevel;
	Chat::sendChatMessage(s->addr, msg.str(), L"System");
}

std::vector<std::wstring> PermissionCommandHandler::getCommandNames(){
	return{ L"permission" };
}

std::wstring PermissionCommandHandler::getDescription(){
	return L"Tells you your current gmlevel";
}

std::wstring PermissionCommandHandler::getShortDescription(){
	return L"Get Gmlevel";
}

std::wstring PermissionCommandHandler::getSyntax(){
	return L"";
}

void AnnouncementCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s->activeCharId);
	if (cinfo.info.gmlevel > 0)
	{
		if (params->size() > 1){
			long long charid;
			std::wstring rec = params->at(0);
			bool flagOne = false;
			if (rec == L"#"){
				charid = s->activeCharId;
				flagOne = true;
			}
			else if (rec == L"*"){
				std::string msg = UtfConverter::ToUtf8(params->at(1));
				std::string title = "Information";
				if (params->size() > 2){
					title = UtfConverter::ToUtf8(params->at(2));
				}
				std::vector<SessionInfo> wsessions = SessionsTable::getClientsInWorld(s->zone);
				for (unsigned int i = 0; i < wsessions.size(); i++){
					Chat::sendMythranInfo(wsessions.at(i).activeCharId, msg, title);
				}
			}
			else{
				charid = CharactersTable::getObjidFromCharacter(UtfConverter::ToUtf8(rec));
				flagOne = true;
			}

			if (flagOne){
				if (charid > 0){
					std::string msg = UtfConverter::ToUtf8(params->at(1));
					std::string title = "Information";
					if (params->size() > 2){
						title = UtfConverter::ToUtf8(params->at(2));
					}
					Chat::sendMythranInfo(charid, msg, title);
				}
				else{
					std::wstring response = L"\"" + rec + L"\" is not a valid Playername";
					Chat::sendChatMessage(s->addr, response);
				}
			}
		}
		else{
			Chat::sendChatMessage(s->addr, L"Usage: /popup " + this->getSyntax());
		}
	}
	else{
		Chat::sendChatMessage(s->addr, L"You do not have permission for this command");
	}
}

std::vector<std::wstring> AnnouncementCommandHandler::getCommandNames(){
	return{ L"announce", L"popup" };
}

std::wstring AnnouncementCommandHandler::getDescription(){
	return L"Sends a localized Mythran announcement";
}

std::wstring AnnouncementCommandHandler::getShortDescription(){
	return L"Send Announcement";
}

std::wstring AnnouncementCommandHandler::getSyntax(){
	return L"(<playername>|*|#) <message> [<title>]";
}

void AdminCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params){
	ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s->activeCharId);
	if (cinfo.info.gmlevel > 0)
	{
		if (params->size() == 1){
			ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s->activeCharId);

			unsigned short maxLevel = AccountsTable::getRank(s->accountid);
			unsigned short oldLevel = cinfo.info.gmlevel;
			std::string checkLevel = UtfConverter::ToUtf8(params->at(0));
			bool flag = true;
			for (unsigned int i = 0; i < checkLevel.length(); i++){
				if (!isdigit(checkLevel.at(i)))
					flag = false;
			}
			if (flag){
				unsigned short newLevel = (unsigned char)std::stoi(params->at(0));
				unsigned char success = 0;
				if (newLevel <= maxLevel){
					success = 1;
				}
				else{
					newLevel = oldLevel;
				}

				if (success == 1) {
					CharactersTable::setGMlevel(s->activeCharId, newLevel);
					Chat::sendChatMessage(s->addr, L"Success!");
				}

				RakNet::BitStream * packet = WorldServer::initPacket(RemoteConnection::CLIENT, 16);
				packet->Write(success);
				packet->Write(maxLevel);
				packet->Write(oldLevel);
				packet->Write(newLevel);
				WorldServer::sendPacket(packet, s->addr);
			}
			else{
				Chat::sendChatMessage(s->addr, L"Syntax: " + AdminCommandHandler::getSyntax());
			}
		}
		else{
			Chat::sendChatMessage(s->addr, L"Syntax: " + AdminCommandHandler::getSyntax());
		}
	}
	else {
		Chat::sendChatMessage(s->addr, L"You do not have permission for this command");
	}
}

std::vector<std::wstring> AdminCommandHandler::getCommandNames(){
	return{ L"gmlevel" };
}

std::wstring AdminCommandHandler::getDescription(){
	return L"Set Moderator Level";
}

std::wstring AdminCommandHandler::getShortDescription(){
	return L"Set GM-Level";
}

std::wstring AdminCommandHandler::getSyntax(){
	return L"<level>";
}

void SpawnCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params) {
	ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s->activeCharId);
	if (cinfo.info.gmlevel > 0){
		if (params->size() == 1) {
			std::stringstream ss;
			ss << "SELECT `type` FROM `Objects` WHERE `id` = '" << stoul(params->at(0)) << "';";

			std::ifstream infile("cdclient.sqlite");
			if (infile.good()) {
				std::cout << "File Exists\n";
			}
			else {
				std::cout << "File Does Not Exist\n";
			}

			sqdb::Statement statement = SQLiteDatabase::Query("cdclient.sqlite", ss.str().c_str());

			while (statement.Next()) {
				std::string type(statement.GetField(0).GetText());

				PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s->activeCharId);
				ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(player->objid);
				
				if (type == "UserGeneratedNPCs" || type == "NPC")
				{

					NPCObject * npc = new NPCObject(stoul(params->at(0)), cinfo.lastPlace.zoneID);

					SimplePhysicsComponent *c3 = npc->getSimplePhysicsComponent();

					COMPONENT3_POSITION pos = COMPONENT3_POSITION(player->getComponent1()->getPosition().x, player->getComponent1()->getPosition().y, player->getComponent1()->getPosition().z);
					COMPONENT3_ROTATION rot = COMPONENT3_ROTATION(player->getComponent1()->getRotation().x, player->getComponent1()->getRotation().y, player->getComponent1()->getRotation().z, player->getComponent1()->getRotation().w);

					c3->setPosition(pos);
					c3->setRotation(rot);
					std::ostringstream st;
					st << "INSERT INTO npcs (`name`, `world`, `x`, `y`, `z`, `rotX`, `rotY`, `rotZ`, `rotW`) VALUES ('" << stoul(params->at(0)) << "', '" << cinfo.lastPlace.zoneID << "' ,'" << player->getComponent1()->getPosition().x << "' , '" << player->getComponent1()->getPosition().y << "', '" << player->getComponent1()->getPosition().z << "', '" << player->getComponent1()->getRotation().x << "', '" << player->getComponent1()->getRotation().y << "', '" << player->getComponent1()->getRotation().z << "', '" << player->getComponent1()->getRotation().w << "');";
					Logger::log("NPC", "SAVE", st.str());
					int state = mysql_query(Database::getConnection(), st.str().c_str());
					if (state != 0){
						std::wstringstream wss;
						wss << "NPC Was NOT saved. MYSQL Error. " << mysql_error(Database::getConnection());
						Chat::sendChatMessage(s->addr, wss.str());
					}
					else{
						std::wstringstream wss;
						wss << "NPC Was Saved";
						Chat::sendChatMessage(s->addr, wss.str());
					}
					ObjectsManager::registerObject(npc);
					ObjectsManager::create(npc);

					std::wstringstream wss;
					wss << "Spawned NPC with Object ID: " << npc->objid;

					Chat::sendChatMessage(s->addr, wss.str());
				}
				
				else if (type == "Enemies") //enemy itself does nothing. Enemies spawn only at 0,0
				{
					EnemyObject * enemy = new EnemyObject(stoul(params->at(0)), cinfo.lastPlace.zoneID);

					ControllablePhysicsComponent *c3 = enemy->getControllablePhysicsComponent();

					COMPONENT1_POSITION pos = COMPONENT1_POSITION(player->getComponent1()->getPosition().x, player->getComponent1()->getPosition().y, player->getComponent1()->getPosition().z);
					COMPONENT1_ROTATION rot = COMPONENT1_ROTATION(player->getComponent1()->getRotation().x, player->getComponent1()->getRotation().y, player->getComponent1()->getRotation().z, player->getComponent1()->getRotation().w);
					COMPONENT1_VELOCITY vel = COMPONENT1_VELOCITY(0, 0, 0);
					COMPONENT1_VELOCITY_ANGULAR ang = COMPONENT1_VELOCITY_ANGULAR(0, 0, 0);


					c3->setPosition(pos);
					c3->setRotation(rot);
					c3->setVelocity(vel);
					c3->setAngularVelocity(ang);

					ObjectsManager::registerObject(enemy);
					ObjectsManager::create(enemy);

					std::wstringstream wss;
					wss << "Spawned ENEMY Object ID: " << enemy->objid;

					Chat::sendChatMessage(s->addr, wss.str());
				}
				else if (type == "Environmental") //works some of the time
				{
					NPCObject * npc = new NPCObject(stoul(params->at(0)), cinfo.lastPlace.zoneID);

					SimplePhysicsComponent *c3 = npc->getSimplePhysicsComponent();

					COMPONENT3_POSITION pos = COMPONENT3_POSITION(player->getComponent1()->getPosition().x, player->getComponent1()->getPosition().y, player->getComponent1()->getPosition().z);
					COMPONENT3_ROTATION rot = COMPONENT3_ROTATION(player->getComponent1()->getRotation().x, player->getComponent1()->getRotation().y, player->getComponent1()->getRotation().z, player->getComponent1()->getRotation().w);

					c3->setPosition(pos);
					c3->setRotation(rot);

					ObjectsManager::registerObject(npc);
					ObjectsManager::create(npc);

					std::wstringstream wss;
					wss << "Spawned Environmental Object ID: " << npc->objid;
				}
				else if (type == "Smashables")
				{
					SmashableObject * smashable = new SmashableObject(stoul(params->at(0)), cinfo.lastPlace.zoneID);

					SimplePhysicsComponent *c3 = smashable->getSimplePhysicsComponent();

					COMPONENT3_POSITION pos = COMPONENT3_POSITION(player->getComponent1()->getPosition().x, player->getComponent1()->getPosition().y, player->getComponent1()->getPosition().z);
					COMPONENT3_ROTATION rot = COMPONENT3_ROTATION(player->getComponent1()->getRotation().x, player->getComponent1()->getRotation().y, player->getComponent1()->getRotation().z, player->getComponent1()->getRotation().w);

					c3->setPosition(pos);
					c3->setRotation(rot);

					ObjectsManager::registerObject(smashable);
					ObjectsManager::create(smashable);

					std::wstringstream wss;
					wss << "Spawned Smashable Object ID: " << smashable->objid;
				}
				else 
				{
					Chat::sendChatMessage(s->addr, L"Object unavailable to spawn!");
					Chat::sendChatMessage(s->addr, L"Spawn Types are: NPC, ENEMIES");
				}

				break;
			}
		}
		else {
			Chat::sendChatMessage(s->addr, L"Syntax: " + SpawnCommandHandler::getSyntax());
		}
	}
	else{
		Chat::sendChatMessage(s->addr, L"You do not have permsission for that command");
	}
	
}

std::vector<std::wstring> SpawnCommandHandler::getCommandNames() {
	return{ L"spawn" };
}

std::wstring SpawnCommandHandler::getDescription() {
	return L"Spawn an in-game Object";
}

std::wstring SpawnCommandHandler::getShortDescription() {
	return L"Spawn an Object";
}

std::wstring SpawnCommandHandler::getSyntax() {
	return L"<LOT>";
}

void RemoveCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params) {
	ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s->activeCharId);
	if (cinfo.info.gmlevel > 0){
		if (params->size() == 1) {
			unsigned long long objid = stoull(params->at(0));

			ReplicaObject *obj = ObjectsManager::getObjectByID(objid);

			if (NPCObject *npc_obj = dynamic_cast<NPCObject*>(obj)) {
				npc_obj->deleteSubObjects();
			}

			ObjectsManager::destruct(obj);
			ObjectsManager::unregisterObject(obj);
		}
		else {
			Chat::sendChatMessage(s->addr, L"Syntax: " + RemoveCommandHandler::getSyntax());
		}
	}
	else
	{
		Chat::sendChatMessage(s->addr, L"You do not have permsission for that command");
	}
	
}

std::vector<std::wstring> RemoveCommandHandler::getCommandNames() {
	return{ L"remove", L"removeobj", L"delete", L"ro" };
}

std::wstring RemoveCommandHandler::getDescription() {
	return L"Remove an in-game Object by it's Object ID";
}

std::wstring RemoveCommandHandler::getShortDescription() {
	return L"Remove an Object";
}

std::wstring RemoveCommandHandler::getSyntax() {
	return L"<Object ID>";
}

void MissionCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params) {
	ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s->activeCharId);
	if (cinfo.info.gmlevel > 0){
		std::string commandMain = UtfConverter::ToUtf8(params->at(0));
		if (params->size() > 2 && commandMain == "offer" | params->size() > 2 && commandMain == "complete"){
			RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(s->activeCharId, 248);
			ef->Write((unsigned long)stoul(params->at(1)));
			ef->Write((unsigned long long) stoull(params->at(2)));

			WorldServer::sendPacket(ef, s->addr);
		}
		else if (params->size() > 1 && commandMain == "Reward"){
			std::stringstream missionData;
			missionData << "SELECT * FROM `Missions` WHERE `id` = '1727';";

			std::ifstream infile("cdclient.sqlite");
			if (infile.good()) {
				std::cout << "File Exists\n";
			}
			else {
				std::cout << "File Does Not Exist\n";
			}
			std::stringstream whereStatement;
			whereStatement << " WHERE `id` = " << stoul(params->at(1)) << ";";
			std::stringstream rewardCurrencyS;
			rewardCurrencyS << "SELECT `reward_currency` FROM `Missions`" << whereStatement.str();
			std::stringstream rewardUscoreS;
			rewardUscoreS << "SELECT `LegoScore` FROM `Missions`" << whereStatement.str();
			std::stringstream rewardRuputation;
			rewardRuputation << "SELECT `reward_reputation` FROM `Missions`" << whereStatement.str();
			int curItem = 1;
			std::stringstream rewardItem;
			rewardItem << "SELECT `reward_item" << curItem << "` FROM `Missions`" << whereStatement.str();
			std::stringstream rewardItemC;
			rewardItemC << "SELECT `reward_item" << curItem << "_count` FROM `Missions`" << whereStatement.str();

			Logger::log("WRL", "STRING", rewardUscoreS.str(), LOG_NORMAL);
			//Prepare for Score
			sqdb::Statement statement = SQLiteDatabase::Query("cdclient.sqlite", rewardUscoreS.str());
			
			if (statement.Next() != false){
				//send Score Modify
				RakNet::BitStream * score = WorldServerPackets::InitGameMessage(s->activeCharId, 1459);
				score->Write((unsigned long long)statement.GetField(0).GetInt());
				score->Write((bool)true);
				score->Write((unsigned long)2);
				WorldServer::sendPacket(score, s->addr);
				Logger::log("WRL", "REWARDS", "Score: " + std::to_string(statement.GetField(0).GetInt()), LOG_DEBUG);

				//Record In DB
				Database::Query("UPDATE `characters` SET `uScore` = '" + std::to_string(cinfo.info.uscore + statement.GetField(0).GetInt()) + "' WHERE `objectID` = '" + std::to_string(s->activeCharId) + "';");
			}
			//Prepare For Reputation
			statement = SQLiteDatabase::Query("cdclient.sqlite", rewardRuputation.str().c_str());

			if (statement.Next() != false){
				//Send Reputation Modify
				RakNet::BitStream * reputation = WorldServerPackets::InitGameMessage(s->activeCharId, 746);
				reputation->Write((unsigned long long)cinfo.info.reputation + 195);
				WorldServer::sendPacket(reputation, s->addr);
				Logger::log("WRL", "REWARDS", "Reput: " + std::to_string(statement.GetField(0).GetInt()), LOG_DEBUG);

				//Record in DB
				Database::Query("UPDATE `characters` SET `reputation` = '" + std::to_string(cinfo.info.reputation + statement.GetField(0).GetInt()) + "' WHERE `objectID` = '" + std::to_string(s->activeCharId) + "';");
			}

			//Prepare For Currency
			statement = SQLiteDatabase::Query("cdclient.sqlite", rewardCurrencyS.str().c_str());

			if (statement.Next() != false){
				//send currency modify
				RakNet::BitStream * currency = WorldServerPackets::InitGameMessage(s->activeCharId, 133);
				currency->Write((signed long long)cinfo.info.currency + statement.GetField(0).GetInt());
				currency->Write((float)0);
				currency->Write((float)0);
				currency->Write((float)0);
				WorldServer::sendPacket(currency, s->addr);
				Logger::log("WRL", "REWARDS", "Currency: " + std::to_string(statement.GetField(0).GetInt()), LOG_DEBUG);

				//Record in DB
				Database::Query("UPDATE `characters` SET `currency` = '" + std::to_string(cinfo.info.currency + statement.GetField(0).GetInt()) + "' WHERE `objectID` = '" + std::to_string(s->activeCharId) + "';");
			}

			//Add 4 Items
			for (int i = 1; i < 4; i++){
				curItem = i;
				statement = SQLiteDatabase::Query("cdclient.sqlite", rewardItem.str().c_str());
				sqdb::Statement statement1 = SQLiteDatabase::Query("cdclient.sqlite", rewardItemC.str().c_str());

				if (statement.Next() != false && statement1.Next() != false){
					Logger::log("WRL", "REWARDS", "Id: " + std::to_string(statement.GetField(0).GetInt()) + "*" + std::to_string(statement1.GetField(0).GetInt()), LOG_DEBUG);
					if (statement.GetField(0).GetInt() != -1){
						Logger::log("WRL", "REWARDS", "Giving Player item: " + std::to_string(statement.GetField(0).GetInt()), LOG_DEBUG);
						unsigned long slot = -1;
						for (int i = 0; (slot == -1) && (i != 24); i++){
							if (InventoryTable::getItemFromSlot(s->activeCharId, i) == -1)
								slot = i;
						}

						if (slot == -1){
							Chat::sendChatMessage(s->addr, L"Can't add requested item to your inventory. There aren't any free slots!");
						}
						else{
							long long objid = ObjectsTable::createObject(statement.GetField(0).GetInt());
							InventoryTable::insertItem(s->activeCharId, objid, statement1.GetField(0).GetInt(), slot, false);
							//send live packet here
							RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(s->activeCharId, 227);
							ef->Write((bool)false);
							ef->Write((bool)false);
							ef->Write((bool)false);
							ef->Write((bool)false);
							ef->Write((unsigned long)0);
							ef->Write((unsigned long)statement.GetField(0).GetInt());
							ef->Write((bool)false);
							ef->Write((bool)false);
							ef->Write((bool)true);
							ef->Write((unsigned long)statement.GetField(0).GetInt());
							ef->Write((bool)true);
							ef->Write((unsigned long)0);
							ef->Write(objid);
							ef->Write((float)0);
							ef->Write((float)0);
							ef->Write((float)0);
							ef->Write((bool)true);
							ef->Write((unsigned long)slot);
							WorldServer::sendPacket(ef, s->addr);
						}
					}
				}
			}
		}
		else{
			Chat::sendChatMessage(s->addr, L"Syntax: " + MissionCommandHandler::getSyntax());
		}
	}
	else{
		Chat::sendChatMessage(s->addr, L"You do not have permsission for that command");
	}
}

std::vector<std::wstring> MissionCommandHandler::getCommandNames() {
	return{ L"mission" };
}

std::wstring MissionCommandHandler::getDescription() {
	return L"Offer/Complete/Give Mission Rewards";
}

std::wstring MissionCommandHandler::getShortDescription() {
	return L"Offer/COmplete/Rewards";
}

std::wstring MissionCommandHandler::getSyntax() {
	return L"<Type: Offer/Complete or Rewards> <Mission ID> <NPC ID>";
}

void DropCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params) {
	ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s->activeCharId);
	if (cinfo.info.gmlevel > 0){
		if (params->size() == 1) {

			DroppedLootObject * loot = new DroppedLootObject(stol(params->at(0)), s->zone, s);
			ObjectsManager::registerObject(loot);
			/*RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(s->activeCharId, 30);
			PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s->activeCharId);

			ef->Write((bool)true);

			ef->Write((bool)true);
			ef->Write((float)player->getComponent1()->getPosition().x);
			ef->Write((float)player->getComponent1()->getPosition().y+10);
			ef->Write((float)player->getComponent1()->getPosition().z);
			ef->Write((int)0);
			ef->Write((unsigned int)stoi(params->at(0)));
			long long id = ObjectID::generateObjectID();
			ef->Write((unsigned long long)id);
			ef->Write((unsigned long long)player->getObjectID());
			ef->Write((unsigned long long)player->getObjectID());
			ef->Write((bool)true);
			ef->Write((float)player->getComponent1()->getPosition().x);
			ef->Write((float)player->getComponent1()->getPosition().y);
			ef->Write((float)player->getComponent1()->getPosition().z);

			WorldServer::sendPacket(ef, s->addr);*/


		}
		else
		{
			Chat::sendChatMessage(s->addr, L"Syntax: " + DropCommandHandler::getSyntax());
		}
	}
	else
	{
		Chat::sendChatMessage(s->addr, L"You do not have permsission for that command");
	}
}

std::vector<std::wstring> DropCommandHandler::getCommandNames() {
	return{ L"drop" };
}

std::wstring DropCommandHandler::getDescription() {
	return L"Drops an item from the ID";
}

std::wstring DropCommandHandler::getShortDescription() {
	return L"Drop an Item";
}

std::wstring DropCommandHandler::getSyntax() {
	return L"<ID>";
}

void PlayAnimationCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params) {
	if (params->size() > 0) {
		GameMSG::playAnimation(s->activeCharId, params->at(0), true);
	}
	else {
		Chat::sendChatMessage(s->addr, L"Syntax: anim <animID>");
	}
}

std::vector<std::wstring> PlayAnimationCommandHandler::getCommandNames() {
	return{ L"animation", L"playanimation", L"anim" };
}

std::wstring PlayAnimationCommandHandler::getDescription() {
	return L"Play Animation";
}
std::wstring PlayAnimationCommandHandler::getShortDescription() {
	return L"Play Animation";
}
std::wstring PlayAnimationCommandHandler::getSyntax() {
	return L"<animID>";
}

void RotationCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params) {
	PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s->activeCharId);
	std::wstringstream wstr;
	if (player != NULL) {
		COMPONENT1_ROTATION rot = player->getComponent1()->getRotation();
		wstr << L"Rotation: (" << rot.x << "|" << rot.y << "|" << rot.z << "|" << rot.w << ")";
	}
	Chat::sendChatMessage(s->addr, wstr.str());
}

std::vector<std::wstring> RotationCommandHandler::getCommandNames() {
	return{ L"rotation", L"rot" };
}

std::wstring RotationCommandHandler::getDescription() {
	return L"Displays the rotation";
}

std::wstring RotationCommandHandler::getShortDescription() {
	return L"Display Rotation";
}

std::wstring RotationCommandHandler::getSyntax() {
	return L"";
}

void FlagCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params) {
	SystemAddress systemAddress = s->addr;
	long long objid = s->activeCharId;
	ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(objid);
	if (cinfo.info.gmlevel > 0) {
		if (params->size() > 1) {
			if (params->at(0) == L"on" || params->at(0) == L"off") {
				unsigned char flag;
				long long flagID;
				if (params->at(0) == L"on") { flag = 1; }
				if (params->at(0) == L"off") { flag = 0; }
				flagID = std::stoi(params->at(1));
				RakNet::BitStream *pc = WorldServerPackets::InitGameMessage(objid, 472);
				pc->Write((unsigned char)flag);
				pc->Write((unsigned long)flagID);
				WorldServer::sendPacket(pc, systemAddress);
			}
			else {
				Chat::sendChatMessage(objid, L"/setflag <on|off> <flagID>");
			}
		}
		else {
			Chat::sendChatMessage(objid, L"/setflag <on|off> <flagID>");
		}
	}
	else {
		Chat::sendChatMessage(objid, L"Sorry, but you have less permission than required!");
	}
}

std::vector<std::wstring> FlagCommandHandler::getCommandNames() {
	return{ L"setflag" };
}

std::wstring FlagCommandHandler::getDescription() {
	return L"";
}

std::wstring FlagCommandHandler::getShortDescription() {
	return L"";
}

std::wstring FlagCommandHandler::getSyntax() {
	return L"";
}

void SmashCommandHandler::handleCommand(SessionInfo *s, std::vector<std::wstring> * params) {
	ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s->activeCharId);
	if (cinfo.info.gmlevel > 0) {
		if (params->size() == 1) {
			long long soid = stoull(params->at(0));
			GameMSG::die(soid, true, true, 1.0F, L"SmashByPlayer", 1.0F, 1.0F, 1.0F, s->activeCharId, s->activeCharId);
			SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(soid));
			RakNet::BitStream *bs = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_GAME_MSG);

			bs->Write(s.activeCharId);
			bs->Write((unsigned short)37);

			std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
			for (unsigned int k = 0; k < sessionsz.size(); k++) {
				WorldServer::sendPacket(bs, sessionsz.at(k).addr);
			}
		}
		else
		{
			Chat::sendChatMessage(s->addr, L"Syntax: " + SmashCommandHandler::getSyntax());
		}
	}
	else
	{
		Chat::sendChatMessage(s->addr, L"You do not have permsission for that command");
	}
}

std::vector<std::wstring> SmashCommandHandler::getCommandNames() {
	return{ L"smsh", L"smash" };
}

std::wstring SmashCommandHandler::getDescription() {
	return L"Smash an Object";
}

std::wstring SmashCommandHandler::getShortDescription() {
	return L"Smash an Object";
}

std::wstring SmashCommandHandler::getSyntax() {
	return L"<oID>";
}