#include <includes.hpp>
#include "tools/tools_nui.hpp"
#include <core/config.hpp>
#include <tools/pool.hpp>
#include <games/bo4/pool.hpp>
#include <games/bo4/offsets.hpp>
#include <utils/memapi_calls.hpp>

namespace {
	static const char* gametypes[]{
		"warzone_solo\0Blackout/Solo",
		"warzone_duo\0Blackout/Duo",
		"warzone_quad\0Blackout/Quad",
		"warzone_escape_duo_dbno\0Blackout/alcatraz portal duo",
		"warzone_escape_quad_dbno\0Blackout/alcatraz portal quad",
		"warzone_hot_pursuit\0Blackout/hot pursuit",
		"warzone_heavy_metal_heroes\0Blackout/Heavy metal heroes",
		"warzone_hardcore_solo\0Blackout/Hardcore solo",
		"warzone_hardcore_duo\0Blackout/Hardcore duo",
		"warzone_hardcore_quad\0Blackout/Hardcore quad",
		"warzone_escape_solo\0Blackout/Escape solo",
		"warzone_bigteam_dbno_quad\0Blackout/Ground war",
		"warzone_pandemic_quad\0Blackout/Pandemic",
		"warzone_ambush_solo\0Blackout/Ambush solo",
		"warzone_ambush_duo\0Blackout/Ambush duo",
		"warzone_ambush_quads\0Blackout/Ambush quads",
		"warzone_deposit\0Blackout/hot pursuit"
		"warzone_heavy_metal\0Blackout/heavy metal(no heroes)",
		"warzone_spectre_rising\0Blackout/spectre rising DLC",
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
		"bounty\0Multiplayer/Heist",
		"bounty_hc\0Multiplayer/Heist (HC)",
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
	};
	static const char* maps[]{
		"wz_open_skyscrapers\0Blackout/Blackout Core Map",
		//"wz_open_skyscrapers_alt\0Blackout/Blackout Core Map Pandemic",
		"wz_escape\0Blackout/Alcatraz",
		"wz_escape_alt\0Blackout/Alcatraz Night",
		"mp_austria\0Multiplayer/Der Schatten",
		"mp_cairo\0Multiplayer/Havana",
		"mp_casino\0Multiplayer/Casino",
		"mp_cosmodrome\0Multiplayer/Launch",
		"mp_elevation\0Multiplayer/Elevation",
		"mp_embassy\0Multiplayer/Masquerade",
		"mp_firingrange2\0Multiplayer/Firing Range",
		"mp_firingrange2_alt\0Multiplayer/Firing Range Night",
		"mp_frenetic\0Multiplayer/Frequency",
		"mp_geothermal\0Multiplayer/Artifact",
		"mp_gridlock\0Multiplayer/Gridlock",
		"mp_grind\0Multiplayer/Grind",
		"mp_hacienda\0Multiplayer/Hacienda",
		"mp_hacienda_alt\0Multiplayer/Hacienda Twilight",
		"mp_icebreaker\0Multiplayer/Icebreaker",
		"mp_jungle2\0Multiplayer/Jungle",
		"mp_jungle2_alt\0Multiplayer/Jungle Flooded",
		"mp_madagascar\0Multiplayer/Madagascar",
		"mp_maldives\0Multiplayer/Lair",
		"mp_militia\0Multiplayer/Militia",
		"mp_morocco\0Multiplayer/Morroco",
		"mp_mountain2\0Multiplayer/Summit",
		"mp_nuketown_4\0Multiplayer/Nuketown",
		"mp_offshore\0Multiplayer/Contraband",
		"mp_offshore_alt\0Multiplayer/Contraband Hurricane",
		"mp_russianbase\0Multiplayer/WMD",
		"mp_seaside\0Multiplayer/Seaside",
		"mp_seaside_alt\0Multiplayer/Seaside Sunset",
		"mp_silo\0Multiplayer/Payload",
		"mp_slums2\0Multiplayer/Slums",
		"mp_station\0Multiplayer/Lockup",
		"mp_urban\0Multiplayer/Arsenal",
		//"mp_urban\0Multiplayer/Arsenal Sandstorm",
		"mp_zombie_museum\0Multiplayer/Remnant",
	};
	static const char* wzgts[]{
		"1wzenable2xscope\0Scope 2x",
		"1wzenable3xscope\0Scope 3x",
		"1wzenable4xscope\0Scope 4x",
		"1wzenableabr\0Abr",
		"1wzenableacidbomb\0Acidbomb",
		"1wzenableactionfigure\0Actionfigure",
		"1wzenablealistairsfolly\0Alistairsfolly",
		"1wzenableamulet\0Amulet",
		"1wzenablean94\0An94",
		"1wzenableannihilator\0Annihilator",
		"1wzenablearav\0Arav",
		"1wzenableargus\0Argus",
		"1wzenablearmorplate\0Armorplate",
		"1wzenableattacklittlebird\0Attacklittlebird",
		"1wzenableatv\0Atv",
		"1wzenableauger\0Auger",
		"1wzenablebackpack\0Backpack",
		"1wzenablebandages\0Bandages",
		"1wzenablebarricade\0Barricade",
		"1wzenableblackjackstash\0Blackjackstash",
		"1wzenableblade\0Blade",
		"1wzenableblundergat\0Blundergat",
		"1wzenablebountyhuntervehicles\0Bountyhuntervehicles",
		"1wzenablebowieknife\0Bowieknife",
		"1wzenablecargotruck\0Cargotruck",
		"1wzenableclustergrenade\0Clustergrenade",
		"1wzenablecoinbag\0Coinbag",
		"1wzenablecombataxe\0Combataxe",
		"1wzenableconcussiongrenade\0Concussiongrenade",
		"1wzenablecontrabandstash\0Contrabandstash",
		"1wzenablecordite\0Cordite",
		"1wzenablecymbalmonkey\0Cymbalmonkey",
		"1wzenabledaemon\0Daemon",
		"1wzenabledart\0Dart",
		"1wzenabledeathoforion\0Deathoforion",
		"1wzenabledoublebarrel\0Doublebarrel",
		"1wzenableelo\0Elo",
		"1wzenableempgrenade\0Empgrenade",
		"1wzenableessex\0Essex",
		"1wzenableextbarrel\0Extbarrel",
		"1wzenableextfastmag\0Extfastmag",
		"1wzenableextmag\0Extmag",
		"1wzenablefastmag\0Fastmag",
		"0wzenableflareguns\0Flareguns",
		"1wzenableforegrip\0Foregrip",
		"1wzenablefraggrenade\0Fraggrenade",
		"1wzenablegambit22\0Gambit22",
		"1wzenablegks\0Gks",
		"1wzenablegrapple\0Grapple",
		"1wzenablegrav\0Grav",
		"1wzenablehades\0Hades",
		"1wzenablehawk\0Hawk",
		"1wzenableheavymetal\0Heavymetal",
		"1wzenableheavymetalvehicles\0Heavymetalvehicles",
		"1wzenablehellsretriever\0Hellsretriever",
		"1wzenableholo\0Holo",
		"1wzenablehomewrecker\0Homewrecker",
		"1wzenablehomunculus\0Homunculus",
		"1wzenablehotpursuit\0Hotpursuit",
		"1wzenableicr\0Icr",
		"1wzenablekap45\0Kap45",
		"1wzenablekn\0Kn",
		"1wzenablekoshka\0Koshka",
		"1wzenablelasersight\0Lasersight",
		"1wzenablelauncher\0Launcher",
		"1wzenablelittlebird\0Littlebird",
		"1wzenablelv1armor\0Lv1armor",
		"1wzenablelv2armor\0Lv2armor",
		"1wzenablelv3armor\0Lv3armor",
		"1wzenablem16\0M16",
		"1wzenablemaddox\0Maddox",
		"1wzenablemedkit\0Medkit",
		"1wzenablemeshmines\0Meshmines",
		"1wzenableminigun\0Minigun",
		"1wzenablemog12\0Mog12",
		"1wzenablemolotov\0Molotov",
		"1wzenablemotorcycle\0Motorcycle",
		"1wzenablemozu\0Mozu",
		"1wzenablemp40\0Mp40",
		"1wzenablemusclecar\0Musclecar",
		"1wzenablemx9\0Mx9",
		"1wzenableoperatorweapons\0Operatorweapons",
		"1wzenableoutlaw\0Outlaw",
		"1wzenablepaladin\0Paladin",
		"1wzenablepbr\0Pbr",
		"1wzenableperkawareness\0Perk awareness",
		"1wzenableperkbrawler\0Perk brawler",
		"1wzenableperkconsumer\0Perk consumer",
		"1wzenableperkdeadsilence\0Perk deadsilence",
		"1wzenableperkengineer\0Perk engineer",
		"1wzenableperkironlungs\0Perk ironlungs",
		"1wzenableperklooter\0Perk looter",
		"1wzenableperkmedic\0Perk medic",
		"1wzenableperkmobility\0Perk mobility",
		"1wzenableperkoutlander\0Perk outlander",
		"1wzenableperkparanoia\0Perk paranoia",
		"1wzenableperkreinforced\0Perk reinforced",
		"1wzenableperksquadlink\0Perk squadlink",
		"0wzenableperkstimulant\0Perk stimulant",
		"1wzenablepistolgrip\0Pistol grip",
		"1wzenablepurifier\0Purifier",
		"1wzenablerampage\0Rampage",
		"1wzenablerampart\0Rampart",
		"1wzenableraygun\0Raygun",
		"1wzenableraygunmark2\0Raygunmark2",
		"1wzenableraygunmark2variants\0Raygunmark2 variants",
		"1wzenablerazorwire\0Razorwire",
		"1wzenablereconcar\0Reconcar",
		"1wzenablereflex\0Reflex",
		"1wzenablereplacercar\0Replacer car",
		"1wzenablerk7\0Rk7",
		"1wzenablesaug\0Saug",
		"1wzenablesavageimpaler\0Savage impaler",
		"1wzenablesdm\0Sdm",
		"1wzenablesecretsanta\0Secretsanta",
		"1wzenablesensordart\0Sensordart",
		"0wzenablesentrygun\0Sentrygun",
		"1wzenablesg12\0SG-12",
		"1wzenableslaybell\0Slaybell",
		"1wzenablesmokegrenade\0Smokegrenade",
		"1wzenablesniperscope\0Sniperscope",
		"1wzenablesparrow\0Sparrow",
		"1wzenablespectregrenade\0Spectregrenade",
		"1wzenablespitfire\0Spitfire",
		"0wzenablespraycans\0Spraycans",
		"1wzenablestock\0Stock",
		"1wzenablestopsign\0Stopsign",
		"1wzenablestrife\0Strife",
		"1wzenablesuppressor\0Suppressor",
		"1wzenablesuv\0Suv",
		"1wzenableswat\0Swat",
		"1wzenableswitchblade\0Switchblade",
		"1wzenableswordfish\0Swordfish",
		"1wzenabletacticalraft\0Tacticalraft",
		"1wzenabletak5\0Tak5",
		"1wzenabletank\0Tank",
		"1wzenabletitan\0Titan",
		"1wzenabletraumakit\0Traumakit",
		"1wzenabletrophysystem\0Trophysystem",
		"1wzenablevapr\0Vapr",
		"1wzenablevendetta\0Vendetta",
		"1wzenablevivaldi\0Vivaldi",
		"1wzenablevkm\0Vkm",
		"1wzenablevmp\0Vmp",
		"1wzenablewallbuyasylum\0Wallbuy Asylum",
		"1wzenablewallbuyboxinggym\0Wallbuy Boxing gym",
		"1wzenablewallbuycemetary\0Wallbuy Cemetary",
		"1wzenablewallbuydiner\0Wallbuy diner",
		"1wzenablewallbuyfarm\0Wallbuy farm",
		"1wzenablewallbuyghosttown\0Wallbuy ghost town",
		"1wzenablewallbuylighthouse\0Wallbuy lighthouse",
		"1wzenablewallbuynuketown\0Wallbuy nuketown",
		"1wzenablewarmachine\0War machine",
		"1wzenablewintersfury\0Wintersfury",
		"1wzenablewraithfire\0Wraith fire",
		"0wzenablezombiearm\0Zombie arm",
		"1wzenablezweihander\0Zweihander",
	};
	static const char* wzgts2[]{
		"1wzenablecowardswayout\0Cowards wayout",
		"0skiplaststand\0Skip last stand",
		"0wzavogadro\0Spawn avogadro",
		"0wzavogadroeverywhere\0Spawn avogadro everywhere",
		"0wzbrutus\0Spawn brutus",
		"0wzbrutuseverywhere\0Spawn brutus everywhere",
		"0wzbrutuslarge\0Spawn brutus boss",
		"0wzbrutuslargeeverywhere\0Spawn brutus boss everywhere",
		"0wzgreeneyes\0Zombies with green eyes",
		//"1deathcircle\0Enable death circle",
	};

