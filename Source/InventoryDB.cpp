#include "InventoryDB.h"
#include "Database.h"
#include "Logger.h"
#include "ObjectID.h"
#include "ReplicaObject.h"
#include "CDClientDB.h"
#include "SQLiteDatabase.h"

#include <sstream>

void InventoryTable::updateSlotOfItem(long long objid, long long charid, unsigned long newslot) {
	std::stringstream oi;
	oi << "UPDATE `inventory` SET `slot` = '" << newslot << "' WHERE `objectid` = " << objid << ";";
	Database::Query(oi.str());
}

bool InventoryTable::moveItemToSlot(long long objid, long long charid, unsigned long slot) {
	unsigned long oldslot = InventoryTable::getSlotFromItem(objid, charid);
	if (oldslot != ~((unsigned long)0)) {
		int tab = InventoryTable::getTab(ObjectsTable::getTemplateOfItem(objid));
		long long ot_objid = InventoryTable::getItemFromSlot(charid, slot, tab);
		if (ot_objid > -1) {
			InventoryTable::updateSlotOfItem(ot_objid, charid, oldslot);
		}
		InventoryTable::updateSlotOfItem(objid, charid, slot);
	}
	return true;
}

unsigned long InventoryTable::getSlotFromItem(long long objid, long long charid) {
	std::stringstream sq;
	sq << "SELECT `slot` FROM `inventory` WHERE `owner` = '" << charid << "' AND `objectid` = '" << objid << "';";
	auto sqr = Database::Query(sq.str());
	if (mysql_num_rows(sqr) > 0) {
		auto row = mysql_fetch_row(sqr);
		unsigned long slot = std::stoi(row[0]);
		return slot;
	}
	else {
		return ~((unsigned long)0); //return max as negative value
	}
}

long long InventoryTable::getItemFromSlot(long long charid, unsigned long slot, int tab) {
	std::stringstream iq;
	iq << "SELECT `objectid` FROM `inventory` WHERE `owner` = '" << charid << "' AND `slot` = '" << slot << "' AND `tab` = '"<<tab<<"';";
	auto iqr = Database::Query(iq.str());
	if (mysql_num_rows(iqr) > 0) {
		auto ir = mysql_fetch_row(iqr);
		long long objid = std::stoll(ir[0]);
		return objid;
	}
	else {
		return -1; //-1 as return value
	}
}

void InventoryTable::deleteInventory(long long charid) {
	std::stringstream eqqr;
	eqqr << "DELETE FROM `inventory` WHERE `owner`='" << charid << "';";
	Database::Query(eqqr.str());
}

int InventoryTable::getTab(long lot) {
	std::stringstream ss;
	ss << "SELECT `type` FROM `Objects` WHERE `id` = '" << lot << "';";
	sqdb::Statement statement = SQLiteDatabase::Query("cdclient.sqlite", ss.str().c_str());
	if (statement.Next()) {
		std::string type = statement.GetField(0).GetString();
		if (type == "Behavior") {
			return 7;
		}
		else if (type == "LEGO brick") {
			return 2;
		}
		else if (type == "Model" || type == "Rebuildables") {
			return 5;
		}
		else {
			return 0;
		}
	}
}

void InventoryTable::insertItem(long long charid, unsigned long objTemplate, long long objid, unsigned long qnt, unsigned long slot, bool linked, long tab) {
	std::stringstream str;
	if (objid < 288000000000000000) { objid = ObjectID::generateObjectID(); }
	str << "INSERT INTO `inventory` (`owner`, `object`, `qnt`, `slot`, `linked`, `tab`, `objectid`) VALUES(" << charid << ", " << objTemplate << ", " << std::to_string(qnt) << ", " << std::to_string(slot) << ", ";
	if (linked) str << "1"; else str << "0";
	str << ", " << tab << ", " << objid << ");";
	Database::Query(str.str());
}

void InventoryTable::deleteItem(long long charid, long long objid) {
	Database::Query("DELETE FROM `inventory` WHERE `owner` = '" + std::to_string(charid) + "' AND `objectid` = '" + std::to_string(objid) + "';");
}


