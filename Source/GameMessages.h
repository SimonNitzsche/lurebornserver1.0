#pragma once
#include "World.h"
#include "Common.h"
#include "ReplicaComponents.h"

#include <string>

enum ObjectType{
	NONE = -1,
	LaunchPad,
	PostBox,
	Binoculars,
	Minigame,
	Plaque,
	Portal,
	Vault,
	WaterPot,
	Machine,
	Object
};

enum GameMessage : long {
	TELEPORT = 0x0013,
	DROP_CLIENT_LOOT = 0x001e,
	DIE = 0x0025,
	REQUEST_DIE = 0x0026,
	PLAY_EMOTE = 0x0029,
	PRELOAD_ANIMATION = 0x002a,
	PLAY_ANIMATION = 0x002b,
	CONTROL_BEHAVIORS = 0x0030,
	SET_NAME = 0x0048,
	ECHO_START_SKILL = 0x0076,
	START_SKILL = 0x0077,
	CASTER_DEAD = 0x0078,
	VERIFY_ACK = 0x0079,
	SELECT_SKILL = 0x007c,
	ADD_SKILL = 0x007f,
	REMOVE_SKILL = 0x0080,
	SET_CURRENCY = 0x0085,
	PICKUP_CURRENCY = 0x0089,
	PICKUP_ITEM = 0x008b,
	TEAM_PICKUP_ITEM = 0x008c,
	PLAY_FX_EFFECT = 0x009a,
	STOP_FX_EFFECT = 0x009b,
	REQUEST_RESURRECT = 0x009f,
	RESURRECT = 0x00a0,
	POP_EQUIPPED_ITEMS_STATE = 0x00c0,
	SET_STUNNED = 0x00c6,
	SET_STUNNED_IMMUNITY = 0x00c8,
	KNOCKBACK = 0x00ca,
	REBUILD_CANCEL = 0x00d1,
	ENABLE_REBUILD = 0x00d5,
	MOVE_ITEM_IN_INVENTORY = 0x00e0,
	ADD_ITEM_TO_INVENTORY_CLIENT_SYNC = 0x00e3,
	REMOVE_ITEM_FROM_INVENTORY = 0x00e6,
	EQUIP_INVENTORY = 0x00e7,
	UNEQUIP_INVENTORY = 0x00e9,
	OFFER_MISSION = 0x00f8,
	RESOND_TO_MISSION = 0x00f9,
	NOTIFY_MISSION = 0x00fe,
	REBUILD_NOTIFY_STATE = 0x0150,
	TOGGLE_INTERACTION_UPDATES = 0x0164,
	TERMINATE_INTERACTION = 0x0165,
	SERVER_TERMINATE_INTERACTION = 0x0166,
	REQUEST_USE = 0x016c,
	VENDOR_OPEN_WINDOW = 0x0173,
	EMOTE_PLAYED = 0x0173,
	BUY_FROM_VENDOR = 0x0175,
	SELL_TO_VENDOR = 0x0176,
	CANCEL_DONATION_ON_PLAYER = 0x017b,
	TEAM_SET_OFF_WORLD_FLAG = 0x017f,
	SET_INVENTORY_SIZE = 0x0185,
	ACKNOWLEDGE_POSSESSION = 0x0187,
	REQUEST_ACTIVITY_EXIT = 0x0194,
	ACTIVITY_ENTER = 0x0195,
	ACTIVITY_EXIT = 0x0196,
	ACTIVITY_START = 0x0197,
	ACTIVITY_STOP = 0x0198,
	SHOOTING_GALLERY_FIRE = 0x019b,
	REQUEST_VENDOR_STATUS_UPDATE = 0x01a0,
	CANCEL_MISSION = 0x01a2,
	RESET_MISSIONS = 0x01a3,
	NOTIFY_CLIENT_SHOOTING_GALLERY_SCORE = 0x01a9,
	CLIENT_ITEM_CONSUMED = 0x01ac,
	UPDATE_SHOOTING_GALLER_ROTATION = 0x01c0,
	SET_USER_CTRL_COMP_PAUSE = 0x01d2,
	SET_TOOLTIP_FLAG = 0x01d5,
	//SET_FLAG = 0x01d7,
	NOTIFY_CLIENT_FLAG_CHANGE = 0x01d8,
	HELP = 0x01db,
	VENDOR_TRANSACTION_RESULT = 0x01dc,
	HAS_BEEN_COLLECTED = 0X01e6,
	HAS_BEEN_COLLECTED_BY_CLIENT = 0X01e7,
	DISPLAY_CHAT_BUBBLE = 0x1f1,
	SPAWN_PET = 0x1f2,
	DESPAWN_PET = 0x01f3,
	PLAYER_LOADED = 0x01f9,
	//PLAYER_READY = 0x01fd,
	TRANSFER_TO_ZONE = 0x0204,
	TRANSFER_TO_ZONE_CHECKED_IM = 0x0205,
	INVALID_ZONE_TRANSFER_LIST = 0x0207,
	MISSION_DIALOGUE_OK = 0x0208,
	TRANSFER_TO_LAST_NONINSTANCE = 0x020f,
	DISPLAY_MESSAGE_BOX = 0x0211,
	MESSAGE_BOX_RESPOND = 0x0212,
	CHOICE_BOX_RESPOND = 0x0213,
	SMASH = 0x0219,
	UNSMASH = 0x021a,
	SET_GRAVITY_SCALE = 0x021d,
	PLACE_MODEL_RESPONSE = 0x0223,
	SET_JETPACK_MODE = 0x0231,
	REGISTER_PET_ID = 0x0235,
	REGISTER_PET_DB_ID = 0x0236,
	SHOW_ACTIVITY_COUNTDOWN = 0x0238,
	DISPLAY_TOOTIP = 0x0239,
	START_ACTIVITY_TIME = 0x0240,
	ACTIVITY_PAUSE = 0x025a,
	USE_NON_EQUIPMENT_ITEM = 0x025b,
	USE_ITEM_RESULT = 0x025f,
	FETCH_MODEL_METADATA_REQUEST = 0x027e,
	COMMAND_PET = 0x0280,
	PET_RESPONSE = 0x0281,
	REQUEST_ACTIVITY_SUMMARY_LEADERBOARD_DATA = 0x0288,
	SEND_ACTIVITY_SUMMARY_LEADERBOARD_DATA = 0x0289,
	CLIENT_NOTIFY_PET = 0x0293,
	NOTIFY_PET = 0x0294,
	NOTIFY_PET_TAMING_MINIGAME = 0x0295,
	START_SERVER_PET_MINIGAME_TIMER = 0x0296,
	CLIENT_EXIT_TAMING_MINIGAME = 0x0297,
	PET_TAMING_MINIGAME_RESULT = 0X029b,
	PET_TAMING_TRY_BUILD_RESULT = 0x029c,
	NOTIFY_TAMING_BUILD_SUCCESS = 0x02a1,
	NOTIFY_TAMING_MODEL_LOADED_ON_SERVER = 0x02a2,
	ADD_PET_TO_PLAYER = 0x02a9,
	REQUEST_SET_PET_NAME = 0x02ab,
	SET_PET_NAME = 0x02ac,
	PET_NAME_CHANGED = 0x02ae,
	SHOW_PET_ACTION_BUTTON = 0x02b5,
	SET_EMOTE_LOCK_STATE = 0x02b5,
	USE_ITEM_REQUIREMENTS_RESPONSE = 0x02bf,
	PLAY_EMBEDDED_EFFECT_ON_ALL_CLIENTS_NEAR_OBJECT = 0x02c9,
	QUERY_PROPERTY_DATA = 0x02cd,
	PROPERTY_EDITOR_BEGIN = 0x02d4,
	PROPERTY_EDITOR_END = 0x02d5,
	NOTIFY_CLIENT_ZONE_OBJECT = 0x02e1,
	UPDATE_REPUTATION = 0x02ea,
	PROPERTY_RENTAL_RESPONSE = 0x02ee,
	REQUEST_PLATFORM_RESYNC = 0x02f8,
	PLATFORM_RESYNC = 0x02f9,
	PLAY_CINEMATIC = 0x02fa,
	END_CINEMATIC = 0x02fb,
	CINEMATIC_UPDATE = 0x02fc,
	TOGGLE_GHOST_REFERENCE_OVERRIDE = 0x02ff,
	SET_GHOST_REFERENCE_POSITION = 0x0300,
	FIRE_EVENT_SERVER_SIDE = 0x0302,
	SCRIPT_NETWORK_VAR_UPDATE = 0x030d,
	UPDATE_MODEL_FROM_CLIENT = 0x0319,
	DELETE_MODEL_FROM_CLIENT = 0x031a,
	PLAY_ND_AUDIO_EMITTER = 0x0335,
	STOP_ND_AUDIO_EMITTER = 0x0336,
	ENTER_PROPERTY_1 = 0x0348,
	PROPERTY_ENTRANCE_SYNC = 0x034a,
	PARSE_CHAT_MESSAGE = 0x0352,
	SET_MISSION_TYPE_STATE = 0x0353,
	BROADCAST_TEXT_TO_CHATBOX = 0x035a,
	OPEN_PROPERTY_VENDOR = 0x035d,
	CLIENT_TRADE_REQUEST = 0x0364,
	SERVER_TRADE_INVITE = 0x0366,
	SERVER_TRADE_INITIAL_REPLY = 0x0369,
	SERVER_TRADE_FINAL_REPLY = 0x036a,
	CLIENT_TRADE_CANCEL = 0x036e,
	CLIENT_TRADE_ACCEPT = 0x0370,
	SERVER_TRADE_ACCEPT = 0x0374,
	READY_FOR_UPDATES = 0x0378,
	SET_LAST_CUSTOM_BUILD = 0x037a,
	GET_LAST_CUSTOM_BUILD = 0x037B,
	SET_IGNORE_PROJECTILE_COLLISION = 0x0387,
	ORIENT_TO_OBJECT = 0x0389,
	ORIENT_TO_POSITION = 0x038a,
	ORIENT_TO_ANGLE = 0x038B,
	PROPERTY_MODERATION_ACTION = 0x0393,
	PROPERTY_MODERATION_STATUS_UPDATE = 0x0395,
	BOUNCE_NOTIFICATION = 0x03a4,
	REQUEST_CLIENT_BOUNCE = 0x03a6,
	BOUNCER_ACTIVE_STATUS = 0x03ae,
	MOVE_INVENTORY_BATCH = 0x03bd,
	OBJECT_ACTIVATED_CLIENT = 0x03d4,
	SET_BBB_AUTOSAVE = 0x03e4,
	BBB_LOAD_ITEM_REQUEST = 0x03e8,
	BBB_SAVE_REQUEST = 0x03e9,
	BBB_RESET_METADATA_SOURCE_ITEM = 0x03ec,
	NOTIFY_CLIENT_OBJECT = 0x0412,
	DISPLAY_ZONE_SUMMARY = 0x0413,
	ZONE_SUMMARY_DISMISSED = 0x0414,
	MODIFY_PLAYER_ZONE_STATISTIC = 0x0416,
	ACTIVITY_STATE_CHANGE_REQUEST = 0x041d,
	START_BUILDING_WITH_ITEM = 0x0421,
	START_ARRANGING_WITH_ITEM = 0x0425,
	FINISH_ARRANGING_WITH_ITEM = 0x0426,
	DONE_ARRANGING_WITH_ITEM = 0x0427,
	SET_BUILD_MODE = 0x042c,
	BUILD_MODE_SET = 0x042d,
	BUILD_EXIT_CONFIRMATION = 0x0430,
	SET_BUILD_MODE_CONFIRMED = 0x0431,
	BUILD_MODEL_NOTIFICATION_REPORT = 0x0433,
	SET_MODEL_TO_BUILD = 0x0435,
	SPAWN_MODEL_BRICKS = 0x0436,
	NOTIFY_CLIENT_FAILED_PRECONDITION = 0x0439,
	MOVE_ITEM_BETWEEN_INVENTORY_TYPES = 0x0445,
	MISSION_DIALOGUE_CANCELLED = 0x0469,
	MODULE_ASSEMBLY_DB_DATA_FOR_CLIENT = 0x046b,
	MODULE_ASSEMBLY_QUERY_DATA = 0x046c,
	ECHO_SYNC_SKILL = 0x0478,
	SYNC_SKILL = 0x0479,
	REQUEST_SERVER_PROJECTILE_IMPACT = 0x047c,
	DO_CLIENT_PROJECTILE_IMPACT = 0x047f,
	SET_PLAYER_ALLOWED_RESPAWN = 0x048d,
	TOGGLE_SENDING_POSITION_UPDATES = 0x048e,
	PLACE_PROPERTY_MODEL = 0x0492,
	UI_MESSAGE_SERVER_TO_SINGLE_CLIENT = 0x04a0,
	UNCAST_SKILL = 0x04b6,
	FIRE_EVENT_CLIENT_SIDE = 0x04bd,
	CHANGE_OBJECT_WORLD_STATE = 0x004c7,
	VEHICLE_LOCK_INPUT = 0x04ce,
	VEHICLE_UNLOCK_INPUT = 0x04cf,
	RESYNC_ENQUIPMENT = 0x04d6,
	RACING_RESET_PLAYER_TO_LAST_RESET = 0x04e4,
	RACING_SET_PLAYER_RESET_INFO = 0x04e6,
	RACING_PLAYER_INFO_RESET_FINISHED = 0x04e7,
	LOCK_NODE_ROTATION = 0x04ec,
	VEHICLE_SET_WHEEL_LOCK_STATE = 0x04f9,
	NOTIFY_VEHICLE_OF_RACING_OBJECT = 0x04fc,
	PLAYER_REACHED_RESPAWN_CHECKPOINT = 0x0510,
	HANDLE_UGC_EQUIP_POST_DELETE_BASED_ON_EDIT_MODE = 0x0514,
	HANDLE_UGC_EQUIP_PRE_CREATE_BASED_ON_EDIT_MODE = 0x0515,
	PROPERTY_CONTENTS_FROM_CLIENT = 0x0519,
	MATCH_RESPONSE = 0x051d,
	MATCH_UPDATE = 0x051e,
	CHANGE_IDLE_FLAGS = 0x0053a,
	VEHICLE_ADD_PASSIVE_BOOST_ACTION = 0x053c,
	VEHICLE_REMOVE_PASSIVE_BOOST_ACTION = 0x053d,
	VEHICLE_NOTIFY_SERVER_ADD_PASSIVE_BOOST_ACTION = 0x053f,
	VEHICLE_NOTIFY_SERVER_REMOVE_PASSIVE_BOOST_ACTION = 0x053f,
	ZONE_PROPERTY_MODEL_ROTATED = 0x055a,
	ZONE_PROPERTY_MODEL_REMOVED_WHILE_EQUIPPED = 0x055b,
	ZONE_PROPERTY_MODEL_EQUIPPED = 0x055c,
	NOTIFY_RACING_CLIENT = 0x056e,
	RACING_PLAYER_LOADED = 0x0570,
	RACING_CLIENT_READY = 0x0571,
	RESET_PROPERTY_BEHAVIORS = 0x057e,
	SET_CONSUMABLE_ITEM = 0x0581,
	USED_INFORMATION_PLAQUE = 0x058b,
	SET_STATUS_IMMUNITY = 0x059b,
	ACTIVATE_BRICK_MODE = 0x059e,
	SET_PET_NAME_MODERATED = 0x05a8,
	CANCEL_SKILL_CAST = 0x05ab,
	MODIFY_LEGO_SCORE = 0x05b3,
	RESTORE_TO_POST_LOAD_STATS = 0x05bc,
	SET_RAIL_MOVEMENT = 0x05bf,
	START_RAIL_MOVEMENT = 0x05c0,
	CANCEL_RAIL_MOVEMENT = 0x05c2,
	CLIENT_RAIL_MOVEMENT_READY = 0x05c4,
	PLAYER_RAIL_ARRIVED_NOTIFICATION = 0x05c5,
	REQUEST_RAIL_ACTIVATOR_STATE = 0x05c7,
	NOTIFY_REQARD_MAILED = 0x05c8,
	UPDATE_PLAYER_STATISTIC = 0x05c9,
	MODIFY_GHOSTING_DISTANCE = 0x05cd,
	REQUERY_PROPERTY_MODELS = 0x05d3,
	MODULAR_ASSEMBLY_NIF_COMPLETED = 0x05da,
	GET_HOT_PROPERTY_DATA = 0x05e7,
	NOTIFY_NOT_ENOUGH_INV_SPACE = 0x05ec,
	NOTIFY_PROPERTY_OF_EDIT_MODE = 0x060a,
	UPDATE_PROPERTY_PERFORMANCE_COST = 0x060b,
	PROPERTY_ENTRANCE_BEGIN = 0x0611,
	//TEAM_SET_LEADER = 0x0615,
	TEAM_INVITE_CONFIRM = 0x0616,
	TEAM_GET_STATUS_RESPONSE = 0x0617,
	TEAM_ADD_PLAYER = 0x061a,
	TEAM_REMOVE_PLAYER = 0x061b,
	SET_EMOTES_ENABLED = 0x0629,
	SET_RESURRECT_RESTORE_VALUES = 0x0637,
	SET_PROPERTY_MODERATION_STATUS = 0x063a,
	UPDATE_PROPERTY_MODEL_COUNT = 0x063b,
	VEHICLE_NOTIFY_HIT_IMAGINATION_SERVER = 0x0646,
	VEHICLE_STOP_BOOST = 0x0651,
	START_CELEBRATION_EFFECT = 0x0652,
	CELEBRATION_COMPLETED = 0x0660,
	SET_LOCAL_TEAM = 0x0664,
	SERVER_DONE_LOADING_ALL_OBJECTS = 0x066a,
	ADD_BUFF = 0x066f,
	REMOVE_BUFF = 0x0670,
	PLAYER_SET_CAMERA_CYCLING_MODE = 0x068c,
	SET_MOUNT_INVENTORY_ID = 0x06be,
	NOTIFY_SERVER_LEVEL_PROCESSING_COMPLETE = 0x06c6,
	NOTIFY_LEVEL_REWARDS = 0x06c7,
	SERVER_CANCEL_MOVE_SKILL = 0x06d2,
	CLIENT_CANCEL_MOVE_SKILL = 0x06d3,
	DISMOUNT_COMPLETE = 0x06dc,
	MARK_INVENTORY_AS_ACTIVE = 0x06e7,

