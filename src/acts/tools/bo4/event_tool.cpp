#include <includes.hpp>
#include "tools/tools_nui.hpp"
#include <core/config.hpp>
#include <tools/pool.hpp>
#include <games/bo4/pool.hpp>
#include <games/bo4/offsets.hpp>

namespace {
	constexpr uint64_t schedule_pc_csv = hash::Hash64("gamedata/events/schedule_pc.csv");
	constexpr uint64_t start = 1545156000;
	constexpr uint64_t end = 2147367600;
	// string,int,int,string

	void bo4_event_tool() {
		tool::nui::NuiUseDefaultWindow dw{};
		ImGui::SeparatorText("Event tool");

		static const char* op1s[]{
			"season_2_stream",
			"season_3_stream",
			"season_4_stream",
			"season_5_stream",
			"season_6_stream",
			"season_7_stream",
			"season_8_stream",
		};
		static const char* op2s[]{
			"",
			"season_2_bonus_stream",
			"summer_break_bundle",
			"fourth_of_july_event_stream",
			"labor_day_event_stream",
		};

		static bool set2xp;
		static bool set2wxp;
		static bool set2m;
		static bool set2nb;
		static bool set2xptier;
		static const char* op1{ op1s[ARRAYSIZE(op1s) - 1] };
		static const char* op2{ op2s[0] };
		static uintptr_t currentData{};
		static size_t currentDataSize{};
		static DWORD currentPID{};
		static std::string notif{};

		static std::once_flag of{};

		std::call_once(of, [] {
			set2xp = core::config::GetBool("ui.bo4event.s2xp", false);
			set2wxp = core::config::GetBool("ui.bo4event.s2wxp", false);
			set2m = core::config::GetBool("ui.bo4event.s2m", false);
			set2nb = core::config::GetBool("ui.bo4event.s2nb", false);
			set2xptier = core::config::GetBool("ui.bo4event.s2xptier", false);
			tool::nui::SaveNextConfig();
		});

		bool c{};
		if (ImGui::Checkbox("Double XP", &set2xp)) {
			core::config::SetBool("ui.bo4event.s2xp", set2xp);
			c = true;
		}
		if (ImGui::Checkbox("Double Weapon XP", &set2wxp)) {
			core::config::SetBool("ui.bo4event.s2wxp", set2wxp);
			c = true;
		}
		if (ImGui::Checkbox("Double Merits", &set2m)) {
			core::config::SetBool("ui.bo4event.s2m", set2m);
			c = true;
		}
		if (ImGui::Checkbox("Double Nebula", &set2nb)) {
			core::config::SetBool("ui.bo4event.s2nb", set2nb);
			c = true;
		}
		if (ImGui::Checkbox("Double Tiers", &set2xptier)) {
			core::config::SetBool("ui.bo4event.s2xptier", set2xptier);
			c = true;
		}
		ImGui::SeparatorText("Main operation");

		if (ImGui::BeginCombo("##op1", op1)) {
			for (int n = 0; n < ARRAYSIZE(op1s); n++)
			{
				bool isSelected = (op1 == op1s[n]);
				if (ImGui::Selectable(op1s[n], isSelected)) {
					op1 = op1s[n];
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SeparatorText("Operation 2");

		if (ImGui::BeginCombo("##op2", op2)) {
			for (int n = 0; n < ARRAYSIZE(op2s); n++)
			{
				bool isSelected = (op2 == op2s[n]);
				if (ImGui::Selectable(op2s[n], isSelected)) {
					op2 = op2s[n];
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (c) tool::nui::SaveNextConfig();
		ImGui::Separator();

		if (ImGui::Button("Inject")) {
			std::vector<const char*> events{};

			// I guess it's mandatory to have them
			events.push_back("digital_refresh_v3");
			events.push_back("reserves_drop_12");
			events.push_back("bribe_offer_holidays_2019");
			events.push_back("reserve_completion_meter");

			if (set2xp) {
				events.push_back("global_2xp_mp_server");
				events.push_back("global_2xp_mp_client");
				events.push_back("global_2xp_zm_server");
				events.push_back("global_2xp_zm_client");
			}
			if (set2wxp) {
				events.push_back("global_2wxp_mp_server");
				events.push_back("global_2wxp_mp_client");
				events.push_back("global_2wxp_zm_server");
				events.push_back("global_2wxp_zm_client");
			}
			if (set2m) {
				events.push_back("global_2x_merits_wz_server");
				events.push_back("global_2x_merits_wz_client");
			}
			if (set2nb) {
				events.push_back("global_2xnp_zm_server");
				events.push_back("global_2xnp_zm_client");
			}
			if (set2xptier) {
				events.push_back("global_2xtier_mp_server");
				events.push_back("global_2xtier_mp_client");
				events.push_back("global_2xtier_wz_server");
				events.push_back("global_2xtier_wz_client");
			}
			if (*op1) {
				events.push_back(op1);
			}
			if (*op2) {
				events.push_back(op2);
			}

			LOG_TRACE("Injecting({})", events.size());
			for (const char* ev : events) {
				LOG_TRACE("{}", ev);
			}


			Process bo4 = L"BlackOps4.exe";
			uintptr_t allocated{};
			size_t allocatedSize{};
			try {

				if (!bo4 || !bo4.Open()) {
					throw std::runtime_error("Can't open bo4");
				}

				std::vector<byte> rfile{};
				std::vector<size_t> eventsStrings{};

				// index 0 is all for event type
				size_t allLoc{ utils::WriteString(rfile, "all") };

				enum StringTableCellType : int32_t {
					STC_TYPE_UNDEFINED = 0,
					STC_TYPE_STRING = 1,
					STC_TYPE_HASHED = 2,
					STC_TYPE_INT = 4,
					STC_TYPE_FLOAT = 5,
					STC_TYPE_BOOL = 6
				};

				struct StringTableCell {
					byte value[20] = {};
					StringTableCellType type{};
				};

				struct StringTableEntry {
					tool::pool::XHash name{};
					int32_t columnCount{};
					int32_t rowCount{};
					int32_t cellscount{};
					int32_t unk24{};
					uintptr_t cells{};
					uintptr_t values{}; // StringTableCell*
					uintptr_t unk48{};
					uintptr_t unk56{};
				}; static_assert(sizeof(StringTableEntry) == 0x40);

				size_t cells{ utils::Allocate(rfile, sizeof(StringTableCell) * 4 * events.size()) };

				for (size_t i = 0; i < events.size(); i++) {
					size_t evLoc{ utils::WriteString(rfile, events[i]) };
					StringTableCell* row{ &reinterpret_cast<StringTableCell*>(&rfile[cells])[i * 4] };

					row[0].type = STC_TYPE_STRING;
					*(size_t*)&(row[0].value) = evLoc;
					row[1].type = STC_TYPE_INT;
					*(int64_t*)&(row[1].value) = start;
					row[2].type = STC_TYPE_INT;
					*(int64_t*)&(row[2].value) = end;
					row[3].type = STC_TYPE_STRING;
					*(size_t*)&(row[3].value) = allLoc;
				}

				StringTableEntry entry{};
				entry.name.name = schedule_pc_csv;
				entry.columnCount = 4;
				entry.rowCount = (int32_t)events.size();

				allocatedSize = rfile.size();
				allocated = bo4.AllocateMemory(allocatedSize);

				if (!allocated) {
					throw std::runtime_error(std::format("Can't allocate {} bytes", rfile.size()));
				}

				// link strings
				entry.values = allocated + cells;
				StringTableCell* row{ reinterpret_cast<StringTableCell*>(&rfile[cells]) };
				for (size_t i = 0; i < events.size(); i++) {
					*(uintptr_t*)row[i * 4].value += allocated;
					*(uintptr_t*)row[i * 4 + 3].value += allocated;
				}

				// write data
				if (!bo4.WriteMemory(allocated, rfile.data(), allocatedSize)) {
					throw std::runtime_error(std::format("Can't write {} bytes", rfile.size()));
				}

				struct XAssetPoolEntry {
					uintptr_t pool;
					uint32_t itemSize;
					int32_t itemCount;
					byte isSingleton;
					int32_t itemAllocCount;
					uintptr_t freeHead;
				};


				auto pe{ bo4.ReadMemoryObjectEx<XAssetPoolEntry>(bo4[games::bo4::offset::assetPool] + games::bo4::pool::ASSET_TYPE_STRINGTABLE * sizeof(XAssetPoolEntry))};

				if (pe->itemSize != sizeof(entry)) {
					throw std::runtime_error("INVALID POOL SIZE");
				}

				auto entries{ bo4.ReadMemoryArrayEx<StringTableEntry>(pe->pool, pe->itemAllocCount) };

				bool patched{};
				for (size_t i = 0; i < pe->itemAllocCount; i++) {
					if (entries[i].name.name != entry.name.name) continue; // not ours
					uintptr_t entryLoc{ pe->pool + sizeof(entries[i]) * i };
					if (!bo4.WriteMemory(entryLoc, &entry, sizeof(entry))) {
						throw std::runtime_error("Can't write data");
					}
					notif = "Injected";
					patched = true;
					break;
				}

				if (!patched) {
					throw std::runtime_error("Can't find schedule_pc.csv");
				}

				// remove previous data
				if (currentPID == bo4.m_pid) {
					bo4.FreeMemory(currentData, currentDataSize);
				}
				currentPID = bo4.m_pid;
				currentData = allocated;
				currentDataSize = rfile.size();
			}
			catch (std::runtime_error& re) {
				if (allocated) {
					bo4.FreeMemory(allocated, allocatedSize);
				}
				notif = re.what();
			}
		}


		if (!notif.empty()) {
			ImGui::Separator();

			ImGui::Text("%s", notif.data());
		}
		
	}

	ADD_TOOL_NUI(bo4_event_tool, "BO4 Events tool", bo4_event_tool);

}