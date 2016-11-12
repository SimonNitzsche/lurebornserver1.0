#include "GameMessages.h"
#include "Common.h"
#include "WorldServer.h"
#include "AccountsDB.h"
#include "Social.h"
#include "WorldConnection.h"
#include "CDClientDB.h"
#include "ChatCommands.h"
#include "Account.h"
#include "Worlds.h"
#include "PlayerObject.h"
#include "LootObject.h"
#include "InventoryDB.h"
#include "UtfConverter.h"
#include "Logger.h"
#include "Packet.h"
#include "Characters.h"
#include "worldLoop.h"
#include "Vector3.h"
#include "GenericObject.h"
#include "Interact.h"

#include <map>
#include <sstream>

#include <chrono>
#include <thread>

std::map<long long, ObjectInformation> Objects;
std::map<long long, ZoneId> LaunchPads;
bool isObjectsInitialized = false;

void initializeObject(ObjectInformation oinfo){
	Objects[oinfo.objid] = oinfo;
}

void initializeObject(long long objid, ZoneId zone, ObjectType type, std::string comment){
	Objects[objid] = ObjectInformation(objid, zone, type, comment);
}

void initializeLaunchPad(long long objid, ZoneId zone, ZoneId target, std::string comment = ""){
	Objects[objid] = ObjectInformation(objid, zone, ObjectType::LaunchPad, comment);
	LaunchPads[objid] = target;
}

