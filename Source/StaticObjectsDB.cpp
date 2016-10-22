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

bool StaticObjectsDB::initDone = true;

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

std::string getPercString(const std::string& msg, int perc) {
	std::stringstream ss;
	ss << "\r" << Logger::getLogStream("SODB", "LOAD").str()<<msg;
	ss << " " << std::to_string(perc) << "%"<<std::flush;
	return ss.str();
}

double getPercOf(double cur, double max) {
	if (max>0){
		return (100.0 / (double)max)*(double)cur;
	}
	throw new Exception("Can't divide trough 0!");
	return 100;
}

sqdb::Statement StaticObjectsDB::Query(std::string querystr) {
	return SQLiteDatabase::Query("../Env/staticObjects.sqlite", querystr);
}

int CountRows(std::string tablename) {
	std::string t = "SELECT Count(*) FROM `" + tablename + "`;";
	sqdb::Statement sm = StaticObjectsDB::Query(t);
	sm.Next();
	return sm.GetField(0).GetInt();
}

void StaticObjectsDB::spawnSmashables() {
	initDone = false;
	std::string fs = "Load Smashables...";
	cout << getPercString(fs, 0);
	int lProg = 0;
	double ct = CountRows("smashables");
	double it = 0;
	std::string str = "SELECT lot,zoneId,posX,posY,posZ,rotX,rotY,rotZ,rotW FROM smashables;";
	sqdb::Statement stm = Query(str);
	while (stm.Next()) {
		it++;
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
		double nProg = getPercOf(it, ct);
		//if (nProg > lProg) {
		cout << getPercString(fs, nProg);
			//lProg = nProg;
		//}
	}
	cout << getPercString(fs, 100) << "\n";
	initDone = true;
}

void StaticObjectsDB::spawnCarBuilders() {
	initDone = false;
	std::string fs = "Load CarBuilders...";
	cout << getPercString(fs, 0);
	int lProg = 0;
	int ct = CountRows("carbuilders");
	int it = 0;
	std::string str = "SELECT lot,zoneId,posX,posY,posZ,rotX,rotY,rotZ,rotW FROM carbuilders;";
	sqdb::Statement stm = Query(str);

	while (stm.Next()) {
		it++;
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
		int nProg = getPercOf(it, ct);
		if (nProg > lProg) {
			cout << getPercString(fs, nProg);
			lProg = nProg;
		}
	}
	cout << getPercString(fs, 100) << "\n";
	initDone = true;
}

void StaticObjectsDB::spawnRocketBuilders() {
	initDone = false;
	std::string fs = "Load RocketBuilders...";
	cout << getPercString(fs, 0);
	int lProg = 0;
	int ct = CountRows("rocketbuilders");
	int it = 0;
	std::string str = "SELECT lot,zoneId,posX,posY,posZ,rotX,rotY,rotZ,rotW FROM rocketbuilders;";
	sqdb::Statement stm = Query(str);

	while (stm.Next()) {
		it++;
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
		int nProg = getPercOf(it, ct);
		if (nProg > lProg) {
			cout << getPercString(fs, nProg);
			lProg = nProg;
		}
	}
	cout << getPercString(fs, 100) << "\n";
	initDone = true;
}

void StaticObjectsDB::spawnNPCs() {
	initDone = false;
	std::string fs = "Load NPCs...";
	cout << getPercString(fs, 0);
	int lProg = 0;
	int ct = CountRows("npcs");
	int it = 0;
	std::string str = "SELECT lot,zoneId,posX,posY,posZ,rotX,rotY,rotZ,rotW FROM npcs;";
	sqdb::Statement stm = Query(str);

	while (stm.Next()) {
		it++;
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
		int nProg = getPercOf(it, ct);
		if (nProg > lProg) {
			cout << getPercString(fs, nProg);
			lProg = nProg;
		}
	}
	cout << getPercString(fs, 100) << "\n";
	initDone = true;
}

