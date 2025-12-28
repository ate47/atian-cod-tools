#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo6_sp.hpp>
#include <tools/fastfile/handlers/bo6sp/bo6sp_unlinker_scriptbundle.hpp>


namespace {
	using namespace fastfile::handlers::bo6sp;

	struct AITypeCharacters {
		Character** characters;
		float* odds;
		XHash32 unk10;
		int32_t charactersCount;
		int32_t unk18;
		int32_t unk1c;
	};
	static_assert(sizeof(AITypeCharacters) == 0x20);

	struct AITypeCharactersList {
		Character** characters;
		uint32_t* unk8;
		XHash32 characterListName;
		int32_t count;
		uint64_t unk18;
	};
	static_assert(sizeof(AITypeCharactersList) == 0x20);

	struct AITypeWeapon {
		XHash64 name;
		ScrString_t* strings;
		ScrString_t weapon;
		int32_t count;
		uint32_t ammo;
		uint32_t unk1c;
	};
	static_assert(sizeof(AITypeWeapon) == 0x20);



	struct AIType {
		XHash64 name;
		XHash64 coverSelectorAsset;
		uint64_t unk10;
		XHash64 enemySelectorAsset;
		uint64_t unk20;
		XHash64 animationStateMachineAsset;
		XHash64 behaviorTreeAsset;
		FootStepsFxTable* footstepsFxTable;
		FoliagesFXTable* foliagesFxTable;
		HandPlantsFXTable* handplantsFxTable;
		AiImpactVFXTable* aiImpactVFXTable;
		Dismemberment* dismemberment;
		const char* typeSuffix;
		WeaponAccuracy* weaponAccuracy;
		AITypeCharactersList* charactersLists;
		XHash64* scriptFiles;
		AITypeWeapon* weapons;
		ScriptBundle* bundle;
		scriptbundle::ScriptBundleObjectData bundleData;
		AITypeCharacters characterTypes;
		ScrString_t species;
		uint32_t unkd4;
		uint64_t unkd8;
		uint64_t unke0;
		uint64_t unke8;
		uint64_t unkf0;
		uint32_t unkf8;
		int32_t charactersListsCount;
		int32_t scriptFilesCount;
		int32_t weaponsCount;
		uint64_t unk108;
		uint64_t unk110;
	};
	; static_assert(sizeof(AIType) == 0x118);
	class ImplWorker : public Worker {
		using Worker::Worker;
		using ScriptBundle = scriptbundle::ScriptBundle;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			AIType& asset{ *(AIType*)ptr };
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset.name);
			json.WriteFieldValueXString("typeSuffix", asset.typeSuffix);
			json.WriteFieldValueScrString("species", asset.species);
			json.WriteFieldValueXHash("animationStateMachineAsset", asset.animationStateMachineAsset);
			json.WriteFieldValueXHash("coverSelectorAsset", asset.coverSelectorAsset);
			json.WriteFieldValueXHash("behaviorTreeAsset", asset.behaviorTreeAsset);
			json.WriteFieldValueXHash("enemySelectorAsset", asset.enemySelectorAsset);

