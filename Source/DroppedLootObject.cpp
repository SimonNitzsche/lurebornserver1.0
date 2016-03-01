#include "Database.h"
#include "DroppedLootObject.h"
#include "Replica.h"
#include "World.h"
#include "WorldServer.h"
#include "Worlds.h"
#include "LootObject.h"
#include "PlayerObject.h"
#include "AccountsDB.h"
#include "WorldConnection.h"

#include "ObjectID.h"

#include "RakNet\ReplicaManager.h"

#include "Logger.h"
#include "UtfConverter.h"

// SQLite
#include "SQLite\sqdb.h"
#include "SQLiteDatabase.h"

#include <map>

using namespace std;

DroppedLootObject::DroppedLootObject(unsigned long lot, unsigned long currentZone, SessionInfo *s) {
	Logger::log("REPL", "LOOT", "Initializing Loot");

	this->zoneID = currentZone;
	initializeDroppedLootObject(lot, s);
}

DroppedLootObject::~DroppedLootObject() {

}

void DroppedLootObject::initializeDroppedLootObject(unsigned long lot, SessionInfo *s) {
	this->objid = ObjectID::generateObjectID();
	this->LOT = lot;

	std::vector<SessionInfo> wsessions = SessionsTable::getClientsInWorld(s->zone);
	for (unsigned int i = 0; i < wsessions.size(); i++){
		RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(wsessions.at(i).activeCharId, 30);
		PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(wsessions.at(i).activeCharId);
		SystemAddress addr = SessionsTable::findCharacter(wsessions.at(i).activeCharId);

		ef->Write((bool)true);

		ef->Write((bool)true);
		ef->Write((float)player->getComponent1()->getPosition().x);
		ef->Write((float)player->getComponent1()->getPosition().y);
		ef->Write((float)player->getComponent1()->getPosition().z);
		ef->Write((long)0);
		ef->Write((unsigned long)lot);
		ef->Write((unsigned long long)this->objid);
		ef->Write((unsigned long long)wsessions.at(i).activeCharId);
		ef->Write((unsigned long long)wsessions.at(i).activeCharId);
		ef->Write((bool)true);
		ef->Write((float)player->getComponent1()->getPosition().x);
		ef->Write((float)player->getComponent1()->getPosition().y+10);
		ef->Write((float)player->getComponent1()->getPosition().z);

		WorldServer::sendPacket(ef, addr);
	}
}