#include <includes.hpp>
#include "tools/tools_nui.hpp"
#include <core/config.hpp>
#include <tools/bo4/pool.hpp>
#include <games/bo4/pool.hpp>
#include <games/bo4/offsets.hpp>
#include <utils/memapi_calls.hpp>

namespace {
	static const char* gametypes[]{
		"conf\0Multiplayer/KC",
		"conf_hc\0Multiplayer/KC (HC)",
		"ctf\0Multiplayer/Capture the flag",
		"ctf_hc\0Multiplayer/Capture the flag (HC)",
		"ctf_bb\0Multiplayer/Capture the flag (BB)",
		"ctf_bb_hc\0Multiplayer/Capture the flag (BB HC)",
		"dom\0Multiplayer/Domination",
		"dom_hc\0Multiplayer/Domination (HC)",
		"dom_snipe_bb\0Multiplayer/Domination sniper only",
		"dom_bb\0Multiplayer/Domination (BB)",
		"dom_bb_hc\0Multiplayer/Domination (BB HC)",
		"dom_dm\0Multiplayer/Domination team deathmatch",
		"dom_cwl\0Multiplayer/Domination (CWL)",
		"gun\0Multiplayer/Gun games",
		"gun_hc\0Multiplayer/Gun games (HC)",
		"koth\0Multiplayer/Hardpoint",
		"koth_hc\0Multiplayer/Hardpoint (HC)",
		"koth_bb\0Multiplayer/Hardpoint (BB)",
		"koth_cwl\0Multiplayer/Hardpoint (CWL)",
		//"bounty\0Multiplayer/Heist",
		//"bounty_hc\0Multiplayer/Heist (HC)",
		"infect\0Multiplayer/Infected",
		"infect_hc\0Multiplayer/Infected (HC)",
		"prop\0Multiplayer/Prop hunt",
		"prop_hc\0Multiplayer/Prop hunt (HD)",
		"sd\0Multiplayer/Search and destroy",
		"sd_bb\0Multiplayer/Search and destroy (BB)",
		"sd_hc\0Multiplayer/Search and destroy (HC)",
		"sd_cwl\0Multiplayer/Search and destroy (CWL)",
		"tdm\0Multiplayer/Team deathmatch",
		"tdm_hc\0Multiplayer/Team deathmatch (HC)",
		"tdm_bb\0Multiplayer/Team deathmatch (BB)",
		"tdm_bb_hc\0Multiplayer/Team deathmatch (BB HC)",
		"tdm_snipe_bb\0Multiplayer/Team deathmatch sniper only (BB)",
		"control\0Multiplayer/Control",
		"control_bb\0Multiplayer/Control (BB)",
		"control_hc\0Multiplayer/Control (HC)",
		"control_cwl\0Multiplayer/Control (CWL)",
		"escort\0Multiplayer/Escort",
		"escort_hc\0Multiplayer/Escort (HC)",
		"sas\0Multiplayer/Sticks and stones",
		"sas_hc\0Multiplayer/Sticks and stones (HC)",
		"oic\0Multiplayer/One in the chamber",
		"clean\0Multiplayer/clean",
		"dropkick\0Multiplayer/dropkick",
		"fireteam\0Multiplayer/fireteam",
		"scream\0Multiplayer/scream",
		"spy\0Multiplayer/spy",
		"vip\0Multiplayer/vip",
		"war\0Multiplayer/war",
	};
	static const char* maps[]{
		"mp_amerika\0mp_amerika",
		"mp_apocalypse\0mp_apocalypse",
		"mp_black_sea\0mp_black_sea",
		"mp_cartel\0mp_cartel",
		"mp_clhanger\0mp_clhanger",
		"mp_common\0mp_common",
		"mp_drivein_rm\0mp_drivein_rm",
		"mp_dune\0mp_dune",
		"mp_echelon\0mp_echelon",
		"mp_express_rm\0mp_express_rm",
		"mp_firebase\0mp_firebase",
		"mp_hijacked_rm\0mp_hijacked_rm",
		"mp_jungle_rm\0mp_jungle_rm",
		"mp_kgb\0mp_kgb",
		"mp_mall\0mp_mall",
		"mp_miami\0mp_miami",
		"mp_miami_strike\0mp_miami_strike",
		"mp_moscow\0mp_moscow",
		"mp_nuketown6\0mp_nuketown6",
		"mp_paintball_rm\0mp_paintball_rm",
		"mp_raid_rm\0mp_raid_rm",
		"mp_russianbase_rm\0mp_russianbase_rm",
		"mp_satellite\0mp_satellite",
		"mp_slums_rm\0mp_slums_rm",
		"mp_sm_amsterdam\0mp_sm_amsterdam",
		"mp_sm_berlin_tunnel\0mp_sm_berlin_tunnel",
		"mp_sm_central\0mp_sm_central",
		"mp_sm_deptstore\0mp_sm_deptstore",
		"mp_sm_finance\0mp_sm_finance",
		"mp_sm_game_show\0mp_sm_game_show",
		"mp_sm_gas_station\0mp_sm_gas_station",
		"mp_sm_market\0mp_sm_market",
		"mp_sm_vault\0mp_sm_vault",
		"mp_tank\0mp_tank",
		"mp_tundra\0mp_tundra",
		"mp_village_rm\0mp_village_rm",
		"mp_zoo_rm\0mp_zoo_rm",
		"wz_duga\0Fireteam/Duga",
		"wz_forest\0Fireteam/Forest",
		"wz_golova\0Fireteam/Golova",
		"wz_sanatorium\0Fireteam/Sanatorium",
		"wz_ski_slopes\0Fireteam/Ski_slopes",
		"wz_zoo\0Fireteam/Zoo",
	};

