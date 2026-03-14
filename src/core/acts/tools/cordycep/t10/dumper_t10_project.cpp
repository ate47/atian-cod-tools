#include <includes.hpp>
#include <tools/cordycep/t10/dumper_t10.hpp>
#include <tools/utils/raw_file_extractor.hpp>

namespace {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t10;
	using namespace bo6;

	struct GameMode;

	struct Project {
		uint64_t name;
		uint64_t unk8;
		uint32_t gamemodesCount;
		GameMode** gamemodes;
		const char* unk20;
	};

	struct ProjectTable {
		uint64_t name;
		uint32_t projectsCount;
		Project* projects;
	};

	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			Project entry{};
			if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
				LOG_ERROR("Can't read {:x}", asset.Header);
				return false;
			}

			std::filesystem::path loc = ctx.outDir / "tables" / "project" / std::format("{}.json", hashutils::ExtractTmp("file", entry.name));
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());
			utils::raw_file_extractor::JsonWriter json{};

			LOG_INFO("Dump {}", loc.string());

			// Dump pool
			json.BeginObject();

			json.WriteFieldNameString("name");
			json.WriteValueHash(entry.name);
			if (entry.unk8) {
				json.WriteFieldNameString("unk8");
				json.WriteValueHash(entry.unk8);
			}
			if (entry.unk20) {
				json.WriteFieldNameString("unk20");
				json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(entry.unk20))));
			}

			if (entry.gamemodesCount) {
				json.WriteFieldNameString("gamemodes");
				json.BeginArray();
				auto gamemodes{ proc.ReadMemoryArrayEx<uintptr_t>(reinterpret_cast<uintptr_t>(entry.gamemodes), entry.gamemodesCount) };

				for (size_t i = 0; i < entry.gamemodesCount; i++) {
					json.WriteValueHash(proc.ReadMemory<uint64_t>(gamemodes[i]));
				}
				json.EndArray();
			}

			json.EndObject();


			if (!json.WriteToFile(loc)) {
				LOG_ERROR("Can't write {}", loc.string());
			}
			return true;
		}

	};


	class TableUnlinkerImpl : public Unlinker {
	public:
		TableUnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			ProjectTable data{};
			if (!proc.ReadMemory(&data, asset.Header, sizeof(data))) {
				LOG_ERROR("Can't read {:x}", asset.Header);
				return false;
			}

			std::filesystem::path loc = ctx.outDir / "tables" / "project" / "table" / std::format("{}.csv", hashutils::ExtractTmp("file", data.name));
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());

			LOG_INFO("Dump {} ({})", loc.string(), data.projectsCount);

			utils::OutFileCE os{ loc };

			if (!os) {
				LOG_ERROR("Can't open {}", loc.string());
				return false;
			}
			os << "id,name";

			if (data.projectsCount) {
				auto projects{ proc.ReadMemoryArrayEx<uintptr_t>(reinterpret_cast<uintptr_t>(data.projects), data.projectsCount) };

				for (size_t i = 0; i < data.projectsCount; i++) {
					os << "\n" << std::dec << i << ",#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(projects[i]));
				}
			}

			return true;
		}

	};
	utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_PROJECT };
	utils::MapAdder<TableUnlinkerImpl, bo6::T10RAssetType, Unlinker> implt{ GetUnlinkers(), bo6::T10R_ASSET_PROJECTTABLE };
}