void initializeObjects(){
	//Collecting all objectids from every zone
	initializeLaunchPad(70368746922751, ZoneId::CRUX_PRIME, ZoneId::NEXUS_TOWER);

	initializeLaunchPad(70368747133960, ZoneId::AVANT_GROVE, ZoneId::AVANT_GARDENS);
	initializeObject(70368747133989, ZoneId::AVANT_GROVE, ObjectType::PostBox, "Hub"); //9

	initializeLaunchPad(70368747143926, ZoneId::NIMBUS_ISLE, ZoneId::NIMBUS_STATION);
	initializeObject(70368747143977, ZoneId::NIMBUS_ISLE, ObjectType::PostBox, "Hub");

	initializeLaunchPad(70368747150591, ZoneId::GNARLED_FOREST, ZoneId::CHANTEY_SHANTEY);
	initializeLaunchPad(70368747150592, ZoneId::GNARLED_FOREST, ZoneId::NIMBUS_STATION, "Pirate Camp");
	initializeLaunchPad(70368747150593, ZoneId::GNARLED_FOREST, ZoneId::NIMBUS_STATION);
	initializeObject(70368747150602, ZoneId::GNARLED_FOREST, ObjectType::Minigame, "Cannon Cove Shooting Gallery"); //9
	initializeObject(70368747150603, ZoneId::GNARLED_FOREST, ObjectType::Minigame, "Keelhaul Canyon Racetrack"); //9
	initializeObject(70368747150623, ZoneId::GNARLED_FOREST, ObjectType::Plaque, "Brig Rock Tunnel"); //9

	initializeObject(70368747161296, ZoneId::FORBIDDEN_VALLEY_DRAGON, ObjectType::Minigame, "Leave Minigame"); //9

	initializeLaunchPad(70368747162328, ZoneId::RAVEN_BLUFF, ZoneId::FORBIDDEN_VALLEY);
	initializeObject(70368747162358, ZoneId::RAVEN_BLUFF, ObjectType::PostBox, "Hub"); //9

	initializeLaunchPad(70368747172233, ZoneId::LEGO_CLUB, ZoneId::GNARLED_FOREST);
	initializeLaunchPad(70368747172234, ZoneId::LEGO_CLUB, ZoneId::FORBIDDEN_VALLEY);
	initializeLaunchPad(70368747172235, ZoneId::LEGO_CLUB, ZoneId::AVANT_GARDENS);
	initializeObject(70368747172236, ZoneId::LEGO_CLUB, ObjectType::PostBox, "Inside"); //9

	initializeLaunchPad(70368747172584, ZoneId::MOON_BASE, ZoneId::STARBASE_3001);
	initializeLaunchPad(70368747173214, ZoneId::DEEP_FREEZE, ZoneId::STARBASE_3001);
	initializeLaunchPad(70368747194845, ZoneId::ROBOT_CITY, ZoneId::STARBASE_3001);

	initializeLaunchPad(70368747531976, ZoneId::NIMBUS_ROCK, ZoneId::NIMBUS_STATION);
	initializeObject(70368747531998, ZoneId::NIMBUS_ROCK, ObjectType::PostBox, "Hub"); //9

	initializeLaunchPad(70368747534285, ZoneId::CHANTEY_SHANTEY, ZoneId::GNARLED_FOREST);
	initializeObject(70368747534312, ZoneId::CHANTEY_SHANTEY, ObjectType::PostBox, "Hub"); //9

	initializeObject(70368747635921, ZoneId::STARBASE_3001, ObjectType::LaunchPad, "Rocket Laucher"); //9
	initializeObject(70368747636011, ZoneId::STARBASE_3001, ObjectType::Binoculars, "Entrance Right"); //9
	initializeObject(70368747636012, ZoneId::STARBASE_3001, ObjectType::Binoculars, "Entrance Left"); //9

	initializeLaunchPad(70368747873772, ZoneId::VENTURE_EXPLORER_RETURN, ZoneId::AVANT_GARDENS, "2");
	initializeObject(70368747873850, ZoneId::VENTURE_EXPLORER_RETURN, ObjectType::Plaque, "Captain's Log"); //9
	initializeObject(70368747873894, ZoneId::VENTURE_EXPLORER_RETURN, ObjectType::Binoculars, "Start Area Left"); //9
	initializeObject(70368747873895, ZoneId::VENTURE_EXPLORER_RETURN, ObjectType::Binoculars, "Start Area Right"); //9
	initializeObject(70368747873896, ZoneId::VENTURE_EXPLORER_RETURN, ObjectType::Binoculars, "Launch Area"); //9
	initializeObject(70368747873897, ZoneId::VENTURE_EXPLORER_RETURN, ObjectType::Binoculars, "Behind Bob"); //9
	initializeObject(70368747873898, ZoneId::VENTURE_EXPLORER_RETURN, ObjectType::Binoculars, "Platform"); //9
	initializeObject(70368747873906, ZoneId::VENTURE_EXPLORER_RETURN, ObjectType::Plaque, "Coded Message From: Beck Strongheart"); //9
	initializeObject(70368747873908, ZoneId::VENTURE_EXPLORER_RETURN, ObjectType::Plaque, "Ship's Registry"); //9

	initializeLaunchPad(70368747874563, ZoneId::NIMBUS_STATION, ZoneId::FORBIDDEN_VALLEY);
	initializeLaunchPad(70368747874564, ZoneId::NIMBUS_STATION, ZoneId::GNARLED_FOREST);
	initializeLaunchPad(70368747874571, ZoneId::NIMBUS_STATION, ZoneId::NIMBUS_ISLE, "Brick Annex");
	initializeLaunchPad(70368747874572, ZoneId::NIMBUS_STATION, ZoneId::NIMBUS_ROCK, "Brick Annex");
	initializeLaunchPad(70368747874573, ZoneId::NIMBUS_STATION, ZoneId::NEXUS_TOWER, "Nexus Checkpoint");
	initializeLaunchPad(70368747874574, ZoneId::NIMBUS_STATION, ZoneId::PET_COVE, "Brick Annex");
	initializeLaunchPad(70368747874575, ZoneId::NIMBUS_STATION, ZoneId::AVANT_GARDENS);
	initializeObject(70368747874576, ZoneId::NIMBUS_STATION, ObjectType::Minigame, "Vertigo Loop Racetrack, Nexus Checkpoint");
	initializeObject(70368747874579, ZoneId::NIMBUS_STATION, ObjectType::Portal, "To LEGO Club"); //9
	initializeObject(70368747874580, ZoneId::NIMBUS_STATION, ObjectType::Minigame, "Battle of Nimbus Station"); //9
	
	initializeLaunchPad(70368747893770, ZoneId::AVANT_GARDENS, ZoneId::VENTURE_EXPLORER_RETURN);
	initializeLaunchPad(70368747893771, ZoneId::AVANT_GARDENS, ZoneId::NIMBUS_STATION);
	initializeLaunchPad(70368747893772, ZoneId::AVANT_GARDENS, ZoneId::BLOCK_YARD);
	initializeLaunchPad(70368747893773, ZoneId::AVANT_GARDENS, ZoneId::AVANT_GROVE);
	initializeObject(70368747893774, ZoneId::AVANT_GARDENS, ObjectType::Minigame, "Survival Mini-Game"); //9
	initializeObject(70368747893777, ZoneId::AVANT_GARDENS, ObjectType::Minigame, "Spider-Queen Battle"); //9
	
	initializeObject(70368747895179, ZoneId::VENTURE_EXPLORER, ObjectType::Machine, "Launch Area Right"); //9
	initializeLaunchPad(70368747895189, ZoneId::VENTURE_EXPLORER, ZoneId::AVANT_GARDENS, "4");
	initializeLaunchPad(70368747895190, ZoneId::VENTURE_EXPLORER, ZoneId::AVANT_GARDENS, "2");
	initializeLaunchPad(70368747895191, ZoneId::VENTURE_EXPLORER, ZoneId::AVANT_GARDENS, "3");
	initializeLaunchPad(70368747895192, ZoneId::VENTURE_EXPLORER, ZoneId::AVANT_GARDENS, "1");
	initializeObject(70368747895218, ZoneId::VENTURE_EXPLORER, ObjectType::Plaque, "Captains Log"); //9
	initializeObject(70368747895266, ZoneId::VENTURE_EXPLORER, ObjectType::Binoculars, "Start Area Left"); //9
	initializeObject(70368747895267, ZoneId::VENTURE_EXPLORER, ObjectType::Binoculars, "Start Area Right"); //9
	initializeObject(70368747895268, ZoneId::VENTURE_EXPLORER, ObjectType::Binoculars, "Launch Area"); //9
	initializeObject(70368747895269, ZoneId::VENTURE_EXPLORER, ObjectType::Binoculars, "Behind Bob"); //9
	initializeObject(70368747895270, ZoneId::VENTURE_EXPLORER, ObjectType::Binoculars, "Platform"); //9
	initializeObject(70368747895311, ZoneId::VENTURE_EXPLORER, ObjectType::Plaque, "Coded Message From: Beck Strongheart"); //9
	initializeObject(70368747895314, ZoneId::VENTURE_EXPLORER, ObjectType::Plaque, "Ship's Registry"); //9

	initializeLaunchPad(70368747895359, ZoneId::FORBIDDEN_VALLEY, ZoneId::RAVEN_BLUFF, "Main Gate");
	initializeLaunchPad(70368747895360, ZoneId::FORBIDDEN_VALLEY, ZoneId::NIMBUS_STATION, "Great Tree");
	initializeLaunchPad(70368747895361, ZoneId::FORBIDDEN_VALLEY, ZoneId::NIMBUS_STATION, "Main Gate");
	initializeObject(70368747895387, ZoneId::FORBIDDEN_VALLEY, ObjectType::Minigame, "Dragon Battle"); //9
	initializeObject(70368747895388, ZoneId::FORBIDDEN_VALLEY, ObjectType::Minigame, "Dragonmaw Chasm Racetrack"); //9

	initializeLaunchPad(70368748024518, ZoneId::NEXUS_TOWER, ZoneId::NINJAGO_MONASTERY, "Outer Platform");
	initializeLaunchPad(70368748024521, ZoneId::NEXUS_TOWER, ZoneId::NIMBUS_STATION, "Outer Platform");
	initializeObject(70368748024542, ZoneId::NEXUS_TOWER, ObjectType::Portal, "To Crux Prime, Sentinel Sector");
	initializeObject(70368748024544, ZoneId::NEXUS_TOWER, ObjectType::Portal, "To LEGO Club, Venture Lague Observatory");

	initializeObject(70368748035362, ZoneId::NINJAGO_MONASTERY, ObjectType::Minigame, "Frakjaw Battle"); //9
	initializeLaunchPad(70368748035363, ZoneId::NINJAGO_MONASTERY, ZoneId::NEXUS_TOWER);
	initializeLaunchPad(70368748035364, ZoneId::NINJAGO_MONASTERY, ZoneId::NEXUS_TOWER);

	initializeLaunchPad(70368748035995, ZoneId::PET_COVE, ZoneId::NIMBUS_STATION);
	initializeObject(70368748036113, ZoneId::PET_COVE, ObjectType::Binoculars, "Tower"); //9
	initializeObject(70368748036114, ZoneId::PET_COVE, ObjectType::Binoculars, "Jumper"); //9
	initializeObject(70368748036115, ZoneId::PET_COVE, ObjectType::Binoculars, "Bridge"); //9
	initializeObject(70368748036116, ZoneId::PET_COVE, ObjectType::Binoculars, "Launch Area"); //9
	initializeObject(70368748036250, ZoneId::PET_COVE, ObjectType::Plaque, "Pet Cove"); //9

	initializeLaunchPad(70368748055482, ZoneId::PORTABELLO, ZoneId::STARBASE_3001);

	initializeObject(70643622146017, ZoneId::CRUX_PRIME, ObjectType::PostBox, "Sentinel Point Zeta"); //1 9
	initializeObject(70643622146030, ZoneId::CRUX_PRIME, ObjectType::Plaque, "Sentinel Point Zeta"); //1 9
	initializeObject(70643622146035, ZoneId::CRUX_PRIME, ObjectType::Binoculars, "Sentinel Point Zeta"); // 1 9
	
	initializeObject(70918500266380, ZoneId::GNARLED_FOREST, ObjectType::Object, "Pirate Camp, Boat"); //0 9
	initializeObject(70918500266383, ZoneId::GNARLED_FOREST, ObjectType::Machine, "Pirate Camp, Bird Blockage"); //0 9
	initializeObject(70918500266432, ZoneId::GNARLED_FOREST, ObjectType::PostBox, "Pirate Camp"); //0 9
	initializeObject(70918500266460, ZoneId::GNARLED_FOREST, ObjectType::Binoculars, "Pirate Camp"); //0 9
	initializeObject(70918500266512, ZoneId::GNARLED_FOREST, ObjectType::Plaque, "Pirate Camp"); //0 9

	initializeObject(70918500270095, ZoneId::FORBIDDEN_VALLEY, ObjectType::PostBox, "Great Tree, Top"); //0 9
	initializeObject(70918500270182, ZoneId::FORBIDDEN_VALLEY, ObjectType::Binoculars, "Great Tree, Top, View Cavalry Hill"); //0 9
	initializeObject(70918500291607, ZoneId::NEXUS_TOWER, ObjectType::Binoculars, "Sentinel Training Chamber");
	initializeObject(70918500349608, ZoneId::AVANT_GARDENS, ObjectType::Binoculars, "Landing Area"); //0 9
	initializeObject(70918500349609, ZoneId::AVANT_GARDENS, ObjectType::Binoculars, "Paradox Research Facility"); //0 9
	initializeObject(70918500353520, ZoneId::NIMBUS_STATION, ObjectType::Plaque, "Red Blocks Amphitheatre"); //0 9

	initializeObject(71193378161101, ZoneId::NEXUS_TOWER, ObjectType::Vault, "Vault 1"); //0 1 9
	initializeObject(71193378161102, ZoneId::NEXUS_TOWER, ObjectType::Vault, "Vault 2"); //0 1 9
	initializeObject(71193378161103, ZoneId::NEXUS_TOWER, ObjectType::Vault, "Vault 3"); //0 1 9
	initializeObject(71193378161104, ZoneId::NEXUS_TOWER, ObjectType::Vault, "Vault 4"); //0 1 9
	initializeObject(71193378161105, ZoneId::NEXUS_TOWER, ObjectType::Vault, "Vault 5"); //0 1 9
	initializeObject(71193378161106, ZoneId::NEXUS_TOWER, ObjectType::Plaque, "Vault"); //0 1 9

	initializeObject(71193378196058, ZoneId::NIMBUS_STATION, ObjectType::Plaque, "Nimbus Station"); //0 1 9
	initializeObject(71193378197171, ZoneId::AVANT_GARDENS, ObjectType::Plaque, "Assembly Monument"); //0 1 9

	initializeObject(71193378197425, ZoneId::FORBIDDEN_VALLEY, ObjectType::Object, "Pond"); //0 1 9
	initializeObject(71193378197511, ZoneId::FORBIDDEN_VALLEY, ObjectType::PostBox, "Main Gate"); //0 1 9
	initializeObject(71193378197589, ZoneId::FORBIDDEN_VALLEY, ObjectType::Plaque, "Main Gate"); //0 1 9
	initializeObject(71193378197605, ZoneId::FORBIDDEN_VALLEY, ObjectType::Binoculars, "Launch Area"); //0 1 9

	initializeObject(71468255879625, ZoneId::FORBIDDEN_VALLEY, ObjectType::Binoculars, "The Great Tree"); //9 15
	initializeObject(71468255879626, ZoneId::FORBIDDEN_VALLEY, ObjectType::Plaque, "The Great Tree"); //9 15

	initializeObject(71468255900780, ZoneId::CRUX_PRIME, ObjectType::Machine, "Rivendark Canyon Bridge Console"); //9 15
	initializeObject(71468255950718, ZoneId::NINJAGO_MONASTERY, ObjectType::Binoculars, "Entrance");

	initializeObject(71743133775548, ZoneId::FORBIDDEN_VALLEY, ObjectType::Plaque, "Mantis Rock"); //1 9 15
	initializeObject(71743133781636, ZoneId::GNARLED_FOREST, ObjectType::Plaque, "Fort");
	initializeObject(71743133781654, ZoneId::NEXUS_TOWER, ObjectType::Plaque, "Sentinel War Room, Infected Bricks -> 5 Faction Tokens"); //1 9 15
	initializeObject(71743133781655, ZoneId::NEXUS_TOWER, ObjectType::Plaque, "Sentinel War Room, Red Imaginite -> Surprise"); //1 9 15
	initializeObject(71743133781658, ZoneId::NEXUS_TOWER, ObjectType::Plaque, "Sentinel War Room"); //1 9 15
	initializeObject(71743133781659, ZoneId::NEXUS_TOWER, ObjectType::PostBox, "Sentinel War Room"); //1 9 15
	initializeObject(71743133781667, ZoneId::NEXUS_TOWER, ObjectType::Binoculars, "Sentinel War Room Top");
	initializeObject(71743133789565, ZoneId::AVANT_GARDENS, ObjectType::Binoculars, "Assembly Monument"); //1 9 15
	initializeObject(71743133790046, ZoneId::NIMBUS_STATION, ObjectType::Binoculars, "Red Blocks Amphitheatre"); //1 9 15

	initializeObject(72018011732500, ZoneId::FORBIDDEN_VALLEY, ObjectType::Binoculars, "Cavalry Hill"); //0 9 15
	initializeObject(72018011738868, ZoneId::CRUX_PRIME, ObjectType::Plaque, "Maelstrom Quarry"); //0 9 15
	initializeObject(72018011738889, ZoneId::CRUX_PRIME, ObjectType::Binoculars, "Studburst Pass"); //0 9 15
	initializeObject(72018011738901, ZoneId::CRUX_PRIME, ObjectType::Binoculars, "Maelstrom Quarry"); //0 9 15
	initializeObject(72018011738902, ZoneId::CRUX_PRIME, ObjectType::Plaque, "Studburst Pass"); //0 9 15
	initializeObject(72018011739912, ZoneId::GNARLED_FOREST, ObjectType::Binoculars, "Race Place"); //0 9 15

	initializeObject(72018011766437, ZoneId::NIMBUS_STATION, ObjectType::Binoculars, "Plaza");

	initializeObject(72292889654081, ZoneId::FORBIDDEN_VALLEY, ObjectType::Plaque, "Cavalry Hill"); //0 1 9 15
	initializeObject(72292889655302, ZoneId::CRUX_PRIME, ObjectType::Plaque, "Caldera Mar"); //0 1 9 15
	initializeObject(72292889655743, ZoneId::GNARLED_FOREST, ObjectType::Binoculars, "Lake Entrance"); //0 1 9 15
	initializeObject(72292889655744, ZoneId::GNARLED_FOREST, ObjectType::Binoculars, "Lake"); //0 1 9 15
	initializeObject(72292889657051, ZoneId::NEXUS_TOWER, ObjectType::Plaque, "Sentinel Command Center"); //0 1 9 15
	initializeObject(72292889665739, ZoneId::AVANT_GARDENS, ObjectType::Binoculars, "Outpost"); //0 1 9 15
	initializeObject(72292889665740, ZoneId::AVANT_GARDENS, ObjectType::Machine, "Outpost Console"); //0 1 9 15
	initializeObject(72292889665744, ZoneId::AVANT_GARDENS, ObjectType::Plaque, "Paradox Research Facility"); //0 1 9 15
	initializeObject(72292889665947, ZoneId::NINJAGO_MONASTERY, ObjectType::Plaque, "Monastery of Spinjitzu"); //0 1 9 15
	initializeObject(72292889667055, ZoneId::NIMBUS_STATION, ObjectType::PostBox, "To Gnarled Forest"); //0 1 9 15
	initializeObject(72292889667069, ZoneId::NIMBUS_STATION, ObjectType::Binoculars, "To Gnarled Forest"); //0 1 9 15

	initializeObject(72567767517764, ZoneId::NIMBUS_STATION, ObjectType::Binoculars, "Brick Annex"); //9 14
	initializeObject(72567767517765, ZoneId::NIMBUS_STATION, ObjectType::PostBox, "Brick Annex"); //9 14
	initializeObject(72567767517778, ZoneId::NIMBUS_STATION, ObjectType::Plaque, "Brick Annex"); //9 14

	initializeObject(72842645471485, ZoneId::GNARLED_FOREST, ObjectType::PostBox, "Launch Pad"); //1 9 14
	initializeObject(72842645471542, ZoneId::GNARLED_FOREST, ObjectType::Binoculars, "Bridge Pond"); //1 9 14
	initializeObject(72842645471543, ZoneId::GNARLED_FOREST, ObjectType::Binoculars, "Bridge"); //1 9 14
	initializeObject(72842645471553, ZoneId::GNARLED_FOREST, ObjectType::Binoculars, "Launch Pad"); //1 9 14
	initializeObject(72842645471602, ZoneId::GNARLED_FOREST, ObjectType::Plaque, "Launch Pad"); //1 9 14
	initializeObject(72842645479744, ZoneId::NIMBUS_STATION, ObjectType::Binoculars, "To Forbidden Valley"); //1 9 14
	initializeObject(72842645480990, ZoneId::NIMBUS_STATION, ObjectType::Plaque, "Maelstrom Forcefield"); //1 9 14

	initializeObject(73117523384592, ZoneId::FORBIDDEN_VALLEY, ObjectType::Binoculars, "Paradox Maelstrom Refinery"); //0 9 14
	initializeObject(73117523384619, ZoneId::FORBIDDEN_VALLEY, ObjectType::Binoculars, "Paradox Maelstrom Refinery 3"); //0 9 14
	initializeObject(73117523384623, ZoneId::FORBIDDEN_VALLEY, ObjectType::Binoculars, "Paradox Maelstrom Refinery 2"); //0 9 14
	initializeObject(73117523384625, ZoneId::FORBIDDEN_VALLEY, ObjectType::Plaque, "Paradox Maelstrom Refinery"); //0 9 14
	initializeObject(73117523384630, ZoneId::FORBIDDEN_VALLEY, ObjectType::Binoculars, "Paradox Maelstrom Refinery 4"); //0 9 14
	initializeObject(73117523384637, ZoneId::FORBIDDEN_VALLEY, ObjectType::Binoculars, "Dragon"); //0 9 14
	initializeObject(73117523391546, ZoneId::NINJAGO_MONASTERY, ObjectType::PostBox, "Monastery Courtyard"); //0 9 14
	initializeObject(73117523391551, ZoneId::NINJAGO_MONASTERY, ObjectType::Binoculars, "Monastery Courtyard"); //0 9 14
	initializeObject(73117523401514, ZoneId::NIMBUS_STATION, ObjectType::Binoculars, "Path To Avant Gardens"); //0 9 14

	initializeObject(73392401181075, ZoneId::FORBIDDEN_VALLEY, ObjectType::Binoculars, "The Great Tree, 2"); //0 1 9 14
	initializeObject(73392401201346, ZoneId::NIMBUS_STATION, ObjectType::Portal, "To Starbase 3001, Brick Annex"); //0 1 9 14
	initializeObject(73392401202185, ZoneId::AVANT_GARDENS, ObjectType::Plaque, "Paradox Maelstrom Mines"); //0 1 9 14
	initializeObject(73392401208497, ZoneId::NINJAGO_MONASTERY, ObjectType::Plaque, "Monastery Courtyard"); //0 1 9 14

	initializeObject(73667279106949, ZoneId::NEXUS_TOWER, ObjectType::Binoculars, "Outer Platform"); //9 14 15
	initializeObject(73667279128744, ZoneId::AVANT_GARDENS, ObjectType::Binoculars, "Sentinel Encampment"); //9 14 15
	initializeObject(73667279128745, ZoneId::AVANT_GARDENS, ObjectType::Plaque, "Sentinel Encampment"); //9 14 15
	initializeObject(73667279128748, ZoneId::AVANT_GARDENS, ObjectType::PostBox, "Sentinel Encampment"); //9 14 15
	initializeObject(73667279133324, ZoneId::NIMBUS_STATION, ObjectType::WaterPot, "Brick Annex"); //9 14 15
	initializeObject(73667279133326, ZoneId::NIMBUS_STATION, ObjectType::Binoculars, "Brick Annex"); //9 14 15

	initializeObject(73942156985321, ZoneId::FORBIDDEN_VALLEY, ObjectType::Plaque, "Maelstrom Valley"); //1 9 14 15

	initializeObject(74217034937052, ZoneId::NIMBUS_STATION, ObjectType::Binoculars, "To Brick Annex"); //0 9 14 15
	initializeObject(74217034937466, ZoneId::NEXUS_TOWER, ObjectType::Plaque, "Nexus Naomi Hologram"); //0 9 14 15
	
	initializeObject(74491912819086, ZoneId::GNARLED_FOREST, ObjectType::Plaque, "Maelstrom Trench"); //0 1 9 14 15
	initializeObject(74491912839034, ZoneId::NIMBUS_STATION, ObjectType::PostBox, "Nexus Checkpoint");
	initializeObject(74491912839048, ZoneId::NIMBUS_STATION, ObjectType::Binoculars, "Nexus Checkpoint");
	initializeObject(74491912839052, ZoneId::NIMBUS_STATION, ObjectType::Plaque, "Nexus Checkpoint");

	initializeObject(74491912850866, ZoneId::NINJAGO_MONASTERY, ObjectType::Plaque, "Dojo of Earth"); //0 1 9 14 15
	initializeObject(74491912850869, ZoneId::NINJAGO_MONASTERY, ObjectType::Binoculars, "Dojo of Earth"); //0 1 9 14 15

	initializeObject(75041668653659, ZoneId::CRUX_PRIME, ObjectType::PostBox, "Mealstrom Quarry Rest"); //1 9 13

	initializeObject(75316546548795, ZoneId::CRUX_PRIME, ObjectType::Binoculars, "Maelstrom Eye"); //0 9 13

	initializeObject(75316546548974, ZoneId::NEXUS_TOWER, ObjectType::Plaque, "Venture League Map Room"); //0 9 13
	initializeObject(75316546548975, ZoneId::NEXUS_TOWER, ObjectType::Plaque, "Venture League Map Room"); //0 9 13
	initializeObject(75316546549183, ZoneId::NIMBUS_STATION, ObjectType::Plaque, "To Avant Gardens Lauch Pad"); //0 9 13
	initializeObject(75316546549184, ZoneId::NIMBUS_STATION, ObjectType::PostBox, "To Avant Gardens Lauch Pad"); //0 9 13

	initializeObject(75591424409874, ZoneId::NIMBUS_STATION, ObjectType::Binoculars, "To Red Blocks Amphitheatre"); //0 1 9 13
	initializeObject(75591424442068, ZoneId::NEXUS_TOWER, ObjectType::PostBox, "Venture League Map Room"); //0 1 9 13
	initializeObject(75591424442077, ZoneId::NEXUS_TOWER, ObjectType::Machine, "Canon to Venture League Observatory");
	initializeObject(75591424442078, ZoneId::NEXUS_TOWER, ObjectType::WaterPot, "Venture Lague Observatory"); //0 1 9 13
	initializeObject(75591424442081, ZoneId::NEXUS_TOWER, ObjectType::Plaque, "Venture Lague Observatory"); //0 1 9 13

	initializeObject(75591424453101, ZoneId::NINJAGO_MONASTERY, ObjectType::Plaque, "Dojo of Lightning"); //0 1 9 13

	initializeObject(75866302347039, ZoneId::CRUX_PRIME, ObjectType::Binoculars, "Rivendark Canyon, Above");
	initializeObject(75866302347041, ZoneId::CRUX_PRIME, ObjectType::Plaque, "Rivendark Canyon");
	initializeObject(75866302347053, ZoneId::CRUX_PRIME, ObjectType::Binoculars, "Rivendark Canyon");
	initializeObject(75866302347055, ZoneId::CRUX_PRIME, ObjectType::Machine, "Rivendark Canyon Bridge Console"); //9 13 15

	initializeObject(76416058130662, ZoneId::NIMBUS_STATION, ObjectType::Plaque, "Nexus Tower Progress");

	initializeObject(76690936104520, ZoneId::NINJAGO_MONASTERY, ObjectType::Plaque, "Dojo of Ice"); //0 1 9 13 15

	initializeObject(76965813951596, ZoneId::NEXUS_TOWER, ObjectType::Plaque, "Assembly Storerooms, Left"); // 9 13 14
	initializeObject(76965813951597, ZoneId::NEXUS_TOWER, ObjectType::Plaque, "Assembly Storerooms, Right"); // 9 13 14

	initializeObject(77240691855441, ZoneId::AVANT_GARDENS, ObjectType::Binoculars, "Broken Bridge");
	
	initializeObject(77515569774747, ZoneId::NEXUS_TOWER, ObjectType::PostBox, "Second Floor, Assembly Sector, Right"); //0 9 13 14
	initializeObject(77790447712134, ZoneId::NINJAGO_MONASTERY, ObjectType::Plaque, "Dojo of Fire"); //0 1 9 13 14

	initializeObject(78065325572326, ZoneId::AVANT_GARDENS, ObjectType::PostBox, "Launch Area"); //9 13 14 15
	initializeObject(78065325582716, ZoneId::NEXUS_TOWER, ObjectType::PostBox, "Second Floor, Assembly Sector, Left"); //9 13 14 15

	initializeObject(78615081386742, ZoneId::NINJAGO_MONASTERY, ObjectType::Plaque, "Ninjago Caves"); //0 9 13 14 15

	initializeObject(78616155129237, ZoneId::AVANT_GARDENS, ObjectType::Binoculars, "Launch Area, Left"); //0 9 13 14 15
	initializeObject(78616155129238, ZoneId::AVANT_GARDENS, ObjectType::Binoculars, "Launch Area, Right"); //0 9 13 14 15
	initializeObject(78616155129240, ZoneId::AVANT_GARDENS, ObjectType::Plaque, "Launch Area"); //0 9 13 14 15
	initializeObject(78616155129241, ZoneId::AVANT_GARDENS, ObjectType::Machine, "Launch Area, Drinkmachine"); //0 9 13 14 15

	initializeObject(78889959312587, ZoneId::NEXUS_TOWER, ObjectType::Machine, "Paradox Logo Machine");
	initializeObject(78889959312604, ZoneId::NEXUS_TOWER, ObjectType::Plaque, "Paradox Main Hall");
	initializeObject(78889959312605, ZoneId::NEXUS_TOWER, ObjectType::Plaque, "Paradox Laboratory");
	initializeObject(78889959312605, ZoneId::NEXUS_TOWER, ObjectType::Plaque, "Faction Leader");
	initializeObject(78889959312607, ZoneId::NEXUS_TOWER, ObjectType::PostBox, "Paradox Main Hall");
	initializeObject(78889959312610, ZoneId::NEXUS_TOWER, ObjectType::Machine, "Paradox Main Hall, Under right Stairwell");
	initializeObject(78889959327071, ZoneId::NINJAGO_MONASTERY, ObjectType::Binoculars, "Black Cave"); //0 1 9 13 14 15
	initializeObject(79439715114396, ZoneId::NEXUS_TOWER, ObjectType::Machine, "Paradox Sector, By Right Nexus Portal");
	initializeObject(79989470935540, ZoneId::NINJAGO_MONASTERY, ObjectType::Binoculars, "Ninjago Caves"); //0 1 9 12

	initializeObject(80539226748439, ZoneId::NEXUS_TOWER, ObjectType::Binoculars, "Nexus Core"); //1 9 2 15;
	initializeObject(80539226748440, ZoneId::NEXUS_TOWER, ObjectType::Plaque, "Nexus Core, Dr. Albert Overbuild"); //1 9 12 15
	

	initializeObject(86311662781347, ZoneId::NEXUS_TOWER, ObjectType::Portal, "To Starbase 3001, Assembly Sector");

	isObjectsInitialized = true;
}