	void CallLobbyFunction(uintptr_t loc, int lobby, const char* val, std::string& logs) {
		Process bo4 = L"BlackOps4.exe";

		if (!bo4) {
			logs = "Can't find game";
			return;
		}

		if (!bo4.Open()) {
			logs = "Can't open game";
			return;
		}

		size_t sizeOut;
		uintptr_t alloc{ bo4.AllocateString(val, &sizeOut) };

		if (!alloc) {
			logs = "Can't allocate string";
			return;
		}

		memapi::Call(bo4, bo4[loc], lobby, alloc);
		bo4.FreeMemory(alloc, sizeOut);
		logs = "done";
	}

	void CfgCbuf(std::string&& cfg, std::string& logs) {
		Process bo4 = L"BlackOps4.exe";

		if (!bo4) {
			logs = "Can't find game";
			return;
		}

		if (!bo4.Open()) {
			logs = "Can't open game";
			return;
		}

		static DWORD oldPid{};
		static uintptr_t oldAlloc{};
		static size_t oldAllocSize;

		size_t sizeOut;
		uintptr_t alloc{ bo4.AllocateString(cfg.c_str(), &sizeOut)};

		if (!alloc) {
			logs = "Can't allocate string";
			return;
		}

		try {

			struct XAssetPoolEntry {
				uintptr_t pool;
				uint32_t itemSize;
				int32_t itemCount;
				byte isSingleton;
				int32_t itemAllocCount;
				uintptr_t freeHead;
			};
			struct RawFileEntry {
				uint64_t name; // 0x8
				uintptr_t pad0; // 0x10
				uintptr_t size; // 0x18
				uintptr_t buffer; // 0x20
			};


			auto pe{ bo4.ReadMemoryObjectEx<XAssetPoolEntry>(bo4[games::bo4::offset::assetPool] + games::bo4::pool::ASSET_TYPE_RAWFILE * sizeof(XAssetPoolEntry)) };

			RawFileEntry entry{};
			constexpr const char* hookCfgName = "gamedata/configs/common/default_systemlink.cfg";
			entry.name = hash::Hash64(hookCfgName);
			entry.buffer = alloc;
			entry.size = sizeOut;

			if (pe->itemSize != sizeof(entry)) {
				throw std::runtime_error("INVALID POOL SIZE");
			}

			auto entries{ bo4.ReadMemoryArrayEx<RawFileEntry>(pe->pool, pe->itemAllocCount) };

			bool patched{};
			for (size_t i = 0; i < pe->itemAllocCount; i++) {
				if (entries[i].name != entry.name) continue; // not ours
				uintptr_t entryLoc{ pe->pool + sizeof(entries[i]) * i };
				if (!bo4.WriteMemory(entryLoc, &entry, sizeof(entry))) {
					throw std::runtime_error("Can't write data");
				}
				patched = true;
				break;
			}

			if (!patched) {
				throw std::runtime_error("Can't find default_systemlink.cfg");
			}

			// remove previous data
			if (bo4.m_pid == oldPid && oldAlloc) {
				bo4.FreeMemory(oldAlloc, oldAllocSize);
				oldAlloc = 0;
			}

			oldPid = bo4.m_pid;
			oldAlloc = alloc;
			oldAllocSize = sizeOut;

			struct CBuff {
				uintptr_t buffer; // const char* 
				int32_t allocated;
				int32_t used;
			};

			auto buff{ bo4.ReadMemoryObjectEx<CBuff>(bo4[0xF99B168]) };

			const char* cmd{ utils::va("exec %s\n", hookCfgName) };
			size_t len{ std::strlen(cmd) };
			bo4.WriteMemory(buff->buffer + buff->used, cmd, len + 1);
			buff->used += (int32_t)len;
			if (!bo4.WriteMemory(bo4[0xF99B168], buff.get(), sizeof(*buff))) {
				throw std::runtime_error("Can't write cbuf");
			}
			logs = "Injected";
		}
		catch (std::runtime_error& re) {
			if (alloc) {
				bo4.FreeMemory(alloc, sizeOut);
			}
			logs = re.what();
		}
	}


