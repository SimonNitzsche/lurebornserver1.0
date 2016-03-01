#pragma once

#include "ReplicaObject.h"
#include "Replica.h"
#include "SQLiteDatabase.h"
#include "SpawnerObject.h"

#include "SQLite\sqdb.h"

#include <string>

class GenericObject : public ReplicaObject {
private:
	SpawnerObject *spawnerObject = NULL;

	unsigned long zoneID = 0; // The ID of the user's zone

	bool isScriptAdded = false;
	bool isSimplyPhysics = false; // isSimplePhysics = true means the object has a SimplePhysics component. Otherwise it has PhantomPhysics
	bool isDestructible = false; // Determines whether or not the object has a DestructibleComponent
	bool isScriptedActivityAdded = false; // Some have this, some don't

	void initializeObject(unsigned long lot);
public:
	GenericObject(unsigned long lot, unsigned long currentZone);
	~GenericObject();

	long long getObjectID();
	long long getSpawnerObjectID();
	unsigned long getLOT();

	SimplePhysicsComponent *getSimplePhysicsComponent(); // Returns the SimplePhysicsComponent if isSimplePhysics = true, NULL if false
	PhantomPhysicsComponent *getPhantomPhysicsComponent(); // Returns the PhantomPhysicsComponent if isSimplePhysics = false, NULL if true
	DestructibleComponent *getDestructibleComponent(); // Returns the DestructibleComponent is isDestructible = true, NULL if not
	ScriptComponent *getScriptComponent(); // Returns the ScriptComponent if isScriptAdded = true, NULL if not
	ScriptedActivityComponent *getScriptedActivityComponent(); // Returns the ScriptedActivityComponent is isScriptedActivity = true, NULL if not
	RenderComponent *getRenderComponent(); // Returns the RenderComponent of the object

	bool didUseSimplePhysics(); // Returns isSimplePhysics
	bool didAddDestructibleComponent(); // Returns isDestructible
	bool didAddScript(); // Returns isScriptAdded
	bool didAddScriptedActivity(); // Returns isScriptedActivityAdded
};
