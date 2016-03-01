#include "SpawnerObject.h"
#include "Worlds.h"

SpawnerObject::SpawnerObject(unsigned long long spawnerID) {
	this->objid = spawnerID;
	this->isConstructed = false;

	ObjectsManager::registerObject(this);
}

SpawnerObject::~SpawnerObject() {
	
}