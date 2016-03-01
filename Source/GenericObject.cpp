#include "GenericObject.h"
#include "Worlds.h"
#include "PlayerObject.h"
#include "SpawnerObject.h"

#include "ObjectID.h"

#include "RakNet\ReplicaManager.h"
#include "ReplicaDefinitions.h"

#include "Logger.h"
#include "UtfConverter.h"

#include "SQLite\sqdb.h"
#include "SQLiteDatabase.h"

#include <map>
#include <sstream>

using namespace std;

GenericObject::GenericObject(unsigned long lot, unsigned long currentZone) {
	Logger::log("REPL", "OBJ", "Initializing New Generic Object");

	this->zoneID = currentZone;
	initializeObject(lot);
}

GenericObject::~GenericObject() {

}

void GenericObject::initializeObject(unsigned long lot) {
	this->objid = ObjectID::generateObjectID();
	this->LOT = lot;

	SpawnerObject *spawner = new SpawnerObject(ObjectID::generateSpawnerID());
	this->spawnerObject = spawner;
	this->spawnerObjID = spawner->objid;
	this->spawner_node_id = 0;

	stringstream conponentsStream;
	conponentsStream << "SELECT `component_id`, `component_type` FROM `ComponentsRegistry` WHERE `id` = " << lot << ";";

	sqdb::Statement components = SQLiteDatabase::Query("cdclient.sqlite", conponentsStream.str());

	while (components.Next())
	{
		long component_id = components.GetField(0).GetInt();
		long component_type = components.GetField(1).GetInt();

		switch (component_type)
		{
		case CONTROLABLE_PHYSICS_COMPONENT: 
			this->addComponent(new ControllablePhysicsComponent());
			break;
		case RENDER_COMPONENT: 
			this->addComponent(new RenderComponent());
			break;
		case SIMPLE_PHYSICS_COMPONENT: 
			this->addComponent(new SimplePhysicsComponent());
			break;
		case CHARACTER_COMPONENT: 
			this->addComponent(new CharacterComponent());
			break;
		case SCRIPT_COMPONENT: 
			this->addComponent(new ScriptComponent());
			break;
		case BOUNCER_COMPONENT: 
			this->addComponent(new BouncerComponent());
			break;
		case DESTRUCTABLE_COMPONENT: 
			this->addComponent(new DestructibleComponent());
			break;
		case SKILL_COMPONENT: 
			this->addComponent(new SkillComponent());
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
			this->addComponent(new VendorComponent());
			break;
		case INVENTORY_COMPONENT: 

			break;
		case PROJECTILE_PHYSICS_COMPONENT:

			break;
		case SHOOTING_GALLERY_COMPONENT:

			break;
		case RIGID_BODY_PHANTOM_PHYSICS_COMPONENT:
			this->addComponent(new RigidBodyPhantomPhysicsComponent());
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
			this->addComponent(new PetComponent());
			break;
		case PLATFORM_BOUNDRY_COMPONENT:

			break;
		case MODULE_COMPONENT:

			break;
		case JET_PACK_PAD_COMPONENT:

			break;
		case VEHICLE_PHYSICS_COMPONENT:
			this->addComponent(new VehiclePhysicsComponent());
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
			this->addComponent(new ScriptedActivityComponent());
			break;
		case PHANTOM_PHYSICS_COMPONENT:
			this->addComponent(new PhantomPhysicsComponent());
			break;
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
			this->addComponent(new VehiclePhysicsComponent());
			break;
		case PHYSICS_SYSTEM_COMPONENT:

			break;
		case REBUILD_COMPONENT:

			break;
		case SWITCH_COMPONENT:
			this->addComponent(new SwitchComponent());
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
			this->addComponent(new BaseCombatAIComponent());
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
			this->addComponent(new RacingControlComponent());
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
			this->addComponent(new Index36Component());
			break;
		case UNKNOWN_108_COMPONENT:
			this->addComponent(new Component108());
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
}



