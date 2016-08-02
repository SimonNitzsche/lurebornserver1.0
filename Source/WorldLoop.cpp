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

// - Mechanics -
#include "Account.h"
#include "Characters.h"
#include "ChatCommands.h"
#include "Social.h"
#include "Worlds.h"
#include "GameMessages.h"

// - Network -
#include "CharPackets.h"
#include "legoPackets.h"
#include "Packet.h"
#include "WorldServer.h"
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


void WorldLoop(CONNECT_INFO* cfg) {
	// Initialize the RakPeerInterface used throughout the entire server
	RakPeerInterface* rakServer = RakNetworkFactory::GetRakPeerInterface();

	// Initialize the PacketFileLogger plugin (for the logs)
	PacketFileLogger* msgFileHandler = NULL;
	if (cfg->logFile) {
		msgFileHandler = new PacketFileLogger();
		rakServer->AttachPlugin(msgFileHandler);
	}

	// Initialize security IF user has enabled it in config.ini
	InitSecurity(rakServer, cfg->useEncryption);

	// Initialize the SocketDescriptor
	SocketDescriptor socketDescriptor(cfg->listenPort, 0);

	SystemAddress ServerAddress;
	ServerAddress.SetBinaryAddress(cfg->redirectIp);
	ServerAddress.port = cfg->listenPort;
	
	// If the startup of the server is successful, print it to the console
	// Otherwise, quit the server (as the char server is REQUIRED for the
	// server to function properly)
	if (rakServer->Startup(8, 30, &socketDescriptor, 1)) {
		Logger::log("WRLD", "", "started! Listening on port " + std::to_string(cfg->listenPort));
		Instances::registerInstance(ServerAddress);
	} else exit(2);

	// Set max incoming connections to 8
	rakServer->SetMaximumIncomingConnections(8);

	// If msgFileHandler is not NULL, save logs of char server
	if (msgFileHandler != NULL) msgFileHandler->StartLog(".\\logs\\world");

	// Initialize the Packet class for the packets
	Packet* packet;

	// This will be used in the saving of packets below
	int i = 0;

	ReplicaManager replicaManager;
	NetworkIDManager networkIdManager;
	// -- REPLICA MANAGER -- //
	rakServer->AttachPlugin(&replicaManager);
	rakServer->SetNetworkIDManager(&networkIdManager);

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
	WorldServer::publishWorldServer(rakServer, &replicaManager, ServerAddress);

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

	bool LUNI_WRLD = true;
	std::vector<unsigned char> buffer;
	bool buffer_started = false;
	//load npcs here
	std::ostringstream st;
	st << "SELECT * FROM npcs";
	int state = mysql_query(Database::getConnection(), st.str().c_str());
	if (state != 0){
		//error
	}
	else{
		//okay
		MYSQL_RES *result = mysql_store_result(Database::getConnection());
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(result)) != NULL) {

			NPCObject * npc = new NPCObject(stol(row[1]), stol(row[2]));

			SimplePhysicsComponent *c3 = npc->getSimplePhysicsComponent();

			COMPONENT3_POSITION pos = COMPONENT3_POSITION(stof(row[3]), stof(row[4]), stof(row[5]));
			COMPONENT3_ROTATION rot = COMPONENT3_ROTATION(stof(row[6]), stof(row[7]), stof(row[8]), stof(row[9]));

			c3->setPosition(pos);
			c3->setRotation(rot);

			ObjectsManager::registerObject(npc);
			ObjectsManager::create(npc);

		}
		Logger::log("WRLD", "SMSH", "Load Rocketboxes!");
		std::vector<vector<double>> RandomRocketBoxSpawnerPositionsList = {
			vector<double>{-230.98828125, 580.8716430664062, 60.03725051879883, 0.0, -0.6300126910209656, 0.0, 0.776584804058075}, //Common Engines
			vector<double>{-204.99325561523438, 603.874755859375, 65.57496643066406, 5.699339453713037e-06, -0.4083457887172699, 2.549541932239663e-06, 0.9128273129463196}, //Common Engines
			vector<double>{-226.02926635742188, 580.739990234375, 85.7234115600586, 0.0, -0.633384644985199, 0.0, 0.773837149143219}, //All Nose Cones
			vector<double>{-228.11669921875, 580.87255859375, 73.39395141601562, 0.0, -0.6340587139129639, 0.0, 0.7732849717140198}, //All Cockpits
			vector<double>{-175.4554443359375, 603.8546752929688, 55.86145782470703, 0.0, -0.8110705018043518, 0.0, 0.5849484205245972}, //Common Engines
			vector<double>{-189.98373413085938, 603.87451171875, 69.27857208251953, 0.0, 0.0, 0.0, 1.0}, //Common Engines
			vector<double>{-122.21607971191406, 580.822998046875, -95.9127197265625, 0.0, -0.7626698017120361, 0.0, 0.6467880606651306}, //All Nose Cones
			vector<double>{-545.647216796875, 631.883544921875, -69.66816711425781, 0.0, -0.17021068930625916, 0.0, 0.9854077100753784}, //All Engines
			vector<double>{-456.68865966796875, 580.7138671875, 23.291332244873047, 0.0, 0.39795026183128357, 0.0, 0.9174069762229919}, //All Nose Cones
			vector<double>{-448.4689636230469, 590.806884765625, -3.259254217147827, 0.0, 0.7083539962768555, 0.0,  0.7058573961257935}, //All Cockpits
			vector<double>{-429.6986083984375, 580.605224609375, 18.859256744384766, 0.0, 0.9426403641700745, 0.0, -0.3338102400302887}, //All Cockpits
			vector<double>{-443.56378173828125, 580.7138671875, 23.4738826751709, 0.0, 0.7193519473075867, 0.0, 0.6946458220481873}, //All Cockpits
			vector<double>{-554.799560546875, 631.88330078125, 3.572044610977173, 0.0, 0.17966069281101227, 0.0, 0.9837286472320557}, //All Engines
			vector<double>{-143.2361297607422, 580.7993774414062, -135.09811401367188, 0.0, 0.9838860630989075, 0.0, 0.1787971407175064}, //All Nose Cones
			vector<double>{-454.46502685546875, 580.7138671875, -81.61656188964844, 0.0, 0.9307457804679871, 0.0, 0.3656669855117798}, //All Cockpits
			vector<double>{-444.6343994140625, 580.789306640625, -81.53125762939453, 0.0, -0.39714813232421875, 0.0, 0.9177545309066772}, //All Cockpits
			vector<double>{-549.9588623046875, 631.88330078125, 15.426615715026855, 0.0, -0.07584899663925171, 0.0, 0.9971193075180054}, //All Engines
			vector<double>{-432.2442932128906, 590.74072265625, -58.52851104736328, 0.0, 0.6870933771133423, 0.0, 0.7265691161155701}, //All Cockpits
			vector<double>{-224.9767608642578, 580.87158203125, -137.7026824951172, 0.0, -0.09497714787721634, 0.0, 0.9954794645309448}, //All Cockpits
			vector<double>{-214.3241729736328, 580.82373046875, -141.78369140625, 0.0, 0.9793984889984131, 0.0, -0.20193716883659363}, //All Nose Cones
			vector<double>{-557.6499633789062, 631.88330078125, -64.70059204101562, 0.0, 0.8870126008987427, 0.0, 0.4617452323436737}, //All Engines
			vector<double>{-187.525390625, 580.7420043945312, -68.24020385742188, 0.0, 0.9235464930534363, 0.0, 0.38348668813705444}, //All Nose Cones
		};
		for (int i = 0; i < RandomRocketBoxSpawnerPositionsList.size(); i++)
		{
			vector<double> cord = RandomRocketBoxSpawnerPositionsList.at(i);
			if (cord.size() > 0) {
				double x = cord.at(0);
				double y = cord.at(1);
				double z = cord.at(2);
				double rX = cord.at(3);
				double rY = cord.at(4);
				double rZ = cord.at(5);
				double rotW = cord.at(6);
				std::vector<unsigned long> RandomRocketBoxTypeLOTList = {
					12444,
					12445,
					12446
				};
				int choice = (1 + (rand() % (int)(RandomRocketBoxTypeLOTList.size() - 1 + 1))) - 1;
				GenericObject * smashable = new GenericObject(RandomRocketBoxTypeLOTList.at(choice), 1000, COMPONENT1_POSITION(x, y, z), COMPONENT1_ROTATION(rX, rY, rZ, rotW), COMPONENT1_VELOCITY(), COMPONENT1_VELOCITY_ANGULAR());

				ObjectsManager::registerObject(smashable);
				//ObjectsManager::registerSmashable(smashable);
				ObjectsManager::create(smashable);
				Logger::log("WRLD", "SMSH", "Spawned RocketBox with Choice: " + std::to_string(choice));
			}
		}
	}

	
	// This will be used in the saving of packets below...
	while (LUNI_WRLD) {
		while (_kbhit()){
			unsigned char key = (unsigned  char) _getch();
			switch (key){
			case 8:
				//Backspace
				if (buffer.size() > 0){
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

				if (str != ""){
					std::cout << std::endl;
					if (str == "quit"){
						LUNI_WRLD = false;
					}
					else{
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
				if (!buffer_started){
					std::cout << "> ";
					buffer_started = true;
					Logger::mute();
				}

				//Whitelist for chars
				if ((48 <= key && key <= 57) || (97 <= key && key <= 122) || (65 <= key && key <= 90) || key == 32){
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
		packet = rakServer->Receive(); // Recieve the packets from the server
		if (packet == NULL) continue; // If packet is NULL, just continue without processing anything









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
				if (session.phase > SessionPhase::PHASE_NONE){
					parsePacket(rakServer, packet->systemAddress, pdata, (unsigned long)(packet->length - 1));
				}
				else{
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
				if (session.phase >= SessionPhase::PHASE_AUTHENTIFIED){
					//auto usr = OnlineUsers->Find(packet->systemAddress);
					if (session.phase >= SessionPhase::PHASE_PLAYING){
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
		rakServer->DeallocatePacket(packet);
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
	rakServer->Shutdown(0);
	RakNetworkFactory::DestroyRakPeerInterface(rakServer);


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
			bool senderIsMythran = false;
			if (SiM == 1){
				senderIsMythran = true;
			}
			std::wstring reciever = PacketTools::ReadFromPacket(data, 33);
			unsigned char RiM;
			data->Read(RiM);
			bool recieverIsMythran = false;
			if (RiM == 1){
				recieverIsMythran = true;
			}
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
			//Friends::sendFriendsList(usr->GetCurrentCharacter()->charobjid);
			//Friends::broadcastFriendLogin(usr->GetCurrentCharacter()->charobjid);
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
			Logger::log("WRLD", "PARSER", "Game Message, ID: " + std::to_string(msgid), LOG_DEBUG);
			Logger::log("WRLD", "PARSER", "OBJECT-ID: " + std::to_string(objid), LOG_DEBUG);

			GameMSG::parseGameMSG(msgid, data, systemAddress);


			/*switch (msgid) {
			case PLAY_EMOTE: {
				SessionInfo s = SessionsTable::getClientSession(systemAdress);

				unsigned short emoteid;
				data->Read(emoteid);
				unsigned long long target;
				data->Read(target);

				Logger::log("WRLD", "SPEEDCHAT", "TargetID: " + std::to_string(target));

				unsigned long left = ((data->GetNumberOfUnreadBits() - 1) >> 3) + 1;
				Logger::log("WRLD", "SPEEDCHAT", "Bytes left in stream: " + std::to_string(left));

				std::wstring animationid = CDClientDB::getAnimationOfEmote(emoteid);
				if (animationid != L"INVALID")
					GameMSG::playAnimation(s.activeCharId, animationid);

				printData = true;
				break;
			}
			case START_SKILL: {
				printData = true;
				break;
			}
			case SELECT_SKILL: {
				printData = true;
				break;
			}
			case MOVE_ITEM_IN_INVENTORY: {
				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				//When you move on item on top of another item, then the two change their place
				bool flag;
				data->Read(flag);
				long long objid;
				data->Read(objid);
				long long unknown;
				data->Read(unknown);
				unsigned long slot;
				data->Read(slot);

				Logger::log("WRLD", "INVENTORY", "Move obj[" + std::to_string(objid) + "] to slot '" + std::to_string(slot) + "' (" + std::to_string(unknown) + "|" + std::to_string(flag), LOG_DEBUG);
				InventoryTable::moveItemToSlot(objid, s.activeCharId, slot);

				bool end;
				for (int k = 0; k < 7; k++){
					data->Read(end);
				}
				break;
			}
			case REMOVE_ITEM_FROM_INVENTORY: {
				bool mode;
				data->Read(mode);
				unsigned long uk1;
				data->Read(uk1);
				unsigned long long uk2;
				data->Read(uk2);
				bool a1, a2, a3, a4, a5, a6, a7;
				data->Read(a1);
				data->Read(a2);
				data->Read(a3);
				data->Read(a4);
				data->Read(a5);
				data->Read(a6);
				data->Read(a7);
				long long itemid;
				data->Read(itemid);
				unsigned char uk4;
				data->Read(uk4);
				unsigned long uk5;
				data->Read(uk5);
				if (mode == true){
					Logger::log("WLRD", "INVENTORY", "Delete item " + std::to_string(objid) + " from character " + std::to_string(objid));
					InventoryTable::deleteItem(objid, itemid);
					ObjectsTable::deleteObject(itemid);
				}
				break;
			}
			case EQUIP_INVENTORY: {

				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				//Equip an item
				bool flag;
				data->Read(flag);
				bool flag2;
				data->Read(flag2);
				long long objid;
				data->Read(objid);
				Logger::log("WORLD", "EQUIP", "Equip obj [" + std::to_string(objid) + "]", LOG_ALL);
				bool end;
				for (int k = 0; k < 7; k++){
					data->Read(end);
				}

				PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s.activeCharId);
				if (player != NULL){
					long lot = player->getComponent17()->equipItem(objid);

					long long test = EquipmentTable::getFromItemType(s.activeCharId, CDClientDB::getItemType(CDClientDB::getComponentID(lot, 11)));
					if (test != -1) {
						bool un = player->getComponent17()->unequipItem(test);
						if (!un){
							Logger::log("WRLD", "EQUIP", "ERROR: item not found", LOG_ERROR);
						}
						else {
							EquipmentTable::unequipItem(s.activeCharId, test);
							unsigned long LOT = ObjectsTable::getTemplateOfItem(test);

							unsigned long skillID = CDClientDB::getSkillID(LOT, 0);
							if (LOT == LOT::LOT_SLITHERSTRIKER ||
								LOT == LOT::LOT_NIGHTLASHER ||
								LOT == LOT::LOT_ENERGY_SPORK ||
								LOT == LOT::LOT_ZAPZAPPER)
								skillID = 148;
							if (skillID != -1)
								GameMSG::removeSkill(s.activeCharId, skillID);

							if (LOT == LOT::LOT_JETPACK || LOT == LOT::LOT_PROPERTY_JETPACK) {
								std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
								for (unsigned int k = 0; k < sessionsz.size(); k++){
									WorldServerPackets::SendGameMessage(sessionsz.at(k).addr, s.activeCharId, 561);
								}
							}
						}
					}

					unsigned long itemType = CDClientDB::getItemType(CDClientDB::getComponentID(lot, 11));
					EquipmentTable::equipItem(s.activeCharId, objid, itemType);

					unsigned long hotbarslot = 4;
					if (itemType == ItemType::HAIR || ItemType::HAT)
						hotbarslot = 3;
					if (itemType == ItemType::NECK)
						hotbarslot = 2;
					if (itemType == ItemType::RIGHT_HAND)
						hotbarslot = 0;
					if (itemType == ItemType::LEFT_HAND)
						hotbarslot = 1;

					unsigned long skillid = CDClientDB::getSkillID(lot, 0);
					if (lot == LOT::LOT_SLITHERSTRIKER ||
						lot == LOT::LOT_NIGHTLASHER ||
						lot == LOT::LOT_ENERGY_SPORK ||
						lot == LOT::LOT_ZAPZAPPER)
						skillid = 148;
					if (skillid != -1)
						GameMSG::addSkill(s.activeCharId, skillid, hotbarslot);

					if (lot == LOT::LOT_JETPACK) {
						RakNet::BitStream *pc = WorldServerPackets::InitGameMessage(s.activeCharId, 561);

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

						std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
						for (unsigned int k = 0; k < sessionsz.size(); k++){
							WorldServer::sendPacket(pc, sessionsz.at(k).addr);
						}
					}

					ObjectsManager::serialize(player);
				}
				break;
			}
			
			case RESOND_TO_MISSION: {
				break;
			}
			case MISSION_DIALOGUE_CANCELLED: {
				break;
			} 
			case UNEQUIP_INVENTORY: {
				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				bool flag;
				data->Read(flag);
				bool flag2;
				data->Read(flag2);
				bool flag3;
				data->Read(flag3);
				long long objid;
				data->Read(objid);
				Logger::log("WRLD", "EQUIP", "Unequip object [" + std::to_string(objid) + "]", LOG_ALL);
				bool end;
				for (int k = 0; k < 5; k++){
					data->Read(end);
				}
				PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s.activeCharId);
				if (player != NULL){
					bool un = player->getComponent17()->unequipItem(objid);
					if (!un){
						Logger::log("WRLD", "EQUIP", "ERROR: item not found", LOG_ERROR);
					}
					else{
						EquipmentTable::unequipItem(s.activeCharId, objid);
						unsigned long LOT = ObjectsTable::getTemplateOfItem(objid);

						unsigned long skillID = CDClientDB::getSkillID(LOT, 0);
						if (LOT == LOT::LOT_SLITHERSTRIKER ||
							LOT == LOT::LOT_NIGHTLASHER ||
							LOT == LOT::LOT_ENERGY_SPORK ||
							LOT == LOT::LOT_ZAPZAPPER)
							skillID = 148;
						if (skillID != -1)
							GameMSG::removeSkill(s.activeCharId, skillID);

						if (LOT == LOT::LOT_JETPACK || LOT == LOT::LOT_PROPERTY_JETPACK) {
							std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
							for (unsigned int k = 0; k < sessionsz.size(); k++){
								WorldServerPackets::SendGameMessage(sessionsz.at(k).addr, s.activeCharId, 561);
							}
						}

						ObjectsManager::serialize(player);
					}
				}
				break;
			}
				
			case REQUEST_USE: {
				//Interaction
				//There has to be a flag somewhere in here, with the speedchat messages I found out that it is offset by one bit.
				//Normally all bits until there are 0, i'll check both sides to see if anything happens -> where the flag is
				bool flag1;
				data->Read(flag1);
				data->SetReadOffset(data->GetReadOffset() - 1);
				unsigned long long something3;
				data->Read(something3);
				bool flag;
				data->Read(flag);
				unsigned long long object;
				data->Read(object);

				ObjectInformation o = getObjectInformation(object);
				Logger::log("WRLD", "INTERACT", getObjectDescription(o, object));

				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s.activeCharId);

				bool b;
				for (unsigned char a = 0; a < 7; a++) data->Read(b);
				if (o.type == ObjectType::NONE){
					//logic here needs to be redone
				}
				else{
					handleObject(o, rakServer, systemAddress);
				}
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
			}
			case PLAYER_LOADED:
			{
				//happens on client ready
				long long object;
				data->Read(object);
				//cout << "Object: " << object << std::endl;
				ObjectInformation o = getObjectInformation(object);
				Logger::log("WRLD", "LOAD?", getObjectDescription(o, object));
				//Some sort of loading, L8: objid

				RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(object, 1043);
				ef->Write((bool)false);
				ef->Write((bool)true);
				ef->Write((bool)false);
				WorldServer::sendPacket(ef, systemAddress);

				RakNet::BitStream * ef2 = WorldServerPackets::InitGameMessage(object, 0x021a);
				ef2->Write((bool)false);
				WorldServer::sendPacket(ef2, systemAddress);

			}
				break;
			case MESSAGE_BOX_RESPOND:
			{
				//Some sort of script
				//e.g. Closing a plaque
				unsigned long something;
				data->Read(something);
				unsigned long len;
				data->Read(len);
				std::vector<wchar_t> mV;
				mV.reserve(len);
				for (unsigned long k = 0; k < len; k++){
					wchar_t mC;
					data->Read(mC);
					mV.push_back(mC);
				}
				std::wstring script(mV.begin(), mV.end());
				unsigned long something2;
				data->Read(something2);
				Logger::log("WRLD", "SCRIPT", "530: " + UtfConverter::ToUtf8(script));
				//if (something > 0) cout << " [1:" << something << "]";
				//if (something2 > 0) cout << " [2:" << something2 << "]";
				//cout << endl;
			}
				break;
			case USE_NON_EQUIPMENT_ITEM:
			{
				//Open Bag
				long long aobjid;
				data->Read(aobjid);
				Logger::log("WRLD", "GMSG", "Open object " + std::to_string(aobjid));
			}
				break;
			case QUERY_PROPERTY_DATA:
			{
				//This happens independant of the Zone you are in, and these are NOT the object IDs of the characters equipment.
				//It is still probably some sort of registering of Objects
				Logger::log("WRLD", "ZONELOAD", "Registering Object???", LOG_ALL);
				break;
			}
				
			case FIRE_EVENT_SERVER_SIDE:
			{
				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				
				//Some sort of script
				//e.g. Closing a postbox
				//For this message, the objid is the ID of the postbox in question

				//Ok actually, this is called from within the LUA script for the post box
				//in L_MAIL_BOX_INTERACT.lua, with the postbox as game message object and the user later on
				//this means, that this here is the server endpoint for the lua function:
				//object:fireEventServerSide('text', object)
				//Which is exactly what we recieve here.

				unsigned long len;
				data->Read(len);
				std::vector<wchar_t> mV;
				mV.reserve(len);
				for (unsigned long k = 0; k < len; k++){
					wchar_t mC;
					data->Read(mC);
					mV.push_back(mC);
				}
				std::wstring script(mV.begin(), mV.end());
				Logger::log("WRLD", "SCRIPT", "770: " + UtfConverter::ToUtf8(script), LOG_DEBUG);
				bool f;
				for (unsigned char k = 0; k < 3; k++){
					data->Read(f);
				}

				unsigned long long object;
				data->Read(object);
				ObjectInformation o = getObjectInformation(object);
				Logger::log("WRLD", "SCRIPT", "Object: " + getObjectDescription(o, object), LOG_DEBUG);

				bool dat;
				for (unsigned char i = 0; i < 5; i++){
					data->Read(dat);
				}

				if (script == L"toggleMail"){
					Mail::closeMailbox(s.activeCharId);
				}
			}
				break;
			case TOGGLE_GHOST_REFERENCE_OVERRIDE:
				bool isStart;
				data->Read(isStart);
				bool dat;
				for (unsigned char i = 0; i < 7; i++){
					data->Read(dat);
				}
				if (isStart) Logger::log("WRLD", "PARSER", "Camerapath started"); else Logger::log("WRLD", "PARSER", "Camerapath finished");
				break;
			case SET_GHOST_REFERENCE_POSITION:
			{
				//Camera moving
				REPLICA_OBJECT_POSITION pos;
				data->Read(pos);
				std::stringstream str;
				str << "Camera-Position: (X:" << pos.posX << "|Y:" << pos.posY << "|Z:" << pos.posZ << ")";
				Logger::log("WRLD", "PARSER", str.str());
			}
				break;
			case PARSE_CHAT_MESSAGE: //Chat messages and commands, starting with '/'
			{
				unsigned long unknown;
				data->Read(unknown);
				unsigned long len;
				data->Read(len);
				Logger::log("WRLD", "CHAT", "Recieved Packet of Length " + std::to_string(len), LOG_ALL);
				vector<wchar_t> msgVector;
				msgVector.reserve(len);
				for (unsigned long k = 0; k < len; k++){
					wchar_t mchr;
					data->Read(mchr);
					msgVector.push_back(mchr);
				}

				std::wstring message(msgVector.begin(), msgVector.end());
				Logger::log("WRLD", "CHAT", "Recieved message: \"" + UtfConverter::ToUtf8(message) + "\"");

				if (message.substr(0, 1) == L"/"){
					std::wstring command;
					std::vector<std::wstring> params;
					unsigned char state = 0;

					for (unsigned int k = 0; k < message.length(); k++){
						wchar_t chr = message.at(k);
						if (state == 0){
							//Nothing is parsed yet
							if (chr == L'/'){ //Needs to start with '/'
								state = 1; //Set state to STATE_COMMAND
							}else{
								break;
							}
						}
						else if (state == 1){
							//We are parsing the command name
							if (chr == L' '){
								state = 2;
							}else{
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
					Logger::log("WRLD", "CHAT", "Command: " + UtfConverter::ToUtf8(command));

					//Packet 1: 2365 (ZoneControl Object)
					//Packet 2: objid: 288300744895889662 LOT: 6635 SpawnerId: 72567767517768 (Mardolf the Orange)
					//Packet 3: objid: 288300744895889664 LOT: 6636 SpawnerId: 72567767517769 (Rad Eccles)
					//Packet 4: objid: 288300744895889669 LOT: 3495 SpawnerId: 72567767517772
					//Packet 5: objid: 288300744895899827 LOT: 6726 SpawnerId: 72567767517771
					//Packet 6: objid: 288300744895909034 LOT: 5637 SpawnerId: 3695535 NodeID: 1, Starts with Index 20 (position), x:-309.645782, y:288.356626, z: 70.64473 (Robot Dog Pet)
					//Packet 7: objid: 288300744895889606 LOT: 3646 SpawnerId: 3695538 NodeId: 3, 
					//Packet 8: objid: 288300744895930870 LOT: 9717 SpawnerId: 72567767517763,
					//Packet 9: PLAYER

					SessionInfo s = SessionsTable::getClientSession(systemAddress);
					ChatCommandManager::handleCommand(command, &s, &params);
				}
			}
				break;
			case READY_FOR_UPDATES:
			{
				long long object;
				data->Read(object);
				ObjectInformation o = getObjectInformation(object);
				Logger::log("WRLD", "OBJECT", getObjectDescription(o, object));
				//Some sort of loading, L8: objid
			}
				break;
			case SYNC_SKILL: //fighting stuff
			{
				bool flag;
				data->Read(flag);
				unsigned long d1;
				data->Read(d1);
				Logger::log("WRLD", "COMBAT", std::to_string(flag) + "|" + std::to_string(d1));
				PacketTools::printBytes(data, d1);
				unsigned long num1;
				data->Read(num1);
				unsigned long num2;
				data->Read(num2);
				Logger::log("WRLD", "COMBAT", std::to_string(num1) + "|" + std::to_string(num2));
			}
				break;
			case 1202: //when the player presses the smash button in the menu
			{
				//we need to smash the player here
				
				SessionInfo *s = &SessionsTable::getClientSession(systemAddress);

				RakNet::BitStream * ef2 = WorldServerPackets::InitGameMessage(s->activeCharId, DIE);
				ef2->Write((bool)false);
				ef2->Write((bool)false);
				ef2->Write((float)-1);
				ef2->Write((long)0);
				ef2->Write((float)0);
				ef2->Write((float)0);
				ef2->Write((float)0);
				ef2->Write((bool)false);
				ef2->Write((unsigned long long)s->activeCharId);
				ef2->Write((unsigned long long)s->activeCharId);
				WorldServer::sendPacket(ef2, systemAddress);


				Logger::log("WRLD", "HELP", "Teleported back to where they started!", LOG_DEBUG);
			}
				break;
			case REQUEST_RESURRECT: //moved
			{
				SessionInfo *s = &SessionsTable::getClientSession(systemAddress);
				RakNet::BitStream * ef2 = WorldServerPackets::InitGameMessage(s->activeCharId, 0x00a0);
				ef2->Write((bool)false);
				WorldServer::sendPacket(ef2, systemAddress);
			}
			break;
			case 1308: //minigames
			{
				//This handles the requirements of a minigame for the server side.
				//Starting with 8 bytes of objectId?, then 4 bytes for the data length
				//Then that with a null byte terminator?
				//Then two 4 byte values
				unsigned long long object;
				data->Read(object);

				Logger::log("WRLD", "MINIGAME", "Cost?: objid: " + std::to_string(objid), LOG_DEBUG);

				unsigned long len;
				data->Read(len);

				if (len > 0){
					std::vector<wchar_t> mV;
					mV.reserve(len);
					for (unsigned long k = 0; k < len; k++){
						wchar_t mC;
						data->Read(mC);
						mV.push_back(mC);
					}
					std::wstring script(mV.begin(), mV.end());
					//I guess this is a null terminator as it only appears with text content, but not included in length
					unsigned short nullT;
					data->Read(nullT);
					Logger::log("WRLD", "MINIGAME", "Data: " + UtfConverter::ToUtf8(script));
				}
				//These 8 bytes should be two values, since the only thing I found so far has content only in the 5th
				unsigned long dataA;
				unsigned long dataB;
				data->Read(dataA);
				data->Read(dataB);
				Logger::log("WRLD", "MINIGAME", "A: " + std::to_string(dataA) + ", B: " + std::to_string(dataB), LOG_DEBUG);
			}
				break;
			case USED_INFORMATION_PLAQUE:
			{
				unsigned long long object;
				data->Read(object);
				Logger::log("WLRD", "UNKNOWN", "Objectid: " + std::to_string(object));
				break;
			}
			case 1734: {
				//when you level up
				//Level UP

				SessionInfo s = SessionsTable::getClientSession(systemAddress);
				ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s.activeCharId);

				Database::Query("UPDATE `characters` SET `level` = '" + std::to_string(cinfo.info.level + 1) + "' WHERE `objectID` = '" + std::to_string(s.activeCharId) + "';");
				std::vector<SessionInfo> wsessions = SessionsTable::getClientsInWorld(s.zone);
				std::wstring name;
				name.assign(cinfo.info.name.begin(), cinfo.info.name.end());
				for (unsigned int i = 0; i < wsessions.size(); i++){
					Chat::sendChatMessage(wsessions.at(i).activeCharId, name + L" is now at level "+std::to_wstring(cinfo.info.level+1), L"System", false);
				}

			}
				break;
			case 1198: //help menu reporting
			{
				printData = true;
				//I dont know what the first byte is, but everything after that is just ascii encoded
				unsigned long unknown;
				data->Read(unknown);
				char letter;
				bool test = true;
				std::string message = ""; 
				std::string otherInfo = "";

				char discard;
				while (data->Read(letter)){
					if (test){
						data->Read(discard);
						message += letter;
					}
					else{
						otherInfo += letter;
					}
					

					if (letter == 0x07) //if we have reached the end of the help string
					{
						test = false;
					}
				}


				Logger::log("WRLD", "HELP", "Recieved help message: " + message);
				Logger::log("WRLD", "HELP", "Help message info: " + otherInfo);

			}
				break;
			case PICKUP_ITEM: 
			{
				SessionInfo *s = &SessionsTable::getClientSession(systemAddress);
				SessionInfo s1 = SessionsTable::getClientSession(systemAddress);
				unsigned long long lootId;
				unsigned long long playerId;

				data->Read(lootId);
				data->Read(playerId);

				ReplicaObject * loot = ObjectsManager::getObjectByID(lootId);

				std::vector<MISSION_DATA> mis = MissionsTable::getMissions(s1.activeCharId);
				if (lootId == 935 && mis[0].missionid == 173 && mis[0].missionStatus < 4){
					mis[0].missioncount += 1;
					Database::Query("UPDATE `missions` SET `count` = '" + std::to_string(mis[0].missioncount) + "' WHERE `character` = '" + std::to_string(s1.activeCharId) + "' AND `missionId` = '173'");
					RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(s1.activeCharId, 851);
					ef->Write((unsigned long)0);
					ef->Write((std::string)"");
					ef->Write((std::string)"");
					WorldServer::sendPacket(ef, s1.addr);
				}

				unsigned long slot = -1;
				for (int i = 0; (slot == -1) && (i != 24); i++){
					if (InventoryTable::getItemFromSlot(playerId, i) == -1)
						slot = i;
				}

				if (slot == -1){
					//no room
				}
				else{

					InventoryTable::insertItem(s->activeCharId, loot->LOT, 1, slot, false);

					RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(s->activeCharId, 227);
					ef->Write((bool)false);
					ef->Write((bool)false);
					ef->Write((bool)true);
					ef->Write((bool)false);
					ef->Write((unsigned long)0);
					ef->Write((unsigned long)loot->LOT);
					ef->Write((bool)false);
					ef->Write((bool)false);
					ef->Write((bool)true);
					ef->Write((unsigned long)1);
					ef->Write((bool)true);
					ef->Write((unsigned long)0);
					ef->Write(lootId);
					ef->Write((float)0);
					ef->Write((float)0);
					ef->Write((float)0);
					ef->Write((bool)true);
					ef->Write((unsigned long)slot);

					WorldServer::sendPacket(ef, s->addr);
				}
			}
			break;
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
			data->SetReadOffset(data->GetReadOffset() + 24); //3 bytes channel?
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
				COMPONENT1_ROTATION rot;
				ZoneId zid = static_cast<ZoneId>(za.zoneid);
				if (za.zoneid == cinfo.lastPlace.zoneID){
					pos = COMPONENT1_POSITION(cinfo.lastPlace.x, cinfo.lastPlace.y, cinfo.lastPlace.z);
					rot = COMPONENT1_ROTATION(cinfo.lastPlace.rotX, cinfo.lastPlace.rotY, cinfo.lastPlace.rotZ, cinfo.lastPlace.rotW);
					if (pos.isNull()) pos = getZoneSpawnPoint(zid);
				}
				else{
					pos = getZoneSpawnPoint(zid);
					rot = COMPONENT1_ROTATION();
				}
				if (za.zoneid == 1000){
					pos = COMPONENT1_POSITION(-625.303, 613.414, -25.6505);
					rot = COMPONENT1_ROTATION();
				}

				WorldServerPackets::CreateCharacter(systemAddress, objid);

				PlayerObject * player = new PlayerObject(objid, UtfConverter::FromUtf8(cinfo.info.name));

				//Temporarily ?
				player->gmlevel = (unsigned char)cinfo.info.gmlevel;
				player->world.zone = zid;

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
				c7->getData4_1Ref()->push_back(50);
				c7->setData4_4_1(true);
				c7->setData5(false);

				InventoryComponent * c17 = player->getComponent17();
				std::vector<long long> equip = EquipmentTable::getItems(objid);
				for (unsigned int k = 0; k < equip.size(); k++){
					unsigned long equipLOT = c17->equipItem(equip.at(k));

					LootObject *lootObj = new LootObject(equipLOT, equip.at(k));
					ObjectsManager::registerObject(lootObj);
				}

				//usr->SetPlayer(player);

				ObjectsManager::clientJoinWorld(player, systemAddress);
				//player->doCreation(packet->systemAddress, zid);

				WorldServerPackets::SendGameMessage(systemAddress, objid, 1642);
				WorldServerPackets::SendGameMessage(systemAddress, objid, 509);

				RakNet::BitStream *pc = WorldServerPackets::InitGameMessage(objid, 472);
				pc->Write((unsigned long)185);
				pc->Write((unsigned char)0);
				WorldServer::sendPacket(pc, systemAddress);

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
						if (pos.y < -105) {
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
			return false;
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