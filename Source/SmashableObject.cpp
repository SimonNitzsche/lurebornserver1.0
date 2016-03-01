#include "Database.h"
#include "SmashableObject.h"
#include "Replica.h"
#include "World.h"
#include "WorldServer.h"
#include "Worlds.h"
#include "LootObject.h"
#include "PlayerObject.h"
#include "AccountsDB.h"

#include "ObjectID.h"

#include "RakNet\ReplicaManager.h"

#include "Logger.h"
#include "UtfConverter.h"

// SQLite
#include "SQLite\sqdb.h"
#include "SQLiteDatabase.h"

#include <map>

using namespace std;

SmashableObject::SmashableObject(unsigned long lot, unsigned long currentZone) {
	Logger::log("REPL", "SMSH", "Initializing Smashable!");

	this->zoneID = currentZone;
	initializeSmashable(lot);
}

SmashableObject::~SmashableObject() {

}

void SmashableObject::initializeSmashable(unsigned long lot) {
	this->objid = ObjectID::generateObjectID();
	this->LOT = lot;

	SpawnerObject *spawner = new SpawnerObject(ObjectID::generateSpawnerID());
	this->spawnerObject = spawner;
	this->spawnerObjID = spawner->objid;
	this->spawner_node_id = 0;

	this->addComponent(new SimplePhysicsComponent());




	this->addComponent(new DestructibleComponent());

	this->world.zone = this->zoneID;

	COMPONENT7_DATA3 d3 = COMPONENT7_DATA3();
	d3.d1 = 0; d3.d2 = 0; d3.d3 = 0; d3.d4 = 0; d3.d5 = 0; d3.d6 = 0; d3.d7 = 0; d3.d8 = 0; d3.d9 = 0;

	DestructibleComponent *c7 = this->getDestructibleComponent();

	c7->setData3(d3);


	stringstream destructibleStream;
	destructibleStream << "SELECT `component_id` FROM `ComponentsRegistry` WHERE `id` = " << lot << " AND `component_type` = " << 7 << ";";

	sqdb::Statement destructibleStatement = SQLiteDatabase::Query("cdclient.sqlite", destructibleStream.str());
	while (destructibleStatement.Next()) {
		long component_id = destructibleStatement.GetField(0).GetInt();

		stringstream destructibleDataStream;
		destructibleDataStream << "SELECT `faction`, `life`, `imagination`, `armor` FROM `DestructibleComponent` WHERE `id` = " << component_id << ";";

		sqdb::Statement dataStatement = SQLiteDatabase::Query("cdclient.sqlite", destructibleDataStream.str());
		while (dataStatement.Next()) {
			long factionID = dataStatement.GetField(0).GetInt();
			long health = dataStatement.GetField(1).GetInt();
			long imagination = dataStatement.GetField(2).GetInt();
			long armor = dataStatement.GetField(3).GetInt();

			COMPONENT7_DATA4 d4 = COMPONENT7_DATA4();
			d4.health = health;
			d4.maxHealth = health;
			d4.maxHealthN = health;

			d4.imagination = imagination;
			d4.maxImagination = imagination;
			d4.maxImaginationN = imagination;

			d4.armor = armor;
			d4.maxArmor = armor;
			d4.maxArmorN = armor;

			c7->setData4(d4);
			c7->getData4_1Ref()->push_back(factionID);
			c7->setTrigger(true);
			c7->setData4_4_1(false);
			c7->setData4_4_2(false);
			c7->setData5(false);
		}
	}

  	this->addComponent(new RenderComponent());
}

SimplePhysicsComponent *SmashableObject::getSimplePhysicsComponent(){ return (SimplePhysicsComponent *)this->getComponent(3); }
DestructibleComponent *SmashableObject::getDestructibleComponent(){ return (DestructibleComponent *)this->getComponent(7); }
RenderComponent *SmashableObject::getRenderComponent(){ return (RenderComponent *)this->getComponent(2); }