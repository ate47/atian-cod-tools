#include <includes.hpp>
#include <tools/cordycep/t10/dumper_t10.hpp>
#include <tools/cordycep/t10/dumper_t10_scriptbundle.hpp>
#include <tools/utils/raw_file_extractor.hpp>

namespace {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t10;
	using namespace tool::cordycep::dump::t10::scriptbundle;
	using namespace bo6;

	struct Curve;
	struct GametypeTable;
	struct MapTable;
	struct BulletPenetration;

	struct GameMode {
		uint64_t name;
		sbv2::ScriptBundle* bundle;
		GametypeTable* gametypeTable;
		MapTable* mapTable;
		BulletPenetration* bulletPenetration;
		const char* omnvarsCsv;
		const char* omnvarsPrefix;
		const char* scriptsGametypesPrefix;
		const char* scriptsMapsPrefix;
		uint64_t unk48;
		uint64_t unk50;
		uint64_t unk58;
		Curve* curve60;
		Curve* curve68;
		uint64_t unk70;
		uint64_t unk78;
		uint64_t unk80;
	};



	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			GameMode data{};
			if (!proc.ReadMemory(&data, asset.Header, sizeof(data))) {
				LOG_ERROR("Can't read {:x}", asset.Header);
				return false;
			}
			utils::raw_file_extractor::JsonWriter json{};

			// Dump pool
			json.BeginObject();

			json.WriteFieldNameString("name"); json.WriteValueHash(data.name);
			if (data.omnvarsCsv) { 
				json.WriteFieldNameString("omnvarsCsv"); 
				json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.omnvarsCsv))));
			}
			if (data.omnvarsPrefix) { 
				json.WriteFieldNameString("omnvarsPrefix"); 
				json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.omnvarsPrefix))));
			}
			if (data.scriptsGametypesPrefix) { 
				json.WriteFieldNameString("scriptsGametypesPrefix"); 
				json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.scriptsGametypesPrefix))));
			}
			if (data.scriptsMapsPrefix) { 
				json.WriteFieldNameString("scriptsMapsPrefix"); 
				json.WriteValueString(opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(data.scriptsMapsPrefix))));
			}


			//json.WriteFieldNameString("unk48"); json.WriteValueHash(data.unk48);
			//json.WriteFieldNameString("unk50"); json.WriteValueHash(data.unk50);
			//json.WriteFieldNameString("unk58"); json.WriteValueHash(data.unk58);
			//json.WriteFieldNameString("unk70"); json.WriteValueHash(data.unk70);
			//json.WriteFieldNameString("unk78"); json.WriteValueHash(data.unk78);
			//json.WriteFieldNameString("unk80"); json.WriteValueHash(data.unk80);
			if (data.gametypeTable) {
				json.WriteFieldNameString("gametypeTable");
				json.WriteValueHash(proc.ReadMemory<uint64_t>(reinterpret_cast<uintptr_t>(data.gametypeTable)));
			}
			if (data.mapTable) {
				json.WriteFieldNameString("mapTable");
				json.WriteValueHash(proc.ReadMemory<uint64_t>(reinterpret_cast<uintptr_t>(data.mapTable)));
			}
			if (data.bulletPenetration) {
				json.WriteFieldNameString("bulletPenetration");
				json.WriteValueHash(proc.ReadMemory<uint64_t>(reinterpret_cast<uintptr_t>(data.bulletPenetration)));
			}
			if (data.curve60) {
				json.WriteFieldNameString("curve60");
				json.WriteValueHash(proc.ReadMemory<uint64_t>(reinterpret_cast<uintptr_t>(data.curve60)));
			}
			if (data.curve68) {
				json.WriteFieldNameString("curve68");
				json.WriteValueHash(proc.ReadMemory<uint64_t>(reinterpret_cast<uintptr_t>(data.curve68)));
			}
			if (data.bundle) {
				auto bundle{ proc.ReadMemoryObjectEx<sbv2::ScriptBundle>(reinterpret_cast<uintptr_t>(data.bundle)) };
				json.WriteFieldNameString("bundle");
				if (!sbv2::WriteBundleData(opt, json, proc, bundle->data, ctx.GetLocalized)) return false;
			}


			json.EndObject();


			std::filesystem::path loc = ctx.outDir / "tables" / "gamemode" / std::format("{}.json", hashutils::ExtractTmp("file", data.name));
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());

			LOG_INFO("Dump {}", loc.string());

			if (!json.WriteToFile(loc)) {
				LOG_ERROR("Can't write {}", loc.string());
			}
			return true;
		}

	};
	utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_GAMEMODE };
}