ObjectInformation getObjectInformation(long long objectid){
	if (!isObjectsInitialized)initializeObjects();
	return Objects[objectid];
}

std::string getObjectDescription(ObjectInformation obj, long long objid){
	long long objidA = obj.objid;
	if (objidA == 0 && objid > -1) objidA = objid;

	std::stringstream ss;
	ss << "[" << objidA << "] ";
	ss << obj.zone << " ";
	switch (obj.type)
	{
	case ObjectType::Binoculars:
		ss << "Binoculars: ";
		break;
	case ObjectType::LaunchPad:
		ss << "to " << LaunchPads[objidA];
		ss << " via ROCKET ";
		break;
	case ObjectType::Machine:
		ss << "Machine:    ";
		break;
	case ObjectType::Minigame:
		ss << "Minigame:   ";
		break;
	case ObjectType::Object:
		ss << "Object:     ";
		break;
	case ObjectType::Plaque:
		ss << "Plaque:     ";
		break;
	case ObjectType::Portal:
		ss << "Portal:     ";
		break;
	case ObjectType::PostBox:
		ss << "PostBox:    ";
		break;
	case ObjectType::Vault:
		ss << "Vault:      ";
		break;
	case ObjectType::WaterPot:
		ss << "Waterpot:   ";
		break;
	case ObjectType::NONE:
		ss << "NOT REGISTERED!";
		return ss.str();
		break;
	default:
		ss << "???:        ";
		break;
	}
	ss << obj.info;
	return ss.str();
}

