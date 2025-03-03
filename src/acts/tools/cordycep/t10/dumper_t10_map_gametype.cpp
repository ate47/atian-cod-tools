#include <includes.hpp>
#include <tools/cordycep/t10/dumper_t10.hpp>
#include <tools/cordycep/t10/dumper_t10_scriptbundle.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <decryptutils.hpp>

namespace {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t10;
	using namespace tool::cordycep::dump::t10::scriptbundle;
	using namespace bo6;

	struct PlayerSpawnSettings;
	struct MapInfoData {
		uint64_t unk0;
		PlayerSpawnSettings* unk8;
	};

	struct MapInfo {
		uint64_t name;
		const char* nameStr;
		const char* gamemodes;
		const char* scriptsPrefix;
		const char* unk20;
		const char* unk28;
		GfxImage* unk30;
		const char* unk38;
		const char* omnvarsCsv;
		uint64_t unk48;
		uint64_t unk50;
		uint64_t unk58;
		uint64_t unk60;
		uint64_t unk68;
		uint64_t unk70;
		uint64_t unk78;
		uint64_t unk88_count;
		MapInfoData* unk88;
		sbv2::ScriptBundleObjectData bundleData;
	};


	struct MapTable {
		uint64_t name;
		uint32_t mapsCount;
		MapInfo* maps;
	};
	struct Gametype {
		uint64_t unk00;
		const char* unk08;
		const char* unk10;
		const char* unk18;
		sbv2::ScriptBundle* bundle;
		const char* unk28;
		uint64_t unk30;
		const char* unk38;
		const char* unk40;
		PlayerSpawnSettings* unk48;
		uint64_t unk50;
		uint64_t unk58;
		uint64_t unk60;
		int32_t unk60_count;
		uint64_t unk70;
		uint64_t unk78;
		uint64_t unk80;
		uint64_t unk88;
		uint64_t unk90;
		uint64_t unk98;
		uint64_t unka0;
		uint64_t unka8;
		uint64_t unkb0;
		uint64_t unkb8;
		uint64_t unkc0;
		uint64_t unkc8;
		uint64_t unkd0;
		uint64_t unkd8;
		uint64_t unke0;
		uint64_t unke8;
		uint64_t unkf0;
		uint64_t unkf8;
		uint64_t unk100;
		uint64_t unk108;
		uint64_t unk110;
		uint64_t unk118;
		uint64_t unk120;
		uint64_t unk128;
		uint64_t unk130;
		uint64_t unk138;
		uint64_t unk140;
		uint64_t unk148;
		uint64_t unk150;
		uint64_t unk158;
		uint64_t unk160;
		uint64_t unk168;
		uint64_t unk170;
		uint64_t unk178;
		uint64_t unk180;
		uint64_t unk188;
		uint64_t unk190;
		uint64_t unk198;
		uint64_t unk1a0;
		uint64_t unk1a8;
		uint64_t unk1b0;
		uint64_t unk1b8;
	};

	struct GametypeTable {
		uint64_t name;
		uint32_t gametypescount;
		Gametype* gametypes;
		uint32_t* unk20;
	};




	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			MapInfo data{};
			if (!proc.ReadMemory(&data, asset.Header, sizeof(data))) {
				LOG_ERROR("Can't read {:x}", asset.Header);
				return false;
			}
			utils::raw_file_extractor::JsonWriter json{};


			// Dump pool
			json.BeginObject();

			json.WriteFieldNameString("name");
			json.WriteValueHash(data.name);

