#pragma once
namespace t8internal {
	enum ScrVarType : UINT32 {
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
	LPCCH ScrVarTypeName(ScrVarType type);



	struct WeaponInfo {
		UINT64 weaponIdx : 9;
		UINT64 attachment1 : 6;
		UINT64 attachment2 : 6;
		UINT64 attachment3 : 6;
		UINT64 attachment4 : 6;
		UINT64 attachment5 : 6;
		UINT64 attachment6 : 6;
		UINT64 attachment7 : 6;
		UINT64 attachment8 : 6;
		UINT64 padding : 7;
	};


	struct ScrVarRef {
		UINT64 nameIndex;
		UINT32 _anon_0;
		UINT32 nextSibling;
		UINT32 prevSibling;
		UINT32 parentId;
		UINT32 nameSearchHashList;
		UINT32 pad0;
	};

	struct ScrVarEntityInfo {
		UINT16 classnum;
		UINT16 clientNum;
	};


	union UnionAll32 {
		UINT32 ui;
		INT64 ll;
		INT32 i;
		FLOAT f;
		ScrVarEntityInfo entVar;
	};

	union UnionAll {
		UINT64 ull;
		UINT32 ui;
		INT64 ll;
		INT32 i;
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
		UINT32 pad0;
	};

	struct __declspec(align(8)) FunctionStack {
		// BYTE*
		uintptr_t bytecodeLocation;
		// ScrVar*
		uintptr_t top;
		// ScrVar*
		uintptr_t startTop;
		UINT32 threadId;
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
		uintptr_t localVars; // UINT32*
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
		UINT64 ull;
		UINT32 ui;
	};

	enum LocalClientNum : UINT32 {
		INVALID_LOCAL_CLIENT = 0xFFFFFFFF,
		LOCAL_CLIENT_0 = 0x0,
		LOCAL_CLIENT_FIRST = 0x0,
		LOCAL_CLIENT_KEYBOARD_AND_MOUSE = 0x0,
		LOCAL_CLIENT_1 = 0x1,
		LOCAL_CLIENT_2 = 0x2,
		LOCAL_CLIENT_3 = 0x3,
		LOCAL_CLIENT_COUNT = 0x4,
	};


	enum ClassNum : UINT16 {
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

	enum FieldType : UINT32
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

	enum WeaponFieldType : UINT32
	{
		F_WEAPON_INVALID = 0x0,
		F_WEAPON_DEF = 0x1,
	};


	struct ScrWeaponField
	{
		UINT32 canonId;
		FieldType type;
		bool isreadonly;
		int ofs;
		int size;
		WeaponFieldType weaponType;
		uintptr_t getter; // void (*ScriptCallbackWeapon)(scriptInstance_t, Weapon, const scr_weapon_field_t *)
	};


	struct __declspec(align(8)) ScrVarStackBuffer {
		byte* pos;
		UINT16 size;
		UINT16 bufLen;
		UINT32 threadId; // ScrVarIndex
		byte buf[1]; // depends on the allocated size
	};

	struct __declspec(align(4)) ScrVarGlobalVars {
		UINT32 name;
		UINT32 id; // ScrVarIndex
		bool persist;
	};


	struct __declspec(align(8)) scrVarPub {
		uintptr_t fieldBuffer; // const char*
		const char *error_message; // const char*
		uintptr_t programBuffer; // byte*
		uintptr_t endScriptBuffer; // byte*
		uintptr_t programHunkUser; // HunkUser*
		ScrVarGlobalVars globalVars[16];
		UINT64 entFieldNameIndex; // ScrVarNameIndex_t
		UINT32 freeEntList; // ScrVarIndex_t
		UINT32 tempVariable; // ScrVarIndex_t
		UINT32 checksum;
		UINT32 entId;
		UINT32 varHighWatermark;
		UINT32 numScriptThreads;
		UINT32 numVarAllocations;
		INT32 varHighWatermarkId;
	};
}