ZoneId getLaunchPadTarget(ObjectInformation obj){
	if (obj.type == ObjectType::LaunchPad){
		return LaunchPads[obj.objid];
	}
	return NO_ZONE;
}


///start refactoring here///
void GameMSG::teleport(long long charid, COMPONENT1_POSITION position) {
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));

	RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(charid, TELEPORT);
	bs->Write(false);
	bs->Write(false);
	bs->Write(false);

	bs->Write(position.x);
	bs->Write(position.y);
	bs->Write(position.z);

	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++){
		WorldServer::sendPacket(bs, sessionsz.at(k).addr);
	}
}

void GameMSG::dropClientLoot(long long charid, COMPONENT1_POSITION position, long currency, unsigned long LOT, long long objid, long long ownerid, long long sourceid, COMPONENT1_POSITION finalPosition) {
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));

	RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(charid, DROP_CLIENT_LOOT);
	bs->Write(true);

	bs->Write(true);
	bs->Write(position.x);
	bs->Write(position.y);
	bs->Write(position.z);

	bs->Write(currency);

	bs->Write(LOT);
	bs->Write(objid);

	bs->Write(ownerid);
	bs->Write(sourceid);

	bs->Write(true);
	bs->Write(finalPosition.x);
	bs->Write(finalPosition.y);
	bs->Write(finalPosition.z);

	WorldServer::sendPacket(bs, s.addr);
}

void GameMSG::die(long long charid, bool clientDeath, bool spawnLoot, float coinSpawnTime, std::wstring deathType, float forceXZ, float forceY, float force, long long killerid, long long lootOwnerID) {
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));

	RakNet::BitStream * bs = WorldServerPackets::InitGameMessage(charid, DIE);
	bs->Write((bool)clientDeath);
	bs->Write((bool)spawnLoot);
	bs->Write((float)coinSpawnTime);
	bs->Write((long)0);
	bs->Write((float)forceXZ);
	bs->Write((float)forceY);
	bs->Write((float)force);
	bs->Write((bool)false);
	bs->Write((unsigned long long) killerid);
	bs->Write((unsigned long long) charid);

	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++) {
		WorldServer::sendPacket(bs, sessionsz.at(k).addr);
	}

	//GameMSG::playAnimation(charid, L"death", true, true);
}

void GameMSG::playAnimation(long long charid, std::wstring animationID, bool playImmediate, bool ignoreSubject) {
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));

	RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(charid, PLAY_ANIMATION);
	bs->Write((unsigned long)animationID.size());

	for (int i = 0; i < animationID.size(); i++) {
		bs->Write((char)animationID.at(i));
		bs->Write((char)0);
	}

	bs->Write((bool)true);
	bs->Write(playImmediate);
	bs->Write((bool)true);

	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++){
		bool canSend = true;
		if (ignoreSubject)
			if (sessionsz.at(k).activeCharId == charid)
				canSend = false;
		if(canSend)
			WorldServer::sendPacket(bs, sessionsz.at(k).addr);
	}
}

void GameMSG::setName(long long charid, std::wstring name) {
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));

	//this one is for the subject player
	RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(charid, SET_NAME);
	bs->Write((unsigned long)name.size());

	for (int i = 0; i < name.size(); i++) {
		bs->Write((char)name.at(i));
		bs->Write((char)0);
	}


	//This one is for the other players
	RakNet::BitStream *bsc = WorldServerPackets::InitGameMessage(charid, SET_NAME);
	name.append(Characters::GetCharacterSubfix(charid));
	bsc->Write((unsigned long)name.size());
	for (int i = 0; i < name.size(); i++) {
		bsc->Write((char)name.at(i));
		bsc->Write((char)0);
	}

	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++){
		if (sessionsz.at(k).activeCharId==charid) {
			WorldServer::sendPacket(bs, sessionsz.at(k).addr);
		}
		else {
			WorldServer::sendPacket(bsc, sessionsz.at(k).addr);
		}
	}
}

void GameMSG::addSkill(long long charid, unsigned long skillid, unsigned long slot) {
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));

	RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(charid, ADD_SKILL);

	for (int i = 0; i < 5; i++) {
		bs->Write((bool)false);
	}

	bs->Write(skillid);
	bs->Write((bool)true);
	bs->Write(slot);
	bs->Write((bool)true);

	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++){
		WorldServer::sendPacket(bs, sessionsz.at(k).addr);
	}
}

void GameMSG::removeSkill(long long charid, unsigned long skillid) {
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));

	RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(charid, REMOVE_SKILL);
	bs->Write((bool)false);
	bs->Write(skillid);

	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++){
		WorldServer::sendPacket(bs, sessionsz.at(k).addr);
	}
}

void GameMSG::SetStunned(long long charid, std::string StateChangeType, bool cantAttack, bool cantAttackOCWA, bool cantEquip, bool cantEquipOCWA,
						 bool cantInteract, bool cantInteractOCWA, bool cantJump, bool cantJumpOCWA, bool cantMove, bool cantMoveOCWA, bool cantTurn, 
						 bool cantTurnOCWA, bool cantUseItem, bool cantUseItemOCWA, bool dontTerminateInteract, bool bIgnoreImmunity) {
	RakNet::BitStream * bs = WorldServerPackets::InitGameMessage(charid, SET_STUNNED);
	bs->Write((long)1);
	bs->Write(cantAttack);
	bs->Write(cantAttackOCWA);
	bs->Write(cantEquip);
	bs->Write(cantEquipOCWA);
	bs->Write(cantInteract);
	bs->Write(cantInteractOCWA);
	bs->Write(cantJump);
	bs->Write(cantJumpOCWA);
	bs->Write(cantMove);
	bs->Write(cantMoveOCWA);
	bs->Write(cantTurn);
	bs->Write(cantTurnOCWA);
	bs->Write(cantUseItem);
	bs->Write(cantUseItemOCWA);
	bs->Write(dontTerminateInteract);
	bs->Write(bIgnoreImmunity);
	WorldServer::sendPacket(bs, SessionsTable::findCharacter(charid));
}

void GameMSG::resurrect(long long charid, bool immediate)
{
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));

	SessionInfo *t = &SessionsTable::getClientSession(s.addr);
	if (s.activeCharId > 0) {
		long long objid = s.activeCharId;
		ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(objid);
		
		WorldServerPackets::CreateCharacter(s.addr, charid);

		RakNet::BitStream * bs = WorldServerPackets::InitGameMessage(s.activeCharId, RESURRECT);
		bs->Write((bool)immediate);
		std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
		for (unsigned int k = 0; k < sessionsz.size(); k++) {
			WorldServer::sendPacket(bs, sessionsz.at(k).addr);
		}
	}
}

void GameMSG::knockback(long long charid, long long casterid, COMPONENT1_POSITION position) {
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));

	RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(charid, KNOCKBACK);
	if (casterid > 0) {
		bs->Write(true);
		bs->Write(casterid);
		bs->Write(true);
		bs->Write(casterid);
	}
	else {
		bs->Write(false);
		bs->Write(false);
	}

	bs->Write(false);

	bs->Write(position.x);
	bs->Write(position.y);
	bs->Write(position.z);

	WorldServer::sendPacket(bs, s.addr);
}