std::vector<InventoryItem> InventoryTable::getItems(long long charid) {
	std::string qr = "SELECT `object`, `qnt`, `slot`, `linked`, `tab`, `objectid` FROM `inventory` WHERE `owner` = " + std::to_string(charid);
	auto qrr = Database::Query(qr);
	unsigned long long num = mysql_num_rows(qrr);
	std::vector<InventoryItem> items;
	items.reserve((int)num);
	MYSQL_ROW row;
	while (row = mysql_fetch_row(qrr)) {
		InventoryItem i;
		i.lot = std::stol(row[0]);
		i.qnt = std::stoi(row[1]);
		i.slot = std::stol(row[2]);
		i.tab = std::stol(row[4]);
		i.objid = std::stoll(row[5]);
		if (std::stoi(row[3]) == 1) i.linked = true;
		items.push_back(i);
	}
	return items;
}

std::string InventoryTable::getName() {
	return "inventory";
}

void InventoryTable::mapTable(std::unordered_map<std::string, compare<ColData *> *> * data) {
	Database::addColToMap(data, "id", new ColData("int(11)", false, "PRI", "NULL", "auto_increment"));
	Database::addColToMap(data, "owner", new ColData("bigint(64)", false, "", "NULL", ""));
	Database::addColToMap(data, "object", new ColData("bigint(64)", false, "", "NULL", ""));
	Database::addColToMap(data, "qnt", new ColData("smallint(1) unsigned", false, "", "0", ""));
	Database::addColToMap(data, "slot", new ColData("smallint(3) unsigned", false, "", "NULL", ""));
	Database::addColToMap(data, "linked", new ColData("tinyint(1)", false, "", "0", ""));
}

long ObjectsTable::getTemplateOfItem(long long objid) {
	std::stringstream str;
	str << "SELECT `object` FROM `inventory` WHERE `objectid` = '" << objid << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL) {
		return -1;
		Logger::logError("IVDB", "MYSQL", "getting LOT", mysql_error(Database::getConnection()));
	}
	if (mysql_num_rows(qr) > 0) {
		auto row = mysql_fetch_row(qr);
		long lot = std::stol(row[0]);
		return lot;
	}
	return -1;
}

ObjectInfo ObjectsTable::getItemInfo(long long objid) {
	return ObjectInfo(objid, ObjectsTable::getTemplateOfItem(objid), ObjectID::generateSpawnerID());
	std::stringstream str;
	str << "SELECT `template`, `spawnid` FROM `objects` WHERE `objectid` = '" << objid << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL) {

	}
	else {
		if (mysql_num_rows(qr) > 0) {
			auto row = mysql_fetch_row(qr);
			long lot = std::stol(row[0]);
			long long spawnid = -1;
			if (row[1] != NULL) {
				spawnid = std::stoll(row[1]);
			}
			return ObjectInfo(objid, lot, spawnid);
		}
	}
	return ObjectInfo(0, 0, 0);
}

long long ObjectsTable::createObject(long lot) {
	std::stringstream str;
	str << "INSERT INTO `luni`.`objects` (`objectid`, `template`) VALUES(NULL, '" << lot << "');";
	Database::Query(str.str());
	long long objid = mysql_insert_id(Database::getConnection());
	return objid;
}

void ObjectsTable::deleteObject(long long objid) {
	Database::Query("DELETE FROM `objects` WHERE `objectid` = '" + std::to_string(objid) + "';");
}

RocketInfo ObjectsTable::getRocketInfo(long long objid) {
	std::stringstream str;
	str << "SELECT `nose_cone_template`, `cockpit_template`, `engine_template` FROM `rockets` WHERE `objectid` = '" << objid << "';";
	auto qr = Database::Query(str.str());
	if (mysql_num_rows(qr) > 0) {
		auto row = mysql_fetch_row(qr);
		long cone = std::stol(row[0]);
		long cockpit = std::stol(row[1]);
		long engine = std::stol(row[2]);
		return RocketInfo(cone, cockpit, engine);
	}
	return RocketInfo(0, 0, 0);
}

