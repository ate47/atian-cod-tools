#include "dll_includes.hpp"


using namespace custom_gsc_func;

// Prototype
static void ScrVm_Error(uint64_t code, scriptinstance::ScriptInstance inst, char* unk, bool terminal);
static void* DB_FindXAssetHeader(BYTE type, UINT64* name, bool errorIfMissing, int waitTime);
static void* StringTable_GetAsset(char const* name);
static void Scr_LogCompilerError(char const* name, ...);
static void Error(UINT32 code, const char* empty);
static void ScrVm_RuntimeError(uint32_t errorCode, scriptinstance::ScriptInstance inst, byte* codePos, const char* msg, bool terminalError);

static bool CScr_GetFunctionReverseLookup(byte* func, UINT32* hash, bool* isFunction);
static bool Scr_GetFunctionReverseLookup(byte* func, UINT32* hash, bool* isFunction);
static BuiltinFunction Scr_GetFunction(UINT32 name, BuiltinType* type, int* min_args, int* max_args);
static BuiltinFunction CScr_GetFunction(UINT32 name, BuiltinType* type, int* min_args, int* max_args);
static BuiltinFunction Scr_GetMethod(UINT32 name, BuiltinType* type, int* min_args, int* max_args);
static BuiltinFunction CScr_GetMethod(UINT32 name, BuiltinType* type, int* min_args, int* max_args);


// Detours
static cliconnect::DetourInfo<void, UINT64, scriptinstance::ScriptInstance, char*, bool> dScrVm_Error{ "ScrVm_Error", bo4::OFFSET_ScrVm_Error, ScrVm_Error };
static cliconnect::DetourInfo<void*, BYTE, UINT64*, bool, int> dDB_FindXAssetHeader{ "DB_FindXAssetHeader", bo4::OFFSET_DB_FindXAssetHeader, DB_FindXAssetHeader };
static cliconnect::DetourInfo<void*, char const*> dStringTable_GetAsset{ "StringTable_GetAsset", bo4::OFFSET_StringTable_GetAsset, StringTable_GetAsset };
static cliconnect::DetourInfo<void> dScr_LogCompilerError{ "Scr_LogCompilerError", bo4::OFFSET_LogCompilerError, reinterpret_cast<void (*)()>(Scr_LogCompilerError) };
static cliconnect::DetourInfo<void, UINT32, const char*> dError{ "Error", bo4::OFFSET_Error, Error };
static cliconnect::DetourInfo<void, uint32_t, scriptinstance::ScriptInstance, byte*, const char*, bool> dScrVm_RuntimeError{ "ScrVm_RuntimeError", bo4::OFFSET_ScrVm_RuntimeError, ScrVm_RuntimeError };

static cliconnect::DetourInfo<bool, byte*, UINT32*, bool*> dCScr_GetFunctionReverseLookup{ "CScr_GetFunctionReverseLookup", bo4::OFFSET_CScr_GetFunctionReverseLookup, CScr_GetFunctionReverseLookup };
static cliconnect::DetourInfo<bool, byte*, UINT32*, bool*> dScr_GetFunctionReverseLookup{ "Scr_GetFunctionReverseLookup", bo4::OFFSET_Scr_GetFunctionReverseLookup, Scr_GetFunctionReverseLookup };
static cliconnect::DetourInfo<BuiltinFunction, UINT32, BuiltinType*, int*, int*> dScr_GetFunction{ "Scr_GetFunction", bo4::OFFSET_Scr_GetFunction, Scr_GetFunction };
static cliconnect::DetourInfo<BuiltinFunction, UINT32, BuiltinType*, int*, int*> dCScr_GetFunction{ "CScr_GetFunction", bo4::OFFSET_CScr_GetFunction, CScr_GetFunction };
static cliconnect::DetourInfo<BuiltinFunction, UINT32, BuiltinType*, int*, int*> dScr_GetMethod{ "Scr_GetMethod", bo4::OFFSET_Scr_GetMethod, Scr_GetMethod };
static cliconnect::DetourInfo<BuiltinFunction, UINT32, BuiltinType*, int*, int*> dCScr_GetMethod{ "CScr_GetMethod", bo4::OFFSET_CScr_GetMethod, CScr_GetMethod };