	void CallLobbyFunction(const char* sig, int lobby, const char* val, std::string& logs) {
		Process cw = L"BlackOpsColdWar.exe";

		if (!cw) {
			logs = "Can't find game";
			return;
		}

		if (!cw.Open()) {
			logs = "Can't open game";
			return;
		}

		static struct SigStorage {
			DWORD lastPid{};
			std::unordered_map<uint64_t, uintptr_t> scans{};
		} storage;

		uintptr_t loc;

		if (storage.lastPid != cw.m_pid) {
			storage.lastPid = cw.m_pid;
			storage.scans.clear();

			loc = cw.Scan(sig);

			if (!loc) {
				logs = "Can't find scan";
				return;
			}

			storage.scans[hash::Hash64(sig)] = loc;
		}
		else {
			auto it = storage.scans.find(hash::Hash64(sig));
			if (it != storage.scans.end()) {
				loc = it->second;
			}
			else {
				loc = cw.Scan(sig);

				if (!loc) {
					logs = "Can't find scan";
					return;
				}

				storage.scans[hash::Hash64(sig)] = loc;
			}
		}


		size_t sizeOut;
		uintptr_t alloc{ cw.AllocateString(val, &sizeOut) };

		if (!alloc) {
			logs = "Can't allocate string";
			return;
		}

		memapi::Call(cw, cw[loc], lobby, alloc);
		cw.FreeMemory(alloc, sizeOut);
		logs = "done";
	}

	void bocw_lobby_tool() {
		tool::nui::NuiUseDefaultWindow dw{};
		ImGui::SeparatorText("Lobby tool");

		static const char* gt{ gametypes[0] };
		static const char* map{ maps[0] };
		static std::string log{};

		if (ImGui::BeginCombo("##gametype", gt + std::strlen(gt) + 1)) {
			for (int n = 0; n < ARRAYSIZE(gametypes); n++) {
				bool isSelected = (gt == gametypes[n]);
				if (ImGui::Selectable(gametypes[n] + std::strlen(gametypes[n]) + 1, isSelected)) {
					gt = gametypes[n];
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}


		if (ImGui::Button("Set gametype")) {
			CallLobbyFunction("E8 ? ? ? ? 48 8B C7 0F B6 80", 0, gt, log);
		}

		if (ImGui::BeginCombo("##map", map + std::strlen(map) + 1)) {
			for (int n = 0; n < ARRAYSIZE(maps); n++) {
				bool isSelected = (map == maps[n]);
				if (ImGui::Selectable(maps[n] + std::strlen(maps[n]) + 1, isSelected)) {
					map = maps[n];
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Set map")) {
			CallLobbyFunction("E8 ? ? ? ? 0F B6 83 ? ? ? ? 38 86", 0, map, log);
		}

		if (!log.empty()) {
			ImGui::Separator();

			ImGui::Text("%s", log.data());
		}
	}
	// ADD_TOOL_NUI(bocw_lobby_tool, "BOCW Lobby tool", bocw_lobby_tool);
}