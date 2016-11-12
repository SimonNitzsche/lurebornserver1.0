#pragma once
#include "serverLoop.h"

// - Common - 
#include "Color.h"
#include "GameMessages.h"
// -- Utility --
#include "UtfConverter.h"
#include "Logger.h"

// - Database - 
#include "Database.h"
#include "CharactersDB.h"
#include "InventoryDB.h"
#include "ServerDB.h"
#include "CDClientDB.h"
#include "StaticObjectsDB.h"
#include "LiveUpdateDB.h"

// - Mechanics -
#include "Account.h"
#include "Characters.h"
#include "ChatCommands.h"
#include "Social.h"
#include "Worlds.h"
#include "GameMessages.h"
#include "WebAPIService.h"
#include "WebSocketService.h"
#include "Interact.h"

// - Network -
#include "CharPackets.h"
#include "legoPackets.h"
#include "Packet.h"
#include "WorldServer.h"
#include "Macro.h"
#include "easywsclient.hpp"

// -- Connections --
#include "WorldConnection.h"

// - Replica -
#include "PlayerObject.h"
#include "NPCObject.h"
#include "SmashableObject.h"
#include "GenericObject.h"
#include "Replica.h"
#include "ReplicaComponents.h"
#include "LootObject.h"

// - Libraries -
// -- RakNet --
#include "RakNet\RakSleep.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\RakNetworkFactory.h"
#include "RakNet\PacketFileLogger.h"
#include "RakNet\BitStream.h"
#include "RakNet\ReplicaManager.h"

// -- zlib (unused) --
#include "zlib.h"

// -- c++ --
#include <conio.h>
#include <cstdlib>
#include <map>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <ctime>

void WorldLoopThread(std::vector<unsigned char> buffer, bool LUNI_WRLD, bool buffer_started, int i) {
	while (_kbhit()) {
		unsigned char key = (unsigned  char)_getch();
		switch (key) {
		case 8:
			//Backspace
			if (buffer.size() > 0) {
				buffer.pop_back();
				std::cout << "\b";
				std::cout << " ";
				std::cout << "\b";
			}
			break;
		case 13:
		{
			//Enter
			std::string str(buffer.begin(), buffer.end());
			buffer.clear();
			//std::cout << "String: " << str << std::endl;

			if (str != "") {
				std::cout << std::endl;
				if (str == "quit") {
					LUNI_WRLD = false;
				}
				else {
					std::cout << "Command '" << str << "' not found!" << std::endl;
				}
			}

			buffer_started = false;
			Logger::unmute();
		}
		break;
		case 27:
			//ESC
			break;
		default:
			if (!buffer_started) {
				std::cout << "> ";
				buffer_started = true;
				Logger::mute();
			}

			//Whitelist for chars
			if ((48 <= key && key <= 57) || (97 <= key && key <= 122) || (65 <= key && key <= 90) || key == 32) {
				buffer.push_back(key);
				std::cout << key;
			}
		}
		//ESC - 27
		//ENTER - 13
		//48 - 57 0-9
		//97 - 122 a-z
		//65 - 90 A-Z
	}

	RakSleep(30);	// This sleep keeps RakNet responsive
	Packet * packet;
	packet = WorldLoop::rakServer->Receive(); // Recieve the packets from the server
	if (packet != NULL) {








		// This will save all packets recieved from the client if running from DEBUG
#ifdef DEBUG
		stringstream packetName;
		packetName << ".//Saves//World_Packet" << i << ".bin";

		if (packet->data[3] != 22) {
			SavePacket(packetName.str(), (char*)packet->data, packet->length);
			i++;
		}
#endif

		RakNet::BitStream *pdata = new RakNet::BitStream(packet->data, packet->length, false);
		unsigned char packetId;
		pdata->Read(packetId);

		// Create and send packets back here according to the one we got
		switch (packetId) {
		case ID_USER_PACKET_ENUM:
		{
			//This is a normal packet, that should be parsed and responded to accordingly.
			SessionInfo session = SessionsTable::getClientSession(packet->systemAddress);
			if (session.phase > SessionPhase::PHASE_NONE) {
				parsePacket(WorldLoop::rakServer, packet->systemAddress, pdata, (unsigned long)(packet->length - 1));
			}
			else {
				Logger::log("WRLD", "CLIENT", "Recieved packet from unconnected user " + std::string(packet->systemAddress.ToString()));
			}
		}
		break;
		case ID_NEW_INCOMING_CONNECTION:
#ifdef DEBUG
			Logger::log("WRLD", "CLIENT", "Receiving a new connection...");
#endif
			break;

		case ID_DISCONNECTION_NOTIFICATION:
		{
			PacketTools::printRest(pdata);
			SessionInfo session = SessionsTable::getClientSession(packet->systemAddress);
			std::string name = AccountsTable::getAccountName(session.accountid);
			//auto usr = OnlineUsers->Find(packet->systemAddress);
			//OnlineUsers->Remove(packet->systemAddress);
			Logger::log("WRLD", "CLIENT", "Disconnected " + name);
			Friends::broadcastFriendLogout(session.activeCharId);
			ObjectsManager::clientLeaveWorld(session.activeCharId, packet->systemAddress);
			//usr->DestructPlayer();
			Session::disconnect(packet->systemAddress, SessionPhase::PHASE_INWORLD);
		}
		break;
		case DefaultMessageIDTypes::ID_CONNECTION_LOST:
		{
			SessionInfo session = SessionsTable::getClientSession(packet->systemAddress);
			Logger::log("WRLD", "", "Lost connection to " + std::string(packet->systemAddress.ToString()), LOG_ERROR);
			if (session.phase >= SessionPhase::PHASE_AUTHENTIFIED) {
				//auto usr = OnlineUsers->Find(packet->systemAddress);
				if (session.phase >= SessionPhase::PHASE_PLAYING) {
					Friends::broadcastFriendLogout(session.activeCharId);
					ObjectsManager::clientLeaveWorld(session.activeCharId, packet->systemAddress);
					//usr->DestructPlayer();
				}
				//OnlineUsers->Remove(packet->systemAddress);
			}
			Session::disconnect(packet->systemAddress, SessionPhase::PHASE_INWORLD);
		}
		break;
		default:
			stringstream s;
			Logger::log("WRLD", "", "recieved unknown packet [" + std::to_string(packetId) + "]", LOG_DEBUG);
			Logger::log("WRLD", "", RawDataToString(packet->data, packet->length), LOG_DEBUG);
		}
		WorldLoop::rakServer->DeallocatePacket(packet);
	}
}

