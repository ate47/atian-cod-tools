#pragma once

namespace bo4 {
    enum XAssetType : byte {
        ASSET_TYPE_PHYSPRESET = 0,
        ASSET_TYPE_PHYSCONSTRAINTS = 1,
        ASSET_TYPE_DESTRUCTIBLEDEF = 2,
        ASSET_TYPE_XANIM = 3,
        ASSET_TYPE_XMODEL = 4,
        ASSET_TYPE_XMODELMESH = 5,
        ASSET_TYPE_MATERIAL = 6,
        ASSET_TYPE_COMPUTE_SHADER_SET = 7,
        ASSET_TYPE_TECHNIQUE_SET = 8,
        ASSET_TYPE_IMAGE = 9,
        ASSET_TYPE_SOUND = 10,
        ASSET_TYPE_CLIPMAP = 11,
        ASSET_TYPE_COMWORLD = 12,
        ASSET_TYPE_GAMEWORLD = 13,
        ASSET_TYPE_GFXWORLD = 14,
        ASSET_TYPE_FONTICON = 15,
        ASSET_TYPE_LOCALIZE_ENTRY = 16,
        ASSET_TYPE_LOCALIZE_LIST = 17,
        ASSET_TYPE_GESTURE = 18,
        ASSET_TYPE_GESTURE_TABLE = 19,
        ASSET_TYPE_WEAPON = 20,
        ASSET_TYPE_WEAPON_FULL = 21,
        ASSET_TYPE_WEAPON_TUNABLES = 22,
        ASSET_TYPE_CGMEDIA = 23,
        ASSET_TYPE_PLAYERSOUNDS = 24,
        ASSET_TYPE_PLAYERFX = 25,
        ASSET_TYPE_SHAREDWEAPONSOUNDS = 26,
        ASSET_TYPE_ATTACHMENT = 27,
        ASSET_TYPE_ATTACHMENT_UNIQUE = 28,
        ASSET_TYPE_WEAPON_CAMO = 29,
        ASSET_TYPE_CUSTOMIZATION_TABLE = 30,
        ASSET_TYPE_CUSTOMIZATION_TABLE_FE_IMAGES = 31,
        ASSET_TYPE_SNDDRIVER_GLOBALS = 32,
        ASSET_TYPE_FX = 33,
        ASSET_TYPE_TAGFX = 34,
        ASSET_TYPE_KLF = 35,
        ASSET_TYPE_IMPACT_FX = 36,
        ASSET_TYPE_IMPACT_SOUND = 37,
        ASSET_TYPE_AITYPE = 38,
        ASSET_TYPE_charACTER = 39,
        ASSET_TYPE_XMODELALIAS = 40,
        ASSET_TYPE_RAWFILE = 41,
        ASSET_TYPE_XANIM_TREE = 42,
        ASSET_TYPE_STRINGTABLE = 43,
        ASSET_TYPE_STRUCTURED_TABLE = 44,
        ASSET_TYPE_LEADERBOARD = 45,
        ASSET_TYPE_DDL = 46,
        ASSET_TYPE_GLASSES = 47,
        ASSET_TYPE_SCRIPTPARSETREE = 48,
        ASSET_TYPE_SCRIPTPARSETREEDBG = 49,
        ASSET_TYPE_SCRIPTPARSETREEFORCED = 50,
        ASSET_TYPE_KEYVALUEPAIRS = 51,
        ASSET_TYPE_VEHICLEDEF = 52,
        ASSET_TYPE_TRACER = 53,
        ASSET_TYPE_SURFACEFX_TABLE = 54,
        ASSET_TYPE_SURFACESOUNDDEF = 55,
        ASSET_TYPE_FOOTSTEP_TABLE = 56,
        ASSET_TYPE_ENTITYFXIMPACTS = 57,
        ASSET_TYPE_ENTITYSOUNDIMPACTS = 58,
        ASSET_TYPE_ZBARRIER = 59,
        ASSET_TYPE_VEHICLEFXDEF = 60,
        ASSET_TYPE_VEHICLESOUNDDEF = 61,
        ASSET_TYPE_TYPEINFO = 62,
        ASSET_TYPE_SCRIPTBUNDLE = 63,
        ASSET_TYPE_SCRIPTBUNDLELIST = 64,
        ASSET_TYPE_RUMBLE = 65,
        ASSET_TYPE_BULLETPENETRATION = 66,
        ASSET_TYPE_LOCDMGTABLE = 67,
        ASSET_TYPE_AIMTABLE = 68,
        ASSET_TYPE_SHOOTTABLE = 69,
        ASSET_TYPE_PLAYERGLOBALTUNABLES = 70,
        ASSET_TYPE_ANIMSELECTORTABLESET = 71,
        ASSET_TYPE_ANIMMAPPINGTABLE = 72,
        ASSET_TYPE_ANIMSTATEMACHINE = 73,
        ASSET_TYPE_BEHAVIORTREE = 74,
        ASSET_TYPE_BEHAVIORSTATEMACHINE = 75,
        ASSET_TYPE_TTF = 76,
        ASSET_TYPE_SANIM = 77,
        ASSET_TYPE_LIGHT_DESCRIPTION = 78,
        ASSET_TYPE_SHELLSHOCK = 79,
        ASSET_TYPE_STATUS_EFFECT = 80,
        ASSET_TYPE_CINEMATIC_CAMERA = 81,
        ASSET_TYPE_CINEMATIC_SEQUENCE = 82,
        ASSET_TYPE_SPECTATE_CAMERA = 83,
        ASSET_TYPE_XCAM = 84,
        ASSET_TYPE_BG_CACHE = 85,
        ASSET_TYPE_TEXTURE_COMBO = 86,
        ASSET_TYPE_FLAMETABLE = 87,
        ASSET_TYPE_BITFIELD = 88,
        ASSET_TYPE_MAPTABLE = 89,
        ASSET_TYPE_MAPTABLE_LIST = 90,
        ASSET_TYPE_MAPTABLE_LOADING_IMAGES = 91,
        ASSET_TYPE_MAPTABLE_PREVIEW_IMAGES = 92,
        ASSET_TYPE_MAPTABLEENTRY_LEVEL_ASSETS = 93,
        ASSET_TYPE_OBJECTIVE = 94,
        ASSET_TYPE_OBJECTIVE_LIST = 95,
        ASSET_TYPE_NAVMESH = 96,
        ASSET_TYPE_NAVVOLUME = 97,
        ASSET_TYPE_LASER = 98,
        ASSET_TYPE_BEAM = 99,
        ASSET_TYPE_STREAMER_HINT = 100,
        ASSET_TYPE_FLOWGRAPH = 101,
        ASSET_TYPE_POSTFXBUNDLE = 102,
        ASSET_TYPE_LUAFILE = 103,
        ASSET_TYPE_LUAFILE_DBG = 104,
        ASSET_TYPE_RENDEROVERRIDEBUNDLE = 105,
        ASSET_TYPE_STATIC_LEVEL_FX_LIST = 106,
        ASSET_TYPE_TRIGGER_LIST = 107,
        ASSET_TYPE_PLAYER_ROLE_TEMPLATE = 108,
        ASSET_TYPE_PLAYER_ROLE_CATEGORY_TABLE = 109,
        ASSET_TYPE_PLAYER_ROLE_CATEGORY = 110,
        ASSET_TYPE_charACTER_BODY_TYPE = 111,
        ASSET_TYPE_PLAYER_OUTFIT = 112,
        ASSET_TYPE_GAMETYPETABLE = 113,
        ASSET_TYPE_FEATURE = 114,
        ASSET_TYPE_FEATURETABLE = 115,
        ASSET_TYPE_UNLOCKABLE_ITEM = 116,
        ASSET_TYPE_UNLOCKABLE_ITEM_TABLE = 117,
        ASSET_TYPE_ENTITY_LIST = 118,
        ASSET_TYPE_PLAYLISTS = 119,
        ASSET_TYPE_PLAYLIST_GLOBAL_SETTINGS = 120,
        ASSET_TYPE_PLAYLIST_SCHEDULE = 121,
        ASSET_TYPE_MOTION_MATCHING_INPUT = 122,
        ASSET_TYPE_BLACKBOARD = 123,
        ASSET_TYPE_TACTICALQUERY = 124,
        ASSET_TYPE_PLAYER_MOVEMENT_TUNABLES = 125,
        ASSET_TYPE_HIERARCHICAL_TASK_NETWORK = 126,
        ASSET_TYPE_RAGDOLL = 127,
        ASSET_TYPE_STORAGEFILE = 128,
        ASSET_TYPE_STORAGEFILELIST = 129,
        ASSET_TYPE_charMIXER = 130,
        ASSET_TYPE_STOREPRODUCT = 131,
        ASSET_TYPE_STORECATEGORY = 132,
        ASSET_TYPE_STORECATEGORYLIST = 133,
        ASSET_TYPE_RANK = 134,
        ASSET_TYPE_RANKTABLE = 135,
        ASSET_TYPE_PRESTIGE = 136,
        ASSET_TYPE_PRESTIGETABLE = 137,
        ASSET_TYPE_FIRSTPARTYENTITLEMENT = 138,
        ASSET_TYPE_FIRSTPARTYENTITLEMENTLIST = 139,
        ASSET_TYPE_ENTITLEMENT = 140,
        ASSET_TYPE_ENTITLEMENTLIST = 141,
        ASSET_TYPE_SKU = 142,
        ASSET_TYPE_LABELSTORE = 143,
        ASSET_TYPE_LABELSTORELIST = 144,
        ASSET_TYPE_CPU_OCCLUSION_DATA = 145,
        ASSET_TYPE_LIGHTING = 146,
        ASSET_TYPE_STREAMERWORLD = 147,
        ASSET_TYPE_TALENT = 148,
        ASSET_TYPE_PLAYERTALENTTEMPLATE = 149,
        ASSET_TYPE_PLAYERANIMATION = 150,
        ASSET_TYPE_ERR_UNUSED = 151,
        ASSET_TYPE_TERRAINGFX = 152,
        ASSET_TYPE_HIGHLIGHTREELINFODEFINES = 153,
        ASSET_TYPE_HIGHLIGHTREELPROFILEWEIGHTING = 154,
        ASSET_TYPE_HIGHLIGHTREELSTARLEVELS = 155,
        ASSET_TYPE_DLOGEVENT = 156,
        ASSET_TYPE_RAWSTRING = 157,
        ASSET_TYPE_BALLISTICDESC = 158,
        ASSET_TYPE_STREAMKEY = 159,
        ASSET_TYPE_RENDERTARGETS = 160,
        ASSET_TYPE_DRAWNODES = 161,
        ASSET_TYPE_GROUPLODMODEL = 162,
        ASSET_TYPE_FXLIBRARYVOLUME = 163,
        ASSET_TYPE_ARENASEASONS = 164,
        ASSET_TYPE_SPRAYORGESTUREITEM = 165,
        ASSET_TYPE_SPRAYORGESTURELIST = 166,
        ASSET_TYPE_HWPLATFORM = 167,
        ASSET_TYPE_COUNT = 168,
        ASSET_TYPE_ASSETLIST = 168,
        ASSET_TYPE_REPORT = 169,
        ASSET_TYPE_FULL_COUNT = 170,
    };

