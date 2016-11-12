#include "WorldConnection.h"

#include "legoPackets.h"
#include "Packet.h"
#include "Logger.h"
#include "UtfConverter.h"
#include "AccountsDB.h"
#include "InventoryDB.h"
#include "WorldServer.h"
#include "LDF.h"

void ClientWorldPackets::HandlePacket(RakNet::BitStream * data, SystemAddress sender){
	unsigned long packetID;
	data->Read(packetID);
	unsigned char padding;
	data->Read(padding);
	switch (packetID){
	case WorldPacketID::CLIENT_VALIDATION:
		bool flag = ClientWorldPackets::HandleValidation(data, sender);
		break;
	}
}

bool ClientWorldPackets::HandleValidation(RakNet::BitStream * data, SystemAddress sender){
	std::wstring name = PacketTools::ReadFromPacket(data, 33);
	std::wstring key = PacketTools::ReadFromPacket(data, 33);
	std::string u_hash = PacketTools::ReadStrFromPacket(data, 32);
	unsigned char flag;
	data->Read(flag);
	Logger::log("WRLD", "VALID", "Client Validation Request", LOG_DEBUG);
	Logger::log("WRLD", "VALID", "Name : " + UtfConverter::ToUtf8(name), LOG_DEBUG);
	Logger::log("WRLD", "VALID", "Key  : " + UtfConverter::ToUtf8(key), LOG_ALL);
	Logger::log("WRLD", "VALID", "Hash?: " + u_hash, LOG_ALL);
	if (flag == 0) Logger::log("WRLD", "VALID", "Flag false", LOG_ALL);
	else Logger::log("WRLD", "VALID", "Flag true", LOG_ALL);

	SessionInfo s = SessionsTable::getClientSession(sender);
	if (s.sessionkey == UtfConverter::ToUtf8(key)){
		Logger::log("WRLD", "VALID", "Validation successful", LOG_DEBUG); //Session keys match
		return true;
	}
	else{
		Logger::log("WRLD", "VALID", "Validation unsuccessful", LOG_DEBUG); //Session keys do not match
		return false;
	}
}

ZoneAddress ClientWorldPackets::HandleLevelLoadComplete(RakNet::BitStream * data, SystemAddress sender){
	ZoneAddress za;
	data->Read(za.zoneid);
	data->Read(za.instanceid);
	data->Read(za.cloneid);
	return za;
}

std::stringstream endstring;

void writeRaw(RakNet::BitStream *data, std::string str){
	endstring << str;
	for (unsigned long i = 0; i < str.size(); i++){
		data->Write(str.at(i));
	}
}

