#include "EnvironmentalObject.h"
#include "Worlds.h"
#include "PlayerObject.h"
#include "SpawnerObject.h"
#include "StaticObjectsDB.h"

#include "ObjectID.h"

#include "RakNet\ReplicaManager.h"

#include "Logger.h"
#include "UtfConverter.h"

#include "SQLite\sqdb.h"
#include "SQLiteDatabase.h"

#include <map>
#include <sstream>

using namespace std;

EnvironmentalObject::EnvironmentalObject(unsigned long lot, unsigned long currentZone) {
	if (StaticObjectsDB::initDone)
		Logger::log("REPL", "ENV", "Initializing Environmental Object");

	this->zoneID = currentZone;
	initializeObject(lot);
}

EnvironmentalObject::~EnvironmentalObject() {

}

void EnvironmentalObject::initializeObject(unsigned long lot) {
	this->objid = ObjectID::generateObjectID();
	this->LOT = lot;

	SpawnerObject *spawner = new SpawnerObject(ObjectID::generateSpawnerID());
	this->spawnerObject = spawner;
	this->spawnerObjID = spawner->objid;
	this->spawner_node_id = 0;

	stringstream physicsStream;
	physicsStream << "SELECT `component_id` FROM `ComponentsRegistry` WHERE `id` = " << lot << " AND `component_type` = 3;";

	sqdb::Statement physicsType = SQLiteDatabase::Query("cdclient.sqlite", physicsStream.str());

	if (physicsType.Next() == true) {
		this->isSimplyPhysics = true;
		this->addComponent(new SimplePhysicsComponent());
	}
	else {
		this->isSimplyPhysics = false;
		this->addComponent(new PhantomPhysicsComponent());
	}

	stringstream destructibleComponentStream;
	destructibleComponentStream << "SELECT `component_id` FROM `ComponentsRegistry` WHERE `id` = " << lot << " AND `component_type` = 7;";

	sqdb::Statement destructibleComponentStatement = SQLiteDatabase::Query("cdclient.sqlite", destructibleComponentStream.str());

	if (destructibleComponentStatement.Next()) {
	//if (true) {
		int component_id = destructibleComponentStatement.GetField(0).GetInt();

		this->isDestructible = true;
		this->addComponent(new DestructibleComponent());

		stringstream destructibleStream;
		destructibleStream << "SELECT `faction`, `life`, `imagination`, `armor` FROM `DestructibleComponent` WHERE `id` = " << component_id << ";";

		sqdb::Statement destructibleStatement = SQLiteDatabase::Query("cdclient.sqlite", destructibleStream.str());

		while (destructibleStatement.Next()) {
			long factionID = destructibleStatement.GetField(0).GetInt();
			long health = destructibleStatement.GetField(1).GetInt();
			long imagination = destructibleStatement.GetField(2).GetInt();
			long armor = destructibleStatement.GetField(3).GetInt();

			DestructibleComponent *c7 = new DestructibleComponent();

			COMPONENT7_DATA3 d3 = COMPONENT7_DATA3();
			d3.d1 = 0; d3.d2 = 0; d3.d3 = 0; d3.d4 = 0; d3.d5 = 0; d3.d6 = 0; d3.d7 = 0; d3.d8 = 0; d3.d9 = 0;
			c7->setData3(d3);

			COMPONENT7_DATA4 d4;
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
			c7->setData4_4_1(true);
			c7->setData5(false);
		}
	}

	stringstream scriptComponentStream;
	scriptComponentStream << "SELECT `component_id` FROM `ComponentsRegistry` WHERE `id` = " << lot << " AND `component_type` = 5;";

	sqdb::Statement scriptComponentStatement = SQLiteDatabase::Query("cdclient.sqlite", scriptComponentStream.str());

	if (scriptComponentStatement.Next()) {
		this->isScriptAdded = true;
		this->addComponent(new ScriptComponent());
	}
}
SimplePhysicsComponent *EnvironmentalObject::getSimplePhysicsComponent(){ return (SimplePhysicsComponent *)this->getComponent(3); }
PhantomPhysicsComponent * EnvironmentalObject::getPhantomPhysicsComponent() { return (PhantomPhysicsComponent *)this->getComponent(40); }
DestructibleComponent *EnvironmentalObject::getDestructibleComponent(){ return (DestructibleComponent *)this->getComponent(7); }
ScriptComponent *EnvironmentalObject::getScriptComponent(){ if (isScriptAdded){ return (ScriptComponent *)this->getComponent(5); } else { return NULL; } }
RenderComponent *EnvironmentalObject::getRenderComponent(){ return (RenderComponent *)this->getComponent(2); }

long long EnvironmentalObject::getObjectID() {
	return this->objid;
}
