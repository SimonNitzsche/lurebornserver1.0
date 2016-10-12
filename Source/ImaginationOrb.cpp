#include "ImaginationOrb.h"

ImaginationOrb::ImaginationOrb(unsigned long LOT, unsigned long objectID) {

	//Add Components (2,7,23,40)
	this->addComponent(new RenderComponent());
	this->addComponent(new DestructibleComponent());
	//this->addComponent(new CollectibleComponent());
	this->addComponent(new PhantomPhysicsComponent());
}