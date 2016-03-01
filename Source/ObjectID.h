#pragma once

class ObjectID {
private:
	static unsigned long mState;
	static void setSeed(unsigned long seed){ if (!seed) seed = 0x333; mState = seed | 0x1; }
	static unsigned long getRandom(){ mState *= 69069; return mState; }

	static unsigned long long getRandomInRange(unsigned long long low, unsigned long long high);

public:
	static unsigned long long generateObjectID();
	static unsigned long long generateSpawnerID();
	static unsigned long long generatePlayerID();

	static bool validateObjectID(unsigned long long objid);
};