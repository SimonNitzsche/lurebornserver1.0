#include "Common.h"

class CDClientDB {
public:
	static std::string getObjectType(unsigned long LOT);
	static unsigned long getComponentID(unsigned long LOT, unsigned long componentType);
	//static std::string getEquipLocation(unsigned long componentID);
	static unsigned long getItemType(unsigned long componentID);
	static unsigned long getSkillID(unsigned long LOT, unsigned long castOnType);
	static bool isAnimationValid(std::string animationID);
	static std::wstring getAnimationOfEmote(unsigned short emoteID);
	static std::vector<unsigned long> CDClientDB::getInventoryComponentData(unsigned long LOT);
	static std::vector<long> getDestructibleComponentData(unsigned long LOT);
};