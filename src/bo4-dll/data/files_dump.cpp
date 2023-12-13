#include <dll_includes.hpp>


#ifdef DEBUG

void* __fastcall Lua_CoD_GetRawFile(const char* filename);
void* __fastcall R_RegisterFont(const char* filename);

static inject::DetourInfo<void*, const char*> dLua_CoD_GetRawFile{ "Lua_CoD_GetRawFile", 0x390FBB0, Lua_CoD_GetRawFile };
static inject::DetourInfo<void*, const char*> dR_RegisterFont{ "R_RegisterFont", 0x35B22A0, R_RegisterFont };

void* __fastcall Lua_CoD_GetRawFile(const char* filename) {
	if (strncmp(filename, "x64:", 4)) {
		LOG_DEBUG("LUA_FILE: %s", filename);
	}

	return dLua_CoD_GetRawFile(filename);
}
void* __fastcall R_RegisterFont(const char* filename) {
	LOG_DEBUG("TTF: %s", filename);

	return dR_RegisterFont(filename);
}

#endif