// Custom detours
static void ScrVm_Error(UINT64 code, scriptinstance::ScriptInstance inst, char* unk, bool terminal) {
	if (code == custom_gsc_func::custom_error_id) { // detect custom error
		LOG_ERROR("VM {} ACTS Error '{}' terminal={}", scriptinstance::Name(inst), unk, terminal ? "true" : "false");
	}
	else {
		auto desc = error_handler::FindDesc((UINT32)code);
		LOG_ERROR("VM {} Error code={} '{}' terminal={}", scriptinstance::Name(inst), code, unk, terminal ? "true" : "false");
		if (desc) {
			// update the error info to match the error
			bo4::scrVarPub[inst].error_message = desc;
		}
	}
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
static void ScrVm_RuntimeError(uint32_t errorCode, scriptinstance::ScriptInstance inst, byte* codePos, const char* msg, bool terminalError) {
	static CHAR error_buffer[2][0x2000];
	bo4::GSCExport *exp;
	bo4::GSCOBJ *obj;
	scriptinstance::ScriptInstance inst2;
	UINT32 rloc;

	auto& buff = error_buffer[inst];
	auto& vm = bo4::scrVmPub[inst];

	size_t w = snprintf(buff, sizeof(buff), "%s", msg);

	for (int i = vm.function_count; i > 0; i--) {
		auto& stack = vm.function_frame_start[i];
		if (w + 1 >= sizeof(buff)) {
			continue; // too many chars
		}

		// add script location at the end of the message
		if (bo4::FindGSCFuncLocation(reinterpret_cast<BYTE*>(stack.bytecodeLocation), inst2, obj, exp, rloc)) {
			w += snprintf(&buff[w], sizeof(buff) - w, "\n%s", hash_lookup::ExtractTmp(inst, obj->name));
			w += snprintf(&buff[w], sizeof(buff) - w, "@%s:%x", hash_lookup::ExtractTmp(inst, exp->name), rloc);
		}
		else {
			snprintf(buff, sizeof(buff), "%s (Can't find script at %llx)", msg, reinterpret_cast<UINT64>(codePos));
		}
	}

	dScrVm_RuntimeError(errorCode, inst, codePos, buff, terminalError);
}

static void Error(UINT32 code, const char* empty) {
	// hard error
	LOG_ERROR("Scr_Error {}{}", code, empty);
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

static bool CScr_GetFunctionReverseLookup(BYTE* func, UINT32* hash, bool* isFunction) {
	auto res = dCScr_GetFunctionReverseLookup(func, hash, isFunction);
	if (res) {
		return res;
	}

	for (auto& blt : custom_functions[scriptinstance::SI_CLIENT]) {
		if (reinterpret_cast<BYTE*>(blt.actionFunc) == func) {
			*hash = blt.name;
			*isFunction = true;
			return true;
		}
	}

	return false;
}

static bool Scr_GetFunctionReverseLookup(BYTE* func, UINT32* hash, bool* isFunction) {
	auto res = dScr_GetFunctionReverseLookup(func, hash, isFunction);
	if (res) {
		return res;
	}

	for (auto& blt : custom_functions[scriptinstance::SI_SERVER]) {
		if (reinterpret_cast<BYTE*>(blt.actionFunc) == func) {
			*hash = blt.name;
			*isFunction = true;
			return true;
		}
	}

	return false;
}

static BuiltinFunction Scr_GetFunction(UINT32 name, BuiltinType* type, int* min_args, int* max_args) {
	auto res = dScr_GetFunction(name, type, min_args, max_args);
	// allow dev functions
	*type = BUILTIN_DEFAULT;
	if (res) {
		return res;
	}

	for (auto& blt : custom_functions[scriptinstance::SI_SERVER]) {
		if (blt.name == name) {
			//*type = blt.type;
			*min_args = blt.min_args;
			*max_args = blt.max_args;
			return blt.actionFunc;
		}
	}

	return NULL;
}

static BuiltinFunction CScr_GetFunction(UINT32 name, BuiltinType* type, int* min_args, int* max_args) {
	auto res = dCScr_GetFunction(name, type, min_args, max_args);
	// allow dev functions
	*type = BUILTIN_DEFAULT;
	if (res) {
		return res;
	}

	for (auto& blt : custom_functions[scriptinstance::SI_CLIENT]) {
		if (blt.name == name) {
			//*type = blt.type;
			*min_args = blt.min_args;
			*max_args = blt.max_args;
			return blt.actionFunc;
		}
	}

	return NULL;
}

static BuiltinFunction Scr_GetMethod(UINT32 name, BuiltinType* type, int* min_args, int* max_args) {
	auto res = dScr_GetMethod(name, type, min_args, max_args);
	// allow dev methods
	*type = BUILTIN_DEFAULT;
	if (res) {
		return res;
	}

	return NULL;
}

static BuiltinFunction CScr_GetMethod(UINT32 name, BuiltinType* type, int* min_args, int* max_args) {
	auto res = dCScr_GetMethod(name, type, min_args, max_args);
	// allow dev methods
	*type = BUILTIN_DEFAULT;
	if (res) {
		return res;
	}

	return NULL;
}