	// Unknown
	LEVEL_UP = 1734,
};

struct ObjectInformation{
	long long objid;
	ZoneId zone;
	ObjectType type;
	std::string info;
	ObjectInformation(long long id, ZoneId z, ObjectType t, std::string s = ""){
		this->objid = id;
		this->zone = z;
		this->type = t;
		this->info = s;
	}
	ObjectInformation(){
		this->objid = 0;
		this->zone = ZoneId::NO_ZONE;
		this->type = ObjectType::NONE;
		this->info = "";
	}
};

ObjectInformation getObjectInformation(long long objectid);
std::string getObjectDescription(ObjectInformation obj, long long objid = -1);
ZoneId getLaunchPadTarget(ObjectInformation obj);

class GameMSG {
public:

	//function to send game messages
	static void teleport(long long charid, COMPONENT1_POSITION position);
	static void dropClientLoot(long long charid, COMPONENT1_POSITION position, long currency, unsigned long LOT, long long objid, long long ownerid, long long sourceid, COMPONENT1_POSITION finalPosition);
	static void die(long long charid, bool clientDeath, bool spawnLoot, float coinSpawnTime, std::wstring deathType, float forceXZ, float forceY, float force, long long killerid, long long lootOwnerID);
	static void playAnimation(long long charid, std::wstring animationID, bool playImmediate = true, bool ignoreSubject = false);
	static void setName(long long charid, std::wstring name);
	static void addSkill(long long charid, unsigned long skillid, unsigned long slot);
	static void removeSkill(long long charid, unsigned long skillid);
	static void SetStunned(long long charid, std::string StateChangeType, bool cantAttack=false, bool cantAttackOCWA=false, bool cantEquip=false, bool cantEquipOCWA=false, bool cantInteract=false, bool cantInteractOCWA=false, bool cantJump=false, bool cantJumpOCWA=false, bool cantMove=false, bool cantMoveOCWA=false, bool cantTurn=false, bool cantTurnOCWA=false, bool cantUseItem=false, bool cantUseItemOCWA=false, bool dontTerminateInteract=false, bool bIgnoreImmunity=true);
	static void resurrect(long long charid, bool immediate);
	static void knockback(long long charid, long long casterid, COMPONENT1_POSITION position);
	static void addItemToInventoryClientSync(long long charid, long long objectID, long lotTemplate, long slotid, bool showFlyingLoot);
	static void displayMessageBox(long long charid, long long callbackClient, std::wstring identifier, std::wstring text, std::wstring userData);
	static void unsmash(long long charid);
	static void setJetPackMode(long long charid, bool bypassChecks, bool doHover, bool use);
	static void displayZoneSummary(long long charid, bool isProperty, bool isZoneStart);
	static void sendLevelUpMessage(long long charid);
	
	//jonny made this, dont know what this is. Not documented
	static void castActiveSkill(long long charid, unsigned long someNumber);

	static void verifyAck(SystemAddress & systemAddress, std::string sBitStream, unsigned long uiHandle, bool bDifferent);

	static void EchoSyncSkill(SystemAddress & systemAddress, std::string sBitStream, unsigned long uiBehaviorHandle, unsigned long uiSkillHandle, bool bDone);
	
	//parses the game messages
	static void parseGameMSG(unsigned short messageID, RakNet::BitStream *data, SystemAddress &systemAddress);
};