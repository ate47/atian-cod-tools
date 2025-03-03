#include <includes.hpp>
#include <tools/pool.hpp>
#include <tools/cordycep/t10/dumper_t10.hpp>
#include <tools/utils/raw_file_extractor.hpp>

namespace {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t10;
	using namespace bo6;

	struct Character {
		uint64_t name;
		uint64_t unk08;
		uint64_t unk10;
		uint64_t unk18;
		uint64_t unk20;
		uint64_t unk28;
		Dismemberment* dismemberment;
		AIAnimSettings* aiAnimSettings;
		ScrString bodyScr;
		ScrString unk44;
		ScrString headScr;
		ScrString unk4c;
		ScrString hatScr;
		ScrString unk54;
		ScrString classScr;
		ScrString animtreeScr;
		int32_t unk68_count;
		const char** unk68;
		uint64_t unk70;
		uint64_t unk78;
	};


	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			Character data;

			if (!proc.ReadMemory(&data, asset.Header, sizeof(data))) {
				LOG_ERROR("Can't read {} header ", hashutils::ExtractTmp("hash", asset.ID));
				return false;
			}
			utils::raw_file_extractor::JsonWriter json{};

			// Dump pool
			json.BeginObject();

			json.WriteFieldNameString("name"); json.WriteValueHash(data.name);
			if (data.dismemberment) {
				json.WriteFieldNameString("dismemberment"); json.WriteValueHash(proc.ReadMemory<uint64_t>(reinterpret_cast<uintptr_t>(data.dismemberment)));
			}
			if (data.aiAnimSettings) {
				json.WriteFieldNameString("aiAnimSettings"); json.WriteValueHash(proc.ReadMemory<uint64_t>(reinterpret_cast<uintptr_t>(data.aiAnimSettings)));
			}
			
			json.WriteFieldNameString("hash08"); json.WriteValueHash(data.unk08);
			json.WriteFieldNameString("hash10"); json.WriteValueHash(data.unk10);
			json.WriteFieldNameString("hash18"); json.WriteValueHash(data.unk18);
			json.WriteFieldNameString("hash20"); json.WriteValueHash(data.unk20);
			json.WriteFieldNameString("hash28"); json.WriteValueHash(data.unk28);

			if (data.bodyScr) { json.WriteFieldNameString("body"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.bodyScr))); }
			if (data.unk44) { json.WriteFieldNameString("unk44"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.unk44))); }
			if (data.headScr) { json.WriteFieldNameString("head"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.headScr))); }
			if (data.unk4c) { json.WriteFieldNameString("unk4c"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.unk4c))); }
			if (data.hatScr) { json.WriteFieldNameString("hat"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.hatScr))); }
			if (data.unk54) { json.WriteFieldNameString("unk54"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.unk54))); }
			if (data.classScr) { json.WriteFieldNameString("class"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.classScr))); }
			if (data.animtreeScr) { json.WriteFieldNameString("animtree"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.animtreeScr))); }
			if (data.unk68_count) {
				json.WriteFieldNameString("unk68");
				json.BeginArray();
				auto unk68{ proc.ReadMemoryArrayEx<uintptr_t>(reinterpret_cast<uintptr_t>(data.unk68), data.unk68_count) };
				for (size_t i = 0; i < data.unk68_count; i++) {
					json.WriteValueString(opt.AddString(proc.ReadStringTmp(unk68[i])));
				}
				json.EndArray();
			}
			json.EndObject();


			std::filesystem::path loc{ ctx.outDir / "tables" / "character" / std::format("{}.json", hashutils::ExtractTmp("file", data.name)) };

			LOG_INFO("Dump {}", loc.string());
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());

			if (!json.WriteToFile(loc)) {
				LOG_ERROR("Can't write {}", loc.string());
			}
			return true;
		}

	};

	utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_CHARACTER };
}