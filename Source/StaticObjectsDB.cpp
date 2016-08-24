#include "StaticObjectsDB.h"
#include "Database.h"
#include "Logger.h"
#include "SQLiteDatabase.h"

#include "GenericObject.h"
#include "NPCObject.h"
#include "EnvironmentalObject.h"

#include <stdlib.h>
#include <cstring>
#include <sstream>
using namespace std;

vector<string> split(const string &s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> tokens;
	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}

unsigned long stringToLOT(std::string lotstr) {
	unsigned long objectLOT;
	std::string lotR = lotstr;
	int lodCount = 1;

	size_t nPos = lotR.find(",", 0); // fist occurrence
	while (nPos != std::string::npos) {
		lodCount++;
		nPos = lotR.find(",", nPos + 1);
	}

	if (lodCount>0) {
		int choice = (1 + (rand() % (int)(lodCount - 1 + 1))) - 1;
		std::vector<std::string> choicable = split(lotR, 44);
		std::string choiced = choicable.at(choice);
		objectLOT = std::stoul(choiced);
	}
	else {
		objectLOT = std::stoul(lotR);
	}
	return objectLOT;
}

sqdb::Statement StaticObjectsDB::Query(std::string querystr) {
	return SQLiteDatabase::Query("../Env/staticObjects.sqlite", querystr);
}

void StaticObjectsDB::spawnSmashables() {
	Logger::log("SODB", "LOAD", "Load Smashables!");
	std::string str = "SELECT lot,zoneId,posX,posY,posZ,rotX,rotY,rotZ,rotW FROM smashables;";
	sqdb::Statement stm = Query(str);

	while (stm.Next()) {
		unsigned long objectLOT = stringToLOT(stm.GetField(0).GetString());

		int zone = stm.GetField(1).GetInt();
		double x = stm.GetField(2).GetDouble();
		double y = stm.GetField(3).GetDouble();
		double z = stm.GetField(4).GetDouble();
		double rX = stm.GetField(5).GetDouble();
		double rY = stm.GetField(6).GetDouble();
		double rZ = stm.GetField(7).GetDouble();
		double rW = stm.GetField(8).GetDouble();
		
		GenericObject * smashable = new GenericObject(objectLOT, zone, COMPONENT1_POSITION(x, y, z), COMPONENT1_ROTATION(rX, rY, rZ, rW), COMPONENT1_VELOCITY(), COMPONENT1_VELOCITY_ANGULAR());

		ObjectsManager::registerObject(smashable);
		//ObjectsManager::registerSmashable(smashable);
		ObjectsManager::create(smashable);
	}
}

void StaticObjectsDB::spawnCarBuilders() {
	Logger::log("SODB", "LOAD", "Load CarBuilders");
	std::string str = "SELECT lot,zoneId,posX,posY,posZ,rotX,rotY,rotZ,rotW FROM carbuilders;";
	sqdb::Statement stm = Query(str);

	while (stm.Next()) {
		unsigned long objectLOT = stringToLOT(stm.GetField(0).GetString());

		int zone = stm.GetField(1).GetInt();
		double x = stm.GetField(2).GetDouble();
		double y = stm.GetField(3).GetDouble();
		double z = stm.GetField(4).GetDouble();
		double rX = stm.GetField(5).GetDouble();
		double rY = stm.GetField(6).GetDouble();
		double rZ = stm.GetField(7).GetDouble();
		double rW = stm.GetField(8).GetDouble();

		NPCObject * carbuilder = new NPCObject(objectLOT, zone);
		SimplePhysicsComponent *c3 = carbuilder->getSimplePhysicsComponent();

		c3->setPosition(COMPONENT3_POSITION(x, y, z));
		c3->setRotation(COMPONENT3_ROTATION(rX, rY, rZ, rW));

		ObjectsManager::registerObject(carbuilder);
		ObjectsManager::create(carbuilder);
	}
}

void StaticObjectsDB::spawnRocketBuilders() {
	Logger::log("SODB", "LOAD", "Load RocketBuilders");
	std::string str = "SELECT lot,zoneId,posX,posY,posZ,rotX,rotY,rotZ,rotW FROM rocketbuilders;";
	sqdb::Statement stm = Query(str);

	while (stm.Next()) {
		unsigned long objectLOT = stringToLOT(stm.GetField(0).GetString());

		int zone = stm.GetField(1).GetInt();
		double x = stm.GetField(2).GetDouble();
		double y = stm.GetField(3).GetDouble();
		double z = stm.GetField(4).GetDouble();
		double rX = stm.GetField(5).GetDouble();
		double rY = stm.GetField(6).GetDouble();
		double rZ = stm.GetField(7).GetDouble();
		double rW = stm.GetField(8).GetDouble();

		NPCObject * rocketbuilder = new NPCObject(objectLOT, zone);
		SimplePhysicsComponent *c3 = rocketbuilder->getSimplePhysicsComponent();

		c3->setPosition(COMPONENT3_POSITION(x, y, z));
		c3->setRotation(COMPONENT3_ROTATION(rX, rY, rZ, rW));

		ObjectsManager::registerObject(rocketbuilder);
		ObjectsManager::create(rocketbuilder);
	}
}

