#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo7.hpp>
#include <tools/ff/handlers/bo7/bo7_unlinker_scriptbundle.hpp>


namespace {
	using namespace fastfile::handlers::bo7;

	struct AiShootStyles {
		XHash64 name;
		ScriptBundle** bundles;
		int32_t count;
		uint32_t unk14;
	};
	struct AiTokenTypes {
		XHash64 name;
		ScriptBundle** bundles;
		int32_t count;
		uint32_t unk14;
	};
	struct AiTokenStealingDefinitions {
		XHash64 name;
		ScriptBundle** bundles;
		int32_t count;
		uint32_t unk14;
	};
	struct AiTokenDefinitions {
		XHash64 name;
		ScriptBundle** bundles;
		int32_t count;
		uint32_t unk14;
	};


	struct AITypeCharacters {
		Character** characters;
		float* odds;
		XHash32 unk10;
		int32_t charactersCount;
		int32_t unk18;
		int32_t unk1c;
	};
	static_assert(sizeof(AITypeCharacters) == 0x20);

	struct AITypeVoiceList {
		Character** characters;
		uint32_t* unk8;
		XHash32 characterListName;
		int32_t count;
		uint64_t unk18;
	};
	static_assert(sizeof(AITypeVoiceList) == 0x20);

	const char* AITypeWeaponTypeNames[]{
		"primary",
		"secondary",
		"sidearm",
		""
	};

	enum AITypeWeaponType : byte {
		AITWT_PRIMARY = 0,
		AITWT_SECONDARY,
		AITWT_SIDEARM,
		AITWT_COUNT,
	};

	const char* AITypeWeaponTypeName(AITypeWeaponType type) {
		return type >= ARRAYSIZE(AITypeWeaponTypeNames) ? utils::va("unit_%u", type) : AITypeWeaponTypeNames[type];
	}

	enum AITypeUnitType : uint32_t {
		AITUT_SOLDIER = 0,
		AITUT_CIVILIAN,
		AITUT_DOG,
		AITUT_ZOMBIE,
		AITUT_JUGGERNAUT,
		AITUT_SUICIDEBOMBER,
		AITUT_COUNT,
	};
	const char* AITypeUnitTypeNames[]{ 
		"soldier",
		"civilian",
		"dog",
		"zombie",
		"juggernaut",
		"suicidebomber",
	};

	const char* AITypeUnitTypeName(AITypeUnitType type) {
		return type >= ARRAYSIZE(AITypeUnitTypeNames) ? utils::va("unit_%u", type) : AITypeUnitTypeNames[type];
	}

	const char* AITypeTeamNames[]{
		"free",
		"axis",
		"allies",
	};

	enum AITypeTeam : byte {
		AITEAM_FREE = 0,
		AITEAM_AXIS,
		AITEAM_ALLIES,
		// after it depends on the mode
	};

	const char* AITypeTeamName(AITypeTeam team) {
		return team >= ARRAYSIZE(AITypeTeamNames) ? utils::va("team_%u", team) : AITypeTeamNames[team];
	}

	struct AITypeWeapon {
		XHash64 name;
		ScrString_t* strings;
		ScrString_t weapon;
		int32_t count;
		AITypeWeaponType type;
	};
	static_assert(sizeof(AITypeWeapon) == 0x20);

	struct AIType {
		XHash64 name;
		XHash64 coverSelectorAsset;
		XHash64 unk_d1e3f54c868c90eb;
		XHash64 unk18;
		XHash64 enemySelectorAsset;
		XHash64 unk28;
		XHash64 animationStateMachineAsset;
		XHash64 behaviorTreeAsset;
		FootStepSFXTable* footStepSFxTable;
		FoliageSFXTable* foliageSFXTable;
		HandPlantSFXTable* handPlantSFXTable;
		AIImpactVFXTable* aiImpactVFXTable;
		ParticleSystem* particleSystem1;
		ParticleSystem* particleSystem2;
		Dismemberment* dismemberment;
		const char* typeSuffix;
		WeaponAccuracy* weaponAccuracy;
		AITypeVoiceList* voiceList;
		XHash64* scriptFiles;
		AITypeWeapon* weapons;
		ScriptBundle* bundle;
		CalloutMarkerPing* calloutMarkerPing;
		scriptbundle::ScriptBundleObjectData bundleData;
		AITypeCharacters characterTypes;
		uint64_t unkf0;
		uint64_t unkf8;
		ScrString_t species;
		AITypeUnitType unittype;
		uint64_t unk108;
		uint64_t unk110;
		uint64_t unk118;
		uint64_t unk120;
		uint64_t unk128;
		uint64_t unk130;
		int32_t voiceListCount;
		int32_t scriptFilesCount;
		int32_t weaponsCount;
		AITypeTeam team;
		byte unk145;
		byte unk146;
		byte unk147;
		uint64_t unk148;
		uint64_t unk150;
		uint64_t unk158;
		GfxImage* unk160;
		GfxImage* unk168;
	}; static_assert(sizeof(AIType) == 0x170);




