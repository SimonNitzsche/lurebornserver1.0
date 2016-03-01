#pragma once

#include "ReplicaData.h"
#include "ReplicaComponents.h"
#include "ReplicaObject.h"
#include "SpawnerObject.h"
#include "LootObject.h"

#include "RakNet\Replica.h"

#include <string>

class NPCObject : public ReplicaObject {
private:
	SystemAddress clientIP;
	SpawnerObject *spawnerObject = NULL;

	unsigned long zoneID = 0;
	bool isCreated = false; // Determines whether or not the object is created
	bool isVendor = false; // BOOL for whether a VendorComponent is added
	bool isScriptAdded = false; // BOOL for whether a ScriptComponent is added
	bool isSkillAdded = false; // BOOL for whether a SkillComponent is added
	bool isInventoryAdded = false; // BOOL for whether an InventoryComponent is added

	void initializeNPC(unsigned long lot);

	std::vector<LootObject *> equippedItems;
public:
	NPCObject(unsigned long lot, unsigned long currentZone); // Will call initializeNPC()
	~NPCObject();

	void deleteSubObjects();
	long long getObjectID();
	long long getSpawnerObjectID();
	unsigned long getLOT();

	SimplePhysicsComponent *getSimplePhysicsComponent();
	DestructibleComponent *getDestructibleComponent();
	InventoryComponent *getInventoryComponent(); // Returns the InventoryComponent if isInventoryAdded = true, NULL if not
	ScriptComponent *getScriptComponent(); // Returns the ScriptComponent if isVender = true, NULL if not
	SkillComponent *getSkillComponent(); // Return the SkillComponent if isSkillAdded = true, NULL if not
	VendorComponent *getVendorComponent(); // Return the VendorComponent if isScriptAdded = true, NULL if not
	RenderComponent *getRenderComponent();

	bool didAddInventory(); // Returns isInventoryAdded
	bool didAddVendorComponent(); // Returns isVendor
	bool didAddScript(); // Returns isScriptAdded
	bool didAddSkill(); // Returns isSkillAdded
};