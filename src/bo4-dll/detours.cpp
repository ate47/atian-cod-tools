#include "dll_includes.hpp"

// Prototype
static void ScrVm_Error(uint64_t code, scriptinstance::ScriptInstance inst, char* unk, bool terminal);
static void* DB_FindXAssetHeader(BYTE type, UINT64* name, bool errorIfMissing, int waitTime);
static void* StringTable_GetAsset(char const* name);
static void Scr_LogCompilerError(char const* name, ...);
static void Error(UINT32 code, const char* empty);

// Detours
static cliconnect::DetourInfo<void, uint64_t, scriptinstance::ScriptInstance, char*, bool> dScrVm_Error{ "ScrVm_Error", bo4::OFFSET_ScrVm_Error, ScrVm_Error };
static cliconnect::DetourInfo<void*, BYTE, UINT64*, bool, int> dDB_FindXAssetHeader{ "DB_FindXAssetHeader", bo4::OFFSET_DB_FindXAssetHeader, DB_FindXAssetHeader };
static cliconnect::DetourInfo<void*, char const*> dStringTable_GetAsset{ "StringTable_GetAsset", bo4::OFFSET_StringTable_GetAsset, StringTable_GetAsset };
static cliconnect::DetourInfo<void> dScr_LogCompilerError{ "Scr_LogCompilerError", bo4::OFFSET_LogCompilerError, reinterpret_cast<void (*)()>(Scr_LogCompilerError) };
static cliconnect::DetourInfo<void, UINT32, const char*> dError{ "Error", bo4::OFFSET_Error, Error };

// Custom detours
static void ScrVm_Error(uint64_t code, scriptinstance::ScriptInstance inst, char* unk, bool terminal) {
	LOG_ERROR("VM {} Error code={} '{}' terminal={}", scriptinstance::Name(inst), code, unk, terminal ? "true" : "false");
	dScrVm_Error(code, inst, unk, terminal);
}

static void Scr_LogCompilerError(char const* name, ...) {
	va_list va;
	va_start(va, name);
	CHAR buffer[2000];

	auto e = vsnprintf(buffer, sizeof(buffer), name, va);

	if (e > 0 && buffer[e - 1] == '\n') {
		buffer[e - 1] = 0; // remove end new line
	}

	LOG_ERROR("LogCompilerError {}", buffer);
}

static void Error(UINT32 code, const char* empty) {
	// hard error
	LOG_ERROR("Scr_Error {:x}{}", code, empty);
	dError(code, empty);
}

static void* StringTable_GetAsset(char const* name) {
	static std::map<std::string, void*> loadedMap{};
	void* sTable = dStringTable_GetAsset(name);

	auto& m = loadedMap[name];

	if (!m || m != sTable) {
		m = sTable;
		LOG_INFO("loading StringTable {} -> {}", name, m);
	}

	return sTable;
}

static void* DB_FindXAssetHeader(BYTE type, UINT64* name, bool errorIfMissing, int waitTime) {
	// for later
	return dDB_FindXAssetHeader(type, name, errorIfMissing, waitTime);
}