	template<typename Asset>
	class ImplWorkerBundleList : public Worker {
		static_assert(sizeof(Asset) == 0x18 && "Bad bundlelist asset size");

		const char* type;
		const char* dir;
	public:
		ImplWorkerBundleList(const char* type, const char* dir) : Worker(sizeof(Asset)), type(type), dir(dir) {};

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			Asset& asset{ *(Asset*)ptr };
			if (!asset.count) {
				LOG_INFO("ignore empty asset {}", hashutils::ExtractTmp("hash", asset.name));
				return;
			}
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset.name);
			json.WriteFieldValueXAssetArray("bundles", SatHashAssetType::SATH_ASSET_SCRIPTBUNDLE, asset.count, asset.bundles);
			if (asset.unk14) json.WriteFieldValueNumber("unk14", asset.unk14);

			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / dir
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset.name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump {} {}", outFile.string(), type);

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

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
			json.WriteFieldValueXHash("coverSelectorAsset", asset.coverSelectorAsset);
			json.WriteFieldValueXHash(0xd1e3f54c868c90eb, asset.unk_d1e3f54c868c90eb);
			json.WriteFieldValueXHash("unk18", asset.unk18);
			json.WriteFieldValueXHash("enemySelectorAsset", asset.enemySelectorAsset);
			json.WriteFieldValueXHash("unk28", asset.unk28);
			json.WriteFieldValueXHash("animationStateMachineAsset", asset.animationStateMachineAsset);
			json.WriteFieldValueXHash("behaviorTreeAsset", asset.behaviorTreeAsset);