			json.WriteFieldValueXAsset("footStepsFxTable", T10HashAssetType::T10H_ASSET_FOOTSTEPSFXTABLE, asset.footstepsFxTable);
			json.WriteFieldValueXAsset("foliagesFXTable", T10HashAssetType::T10H_ASSET_FOLIAGESFXTABLE, asset.foliagesFxTable);
			json.WriteFieldValueXAsset("handPlantsFXTable", T10HashAssetType::T10H_ASSET_HANDPLANTSFXTABLE, asset.handplantsFxTable);
			json.WriteFieldValueXAsset("aiImpactVFXTable", T10HashAssetType::T10H_ASSET_AIIMPACTVFXTABLE, asset.aiImpactVFXTable);
			json.WriteFieldValueXAsset("dismemberment", T10HashAssetType::T10H_ASSET_DISMEMBERMENT, asset.dismemberment);
			json.WriteFieldValueXAsset("weaponAccuracy", T10HashAssetType::T10H_ASSET_WEAPONACCURACY, asset.weaponAccuracy);
			json.WriteFieldValueXAsset("scriptbundle", T10HashAssetType::T10H_ASSET_SCRIPTBUNDLE, asset.bundle);
			json.WriteFieldValueXHash("characters.unk10", asset.characterTypes.unk10);
			json.WriteFieldValueUnknown("characterTypes:unk18", asset.characterTypes.unk18);
			json.WriteFieldValueUnknown("characterTypes:unk1c", asset.characterTypes.unk1c);
			if (asset.characterTypes.charactersCount) {
				json.WriteFieldNameString("characterTypes");
				json.BeginArray();
				bool showOdd{};
				if (asset.characterTypes.odds) {
					for (size_t i = 0; i < asset.characterTypes.charactersCount; i++) {
						if (asset.characterTypes.odds[i] != 1) showOdd = true;
					}
				}

				for (size_t i = 0; i < asset.characterTypes.charactersCount; i++) {
					json.BeginObject();
					if (asset.characterTypes.characters) json.WriteFieldValueXAsset("character", T10HashAssetType::T10H_ASSET_CHARACTER, asset.characterTypes.characters[i]);
					if (showOdd) json.WriteFieldValueNumber("odd", asset.characterTypes.odds[i]);
					json.EndObject();
				}
				json.EndArray();
			}
			if (asset.weapons) {
				json.WriteFieldNameString("weapons");
				json.BeginArray();
				for (size_t i = 0; i < asset.weaponsCount; i++) {
					AITypeWeapon* str{ &asset.weapons[i] };

					json.BeginObject();
					json.WriteFieldValueXHash("name", str->name);
					json.WriteFieldValueNumber("ammo", str->ammo);
					json.WriteFieldValueUnknown("weapons:unk1c", str->unk1c); // empty?

					json.WriteFieldValueScrString("weapon", str->weapon);
					if (str->count) {
						json.WriteFieldNameString("strings");
						json.BeginArray();
						for (size_t i = 0; i < str->count; i++) {
							json.WriteValueScrString(str->strings[i]);
						}
						json.EndArray();
					}

					json.EndObject();
				}
				json.EndArray();
			}
			if (asset.scriptFiles) {
				json.WriteFieldNameString("scriptFiles");
				json.BeginArray();
				for (size_t i = 0; i < asset.scriptFilesCount; i++) {
					json.WriteValueHash(asset.scriptFiles[i]);
				}
				json.EndArray();
			}
			if (asset.charactersLists) {
				json.WriteFieldNameString("charactersLists");
				json.BeginArray();
				for (size_t i = 0; i < asset.charactersListsCount; i++) {
					AITypeCharactersList* list{ &asset.charactersLists[i] };

					json.BeginObject();
					json.WriteFieldValueXHash("characterListName", list->characterListName);
					json.WriteFieldValueXAssetArray("characters", bo6::T10H_ASSET_CHARACTER, list->count, list->characters);
					json.WriteFieldValueUnknown("list:unk18", list->unk18);
					json.EndObject();
				}
				json.EndArray();
			}

			json.WriteFieldNameString("bundleData");
			if (!scriptbundle::WriteData(json, asset.bundleData)) {
				return;
			}

			if (opt.testDump) {

				// unused? maybe to store the assets
				json.WriteFieldValueUnknown("unk10", asset.unk10);
				json.WriteFieldValueUnknown("unk20", asset.unk20);


				json.WriteFieldValueUnknown("unkd4", asset.unkd4);
				json.WriteFieldValueUnknown("unkd8", asset.unkd8);
				json.WriteFieldValueUnknown("unke0", asset.unke0);
				json.WriteFieldValueUnknown("unke8", asset.unke8);
				json.WriteFieldValueUnknown("unkf0", asset.unkf0);
				json.WriteFieldValueUnknown("unkf8", asset.unkf8);
				json.WriteFieldValueUnknown("unk108", asset.unk108);
				json.WriteFieldValueUnknown("unk110", asset.unk110);
			}


			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / "bo6sp" / "source" / "tables" / "ai" / "type"
				/ fastfile::GetCurrentContext().ffname
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset.name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump aitype {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> impl{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_AITYPE, sizeof(AIType) };
}