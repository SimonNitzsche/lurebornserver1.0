#pragma once

#include "Worlds.h"
#include "ReplicaObject.h"

class LootObject : public ReplicaObject {
private:
	unsigned long long ownerID;
public:
	LootObject(unsigned long LOT, unsigned long objectID);
	~LootObject();

	unsigned long long getOwnerID();
	void setOwnerID(unsigned long long ownerID);
};