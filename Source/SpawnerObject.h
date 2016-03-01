#pragma once

#include "ReplicaObject.h"

class SpawnerObject : public ReplicaObject {
public:
	SpawnerObject(unsigned long long spawnerID);
	~SpawnerObject();

	void deleteSubObjects();
};