#pragma once

enum LOT : long{
	LOT_NONE = 0,
	LOT_PLAYER = 1,
	LOT_SHIRT_LU_PURPLE = 16065,
	LOT_SHIRT_LU_LIGHT_GREY = 16066,
	LOT_SHIRT_LU_GREY = 16067,
	LOT_SHIRT_LU_GREEN = 16068,
	LOT_SHIRT_LU_ORANGE = 16069,
	LOT_SHIRT_LU_BEIGE = 16070,
	LOT_SHIRT_ARMOR_RED = 16071,
	LOT_SHIRT_ARMOR_BLACK = 16072,
	LOT_SHIRT_ARMOR_BLUE = 16073,
	LOT_SHIRT_ARMOR_WHITE = 16074,
	LOT_SHIRT_ARMOR_PURPLE = 16075,
	LOT_SHIRT_ARMOR_LIGHT_GREY = 16077,
	LOT_SHIRT_ARMOR_GREY = 16078,
	LOT_SHIRT_ARMOR_GREEN_PALE = 16079,
	LOT_SHIRT_ARMOR_ORANGE = 16080,
	LOT_SHIRT_ARMOR_BEIGE = 16081,
	LOT_SHIRT_OLD_GREY = 16082,
	LOT_JETPACK = 1727,
	LOT_PROPERTY_JETPACK = 7292,
	LOT_KINGS_CROWN = 8544,
	LOT_UNKNOWN4 = 8642,
	LOT_SHIELD_WOLF = 12677,
	LOT_FANTASTIC_PILUM = 12730,
	LOT_NIGHTLASHER = 13275,
	LOT_SLITHERSTRIKER = 13276,
	LOT_ENERGY_SPORK = 13277,
	LOT_ZAPZAPPER = 13278
};

enum REPLICA_PACKET_TYPE{
	REPLICA_CONSTRUCTION_PACKET = 0,
	REPLICA_SERIALIZATION_PACKET = 1,
	REPLICA_DESTRUCTION_PACKAGE = 2
};

enum COMPONENT_TYPE : long {
	CONTROLABLE_PHYSICS_COMPONENT = 1,
	RENDER_COMPONENT,
	SIMPLE_PHYSICS_COMPONENT,
	CHARACTER_COMPONENT, //only two in table
	SCRIPT_COMPONENT,
	BOUNCER_COMPONENT,
	DESTRUCTABLE_COMPONENT,

	SKILL_COMPONENT = 9,
	SPAWNER_COMPONENT, //used for LOT 176, a spawner
	ITEM_COMPONENT, //used for quite a few objects, they vary too much to determine what it is
	MODULAR_BUILD_COMPONENT, //This is for modular building areas, rocket bays and car bays
	REBUILD_START_COMPONENT, //Rebuildables and Thinking hat has this for some reason
	REBUILD_ACTIVATOR_COMPONENT, //only one in table, but no object matches that id
	ICON_ONLY_RENDER_COMPONENT, //only one in table, but no object matches that id
	VENDOR_COMPONENT,
	INVENTORY_COMPONENT,
	PROJECTILE_PHYSICS_COMPONENT, //this is used by shooting gallery objects
	SHOOTING_GALLERY_COMPONENT, //cannon component? Is used by cannon objects
	RIGID_BODY_PHANTOM_PHYSICS_COMPONENT, //Is used by objects in racing

	CHEST_COMPONENT = 22, //Only used by a treasure chest
	COLLECTIBLE_COMPONENT, //used by collectable spawners
	BLUEPRINT_COMPONENT, //used in showing a model in the inventory
	MOVING_PLATFORM_COMPONENT, //Is used by moving platforms, could be a moving platform component
	PET_COMPONENT,
	PLATFORM_BOUNDRY_COMPONENT, //another moving platform component, potentially
	MODULE_COMPONENT, //Modular Component? All the objects are pieces to rockets, etc.
	JET_PACK_PAD_COMPONENT, //JetpackComponent? All objects using this have to do with jetpacks
	VEHICLE_PHYSICS_COMPONENT,
	MOVEMENT_AI_COMPONENT, //only enemies have this for the most part
	EXHIBIT_COMPONENT, //Exhibit Component?