RakPeerInterface* WorldLoop::rakServer;
void WorldLoop(CONNECT_INFO* cfg) {

	// Initialize the RakPeerInterface used throughout the entire server
	WorldLoop::rakServer = RakNetworkFactory::GetRakPeerInterface();

	// Initialize the PacketFileLogger plugin (for the logs)
	PacketFileLogger* msgFileHandler = NULL;
	if (cfg->logFile) {
		msgFileHandler = new PacketFileLogger();
		WorldLoop::rakServer->AttachPlugin(msgFileHandler);
	}

	// Initialize security IF user has enabled it in config.ini
	InitSecurity(WorldLoop::rakServer, cfg->useEncryption);

	// Initialize the SocketDescriptor
	SocketDescriptor socketDescriptor(cfg->listenPort, 0);

	SystemAddress ServerAddress;
	ServerAddress.SetBinaryAddress(cfg->redirectIp);
	ServerAddress.port = cfg->listenPort;
	
	// If the startup of the server is successful, print it to the console
	// Otherwise, quit the server (as the char server is REQUIRED for the
	// server to function properly)
	if (WorldLoop::rakServer->Startup(8, 30, &socketDescriptor, 1)) {
		Logger::log("WRLD", "", "started! Listening on port " + std::to_string(cfg->listenPort));
		
		Instances::currentInstance = Instances::registerInstance(ServerAddress);
	} else exit(2);

	// Set max incoming connections
	WorldLoop::rakServer->SetMaximumIncomingConnections(cfg->slots);

	// If msgFileHandler is not NULL, save logs of char server
	if (msgFileHandler != NULL) msgFileHandler->StartLog(".\\logs\\world");

	// Initialize the Packet class for the packets
	Packet* packet;

	// This will be used in the saving of packets below
	int i = 0;

	ReplicaManager replicaManager;
	NetworkIDManager networkIdManager;
	// -- REPLICA MANAGER -- //
	WorldLoop::rakServer->AttachPlugin(&replicaManager);
	WorldLoop::rakServer->SetNetworkIDManager(&networkIdManager);

	replicaManager.SetAutoParticipateNewConnections(true);
	//replicaManager.SetAutoConstructToNewParticipants(true);
	replicaManager.SetAutoSerializeInScope(true);

	//Since we are the server, we have the ID Authority
	networkIdManager.SetIsNetworkIDAuthority(true);

	// 
	//	We can implement this, but it seems we don't have to atm.
	//	Actually we should do that at some point to find out if the client tries to create anything like minigames or so
	//
	// Set the function that will get messages from Replica::SendConstruction
	//replicaManager.SetReceiveConstructionCB(&constructionCB);
	// Set the optional callbacks to send and receive download complete notifications
	//replicaManager.SetDownloadCompleteCB(&sendDownloadCompleteCB, &receiveDownloadCompleteCB);

	//Before we start handling packets, we set this RakPeer as the world server of this instance
	WorldServer::publishWorldServer(WorldLoop::rakServer, &replicaManager, ServerAddress);

	ChatCommandManager::registerCommands(new FlightCommandHandler());
	ChatCommandManager::registerCommands(new TeleportCommandHandler());
	ChatCommandManager::registerCommands(new WhisperCommandHandler());
	ChatCommandManager::registerCommands(new TestmapCommandHandler());
	ChatCommandManager::registerCommands(new SwitchCommandHandler());
	ChatCommandManager::registerCommands(new AddItemCommandHandler());
	ChatCommandManager::registerCommands(new ItemsCommandHandler());
	ChatCommandManager::registerCommands(new PositionCommandHandler());
	ChatCommandManager::registerCommands(new ClientCommandHandler());
	ChatCommandManager::registerCommands(new AttributeCommandHandler());
	ChatCommandManager::registerCommands(new PermissionCommandHandler());
	ChatCommandManager::registerCommands(new AnnouncementCommandHandler());
	ChatCommandManager::registerCommands(new AdminCommandHandler());
	ChatCommandManager::registerCommands(new SpawnCommandHandler());
	ChatCommandManager::registerCommands(new RemoveCommandHandler());
	ChatCommandManager::registerCommands(new MissionCommandHandler());
	ChatCommandManager::registerCommands(new DropCommandHandler());
	ChatCommandManager::registerCommands(new RotationCommandHandler());
	ChatCommandManager::registerCommands(new PlayAnimationCommandHandler());
	ChatCommandManager::registerCommands(new SmashCommandHandler());
	ChatCommandManager::registerCommands(new FlagCommandHandler());
	ChatCommandManager::registerCommands(new MacroCommandHandler());
	ChatCommandManager::registerCommands(new KickCommandHandler());
	ChatCommandManager::registerCommands(new CloakCommandHandler());
	ChatCommandManager::registerCommands(new TestCommandHandler());

	bool LUNI_WRLD = true;
	std::vector<unsigned char> buffer;
	bool buffer_started = false;

	CharactersTable::UpgradeTable();

	Macro::LoadMacros();
	StaticObjectsDB::loadAll();

	//VE - Imagination Orbs
	StaticObjectsDB::initDone = false;
	ObjectsManager::registerObject(new GenericObject(4768, 1000, COMPONENT1_POSITION(-281.1628112792969, 606.0819702148438,32.6438102722168  )));
	ObjectsManager::registerObject(new GenericObject(4768, 1000, COMPONENT1_POSITION(-254.1886444091797, 601.4710083007812,-75.30770874023438)));
	ObjectsManager::registerObject(new GenericObject(4768, 1000, COMPONENT1_POSITION(-281.0174865722656, 601.5693359375,   -75.13247680664062)));
	ObjectsManager::registerObject(new GenericObject(4768, 1000, COMPONENT1_POSITION(-267.5953063964844, 606.9742431640625,-97.90109252929688)));
	ObjectsManager::registerObject(new GenericObject(4768, 1000, COMPONENT1_POSITION(-254.92630004882812,606.8737182617188,32.47333908081055 )));
	ObjectsManager::registerObject(new GenericObject(4768, 1000, COMPONENT1_POSITION(-267.8657531738281, 608.3709716796875,24.153282165527344)));
	StaticObjectsDB::initDone = true;

	// some service thread
	std::thread wsThrd(WebSocketService::WSService);
	wsThrd.detach();

	// This will be used in the saving of packets below...
	LiveUpdateTable::createIfNotExists();
	std::thread LIT_THRD(LiveUpdateTable::instance);
	LIT_THRD.detach();
	while (LUNI_WRLD) {
		WorldLoopThread(buffer, LUNI_WRLD, buffer_started, i);
	}

	int instanceid = InstancesTable::getInstanceId(ServerAddress);
	std::vector<SessionInfo> sessions_logout = SessionsTable::getClientsInInstance(instanceid);

	RakNet::BitStream * disconnectNote = WorldServer::initPacket(RemoteConnection::GENERAL, 1);
	disconnectNote->Write((int) 5); //Server Shutdown
	disconnectNote->Write((int) 0); //???
	for (unsigned int k = 0; k < sessions_logout.size(); k++){
		SessionsTable::disconnect(sessions_logout.at(k).addr);
		WorldServer::sendPacket(disconnectNote, sessions_logout.at(k).addr);
		//rakServer->CloseConnection(sessions_logout.at(k).addr, true);
		Logger::log("WRLD", "SERVER", "Close connection to " + std::string(sessions_logout.at(k).addr.ToString()), LOG_DEBUG);
	}

	InstancesTable::unregisterInstance(ServerAddress);

	Logger::log("WRLD", "", "Quitting");
	WorldLoop::rakServer->Shutdown(0);
	RakNetworkFactory::DestroyRakPeerInterface(WorldLoop::rakServer);


	while (!_kbhit()){

	}
	//LUNI_WRLD = false;
}