    enum BGCacheTypes : byte
    {
        BG_CACHE_TYPE_INVALID = 0x0,
        BG_CACHE_TYPE_VEHICLE = 0x1,
        BG_CACHE_TYPE_MODEL = 0x2,
        BG_CACHE_TYPE_AITYPE = 0x3,
        BG_CACHE_TYPE_charACTER = 0x4,
        BG_CACHE_TYPE_XMODELALIAS = 0x5,
        BG_CACHE_TYPE_WEAPON = 0x6,
        BG_CACHE_TYPE_GESTURE = 0x7,
        BG_CACHE_TYPE_GESTURETABLE = 0x8,
        BG_CACHE_TYPE_ZBARRIER = 0x9,
        BG_CACHE_TYPE_RUMBLE = 0xA,
        BG_CACHE_TYPE_SHELLSHOCK = 0xB,
        BG_CACHE_TYPE_STATUSEFFECT = 0xC,
        BG_CACHE_TYPE_XCAM = 0xD,
        BG_CACHE_TYPE_DESTRUCTIBLE = 0xE,
        BG_CACHE_TYPE_STREAMERHINT = 0xF,
        BG_CACHE_TYPE_FLOWGRAPH = 0x10,
        BG_CACHE_TYPE_XANIM = 0x11,
        BG_CACHE_TYPE_SANIM = 0x12,
        BG_CACHE_TYPE_SCRIPTBUNDLE = 0x13,
        BG_CACHE_TYPE_TALENT = 0x14,
        BG_CACHE_TYPE_STATUSICON = 0x15,
        BG_CACHE_TYPE_LOCATIONSELECTOR = 0x16,
        BG_CACHE_TYPE_MENU = 0x17,
        BG_CACHE_TYPE_MATERIAL = 0x18,
        BG_CACHE_TYPE_STRING = 0x19,
        BG_CACHE_TYPE_EVENTSTRING = 0x1A,
        BG_CACHE_TYPE_MOVIEFILE = 0x1B,
        BG_CACHE_TYPE_OBJECTIVE = 0x1C,
        BG_CACHE_TYPE_FX = 0x1D,
        BG_CACHE_TYPE_LUI_MENU_DATA = 0x1E,
        BG_CACHE_TYPE_LUI_ELEM = 0x1F,
        BG_CACHE_TYPE_LUI_ELEM_UID = 0x20,
        BG_CACHE_TYPE_RADIANT_EXPLODER = 0x21,
        BG_CACHE_TYPE_SOUNDALIAS = 0x22,
        BG_CACHE_TYPE_CLIENT_FX = 0x23,
        BG_CACHE_TYPE_CLIENT_TAGFXSET = 0x24,
        BG_CACHE_TYPE_CLIENT_LUI_ELEM = 0x25,
        BG_CACHE_TYPE_CLIENT_LUI_ELEM_UID = 0x26,
        BG_CACHE_TYPE_REQUIRES_IMPLEMENTS = 0x27,
        BG_CACHE_TYPE_COUNT
    };