	MINIFIG_COMPONENT = 35, //All NPC's have this component...
	PROPERTY_COMPONENT, //This component is used by property related objects
	PET_NEST_COMPONENT, //only one in table, used by LOT 3336, which is described as a nest asset. Possibly a petNestComponent
	MODEL_BUILDER_COMPONENT, //only two in table, LWOModelBuilderComponent is listed in the description of LOT 6228
	SCRIPTED_ACTIVITY_COMPONENT,
	PHANTOM_PHYSICS_COMPONENT,
	SPRINGPAD_COMPONENT, //A component for testing "new springpads" LOT 4816 for example
	MODEL_BEHAVIORS_COMPONENT, //Models, or something...?
	PROPERTY_ENTRANCE_COMPONENT, //Property Lauchpad components
	FX_COMPONENT, //Not one object uses this
	PROPERTY_MANAGEMENT_COMPONENT, //only one in table, LOT 3315
	VEHICLE_PHYSICS_2_COMPONENT, //Flying vehicle tests component
	PHYSICS_SYSTEM_COMPONENT, //Used by a lot of LUP freebuild objects, LOT 7138
	REBUILD_COMPONENT, 
	SWITCH_COMPONENT,
	MINIGAME_COMPONENT, //only two in table, one is the biplane(LOT 4625), the other is LOT 2365, a zone control object
	CHANGLING_COMPONENT, //used by "Kipper Duel"  models...
	CHOICE_BUILD_COMPONENT, //choice build component?
	PACKAGE_COMPONENT, //Loot pack component?
	SOUND_REPEATER_COMPONENT, //only two in table, both are sound objects, this must be a sound component
	SOUND_AMBIENT_2D_COMPONENT, //only two in table, and those are the player objects
	SOUND_AMBIENT_3D_COMPONENT, //only one in table, which is an ambient sound object
	PLAYER_FLAGS_COMPONENT, //used in pirate siege...

	CUSTUM_BUILD_ASSEMBLY_COMPONENT = 59, //only one in table, LOT 6398. a test rocket
	BASE_COMBAT_AI_COMPONENT,
	MODULE_ASSEMBLY_COMPONENT, //used by cars and rockets, modular stuff
	SHOWCASE_MODEL_COMPONENT, //showcase component? (LOT 6545)
	RACING_MODULE_COMPONENT, //another modular related component
	GENERIC_ACTIVATOR_COMPONENT, //only three in table, a middle module component?
	PROPERTY_VENDOR_COMPONENT, //only two in table, property selling venders
	HF_LIGHT_DIRECTION_COMPONENT, //only one in table, LOT 6968, a light direction component?
	ROCKET_LAUNCHPAD_CONTROL_COMPONENT, //launchpad related component
	ROCKET_LANDING_COMPONENT, //only two in table, and those are the player objects

	RACING_CONTROL_COMPONENT = 71,
	FACTION_TRIGGER_COMPONENT, //something to do with rank items... maybe to do with skills?
	MISSION_NPC_COMPONENT, //missions giver component?
	RACING_STATS_COMPONENT, //only two in table, racing car related
	LUP_EXHIBIT_COMPONENT, //only one in table, LUP exhibit related, LOT 9461

	SOUND_TRIGGER_COMPONENT = 77, //sound trigger component
	PROXIMITY_MONITOR_COMPONENT, //more launchpad related stuff
	RACING_SOUND_TRIGGER_COMPONENT, //only two in table, sound trigger related

	USER_CONTROL_COMPONENT = 95, //skateboard component

	UNKOWN_97_COMPONENT = 97, //only two in table, both are Starbase 3001 launcher related

	UNKOWN_100_COMPONENT = 100, //brick donation component

