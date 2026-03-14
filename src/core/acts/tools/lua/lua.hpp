#pragma once

namespace tool::lua {
	enum LuaType {
		LT_NIL = 0,
		LT_BOOLEAN,
		LT_LIGHTUSERDATA,
		LT_NUMBER,
		LT_STRING,
		LT_TABLE,
		LT_FUNCTION,
		LT_USERDATA,
		LT_THREAD,
		LT_IFUNCTION,
		LT_CFUNCTION,
		LT_UI64,
		LT_STRUCT,
		LT_XHASH,

		LT_BADTYPE = 0xFFFF
	};
	LuaType ReadLuaType(const char* type);
}