#pragma once

#include "ReplicaData.h"
#include "ReplicaComponents.h"
#include "ReplicaObject.h"
#include "SpawnerObject.h"
#include "LootObject.h"

#include "RakNet\Replica.h"

#include <string>

class SmashableObject : public ReplicaObject {
private:
	SystemAddress clientIP;
	SpawnerObject *spawnerObject = NULL;

	unsigned long zoneID = 0;
	bool isCreated = false; // Determines whether or not the object is created

	void initializeSmashable(unsigned long lot);

public:
	SmashableObject(unsigned long lot, unsigned long currentZone); // Will call initializeNPC()
	~SmashableObject();

	void deleteSubObjects();
	long long getObjectID();
	long long getSpawnerObjectID();
	unsigned long getLOT();

	SimplePhysicsComponent *getSimplePhysicsComponent();
	DestructibleComponent *getDestructibleComponent();
	RenderComponent *getRenderComponent();
};