CarInfo ObjectsTable::getCarInfo(long long objid) {
	std::stringstream str;
	str << "SELECT `front_bumper_template`, `engine_panel_template`, `side_panel_template`, `rear_panel_template`, `rear_bumper_template`, `chassis_template`, `wheels_template` FROM `cars` WHERE `objectid` = '" << objid << "';";
	auto qr = Database::Query(str.str());
	if (mysql_num_rows(qr) > 0) {
		auto row = mysql_fetch_row(qr);
		long frontB = std::stol(row[0]);
		long engineB = std::stol(row[1]);
		long sideP = std::stol(row[2]);
		long rearP = std::stol(row[3]);
		long rearB = std::stol(row[4]);
		long chassis = std::stol(row[5]);
		long wheels = std::stol(row[6]);
		return CarInfo(frontB, engineB, sideP, rearP, rearB, wheels, chassis);
	}
	else {
		return CarInfo(0, 0, 0, 0, 0, 0, 0);
	}
}

std::string ObjectsTable::getName() {
	return "objects";
}

void ObjectsTable::mapTable(std::unordered_map<std::string, compare<ColData *> *> * data) {
	Database::addColToMap(data, "objectid", new ColData("bigint(64)", false, "PRI", "NULL", "auto_increment"));
	Database::addColToMap(data, "template", new ColData("int(32) unsigned", false, "", "NULL", ""));
	Database::addColToMap(data, "spawnid", new ColData("bigint(20)", true, "", "NULL", ""));
	Database::addColToMap(data, "nose_cone_template", new ColData("int(11)", true, "", "NULL", ""));
	Database::addColToMap(data, "cockpit_template", new ColData("int(11)", true, "", "NULL", ""));
	Database::addColToMap(data, "engine_template", new ColData("int(11)", true, "", "NULL", ""));
}

std::vector<long long> EquipmentTable::getItems(long long charid) {
	auto qr = Database::Query("SELECT `object` FROM `equipment` WHERE `owner` = '" + std::to_string(charid) + "';");

	unsigned int numrows = (uint)mysql_num_rows(qr);
	std::vector<long long> items;
	items.reserve(numrows);
	for (unsigned int k = 0; k < numrows; k++) {
		auto ftc = mysql_fetch_row(qr);
		long long itemid = std::stoll(ftc[0]);
		items.push_back(itemid);
	}
	return items;
}

long long EquipmentTable::getFromItemType(long long charid, unsigned long it) {
	std::vector<long long> itms = EquipmentTable::getItems(charid);
	for each (long long itm in itms)
		if (CDClientDB::getItemType(CDClientDB::getComponentID(ObjectsTable::getTemplateOfItem(itm), 11)) == it)
			return itm;
	return NULL;
}

void EquipmentTable::equipItem(long long charid, long long objid, unsigned long itemType) {
	std::stringstream eqqr;

	eqqr << "INSERT INTO `equipment` (`id`, `owner`, `object`) VALUES(NULL, '" << charid << "', '" << objid << "');";
	Database::Query(eqqr.str());
}

void EquipmentTable::unequipItem(long long charid, long long objid) {
	std::stringstream eqqr;
	eqqr << "DELETE FROM `luni`.`equipment` WHERE `owner`='" << charid << "' AND `object`='" << objid << "';";
	Database::Query(eqqr.str());
}

void EquipmentTable::deleteEquipment(long long charid) {
	std::stringstream eqqr;
	eqqr << "DELETE FROM `equipment` WHERE `owner`='" << charid << "';";
	Database::Query(eqqr.str());
}

std::string EquipmentTable::getName() {
	return "equipment";
}

void EquipmentTable::mapTable(std::unordered_map<std::string, compare<ColData *> *> * data) {
	Database::addColToMap(data, "id", new ColData("int(11)", false, "PRI", "NULL", "auto_increment"));
	Database::addColToMap(data, "owner", new ColData("bigint(20)", false, "", "NULL", ""));
	Database::addColToMap(data, "object", new ColData("bigint(20)", false, "", "NULL", ""));
}