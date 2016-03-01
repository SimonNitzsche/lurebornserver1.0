#include "Database.h"
#include "EnemyObject.h"
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

EnemyObject::EnemyObject(unsigned long lot, unsigned long currentZone) {
	Logger::log("REPL", "ENEMY", "Initializing Enemy");

	this->zoneID = currentZone;
	initializeEnemy(lot);
}

EnemyObject::~EnemyObject() {

}

void EnemyObject::initializeEnemy(unsigned long lot) {
	this->objid = ObjectID::generateObjectID();
	this->LOT = lot;

	SpawnerObject *spawner = new SpawnerObject(ObjectID::generateSpawnerID());
	this->spawnerObject = spawner;
	this->spawnerObjID = spawner->objid;
	this->spawner_node_id = 0;

	this->addComponent(new ControllablePhysicsComponent());
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
			c7->setData4_4_1(true);
			c7->setData5(false);
		}
	}

	stringstream ss1, ss2, ss3, ss4;
	ss1 << "SELECT `component_id` FROM `ComponentsRegistry` WHERE `id` = " << lot << " AND `component_type` = " << 17 << ";";
	ss2 << "SELECT `component_id` FROM `ComponentsRegistry` WHERE `id` = " << lot << " AND `component_type` = " << 5 << ";";
	ss3 << "SELECT `component_id` FROM `ComponentsRegistry` WHERE `id` = " << lot << " AND `component_type` = " << 9 << ";";
	ss4 << "SELECT `component_id` FROM `ComponentsRegistry` WHERE `id` = " << lot << " AND `component_type` = " << 16 << ";";

	sqdb::Statement state1 = SQLiteDatabase::Query("cdclient.sqlite", ss1.str().c_str());
	if (state1.Next() != false) {
		this->isInventoryAdded = true;
		this->addComponent(new InventoryComponent());

		InventoryComponent *c17 = this->getInventoryComponent();

		int componentId = state1.GetField(0);

		stringstream itemsQuery;
		itemsQuery << "SELECT group_concat(`itemid`) FROM `InventoryComponent` WHERE `id` = '" << componentId << "';";

		sqdb::Statement state1_1 = SQLiteDatabase::Query("cdclient.sqlite", itemsQuery.str().c_str());

		if (state1_1.Next()) {
			cout << "Object with LOT " << lot << " has item " << state1_1.GetField(0).GetText() << endl;

			std::stringstream stringstream(state1_1.GetField(0).GetText());
			std::string token;

			unsigned long long oldID = 0;
			while (std::getline(stringstream, token, ',')) {
				unsigned long equipLOT = stoul(token);
				LootObject *lootObj = new LootObject(lot, ObjectID::generateObjectID());

				c17->equipNPCItem(equipLOT, lootObj->objid);
				equippedItems.push_back(lootObj);
			}
			c17->setData2(0);
		}
	}

	sqdb::Statement state2 = SQLiteDatabase::Query("cdclient.sqlite", ss2.str().c_str());
	if (state2.Next() != false) {
		this->addComponent(new ScriptComponent());
		this->isScriptAdded = true;
	}

	sqdb::Statement state3 = SQLiteDatabase::Query("cdclient.sqlite", ss3.str().c_str());
	if (state3.Next() != false) {
		this->addComponent(new SkillComponent());
		this->isSkillAdded = true;
	}

	sqdb::Statement state4 = SQLiteDatabase::Query("cdclient.sqlite", ss4.str().c_str());
	if (state4.Next() != false) {
		this->addComponent(new VendorComponent());
		this->isVendor = true;
	}

	this->addComponent(new RenderComponent());
}

void EnemyObject::deleteSubObjects() {
	for (int k = 0; k < this->equippedItems.size(); k++) {
		ObjectsManager::unregisterObject(this->equippedItems.at(k));
	}
}

ControllablePhysicsComponent *EnemyObject::getControllablePhysicsComponent(){ return (ControllablePhysicsComponent *)this->getComponent(1); }
DestructibleComponent *EnemyObject::getDestructibleComponent(){ return (DestructibleComponent *)this->getComponent(7); }
InventoryComponent *EnemyObject::getInventoryComponent(){ if (isInventoryAdded) { return (InventoryComponent *)this->getComponent(17); } else { return NULL; } }
ScriptComponent *EnemyObject::getScriptComponent(){ if (isScriptAdded){ return (ScriptComponent *)this->getComponent(5); } else { return NULL; } }
SkillComponent *EnemyObject::getSkillComponent(){ if (isSkillAdded){ return (SkillComponent *)this->getComponent(9); } else { return NULL; } }
VendorComponent *EnemyObject::getVendorComponent(){ if (isVendor){ return (VendorComponent *)this->getComponent(16); } else { return NULL; } }
RenderComponent *EnemyObject::getRenderComponent(){ return (RenderComponent *)this->getComponent(2); }

//bool EnemyObject::didAddInventory(){ return this->isInventoryAdded; }
//bool EnemyObject::didAddVendorComponent(){ return this->isVendor; }
//bool EnemyObject::didAddScript(){ return this->isScriptAdded; }
//bool EnemyObject::didAddSkill(){ return this->isSkillAdded; }