    struct XHash {
        uint64_t name;
        const char* ptr{};
    };

	typedef uint32_t ScrVarIndex_t;
	typedef uint64_t ScrVarNameIndex_t;

	struct GSC_IMPORT_ITEM {
		uint32_t name;
		uint32_t name_space;
		uint16_t num_address;
		uint8_t param_count;
		uint8_t flags;
	};

	enum GSC_EXPORT_FLAGS : byte {
		GEF_LINKED = 0x01,
		GEF_AUTOEXEC = 0x02,
		GEF_PRIVATE = 0x04,
		GEF_CLASS_MEMBER = 0x08,
		GEF_CLASS_DESTRUCTOR = 0x10,
		GEF_VE = 0x20,
		GEF_EVENT = 0x40,
		GEF_CLASS_LINKED = 0x80,
		GEF_CLASS_VTABLE = 0x86
	};

	enum GSC_IMPORT_FLAGS : byte {
		GIF_FUNC_METHOD = 0x1,
		GIF_FUNCTION = 0x2,
		GIF_FUNCTION_THREAD = 0x3,
		GIF_FUNCTION_CHILDTHREAD = 0x4,
		GIF_METHOD = 0x5,
		GIF_METHOD_THREAD = 0x6,
		GIF_METHOD_CHILDTHREAD = 0x7,
		GIF_CALLTYPE_MASK = 0xF,
		GIF_DEV_CALL = 0x10,
		GIF_GET_CALL = 0x20,
	};

