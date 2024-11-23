#include <dll_includes.hpp>

// Prototype
static void ScrVm_Error(uint64_t code, scriptinstance::ScriptInstance inst, char* unk, bool terminal);
static void* DB_FindXAssetHeader(byte type, uint64_t* name, bool errorIfMissing, int waitTime);
static void* StringTable_GetAsset(char const* name);
static void Scr_LogCompilerError(char const* name, ...);
static void Error(uint32_t code, const char* empty);
static void ScrVm_RuntimeError(uint32_t errorCode, scriptinstance::ScriptInstance inst, byte* codePos, const char* msg, bool terminalError);

static bool CScr_GetFunctionReverseLookup(byte* func, uint32_t* hash, bool* isFunction);
static bool Scr_GetFunctionReverseLookup(byte* func, uint32_t* hash, bool* isFunction);
static bo4::BuiltinFunction Scr_GetFunction(uint32_t name, bo4::BuiltinType* type, int* min_args, int* max_args);
static bo4::BuiltinFunction CScr_GetFunction(uint32_t name, bo4::BuiltinType* type, int* min_args, int* max_args);
static bo4::BuiltinFunction Scr_GetMethod(uint32_t name, bo4::BuiltinType* type, int* min_args, int* max_args);
static bo4::BuiltinFunction CScr_GetMethod(uint32_t name, bo4::BuiltinType* type, int* min_args, int* max_args);

// Detours
static inject::DetourInfo<void, uint64_t, scriptinstance::ScriptInstance, char*, bool> dScrVm_Error{ "ScrVm_Error", bo4::OFFSET_ScrVm_Error, ScrVm_Error };
static inject::DetourInfo<void*, byte, uint64_t*, bool, int> dDB_FindXAssetHeader{ "DB_FindXAssetHeader", bo4::OFFSET_DB_FindXAssetHeader, DB_FindXAssetHeader };
static inject::DetourInfo<void*, char const*> dStringTable_GetAsset{ "StringTable_GetAsset", bo4::OFFSET_StringTable_GetAsset, StringTable_GetAsset };
static inject::DetourInfo<void> dScr_LogCompilerError{ "Scr_LogCompilerError", bo4::OFFSET_LogCompilerError, reinterpret_cast<void (*)()>(Scr_LogCompilerError) };
static inject::DetourInfo<void, uint32_t, const char*> dError{ "Error", bo4::OFFSET_Error, Error };
static inject::DetourInfo<void, uint32_t, scriptinstance::ScriptInstance, byte*, const char*, bool> dScrVm_RuntimeError{ "ScrVm_RuntimeError", bo4::OFFSET_ScrVm_RuntimeError, ScrVm_RuntimeError };

static inject::DetourInfo<bool, byte*, uint32_t*, bool*> dCScr_GetFunctionReverseLookup{ "CScr_GetFunctionReverseLookup", bo4::OFFSET_CScr_GetFunctionReverseLookup, CScr_GetFunctionReverseLookup };
static inject::DetourInfo<bool, byte*, uint32_t*, bool*> dScr_GetFunctionReverseLookup{ "Scr_GetFunctionReverseLookup", bo4::OFFSET_Scr_GetFunctionReverseLookup, Scr_GetFunctionReverseLookup };
static inject::DetourInfo<bo4::BuiltinFunction, uint32_t, bo4::BuiltinType*, int*, int*> dScr_GetFunction{ "Scr_GetFunction", bo4::OFFSET_Scr_GetFunction, Scr_GetFunction };
static inject::DetourInfo<bo4::BuiltinFunction, uint32_t, bo4::BuiltinType*, int*, int*> dCScr_GetFunction{ "CScr_GetFunction", bo4::OFFSET_CScr_GetFunction, CScr_GetFunction };
static inject::DetourInfo<bo4::BuiltinFunction, uint32_t, bo4::BuiltinType*, int*, int*> dScr_GetMethod{ "Scr_GetMethod", bo4::OFFSET_Scr_GetMethod, Scr_GetMethod };
static inject::DetourInfo<bo4::BuiltinFunction, uint32_t, bo4::BuiltinType*, int*, int*> dCScr_GetMethod{ "CScr_GetMethod", bo4::OFFSET_CScr_GetMethod, CScr_GetMethod };

