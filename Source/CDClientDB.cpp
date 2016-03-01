#include "CDClientDB.h"
#include "SQLiteDatabase.h"
#include "UtfConverter.h"
#include "Logger.h"

std::string CDClientDB::getObjectType(unsigned long LOT) {
	std::stringstream ss;
	ss << "SELECT type FROM Objects WHERE id = '" << LOT << "';";
	sqdb::Statement stm = SQLiteDatabase::Query("cdclient.sqlite", ss.str());

	if (stm.Next())
		return stm.GetField(0).GetText();
	else
		return "INVALID";
}

unsigned long CDClientDB::getComponentID(unsigned long LOT, unsigned long componentType) {
	std::stringstream ss;
	ss << "SELECT component_id FROM ComponentsRegistry WHERE id = '" << LOT << "' AND component_type = '" << componentType << "';";
	sqdb::Statement stm = SQLiteDatabase::Query("cdclient.sqlite", ss.str());

	if (stm.Next())
		return stm.GetField(0).GetInt();
	else
		return -1;
}

//std::string CDClientDB::getEquipLocation(unsigned long componentID) {
//	std::stringstream ss;
//	ss << "SELECT equipLocation FROM ItemComponent WHERE id = '" << componentID << "';";
//	sqdb::Statement stm = SQLiteDatabase::Query("cdclient.sqlite", ss.str());
//
//	if (stm.Next())
//		return stm.GetField(0).GetText();
//	else
//		return "INVALID";
//}

unsigned long CDClientDB::getItemType(unsigned long componentID) {
		std::stringstream ss;
		ss << "SELECT itemType FROM ItemComponent WHERE id = '" << componentID << "';";
		sqdb::Statement stm = SQLiteDatabase::Query("cdclient.sqlite", ss.str());

		if (stm.Next())
			return stm.GetField(0).GetInt();
		else
			return -1;
}

unsigned long CDClientDB::getSkillID(unsigned long LOT, unsigned long castOnType) {
	std::stringstream ss;
	ss << "SELECT skillID FROM ObjectSkills WHERE objectTemplate = '" << LOT << "' AND castOnType = '" << castOnType << "';";
	sqdb::Statement stm = SQLiteDatabase::Query("cdclient.sqlite", ss.str());

	if (stm.Next())
		return stm.GetField(0).GetInt();
	else
		return -1;
}

bool CDClientDB::isAnimationValid(std::string animationID) {
	std::stringstream ss;
	ss << "SELECT animationGroupID FROM Animations WHERE animation_type = '" << animationID << "';";
	sqdb::Statement stm = SQLiteDatabase::Query("cdclient.sqlite", ss.str());

	if (stm.Next())
		return true;
	else
		return false;
}

std::wstring CDClientDB::getAnimationOfEmote(unsigned short emoteID) {
	std::stringstream ss;
	ss << "SELECT animationName FROM Emotes WHERE id = '" << emoteID << "';";
	sqdb::Statement stm = SQLiteDatabase::Query("cdclient.sqlite", ss.str());

	if (stm.Next())
		return UtfConverter::FromUtf8(stm.GetField(0).GetString());
	else
		return L"INVALID";
}

std::vector<unsigned long> CDClientDB::getInventoryComponentData(unsigned long LOT) {
	unsigned long componentID = CDClientDB::getComponentID(LOT, 17);

	std::stringstream ss;
	ss << "SELECT itemid FROM InventoryComponent WHERE id = '" << componentID << "';";
	sqdb::Statement stm = SQLiteDatabase::Query("cdclient.sqlite", ss.str());

	std::vector<unsigned long> equipment;
	while (stm.Next()) {
		equipment.push_back(stm.GetField(0).GetInt());
	}

	return equipment;
}

std::vector<long> CDClientDB::getDestructibleComponentData(unsigned long LOT) {
	unsigned long componentID = CDClientDB::getComponentID(LOT, 7);

	std::stringstream ss;
	ss << "SELECT `faction`, `life`, `imagination`, `armor` FROM `DestructibleComponent` WHERE `id` = " << componentID << ";";
	sqdb::Statement stm = SQLiteDatabase::Query("cdclient.sqlite", ss.str());

	std::vector<long> data;
	if (stm.Next()) {
		data.push_back(stm.GetField(0).GetInt());
		data.push_back(stm.GetField(1).GetInt());
		data.push_back(stm.GetField(2).GetInt());
		data.push_back(stm.GetField(3).GetInt());
	}

	return data;
}