	struct GSC_EXPORT_ITEM {
		uint32_t checksum;
		uint32_t address;
		uint32_t name;
		uint32_t name_space;
		uint32_t callback_event;
		uint8_t param_count;
		uint8_t flags;
	};

	struct GSC_OBJ {
        byte magic[8];
        int32_t crc;
        int32_t pad;
        uint64_t name;
        int32_t include_offset;
        uint16_t string_count;
        uint16_t exports_count;
        int32_t cseg_offset;
        int32_t string_offset;
        int16_t imports_count;
        uint16_t fixup_count;
        int32_t devblock_string_offset;
        int32_t export_table_offset;
        int32_t ukn34;
        int32_t imports_offset;
        uint16_t globalvar_count;
        int32_t fixup_offset;
        int32_t globalvar_offset;
        int32_t script_size;
        int32_t requires_implements_offset;
        int16_t ukn50;
        int16_t devblock_string_count;
        int32_t cseg_size;
        uint16_t include_count;
        byte ukn5a;
        byte requires_implements_count;
	};

	typedef float vec_t;
	typedef vec_t vec2_t[2];
	typedef vec_t vec3_t[3];
	typedef vec_t vec4_t[4];

	enum scriptInstance_t : int32_t {
		SCRIPTINSTANCE_SERVER = 0x0,
		SCRIPTINSTANCE_CLIENT = 0x1,
		SCRIPTINSTANCE_MAX = 0x2,
	};

