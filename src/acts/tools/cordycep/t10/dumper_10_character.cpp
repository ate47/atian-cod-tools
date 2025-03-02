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
		Dismemberment* unk30;
		AIAnimSettings* unk38;
		ScrString unk40;
		ScrString unk44;
		ScrString unk48;
		ScrString unk4c;
		ScrString unk50;
		ScrString unk54;
		ScrString unk58;
		ScrString unk5c;
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


			std::filesystem::path loc{ ctx.outDir / "tables" / "character" / std::format("{}.json", hashutils::ExtractTmp("file", data.name)) };
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());
			utils::raw_file_extractor::JsonWriter json{};

			LOG_INFO("Dump {}", loc.string());

			// Dump pool
			json.BeginObject();

			json.WriteFieldNameString("name");
			json.WriteValueHash(data.name);
			json.WriteFieldNameString("unk40"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.unk40)));
			json.WriteFieldNameString("unk44"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.unk44)));
			json.WriteFieldNameString("unk48"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.unk48)));
			json.WriteFieldNameString("unk4c"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.unk4c)));
			json.WriteFieldNameString("unk50"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.unk50)));
			json.WriteFieldNameString("unk54"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.unk54)));
			json.WriteFieldNameString("unk58"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.unk58)));
			json.WriteFieldNameString("unk5c"); json.WriteValueString(opt.AddString(ctx.GetScrString(data.unk5c)));
			json.WriteFieldNameString("unk68");
			json.BeginArray();
			if (data.unk68_count) {
				auto unk68{ proc.ReadMemoryArrayEx<uintptr_t>(reinterpret_cast<uintptr_t>(data.unk68), data.unk68_count) };
				for (size_t i = 0; i < data.unk68_count; i++) {
					json.WriteValueString(opt.AddString(proc.ReadStringTmp(unk68[i])));
				}
			}
			json.EndArray();
			json.EndObject();


			if (!json.WriteToFile(loc)) {
				LOG_ERROR("Can't write {}", loc.string());
			}
			return true;
		}

	};

	//utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_CHARACTER };
}