//53-05-00-04
void WorldServerPackets::CreateCharacter(SystemAddress address, long long charobjid){
	SessionInfo s = SessionsTable::getClientSession(address);
	//if (s.activeCharId != charobjid){
	//	Logger::log("WSP5", "04", "Session Character and Charobjid do not match", LOG_WARNING);
	//}
	ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(charobjid);

	RakNet::BitStream * packet = WorldServer::initPacket(RemoteConnection::CLIENT, WorldServerPacketIds::MSG_CLIENT_CREATE_CHARACTER);
	LDF *ldf = new LDF();
	ldf->writeS64(L"accountID", s.accountid);
	ldf->writeS32(L"chatmode", (cinfo.info.cloaked)?1:0); //0 - Normal, 1 - Super
	ldf->writeBOOL(L"editor_enabled", true);
	ldf->writeS32(L"editor_level", 0);
	ldf->writeBOOL(L"freetrial", false);
	ldf->writeS32(L"gmlevel", (cinfo.info.cloaked)?1:0); //0 - Normal, 1 - Mythran
	ldf->writeBOOL(L"legoclub", true);

	unsigned long long levelid = cinfo.lastPlace.zoneID + (((unsigned long long) cinfo.lastPlace.mapInstance) << 16) + (((unsigned long long) cinfo.lastPlace.mapClone) << 32);
	ldf->writeS64(L"levelid", levelid);

	//Try  e3  04    f4    74     95 51 08 20   if this breaks
	//    ( ZONE ) ( INSTANCE ) (    CLONE    ) ?

	std::wstring name = UtfConverter::FromUtf8(cinfo.info.name);

	ldf->writeWSTRING(L"name", name);
	ldf->writeID(L"objid", charobjid);
	ldf->writeFLOAT(L"position.x", cinfo.lastPlace.x);
	ldf->writeFLOAT(L"position.y", cinfo.lastPlace.y);
	ldf->writeFLOAT(L"position.z", cinfo.lastPlace.z);
	ldf->writeS64(L"reputation", 100); //Set reputation here
	ldf->writeS32(L"template", 1);

	RakNet::BitStream *xml = new RakNet::BitStream();
	writeRaw(xml, "<?xml version=\"1.0\"?>");
	writeRaw(xml, "<obj v=\"1\">");
	writeRaw(xml, "<buff/>");
	writeRaw(xml, "<skil/>");
	writeRaw(xml, "<inv>");
	writeRaw(xml, "<bag>");
	writeRaw(xml, "<b t=\"0\" m=\"" + std::to_string(cinfo.info.inventorySize.at(0)) + "\"/>");
	writeRaw(xml, "<b t=\"1\" m=\"" + std::to_string(cinfo.info.inventorySize.at(1)) + "\"/>");
	writeRaw(xml, "<b t=\"14\" m=\"" + std::to_string(cinfo.info.inventorySize.at(2)) + "\"/>");
	writeRaw(xml, "<b t=\"15\" m=\"" + std::to_string(cinfo.info.inventorySize.at(3)) + "\"/>");
	writeRaw(xml, "</bag>");
	writeRaw(xml, "<grps/>");
	writeRaw(xml, "<items>");
	//Adding items to the inventory:
	//l: LOT, template, type of item
	//id: objid, unique, this object
	//s: slot in inventory
	//b: present and set to "1" if object is linked to the player
	//c: amout of this item
	std::vector<InventoryItem> items = InventoryTable::getItems(charobjid);
	std::vector<std::vector<InventoryItem>> sorteditems = {{},{},{},{},{},{},{}};
	std::vector<int> tabi = { 0,1,2,5,7,8,12 };
	for (unsigned int k = 0; k < items.size(); k++){
		//long lot = ObjectsTable::getTemplateOfItem(items.at(k).objid);
		ObjectInfo oinfo = ObjectsTable::getItemInfo(items.at(k).objid);
		for (int ti = 0; ti < tabi.size(); ti++)
			if (tabi.at(ti) == items.at(k).tab)
				sorteditems.at(ti).push_back(items.at(k));
	}
	for (int sl = 0; sl < 7; sl++) {
		writeRaw(xml, "<in t=\""+std::to_string(tabi.at(sl))+"\">");
		for each (InventoryItem itm in sorteditems.at(sl)){
			ObjectInfo oinfo = ObjectsTable::getItemInfo(itm.objid);
			Logger::log("USER", "CHARDATA", "Adding item " + std::to_string(oinfo.lot) + "[" + std::to_string(oinfo.objid) + "] " + std::to_string(oinfo.spawnerid), LOG_ALL);
			if (oinfo.lot > -1) {
				std::stringstream adddata;
				adddata << "<i l=\"" << oinfo.lot << "\" id=\"" << itm.objid << "\" s=\"" << itm.slot << "\"";
				if (itm.qnt > 1) {
					adddata << " c=\"" << itm.qnt << "\"";
				}
				if (oinfo.spawnerid > -1) {
					adddata << " sk=\"" << oinfo.spawnerid << "\"";
				}
				if (itm.linked) {
					adddata << " b=\"1\"";
				}
				if (oinfo.spawnerid > -1) {
					

					ObjectInfo sinfo = ObjectsTable::getItemInfo(oinfo.spawnerid); //gnerates spawnerId from spawnerId <-- This should be objid but objid would result in a mess of bugs.
					if (sinfo.spawnerid == -1) {
						//Not an item for an object itself
						if (sinfo.lot == 6416) {
							adddata << ">";
							//This is a custom Rocket
							RocketInfo rinfo = ObjectsTable::getRocketInfo(sinfo.objid);
							if (rinfo.cockpit_template > 0 && rinfo.engine_template > 0 && rinfo.nose_cone_template > 0) {
								// NOTE: Who ever commented this out - don't forget, that you commented this.
								//adddata << "<x ma=\"0:1:" << rinfo.nose_cone_template << "+1:" << rinfo.cockpit_template << "+1:" << rinfo.engine_template << "\"/>";
								Logger::log("USER", "CHARDATA", "Adding Rocket");
							}
							adddata << "</i>";
						}
						else {
							adddata << "/>";
						}
					}
					else {
						adddata << "/>";
					}
				}
				else {
					adddata << "/>";
				}
				writeRaw(xml, adddata.str());
			}
		}
		writeRaw(xml, "</in>");
	}
	writeRaw(xml, "</items>");
	writeRaw(xml, "</inv>");
	writeRaw(xml, "<mf/>");
	writeRaw(xml, "<char cc=\"");
	writeRaw(xml, std::to_string(cinfo.info.currency)+"\"");
	writeRaw(xml, " gm=\""+std::to_string(cinfo.info.cloaked?1:0)+"\"");
	writeRaw(xml, ">");
	writeRaw(xml, "</char>");
	std::stringstream adddata;
	adddata << "<lvl";
	adddata << " l=\""+std::to_string(cinfo.info.level)+"\"";
	adddata << "/>";
	adddata << "<flag>";
	adddata << "<f id=\"2\" v=\"1\"/>";
	writeRaw(xml, adddata.str());
	writeRaw(xml, "</flag>");
	writeRaw(xml, "<pet/>");
	std::vector<MISSION_DATA> missions = MissionsTable::getMissions(charobjid);
	if (missions.size() > 0){
		writeRaw(xml, "<mis>");
		writeRaw(xml, "<cur>");
		for (unsigned int k = 0; k < missions.size(); k++){
			if (missions[k].missionStatus <= 3){
				std::stringstream adddata;
				adddata << "<m";
				adddata << " id=\"" << std::to_string(missions.at(k).missionid) << "\">";
				adddata << "<sv v=\"" << std::to_string(missions.at(k).missioncount) << "\"/>";
				adddata << "</m>";
				writeRaw(xml, adddata.str());
			}
		}
		writeRaw(xml, "</cur>");
		writeRaw(xml, "<done>");
		for (unsigned int k = 0; k < missions.size(); k++){
			if (missions[k].missionStatus >= 4){
				std::stringstream adddata;
				adddata << "<m";
				adddata << " id=\"" << std::to_string(missions.at(k).missionid) << "\"";
				adddata << " cts=\"" << std::to_string(missions.at(k).timestamp) << "\"";
				adddata << " cct=\"" << std::to_string(missions.at(k).missioncount) << "\"";
				adddata << "/>";
				writeRaw(xml, adddata.str());
			}
		}
		writeRaw(xml, "</done>");
		writeRaw(xml, "</mis>");
	}
	else{
		writeRaw(xml, "<mis/>");
	}
	writeRaw(xml, "<mnt/>");
	writeRaw(xml, "<dest hm=\"9\" hc=\"9\" im=\"23\" ic=\"33\" am=\"0\" ac=\"14\" rsh=\"4\" rsi=\"6\" d=\"0\" imm=\"0\"/>");
	writeRaw(xml, "</obj>");

	/*std::ofstream mf;
	mf.open("cl.xml");
	mf << endstring.str();
	mf.close();*/

	ldf->writeBYTES(L"xmlData", xml);

	packet->Write((unsigned long)(ldf->getSize() + 1));
	packet->Write((unsigned char)0);
	ldf->writeToPacket(packet);
	WorldServer::sendPacket(packet, address);
}

void WorldServerPackets::SendGameMessage(SystemAddress address, long long charobjid, unsigned short gmsgid){
	WorldServer::sendPacket(WorldServerPackets::InitGameMessage(charobjid, gmsgid), address);
}

RakNet::BitStream *WorldServerPackets::InitGameMessage(long long charobjid, unsigned short gmsgid){
	RakNet::BitStream *packet = WorldServer::initPacket(RemoteConnection::CLIENT, WorldServerPacketIds::MSG_CLIENT_GAME_MSG);
	packet->Write(charobjid);
	packet->Write(gmsgid);
	return packet;
}