	typedef void (*BuiltinFunction)(scriptInstance_t);

	struct BO4_BuiltinFunctionDef {
		uint32_t canonId;
		uint32_t min_args;
		uint32_t max_args;
		BuiltinFunction actionFunc;
		uint32_t type;
	};

	struct __declspec(align(4)) scrVarGlobalVars_t {
		uint32_t name;
		ScrVarIndex_t id;
		bool persist;
	};

	enum ScrVarType_t : uint32_t {
		TYPE_UNDEFINED = 0x0,
		TYPE_POINTER = 0x1,
		TYPE_STRING = 0x2,
		TYPE_VECTOR = 0x3,
		TYPE_HASH = 0x4,
		TYPE_FLOAT = 0x5,
		TYPE_INTEGER = 0x6,
		TYPE_UINTPTR = 0x7,
		TYPE_ENTITY_OFFSET = 0x8,
		TYPE_CODEPOS = 0x9,
		TYPE_PRECODEPOS = 0xA,
		TYPE_API_FUNCTION = 0xB,
		TYPE_SCRIPT_FUNCTION = 0xC,
		TYPE_STACK = 0xD,
		TYPE_THREAD = 0xE,
		TYPE_NOTIFY_THREAD = 0xF,
		TYPE_TIME_THREAD = 0x10,
		TYPE_FRAME_THREAD = 0x11,
		TYPE_CHILD_THREAD = 0x12,
		TYPE_CLASS = 0x13,
		TYPE_SHARED_STRUCT = 0x14,
		TYPE_STRUCT = 0x15,
		TYPE_REMOVED_ENTITY = 0x16,
		TYPE_ENTITY = 0x17,
		TYPE_ARRAY = 0x18,
		TYPE_REMOVED_THREAD = 0x19,
		TYPE_FREE = 0x1a,
		TYPE_THREAD_LIST = 0x1b,
		TYPE_ENT_LIST = 0x1c,
		TYPE_COUNT
	};

	struct BO4_scrVarPub {
		const char* fieldBuffer;
		const char* error_message;
		byte* programBuffer;
		byte* endScriptBuffer;
		byte* programHunkUser; // HunkUser
		scrVarGlobalVars_t globalVars[16];
		ScrVarNameIndex_t entFieldNameIndex;
		ScrVarIndex_t freeEntList;
		ScrVarIndex_t tempVariable;
		uint32_t checksum;
		uint32_t entId;
		uint32_t varHighWatermark;
		uint32_t numScriptThreads;
		uint32_t numVarAllocations;
		int32_t varHighWatermarkId;
	};

	union ScrVarValueUnion_t
	{
		int64_t intValue;
		uintptr_t uintptrValue;
		float floatValue;
		int32_t stringValue;
		const float* vectorValue;
		byte* codePosValue;
		ScrVarIndex_t pointerValue;
	};

	struct ScrVarValue_t {
		ScrVarValueUnion_t u;
		ScrVarType_t type;
	};

	struct ScrVar_t_Info {
		uint32_t nameType : 3;
		uint32_t flags : 5;
		uint32_t refCount : 24;
	};

	struct ScrVar_t {
		ScrVarNameIndex_t nameIndex;
		ScrVar_t_Info _anon_0;
		ScrVarIndex_t nextSibling;
		ScrVarIndex_t prevSibling;
		ScrVarIndex_t parentId;
		ScrVarIndex_t nameSearchHashList;
		uint32_t pad0;
	};

