#include <dll_includes.hpp>
#include <core/system.hpp>
#include <hook/library.hpp>
#include "data/cw.hpp"

namespace exported {
	void (*LobbyData_SetGameType)(uint32_t lobbyType, const char* mode) {};
	void (*LobbyData_SetMap)(uint32_t lobbyType, const char* map) {};
}

namespace {
	void Init(uint64_t id) {
	}

	void PostInit(uint64_t id) {
		LOG_INFO("Loading exported");
		exported::LobbyData_SetGameType = hook::library::QueryScanContainerSingle("SetMap", "E8 ? ? ? ? 48 8B C7 0F B6 80").GetRelative<int32_t, decltype(exported::LobbyData_SetGameType)>(1);
		exported::LobbyData_SetMap = hook::library::QueryScanContainerSinglePtr<decltype(exported::LobbyData_SetMap)>("SetGameType", "48 89 5C 24 10 57 48 83 EC 20 48 8B DA 8B F9 8B D1 33 C9 E8 ? ? ? ? 84 C0 0F 84 B1 00 00 00 8B D7 48 89 74 24 30 33 C9 E8 ? ? ? ? 48 85 C0 75 08 8D 70 78");
	}


}
EXPORT void ACTS_EXPORT_SetLobbyGameType(const char* mode) {
	if (!exported::LobbyData_SetGameType) {
		LOG_ERROR("Invalid exported::LobbyData_SetGameType");
		return;
	}
	exported::LobbyData_SetGameType(0, mode);
}
EXPORT void ACTS_EXPORT_SetLobbyMap(const char* map) {
	if (!exported::LobbyData_SetMap) {
		LOG_ERROR("Invalid exported::LobbyData_SetMap");
		return;
	}
	exported::LobbyData_SetMap(0, map);
}
REGISTER_SYSTEM(exported, Init, PostInit);