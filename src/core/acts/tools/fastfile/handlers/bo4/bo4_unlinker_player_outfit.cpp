#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/fastfile/handlers/bo4/bo4_unlinker_scriptbundle.hpp>
#include <core/hashes/raw_file_extractor.hpp>
#include <tools/utils/data_utils.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	using namespace games::bo4::pool;
	struct Ragdoll;

	// see: 6969A0     ; void __fastcall Scr_function_d7c3cf6c(scriptInstance_t inst)

	struct PlayerOutfitUnk68 {
		XHash desc;
		bool unk10;
		XHash unk18;
		GfxImage* image;
		ScriptBundle* scriptbundle;
		XModel* model;
		uint32_t unk48_count;
		Material** unk48;
		uint64_t unk50;
	};
	static_assert(sizeof(PlayerOutfitUnk68) == 0x58);

	struct PlayerOutfitUnk78 {
		XHash desc;
		bool unk10;
		XHash unk18;
		GfxImage* image;
		XModel* model;
		uint32_t unk40_count;
		Material** unk40;
		uint32_t unk50_count;
		Material** unk50;
		uint64_t unk58;
	};
	static_assert(sizeof(PlayerOutfitUnk78) == 0x60);


	struct PlayerOutfit {
		XHash name;
		uint64_t unk10;
		XHash desc;
		GfxImage* unk28;
		uint64_t unk30;
		PlayerFXTable* playerFXTable;
		TagFxSet* tagFX;
		Ragdoll* ragdoll;
		FxEffectDef* fxEffect;
		ScrString_t tag;
		uint32_t unk5c;
		uint32_t unk68_count;
		uint32_t unk64;
		PlayerOutfitUnk68* unk68;
		uint32_t unk78_count;
		uint32_t unk74;
		PlayerOutfitUnk78* unk78;
		uint32_t unk88_count;
		uint32_t unk84;
		void* unk88;
		uint32_t unk98_count;
		uint32_t unk94;
		void* unk98;
		uint32_t unka8_count;
		uint32_t unka4;
		void* unka8;
		uint32_t unkb8_count;
		uint32_t unkb4;
		void* unkb8;
		uint32_t unkc8_count;
		uint32_t unkc4;
		void* unkc8;
		uint32_t unkd8_count;
		uint32_t unkd4;
		void* unkd8;
		uint32_t unke8_count;
		uint32_t unke4;
		void* unke8;
		uint32_t unkf8_count;
		uint32_t unkf4;
		void* unkf8;
	};
	static_assert(sizeof(PlayerOutfit) == 0x100);

	class ImplWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {

			PlayerOutfit& asset{ *(PlayerOutfit*)ptr };


			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "player" / "outfit"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset.name.name)) };
			std::filesystem::create_directories(outFile.parent_path());

			BO4JsonWriter json{};

			LOG_OPT_INFO("Dump playeroutfit {}", outFile.string());


			json.BeginObject();

			json.WriteFieldValueXHash("name", asset.name);
			json.WriteFieldValueXHash("desc", asset.desc);
			json.WriteFieldValueXAsset("unk28", ASSET_TYPE_IMAGE, asset.unk28);
			json.WriteFieldValueScrString("tag", asset.tag);
			json.WriteFieldValueXAsset("playerFXTable", ASSET_TYPE_PLAYERFX, asset.playerFXTable);
			json.WriteFieldValueXAsset("tagFX", ASSET_TYPE_TAGFX, asset.tagFX);
			json.WriteFieldValueXAsset("fxEffect", ASSET_TYPE_FX, asset.fxEffect);
			json.WriteFieldValueXAsset("ragdoll", ASSET_TYPE_RAGDOLL, asset.ragdoll);
			json.WriteFieldValueUnknown("unk10", asset.unk10);
			json.WriteFieldValueUnknown("unk30", asset.unk30);

			if (asset.unk68) {
				json.WriteFieldNameString("unk68");
				json.BeginArray();
				for (size_t i = 0; i < asset.unk68_count; i++) {
					PlayerOutfitUnk68& obj{ asset.unk68[i] };
					json.BeginObject();
					json.WriteFieldValueXHash("desc", obj.desc);
					json.WriteFieldValueXHash("unk68:unk18", obj.unk18);
					json.WriteFieldValueBool("unk68:unk10", obj.unk10);
					json.WriteFieldValueXAsset("image", ASSET_TYPE_IMAGE, obj.image);
					json.WriteFieldValueXAsset("scriptbundle", ASSET_TYPE_SCRIPTBUNDLE, obj.scriptbundle);
					json.WriteFieldValueXAsset("model", ASSET_TYPE_XMODEL, obj.model);
					json.WriteFieldValueXAssetArray("unk68:unk48", ASSET_TYPE_MATERIAL, obj.unk48_count, obj.unk48);
					json.WriteFieldValueNumber("unk68:unk50", obj.unk50);
					json.EndObject();
				}
				json.EndArray();
			}

			if (asset.unk78) {
				json.WriteFieldNameString("unk78");
				json.BeginArray();
				for (size_t i = 0; i < asset.unk78_count; i++) {
					PlayerOutfitUnk78& obj{ asset.unk78[i] };
					json.BeginObject();
					json.WriteFieldValueXHash("desc", obj.desc);
					json.WriteFieldValueXHash("unk78:unk18", obj.unk18);
					json.WriteFieldValueBool("unk78:unk10", obj.unk10);
					json.WriteFieldValueXAsset("image", ASSET_TYPE_IMAGE, obj.image);
					json.WriteFieldValueXAsset("model", ASSET_TYPE_XMODEL, obj.model);
					json.WriteFieldValueXAssetArray("unk78:unk48", ASSET_TYPE_MATERIAL, obj.unk40_count, obj.unk40);
					json.WriteFieldValueXAssetArray("unk78:unk48", ASSET_TYPE_MATERIAL, obj.unk50_count, obj.unk50);
					json.WriteFieldValueUnknown("unk78:unk58", obj.unk58);
					json.EndObject();
				}
				json.EndArray();
			}
			json.EndObject();

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, XAssetType, Worker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_PLAYER_OUTFIT };
}