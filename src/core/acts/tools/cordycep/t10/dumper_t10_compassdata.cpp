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
	struct CompassData
	{
		uint64_t name;
		GfxImage* iconMinimapPlayer;
		GfxImage* hudIconMinimapPlayer;
		GfxImage* hudIconMinimapPlayerShoot;
		GfxImage* hudIconMinimapPlayerTalk;
		GfxImage* unk28;
		GfxImage* unk30;
		GfxImage* unk38;
		GfxImage* unk40;
		GfxImage* unk48;
		GfxImage* unk50;
		GfxImage* unk58;
		GfxImage* unk60;
		GfxImage* unk68;
		GfxImage* unk70;
		GfxImage* unk78;
		GfxImage* unk80;
		GfxImage* unk88;
		GfxImage* unk90;
		GfxImage* unk98;
		GfxImage* unka0;
		GfxImage* unka8;
		GfxImage* unkb0;
		GfxImage* unkb8;
		GfxImage* unkc0;
		GfxImage* unkc8;
		GfxImage* unkd0;
		GfxImage* unkd8;
		GfxImage* unke0;
		GfxImage* unke8;
		GfxImage* unkf0;
		GfxImage* unkf8;
		GfxImage* unk100;
		GfxImage* unk108;
		GfxImage* iconMinimapCruseMissile;
		GfxImage* unk118;
		GfxImage* unk120;
		GfxImage* unk128;
		GfxImage* unk130;
		GfxImage* unk138;
		GfxImage* unk140[6];
		GfxImage* unk170[3];
		GfxImage* unk188[3];
		GfxImage* unk1a0;
		GfxImage* unk1a8;
		GfxImage* unk1b0;
		GfxImage* unk1b8;
		GfxImage* unk1c0;
		GfxImage* unk1c8;
		GfxImage* unk1d0;
		GfxImage* unk1d8;
		GfxImage* unk1e0;
		GfxImage* unk1e8;
		GfxImage* unk1f0;
		GfxImage* unk1f8;
		GfxImage* unk200;
		GfxImage* unk208;
		GfxImage* unk210;
		GfxImage* unk218;
		GfxImage* unk220;
		GfxImage* unk228;
		GfxImage* unk230;
		GfxImage* unk238;
		GfxImage* unk240;
		GfxImage* unk248;
		GfxImage* unk250[3];
		GfxImage* unk268[3];
		GfxImage* unk280;
		uint64_t unk288;
		uint64_t unk290;
		uint64_t unk298;
		uint64_t unk2a0;
		uint64_t unk2a8;
		uint64_t unk2b0;
		scriptbundle::sbv2::ScriptBundleObjectData bundle;
	};




	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			CompassData data{};
			if (!proc.ReadMemory(&data, asset.Header, sizeof(data))) {
				LOG_ERROR("Can't read {:x}", asset.Header);
				return false;
			}
			utils::raw_file_extractor::JsonWriter json{};

			// Dump pool
			json.BeginObject();

			json.WriteFieldNameString("name"); json.WriteValueHash(data.name);

			auto WriteImage = [&json, &proc](const char* name, void* ptr) {
				if (!ptr) return;
				json.WriteFieldNameString(name);
				json.WriteValueHash(proc.ReadMemory<uint64_t>(reinterpret_cast<uintptr_t>(ptr)));
			};


			WriteImage("iconMinimapPlayer", data.iconMinimapPlayer);
			WriteImage("hudIconMinimapPlayer", data.hudIconMinimapPlayer);
			WriteImage("hudIconMinimapPlayerShoot", data.hudIconMinimapPlayerShoot);
			WriteImage("hudIconMinimapPlayerTalk", data.hudIconMinimapPlayerTalk);
			WriteImage("unk28", data.unk28);
			WriteImage("unk30", data.unk30);
			WriteImage("unk38", data.unk38);
			WriteImage("unk40", data.unk40);
			WriteImage("unk48", data.unk48);
			WriteImage("unk50", data.unk50);
			WriteImage("unk58", data.unk58);
			WriteImage("unk60", data.unk60);
			WriteImage("unk68", data.unk68);
			WriteImage("unk70", data.unk70);
			WriteImage("unk78", data.unk78);
			WriteImage("unk80", data.unk80);
			WriteImage("unk88", data.unk88);
			WriteImage("unk90", data.unk90);
			WriteImage("unk98", data.unk98);
			WriteImage("unka0", data.unka0);
			WriteImage("unka8", data.unka8);
			WriteImage("unkb0", data.unkb0);
			WriteImage("unkb8", data.unkb8);
			WriteImage("unkc0", data.unkc0);
			WriteImage("unkc8", data.unkc8);
			WriteImage("unkd0", data.unkd0);
			WriteImage("unkd8", data.unkd8);
			WriteImage("unke0", data.unke0);
			WriteImage("unke8", data.unke8);
			WriteImage("unkf0", data.unkf0);
			WriteImage("unkf8", data.unkf8);
			WriteImage("unk100", data.unk100);
			WriteImage("unk108", data.unk108);
			WriteImage("iconMinimapCruseMissile", data.iconMinimapCruseMissile);
			WriteImage("unk118", data.unk118);
			WriteImage("unk120", data.unk120);
			WriteImage("unk128", data.unk128);
			WriteImage("unk130", data.unk130);
			WriteImage("unk138", data.unk138);
			WriteImage("unk1a0", data.unk1a0);
			WriteImage("unk1a8", data.unk1a8);
			WriteImage("unk1b0", data.unk1b0);
			WriteImage("unk1b8", data.unk1b8);
			WriteImage("unk1c0", data.unk1c0);
			WriteImage("unk1c8", data.unk1c8);
			WriteImage("unk1d0", data.unk1d0);
			WriteImage("unk1d8", data.unk1d8);
			WriteImage("unk1e0", data.unk1e0);
			WriteImage("unk1e8", data.unk1e8);
			WriteImage("unk1f0", data.unk1f0);
			WriteImage("unk1f8", data.unk1f8);
			WriteImage("unk200", data.unk200);
			WriteImage("unk208", data.unk208);
			WriteImage("unk210", data.unk210);
			WriteImage("unk218", data.unk218);
			WriteImage("unk220", data.unk220);
			WriteImage("unk228", data.unk228);
			WriteImage("unk230", data.unk230);
			WriteImage("unk238", data.unk238);
			WriteImage("unk240", data.unk240);
			WriteImage("unk248", data.unk248);
			WriteImage("unk280", data.unk280);

			/*
			
			
			GfxImage* unk140[6];
			GfxImage* unk170[3];
			GfxImage* unk188[3];
			GfxImage* unk250[3];
			GfxImage* unk268[3];
			*/

			json.WriteFieldNameString("bundle");
			if (!sbv2::WriteBundleData(opt, json, proc, data.bundle, ctx.GetLocalized)) return false;

			json.EndObject();


			std::filesystem::path loc = ctx.outDir / "tables" / "compassdata" / std::format("{}.json", hashutils::ExtractTmp("file", data.name));
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());

			LOG_INFO("Dump {}", loc.string());

			if (!json.WriteToFile(loc)) {
				LOG_ERROR("Can't write {}", loc.string());
			}
			return true;
		}

	};
	utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_COMPASSDATA };
}