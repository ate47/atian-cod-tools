#pragma once

namespace bo3 {
	typedef uint64_t XUID;

	struct userData_t {
		XUID xuid;
		char gamertag[32];
		bool isActive;
		bool isGuest;
		byte __pad[452];
	};

	enum eNetworkModes : uint32_t {
		MODE_NETWORK_OFFLINE = 0x0,
		MODE_NETWORK_SYSTEMLINK = 0x1,
		MODE_NETWORK_ONLINE = 0x2,
		MODE_NETWORK_COUNT = 0x3,
		MODE_NETWORK_INVALID = 0x3,
	};

	enum eModes : uint32_t {
		MODE_ZOMBIES = 0x0,
		MODE_MULTIPLAYER = 0x1,
		MODE_CAMPAIGN = 0x2,
		MODE_COUNT = 0x3,
		MODE_INVALID = 0x3,
		MODE_FIRST = 0x0,
	};

	enum dvarType_t {
		DVAR_TYPE_INVALID = 0x0,
		DVAR_TYPE_BOOL = 0x1,
		DVAR_TYPE_FLOAT = 0x2,
		DVAR_TYPE_FLOAT_2 = 0x3,
		DVAR_TYPE_FLOAT_3 = 0x4,
		DVAR_TYPE_FLOAT_4 = 0x5,
		DVAR_TYPE_INT = 0x6,
		DVAR_TYPE_ENUM = 0x7,
		DVAR_TYPE_STRING = 0x8,
		DVAR_TYPE_COLOR = 0x9,
		DVAR_TYPE_INT64 = 0xA,
		DVAR_TYPE_UINT64 = 0xB,
		DVAR_TYPE_LINEAR_COLOR_RGB = 0xC,
		DVAR_TYPE_COLOR_XYZ = 0xD,
		DVAR_TYPE_COLOR_LAB = 0xE,
		DVAR_TYPE_SESSIONMODE_BASE_DVAR = 0xF,
		DVAR_TYPE_COUNT = 0x10,
	};

	struct dvar_t;
	typedef float vec_t;

	union DvarValue {
		bool enabled;
		uint64_t u64;
		uint32_t u32;
		float f;
		byte color[4];
		dvar_t* indirect[3];
	};

	union DvarLimits {
		struct {
			int stringCount;
			const char** strings;
		} enumeration;
		struct {
			int min;
			int max;
		} integer;
		struct {
			int64_t min;
			int64_t max;
		} integer64;
		struct{
			uint64_t min;
			uint64_t max;
		} unsignedInt64;
		struct {
			float min;
			float max;
		} value;
		struct {
			vec_t min;
			vec_t max;
		} vector;
	};
	static_assert(sizeof(DvarLimits) == 0x10, "DvarLimits size mismatch");

	struct dvar_t {
		uint32_t name;
		const char* debugName;
		const char* description;
		unsigned int flags;
		dvarType_t type;
		bool modified;
		DvarValue current;
		DvarValue latched;
		DvarValue reset;
		DvarLimits domain;
		dvar_t* hashNext;
	};
	static_assert(sizeof(dvar_t) == 0x88, "dvar_t size mismatch");

	struct FixedClientInfo {
		XUID xuid;
		char gamertag[32];
		byte __pad[168];
	};

	struct ActiveClient {
		byte mutableClientInfo[1040];
		FixedClientInfo fixedClientInfo;
	};

	struct MatchInfo {
		uint32_t score;
	};

	struct VoiceChatInfo {
		int32_t lastCaughtTalking;
		int32_t connectivityBits;
		bool isGameRestricted;
	};

	struct SessionClient {
		XUID xuid;
		ActiveClient* activeClient;
		MatchInfo matchInfo;
		VoiceChatInfo voiceInfo;
		uint32_t joinOrder;
		uint64_t joinNonce;
	};
	static_assert(sizeof(SessionClient) == 0x30, "SessionClient size mismatch");

	enum LobbyType : int32_t {
		LOBBY_TYPE_INVALID = -1,
		LOBBY_TYPE_PRIVATE = 0x0,
		LOBBY_TYPE_GAME = 0x1,
		LOBBY_TYPE_TRANSITION = 0x2,
		LOBBY_TYPE_COUNT = 0x3,
		LOBBY_TYPE_FIRST = 0x0,
		LOBBY_TYPE_LAST = 0x2,
		LOBBY_TYPE_AUTO = 0x3,
	};


}