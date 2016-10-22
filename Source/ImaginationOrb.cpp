#include "ImaginationOrb.h"

ImaginationOrb::ImaginationOrb(unsigned long LOT, unsigned short collectibleID, long long objectID) {

	//Add Components (2,7,23,40)
	this->addComponent(new RenderComponent());
	this->addComponent(new DestructibleComponent());
	this->addComponent(new CollectibleComponent(collectibleID));
	this->addComponent(new PhantomPhysicsComponent());
}