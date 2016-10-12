#include "GenericObject.h"
#include "Worlds.h"
#include "PlayerObject.h"
#include "SpawnerObject.h"
#include <algorithm>

#include "ObjectID.h"

#include "RakNet\ReplicaManager.h"
#include "ReplicaDefinitions.h"

#include "Logger.h"
#include "UtfConverter.h"

#include "SQLite\sqdb.h"
#include "SQLiteDatabase.h"
#include "StaticObjectsDB.h"

#include <map>
#include <sstream>

using namespace std;

bool outputAll = false; //Prints everything to the console

GenericObject::GenericObject(unsigned long lot, unsigned long currentZone, COMPONENT1_POSITION pos, COMPONENT1_ROTATION rot, COMPONENT1_VELOCITY vel, COMPONENT1_VELOCITY_ANGULAR vel_ang) {
	if(StaticObjectsDB::initDone)
		Logger::log("REPL", "OBJ", "Initializing New Generic Object");

	this->zoneID = currentZone;
	initializeObject(lot, pos, rot, vel, vel_ang);
}

GenericObject::~GenericObject() {

}

void GenericObject::initializeObject(unsigned long lot, COMPONENT1_POSITION pos, COMPONENT1_ROTATION rot, COMPONENT1_VELOCITY vel, COMPONENT1_VELOCITY_ANGULAR vel_ang) {
	this->objid = ObjectID::generateObjectID();
	this->LOT = lot;
	this->name = std::to_wstring(this->objid);

	SpawnerObject *spawner = new SpawnerObject(ObjectID::generateSpawnerID());
	this->spawnerObject = spawner;
	this->spawnerObjID = spawner->objid;
	this->spawner_node_id = 0;
	this->world.zone = this->zoneID;

	stringstream conponentsStream;
	conponentsStream << "SELECT `component_id`, `component_type` FROM `ComponentsRegistry` WHERE `id` = " << lot << ";";

	sqdb::Statement components = SQLiteDatabase::Query("cdclient.sqlite", conponentsStream.str());

	std::vector<ReplicaComponent *> list;

	while (components.Next())
	{
		long component_id = components.GetField(0).GetInt();
		long component_type = components.GetField(1).GetInt();

		switch (component_type)
		{
		case CONTROLABLE_PHYSICS_COMPONENT: {
			ControllablePhysicsComponent * comp = new ControllablePhysicsComponent();
			comp->setPosition(COMPONENT1_POSITION(pos.x, pos.y, pos.z));
			comp->setRotation(COMPONENT1_ROTATION(rot.x, rot.y, rot.z, rot.w));
			comp->setVelocity(COMPONENT1_VELOCITY(vel.x, vel.y, vel.z));
			comp->setAngularVelocity(COMPONENT1_VELOCITY_ANGULAR(vel_ang.x, vel_ang.y, vel_ang.z));
			list.push_back(comp);
			break;
		}
		case RENDER_COMPONENT: 
			list.push_back(new RenderComponent());
			break;
		case SIMPLE_PHYSICS_COMPONENT: {
			SimplePhysicsComponent *comp = new SimplePhysicsComponent();
			comp->setPosition(COMPONENT3_POSITION(pos.x, pos.y, pos.z));
			comp->setRotation(COMPONENT3_ROTATION(rot.x, rot.y, rot.z, rot.w));
			list.push_back(comp);
			break;
		}
		case CHARACTER_COMPONENT: 
			list.push_back(new CharacterComponent());
			break;
		case SCRIPT_COMPONENT: 
			list.push_back(new ScriptComponent());
			break;
		case BOUNCER_COMPONENT: 
			list.push_back(new BouncerComponent());
			break;
		case DESTRUCTABLE_COMPONENT: {
			DestructibleComponent *comp = new DestructibleComponent();

			COMPONENT7_DATA3 d3 = COMPONENT7_DATA3();
			d3.d1 = 0; d3.d2 = 0; d3.d3 = 0; d3.d4 = 0; d3.d5 = 0; d3.d6 = 0; d3.d7 = 0; d3.d8 = 0; d3.d9 = 0;

			comp->setData3(d3);


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

					if (lot == 4894 || lot == 7844) //explosive Crates from Gnarled Forest
						health = 9999;

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

					comp->setData4(d4);
					comp->getData4_1Ref()->push_back(factionID);
					comp->setTrigger(true);
					comp->setData4_4_1(false);
					comp->setData5(false);
				}
				list.push_back(comp);
			}
			break;
		}
		case SKILL_COMPONENT: 
			list.push_back(new SkillComponent());
			break;
		case SPAWNER_COMPONENT:
			
			break;
		case ITEM_COMPONENT:
			
			break;
		case MODULAR_BUILD_COMPONENT:

			break;
		case REBUILD_START_COMPONENT:

			break;
		case REBUILD_ACTIVATOR_COMPONENT:
			
			break;
		case ICON_ONLY_RENDER_COMPONENT:
			
			break;
		case VENDOR_COMPONENT: 
			list.push_back(new VendorComponent());
			break;
		case INVENTORY_COMPONENT: {
			InventoryComponent * comp = new InventoryComponent();
			stringstream itemsQ;
			itemsQ << "SELECT group_concat(`itemid`) FROM `InventoryComponent` WHERE `id` = '" << component_id << "';";
			sqdb::Statement items = SQLiteDatabase::Query("cdclient.sqlite", itemsQ.str());
			if (items.Next()){
				stringstream stringstream(items.GetField(0).GetText());
				string token;
				unsigned long long oldID = 0;
				while (getline(stringstream, token, ',')) {
					unsigned long equipLOT = stoul(token);
					LootObject *lootObj = new LootObject(lot, ObjectID::generateObjectID());
					comp->equipNPCItem(equipLOT, lootObj->objid);
					equippedItems.push_back(lootObj);
				}
				comp->setData2(0);
			}
			list.push_back(comp);
			break;
		}
		case PROJECTILE_PHYSICS_COMPONENT:

			break;
		case SHOOTING_GALLERY_COMPONENT:

			break;
		case RIGID_BODY_PHANTOM_PHYSICS_COMPONENT:
			list.push_back(new RigidBodyPhantomPhysicsComponent());
			break;
		case CHEST_COMPONENT:

			break;
		case COLLECTIBLE_COMPONENT:

			break;
		case BLUEPRINT_COMPONENT:

			break;
		case MOVING_PLATFORM_COMPONENT:

			break;
		case PET_COMPONENT:
			list.push_back(new PetComponent());
			break;
		case PLATFORM_BOUNDRY_COMPONENT:

			break;
		case MODULE_COMPONENT:

			break;
		case JET_PACK_PAD_COMPONENT:

			break;
		case VEHICLE_PHYSICS_COMPONENT:
			list.push_back(new VehiclePhysicsComponent());
			break;
		case MOVEMENT_AI_COMPONENT:

			break;
		case EXHIBIT_COMPONENT:

			break;
		case MINIFIG_COMPONENT:

			break;
		case PROPERTY_COMPONENT:

			break;
		case PET_NEST_COMPONENT:

			break;
		case MODEL_BUILDER_COMPONENT:

			break;
		case SCRIPTED_ACTIVITY_COMPONENT: 
			list.push_back(new ScriptedActivityComponent());
			break;
		case PHANTOM_PHYSICS_COMPONENT: {
			PhantomPhysicsComponent *comp = new PhantomPhysicsComponent;
			comp->setPosition(COMPONENT40_POSITION(pos.x, pos.y, pos.z));
			comp->setRotation(COMPONENT40_ROTATION(rot.x, rot.y, rot.z, rot.w));
			list.push_back(comp);
			break;
		}
		case SPRINGPAD_COMPONENT:

			break;
		case MODEL_BEHAVIORS_COMPONENT:

			break;
		case PROPERTY_ENTRANCE_COMPONENT:

			break;
		case FX_COMPONENT:

			break;
		case PROPERTY_MANAGEMENT_COMPONENT:

			break;
		case VEHICLE_PHYSICS_2_COMPONENT:
			list.push_back(new VehiclePhysicsComponent());
			break;
		case PHYSICS_SYSTEM_COMPONENT:

			break;
		case REBUILD_COMPONENT:

			break;
		case SWITCH_COMPONENT:
			list.push_back(new SwitchComponent());
			break;
		case MINIGAME_COMPONENT:

			break;
		case CHANGLING_COMPONENT:

			break;
		case CHOICE_BUILD_COMPONENT:

			break;
		case PACKAGE_COMPONENT:

			break;
		case SOUND_REPEATER_COMPONENT:

			break;
		case SOUND_AMBIENT_2D_COMPONENT:

			break;
		case SOUND_AMBIENT_3D_COMPONENT:

			break;
		case PLAYER_FLAGS_COMPONENT:

			break;
		case CUSTUM_BUILD_ASSEMBLY_COMPONENT:

			break;
		case BASE_COMBAT_AI_COMPONENT:
			list.push_back(new BaseCombatAIComponent());
			break;
		case MODULE_ASSEMBLY_COMPONENT:

			break;
		case SHOWCASE_MODEL_COMPONENT:

			break;
		case RACING_MODULE_COMPONENT:

			break;
		case GENERIC_ACTIVATOR_COMPONENT:

			break;
		case PROPERTY_VENDOR_COMPONENT:

			break;
		case HF_LIGHT_DIRECTION_COMPONENT:

			break;
		case ROCKET_LAUNCHPAD_CONTROL_COMPONENT:

			break;
		case ROCKET_LANDING_COMPONENT:

			break;
		case RACING_CONTROL_COMPONENT:
			list.push_back(new RacingControlComponent());
			break;
		case FACTION_TRIGGER_COMPONENT:

			break;
		case MISSION_NPC_COMPONENT:

			break;
		case RACING_STATS_COMPONENT:

			break;
		case LUP_EXHIBIT_COMPONENT:

			break;
		case SOUND_TRIGGER_COMPONENT:

			break;
		case PROXIMITY_MONITOR_COMPONENT:

			break;
		case RACING_SOUND_TRIGGER_COMPONENT:

			break;
		case USER_CONTROL_COMPONENT:

			break;
		case UNKOWN_97_COMPONENT:

			break;
		case UNKOWN_100_COMPONENT:

			break;
		case UNKOWN_102_COMPONENT:

			break;
		case UNKOWN_103_COMPONENT:

			break;
		case RAIL_ACTIVATOR_COMPONENT:

			break;
		case UNKOWN_105_COMPONENT:

			break;
		case UNKOWN_106_COMPONENT:

			break;
		case UNKNOWN_107_COMPONENT:
			list.push_back(new Index36Component());
			break;
		case UNKNOWN_108_COMPONENT:
			list.push_back(new Component108());
			break;
		case UNKOWN_113_COMPONENT:

			break;
		case UNKOWN_114_COMPONENT:

			break;
		case UNKOWN_116_COMPONENT:

			break;
		default:
			break;
		}
	}
	sort(list.begin(), list.end(), compareComponent);
	for (int i = 0; i < list.size(); i++){
		ReplicaComponent *r = list.at(i);
		if (outputAll) {
			Logger::log("GENERIC", "OBJECT", std::to_string(r->serialization));
		}
		this->addComponent(list.at(i));
	}
}

COMPONENT1_POSITION GenericObject::getPosition() {
	if (this->getComponent(SIMPLE_PHYSICS_COMPONENT) != NULL){
		SimplePhysicsComponent * phy = (SimplePhysicsComponent *)this->getComponent(SIMPLE_PHYSICS_COMPONENT);
		return COMPONENT1_POSITION(phy->getPosition().posX, phy->getPosition().posY, phy->getPosition().posZ);
	}
}

ControllablePhysicsComponent *GenericObject::getControllablePhysicsComponent() { return (ControllablePhysicsComponent*) this->getComponent(1); }
SimplePhysicsComponent  *GenericObject::getSimplePhysicsComponent() { return (SimplePhysicsComponent *) this->getComponent(3); }
DestructibleComponent *GenericObject::getDestructibleComponent(){ return (DestructibleComponent *)this->getComponent(7); }
InventoryComponent *GenericObject::getInventoryComponent(){ return (InventoryComponent *)this->getComponent(17); }
long long GenericObject::getObjectID() {
	return this->objid;
}


