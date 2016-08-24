#pragma once
#include <vector>
#include "Database.h"

struct InventoryItem {
	long long objid = 0;
	long lot;
	long slot = 0;
	long tab = 0;
	int qnt = 1;
	bool linked = false;
};

struct ObjectInfo {
	long long objid = 0;
	long lot = 0;
	long long spawnerid = 0;
	ObjectInfo(long long id, long t, long long s) {
		this->objid = id;
		this->lot = t;
		this->spawnerid = s;
	}
};

struct RocketInfo {
	long nose_cone_template = 0;
	long cockpit_template = 0;
	long engine_template = 0;
	RocketInfo(long cone, long cockpit, long engine) {
		this->nose_cone_template = cone;
		this->cockpit_template = cockpit;
		this->engine_template = engine;
	}
};

struct CarInfo {
	long front_bumper_template = 0;
	long engine_panel_template = 0;
	long side_panel_template = 0;
	long rear_panel_template = 0;
	long rear_bumper_template = 0;
	long wheels_template = 0;
	long chassis_template = 0;

	CarInfo(long frontB, long engineP, long sideP, long rearP, long rearB, long wheels, long chassis) {
		this->front_bumper_template = frontB;
		this->engine_panel_template = engineP;
		this->rear_bumper_template = rearB;
		this->rear_panel_template = rearP;
		this->side_panel_template = sideP;
		this->wheels_template = wheels;
		this->chassis_template = chassis;
	}
};

class InventoryTable : public MySQLTable {
private:
	static void updateSlotOfItem(long long objid, long long charid, unsigned long newslot);
public:
	static bool moveItemToSlot(long long objid, long long charid, unsigned long slot);
	static unsigned long getSlotFromItem(long long objid, long long charid);
	static long long getItemFromSlot(long long charid, unsigned long slot);
	static void deleteInventory(long long charid);
	static void insertItem(long long charid, unsigned long objTemplate, long long objid, unsigned long qnt, unsigned long slot, bool linked=0, long tab=0);
	static void deleteItem(long long charid, long long objid);
	static std::vector<InventoryItem> getItems(long long charid);

	std::string getName();
	void mapTable(std::unordered_map<std::string, compare<ColData *> *> * data);
};

class ObjectsTable : public MySQLTable {
public:
	//Get the LOT of an object
	static long getTemplateOfItem(long long objid);

	static ObjectInfo getItemInfo(long long objid);
	//Create an object with the specified LOT
	static long long createObject(long lot);
	static void deleteObject(long long objid);

	//Custom Objects:
	//Rocket (6416)
	static RocketInfo getRocketInfo(long long objid);

	//Racing Car(8092)


	static CarInfo getCarInfo(long long objid);

	std::string getName();
	void mapTable(std::unordered_map<std::string, compare<ColData *> *> * data);
};

class EquipmentTable : public MySQLTable {
public:
	static std::vector<long long> getItems(long long charid);
	static void equipItem(long long charid, long long objectid, unsigned long itemType);
	static void unequipItem(long long charid, long long objectid);
	static void deleteEquipment(long long charid);

	std::string getName();
	void mapTable(std::unordered_map<std::string, compare<ColData *> *> * data);
};


enum ItemType : unsigned char {
	INVALID = 0,
	BRICK,
	HAT,
	HAIR,
	NECK,
	LEFT_HAND,
	RIGHT_HAND,
	LEGS,
	LEFT_TRINKET,
	RIGHT_TRINKET,
	BEHAVIOR,
	PROPERTY,
	MODEL,
	COLLECTABLE,
	CONSUMABLE,
	CHEST,
	EGG,
	PET_FOOD,
	QUEST_OBJECT,
	PET_INVENTORY_ITEM,
	t20_PACKAGE,
	LOOT_MODEL,
	VEHICLE
};