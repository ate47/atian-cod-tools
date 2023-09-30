#include <dll_includes.hpp>
#include "cli_connect.hpp"

clisync::CliSyncData g_cliData{};

struct DetourRegistryData {
	LPCCH title;
	LPVOID pointer;
	LPVOID detour;
};

static std::vector<DetourRegistryData>& detourRegistryData() {
	static std::vector<DetourRegistryData> vec{};
	return vec;
};

void cliconnect::RegisterDetour(LPCCH title, LPVOID pointer, LPVOID detour) {
	detourRegistryData().emplace_back(title, pointer, detour);
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

	auto error = false;
	LONG err;
	for (auto& detourData : detourRegistryData()) {
		LOG_INFO("detouring {}@{:x}->{:x}", detourData.title, reinterpret_cast<uintptr_t>(detourData.pointer), reinterpret_cast<uintptr_t>(detourData.detour));
		err = DetourAttach(reinterpret_cast<PVOID*>(detourData.pointer), detourData.detour);

		if (err != NO_ERROR) {
			error = true;
			LPSTR messageBuffer = nullptr;
			FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
			LOG_ERROR("Error when attaching detour {}: {} ({:x})", detourData.title, messageBuffer, err);
			LocalFree(messageBuffer);
			break;
		}
	}

	if (!error) {
		err = DetourTransactionCommit();
		if (err != NO_ERROR) {
			error = true;
			LPSTR messageBuffer = nullptr;
			FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
			LOG_ERROR("Error when commit transaction: {} ({:x})", messageBuffer, err);
			LocalFree(messageBuffer);
		}
	}

	if (error) {
		DetourTransactionAbort();
		return;
	}


	auto strLookup = (std::filesystem::path{ g_cliData.workDir } / "strings.txt").string();

	LOG_INFO("Syncing hash map '{}'", strLookup.data());

	hash_lookup::LoadFile(strLookup.data());

	stringtables::SyncTables();

	// add the commands

	REGISTER_COMMAND(ACTS_CSV_READER, "acts_csv", []() { stringtables::SyncTables(); });

	LOG_INFO("Done.");
}

EXPORT void SyncCLI(clisync::CliSyncData* data) {
	static std::once_flag flag;
	std::call_once(flag, [data]() { SyncCLIOnce(data); });
}
