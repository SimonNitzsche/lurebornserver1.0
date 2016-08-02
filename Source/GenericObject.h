#pragma once

#include "ReplicaObject.h"
#include "Replica.h"
#include "SQLiteDatabase.h"
#include "SpawnerObject.h"
#include "LootObject.h"
#include "SQLite\sqdb.h"

#include <string>

class GenericObject : public ReplicaObject {
private:
	SpawnerObject *spawnerObject = NULL;

	unsigned long zoneID = 0; // The ID of the user's zone
	
	std::vector<LootObject *> equippedItems;

	void initializeObject(unsigned long lot, COMPONENT1_POSITION pos, COMPONENT1_ROTATION rot, COMPONENT1_VELOCITY vel, COMPONENT1_VELOCITY_ANGULAR vel_ang);
public:
	GenericObject(unsigned long lot, unsigned long currentZone, COMPONENT1_POSITION pos, COMPONENT1_ROTATION rot, COMPONENT1_VELOCITY vel, COMPONENT1_VELOCITY_ANGULAR vel_ang);
	~GenericObject();

	long long getObjectID();
	long long getSpawnerObjectID();
	unsigned long getLOT();

	SimplePhysicsComponent *getSimplePhysicsComponent(); // Returns the SimplePhysicsComponent if isSimplePhysics = true, NULL if false
	ControllablePhysicsComponent *getControllablePhysicsComponent();
	InventoryComponent *getInventoryComponent();
	PhantomPhysicsComponent *getPhantomPhysicsComponent(); // Returns the PhantomPhysicsComponent if isSimplePhysics = false, NULL if true
	DestructibleComponent *getDestructibleComponent(); // Returns the DestructibleComponent is isDestructible = true, NULL if not
	ScriptComponent *getScriptComponent(); // Returns the ScriptComponent if isScriptAdded = true, NULL if not
	ScriptedActivityComponent *getScriptedActivityComponent(); // Returns the ScriptedActivityComponent is isScriptedActivity = true, NULL if not
	RenderComponent *getRenderComponent(); // Returns the RenderComponent of the object

	bool didUseSimplePhysics(); // Returns isSimplePhysics
	bool didAddDestructibleComponent(); // Returns isDestructible
	bool didAddScript(); // Returns isScriptAdded
	bool didAddScriptedActivity(); // Returns isScriptedActivityAdded

	COMPONENT1_POSITION getPosition();
};