void GameMSG::addItemToInventoryClientSync(long long charid, long long objectID, long lotTemplate, long slotid, bool showFlyingLoot) {
	SystemAddress addr = SessionsTable::findCharacter(charid);
	
	RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(charid, ADD_ITEM_TO_INVENTORY_CLIENT_SYNC);
	/*ef->Write((bool)false);						//bBound
	ef->Write((bool)false);						//bIsBOE
	ef->Write((bool)false);						//bIsBOP
	ef->Write((bool)false);						//eLootTypeSource
	ef->Write((unsigned long)0);				//extraInfo
	ef->Write((unsigned long)lotTemplate);		//iObjTemplate
	ef->Write((bool)false);						//iSubkey
	ef->Write((bool)false);						//invType
	ef->Write((bool)true);						//itemCount
	ef->Write((unsigned long)lotTemplate);		//itemsTotal
	ef->Write((bool)true);						//newObjID
	ef->Write((unsigned long)0);				
	ef->Write(objectID);
	ef->Write((float)0);
	ef->Write((float)0);
	ef->Write((float)0);
	ef->Write((bool)true);
	ef->Write((unsigned long)slotid);*/

	ef->Write((bool)false);						//bBound
	ef->Write((bool)false);						//bIsBOE
	ef->Write((bool)false);						//bIsBOP
	ef->Write((bool)false);						//eLootTypeSource
	ef->Write((unsigned long)0);				//extraInfo
	ef->Write((unsigned long)lotTemplate);		//iObjTemplate
	ef->Write((long long)0);						//iSubkey
	ef->Write((int)false);						//invType
	//ef->Write((bool)true);						//itemCount
	ef->Write((bool)true);						//newObjID
	ef->Write((unsigned long)0);
	ef->Write(objectID);
	ef->Write((float)0);
	ef->Write((float)0);
	ef->Write((float)0);
	ef->Write((bool)true);
	ef->Write((unsigned long)slotid);
	WorldServer::sendPacket(ef, addr);
}

void GameMSG::displayMessageBox(long long charid, long long callbackClient, std::wstring identifier, std::wstring text, std::wstring userData) {
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));

	RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(charid, DISPLAY_MESSAGE_BOX); 
	bs->Write(true);
	bs->Write(callbackClient);

	bs->Write((unsigned long)identifier.size());
	for (int i = 0; i < identifier.size(); i++) {
		bs->Write((char)identifier.at(i));
		bs->Write((char)0);
	}

	bs->Write((long)0);

	bs->Write((unsigned long)text.size());
	for (int i = 0; i < text.size(); i++) {
		bs->Write((char)text.at(i));
		bs->Write((char)0);
	}

	bs->Write((unsigned long)userData.size());
	for (int i = 0; i < userData.size(); i++) {
		bs->Write((char)userData.at(i));
		bs->Write((char)0);
	}

	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++){
		WorldServer::sendPacket(bs, sessionsz.at(k).addr);
	}
}

void GameMSG::unsmash(long long charid) {

}

void GameMSG::setJetPackMode(long long charid, bool bypassChecks, bool doHover, bool use) {
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));

	RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(charid, SET_JETPACK_MODE);
	bs->Write(bypassChecks);
	bs->Write(doHover);
	bs->Write(use);

	bs->Write(false); // effectID
	bs->Write(false); // airSpeed
	bs->Write(false); // maxAirSpeed
	bs->Write(false); // vertVel
	bs->Write(false); // warningEffectID

	WorldServer::sendPacket(bs, s.addr);
} // TODO: Fix!

void GameMSG::displayZoneSummary(long long charid, bool isProperty, bool isZoneStart) {
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));

	RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(charid, DISPLAY_ZONE_SUMMARY);
	bs->Write(isProperty);
	bs->Write(isZoneStart);
	bs->Write((unsigned long long)charid);

	WorldServer::sendPacket(bs, s.addr);
}

void GameMSG::sendLevelUpMessage(long long charid) {
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));
	std::vector<SessionInfo> wsessions = SessionsTable::getClientsInWorld(s.zone);
	std::wstring name;
	ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s.activeCharId);
	name.assign(cinfo.info.name.begin(), cinfo.info.name.end());
	for (unsigned int i = 0; i < wsessions.size(); i++){
		Chat::sendChatMessage(wsessions.at(i).activeCharId, name + L" is now at level " + std::to_wstring(cinfo.info.level + 1), L"System", false);
	}
}

/*unrefactored stuff here*/
void GameMSG::castActiveSkill(long long charid, unsigned long someNumber) {
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));

	RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(charid, 125);
	bs->Write(someNumber);

	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++){
		if (!(sessionsz.at(k).activeCharId == charid)) {
			WorldServer::sendPacket(bs, sessionsz.at(k).addr);
		}
	}
}

void GameMSG::verifyAck(SystemAddress &systemAddress, std::string sBitStream, unsigned long uiHandle, bool bDifferent) {
	
	RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(SessionsTable::getClientSession(systemAddress).activeCharId, GameMessage::VERIFY_ACK);
	bs->Write(bDifferent);
	bs->Write(sBitStream);
	bs->Write(uiHandle);

	WorldServer::sendPacket(bs, systemAddress);
}

void GameMSG::EchoSyncSkill(SystemAddress &systemAddress, std::string sBitStream, unsigned long uiBehaviorHandle, unsigned long uiSkillHandle, bool bDone) {
	long long charId = SessionsTable::getClientSession(systemAddress).activeCharId;
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charId));
	RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(charId, GameMessage::ECHO_SYNC_SKILL);
	bs->Write(sBitStream);
	bs->Write(uiBehaviorHandle);
	bs->Write(uiSkillHandle);
	bs->Write(bDone);

	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++)
		if (sessionsz.at(k).activeCharId != charId)
			WorldServer::sendPacket(bs, sessionsz.at(k).addr);
}

void GameMSG::EchoStartSkill(SystemAddress &systemAddress, bool bUsedMouse, float fCasterLatency, int iCastType, NiPoint3 lastClickedPos,
long long optionalOriginatorID, long long optionalTargetID, NiQuaternion originatorRot, std::string sBitStream, unsigned long skillID,
unsigned long uiSkillHandle)
{
	long long charId = SessionsTable::getClientSession(systemAddress).activeCharId;
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charId));
	RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(charId, GameMessage::ECHO_START_SKILL);
	bs->Write(bUsedMouse);
	bs->Write(fCasterLatency);
	bs->Write(iCastType);
	bs->Write(lastClickedPos.x);
	bs->Write(lastClickedPos.y);
	bs->Write(lastClickedPos.z);
	bs->Write(optionalOriginatorID);
	bs->Write(originatorRot.x);
	bs->Write(originatorRot.y);
	bs->Write(originatorRot.z);
	bs->Write(originatorRot.w);
	bs->Write(sBitStream);
	bs->Write(std::to_string(skillID));
	bs->Write(uiSkillHandle);

	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++) 
		if (sessionsz.at(k).activeCharId != charId) 
			WorldServer::sendPacket(bs, sessionsz.at(k).addr);
}

void GameMSG::equipItem(long long charid, long long objid){
	PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(charid);
	if (player != NULL) {
		long long test = EquipmentTable::getFromItemType(charid, CDClientDB::getItemType(CDClientDB::getComponentID(ObjectsTable::getTemplateOfItem(objid), 11)));
		if (test != NULL) {
			bool un = player->getComponent17()->unequipItem(test);
			if (!un) {
				Logger::log("WRLD", "EQUIP", "ERROR: item not found", LOG_ERROR);
			}
			else {
				EquipmentTable::unequipItem(charid, test);
				unsigned long LOT = ObjectsTable::getTemplateOfItem(test);

				unsigned long skillID = CDClientDB::getSkillID(LOT, 0);
				if (LOT == LOT::LOT_SLITHERSTRIKER ||
					LOT == LOT::LOT_NIGHTLASHER ||
					LOT == LOT::LOT_ENERGY_SPORK ||
					LOT == LOT::LOT_ZAPZAPPER)
					skillID = 148;
				if (skillID != -1)
					GameMSG::removeSkill(charid, skillID);

				if (LOT == LOT::LOT_JETPACK || LOT == LOT::LOT_PROPERTY_JETPACK) {
					std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(charid);
					for (unsigned int k = 0; k < sessionsz.size(); k++) {
						WorldServerPackets::SendGameMessage(sessionsz.at(k).addr, charid, 561);
					}
				}
			}
		}
		else {
			Logger::log("WORLD", "EQUIP", "Equip object failed!");
		}

		long lot = player->getComponent17()->equipItem(objid);

		unsigned long itemType = CDClientDB::getItemType(CDClientDB::getComponentID(lot, 11));
		EquipmentTable::equipItem(charid, objid, itemType);

		unsigned long hotbarslot = 4;
		if (itemType == ItemType::HAIR || ItemType::HAT)
			hotbarslot = 3;
		if (itemType == ItemType::NECK)
			hotbarslot = 2;
		if (itemType == ItemType::RIGHT_HAND)
			hotbarslot = 0;
		if (itemType == ItemType::LEFT_HAND)
			hotbarslot = 1;

		unsigned long skillid = CDClientDB::getSkillID(lot, 0);
		if (lot == LOT::LOT_SLITHERSTRIKER ||
			lot == LOT::LOT_NIGHTLASHER ||
			lot == LOT::LOT_ENERGY_SPORK ||
			lot == LOT::LOT_ZAPZAPPER)
			skillid = 148;
		if (skillid != -1)
			GameMSG::addSkill(charid, skillid, hotbarslot);

		if (lot == LOT::LOT_JETPACK) {
			RakNet::BitStream *pc = WorldServerPackets::InitGameMessage(charid, 561);

			pc->Write((unsigned long)0x70ba);
			pc->Write((unsigned short)0x8);
			pc->Write((unsigned char)0x5);
			pc->Write((unsigned char)0x2);
			pc->Write((unsigned short)0xc);
			pc->Write((unsigned char)0x3);
			pc->Write((unsigned short)0x6c1);
			pc->Write((unsigned char)0x0);
			pc->Write((unsigned char)0x1);
			pc->Write((unsigned char)0x80);
			pc->Write((unsigned char)0x7f);
			pc->Write((unsigned long)0xa7);

			std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(SessionsTable::getClientSession(SessionsTable::findCharacter(charid)).zone);
			for (unsigned int k = 0; k < sessionsz.size(); k++) {
				WorldServer::sendPacket(pc, sessionsz.at(k).addr);
			}
		}

		ObjectsManager::serialize(player);
	}
}
void GameMSG::unequipItem(long long charid, long long objid){
	PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(charid);
	if (player != NULL) {
		bool un = player->getComponent17()->unequipItem(objid);
		if (!un) {
			Logger::log("WRLD", "EQUIP", "ERROR: item not found", LOG_ERROR);
		}
		else {
			EquipmentTable::unequipItem(charid, objid);
			unsigned long LOT = ObjectsTable::getTemplateOfItem(objid);

			unsigned long skillID = CDClientDB::getSkillID(LOT, 0);
			if (LOT == LOT::LOT_SLITHERSTRIKER ||
				LOT == LOT::LOT_NIGHTLASHER ||
				LOT == LOT::LOT_ENERGY_SPORK ||
				LOT == LOT::LOT_ZAPZAPPER)
				skillID = 148;
			if (skillID != -1)
				GameMSG::removeSkill(charid, skillID);

			if (LOT == LOT::LOT_JETPACK || LOT == LOT::LOT_PROPERTY_JETPACK) {
				std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(SessionsTable::getClientSession(SessionsTable::findCharacter(charid)).zone);
				for (unsigned int k = 0; k < sessionsz.size(); k++) {
					WorldServerPackets::SendGameMessage(sessionsz.at(k).addr, charid, 561);
				}
			}

			ObjectsManager::serialize(player);
		}
	}
}

