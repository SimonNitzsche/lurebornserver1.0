#include <string>
#include "SQLite/sqdb.h"

class StaticObjectsDB {
public:
	static bool initDone;
	static sqdb::Statement Query(std::string querystr);
	static void spawnSmashables();
	static void spawnCarBuilders();
	static void spawnRocketBuilders();
	static void spawnNPCs();
	static void spawnChoicebuilds();
	static void spawnWorldObjects();
	static void spawnAmbientSound();
	static void spawnMissionObjects();
	static void spawnOtherObjects();
	static void loadAll();
};