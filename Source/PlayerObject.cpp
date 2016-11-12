#include "Database.h"
#include "PlayerObject.h"
#include "Replica.h"

#include "RakNet\ReplicaManager.h"
#include "AccountsDB.h"
#include "Worlds.h"

#include "Logger.h"
#include "UtfConverter.h"
#include "Characters.h"
#include <map>

//extern ReplicaManager replicaManager;
//extern std::map<SystemAddress, ZoneId> Player;

PlayerObject::PlayerObject(long long objid, std::wstring name){
	this->objid = objid;
	
	int rank = CharactersTable::getCharacterInfo(objid).info.cloaked;
	
	this->name = name.append(Characters::GetCharacterSubfix(objid));
	
	this->LOT = 1UL;

	//The order is VERY IMPORTANT for the packet: 1,7,4,17,9,2,107
	this->addComponent(new ControllablePhysicsComponent());
	this->addComponent(new DestructibleComponent());
	this->addComponent(new CharacterComponent());
	this->addComponent(new InventoryComponent());
	this->addComponent(new SkillComponent());
	this->addComponent(new RenderComponent());
	this->addComponent(new Index36Component());

	setRocket(L"1:14695;1:14696;1:14697;");

	ObjectsManager::registerObject(this);
}

void PlayerObject::setRocket(std::wstring ldfString) {
	this->getComponent4()->setData8(ldfString);
}

ControllablePhysicsComponent *PlayerObject::getComponent1(){ return (ControllablePhysicsComponent *) this->getComponent(1); }
DestructibleComponent *PlayerObject::getComponent7(){ return (DestructibleComponent *) this->getComponent(7); }
CharacterComponent *PlayerObject::getComponent4(){ return (CharacterComponent *) this->getComponent(4); }
InventoryComponent *PlayerObject::getComponent17(){ return (InventoryComponent *) this->getComponent(17); }
SkillComponent *PlayerObject::getComponent9(){ return (SkillComponent *) this->getComponent(9); }
RenderComponent *PlayerObject::getComponent2(){ return (RenderComponent *) this->getComponent(2); }
Index36Component *PlayerObject::getComponent107(){ return (Index36Component *) this->getComponent(107); }