void StaticObjectsDB::spawnNPCs() {
	Logger::log("SODB", "LOAD", "Load Non Player Characters");
	std::string str = "SELECT lot,zoneId,posX,posY,posZ,rotX,rotY,rotZ,rotW FROM npcs;";
	sqdb::Statement stm = Query(str);

	while (stm.Next()) {
		unsigned long objectLOT = stringToLOT(stm.GetField(0).GetString());

		int zone = stm.GetField(1).GetInt();
		double x = stm.GetField(2).GetDouble();
		double y = stm.GetField(3).GetDouble();
		double z = stm.GetField(4).GetDouble();
		double rX = stm.GetField(5).GetDouble();
		double rY = stm.GetField(6).GetDouble();
		double rZ = stm.GetField(7).GetDouble();
		double rW = stm.GetField(8).GetDouble();

		NPCObject * npc = new NPCObject(objectLOT, zone);
		SimplePhysicsComponent *c3 = npc->getSimplePhysicsComponent();

		c3->setPosition(COMPONENT3_POSITION(x, y, z));
		c3->setRotation(COMPONENT3_ROTATION(rX, rY, rZ, rW));

		ObjectsManager::registerObject(npc);
		ObjectsManager::create(npc);
	}
}

void StaticObjectsDB::spawnChoicebuilds() {
	Logger::log("SODB", "LOAD", "Load Quickbuilds");
	std::string str = "SELECT * FROM choicebuilds";
	/*sqdb::Statement stm = Query(str);

	while (stm.Next()) {
		unsigned long objectLOT = stm.GetField(1).GetLongLong();
		unsigned long targetLOT = stm.GetField(2).GetLongLong();
		int zone = stm.GetField(3).GetInt();
		double x = stm.GetField(4).GetDouble();
		double y = stm.GetField(5).GetDouble();
		double z = stm.GetField(6).GetDouble();
		double rX = stm.GetField(7).GetDouble();
		double rY = stm.GetField(8).GetDouble();
		double rZ = stm.GetField(9).GetDouble();
		double rW = stm.GetField(10).GetDouble();

		EnvironmentalObject * qb = new EnvironmentalObject(targetLOT, zone);

		ObjectsManager::registerObject(qb);
		ObjectsManager::create(qb);

		EnvironmentalObject * quickbuild = new EnvironmentalObject(objectLOT, zone);
		PhantomPhysicsComponent * ppc = quickbuild->getPhantomPhysicsComponent();
		ppc->setPosition(COMPONENT40_POSITION(x, y, z));
		ppc->setRotation(COMPONENT40_ROTATION(rX, rY, rZ, rW));
		quickbuild->target = qb->getObjectID();

		ObjectsManager::registerObject(quickbuild);
		ObjectsManager::create(quickbuild);
	}*/
	unsigned long objectLOT = 6604;
	unsigned long targetLOT = 7796;
	int zone = 1200;
	double x = -350.726409912109;
	double y = 285.451385498047;
	double z = 74.3901977539062;
	double rX = 0;
	double rY = 0;
	double rZ = 0;
	double rW = 1;

	EnvironmentalObject * qb = new EnvironmentalObject(targetLOT, zone);

	ObjectsManager::registerObject(qb);
	ObjectsManager::create(qb);

	GenericObject * quickbuild = new GenericObject(objectLOT, zone, COMPONENT1_POSITION(x,y,z), COMPONENT1_ROTATION(rX,rY,rZ,rW), COMPONENT1_VELOCITY(), COMPONENT1_VELOCITY_ANGULAR());

	//quickbuild->target = qb->getObjectID();

	ObjectsManager::registerObject(quickbuild);
	ObjectsManager::create(quickbuild);
}

void StaticObjectsDB::loadAll() {
	std::ostringstream st;
	st << "SELECT * FROM npcs"; //deprecated!
	int state = mysql_query(Database::getConnection(), st.str().c_str());
	if (state == 0) {
		//okay
		MYSQL_RES *result = mysql_store_result(Database::getConnection());
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(result)) != NULL) {

			NPCObject * npc = new NPCObject(stol(row[1]), stol(row[2]));

			SimplePhysicsComponent *c3 = npc->getSimplePhysicsComponent();

			COMPONENT3_POSITION pos = COMPONENT3_POSITION(stof(row[3]), stof(row[4]), stof(row[5]));
			COMPONENT3_ROTATION rot = COMPONENT3_ROTATION(stof(row[6]), stof(row[7]), stof(row[8]), stof(row[9]));

			c3->setPosition(pos);
			c3->setRotation(rot);

			ObjectsManager::registerObject(npc);
			ObjectsManager::create(npc);

		}
	}

	StaticObjectsDB::spawnNPCs();
	StaticObjectsDB::spawnSmashables();
	StaticObjectsDB::spawnCarBuilders();
	StaticObjectsDB::spawnRocketBuilders();
	StaticObjectsDB::spawnChoicebuilds();

	Logger::log("WRLD", "LOAD", "Done with initializing!");
}