void StaticObjectsDB::spawnChoicebuilds() {
	initDone = false;
	std::string fs = "Load Quickbuilds...";
	cout << getPercString(fs, 0);
	int lProg = 0;
	int ct = CountRows("choicebuilds");
	int it = 0;
	std::string str = "SELECT * FROM choicebuilds";
	/*sqdb::Statement stm = Query(str);

	while (stm.Next()) {
		it++;
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
		int nProg = getPercOf(it, ct);
		if (nProg > lProg) {
			cout << getPercString(fs, nProg);
			lProg = nProg;
		}
	}
	cout << getPercString(fs, 100) << "\n";*/
	unsigned long targetLOT = 6604;
	unsigned long objectLOT = 7796;
	int zone = 1200;
	double x = -362.5623779296875;
	double y = 285.451385498047;
	double z = 74.3901977539062;
	double rX = 0;
	double rY = 0.7138688564300537;
	double rZ = 0;
	double rW = 0.7002794146537781;

	//The thing wich gets built
	EnvironmentalObject * qb = new EnvironmentalObject(targetLOT, zone);

	ObjectsManager::registerObject(qb);
	ObjectsManager::create(qb);

	//The thing wich builds the thing
	GenericObject * quickbuild = new GenericObject(objectLOT, zone, COMPONENT1_POSITION(x,y,z), COMPONENT1_ROTATION(rX,rY,rZ,rW), COMPONENT1_VELOCITY(), COMPONENT1_VELOCITY_ANGULAR());

	//quickbuild->target = qb->getObjectID();

	ObjectsManager::registerObject(quickbuild);
	ObjectsManager::create(quickbuild);
	cout << getPercString(fs, 100) << "\n";
	initDone = true;
}

void StaticObjectsDB::spawnWorldObjects() {
	initDone = false;
	std::string fs = "Load World-Objects...";
	cout << getPercString(fs, 0);
	int lProg = 0;
	int ct = CountRows("worldobjects");
	int it = 0;
	std::string str = "SELECT lot,zoneId,posX,posY,posZ,rotX,rotY,rotZ,rotW FROM worldobjects;";
	sqdb::Statement stm = Query(str);

	while (stm.Next()) {
		it++;
		unsigned long objectLOT = stringToLOT(stm.GetField(0).GetString());

		int zone = stm.GetField(1).GetInt();
		double x = stm.GetField(2).GetDouble();
		double y = stm.GetField(3).GetDouble();
		double z = stm.GetField(4).GetDouble();
		double rX = stm.GetField(5).GetDouble();
		double rY = stm.GetField(6).GetDouble();
		double rZ = stm.GetField(7).GetDouble();
		double rW = stm.GetField(8).GetDouble();

		GenericObject * worldobject = new GenericObject(objectLOT, zone, COMPONENT1_POSITION(x, y, z), COMPONENT1_ROTATION(rX, rY, rZ, rW), COMPONENT1_VELOCITY(), COMPONENT1_VELOCITY_ANGULAR());

		ObjectsManager::registerObject(worldobject);
		ObjectsManager::create(worldobject);
		int nProg = getPercOf(it, ct);
		if (nProg > lProg) {
			cout << getPercString(fs, nProg);
			lProg = nProg;
		}
	}
	cout << getPercString(fs, 100) << "\n";
	initDone = true;
}

void StaticObjectsDB::spawnAmbientSound() {
	initDone = false;
	std::string fs = "Load AmbientSound...";
	cout << getPercString(fs, 0);
	int lProg = 0;
	int ct = CountRows("AmbientSound");
	int it = 0;
	std::string str = "SELECT lot,zoneId,posX,posY,posZ,rotX,rotY,rotZ,rotW FROM AmbientSound;";
	sqdb::Statement stm = Query(str);

	while (stm.Next()) {
		it++;
		unsigned long objectLOT = stringToLOT(stm.GetField(0).GetString());

		int zone = stm.GetField(1).GetInt();
		double x = stm.GetField(2).GetDouble();
		double y = stm.GetField(3).GetDouble();
		double z = stm.GetField(4).GetDouble();
		double rX = stm.GetField(5).GetDouble();
		double rY = stm.GetField(6).GetDouble();
		double rZ = stm.GetField(7).GetDouble();
		double rW = stm.GetField(8).GetDouble();

		GenericObject * ambientsound = new GenericObject(objectLOT, zone, COMPONENT1_POSITION(x, y, z), COMPONENT1_ROTATION(rX, rY, rZ, rW), COMPONENT1_VELOCITY(), COMPONENT1_VELOCITY_ANGULAR());

		ObjectsManager::registerObject(ambientsound);
		ObjectsManager::create(ambientsound);
		int nProg = getPercOf(it, ct);
		if (nProg > lProg) {
			cout << getPercString(fs, nProg);
			lProg = nProg;
		}
	}
	cout << getPercString(fs, 100) << "\n";
	initDone = true;
}