	void bo4_lobby_tool() {
		tool::nui::NuiUseDefaultWindow dw{};
		ImGui::SeparatorText("Lobby tool");

		static const char* gt{ gametypes[0] };
		static const char* map{ maps[0] };
		static bool gts[ARRAYSIZE(wzgts)]{};
		static bool gts2[ARRAYSIZE(wzgts2)]{};
		static int gtsInit{ ([]() {
			for (size_t i = 0; i < ARRAYSIZE(gts); i++) {
				gts[i] = core::config::GetBool(std::format("ui.bo4lobby.{}", wzgts[i] + 1), wzgts[i][0] == '1');
			}
			for (size_t i = 0; i < ARRAYSIZE(gts2); i++) {
				gts2[i] = core::config::GetBool(std::format("ui.bo4lobby.{}", wzgts2[i] + 1), wzgts2[i][0] == '1');
			}
			return 0;
		})() };
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
			// LobbySetGameType(LobbyType, char const*).text	000000000398E410	00000010	00000028		R	.	.	.	..T	.
			CallLobbyFunction(0x398E410, 0, gt, log);
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
			// LobbySetMap(LobbyType, char const*).text	000000000398E420	00000010	00000028		R	.	.	.	..T	.
			CallLobbyFunction(0x398E420, 0, map, log);
		}

