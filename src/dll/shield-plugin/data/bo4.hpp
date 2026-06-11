#pragma once
#include <games/bo4/pool.hpp>
#include <gsc/gsc_acts_debug.hpp>
#include <games/bo4/scriptinstance.hpp>

namespace bo4 {
	using XHash = ::XHash;
	using XAssetType = games::bo4::pool::XAssetType;
	using BGCacheTypes = games::bo4::pool::BGCacheTypes;
	using scriptInstance_t = games::bo4::scriptinstance::ScriptInstance;

	struct XAsset {
		XAssetType type;
		byte unk1;
		void* header;
	}; static_assert(sizeof(XAsset) == 0x10);

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

	enum fieldtype_t : int32_t {
		F_INT = 0x0,
		F_SHORT = 0x1,
		F_BYTE = 0x2,
		F_FLOAT = 0x3,
		F_LSTRING = 0x4,
		F_STRING = 0x5,
		F_HASH = 0x6,
		F_VECTOR = 0x7,
		F_ENTITY = 0x8,
		F_ENTHANDLE = 0x9,
		F_ACTOR = 0xA,
		F_SENTIENT = 0xB,
		F_SENTIENTHANDLE = 0xC,
		F_CLIENT = 0xD,
		F_PATHNODE = 0xE,
		F_ACTORGROUP = 0xF,
		F_OBJECT = 0x10,
		F_XMODEL_INDEX = 0x11,
		F_XMODEL = 0x12,
		F_BITFLAG = 0x13,
		F_BITFLAG64 = 0x14,
		F_FX = 0x15,
		F_WEAPON = 0x16,
		F_RUMBLE = 0x17,
		F_SCRIPTBUNDLE = 0x18,
		F_COUNT = 0x19,
	};
	template<typename T>
	constexpr fieldtype_t GetFieldType() {
		if constexpr (std::is_same_v<T, int32_t>) return F_INT;
		else if constexpr (std::is_same_v<T, int16_t>) return F_SHORT;
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, char> || std::is_same_v<T, bool>) return F_BYTE;
		else if constexpr (std::is_same_v<T, float>) return F_FLOAT;
		else if constexpr (std::is_same_v<T, XHash>) return F_HASH;
		else if constexpr (std::is_same_v<T, vec3_t>) return F_VECTOR;
		else static_assert(false, "Invalid field type");
	}


	enum LocalClientNum_t : int32_t {
		INVALID_LOCAL_CLIENT = -1,
		LOCAL_CLIENT_0 = 0x0,
		LOCAL_CLIENT_FIRST = 0x0,
		LOCAL_CLIENT_KEYBOARD_AND_MOUSE = 0x0,
		LOCAL_CLIENT_1 = 0x1,
		LOCAL_CLIENT_COUNT = 0x2, // bo4 pc is 2 for the count
	};

	enum ControllerIndex_t : int32_t {
		INVALID_CONTROLLER_PORT = -1,
		CONTROLLER_INDEX_0 = 0x0,
		CONTROLLER_INDEX_FIRST = 0x0,
		CONTROLLER_INDEX_1 = 0x1,
		CONTROLLER_INDEX_2 = 0x2,
		CONTROLLER_INDEX_3 = 0x3,
		CONTROLLER_INDEX_COUNT = 0x4,
	};

	enum ClassNum : uint16_t {
		CLASS_NUM_ENTITY = 0x0,
		CLASS_NUM_HUDELEM = 0x1,
		CLASS_NUM_PATHNODE = 0x2,
		CLASS_NUM_VEHICLENODE = 0x3,
		CLASS_NUM_TACPOINT = 0x4,
		CLASS_NUM_DYNENTITY = 0x5,
		CLASS_NUM_WEAPON = 0x6,
		CLASS_NUM_COUNT = 0x7,
	};
	union EntRefUnion {
		int32_t entnum;
		uint32_t hudElemIndex;
		uint64_t val;
	};


	struct scr_entref_t {
		EntRefUnion u;
		ClassNum classnum;
		LocalClientNum_t client;
	};
	static_assert(sizeof(scr_entref_t) == 0x10);


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
	typedef void (*VM_OP_ERR_FUNC)(scriptInstance_t, function_stack_t*, ScrVmContext_t*);
	
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
	struct lua_State;

	typedef void (*HksLogFunc)(lua_State*, const char*, ...);

	struct HksGlobal {
		byte _pad[1456];
		HksLogFunc m_logFunction;
	};
	struct HksClosure;
	struct HksCallStackActivationRecord {};
	struct hksInstruction {};
	struct HksUpvalue {};
	typedef void* HksErrorhandler;

	struct HksCallstack {
		HksCallStackActivationRecord* m_records;
		HksCallStackActivationRecord* m_lastrecord;
		HksCallStackActivationRecord* m_current;
		const hksInstruction* m_current_lua_pc;
		const hksInstruction* m_hook_return_addr;
		int32_t m_hook_level;
	};

	enum HksType : uint32_t {
		HKST_TNIL = 0x0,
		HKST_TBOOLEAN = 0x1,
		HKST_TLIGHTUSERDATA = 0x2,
		HKST_TNUMBER = 0x3,
		HKST_TSTRING = 0x4,
		HKST_TTABLE = 0x5,
		HKST_TFUNCTION = 0x6,
		HKST_TUSERDATA = 0x7,
		HKST_TTHREAD = 0x8,
		HKST_TIFUNCTION = 0x9,
		HKST_TCFUNCTION = 0xA,
		HKST_TUI64 = 0xB,
		HKST_TSTRUCT = 0xC,
		HKST_TXHASH = 0xD,
		HKST_COUNT = 0xE,
	};

	enum HksIndex {
		LUA_REGISTRY_INDEX = -10000,
		LUA_ENVIRON_INDEX = -10001,
		LUA_GLOBALS_INDEX = -10002,
	};

	struct HksObject {
		HksType t;
		union {
			void* ptr;
			float number;
			int32_t boolean;
			uint32_t native;
			uint64_t hash;
			lua_State* thread;
			HksClosure* closure;
		} v;
	};
	struct hks_api_stack {
		HksObject* top;
		HksObject* base;
		HksObject* alloc_top;
		HksObject* bottom;
	};

	enum lua_State_Status : uint32_t {
		NEW = 0x1,
		RUNNING = 0x2,
		YIELDED = 0x3,
		DEAD_ERROR = 0x4,
	};

	enum HksError : uint32_t {
		HKS_NO_ERROR = 0x0,
		LUA_ERRSYNTAX = 0xFFFFFFFC,
		LUA_ERRFILE = 0xFFFFFFFB,
		LUA_ERRRUN = 0xFFFFFF9C,
		LUA_ERRMEM = 0xFFFFFF38,
		LUA_ERRERR = 0xFFFFFED4,
		HKS_THROWING_ERROR = 0xFFFFFE0C,
		HKS_GC_YIELD = 0x1,
	};

	struct GenericChunkHeader {
		uint64_t m_flags;
	};

	struct ChunkHeader : GenericChunkHeader {
		ChunkHeader* m_next;
	};

	struct HksMethodDebugInfo { };

	template<typename T>
	struct HksArray {
		uint32_t count;
		T* data;
	};

	struct HksMethod : ChunkHeader {
		uint32_t hash;
		uint16_t num_upvals;
		uint16_t m_numRegisters;
		byte num_params;
		byte m_flags;
		HksArray<const hksInstruction> instructions;
		HksArray<void> constants;
		HksArray<void> children;
		HksMethodDebugInfo* m_debug;
	};
	static_assert(sizeof(HksMethod) == 0x58);

	struct HksClosure : ChunkHeader {
		HksMethod* m_method;
		// ...
	};

	struct lua_Debug {
		int32_t event;
		const char* name;
		const char* namewhat;
		const char* what;
		const char* source;
		int32_t currentline;
		int32_t nups;
		int32_t nparams;
		bool ishksfunc;
		int32_t linedefined;
		int32_t lastlinedefined;
		char short_src[512];
		int32_t callstack_level;
		bool is_tail_call;
	};
	static_assert(sizeof(lua_Debug) == 0x248);


	struct lua_State : ChunkHeader {
		HksGlobal* m_global;
		HksCallstack m_callStack;
		hks_api_stack m_apistack;
		HksUpvalue* pending;
		HksObject globals;
		HksObject m_cEnv;
		HksErrorhandler m_callsites;
		int32_t m_numberOfCCalls;
		byte* m_context;

		void* m_name;
		lua_State* m_next2;
		lua_State* m_nextStateStack;
		lua_State_Status m_status;
		HksError m_error;
		// ...
	};

	enum ScopedCriticalSectionType : int32_t {
		SCOPED_CRITSECT_NORMAL = 0x0,
		SCOPED_CRITSECT_DISABLED = 0x1,
		SCOPED_CRITSECT_RELEASE = 0x2,
		SCOPED_CRITSECT_TRY = 0x3,
	};

	enum CriticalSection : int32_t {
		CRITSECT_VM = 0x36,
	};

	struct ScriptParseTreeDBG {
		XHash name;
		int32_t gdbLen;
		int32_t srcLen;
		shared::gsc::acts_debug::GSC_ACTS_DEBUG* gdb;
		const char* src;
	}; static_assert(sizeof(ScriptParseTreeDBG) == 0x28);

	struct ScriptParseTree {
		XHash name;
		byte* buffer;
		uint32_t len;
	}; static_assert(sizeof(ScriptParseTree) == 0x20);

	struct LuaFile {
		XHash name;
		int len;
		const char* buffer;
	}; static_assert(sizeof(LuaFile) == 0x20);

	struct LocalizeEntry {
		const char* value;
		XHash name;
	};
	static_assert(sizeof(LocalizeEntry) == 0x18);

	struct GfxImage;
	struct Material;
	struct TTFDef;
	struct Font;

	union XAssetHeader {
		void* data;
		ScriptParseTree* spt;
		ScriptParseTreeDBG* sptdbg;
		LuaFile* luafile;
		LocalizeEntry* localize;
		GfxImage* image;
		Material* material;
		TTFDef* ttf;
		Font* font;
	}; static_assert(sizeof(XAssetHeader) == 8);

	enum BuiltinType : uint32_t {
		BUILTIN_DEFAULT = 0,
		BUILTIN_DEV = 1
	};

	struct BuiltinFunctionDef {
		uint32_t canonId;
		uint32_t min_args;
		uint32_t max_args;
		void* actionFunc;
		BuiltinType type;
	};

	struct dvar_t;

	enum dvarType_t : __int32 {
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
	union DvarLimits {
		struct {
			vec_t min;
			vec_t max;
		} vector;
		struct {
			float min;
			float max;
		} value;
		struct {
			uint64_t min;
			uint64_t max;
		} uint64;
		struct {
			int64_t min;
			int64_t max;
		} int64;
		struct {
			int32_t min;
			int32_t max;
		} int32;
		struct {
			int stringCount;
			const char** strings;
		} enumeration;
	};

	union DvarValue {
		bool enabled;
		int32_t integer;
		uint32_t unsignedInt;
		int64_t integer64;
		uint64_t unsignedInt64;
		float value;
		vec4_t vector;
		const char* string;
		byte color[4];
		const dvar_t* indirect[3];
	};

	struct DvarData {
		DvarValue current;
		DvarValue latched;
		DvarValue reset;
		DvarValue unk48;
	};
	static_assert(sizeof(DvarData) == 0x60);

	struct dvar_t {
		XHash name;
		dvar_t* hashnext;
		DvarData* value;
		dvarType_t type;
		unsigned int flags;
		DvarLimits domain;
		uint64_t unk38;
	};
	static_assert(sizeof(dvar_t) == 0x40);

	typedef void (*xcommand_t)();

	struct cmd_function_t {
		cmd_function_t* next;
		XHash name;
		const char* autoCompleteDir;
		const char* autoCompleteExt;
		xcommand_t function;
	};
	static_assert(sizeof(cmd_function_t) == 0x30);

	struct MessageString {
		uint32_t len;
		char string[1024];
	};

	struct AssetLink {
		AssetLink* next;
	};

	struct XAssetPool {
		void* pool;
		unsigned int itemSize;
		int itemCount;
		bool isSingleton;
		int itemAllocCount;
		AssetLink* freeHead;
	}; static_assert(sizeof(XAssetPool) == 0x20);

	struct CachedAssets_t {
		Material* whiteMaterial;
		Material* cursor;
		Material* blur;
		TTFDef* devFont;
		TTFDef* defaultFont;
		Font* bigFont;
		Font* smallFont;
		Font* consoleFont;
		Font* boldFont;
		Font* textFont;
		Font* extraBigFont;
		Font* extraSmallFont;
		Font* fxFont;
	};
	static_assert(sizeof(CachedAssets_t) == 0x68);

	struct sharedUiInfo_t {
		CachedAssets_t assets;
		Material* talkingIcons[3];

		// ...
	};
	static_assert(sizeof(sharedUiInfo_t) == 0x80);


	enum itemTextStyle {
		ITEM_TEXTSTYLE_NORMAL = 0,
		ITEM_TEXTSTYLE_SHADOWED = 3,
		ITEM_TEXTSTYLE_SHADOWEDMORE = 6,
		ITEM_TEXTSTYLE_BORDERED = 7,
		ITEM_TEXTSTYLE_BORDEREDMORE = 8,
		ITEM_TEXTSTYLE_MONOSPACE = 128,
		ITEM_TEXTSTYLE_MONOSPACESHADOWED = 132,
	};

	struct ScreenPlacement {
		vec2_t scaleVirtualToReal;
		vec2_t scaleVirtualToFull;
		vec2_t scaleRealToVirtual;
		vec2_t virtualViewableMin;
		vec2_t virtualViewableMax;
		vec2_t virtualTweakableMin;
		vec2_t virtualTweakableMax;
		vec2_t realViewportBase;
		vec2_t realViewportSize;
		vec2_t realViewportMid;
		vec2_t realViewableMin;
		vec2_t realViewableMax;
		vec2_t realTweakableMin;
		vec2_t realTweakableMax;
		float hudSplitscreenScale;
	};
	static_assert(sizeof(ScreenPlacement) == 0x74);

	struct va_info_t;
	struct TraceThreadInfo;
	struct ErrorThreadLocal;

	struct CmdArgs {
		int nesting;
		LocalClientNum_t localClientNum[8];
		ControllerIndex_t controllerIndex[8];
		int argshift[8];
		int argc[8];
		const char** argv[8];
		char textPool[8192];
		const char* argvPool[512];
		int usedTextPool[8];
		int totalUsedArgvPool;
		int totalUsedTextPool;
	};

	struct TLSData {
		va_info_t* vaInfo;
		jmp_buf* errorJmpBuf;
		TraceThreadInfo* traceInfo;
		CmdArgs* cmdArgs;
		ErrorThreadLocal* errorData;
	};

	enum SVSCMD_e : byte {
		SVSCMD_BASE = 0x22,
		SVSCMD_AWARDACHIEVEMENT = 0x23,
		SVSCMD_AWARDGAMERPIC = 0x24,
		SVSCMD_BIG_CONFIG_0 = 0x25,
		SVSCMD_BIG_CONFIG_1 = 0x26,
		SVSCMD_BIG_CONFIG_2 = 0x27,
		SVSCMD_BLUR = 0x28,
		SVSCMD_TELEPORT_VEHICLE = 0x2B,
		SVSCMD_CONFIG_STRING = 0x2E,
		SVSCMD_DISCONNECT = 0x2F,
		SVSCMD_EXPLODER = 0x31,
		SVSCMD_GAME_MESSAGE = 0x32,
		SVSCMD_GAME_MESSAGE_BOLD = 0x33,
		SVSCMD_UI_NOTIFY = 0x39,
		SVSCMD_PRINT = 0x3F,
		SVSCMD_SCR_CAMERA = 0x45,
		SVSCMD_SELECT_OFFHAND = 0x47,
		SVSCMD_SET_DVAR = 0x48,
		SVSCMD_SET_PROFILEVAR = 0x49,
		SVSCMD_START_FADING_BLUR = 0x4C,
		SVSCMD_UPLOAD_LEADERBOARDS = 0x52,
		SVSCMD_RESET_FOV = 0x57,
		SVSCMD_DOUBLEXP_VALUES = 0x5A,
		SVSCMD_LOOT_CONSUME = 0x5B,
		SVCCMD_CSC_EV_BFC28859 = 0x5C,
		SVCCMD_UNK5F = 0x5F,


		SVCCMD_ACTS_CUSTOM_CMD = 0x9a,
	};

	enum svscmd_type : int32_t {
		SV_CMD_CAN_IGNORE = 0x0,
		SV_CMD_RELIABLE = 0x1,
	};

	struct client_t;

}