void StaticObjectsDB::spawnMissionObjects() { //GenericObjects-Table
	initDone = false;
	std::string fs = "Load Mission-Objects...";
	cout << getPercString(fs, 0);
	int lProg = 0;
	int ct = CountRows("missionobjects");
	int it = 0;
	std::string str = "SELECT lot,zoneId,mission,disappear,posX,posY,posZ,rotX,rotY,rotZ,rotW FROM missionobjects;";
	sqdb::Statement stm = Query(str);

	while (stm.Next()) {
		it++;
		unsigned long objectLOT = stringToLOT(stm.GetField(0).GetString());

		int zone = stm.GetField(1).GetInt();
		int mis = stm.GetField(2).GetInt();
		bool disappearAfterComplete = (stm.GetField(3).GetInt() == 1);
		double x = stm.GetField(4).GetDouble();
		double y = stm.GetField(5).GetDouble();
		double z = stm.GetField(6).GetDouble();
		double rX = stm.GetField(7).GetDouble();
		double rY = stm.GetField(8).GetDouble();
		double rZ = stm.GetField(9).GetDouble();
		double rW = stm.GetField(10).GetDouble();

		GenericObject * misobj = new GenericObject(objectLOT, zone, COMPONENT1_POSITION(x, y, z), COMPONENT1_ROTATION(rX, rY, rZ, rW), COMPONENT1_VELOCITY(), COMPONENT1_VELOCITY_ANGULAR());

		ObjectsManager::registerObject(misobj);
		ObjectsManager::create(misobj);
		int nProg = getPercOf(it, ct);
		if (nProg > lProg) {
			cout << getPercString(fs, nProg);
			lProg = nProg;
		}
	}
	cout << getPercString(fs, 100) << "\n";
	initDone = true;
}

void StaticObjectsDB::spawnOtherObjects() { //GenericObjects-Table
	initDone = false;
	std::string fs = "Load Undefined Objects...";
	cout << getPercString(fs, 0);
	int lProg = 0;
	int ct = CountRows("OtherObjects");
	int it = 0;
	std::string str = "SELECT lot,zoneId,posX,posY,posZ,rotX,rotY,rotZ,rotW FROM OtherObjects;";
	sqdb::Statement stm = Query(str);

	while (stm.Next()) {
		it++;
		unsigned long objectLOT = stringToLOT(stm.GetField(0).GetString());

		int zone = stm.GetField(1).GetInt();
		double x = stm.GetField(2).GetDouble();
		double y = stm.GetField(3).GetDouble();
		double z = stm.GetField(4).GetDouble();
		double rX = stm.GetField(5).GetDouble();
		double rY = stm.GetField(6).GetDouble();
		double rZ = stm.GetField(7).GetDouble();
		double rW = stm.GetField(8).GetDouble();

		GenericObject * other = new GenericObject(objectLOT, zone, COMPONENT1_POSITION(x, y, z), COMPONENT1_ROTATION(rX, rY, rZ, rW), COMPONENT1_VELOCITY(), COMPONENT1_VELOCITY_ANGULAR());

		ObjectsManager::registerObject(other);
		ObjectsManager::create(other);
		int nProg = getPercOf(it, ct);
		if (nProg > lProg) {
			cout << getPercString(fs, nProg);
			lProg = nProg;
		}
	}
	cout << getPercString(fs, 100) << "\n";
	initDone = true;
}

void StaticObjectsDB::loadAll() {
	initDone = false;
	std::ostringstream st;
	std::string fs="Load old table (deprecated)...";
	cout << getPercString(fs, 0);
	int lProg=0;
	int ct = 0;
	int it = 0;
	st << "SELECT * FROM npcs"; //deprecated!
	int state = mysql_query(Database::getConnection(), st.str().c_str());
	if (state == 0) {
		//okay
		MYSQL_RES *result = mysql_store_result(Database::getConnection());
		MYSQL_ROW row;
		ct=mysql_num_rows(result);
		while ((row = mysql_fetch_row(result)) != NULL) {
			it++;
			NPCObject * npc = new NPCObject(stol(row[1]), stol(row[2]));

			SimplePhysicsComponent *c3 = npc->getSimplePhysicsComponent();

			COMPONENT3_POSITION pos = COMPONENT3_POSITION(stof(row[3]), stof(row[4]), stof(row[5]));
			COMPONENT3_ROTATION rot = COMPONENT3_ROTATION(stof(row[6]), stof(row[7]), stof(row[8]), stof(row[9]));

			c3->setPosition(pos);
			c3->setRotation(rot);

			ObjectsManager::registerObject(npc);
			ObjectsManager::create(npc);

			int nProg = getPercOf(it, ct);
			if (nProg > lProg) {
				cout << getPercString(fs, nProg);
				lProg = nProg;
			}
		}
		cout << getPercString(fs, 100) << "\n";
	}
	initDone = true;
	
	StaticObjectsDB::spawnNPCs();
	StaticObjectsDB::spawnSmashables();
	StaticObjectsDB::spawnCarBuilders();
	StaticObjectsDB::spawnRocketBuilders();
	StaticObjectsDB::spawnChoicebuilds();
	StaticObjectsDB::spawnWorldObjects();
	StaticObjectsDB::spawnAmbientSound();
	StaticObjectsDB::spawnMissionObjects();
	StaticObjectsDB::spawnOtherObjects();

	Logger::log("WRLD", "LOAD", "Done with initialisation!");
}