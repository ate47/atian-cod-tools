#include <includes_shared.hpp>
#include <hook/error.hpp>
#include <hook/library.hpp>

#define EXPORT extern "C" __declspec(dllexport)

namespace {
	hook::library::Detour GetSystemMetricsDetour;
	hook::library::Detour ExitProcessDetour;
	hook::library::Detour GetThreadContextDetour;

	DECLSPEC_NORETURN void ExitProcessStub(UINT uExitCode) {
		if (uExitCode) {
			LOG_ERROR("ExitProcess with {}", uExitCode);
			hook::error::DumpStackTraceFrom(alogs::LVL_ERROR);
		}
		else {
			LOG_INFO("ExitProcess with {}", uExitCode);
			hook::error::DumpStackTraceFrom(alogs::LVL_INFO);
		}
		ExitProcessDetour.Call(uExitCode);
	}

	BOOL WINAPI GetThreadContextStub(HANDLE hThread, LPCONTEXT lpContext) {
		LOG_INFO("GetThreadContext({}, {})", hThread, (void*)lpContext);
		return GetThreadContextDetour.Call<BOOL>(hThread, lpContext);
	}
	int GetSystemMetricsStub(int nIndex) {
		LOG_INFO("GetSystemMetrics({}) / {}", nIndex, GetCurrentThread());
		return GetSystemMetricsDetour.Call<int>(nIndex);
	}
	void Main() {
		alogs::setfile("acts-mwiii.log");
		alogs::setlevel(alogs::LVL_TRACE);
		LOG_INFO("Init MWIII dll");

		hook::error::InstallErrorHooks(true);
		hook::error::EnableHeavyDump();

		hook::library::Library kernel32 = "kernel32.dll";
		hook::library::Library user32 = "user32.dll";

		GetSystemMetricsDetour.Create(user32["GetSystemMetrics"], GetSystemMetricsStub);
		ExitProcessDetour.Create(kernel32["ExitProcess"], ExitProcessStub);
		GetThreadContextDetour.Create(kernel32["GetThreadContext"], GetThreadContextStub);


		//hook::library::InitScanContainer("acts-mwiii.scan");

		//hook::library::SaveScanContainer();

		LOG_INFO("system loaded");
	}
}
BOOL WINAPI DllMain(HMODULE hModule, DWORD Reason, LPVOID lpVoid) {
	if (Reason == DLL_PROCESS_ATTACH) {
		Main();
	}
	return TRUE;
}

namespace {
	int64_t DiscordCreateFake() {
		LOG_WARNING("Tried to get discord_game_sdk_old.dll#DiscordCreate(), but failed, did the user renamed discord_game_sdk to discord_game_sdk_old?");
		return 0;
	}
}

// 000000001368F14C		DiscordCreate	discord_game_sdk
// hook discord_game_sdk.dll for auto injection
EXPORT int64_t DiscordCreate() {
	static auto func = [] {
		hook::library::Library discord_game_sdk{ "discord_game_sdk_old.dll", true };

		if (!discord_game_sdk) return DiscordCreateFake;

		return reinterpret_cast<decltype(&DiscordCreate)>(discord_game_sdk["DiscordCreate"]);
	}();

	return func();
}