	UNKOWN_102_COMPONENT = 102, //only two in table, commendation vendor component?
	UNKOWN_103_COMPONENT, //only two in table, nothing in objects
	RAIL_ACTIVATOR_COMPONENT, //rail activator related
	UNKOWN_105_COMPONENT, //only three in table, ? I haven't a clue as to what this is supposed to be
	UNKOWN_106_COMPONENT, //only one in table, related to skateboard mount, LOT 16684
	UNKNOWN_107_COMPONENT, //only one in table, generic player
	UNKNOWN_108_COMPONENT, //for vehicles

	UNKOWN_113_COMPONENT = 113, //only one in table, property plaque
	UNKOWN_114_COMPONENT, //used by building bays

	UNKOWN_116_COMPONENT = 116 //only one in table, LOT 16512, a culling plane, culls objects behind them
};

#pragma pack(push, 1)
//need to keep this stuff in a db with the rest of it
struct PLAYER_STATS{
	unsigned long long CurrencyCollected = 1;
	unsigned long long BricksCollected = 2;
	unsigned long long SmashablesSmashed = 3;
	unsigned long long QuickBuildsCompleted = 4;
	unsigned long long EnemiesSmashed = 5;
	unsigned long long RocketsUsed = 6;
	unsigned long long MissionsCompleted = 7;
	unsigned long long PetsTamed = 8;
	unsigned long long ImaginationPowerupsCollected = 9;
	unsigned long long LifePowerupsCollected = 10;
	unsigned long long ArmorPowerupsCollected = 11;
	unsigned long long DistanceTraveled = 12;
	unsigned long long Smashed = 13;
	unsigned long long DamageTaken = 14;
	unsigned long long DamageHealed = 15;
	unsigned long long ArmorRepaired = 16;
	unsigned long long ImaginationRestored = 17;
	unsigned long long ImaginationUsed = 18;
	unsigned long long DistanceDriven = 19;
	unsigned long long AirborneInRacecar = 20;
	unsigned long long RacingImaginationPowerupsCollected = 21;
	unsigned long long RacingImaginationCratesSmashed = 22;
	unsigned long long RaceCarBoostActivated = 23;
	unsigned long long WrecksInRaceCar = 24;
	unsigned long long RacingSmashablesSmashed = 25;
	unsigned long long RacesFinished = 26;
	unsigned long long Place1RacesFinishes = 27;
};

struct PLAYER_STYLE{
	unsigned long hairColor = 0;
	unsigned long hairStyle = 0;
	unsigned long HD_HDC = 0;
	unsigned long shirtColor = 0;
	unsigned long pantsColor = 0;
	unsigned long CD = 0;
	unsigned long HDC_HD = 0;
	unsigned long eyebrowsStyle = 0;
	unsigned long eyesStyle = 0;
	unsigned long mouthStyle = 0;
};

struct PLAYER_INFO{
	unsigned long long accountID = 0;
	unsigned long long llog = 0;
	unsigned long long unknown = 0;
	unsigned long long legoScore = 0;
	bool isFreeToPlay = false;
};

struct REPLICA_INDEX29_6_DATA{
	bool d29_6_1 = false;
	bool d29_6_2 = false;
	unsigned char d29_6_3 = 0;
	bool d29_6_4 = false;
	unsigned char d29_6_5 = 0;
};

struct REPLICA_INDEX29_8_DATA{
	unsigned long long d29_8_1 = 0;
	unsigned char d29_8_2 = 0;
	bool d29_8_3 = true;
	unsigned long d29_8_4 = 0xFFFFFFFF;
};

struct REPLICA_OBJECT_BASE{
	unsigned long long objid;
	LOT LOT;
};

struct REPLICA_OBJECT_EQUIPMENT{
	REPLICA_OBJECT_BASE obj;
	unsigned short slot;
};

struct REPLICA_OBJECT_POSITION{
	float posX = 0.0F;
	float posY = 0.0F;
	float posZ = 0.0F;
	void setPosition(float x, float y, float z){
		posX = x;
		posY = y;
		posZ = z;
	};
	REPLICA_OBJECT_POSITION(float x, float y, float z){
		posX = x;
		posY = y;
		posZ = z;
	}
	REPLICA_OBJECT_POSITION(){
		posX = 0;
		posY = 0;
		posZ = 0;
	}
};
#pragma pack(pop)