			if (data.nameStr) {
				json.WriteFieldNameString("nameStr");
				json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.nameStr))));
			}
			if (data.gamemodes) {
				json.WriteFieldNameString("gamemodes");
				json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.gamemodes))));
			}
			if (data.scriptsPrefix) {
				json.WriteFieldNameString("scriptsPrefix");
				json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.scriptsPrefix))));
			}
			if (data.unk20) {
				json.WriteFieldNameString("unk20");
				json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.unk20))));
			}
			//if (data.unk38) {
			//	json.WriteFieldNameString("unk38");
			//	json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.unk38))));
			//}
			if (data.omnvarsCsv) {
				json.WriteFieldNameString("omnvarsCsv");
				json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.omnvarsCsv))));
			}
			if (data.unk30) {
				json.WriteFieldNameString("unk30");
				json.WriteValueHash(proc.ReadMemory<uint64_t>(reinterpret_cast<uintptr_t>(data.unk30)));
			}
			
			//json.WriteFieldNameString("unk48"); json.WriteValueHash(data.unk48);
			//json.WriteFieldNameString("unk50"); json.WriteValueHash(data.unk50);
			//json.WriteFieldNameString("unk58"); json.WriteValueHash(data.unk58);
			//json.WriteFieldNameString("unk60"); json.WriteValueHash(data.unk60);
			//json.WriteFieldNameString("unk68"); json.WriteValueHash(data.unk68);
			//json.WriteFieldNameString("unk70"); json.WriteValueHash(data.unk70);
			//json.WriteFieldNameString("unk78"); json.WriteValueHash(data.unk78);

			json.WriteFieldNameString("bundle");
			sbv2::WriteBundleData(opt, json, proc, data.bundleData, ctx.GetLocalized);
			json.EndObject();


			std::filesystem::path loc = ctx.outDir / "tables" / "map" / std::format("{}.json", hashutils::ExtractTmp("file", data.name));
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());

			LOG_INFO("Dump {}", loc.string());

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
			MapTable data{};
			if (!proc.ReadMemory(&data, asset.Header, sizeof(data))) {
				LOG_ERROR("Can't read {:x}", asset.Header);
				return false;
			}
			std::filesystem::path loc = ctx.outDir / "tables" / "map" / "table" / std::format("{}.csv", hashutils::ExtractTmp("file", data.name));
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());

			LOG_INFO("Dump {}", loc.string());

			utils::OutFileCE os{ loc };

			if (!os) {
				LOG_ERROR("Can't write {}", loc.string());
				return false;
			}

			os << "id,map";

			if (data.mapsCount) {
				auto maps{ proc.ReadMemoryArrayEx<uintptr_t>(reinterpret_cast<uintptr_t>(data.maps), data.mapsCount) };
				for (size_t i = 0; i < data.mapsCount; i++) {
					os << "\n" << std::dec << i << ",#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(maps[i]));
				}
			}


			return true;
		}

	};

	class TableGTUnlinkerImpl : public Unlinker {
	public:
		TableGTUnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			GametypeTable data{};
			if (!proc.ReadMemory(&data, asset.Header, sizeof(data))) {
				LOG_ERROR("Can't read {:x}", asset.Header);
				return false;
			}
			std::filesystem::path loc = ctx.outDir / "tables" / "gametype" / "table" / std::format("{}.csv", hashutils::ExtractTmp("file", data.name));
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());

			LOG_INFO("Dump {}", loc.string());

			utils::OutFileCE os{ loc };

			if (!os) {
				LOG_ERROR("Can't write {}", loc.string());
				return false;
			}

			os << "id,map";

			if (data.unk20) os << ",val";

			if (data.gametypescount) {
				auto gametypes{ proc.ReadMemoryArrayEx<uintptr_t>(reinterpret_cast<uintptr_t>(data.gametypes), data.gametypescount) };
				auto unk20{ data.unk20 ? proc.ReadMemoryArrayEx<uint32_t>(reinterpret_cast<uintptr_t>(data.unk20), data.gametypescount) : std::make_unique<uint32_t[]>(data.gametypescount) };
				for (size_t i = 0; i < data.gametypescount; i++) {
					os << "\n"
						<< std::dec << i
						<< ",#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(gametypes[i]));

					if (data.unk20) os << "," << unk20[i];
				}
			}


			return true;
		}

	};
	utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_MAPINFO };
	utils::MapAdder<TableUnlinkerImpl, bo6::T10RAssetType, Unlinker> implt{ GetUnlinkers(), bo6::T10R_ASSET_MAPTABLE };
	utils::MapAdder<TableGTUnlinkerImpl, bo6::T10RAssetType, Unlinker> implgt{ GetUnlinkers(), bo6::T10R_ASSET_GAMETYPETABLE };
}