// Custom detours
static void ScrVm_Error(uint64_t code, scriptinstance::ScriptInstance inst, char* unk, bool terminal) {
	static char errorBuffer[2][0x200] = { 0 };
	if (code == custom_gsc_func::custom_error_id) { // ACTS detect custom error, the error is already set
		LOG_ERROR("VM {} ACTS Error '{}' terminal={}", scriptinstance::Name(inst), unk, terminal ? "true" : "false");
	}
	else if (code == 2737681163) { // Assert(val, msg) with message error
		const auto* msg = bo4::ScrVm_GetString(inst, 1);
		sprintf_s(errorBuffer[inst], "assert fail: %s", msg);
		bo4::scrVarPub[inst].error_message = errorBuffer[inst];
	}
	else if (code == 1385570291) { // AssertMsg(msg)
		const auto* msg = bo4::ScrVm_GetString(inst, 0);
		sprintf_s(errorBuffer[inst], "assert fail: %s", msg);
		bo4::scrVarPub[inst].error_message = errorBuffer[inst];
	}
	else if (code == 2532286589) { // ErrorMsg(msg)
		const auto* msg = bo4::ScrVm_GetString(inst, 0);
		sprintf_s(errorBuffer[inst], "error: %s", msg);
		bo4::scrVarPub[inst].error_message = errorBuffer[inst];
	}
	else {
		auto desc = error_handler::FindDesc((uint32_t)code);
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
	char buffer[2000];

	auto e = vsprintf_s(buffer, name, va);
	va_end(va);

	if (e > 0 && buffer[e - 1] == '\n') {
		buffer[e - 1] = 0; // remove end new line
	}

	LOG_ERROR("LogCompilerError {}", buffer);
}
static void ScrVm_RuntimeError(uint32_t errorCode, scriptinstance::ScriptInstance inst, byte* codePos, const char* msg, bool terminalError) {
	static char error_buffer[2][0x2000];
	bo4::GSCExport *exp;
	bo4::GSCOBJ *obj;
	scriptinstance::ScriptInstance inst2;
	uint32_t rloc;

	auto& buff = error_buffer[inst];
	auto& vm = bo4::scrVmPub[inst];

	size_t w = sprintf_s(buff, "%s", msg);

	for (int i = vm.function_count; i > 0; i--) {
		auto& stack = vm.function_frame_start[i];
		if (w + 1 >= sizeof(buff)) {
			continue; // too many chars
		}

		// add script location at the end of the message
		if (bo4::FindGSCFuncLocation(reinterpret_cast<byte*>(stack.bytecodeLocation), inst2, obj, exp, rloc)) {
			w += snprintf(&buff[w], sizeof(buff) - w, "\n%s", hash_lookup::ExtractTmp(inst, obj->name));
			w += snprintf(&buff[w], sizeof(buff) - w, "@%s:%x", hash_lookup::ExtractTmp(inst, exp->name), rloc);
		}
		else {
			w += snprintf(&buff[w], sizeof(buff) - w, "%s (Can't find script at %llx)", msg, reinterpret_cast<uint64_t>(codePos));
		}
	}

	dScrVm_RuntimeError(errorCode, inst, codePos, buff, terminalError);
}

static void Error(uint32_t code, const char* empty) {
	// hard error
	if (code == find_linking::ERROR_LINKING) {
		int err = 0;
		for (size_t inst = 0; inst < scriptinstance::SI_COUNT; inst++) {
			err += find_linking::CheckLinkingError((scriptinstance::ScriptInstance)inst);
		}
		static char linkingErrorBuff[0x100];
		if (err) {
			sprintf_s(linkingErrorBuff, "Find %d errors, check ACTS logs", err);
			empty = linkingErrorBuff;
		}
	}
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

static void* DB_FindXAssetHeader(byte type, uint64_t* name, bool errorIfMissing, int waitTime) {
	if (type == pool::ASSET_TYPE_STRINGTABLE) {
		auto* ptr = stringtables::GetTable(*name);

		if (ptr) {
			return ptr;
		}
	}
	return dDB_FindXAssetHeader(type, name, errorIfMissing, waitTime);
}

static bool CScr_GetFunctionReverseLookup(byte* func, uint32_t* hash, bool* isFunction) {
	auto res = dCScr_GetFunctionReverseLookup(func, hash, isFunction);
	if (res) {
		return res;
	}

	for (auto& blt : custom_gsc_func::custom_functions[scriptinstance::SI_CLIENT]) {
		if (reinterpret_cast<byte*>(blt.actionFunc) == func) {
			*hash = blt.name;
			*isFunction = true;
			return true;
		}
	}

	return false;
}

static bool Scr_GetFunctionReverseLookup(byte* func, uint32_t* hash, bool* isFunction) {
	auto res = dScr_GetFunctionReverseLookup(func, hash, isFunction);
	if (res) {
		return res;
	}

	for (auto& blt : custom_gsc_func::custom_functions[scriptinstance::SI_SERVER]) {
		if (reinterpret_cast<byte*>(blt.actionFunc) == func) {
			*hash = blt.name;
			*isFunction = true;
			return true;
		}
	}

	return false;
}

static bo4::BuiltinFunction Scr_GetFunction(uint32_t name, bo4::BuiltinType* type, int* min_args, int* max_args) {
	auto res = dScr_GetFunction(name, type, min_args, max_args);
	// allow dev functions
	*type = bo4::BUILTIN_DEFAULT;
	if (res) {
		return res;
	}

	for (auto& blt : custom_gsc_func::custom_functions[scriptinstance::SI_SERVER]) {
		if (blt.name == name) {
			//*type = blt.type;
			*min_args = blt.min_args;
			*max_args = blt.max_args;
			return blt.actionFunc;
		}
	}

	return NULL;
}

static bo4::BuiltinFunction CScr_GetFunction(uint32_t name, bo4::BuiltinType* type, int* min_args, int* max_args) {
	auto res = dCScr_GetFunction(name, type, min_args, max_args);
	// allow dev functions
	*type = bo4::BUILTIN_DEFAULT;
	if (res) {
		return res;
	}

	for (auto& blt : custom_gsc_func::custom_functions[scriptinstance::SI_CLIENT]) {
		if (blt.name == name) {
			//*type = blt.type;
			*min_args = blt.min_args;
			*max_args = blt.max_args;
			return blt.actionFunc;
		}
	}

	return NULL;
}

static bo4::BuiltinFunction Scr_GetMethod(uint32_t name, bo4::BuiltinType* type, int* min_args, int* max_args) {
	auto res = dScr_GetMethod(name, type, min_args, max_args);
	// allow dev methods
	*type = bo4::BUILTIN_DEFAULT;
	if (res) {
		return res;
	}

	return NULL;
}

static bo4::BuiltinFunction CScr_GetMethod(uint32_t name, bo4::BuiltinType* type, int* min_args, int* max_args) {
	auto res = dCScr_GetMethod(name, type, min_args, max_args);
	// allow dev methods
	*type = bo4::BUILTIN_DEFAULT;
	if (res) {
		return res;
	}

	return NULL;
}
