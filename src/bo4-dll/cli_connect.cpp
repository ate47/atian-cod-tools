#include <dll_includes.hpp>

clisync::CliSyncData g_cliData{};

// void* StringTable_GetAsset(char const*).text 0x28A2660.
// void* DB_FindXAssetHeader(XAssetType, char const*, bool, int).text 0x2EB75B0

typedef void* (__fastcall* tStringTable_GetAsset)(char const*);
typedef void* (__fastcall* tDB_FindXAssetHeader)(BYTE type, UINT64* name, bool errorIfMissing, int waitTime);
tStringTable_GetAsset trueStringTable_GetAsset = NULL;
tDB_FindXAssetHeader trueDB_FindXAssetHeader = NULL;


void* StringTable_GetAsset(char const* name) {
	static std::map<std::string, void*> loadedMap{};
	void* sTable = trueStringTable_GetAsset(name);

	auto& m = loadedMap[name];

	if (!m || m != sTable) {
		m = sTable;
		LOG_INFO("loading StringTable {} -> {}", name, m);
	}

	return sTable;
}

void* DB_FindXAssetHeader(BYTE type, UINT64* name, bool errorIfMissing, int waitTime) {
	// for later
	return trueDB_FindXAssetHeader(type, name, errorIfMissing, waitTime);
}

void SyncCLIOnce(clisync::CliSyncData* data) {
	static std::string logpath{};

	// sync data
	g_cliData = *data;

	logpath = (std::filesystem::path{ g_cliData.workDir } / "acts-bo4.log").string();
	

	alogs::setfile(logpath.c_str());

	auto modStart = bo4::BasePtr();

	LOG_INFO("-- Dll sync -----------------------");
	LOG_INFO("workdir: {0}", g_cliData.workDir);
	LOG_INFO("execdir: {0}", g_cliData.execDir);
	LOG_INFO("modstrt: {:#010x}", reinterpret_cast<uintptr_t>(modStart));
	LOG_INFO("-----------------------------------");

	if (!modStart) {
		LOG_ERROR("BAD MODULE START, ABORT");
		return;
	}

	LOG_INFO("Install detours...");

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	trueStringTable_GetAsset = reinterpret_cast<tStringTable_GetAsset>(&modStart[0x28A2660]);
	DetourAttach((PVOID*)&trueStringTable_GetAsset, StringTable_GetAsset);
	trueDB_FindXAssetHeader = reinterpret_cast<tDB_FindXAssetHeader>(&modStart[0x2EB75B0]);
	DetourAttach((PVOID*)&trueDB_FindXAssetHeader, DB_FindXAssetHeader);
	DetourTransactionCommit();

	LOG_INFO("Done.");
}

EXPORT void SyncCLI(clisync::CliSyncData* data) {
	static std::once_flag flag;
	std::call_once(flag, [data]() { SyncCLIOnce(data); });
}