//This function should get the packet WITHOUT the leading 0x53. bytelength should account for that
//Please note that it CAN be caused recursively, so the read offset should never be set to a static length
void parsePacket(RakPeerInterface* rakServer, SystemAddress &systemAddress, RakNet::BitStream *data, unsigned long bytelength){
	//unsigned long packetLength = data->GetNumberOfUnreadBits() >> 3;
	unsigned short connectionType;
	data->Read(connectionType);
	unsigned long packetId;
	data->Read(packetId);
	unsigned char padding;
	data->Read(padding);

	bool printData = false;

	switch (connectionType){
	case RemoteConnection::GENERAL:
		//Message for Handshake
		switch (packetId){
		case ServerPacketID::VERSION_CONFIRM:
		{
			DoHandshake(rakServer, systemAddress, data, "WRLD");
		}
			break;
		default:
			Logger::log("WRLD", "PACKETS", "Recieved unknown GENERAL packet (" + std::to_string(packetId) + ")"), LOG_DEBUG;
		}
		printData = true;
		break;
	case RemoteConnection::AUTH:
		//Message during AUTHENTIFICATION
		Logger::log("WRLD", "PACKETS", "Recieved unknown AUTH packet (" + std::to_string(packetId) + ")"), LOG_DEBUG;
		printData = true;
		break;
	case RemoteConnection::CHAT:
		//Chat related messages
		switch (packetId){
		case ChatPacketID::PRIVATE_CHAT_MESSAGE:
		{
			unsigned long long a0;
			unsigned char chatChannel;
			unsigned long msgLength;
			data->Read(a0);
			data->Read(chatChannel);
			data->Read(msgLength);
			std::wstring sender = PacketTools::ReadFromPacket(data, 33);
			long long sobjid;
			data->Read(sobjid);
			unsigned short u;
			data->Read(u);
			unsigned char SiM;
			data->Read(SiM);
			bool senderIsMythran = (SiM == 1);
			std::wstring reciever = PacketTools::ReadFromPacket(data, 33);
			unsigned char RiM;
			data->Read(RiM);
			bool recieverIsMythran = (RiM == 1);
			unsigned char returnValue;
			data->Read(returnValue);
			std::wstring msg = PacketTools::ReadFromPacket(data, msgLength);
			Logger::log("WRLD", "CHAT", "[" + std::to_string(chatChannel) + "] Private Chat Message: status " + std::to_string(returnValue), LOG_DEBUG);
			std::stringstream senders;
			senders << std::to_string(sobjid);
			if (senderIsMythran) senders << " (Mythran)";
			Logger::log("WRLD", "CHAT", "[" + std::to_string(chatChannel) + "] SENDER:   " + senders.str(), LOG_DEBUG);
			Logger::log("WRLD", "CHAT", "[" + std::to_string(chatChannel) + "] RECIEVER: " + UtfConverter::ToUtf8(reciever), LOG_DEBUG);
			std::string msgs = UtfConverter::ToUtf8(msg);
			if (msgs.size() > 20) msgs = msgs.substr(0, 17) + "...";
			Logger::log("WRLD", "CHAT", "[" + std::to_string(chatChannel) + "] MESSAGE:  " + msgs, LOG_DEBUG);
		}
			break;
		case ChatPacketID::ADD_FRIEND_REQUEST:
		{
			unsigned long long unknown;
			data->Read(unknown);
			if (unknown != 0) Logger::log("WRLD", "FRIENDS_REQUEST", "8bytes not 0 please investigate", LOG_DEBUG);
			std::vector<wchar_t> input;
			bool eos = false;
			for (unsigned int k = 0; k < 33; k++){
				wchar_t chr;
				data->Read(chr);
				if (chr == 0) eos = true;
				if (!eos) input.push_back(chr);
			}
			std::wstring name(input.begin(), input.end());
			unsigned char isBestFriendRequest;
			data->Read(isBestFriendRequest);

			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s.activeCharId);

			std::string sname = UtfConverter::ToUtf8(name);
			std::string uname = cinfo.info.name;
			Friends::sendFriendRequest(uname, sname, (isBestFriendRequest == 1));
		}
			break;
		case ChatPacketID::ADD_FRIEND_RESPONSE:
		{
			SessionInfo s = SessionsTable::getClientSession(systemAddress);

			long long unknown;
			data->Read(unknown);
			unsigned char status;
			data->Read(status);
			std::wstring name = PacketTools::ReadFromPacket(data, 33);
			Logger::log("WRLD", "SOCIAL", "DECLINED? " + std::to_string(status), LOG_DEBUG);
			Friends::handleFriendRequestResponse(s.activeCharId, name, status);
		}
			break;
		case ChatPacketID::GET_FRIENDS_LIST:
		{
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			Logger::log("WRLD", "PARSER", "Requesting Friends-List");
			Friends::handleWorldJoin(s.activeCharId);
		}
			break;
		case ChatPacketID::GET_IGNORE_LIST:
			Logger::log("WRLD", "PARSER", "Requesting Ignore-List");
			//TODO: probably similar to implementing friends using 53-05-00-1e
			break;
		case ChatPacketID::TEAM_INVITE:
			Logger::log("WRLD", "PARSER", "Send Team invite", LOG_DEBUG);
			break;
		case ChatPacketID::TEAM_GET_STATUS:
			Logger::log("WRLD", "PARSER", "Requesting Team status");
			//TODO
			break;
		case ChatPacketID::REQUEST_MINIMUM_CHAT_MODE_PRIVATE:
		{
			unsigned long long a;
			unsigned char b;
			data->Read(a);
			data->Read(b);
			std::wstring pstr = PacketTools::ReadFromPacket(data, 33);
			Logger::log("WRLD", "CHAT", "Client requests private chat with " + UtfConverter::ToUtf8(pstr) + " (" + std::to_string(a) + "|" + std::to_string(b) + ")");
		}
			break;
		default:
			Logger::log("WRLD", "PACKET", "Recieved unknown CHAT packet (" + std::to_string(packetId) + ")");
			printData = true;
			break;
		}
		printData = true;
		break;
	case RemoteConnection::UNKNOWNCONN:
		//Unknown connection type
		Logger::log("WRLD", "PACKET", "Recieved unknown UNKNOWNCONN packet (" + std::to_string(packetId) + ")");
		printData = true;
		break;
	case RemoteConnection::SERVER:
		//Message to server
		switch (packetId){
		// 1
		case WorldPacketID::CLIENT_VALIDATION:
		{
			//When the world server recieves a validation packet, check it and continue
			bool flag = ClientWorldPackets::HandleValidation(data, systemAddress);
			if (!flag){
				rakServer->CloseConnection(systemAddress, true);
			}

			//When the client is validated, send him to the right world.
			//Actually, this should finally not be done here, but as a response to the char packet that sends the client to this server

			//ONLY USE WORLD FROM NOW ON
			//SessionInfo s = SessionsTable::getClientSession(systemAddress);
			//ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s.activeCharId);
			//COMPONENT1_POSITION pos = COMPONENT1_POSITION(cinfo.lastPlace.x, cinfo.lastPlace.y, cinfo.lastPlace.z);
			//ZoneId zone = static_cast<ZoneId>(cinfo.lastPlace.zoneID);
			//Worlds::loadWorld(systemAddress, zone, pos, cinfo.lastPlace.mapInstance, cinfo.lastPlace.mapClone);
		}
			break;
		// 2
		case WorldPacketID::CLIENT_CHARACTER_LIST_REQUEST:
		{
			//Only happens when getting charlist ingame
			Logger::log("WORLD", "CHARS", "CLIENT_CHARACTER_LIST_REQUEST");
			Logger::log("WORLD", "CHARS", "Sending char packet...");
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			if (s.phase > SessionPhase::PHASE_PLAYING){
				ObjectsManager::clientLeaveWorld(s.activeCharId, systemAddress);
				Session::quit(s.activeCharId);
			}
			SendCharPacket(rakServer, systemAddress, s.accountid);
		}
			break;
		// 3
		case WorldPacketID::CLIENT_CHARACTER_CREATE_REQUEST:
		{
			// Find online user by systemAddress
			//auto usr = OnlineUsers->Find(packet->systemAddress);
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			bool success = false;
			if (s.phase > SessionPhase::PHASE_CONNECTED) {
				success = Characters::CreateCharacter(data, systemAddress, s.accountid);
			}
			else {
				Logger::logError("CHAR", "", "saving user", "not authentified");
			}

			// If the username is in use, do NOT send the char packet. Otherwise, send it
			if (success) {
				SendCharPacket(rakServer, systemAddress, s.accountid);
			}
		}
			break;
		// 4
		case WorldPacketID::CLIENT_LOGIN_REQUEST:
		{
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			SessionInfo *t = &SessionsTable::getClientSession(systemAddress);
			long long objid;
			data->Read(objid);
			if (s.phase >= SessionPhase::PHASE_AUTHENTIFIED) {
				//usr->DestructPlayer();
				//usr->SetCharacter(objid);
				Session::play(s.accountid, objid);
				s = SessionsTable::getClientSession(systemAddress);
				
				ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s.activeCharId);
				COMPONENT1_POSITION pos = COMPONENT1_POSITION(cinfo.lastPlace.x, cinfo.lastPlace.y, cinfo.lastPlace.z);
				ZoneId zone = static_cast<ZoneId>(cinfo.lastPlace.zoneID);
				Worlds::loadWorld(systemAddress, zone, pos, cinfo.lastPlace.mapInstance, cinfo.lastPlace.mapClone);
				
				//RakNet::BitStream worldLoad;
				//usr->LoadWorld(usr->getWorld(), &worldLoad);
				//rakServer->Send(&worldLoad, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
			}
		}
			break;
		case WorldPacketID::SERVER_STATES: {
			//Todo
		}
		// 5
		case WorldPacketID::CLIENT_GAME_MSG:
		{
			long long objid;
			data->Read(objid);

			std::stringstream ss;
			data->SetReadOffset(data->GetReadOffset() - 32);
			for (unsigned int k = 0; k < 32; k++){
				bool flag;
				data->Read(flag);
				if (flag){
					ss << " [" << (k) << "]";
				}
			}
			std::string flags = ss.str();

			unsigned short msgid;
			data->Read(msgid);

			//We ignore what was here, because bitstream seems way better to use in this case.
			//The code gave a hint that the "objid" could be the character using the object
			//Actually it is mostly the charcter, but in some cases it is not
			if (!msgid==888) {
				Logger::log("WRLD", "PARSER", "Game Message, ID: " + std::to_string(msgid), LOG_DEBUG);
				Logger::log("WRLD", "PARSER", "OBJECT-ID: " + std::to_string(objid), LOG_DEBUG);
			}

			GameMSG::parseGameMSG(msgid, data, systemAddress);

			//I left this there because Alan hasn't released his second version of the missions already and i don't want to damage it.
			/*switch (msgid) {
			case RESOND_TO_MISSION: {
				break;
			}
			case MISSION_DIALOGUE_CANCELLED: {
				break;
			} 
			case MISSION_DIALOGUE_OK: {
				bool bIsComplete;
				data->Read(bIsComplete);
				int MissionState;
				data->Read(MissionState);
				int missionID;
				data->Read(missionID);
				//The Responder is not required, it is the Char objid. That we can get from SessionsTable

				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s.activeCharId);

				//Write Accept In Database - If you allready have the mission, we just update the Status.
				if (bIsComplete){
					//Write Update in Database
					Database::Query("UPDATE `missions` SET `status` = '" + std::to_string(MissionState) + "' WHERE `missions`.`character` = '" + std::to_string(s.activeCharId) + "' AND `missions`.`missionId` = '" + std::to_string(missionID) + "'");
					//Log
					Logger::log("WRL", "MISSION", "Mission Updated! Mission Id: " + std::to_string(missionID) + " - Char: " + std::to_string(s.activeCharId) + " - Status: "+std::to_string(MissionState), LOG_DEBUG);
					Chat::sendMythranInfo(s.activeCharId, "Missions are in BETA stage, if you get any troubles with it, try relog before posting bugs!.", "WARNING!");
					MissionCommandHandler handler;
					SessionInfo *s = &SessionsTable::getClientSession(systemAddress);
					std::vector<std::wstring> param;
					param.push_back(L"Reward");
					param.push_back(std::to_wstring(missionID));
					handler.handleCommand(s, &param);
				}
				else{
					auto qr = Database::Query("SELECT * FROM `missions` WHERE `character` = '"+std::to_string(s.activeCharId)+"' AND `missionId` = '"+std::to_string(missionID)+"'");
					if (mysql_num_rows(qr) <= 0){
						//Inserct In Databse
						Database::Query("INSERT INTO `missions` (`id`, `character`, `missionid`, `time`, `count`, `status`) VALUES (NULL, '" + std::to_string(s.activeCharId) + "', '" + std::to_string(missionID) + "', CURRENT_TIMESTAMP, '0', '" + std::to_string(MissionState) + "');");
						//Log
						Logger::log("WRL", "MISSION", "Mission Accepted! Mission Id: " + std::to_string(missionID) + " - Char: " + std::to_string(s.activeCharId), LOG_DEBUG);
					}
				}
				RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(s.activeCharId, 254);
				ef->Write((unsigned long)missionID);
				ef->Write((unsigned long)2);
				ef->Write((bool)bIsComplete);
				WorldServer::sendPacket(ef, s.addr);
				break;
			}
			case CANCEL_MISSION:
			{
				int missionId;
				data->Read(missionId);
				bool resetCompleted;
				data->Read(resetCompleted);

				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s.activeCharId);

				//Log
				Logger::log("WRL", "MISSION", "Player: " + std::to_string(s.activeCharId) + " - has cancelled mission: " + std::to_string(missionId), LOG_DEBUG);

				//Delete Mission From Database
				Database::Query("DELETE FROM `missions` WHERE `missions`.`character` = '" + std::to_string(s.activeCharId) + "' AND `missions`.`missionid` = '" + std::to_string(missionId) + "';");
			}
			break;
			case RESET_MISSIONS:
			{
				int missionId;
				data->Read(missionId);

				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s.activeCharId);

				//Log
				Logger::log("WRL", "MISSION", "Player: " + std::to_string(s.activeCharId) + " - has reset mission: " + std::to_string(missionId), LOG_DEBUG);

				//Reset Mission In DB
				auto qr = Database::Query("SELECT * FROM `missions` WHERE `missions`.`character` = '" + std::to_string(s.activeCharId) + "' AND `missions`.`missionid` = '" + std::to_string(missionId) + "';");
				if (mysql_num_rows(qr) > 0){
					Database::Query("UPDATE `missions` SET `count` = '0', `status` = '0' WHERE `missions`.`character` = '" + std::to_string(s.activeCharId) + "' AND `missions.`missionid` = '" + std::to_string(missionId) + "';");
				}
				else{
					//Rows = 0?
				}
			} break
			
			default:
				Logger::log("WRLD", "GAMEMESSAGE", "Unknown Game Message: " + std::to_string(msgid));
				break;
			}
			printData = true;*/
		}
			break;




		// 6
		case WorldPacketID::CLIENT_CHARACTER_DELETE_REQUEST:
		{
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			long long charid;
			data->Read(charid);
			Characters::DeleteCharacter(s.accountid, charid);
			RakNet::BitStream * bitStream = WorldServer::initPacket(RemoteConnection::CLIENT, 12);
			bitStream->Write((unsigned char)1); // Success?
			WorldServer::sendPacket(bitStream, systemAddress);
		}
			break;
		// 14
		case WorldPacketID::CLIENT_GENERAL_CHAT_MESSAGE:
		{
			//data->SetReadOffset(data->GetReadOffset() + 24); //3 bytes channel?
			byte cb1;
			byte cb2;
			byte cb3;
			data->Read(cb1);
			data->Read(cb2);
			data->Read(cb3);

			Logger::log("WRLD", "CLIENT_GENERAL_CHAT_MESSAGE", "Channel: " + std::to_string(cb1) + " " + std::to_string(cb2) + " " + std::to_string(cb3));

			unsigned long length;
			data->Read(length);
			std::vector<wchar_t> messageVector;
			messageVector.reserve(length);
			for (unsigned long k = 0; k < length; k++){
				wchar_t mc;
				data->Read(mc);
				if (mc == 0) break; else messageVector.push_back(mc);
			}

			std::wstring message(messageVector.begin(), messageVector.end());
			
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s.activeCharId);
			
			Logger::log("WRLD", "CHAT", cinfo.info.name + ": " + UtfConverter::ToUtf8(message));
			Chat::broadcastChatMessage(s.zone, message, UtfConverter::FromUtf8(cinfo.info.name));
		}
			break;
		// 19
		case WorldPacketID::CLIENT_LEVEL_LOAD_COMPLETE:
		{
			ZoneAddress za = ClientWorldPackets::HandleLevelLoadComplete(data, systemAddress);

			Logger::log("WRLD", "LEVEL", "Client loading complete");
			Logger::log("WRLD", "LEVEL", "Replying to Character");

			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			SessionInfo *t = &SessionsTable::getClientSession(systemAddress);
			if (s.activeCharId > 0){
				long long objid = s.activeCharId;
				ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(objid);

				COMPONENT1_POSITION pos;
				COMPONENT1_ROTATION rot = COMPONENT1_ROTATION();
				ZoneId zid = static_cast<ZoneId>(za.zoneid);
				if (za.zoneid == cinfo.lastPlace.zoneID){
					pos = COMPONENT1_POSITION(cinfo.lastPlace.x, cinfo.lastPlace.y, cinfo.lastPlace.z);
					rot = COMPONENT1_ROTATION(cinfo.lastPlace.rotX, cinfo.lastPlace.rotY, cinfo.lastPlace.rotZ, cinfo.lastPlace.rotW);
					if (pos.isNull()) pos = getZoneSpawnPoint(zid);
				}
				else{
					pos = getZoneSpawnPoint(zid);
				}
				if (za.zoneid == 1000){
					pos = COMPONENT1_POSITION(-625.303, 613.414, -25.6505);
					rot = COMPONENT1_ROTATION(0, 0.7, 0, 0.7);
				}

				WorldServerPackets::CreateCharacter(systemAddress, objid);

				PlayerObject * player = new PlayerObject(objid, UtfConverter::FromUtf8(cinfo.info.name));

				//Temporarily ?
				player->gmlevel = (unsigned char)cinfo.info.gmlevel;
				player->world.zone = zid;
				player->world.instance = Instances::currentInstance;
				player->world.clone = zid; //If on property -> change to propertyID -> 3000 + properyId

				// TODO: Add Clone-Switch while adding properties

				ControllablePhysicsComponent * c1 = player->getComponent1();
				c1->setPosition(pos);
				c1->setRotation(rot);


				//It looks as though this is the code that needs to be changed in order to fix levels
				CharacterComponent * c4 = player->getComponent4();
				c4->setLevel(cinfo.info.level);
				PLAYER_INFO pi;
				pi.accountID = s.accountid;
				pi.isFreeToPlay = cinfo.info.isFreeToPlay;
				pi.legoScore = cinfo.info.uscore;
				c4->setInfo(pi);
				PLAYER_STYLE ps;
				ps.eyebrowsStyle = cinfo.style.eyebrows;
				ps.eyesStyle = cinfo.style.eyes;
				ps.hairColor = cinfo.style.hairColor;
				ps.hairStyle = cinfo.style.hairStyle;
				ps.mouthStyle = cinfo.style.mouth;
				ps.pantsColor = cinfo.style.pantsColor;
				ps.shirtColor = cinfo.style.shirtColor;
				c4->setStyle(ps);

				DestructibleComponent * c7 = player->getComponent7();
				COMPONENT7_DATA4 d4 = c7->getData4();
				d4.health = 4;
				d4.maxHealthN = 4.0F;
				d4.maxHealth = 4.0F;

				d4.imagination = 0;
				d4.maxImagination = 0.0f;
				d4.maxImaginationN = 0.0f;

				d4.armor = 0;
				d4.maxArmor = 0.0f;
				d4.maxArmorN = 0.0f;

				c7->setData4(d4);
				c7->getData4_1Ref()->push_back(1);
				c7->setData4_4_1(true);
				c7->setData5(false);

				InventoryComponent * c17 = player->getComponent17();
				std::vector<long long> equip = EquipmentTable::getItems(objid);
				for (unsigned int k = 0; k < equip.size(); k++){
					unsigned long equipLOT = c17->equipItem(equip.at(k));

					LootObject *lootObj = new LootObject(equipLOT, equip.at(k));
					ObjectsManager::registerObject(lootObj);
				}

				player->getComponent4()->setStats(StatsTable::getPlayerStats(s.activeCharId));

				AccountsTable::setFrontChar(player->objid);

				//usr->SetPlayer(player);

				ObjectsManager::clientJoinWorld(player, systemAddress);
				//player->doCreation(packet->systemAddress, zid);

				WorldServerPackets::SendGameMessage(systemAddress, objid, 1642);
				WorldServerPackets::SendGameMessage(systemAddress, objid, 509);

				RakNet::BitStream *pc = WorldServerPackets::InitGameMessage(objid, 472);
				pc->Write((unsigned long)185);
				pc->Write((unsigned char)0);
				WorldServer::sendPacket(pc, systemAddress);

				long long leftHand = EquipmentTable::getFromItemType(s.activeCharId, ItemType::LEFT_HAND);
				long long rightHand = EquipmentTable::getFromItemType(s.activeCharId, ItemType::RIGHT_HAND);
				if (leftHand != NULL)
					GameMSG::equipItem(s.activeCharId, leftHand);
				if (rightHand != NULL)
					GameMSG::equipItem(s.activeCharId, rightHand);

				Logger::log("WRLD", "PARSER", "Enter session.");

				Session::enter(s.activeCharId, zid);

				Logger::log("WRLD", "PARSER", "Client: Level loading complete " + zid);
				
			}			
		}
			break;
		// 21
		case WorldPacketID::CLIENT_ROUTE_PACKET:
		{
			unsigned long subPacketLength;
			data->Read(subPacketLength);
			Logger::log("WRLD", "PARSER", "Recieved routing packet; Length: " + std::to_string(subPacketLength));
			parsePacket(rakServer, systemAddress, data, subPacketLength);
		}
			break;
		// 22
		case WorldPacketID::CLIENT_POSITION_UPDATE: //user moving / update request?
		{
			SessionInfo i = SessionsTable::getClientSession(systemAddress);
			if (i.phase > SessionPhase::PHASE_PLAYING){
				PlayerObject * player = (PlayerObject *)ObjectsManager::getObjectByID(i.activeCharId);

				if (player != NULL){
					ControllablePhysicsComponent *c1 = player->getComponent1();

					float x, y, z;
					data->Read(x);
					data->Read(y);
					data->Read(z);
					c1->setPosition(COMPONENT1_POSITION(x, y, z));

					float rx, ry, rz, rw;
					data->Read(rx);
					data->Read(ry);
					data->Read(rz);
					data->Read(rw);
					c1->setRotation(COMPONENT1_ROTATION(rx, ry, rz, rw));

					bool onGround;
					data->Read(onGround);
					c1->setOnGround(onGround);


					bool unknownBit;
					data->Read(unknownBit);
					c1->setData6_d4(unknownBit);
					bool hasVelocityInfo;
					data->Read(hasVelocityInfo);

					if (hasVelocityInfo){
						float vx, vy, vz;
						data->Read(vx);
						data->Read(vy);
						data->Read(vz);
						c1->setVelocity(COMPONENT1_VELOCITY(vx, vy, vz));
					}
					else{
						c1->setVelocity(COMPONENT1_VELOCITY(0, 0, 0));
					}

					bool hasAngularVelocityInfo;
					data->Read(hasAngularVelocityInfo);

					if (hasAngularVelocityInfo){
						float avx, avy, avz;
						data->Read(avx);
						data->Read(avy);
						data->Read(avz);
						c1->setAngularVelocity(COMPONENT1_VELOCITY_ANGULAR(avx, avy, avz));
					}
					else{
						c1->setAngularVelocity(COMPONENT1_VELOCITY_ANGULAR(0, 0, 0));
					}
					ObjectsManager::serialize(player); //player->serialize();

					WorldPlace place;
					place.zoneID = player->world.zone;
					place.mapClone = player->world.clone;
					place.mapInstance = player->world.instance;
					place.x = x;
					place.y = y;
					place.z = z;
					place.rotX = rx;
					place.rotY = ry;
					place.rotZ = rz;
					place.rotW = rw;

					CharactersTable::setCharactersPlace(player->objid, place);
					if (unknownBit) {
						Logger::log("WRLD", "PMUP", "ALERT: Unknown Bit of Player is TRUE!");
						Chat::sendMythranInfo(i.activeCharId, "Congratulations! You found an unknown thing! Please Report to a developer \"found unknownBit GM:22\" and then describe what you did! Thanks!", "Server");
					}
					//PlayerObject (PlayerObject *)ObjectsManager::getObjectByID(i.activeCharId));
					unsigned short world = player->world.zone;
					COMPONENT1_POSITION pos = player->getComponent1()->getPosition();
					COMPONENT1_ROTATION rot = player->getComponent1()->getRotation();
					switch (world) {
					case 1000: {
						if (pos.y < 566) {
							Logger::log("WRLD", "TRIGGER", "Smashing Character");
							GameMSG::die(player->objid, true, false, 0.0f, L"WorldDeath", 0.0f, 0.0f, 0.0f, player->objid, player->objid);
						}
						break;
					}
					case 1400: {
						if (pos.y < 0) {
							Logger::log("WRLD", "TRIGGER", "Smashing Character");
							GameMSG::die(player->objid, true, false, 0.0f, L"WorldDeath", 0.0f, 0.0f, 0.0f, player->objid, player->objid);
						}
						break;
					}
					case 1800: {
						if (pos.y < -70) {
							Logger::log("WRLD", "TRIGGER", "Smashing Character");
							GameMSG::die(player->objid, true, false, 0.0f, L"WorldDeath", 0.0f, 0.0f, 0.0f, player->objid, player->objid);
						}
						break;
					}
					case 2000: {
						if (pos.y < 50) {
							Logger::log("WRLD", "TRIGGER", "Smashing Character");
							GameMSG::die(player->objid, true, false, 0.0f, L"WorldDeath", 0.0f, 0.0f, 0.0f, player->objid, player->objid);
						}
						break;
					}
					case 1450:
					case 1350:
					case 1250:
					case 1150:
					case 1151:
					{
						if (pos.y < 400) {
							Logger::log("WRLD", "TRIGGER", "Smashing Character");
							GameMSG::die(player->objid, true, false, 0.0f, L"WorldDeath", 0.0f, 0.0f, 0.0f, player->objid, player->objid);
						}
						break;
					}
					}
				}
				else{
					//Player is null????
				}
			}
		}
			break;
		// 23
		case WorldPacketID::CLIENT_MAIL:
		{
			long mailid;
			data->Read(mailid);
			switch (mailid){
			case 0:
			{
				std::wstring subject = PacketTools::ReadFromPacket(data, 50);
				std::wstring text = PacketTools::ReadFromPacket(data, 400);
				std::wstring recipient = PacketTools::ReadFromPacket(data, 32);
				long long d1;
				data->Read(d1);
				long long attach;
				data->Read(attach);
				unsigned short attcount;
				data->Read(attcount);
				unsigned short lang;
				data->Read(lang);
				unsigned long d3;
				data->Read(d3);

				Logger::log("WLRD", "MAIL", "Send Mail Request", LOG_DEBUG);
				Logger::log("WLRD", "MAIL", "TO:           " + UtfConverter::ToUtf8(recipient), LOG_ALL);
				Logger::log("WLRD", "MAIL", "SUBJECT:      " + UtfConverter::ToUtf8(subject), LOG_ALL);
				Logger::log("WLRD", "MAIL", "TEXT:         " + UtfConverter::ToUtf8(text), LOG_ALL);
				Logger::log("WLRD", "MAIL", "LANGUAGE:     " + std::to_string(lang), LOG_ALL);
				Logger::log("WLRD", "MAIL", "ATTACHMENT:   " + std::to_string(attach) + "[" + std::to_string(attcount) + "]", LOG_ALL);
				Logger::log("WLRD", "MAIL", "D1/D3:        " + std::to_string(d1) + "/" + std::to_string(d3), LOG_ALL);

				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				long long charid = s.activeCharId;
				Mail::requestMailSending(charid, subject, text, recipient, attach, attcount);
			}
				break;
			case 3:
			{
				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				Mail::loadMailboxData(s.activeCharId);
			}
				break;
			case 5:
			{
				unsigned long ua;
				data->Read(ua);

				long long mid;
				data->Read(mid);
				long long objid;
				data->Read(objid);
				Logger::log("WRLD", "MAIL", "Requesting remove attachment for mail id " + std::to_string(mid), LOG_DEBUG);
				Logger::log("WRLD", "MAIL", std::to_string(ua), LOG_DEBUG);
				Mail::removeAttachment(mid, objid);
			}
				break;
			case 7:
			{
				long d4;
				data->Read(d4);
				long long mid;
				data->Read(mid);
				long long charid;
				data->Read(charid);
				Mail::deleteMail(mid, charid);
			}
				break;
			case 9:
			{
				long d2;
				data->Read(d2);
				long long mid;
				data->Read(mid);
				MailsTable::setIsRead(mid);
				Logger::log("WRLD", "MAIL", "Mail Read: " + std::to_string(mid), LOG_DEBUG);
			}
				break;
			default:
				Logger::log("WLRD", "MAIL", "Mail ID: " + std::to_string(mailid), LOG_DEBUG);
				printData = true;
			}
		}
			break;
		// 25
		case WorldPacketID::CLIENT_STRING_CHECK: //check the chat string
		{
			unsigned char superchatlevel;
			data->Read(superchatlevel);
			unsigned char requestID;
			data->Read(requestID);
			std::wstring reciever = PacketTools::ReadFromPacket(data, 42);
			unsigned short length;
			data->Read(length);
			std::wstring text = PacketTools::ReadFromPacket(data, length);

			std::stringstream str;
			str << "Moderation Request #" << std::to_string(requestID);
			if (reciever != L""){
				str << " for " << UtfConverter::ToUtf8(reciever);
			}
			str << ": '";
			if (text.size() > 20){
				str << UtfConverter::ToUtf8(text.substr(0, 17)) << "...";
			}
			else{
				str << UtfConverter::ToUtf8(text);
			}
			str << "'";
			Logger::log("WRLD", "MODR", str.str(), LOG_DEBUG);

			RakNet::BitStream  * bs = WorldServer::initPacket(RemoteConnection::CLIENT, 59);
			bs->Write((unsigned char)1); //should be 1, zero here means not allowed
			bs->Write((unsigned short)0);
			bs->Write(requestID);
			WorldServer::sendPacket(bs, systemAddress);
		}
			break;
		// 120
		case WorldPacketID::UGC_DOWNLOAD_FAILED:
		{
			long d1;
			data->Read(d1);
			long long objid;
			data->Read(objid);
			long d2;
			data->Read(d2);
			long long charid;
			data->Read(charid);
			Logger::log("WLRD", "UGC", "Download failed (" + std::to_string(d1) + "|" + std::to_string(d2) + ")", LOG_WARNING);
			Logger::log("WLRD", "UGC", "Object: " + std::to_string(objid), LOG_WARNING);
			Logger::log("WLRD", "UGC", "Player: " + std::to_string(charid), LOG_WARNING);
		}
			break;
		default:
			Logger::log("WLRD", "PACKETS", "Recieved unknown OTHER packet (" + std::to_string(packetId) + ")", LOG_DEBUG);
			printData = true;
			break;
		}
		break;
	case RemoteConnection::CLIENT:
		//pretty sure this happens when the help menu is opened...
		//Message to client
		Logger::log("WLRD", "HELP", "Client opened help menu?", LOG_DEBUG);
		break;
	default:
		Logger::log("WLRD", "PACKETS", "Recieved unknown SERVER packet with id of (" + std::to_string(packetId) + ")", LOG_DEBUG);
		printData = true;
		break;
	}
	

	if (printData){
		if (data->GetNumberOfUnreadBits() > 0){
			PacketTools::printRest(data);
			//cout << RawDataToString(data->GetData(), data->GetNumberOfBytesUsed(), false, (data->GetReadOffset() >> 3)) << endl;
		}else{
			Logger::log("DATA", "", "No more data");
		}
		
	}
}