	union ScrVarObjectInfo1_t {
		uint64_t object_o;
		unsigned int size;
		ScrVarIndex_t nextEntId;
		ScrVarIndex_t self;
		ScrVarIndex_t free;
	};

	union ScrVarObjectInfo2_t {
		uint32_t object_w;
		ScrVarIndex_t stackId;
	};

	struct function_stack_t {
		byte* pos;
		ScrVarValue_t* top;
		ScrVarValue_t* startTop;
		ScrVarIndex_t threadId;
		uint16_t localVarCount;
		uint16_t profileInfoCount;
	};

	struct function_frame_t {
		function_stack_t fs;
	};

	struct ScrVmContext_t {
		ScrVarIndex_t fieldValueId;
		ScrVarIndex_t objectId;
		byte* lastGoodPos;
		ScrVarValue_t* lastGoodTop;
	};

	typedef void (*VM_OP_FUNC)(scriptInstance_t, function_stack_t*, ScrVmContext_t*, bool*);

	struct scrVarGlob_t {
		ScrVarIndex_t* scriptNameSearchHashList;
		ScrVar_t* scriptVariables;
		ScrVarObjectInfo1_t* scriptVariablesObjectInfo1;
		ScrVarObjectInfo2_t* scriptVariablesObjectInfo2;
		ScrVarValue_t* scriptValues;
	};

	struct scrVmPub_t {
		void* unk0;
		void* unk8;
		void* executionQueueHeap; // HunkUser
		void* timeByValueQueue; // VmExecutionQueueData_t
		void* timeByThreadQueue[1024]; // VmExecutionQueue_t
		void* frameByValueQueue; // VmExecutionQueueData_t
		void* frameByThreadQueue[1024]; // VmExecutionQueue_t
		void* timeoutByValueList; // VmExecutionQueueData_t
		void* timeoutByThreadList[1024]; // VmExecutionQueue_t
		void* notifyByObjectQueue[1024]; // VmExecutionNotifyQueue_t
		void* notifyByThreadQueue[1024]; // VmExecutionNotifyQueue_t
		void* endonByObjectList[1024]; // VmExecutionNotifyQueue_t
		void* endonByThreadList[1024]; // VmExecutionNotifyQueue_t
		ScrVarIndex_t* localVars;
		ScrVarValue_t* maxstack;
		function_frame_t* function_frame;
		ScrVarValue_t* top;
		function_frame_t function_frame_start[64];
		ScrVarValue_t stack[2048];
		uint32_t time;
		uint32_t frame;
		int function_count;
		int callNesting;
		unsigned int inparamcount;
		bool showError;
		bool systemInitialized;
		bool vmInitialized;
		bool isShutdown;
	};

	struct objFileInfo_t {
		GSC_OBJ* activeVersion;
		int slot;
		int refCount;
		uint32_t groupId;
	};

	struct HksGlobal {};
	struct HksCallStackActivationRecord {};
	struct hksInstruction {};
	struct HksUpvalue {};
	typedef void* HksErrorhandler;
	struct lua_State;

	struct HksCallstack {
		HksCallStackActivationRecord* m_records;
		HksCallStackActivationRecord* m_lastrecord;
		HksCallStackActivationRecord* m_current;
		const hksInstruction* m_current_lua_pc;
		const hksInstruction* m_hook_return_addr;
		int32_t m_hook_level;
	};
	struct HksObject {
		uint32_t t;
		union {
			void* ptr;
			float number;
			int32_t boolean;
			uint32_t native;
			lua_State* thread;
		} v;
	};
	struct hks_api_stack {
		HksObject* top;
		HksObject* base;
		HksObject* alloc_top;
		HksObject* bottom;
	};

	struct lua_State {
		// hks::GenericChunkHeader
		size_t m_flags;
		// hks::ChunkHeader
		void* m_next;

		HksGlobal* m_global;
		HksCallstack m_callStack;
		hks_api_stack m_apistack;
		HksUpvalue* pending;
		HksObject globals;
		HksObject m_cEnv;
		HksErrorhandler m_callsites;
		int32_t m_numberOfCCalls;
		byte* m_context;
		// ...
	};
}