void GameMSG::OfferMission(int misID, SystemAddress systemAddress, long long npc) {
	long long charid = SessionsTable::getClientSession(systemAddress).activeCharId;
	RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(charid, OFFER_MISSION);
	ef->Write((int)misID);
	ef->Write((long long)npc);

	WorldServer::sendPacket(ef, systemAddress);
}

void GameMSG::NotifyMission(int misID, int misState, bool rewards, SystemAddress systemAddress) {
	long long charid = SessionsTable::getClientSession(systemAddress).activeCharId;
	RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(charid, NOTIFY_MISSION);
	ef->Write((int)misID);
	ef->Write((int)misState);
	ef->Write((bool)rewards);

	WorldServer::sendPacket(ef, systemAddress);
}

bool handleObjectInteraction(ObjectInformation obj, SystemAddress &systemAddress) {
	if (obj.type != ObjectType::NONE) {
		switch (obj.type)
		{
		case ObjectType::LaunchPad:
		{
			ZoneId zone = getLaunchPadTarget(obj);
			if (zone != ZoneId::NO_ZONE && zone != ZoneId::KEELHAUL_CANYON) {
				SessionInfo s = SessionsTable::getClientSession(systemAddress);

				RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(s.activeCharId, GameMessage::FIRE_EVENT_CLIENT_SIDE);
				bs->Write(L"RocketEquipped");
				bs->Write(0); //rocketObjID
				bs->Write(0);
				bs->Write(-1);
				bs->Write(s.activeCharId);
				WorldServer::sendPacket(bs, systemAddress);
				/*COMPONENT1_POSITION pos = getZoneSpawnPoint(zone, static_cast<ZoneId>(s.zone));
				bool flag = Worlds::loadWorld(systemAddress, zone, pos, 0, 0);
				if (flag) {
					Session::leave(s.activeCharId);

					WorldPlace place;
					place.zoneID = zone;
					place.mapClone = 0;
					place.mapInstance = 0;
					place.x = pos.x;
					place.y = pos.y;
					place.z = pos.z;
					CharactersTable::setCharactersPlace(s.activeCharId, place);

					ObjectsManager::clientLeaveWorld(s.activeCharId, systemAddress);
					//usr->DestructPlayer();
				}*/
			}
		}
		break;
		case ObjectType::PostBox:
		{
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			//Chat::sendMythranInfo(usr->GetCurrentCharacter()->charobjid, "We are sorry to inform you the Mail system isn't working yet!", "Our deepest apologies");
			Mail::openMailbox(s.activeCharId);
		}
		break;
		default:
			Logger::log("", "", "LALALA MANAGING INTERACT (B)");
			try{
				Interact(obj.objid, systemAddress);
				return true;
			}
			catch (const std::exception&){
				return false;
			}
			return true;
			break;
		}
	}
	return true;
}