		ImGui::SeparatorText("Blackout config");

		if (ImGui::BeginListBox("Items")) {
			for (size_t i = 0; i < ARRAYSIZE(wzgts); i++) {
				if (ImGui::Checkbox(wzgts[i] + std::strlen(wzgts[i]) + 1, &gts[i])) {
					// add cfg
					core::config::SetBool(std::format("ui.bo4lobby.{}", wzgts[i] + 1), gts[i]);
					tool::nui::SaveNextConfig();
				}
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("Inject config")) {

			std::stringstream gtsCfg{};

			for (size_t i = 0; i < ARRAYSIZE(wzgts); i++) {
				gtsCfg << "gts " << (wzgts[i] + 1) << " " << (gts[i] ? '1' : '0') << "\n";
			}

			CfgCbuf(gtsCfg.str(), log);
		}
		if (ImGui::BeginListBox("Options")) {
			for (size_t i = 0; i < ARRAYSIZE(wzgts2); i++) {
				if (ImGui::Checkbox(wzgts2[i] + std::strlen(wzgts2[i]) + 1, &gts2[i])) {
					// add cfg
					core::config::SetBool(std::format("ui.bo4lobby.{}", wzgts2[i] + 1), gts2[i]);
					tool::nui::SaveNextConfig();
				}
			}
			ImGui::EndListBox();
		}
		if (ImGui::Button("Inject config")) {

			std::stringstream gtsCfg{};

			for (size_t i = 0; i < ARRAYSIZE(wzgts2); i++) {
				gtsCfg << "gts " << (wzgts2[i] + 1) << " " << (gts2[i] ? '1' : '0') << "\n";
			}

			CfgCbuf(gtsCfg.str(), log);
		}

		if (!log.empty()) {
			ImGui::Separator();

			ImGui::Text("%s", log.data());
		}
	}
	ADD_TOOL_NUI(bo4_lobby_tool, "BO4 Lobby tool", bo4_lobby_tool);
}