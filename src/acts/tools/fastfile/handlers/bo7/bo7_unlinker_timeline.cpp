#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo6.hpp>
#include <tools/fastfile/handlers/bo7/bo7_unlinker_scriptbundle.hpp>
namespace {
	using namespace fastfile::handlers::bo7;

	struct Timeline {
		XHash64 name;
		GfxImage* unk8;
		GfxImage* unk10;
		GfxImage* unk18;
		GfxImage* unk20;
		GfxImage* unk28;
		GfxImage* unk30;
		float unk38;
		float unk3c;
		float unk40;
		float unk44;
		float unk48;
		float unk4c;
		float unk50;
		float unk54;
		float unk58;
		float unk5c;
		const char* unk60;
		const char* unk68;
		const char* unk70;
		const char* unk78;
		const char* unk80;
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
		scriptbundle::ScriptBundleObjectData bundle;
	};

	static_assert(sizeof(Timeline) == 0xf8);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			Timeline* asset{ (Timeline*)ptr };
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			
			json.WriteFieldValueXAsset("image8", SATH_ASSET_IMAGE, asset->unk8);
			json.WriteFieldValueXAsset("image10", SATH_ASSET_IMAGE, asset->unk10);
			json.WriteFieldValueXAsset("image18", SATH_ASSET_IMAGE, asset->unk18);
			json.WriteFieldValueXAsset("image20", SATH_ASSET_IMAGE, asset->unk20);
			json.WriteFieldValueXAsset("image28", SATH_ASSET_IMAGE, asset->unk28);
			json.WriteFieldValueXAsset("image30", SATH_ASSET_IMAGE, asset->unk30);

			json.WriteFieldValueXString("unk60", asset->unk60);
			json.WriteFieldValueXString("unk68", asset->unk68);
			json.WriteFieldValueXString("unk70", asset->unk70);
			json.WriteFieldValueXString("unk78", asset->unk78);
			json.WriteFieldValueXString("unk80", asset->unk80);

			json.WriteFieldValueNumber("unk38", asset->unk38);
			json.WriteFieldValueNumber("unk3c", asset->unk3c);
			json.WriteFieldValueNumber("unk40", asset->unk40);
			json.WriteFieldValueNumber("unk44", asset->unk44);
			json.WriteFieldValueNumber("unk48", asset->unk48);
			json.WriteFieldValueNumber("unk4c", asset->unk4c);
			json.WriteFieldValueNumber("unk50", asset->unk50);
			json.WriteFieldValueNumber("unk54", asset->unk54);
			json.WriteFieldValueNumber("unk58", asset->unk58);
			json.WriteFieldValueNumber("unk5c", asset->unk5c);


			json.WriteFieldValueUnknown("unk88", asset->unk88);
			json.WriteFieldValueUnknown("unk90", asset->unk90);
			json.WriteFieldValueUnknown("unk98", asset->unk98);
			json.WriteFieldValueUnknown("unka0", asset->unka0);
			json.WriteFieldValueUnknown("unka8", asset->unka8);
			json.WriteFieldValueUnknown("unkb0", asset->unkb0);
			json.WriteFieldValueUnknown("unkb8", asset->unkb8);
			json.WriteFieldValueUnknown("unkc0", asset->unkc0);
			json.WriteFieldValueUnknown("unkc8", asset->unkc8);
			json.WriteFieldValueUnknown("unkd0", asset->unkd0);
			

			json.WriteFieldNameString("bundle");
			scriptbundle::WriteData(json, asset->bundle);
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "timeline"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump timeline {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, HandlerHashedAssetType, Worker> impl{ GetWorkers(), HandlerHashedAssetType::SATH_ASSET_TIMELINE, sizeof(Timeline) };
}