/*TO PARSE ALL GAME MESSAGES*/
void GameMSG::parseGameMSG(unsigned short messageID, RakNet::BitStream *data, SystemAddress &systemAddress) {

	SessionInfo s = SessionsTable::getClientSession(systemAddress);

	switch (messageID) {
		case PLAY_EMOTE: {

			//read packet
			unsigned short emoteid;
			data->Read(emoteid);
			unsigned long long target;
			data->Read(target);

			//response
			std::wstring animationid = CDClientDB::getAnimationOfEmote(emoteid);
			if (animationid != L"INVALID") {
				GameMSG::playAnimation(s.activeCharId, animationid);
			}

		} break;
		case START_SKILL: {
			Logger::log("GMSG", "CMBT", "Start Skill");
			//read packet
			bool bUsedMouse;
			data->Read(bUsedMouse);
			long long consumableItemID;
			data->Read(consumableItemID);
			float fCasterLatency;
			data->Read(fCasterLatency);
			int iCastType;
			data->Read(iCastType);
			float posX;
			data->Read(posX);
			float posY;
			data->Read(posY);
			float posZ;
			data->Read(posZ);
			long long optionalOriginatorID;
			data->Read(optionalOriginatorID);
			long long optionalTargetID;
			data->Read(optionalTargetID);
			float rotX;
			data->Read(rotX);
			float rotY;
			data->Read(rotY);
			float rotZ;
			data->Read(rotZ);
			float rotW;
			data->Read(rotW);
			std::string sBitStream;
			data->Read(sBitStream);
			long skillID;
			data->Read(skillID);
			unsigned int uiSkillHandle;
			data->Read(uiSkillHandle);

			EchoStartSkill(systemAddress, bUsedMouse, fCasterLatency, iCastType, { posX,posY,posZ }, optionalOriginatorID, optionalTargetID, { rotX,rotY,rotZ,rotW }, sBitStream, skillID, uiSkillHandle);

			//response
			bool doLog = true; //Prevent spam
			if (doLog) {
				Logger::log("WRLD", "ATTACK", "StartSkill");
				Logger::log("WRLD", "ATTACK", "bUsedMouse: " + std::to_string(bUsedMouse));
				Logger::log("WRLD", "ATTACK", "consumableItemID: " + std::to_string(consumableItemID));
				Logger::log("WRLD", "ATTACK", "fCasterLatency: " + std::to_string(fCasterLatency));
				Logger::log("WRLD", "ATTACK", "iCastType: " + std::to_string(iCastType));
				Logger::log("WRLD", "ATTACK", "PosX: " + std::to_string(posX));
				Logger::log("WRLD", "ATTACK", "PosY: " + std::to_string(posY));
				Logger::log("WRLD", "ATTACK", "PosZ: " + std::to_string(posZ));
				Logger::log("WRLD", "ATTACK", "optionalOriginatorID: " + std::to_string(optionalOriginatorID));
				Logger::log("WRLD", "ATTACK", "optionalTargetID: " + std::to_string(optionalTargetID));
				Logger::log("WRLD", "ATTACK", "RotX: " + std::to_string(rotX));
				Logger::log("WRLD", "ATTACK", "RotY: " + std::to_string(rotY));
				Logger::log("WRLD", "ATTACK", "RotZ: " + std::to_string(rotZ));
				Logger::log("WRLD", "ATTACK", "RotW: " + std::to_string(rotW));
				Logger::log("WRLD", "ATTACK", "sBitStream: " + sBitStream);
				Logger::log("WRLD", "ATTACK", "skillID: " + std::to_string(skillID));
				Logger::log("WRLD", "ATTACK", "uiSkillHandle: " + std::to_string(uiSkillHandle));
			}
		} break;
		case SYNC_SKILL: {
			Logger::log("GMSG", "CMBT", "Sync Skill");
			//read packet
			bool flag;
			data->Read(flag);
			unsigned long d1;
			data->Read(d1);
			Logger::log("WRLD", "COMBAT", std::to_string(flag) + "|" + std::to_string(d1));
			//PacketTools::printBytes(data, d1);
			std::string bitstream = PacketTools::BytesToString(data, d1);
			unsigned long num1;
			data->Read(num1);
			unsigned long num2;
			data->Read(num2);
			Logger::log("WRLD", "COMBAT", std::to_string(num1) + "|" + std::to_string(num2));
			Logger::log("WRLD", "COMBAT", bitstream);
			//response
			EchoSyncSkill(systemAddress, bitstream, num1, num2, flag);



			/*RakNet::BitStream *bs = WorldServerPackets::InitGameMessage(SessionsTable::getClientSession(systemAddress).activeCharId, 0x0020);
			bs->Write(false);
			bs->Write(false);
			bs->Write(false);
			bs->Write(false);

			std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
			for (unsigned int k = 0; k < sessionsz.size(); k++) {
			WorldServer::sendPacket(bs, systemAddress);
			}
			*/

		} break;
		case SELECT_SKILL: {

			//read packet
			
			//response

		} break;
		case REQUEST_RESURRECT: {

			//read packet

			//response
			resurrect(s.activeCharId, false);

		} break;
		case MOVE_ITEM_IN_INVENTORY: {

			//read packet
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			//When you move on item on top of another item, then the two change their place
			bool flag;
			data->Read(flag);
			long long objid;
			data->Read(objid);
			long long unknown;
			data->Read(unknown);
			unsigned long slot;
			data->Read(slot);

			//response
			Logger::log("WRLD", "INVENTORY", "Move obj[" + std::to_string(objid) + "] to slot '" + std::to_string(slot) + "' (" + std::to_string(unknown) + "|" + std::to_string(flag), LOG_DEBUG);
			InventoryTable::moveItemToSlot(objid, s.activeCharId, slot);

			bool end;
			for (int k = 0; k < 7; k++) {
				data->Read(end);
			}
		} break;
		case REMOVE_ITEM_FROM_INVENTORY: {

			//read packet
			bool mode;
			data->Read(mode);
			unsigned long uk1;
			data->Read(uk1);
			unsigned long long uk2;
			data->Read(uk2);
			bool a1, a2, a3, a4, a5, a6, a7;
			data->Read(a1);
			data->Read(a2);
			data->Read(a3);
			data->Read(a4);
			data->Read(a5);
			data->Read(a6);
			data->Read(a7);
			long long itemid;
			data->Read(itemid);
			unsigned char uk4;
			data->Read(uk4);
			unsigned long uk5;
			data->Read(uk5);

			//response
			if (mode == true) {
				long long objid = SessionsTable::getClientSession(systemAddress).activeCharId;
				Logger::log("WLRD", "INVENTORY", "Delete item " + std::to_string(itemid) + " from character " + std::to_string(objid));
				InventoryTable::deleteItem(objid, itemid);
				ObjectsTable::deleteObject(itemid);
			}
		} break; 
		case EQUIP_INVENTORY: {

			//read packet
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			//Equip an item
			bool flag;
			data->Read(flag);
			bool flag2;
			data->Read(flag2);
			long long objid;
			data->Read(objid);
			Logger::log("WORLD", "EQUIP", "Equip obj [" + std::to_string(objid) + "]", LOG_ALL);
			bool end;
			for (int k = 0; k < 7; k++) {
				data->Read(end);
			}

			//response
			GameMSG::equipItem(s.activeCharId, objid);
		} break;
		case UNEQUIP_INVENTORY: {

			//read packet
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			bool flag;
			data->Read(flag);
			bool flag2;
			data->Read(flag2);
			bool flag3;
			data->Read(flag3);
			long long objid;
			data->Read(objid);
			Logger::log("WRLD", "EQUIP", "Unequip object [" + std::to_string(objid) + "]", LOG_ALL);
			bool end;
			for (int k = 0; k < 5; k++) {
				data->Read(end);
			}

			//response
			GameMSG::unequipItem(s.activeCharId, objid);
		} break;
		case REQUEST_USE: {

			//Interaction
			//There has to be a flag somewhere in here, with the speedchat messages I found out that it is offset by one bit.
			//Normally all bits until there are 0, i'll check both sides to see if anything happens -> where the flag is
			bool flag1;
			data->Read(flag1);
			data->SetReadOffset(data->GetReadOffset() - 1);
			unsigned long long something3;
			data->Read(something3);
			bool flag;
			data->Read(flag);
			unsigned long long object;
			data->Read(object);

			ObjectInformation o = getObjectInformation(object);
			Logger::log("GMSG", "INTERACT", getObjectDescription(o, object));
			
			Logger::log("GMSG", "INTERACT", std::to_string(flag1));
			Logger::log("GMSG", "INTERACT", std::to_string(something3));
			Logger::log("GMSG", "INTERACT", std::to_string(flag));
			Logger::log("GMSG", "INTERACT", std::to_string(object));


			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s.activeCharId);

			bool b;
			for (unsigned char a = 0; a < 7; a++) data->Read(b);
			if (o.type == ObjectType::NONE) {
				//logic here needs to be redone
				//its either a npc or something is not working here
				try {
					Interact((long long)object, systemAddress);
					//object handled
				}
				catch (const std::exception&) {
					//object NOT handled
					Logger::log("GMSG", "INTERACT", "Can't handle Object");
				}
			}
			else {
				Logger::log("", "", "LALALA MANAGING INTERACT (-A)");
				handleObjectInteraction(o, systemAddress);
			}

			break;

		} break;
		case MISSION_DIALOGUE_OK: {
			bool bIsComplete;
			data->Read(bIsComplete);
			int MissionState;
			data->Read(MissionState);
			int missionID;
			data->Read(missionID);
			//The Responder is not required, it is the Char objid. That we can get from SessionsTable

			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s.activeCharId);

			//Write Accept In Database - If you allready have the mission, we just update the Status.
			if (bIsComplete) {
				//Write Update in Database
				Database::Query("UPDATE `missions` SET `status` = '" + std::to_string(MissionState) + "' WHERE `missions`.`character` = '" + std::to_string(s.activeCharId) + "' AND `missions`.`missionId` = '" + std::to_string(missionID) + "'");
				//Log
				Logger::log("WRL", "MISSION", "Mission Updated! Mission Id: " + std::to_string(missionID) + " - Char: " + std::to_string(s.activeCharId) + " - Status: " + std::to_string(MissionState), LOG_DEBUG);
				Chat::sendMythranInfo(s.activeCharId, "Missions are in BETA stage, if you get any troubles with it, try relog before posting bugs!.", "WARNING!");
				MissionCommandHandler handler;
				SessionInfo *s = &SessionsTable::getClientSession(systemAddress);
				std::vector<std::wstring> param;
				param.push_back(L"Reward");
				param.push_back(std::to_wstring(missionID));
				handler.handleCommand(s, &param);
			}
			else {
				auto qr = Database::Query("SELECT * FROM `missions` WHERE `character` = '" + std::to_string(s.activeCharId) + "' AND `missionId` = '" + std::to_string(missionID) + "'");
				if (mysql_num_rows(qr) <= 0) {
					//Inserct In Databse
					Database::Query("INSERT INTO `missions` (`id`, `character`, `missionid`, `time`, `count`, `status`) VALUES (NULL, '" + std::to_string(s.activeCharId) + "', '" + std::to_string(missionID) + "', CURRENT_TIMESTAMP, '0', '" + std::to_string(MissionState) + "');");
					//Log
					Logger::log("WRL", "MISSION", "Mission Accepted! Mission Id: " + std::to_string(missionID) + " - Char: " + std::to_string(s.activeCharId), LOG_DEBUG);
				}
			}
			RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(s.activeCharId, 254);
			ef->Write((unsigned long)missionID);
			ef->Write((unsigned long)2);
			ef->Write((bool)bIsComplete);
			WorldServer::sendPacket(ef, s.addr);
			break;
		}

		case CANCEL_MISSION:
		{
			int missionId;
			data->Read(missionId);
			bool resetCompleted;
			data->Read(resetCompleted);

			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s.activeCharId);

			//Log
			Logger::log("WRL", "MISSION", "Player: " + std::to_string(s.activeCharId) + " - has cancelled mission: " + std::to_string(missionId), LOG_DEBUG);

			//Delete Mission From Database
			Database::Query("DELETE FROM `missions` WHERE `missions`.`character` = '" + std::to_string(s.activeCharId) + "' AND `missions`.`missionid` = '" + std::to_string(missionId) + "';");
		}
		break;

		case RESET_MISSIONS:
		{
			int missionId;
			data->Read(missionId);

			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(s.activeCharId);

			//Log
			Logger::log("WRL", "MISSION", "Player: " + std::to_string(s.activeCharId) + " - has reset mission: " + std::to_string(missionId), LOG_DEBUG);

			//Reset Mission In DB
			auto qr = Database::Query("SELECT * FROM `missions` WHERE `missions`.`character` = '" + std::to_string(s.activeCharId) + "' AND `missions`.`missionid` = '" + std::to_string(missionId) + "';");
			if (mysql_num_rows(qr) > 0) {
				Database::Query("UPDATE `missions` SET `count` = '0', `status` = '0' WHERE `missions`.`character` = '" + std::to_string(s.activeCharId) + "' AND `missions.`missionid` = '" + std::to_string(missionId) + "';");
			}
			else {
				//Rows = 0?
			}
		}
		break;

		case PLAYER_LOADED:
		{
			//happens on client ready
			long long object;
			data->Read(object);
			//cout << "Object: " << object << std::endl;
			//ObjectInformation o = getObjectInformation(object);
			//Logger::log("WRLD", "LOAD?", getObjectDescription(o, object));
			//Some sort of loading, L8: objid

			RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(object, 1043);
			ef->Write((bool)false);
			ef->Write((bool)true);
			ef->Write((bool)false);
			WorldServer::sendPacket(ef, systemAddress);

			RakNet::BitStream * ef2 = WorldServerPackets::InitGameMessage(object, 0x021a);
			ef2->Write((bool)false);
			WorldServer::sendPacket(ef2, systemAddress);

		}
		break;

		case MESSAGE_BOX_RESPOND: {

			//read packet
			//Some sort of script
			//e.g. Closing a plaque
			unsigned long something;
			data->Read(something);
			unsigned long len;
			data->Read(len);
			std::vector<wchar_t> mV;
			mV.reserve(len);
			for (unsigned long k = 0; k < len; k++) {
				wchar_t mC;
				data->Read(mC);
				mV.push_back(mC);
			}
			std::wstring script(mV.begin(), mV.end());
			unsigned long something2;
			data->Read(something2);

			//response
			Logger::log("WRLD", "SCRIPT", "530: " + UtfConverter::ToUtf8(script));
			//if (something > 0) cout << " [1:" << something << "]";
			//if (something2 > 0) cout << " [2:" << something2 << "]";
			//cout << endl;
		} break;
		case USE_NON_EQUIPMENT_ITEM: {

			//read packet
			long long aobjid;
			data->Read(aobjid);

			//response
			Logger::log("WRLD", "GMSG", "Open object " + std::to_string(aobjid));
		} break;
		case QUERY_PROPERTY_DATA: {

			//read packet

			//response
			//Logger::log("WRLD", "ZONELOAD", "Registering Object???", LOG_ALL);

		} break;
		case FIRE_EVENT_SERVER_SIDE: {

			//read packet
			SessionInfo s = SessionsTable::getClientSession(systemAddress);

			//Some sort of script
			//e.g. Closing a postbox
			//For this message, the objid is the ID of the postbox in question

			//Ok actually, this is called from within the LUA script for the post box
			//in L_MAIL_BOX_INTERACT.lua, with the postbox as game message object and the user later on
			//this means, that this here is the server endpoint for the lua function:
			//object:fireEventServerSide('text', object)
			//Which is exactly what we recieve here.

			unsigned long len;
			data->Read(len);
			std::vector<wchar_t> mV;
			mV.reserve(len);
			for (unsigned long k = 0; k < len; k++) {
				wchar_t mC;
				data->Read(mC);
				mV.push_back(mC);
			}
			std::wstring script(mV.begin(), mV.end());
			//Logger::log("WRLD", "SCRIPT", "770: " + UtfConverter::ToUtf8(script), LOG_DEBUG);
			bool f;
			for (unsigned char k = 0; k < 3; k++) {
				data->Read(f);
			}

			unsigned long long object;
			data->Read(object);
			ObjectInformation o = getObjectInformation(object);
			//Logger::log("WRLD", "SCRIPT", "Object: " + getObjectDescription(o, object), LOG_DEBUG);

			bool dat;
			for (unsigned char i = 0; i < 5; i++) {
				data->Read(dat);
			}

			//response
			bool foundHandler = true;
			if (script == L"toggleMail") {
				Mail::closeMailbox(s.activeCharId);
			}
			else if (script == L"achieve") {
				//die(s.activeCharId, true, true, 0.0F, L"big-shark-death", 0.0F, 0.0F, 0.0F, s.activeCharId, s.activeCharId);
				RakNet::BitStream * bs = WorldServerPackets::InitGameMessage(object, FIRE_EVENT_CLIENT_SIDE);
				bs->Write(L"achieve");
				bs->Write(object);
				bs->Write(0);
				bs->Write(-1);
				bs->Write(object);
				WorldServer::sendPacket(bs, systemAddress);
			}
			else if(script == L"physicsReady"){
				
			}
			else {
				std::string s((const char*)&script[0], sizeof(wchar_t) / sizeof(char)*script.size());
				Logger::log("GM", "PCKT", "New Fire Event Server Side: " + s);
				Logger::log("GM", "UKNW", std::to_string(object));
				Logger::log("GM", "UKNW", std::to_string(dat));
				foundHandler = false;
			}
			if(foundHandler)
				Logger::log("GM", "PCKT", "\"FireEventServerSide\" has been handled!");
		} break;
		case TOGGLE_GHOST_REFERENCE_OVERRIDE: {

			//read packet
			bool isStart;
			data->Read(isStart);
			bool dat;
			for (unsigned char i = 0; i < 7; i++) {
				data->Read(dat);
			}

			//response
			if (isStart) Logger::log("WRLD", "PARSER", "Camerapath started"); else Logger::log("WRLD", "PARSER", "Camerapath finished", LOG_ALL);
		} break;
		case SET_GHOST_REFERENCE_POSITION: {

			//read packet
			//Camera moving
			REPLICA_OBJECT_POSITION pos;
			data->Read(pos);

			//response
			std::stringstream str;
			str << "Camera-Position: (X:" << pos.posX << "|Y:" << pos.posY << "|Z:" << pos.posZ << ")";
			Logger::log("WRLD", "PARSER", str.str(), LOG_ALL);
		} break;
		case PARSE_CHAT_MESSAGE: {

			//read packet
			unsigned long unknown;
			data->Read(unknown);
			unsigned long len;
			data->Read(len);
			std::vector<wchar_t> msgVector;
			msgVector.reserve(len);
			for (unsigned long k = 0; k < len; k++){
				wchar_t mchr;
				data->Read(mchr);
				msgVector.push_back(mchr);
			}

			std::wstring message(msgVector.begin(), msgVector.end());

			//response
			if (message.substr(0, 1) == L"/"){
				ChatCommandManager::handleCommand(message, &s);
			}
			//Packet 1: 2365 (ZoneControl Object)
			//Packet 2: objid: 288300744895889662 LOT: 6635 SpawnerId: 72567767517768 (Mardolf the Orange)
			//Packet 3: objid: 288300744895889664 LOT: 6636 SpawnerId: 72567767517769 (Rad Eccles)
			//Packet 4: objid: 288300744895889669 LOT: 3495 SpawnerId: 72567767517772
			//Packet 5: objid: 288300744895899827 LOT: 6726 SpawnerId: 72567767517771
			//Packet 6: objid: 288300744895909034 LOT: 5637 SpawnerId: 3695535 NodeID: 1, Starts with Index 20 (position), x:-309.645782, y:288.356626, z: 70.64473 (Robot Dog Pet)
			//Packet 7: objid: 288300744895889606 LOT: 3646 SpawnerId: 3695538 NodeId: 3, 
			//Packet 8: objid: 288300744895930870 LOT: 9717 SpawnerId: 72567767517763,
			//Packet 9: PLAYER
		} break;
		case READY_FOR_UPDATES: {

			//read packet
			long long object;
			data->Read(object);
			ObjectInformation o = getObjectInformation(object);

			//response
			//Logger::log("WRLD", "OBJECT", getObjectDescription(o, object));
			//Some sort of loading, L8: objid
		} break;
		case HELPMENU_SMASH: {
			
			//read packet
			
			//response
			SessionInfo *s = &SessionsTable::getClientSession(systemAddress);
			die(s->activeCharId, false, false, -1, L"death", 0, 0, 0, 0, s->activeCharId);
		} break;
		case MATCH_REQUEST: //minigames
		{

			//read packet
			//This handles the requirements of a minigame for the server side.
			//Starting with 8 bytes of objectId?, then 4 bytes for the data length
			//Then that with a null byte terminator?
			//Then two 4 byte values
			unsigned long long object;
			data->Read(object);

			Logger::log("WRLD", "MINIGAME", "Cost?: objid: " + std::to_string(object), LOG_DEBUG);

			unsigned long len;
			data->Read(len);

			if (len > 0) {
				std::vector<wchar_t> mV;
				mV.reserve(len);
				for (unsigned long k = 0; k < len; k++) {
					wchar_t mC;
					data->Read(mC);
					mV.push_back(mC);
				}
				std::wstring script(mV.begin(), mV.end());
				//I guess this is a null terminator as it only appears with text content, but not included in length
				unsigned short nullT;
				data->Read(nullT);
				Logger::log("WRLD", "MINIGAME", "Data: " + UtfConverter::ToUtf8(script));
			}
			//These 8 bytes should be two values, since the only thing I found so far has content only in the 5th
			unsigned long dataA;
			unsigned long dataB;
			data->Read(dataA);
			data->Read(dataB);
			Logger::log("WRLD", "MINIGAME", "A: " + std::to_string(dataA) + ", B: " + std::to_string(dataB), LOG_DEBUG);
			
			//repsonse

		} break;
		case USED_INFORMATION_PLAQUE: {

			//read packet
			unsigned long long object;
			data->Read(object);
			Logger::log("WLRD", "UNKNOWN", "Objectid: " + std::to_string(object));

			//response

		} break;
		case LEVEL_UP: {

			//read packet
			SessionInfo s = SessionsTable::getClientSession(systemAddress);
			ListCharacterInfo cinfo = CharactersTable::getCharacterInfo(s.activeCharId);

			Database::Query("UPDATE `characters` SET `level` = '" + std::to_string(cinfo.info.level + 1) + "' WHERE `objectID` = '" + std::to_string(s.activeCharId) + "';");
			GameMSG::sendLevelUpMessage(s.activeCharId);
			//response
		} break;
		case HELPMENU_REPORT: {

			//read packet
			//I dont know what the first byte is, but everything after that is just ascii encoded
			unsigned long unknown;
			data->Read(unknown);
			char letter;
			bool test = true;
			std::string message = "";
			std::string otherInfo = "";

			char discard;
			while (data->Read(letter)) {
				if (test) {
					data->Read(discard);
					message += letter;
				}
				else {
					otherInfo += letter;
				}


				if (letter == 0x07) //if we have reached the end of the help string
				{
					test = false;
				}
			}

			//response
			Logger::log("WRLD", "HELP", "Recieved help message: " + message);
			Logger::log("WRLD", "HELP", "Help message info: " + otherInfo);
		}break;
		case PICKUP_ITEM: {

			//read packet
			SessionInfo *s = &SessionsTable::getClientSession(systemAddress);
			SessionInfo s1 = SessionsTable::getClientSession(systemAddress);
			unsigned long long lootId;
			unsigned long long playerId;

			data->Read(lootId);
			data->Read(playerId);

			//response
			ReplicaObject * loot = ObjectsManager::getObjectByID(lootId);

			std::vector<MISSION_DATA> mis = MissionsTable::getMissions(s1.activeCharId);
			if (lootId == 935 && mis[0].missionid == 173 && mis[0].missionStatus < 4) {
				mis[0].missioncount += 1;
				Database::Query("UPDATE `missions` SET `count` = '" + std::to_string(mis[0].missioncount) + "' WHERE `character` = '" + std::to_string(s1.activeCharId) + "' AND `missionId` = '173'");
				RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(s1.activeCharId, 851);
				ef->Write((unsigned long)0);
				ef->Write((std::string)"");
				ef->Write((std::string)"");
				WorldServer::sendPacket(ef, s1.addr);
			}

			unsigned long slot = -1;
			for (int i = 0; (slot == -1) && (i != 24); i++) {
				if (InventoryTable::getItemFromSlot(playerId, i, InventoryTable::getTab(loot->LOT)) == -1)
					slot = i;
			}

			if (slot == -1) {
				//no room
			}
			else {

				InventoryTable::insertItem(s->activeCharId, loot->LOT, 1, slot, false);

				RakNet::BitStream * ef = WorldServerPackets::InitGameMessage(s->activeCharId, 227);
				ef->Write((bool)false);
				ef->Write((bool)false);
				ef->Write((bool)true);
				ef->Write((bool)false);
				ef->Write((unsigned long)0);
				ef->Write((unsigned long)loot->LOT);
				ef->Write((bool)false);
				ef->Write((bool)false);
				ef->Write((bool)true);
				ef->Write((unsigned long)1);
				ef->Write((bool)true);
				ef->Write((unsigned long)0);
				ef->Write(lootId);
				ef->Write((float)0);
				ef->Write((float)0);
				ef->Write((float)0);
				ef->Write((bool)true);
				ef->Write((unsigned long)slot);

				WorldServer::sendPacket(ef, s->addr);
			}
		} break;
		case HAS_BEEN_COLLECTED: {
			long long playerid;
			data->Read(playerid);

			PlayerObject* po = (PlayerObject*)ObjectsManager::getObjectByID(s.activeCharId);

			std::vector<ReplicaObject*> oow = ObjectsManager::getObjectsOfWorld(s.zone);
			GenericObject *geo = NULL;
			for (int i = 0; i < oow.size(); i++) {
				GenericObject *go = (GenericObject*)oow.at(i);
				if (go != NULL) {
					if (go->inRange(po, 15)&&geo==NULL) {
						geo = go;
						break;
					}
				}
			}
			if (geo != NULL) {
				Chat::sendChatMessage(s.activeCharId, L"Picked up Collectible " + std::to_wstring(geo->objid));
				GameMSG::NotifyMission(173,4,false, s.addr);
			}
			else {
				Chat::sendChatMessage(s.activeCharId, L"No collectible in range found!");
			}
		} break;
		case SETFLAG: {
			bool value;
			int flagID;
			data->Read(value);
			data->Read(flagID);
			std::wstringstream wss;
			wss << L"Set Flag #" << std::to_wstring(flagID) << L": " << (value ? L"on" : L"off");
			Chat::sendChatMessage(s.activeCharId, wss.str());
		} break;
		default:
		{
			Logger::log("GMSG", "PARSE", "GameMessage not handled: " + std::to_string(messageID));
		}
	}
}
