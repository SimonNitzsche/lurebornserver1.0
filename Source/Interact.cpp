#include "Interact.h"
#include "Worlds.h"
#include "ReplicaObject.h"
#include "RakNet\BitStream.h"
#include "CharactersDB.h"
#include "AccountsDB.h"
#include "WorldServer.h"
#include "Logger.h"
#include "Packet.h"
#include "WorldConnection.h"
#include "GameMessages.h"

Interact::Interact(long long objid, SystemAddress systemAddress) {
	try {
		SessionInfo s = SessionsTable::getClientSession(systemAddress);
		long long charid = s.activeCharId;
		auto o = ObjectsManager::getObjectByID(objid);
		unsigned long lot = (o!=NULL)?o->LOT:0;
		Logger::log("INTR", "INTR", "LOT: " + std::to_string(lot));

		switch (lot) {
		case 4009: {
			Logger::log("INTR", "INTERACT", "Player Interacted with Bob!", LOG_NORMAL);
			std::vector<MISSION_DATA> mis = MissionsTable::getMissions(s.activeCharId);

			if (mis.size() > 0) {
				MISSION_DATA cm = mis[0];
				if (cm.missionid == 1727 && cm.missionStatus < 4) {
					GameMSG::OfferMission(1727, systemAddress, objid);
				}
				else if (cm.missionid == 1727 && cm.missionStatus >= 4) {
					MISSION_DATA cm1 = mis[0];
					if (cm1.missionid == 1727) {
						GameMSG::OfferMission(173, systemAddress, objid);
					}
				}
				else if (cm.missionid == 173 && cm.missionStatus < 4) {
					GameMSG::OfferMission(173, systemAddress, objid);
				}
			}
		}
				   break;

		case 14349: {
			Logger::log("INTR", "INTERACT", "Player Interacted with Jett Moonshot!", LOG_NORMAL);
			GameMSG::OfferMission(1727, systemAddress, objid);
		}
					break;

		case 9980: {
			Logger::log("INTR", "INTERACT", "Player Interacted With Rocket Builder!", LOG_NORMAL);
		}
				   break;

		case 5997: {
			Logger::log("INTR", "INTERACT", "Player Interacted With Sky Lane!", LOG_NORMAL);
			std::vector<MISSION_DATA> mis = MissionsTable::getMissions(s.activeCharId);

			if (mis.size() > 0) {
				if (mis[0].missionid == 660 && mis[0].missionStatus < 4) {
					GameMSG::OfferMission(660, systemAddress, objid);
				}
				else if (mis[0].missionid == 660 && mis[0].missionStatus >= 4) {
					GameMSG::OfferMission(1895, systemAddress, objid);
				}
				else if (mis[0].missionid == 1896 && mis[0].missionStatus < 4) {
					GameMSG::OfferMission(1896, systemAddress, objid);
				}
				else if (mis[0].missionid == 1896 && mis[0].missionStatus >= 4) {
					GameMSG::OfferMission(308, systemAddress, objid);
				}
				else if (mis[0].missionid = 308 && mis[0].missionStatus < 4) {
					GameMSG::OfferMission(308, systemAddress, objid);
				}
				else if (mis[0].missionid = 308 && mis[0].missionStatus >= 4) {
					GameMSG::OfferMission(1732, systemAddress, objid);
				}
			}
		}
				   break;

		default: {
			//throw new std::exception();
		}
		}
	}catch(const std::exception&){}
}