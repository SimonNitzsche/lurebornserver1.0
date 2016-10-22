#pragma once

enum MissionState : unsigned short {
	MISSION_AVAILABLE = 1, //0 or 1 (not sure)
	MISSION_ACTIVE,
	MISSION_READY_TO_COMPLETE,
	MISSION_COMPLETE,
	MISSION_FAIL,
	MISSION_READY_TO_COMPLETE_REPORTED,
	MISSION_UNKNOWN8=8 //? this is used by the imagination mission
};