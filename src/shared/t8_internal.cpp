#include <includes_shared.hpp>

LPCCH t8internal::ScrVarTypeName(ScrVarType type) {
	switch (type) {
	case TYPE_UNDEFINED: return "UNDEFINED";
	case TYPE_POINTER: return "POINTER";
	case TYPE_STRING: return "STRING";
	case TYPE_VECTOR: return "VECTOR";
	case TYPE_HASH: return "HASH";
	case TYPE_FLOAT: return "FLOAT";
	case TYPE_INTEGER: return "INTEGER";
	case TYPE_UINTPTR: return "UINTPTR";
	case TYPE_ENTITY_OFFSET: return "ENTITY_OFFSET";
	case TYPE_CODEPOS: return "CODEPOS";
	case TYPE_PRECODEPOS: return "PRECODEPOS";
	case TYPE_API_FUNCTION: return "API_FUNCTION";
	case TYPE_SCRIPT_FUNCTION: return "SCRIPT_FUNCTION";
	case TYPE_STACK: return "STACK";
	case TYPE_THREAD: return "THREAD";
	case TYPE_NOTIFY_THREAD: return "NOTIFY_THREAD";
	case TYPE_TIME_THREAD: return "TIME_THREAD";
	case TYPE_FRAME_THREAD: return "FRAME_THREAD";
	case TYPE_CHILD_THREAD: return "CHILD_THREAD";
	case TYPE_CLASS: return "CLASS";
	case TYPE_SHARED_STRUCT: return "SHARED_STRUCT";
	case TYPE_STRUCT: return "STRUCT";
	case TYPE_REMOVED_ENTITY: return "REMOVED_ENTITY";
	case TYPE_ENTITY: return "ENTITY";
	case TYPE_ARRAY: return "ARRAY";
	case TYPE_REMOVED_THREAD: return "REMOVED_THREAD";
	case TYPE_FREE: return "FREE";
	case TYPE_THREAD_LIST: return "THREAD_LIST";
	default: return "<unknown type>";
	}
}
