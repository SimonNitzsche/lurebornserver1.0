#pragma once

//#include "Common.h"

#include <iostream>

// This is the FactionID enum that contains faction
// names and values
enum FactionId : unsigned short {
	NO_FACTION = -1,
	VENTURE_LEAGUE,
	ASSEMBLY,
	PARADOX,
	SENTINEL,
};

//The mission wich you get after choosing a faction (This one sets your faction clientside)
enum FactionMission : unsigned short {
	FACTIONMIS_ASSEMBLY = 588,
	FACTIONMIS_VENTURE,
	FACTIONMIS_PARADOX,
	FACTIONMIS_SENTINEL
};

// Prints Faction name
std::ostream& operator<<(std::ostream& out, const FactionId value);

// This is the Faction struct that stores a FactionId
struct Faction {
	FactionId faction;
};