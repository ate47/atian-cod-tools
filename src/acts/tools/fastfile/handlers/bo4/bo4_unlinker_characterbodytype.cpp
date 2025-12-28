#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/fastfile/handlers/bo4/bo4_unlinker_scriptbundle.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/utils/data_utils.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	using namespace games::bo4::pool;

	struct CharacterBodyType {
		XHash name;
		XHash displayName;
		XHash description;
		XHash unk30;
		ScriptBundle* mpDialog;
		const char* chrName;
		GfxImage* unk50;
		GfxImage* unk58;
		GfxImage* unk60;
		GfxImage* unk68;
		GfxImage* unk70;
		GfxImage* unk78;
		GfxImage* unk80;
		GfxImage* unk88;
		GfxImage* positionDraftPortrait;
		GfxImage* positionDraftIcon;
		GfxImage* lootUiIcon;
		uint64_t unka8;
		uint64_t unkb0;
		uint64_t unkb8;
		uint64_t unkc0;
		scriptbundle::SB_ObjectsArray bundle;
		PlayerSoundsTable* playerSoundsTable;
		PlayerFXTable* playerFXTable;
		TagFxSet* tagFxSet;
		FootstepTableDef* characterFootsteps;
		FootstepTableDef* characterFootstepsQuiet;
		FootstepTableDef* characterFootstepsNPC;
		FootstepTableDef* characterFootstepsNPCLoud;
		FootstepTableDef* characterFootstepsNPCQuiet;
		XModel* dogtagFriendly;
		XModel* dogTagEnemy;
		uint64_t unk138;
		uint64_t unk140;
		uint64_t unk148;
		uint64_t unk150;
	};


	static_assert(sizeof(CharacterBodyType) == 0x158);

	class ImplWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {

			CharacterBodyType* asset{ (CharacterBodyType*)ptr };
			// refs
			constexpr size_t postWeaponDefNameField = offsetof(CharacterBodyType, name) + sizeof(CharacterBodyType::name);
			constexpr size_t testLen = sizeof(CharacterBodyType) - postWeaponDefNameField;

			if (utils::data::IsNulled(&((byte*)ptr)[postWeaponDefNameField], testLen)) {
				LOG_INFO("ignore empty character {}", hashutils::ExtractTmp("file", asset->name.name));
				return;
			}


			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "characterbodytype"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name.name)) };
			std::filesystem::create_directories(outFile.parent_path());

			BO4JsonWriter json{};

			LOG_INFO("Dump characterbodytype {}", outFile.string());


			json.BeginObject();

			
			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXHash("displayName", asset->displayName);
			json.WriteFieldValueXHash("description", asset->description);
			json.WriteFieldValueXHash("unk30", asset->unk30);
			json.WriteFieldValueXString("chrName", asset->chrName);
			json.WriteFieldValueXAsset("mpDialog", ASSET_TYPE_SCRIPTBUNDLE, asset->mpDialog);
			json.WriteFieldValueXAsset("playerSoundsTable", ASSET_TYPE_PLAYERSOUNDS, asset->playerSoundsTable);
			json.WriteFieldValueXAsset("playerFXTable", ASSET_TYPE_PLAYERFX, asset->playerFXTable);
			
			json.WriteFieldValueXAsset("image_50", ASSET_TYPE_IMAGE, asset->unk50);
			json.WriteFieldValueXAsset("image_58", ASSET_TYPE_IMAGE, asset->unk58);
			json.WriteFieldValueXAsset("image_60", ASSET_TYPE_IMAGE, asset->unk60);
			json.WriteFieldValueXAsset("image_68", ASSET_TYPE_IMAGE, asset->unk68);
			json.WriteFieldValueXAsset("image_70", ASSET_TYPE_IMAGE, asset->unk70);
			json.WriteFieldValueXAsset("image_78", ASSET_TYPE_IMAGE, asset->unk78);
			json.WriteFieldValueXAsset("image_80", ASSET_TYPE_IMAGE, asset->unk80);
			json.WriteFieldValueXAsset("image_88", ASSET_TYPE_IMAGE, asset->unk88);
			json.WriteFieldValueXAsset("positionDraftPortrait", ASSET_TYPE_IMAGE, asset->positionDraftPortrait);
			json.WriteFieldValueXAsset("positionDraftIcon", ASSET_TYPE_IMAGE, asset->positionDraftIcon);
			json.WriteFieldValueXAsset("lootUiIcon", ASSET_TYPE_IMAGE, asset->lootUiIcon);

			json.WriteFieldValueXAsset("dogtagFriendly", ASSET_TYPE_XMODEL, asset->dogtagFriendly);
			json.WriteFieldValueXAsset("dogTagEnemy", ASSET_TYPE_XMODEL, asset->dogTagEnemy);
			json.WriteFieldValueXAsset("tagFxSet", ASSET_TYPE_TAGFX, asset->tagFxSet);
			json.WriteFieldValueXAsset("characterFootsteps", ASSET_TYPE_FOOTSTEP_TABLE, asset->characterFootsteps);
			json.WriteFieldValueXAsset("characterFootstepsQuiet", ASSET_TYPE_FOOTSTEP_TABLE, asset->characterFootstepsQuiet);
			json.WriteFieldValueXAsset("characterFootstepsNPC", ASSET_TYPE_FOOTSTEP_TABLE, asset->characterFootstepsNPC);
			json.WriteFieldValueXAsset("characterFootstepsNPCLoud", ASSET_TYPE_FOOTSTEP_TABLE, asset->characterFootstepsNPCLoud);
			json.WriteFieldValueXAsset("characterFootstepsNPCQuiet", ASSET_TYPE_FOOTSTEP_TABLE, asset->characterFootstepsNPCQuiet);

			bool error{};
			json.WriteFieldNameString("bundle");
			scriptbundle::WriteObject(json, asset->bundle, error);

			if (error) return;

			json.EndObject();

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_CHARACTER_BODY_TYPE };
}