			json.WriteFieldValueXAsset("footStepsFxTable", SatHashAssetType::SATH_ASSET_FOOTSTEPSFXTABLE, asset.footStepSFxTable);
			json.WriteFieldValueXAsset("foliagesFXTable", SatHashAssetType::SATH_ASSET_FOLIAGESFXTABLE, asset.foliageSFXTable);
			json.WriteFieldValueXAsset("handPlantsFXTable", SatHashAssetType::SATH_ASSET_HANDPLANTSFXTABLE, asset.handPlantSFXTable);
			json.WriteFieldValueXAsset("aiImpactVFXTable", SatHashAssetType::SATH_ASSET_AIIMPACTVFXTABLE, asset.aiImpactVFXTable);
			json.WriteFieldValueXAsset("dismemberment", SatHashAssetType::SATH_ASSET_DISMEMBERMENT, asset.dismemberment);
			json.WriteFieldValueXAsset("weaponAccuracy", SatHashAssetType::SATH_ASSET_WEAPONACCURACY, asset.weaponAccuracy);
			json.WriteFieldValueXAsset("scriptbundle", SatHashAssetType::SATH_ASSET_SCRIPTBUNDLE, asset.bundle);
			json.WriteFieldValueXAsset("particleSystem1", SatHashAssetType::SATH_ASSET_PARTICLESYSTEM, asset.particleSystem1);
			json.WriteFieldValueXAsset("particleSystem2", SatHashAssetType::SATH_ASSET_PARTICLESYSTEM, asset.particleSystem2);
			json.WriteFieldValueXAsset("calloutMarkerPing", SatHashAssetType::SATH_ASSET_CALLOUTMARKERPING, asset.calloutMarkerPing);
			json.WriteFieldValueXAsset("image160", SatHashAssetType::SATH_ASSET_IMAGE, asset.unk160);
			json.WriteFieldValueXAsset("unk168", SatHashAssetType::SATH_ASSET_IMAGE, asset.unk168);
			json.WriteFieldValueString("unitType", AITypeUnitTypeName(asset.unittype));
			json.WriteFieldValueString("team", AITypeTeamName(asset.team));
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
					if (asset.characterTypes.characters) json.WriteFieldValueXAsset("character", SatHashAssetType::SATH_ASSET_CHARACTER, asset.characterTypes.characters[i]);
					if (showOdd) json.WriteFieldValueNumber("odd", asset.characterTypes.odds[i]);
					json.EndObject();
				}
				json.EndArray();
			}
			if (asset.weapons) {
				json.WriteFieldNameString("weaponlist");
				json.BeginArray();
				for (size_t i = 0; i < asset.weaponsCount; i++) {
					AITypeWeapon* str{ &asset.weapons[i] };

					json.BeginObject();
					json.WriteFieldValueXHash("weaponhash", str->name);
					json.WriteFieldValueString("weapontype", AITypeWeaponTypeName(str->type));

					json.WriteFieldValueScrString("weapon", str->weapon);
					if (str->count) {
						json.WriteFieldNameString("attachments");
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
			if (asset.voiceList) {
				json.WriteFieldNameString("voiceList");
				json.BeginArray();
				for (size_t i = 0; i < asset.voiceListCount; i++) {
					AITypeVoiceList* list{ &asset.voiceList[i] };

					json.BeginObject();
					json.WriteFieldValueXHash("characterListName", list->characterListName);
					json.WriteFieldValueXAssetArray("characters", SATH_ASSET_CHARACTER, list->count, list->characters);
					json.WriteFieldValueUnknown("list:unk18", list->unk18);
					json.EndObject();
				}
				json.EndArray();
			}

			json.WriteFieldNameString("bundleData");
			scriptbundle::WriteData(json, asset.bundleData);

			if (opt.testDump) {

				json.WriteFieldValueUnknown("unkf0", asset.unkf0);
				json.WriteFieldValueUnknown("unkf8", asset.unkf8);
				json.WriteFieldValueUnknown("unk108", asset.unk108);
				json.WriteFieldValueUnknown("unk110", asset.unk110);
				json.WriteFieldValueUnknown("unk118", asset.unk118);
				json.WriteFieldValueUnknown("unk120", asset.unk120);
				json.WriteFieldValueUnknown("unk128", asset.unk128);
				json.WriteFieldValueUnknown("unk130", asset.unk130);
				json.WriteFieldValueUnknown("unk148", asset.unk148);
				json.WriteFieldValueUnknown("unk150", asset.unk150);
				json.WriteFieldValueUnknown("unk158", asset.unk158);
			}


			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "ai" / "type"
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
	utils::MapAdder<ImplWorkerBundleList<AiShootStyles>, SatHashAssetType, Worker> implAiShootStyles{
		GetWorkers(), SatHashAssetType::SATH_ASSET_AISHOOTSTYLESLIST, "aishootstyleslist", "ai/shootstyleslist"
	};
	utils::MapAdder<ImplWorkerBundleList<AiTokenTypes>, SatHashAssetType, Worker> implAiTokenTypes{
		GetWorkers(), SatHashAssetType::SATH_ASSET_AI_TOKEN_TYPES, "ai_token_types", "ai/token/types"
	};
	utils::MapAdder<ImplWorkerBundleList<AiTokenStealingDefinitions>, SatHashAssetType, Worker> implAiTokenStealingDefinitions{
		GetWorkers(), SatHashAssetType::SATH_ASSET_AI_TOKEN_STEALING_DEFINITIONS, "ai_token_stealing_definitions", "ai/token/stealingdefinitions"
	};
	utils::MapAdder<ImplWorkerBundleList<AiTokenDefinitions>, SatHashAssetType, Worker> implAiTokenDefinitions{
		GetWorkers(), SatHashAssetType::SATH_ASSET_AI_TOKEN_DEFINITIONS, "ai_token_definitions", "ai/token/definitions"
	};

	utils::MapAdder<ImplWorker, SatHashAssetType, Worker> impl{ GetWorkers(), SatHashAssetType::SATH_ASSET_AITYPE, sizeof(AIType) };
}