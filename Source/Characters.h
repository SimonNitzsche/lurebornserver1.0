#pragma once
#include "RakNet\BitStream.h"
#include <stdlib.h>
#include <string>

class Characters{
public:
	static bool CreateCharacter(RakNet::BitStream *packet, SystemAddress address, unsigned int accountid);
	static void DeleteCharacter(unsigned int accountid, long long charid);
	static std::wstring GetCharacterSubfix(long long charid);
};