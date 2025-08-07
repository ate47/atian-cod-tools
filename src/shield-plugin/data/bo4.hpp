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


	union XAssetHeader {
		void* data;
		ScriptParseTree* spt;
		ScriptParseTreeDBG* sptdbg;
		LuaFile* luafile;
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
}