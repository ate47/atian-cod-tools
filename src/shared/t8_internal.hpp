#pragma once
namespace t8internal {
	enum ScrVarType : uint32_t {
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
		TYPE_ENT_LIST = 0x1c
	};
	const char* ScrVarTypeName(ScrVarType type);



	struct WeaponInfo {
		uint64_t weaponIdx : 9;
		uint64_t attachment1 : 6;
		uint64_t attachment2 : 6;
		uint64_t attachment3 : 6;
		uint64_t attachment4 : 6;
		uint64_t attachment5 : 6;
		uint64_t attachment6 : 6;
		uint64_t attachment7 : 6;
		uint64_t attachment8 : 6;
		uint64_t padding : 7;
	};


	struct ScrVarRef {
		uint64_t nameIndex;
		uint32_t _anon_0;
		uint32_t nextSibling;
		uint32_t prevSibling;
		uint32_t parentId;
		uint32_t nameSearchHashList;
		uint32_t pad0;
	};

	struct ScrVarEntityInfo {
		uint16_t classnum;
		uint16_t clientNum;
	};


	union UnionAll32 {
		uint32_t ui;
		int64_t ll;
		int32_t i;
		FLOAT f;
		ScrVarEntityInfo entVar;
	};

	union UnionAll {
		uint64_t ull;
		uint32_t ui;
		int64_t ll;
		int32_t i;
		DOUBLE d;
		FLOAT f;
		uintptr_t ptr;
		ScrVarEntityInfo entVar;
		WeaponInfo weaponInfo;
		UnionAll32 b32;
	};

	struct ScrVar {
		UnionAll value;
		ScrVarType type;
		uint32_t pad0;
	};

	struct __declspec(align(8)) FunctionStack {
		// byte*
		uintptr_t bytecodeLocation;
		// ScrVar*
		uintptr_t top;
		// ScrVar*
		uintptr_t startTop;
		uint32_t threadId;
		uint16_t localVarCount;
		uint16_t profileInfoCount;
	};

	struct __declspec(align(8)) scrVmPub {
		uintptr_t unknown1;
		uintptr_t unknown2;
		uintptr_t executionQueueHeap;
		uintptr_t timeByValueQueue; // VmExecutionQueueData*
		uintptr_t timeByThreadQueue[1024]; // VmExecutionQueue*
		uintptr_t frameByValueQueue; // VmExecutionQueueData*
		uintptr_t frameByThreadQueue[1024]; // VmExecutionQueue*
		uintptr_t timeoutByValueList; // VmExecutionQueueData*
		uintptr_t timeoutByThreadList[1024]; // VmExecutionQueue*
		uintptr_t notifyByObjectQueue[1024]; // VmExecutionNotifyQueue*
		uintptr_t notifyByThreadQueue[1024]; // VmExecutionNotifyQueue*
		uintptr_t endonByObjectList[1024]; // VmExecutionNotifyQueue*
		uintptr_t endonByThreadList[1024]; // VmExecutionNotifyQueue*
		uintptr_t localVars; // uint32_t*
		uintptr_t maxstack; // ScrVar*
		uintptr_t function_frame; // function_frame*
		uintptr_t top; // ScrVar*
		FunctionStack function_frame_start[64];
		ScrVar stack[2048];
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

	union ScrVarObjectInfo1 {
		uint64_t ull;
		uint32_t ui;
	};

	enum LocalClientNum : uint32_t {
		INVALID_LOCAL_CLIENT = 0xFFFFFFFF,
		LOCAL_CLIENT_0 = 0x0,
		LOCAL_CLIENT_FIRST = 0x0,
		LOCAL_CLIENT_KEYBOARD_AND_MOUSE = 0x0,
		LOCAL_CLIENT_1 = 0x1,
		LOCAL_CLIENT_2 = 0x2,
		LOCAL_CLIENT_3 = 0x3,
		LOCAL_CLIENT_COUNT = 0x4,
	};


	enum ClassNum : uint16_t {
		CLASSNUM_ENTITY = 0,
		CLASSNUM_HUD_ELEM = 1,
		CLASSNUM_PATHNODE = 2,
		CLASSNUM_VEHICLENODE = 3,
		CLASSNUM_TACPOINT = 4,
		CLASSNUM_DYNENT = 5,
		CLASSNUM_WEAPON = 6,
		CLASSNUM_COUNT = 0x7,
	};

	struct ScrEntityRef {
		UnionAll u;
		ClassNum classnum;
		LocalClientNum client;
	};

	enum FieldType : uint32_t
	{
		F_INT = 0x0,
		F_SHORT = 0x1,
		F_BYTE = 0x2,
		F_BOOL = 0x2,
		F_FLOAT = 0x3,
		F_LSTRING = 0x4,
		F_STRING = 0x5,
		F_VECTOR = 0x6,
		F_ENTITY = 0x7,
		F_ENTHANDLE = 0x8,
		F_ACTOR = 0x9,
		F_SENTIENT = 0xA,
		F_SENTIENTHANDLE = 0xB,
		F_CLIENT = 0xC,
		F_PATHNODE = 0xD,
		F_ACTORGROUP = 0xE,
		F_OBJECT = 0xF,
		F_XMODEL_INDEX = 0x10,
		F_XMODEL = 0x11,
		F_BITFLAG = 0x12,
		F_BITFLAG64 = 0x13,
		F_FX = 0x14,
		F_WEAPON = 0x15,
		F_RUMBLE = 0x16,
		F_SCRIPTBUNDLE = 0x17,
		F_COUNT = 0x18,
	};

	enum WeaponFieldType : uint32_t
	{
		F_WEAPON_INVALID = 0x0,
		F_WEAPON_DEF = 0x1,
	};


	struct ScrWeaponField
	{
		uint32_t canonId;
		FieldType type;
		bool isreadonly;
		int ofs;
		int size;
		WeaponFieldType weaponType;
		uintptr_t getter; // void (*ScriptCallbackWeapon)(scriptInstance_t, Weapon, const scr_weapon_field_t *)
	};


	struct __declspec(align(8)) ScrVarStackBuffer {
		byte* pos;
		uint16_t size;
		uint16_t bufLen;
		uint32_t threadId; // ScrVarIndex
		byte buf[1]; // depends on the allocated size
	};

	struct __declspec(align(4)) ScrVarGlobalVars {
		uint32_t name;
		uint32_t id; // ScrVarIndex
		bool persist;
	};


	struct __declspec(align(8)) scrVarPub {
		uintptr_t fieldBuffer; // const char*
		const char *error_message; // const char*
		uintptr_t programBuffer; // byte*
		uintptr_t endScriptBuffer; // byte*
		uintptr_t programHunkUser; // HunkUser*
		ScrVarGlobalVars globalVars[16];
		uint64_t entFieldNameIndex; // ScrVarNameIndex_t
		uint32_t freeEntList; // ScrVarIndex_t
		uint32_t tempVariable; // ScrVarIndex_t
		uint32_t checksum;
		uint32_t entId;
		uint32_t varHighWatermark;
		uint32_t numScriptThreads;
		uint32_t numVarAllocations;
		int32_t varHighWatermarkId;
	};
}