bool handleObject(ObjectInformation obj, RakPeerInterface* rakServer, SystemAddress &systemAddress){
	if (obj.type != ObjectType::NONE){
		switch (obj.type)
		{
		case ObjectType::LaunchPad:
		{
			ZoneId zone = getLaunchPadTarget(obj);
			if (zone != ZoneId::NO_ZONE && zone != ZoneId::KEELHAUL_CANYON){
				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				COMPONENT1_POSITION pos = getZoneSpawnPoint(zone, static_cast<ZoneId>(s.zone));
				bool flag = Worlds::loadWorld(systemAddress, zone, pos, 0, 0);
				if (flag){
					Session::leave(s.activeCharId);

					WorldPlace place;
					place.zoneID = zone;
					place.mapClone = 0;
					place.mapInstance = 0;
					place.x = pos.x;
					place.y = pos.y;	
					place.z = pos.z;
					CharactersTable::setCharactersPlace(s.activeCharId, place);

					ObjectsManager::clientLeaveWorld(s.activeCharId, systemAddress);
					//usr->DestructPlayer();
				}
			}
		}
			break;
		case ObjectType::PostBox:
		{
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			//Chat::sendMythranInfo(usr->GetCurrentCharacter()->charobjid, "We are sorry to inform you the Mail system isn't working yet!", "Our deepest apologies");
			Mail::openMailbox(s.activeCharId);
		}
			break;
		default:
			try {
				Interact::Interact(obj.objid, systemAddress);
				return true;
			}
			catch (const std::exception&) {
				return false;
			}
			break;
		}
	}
	return true;
}

string lowerString(string str) {
	std::transform(str.begin(), str.end(),
		str.begin(), ::tolower);
	return str;
}