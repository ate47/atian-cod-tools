#pragma once
#include <includes.hpp>

#define CW_INCLUDES

namespace cw {
	enum T9ScrVarType : unsigned __int32 {
		TYPE_UNDEFINED = 0x0,
		TYPE_POINTER = 0x1,
		TYPE_STRING = 0x2,
		TYPE_VECTOR = 0x3,
		TYPE_HASH = 0x4,
		TYPE_FLOAT = 0x5,
		TYPE_INTEGER = 0x6,
		TYPE_FINALIZATION = 0x7,
		TYPE_UINTPTR = 0x8,
		TYPE_ENTITY_OFFSET = 0x9,
		TYPE_CODEPOS = 0xA,
		TYPE_PRECODEPOS = 0xB,
		TYPE_API_FUNCTION = 0xC,
		TYPE_SCRIPT_FUNCTION = 0xD,
		TYPE_STACK = 0xE,
		TYPE_THREAD = 0xF,
		TYPE_NOTIFY_THREAD = 0x10,
		TYPE_TIME_THREAD = 0x11,
		TYPE_FRAME_THREAD = 0x12,
		TYPE_CHILD_THREAD = 0x13,
		TYPE_REMOVED_THREAD = 0x14,
		TYPE_ARRAY = 0x15,
		TYPE_CLASS = 0x16,
		TYPE_SHARED_STRUCT = 0x17,
		TYPE_STRUCT = 0x18,
		TYPE_REMOVED_ENTITY = 0x19,
		TYPE_ENTITY = 0x1A,
		TYPE_FREE = 0x1B,
		TYPE_THREAD_LIST = 0x1C,
		TYPE_ENT_LIST = 0x1D,
	};

	char* DecryptString(char* str);
	byte* DecryptRawBuffer(byte* buffer);
	uintptr_t ScanPool(Process& proc);
}

