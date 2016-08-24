#include "ObjectID.h"

#include "Worlds.h"

#include <iostream>
#include <climits>
#include <ctime>

using namespace std;

unsigned long ObjectID::mState = rand();

unsigned long long ObjectID::generateObjectID() {
	unsigned long long most = 288399999999999999;
	unsigned long long least = 288300000000000000;
	
	unsigned long long objectID = getRandomInRange(least, most);
	//cout << "Generated Object ID = " << objectID << endl;

	if (validateObjectID(objectID) == false) {
		return generateObjectID();
	}

	return objectID;
}

unsigned long long ObjectID::generateSpawnerID() {
	unsigned long long most = 799999999999999999;
	unsigned long long least = 700000000000000000;

	unsigned long long spawnerID = getRandomInRange(least, most);
	//cout << "Generated Spawner ID = " << spawnerID << endl;

	if (!validateObjectID(spawnerID)) {
		return generateSpawnerID();
	}

	return spawnerID;
}

bool ObjectID::validateObjectID(unsigned long long objid) {
	ReplicaObject *object = ObjectsManager::getObjectByID(objid);

	bool isObjectNULL = (object == NULL);
	return isObjectNULL;
}

unsigned long long ObjectID::generatePlayerID() {
	// TODO: Use this later to generate PlayerID

	return 0;
}

// Function taken from http://oroboro.com/large-random-in-range/
unsigned long long ObjectID::getRandomInRange(unsigned long long low, unsigned long long high) {
	unsigned long long diff = high - low;
	if (diff < ULONG_MAX)
		return getRandomInRange((unsigned long long)low, (unsigned long long)high);

	setSeed(time(NULL) + rand());

	unsigned long long rlow = getRandom();
	unsigned long long rhigh = getRandom();
	unsigned long long vlow = diff & 0xFFFFFFFF;
	unsigned long long vhigh = diff >> 32;

	unsigned long long r = ((rhigh * vlow) >> 32);
					   r += ((rlow * vhigh) >> 32);
					   r += (rhigh * vhigh);
					   r += low;
	return r;
}