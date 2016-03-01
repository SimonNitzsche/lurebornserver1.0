#pragma once

#include "ReplicaData.h"
#include "ReplicaComponents.h"
#include "ReplicaObject.h"
#include "SpawnerObject.h"
#include "LootObject.h"
#include "AccountsDB.h"

#include "RakNet\Replica.h"

#include <string>

class DroppedLootObject : public ReplicaObject {
private:
	SystemAddress clientIP;
	SpawnerObject *spawnerObject = NULL;

	unsigned long zoneID = 0;

	void initializeDroppedLootObject(unsigned long lot, SessionInfo *s);

public:
	DroppedLootObject(unsigned long lot, unsigned long currentZone, SessionInfo *s); // Will call initializeNPC()
	~DroppedLootObject();

	long long getObjectID();
	unsigned long getLOT();
};