#include "LootObject.h"

LootObject::LootObject(unsigned long LOT, unsigned long objectID) {
	this->objid = objectID;
	this->LOT = LOT;
	this->isConstructed = false;

	ObjectsManager::registerObject(this);
}

LootObject::~LootObject() {

}

unsigned long long LootObject::getOwnerID(){ return this->ownerID; }
void LootObject::setOwnerID(unsigned long long ownerID){ this->ownerID = ownerID; }