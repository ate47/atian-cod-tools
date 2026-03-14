#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo6.hpp>
#include <tools/fastfile/handlers/bo6/bo6_unlinker_scriptbundle.hpp>

namespace {
	using namespace fastfile::handlers::bo6;
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
				LOG_OPT_INFO("ignore empty asset {}", hashutils::ExtractTmp("hash", asset.name));
				return;
			}
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset.name);
			json.WriteFieldValueXAssetArray("bundles", T10HashAssetType::T10H_ASSET_SCRIPTBUNDLE, asset.count, asset.bundles);
			if (asset.unk14) json.WriteFieldValueNumber("unk14", asset.unk14);

			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / dir
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset.name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump {} {}", outFile.string(), type);

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};



	struct AITypeCharacters {
		Character** characters;
		float* odds;
		XHash32 unk10;
		int32_t charactersCount;
		int32_t unk18;
		int32_t unk1c;
	};

	struct AIType {
		XHash64 name;
		XHash64 unk08;
		uint64_t unk10;
		XHash64 unk18;
		uint64_t unk20;
		XHash64 unk28;
		XHash64 unk30;
		FootStepsFxTable* footStepsFxTable;
		FoliagesFXTable* foliagesFXTable;
		HandPlantsFXTable* handPlantsFXTable;
		AiImpactVFXTable* aiImpactVFXTable;
		ParticleSystem* particleSystem1;
		ParticleSystem* particleSystem2;
		Dismemberment* dismemberment;
		const char* typeSuffix;
		WeaponAccuracy* weaponAccuracy;
		uint64_t unk80;
		uint64_t unk88;
		uint64_t unk90;
		ScriptBundle* bundle;
		CalloutMarkerPing* calloutMarkerPing;
		uint64_t unka8;
		uint64_t unkb0;
		uint64_t unkb8;
		uint64_t unkc0;
		AITypeCharacters characters;
		uint64_t unke8;
		uint64_t unkf0;
		ScrString_t species;
		uint64_t unk100;
		uint64_t unk108;
		uint64_t unk110;
		uint64_t unk118;
		uint64_t unk120;
		uint64_t unk128;
		uint64_t unk130;
		uint64_t unk138;
		uint64_t unk140;
		GfxImage* unk148;
		GfxImage* unk150;
	};
	; static_assert(sizeof(AIType) == 0x158);
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
			
			json.WriteFieldValueXAsset("footStepsFxTable", T10HashAssetType::T10H_ASSET_FOOTSTEPSFXTABLE, asset.footStepsFxTable);
			json.WriteFieldValueXAsset("foliagesFXTable", T10HashAssetType::T10H_ASSET_FOLIAGESFXTABLE, asset.foliagesFXTable);
			json.WriteFieldValueXAsset("handPlantsFXTable", T10HashAssetType::T10H_ASSET_HANDPLANTSFXTABLE, asset.handPlantsFXTable);
			json.WriteFieldValueXAsset("aiImpactVFXTable", T10HashAssetType::T10H_ASSET_AIIMPACTVFXTABLE, asset.aiImpactVFXTable);
			json.WriteFieldValueXAsset("particleSystem1", T10HashAssetType::T10H_ASSET_PARTICLESYSTEM, asset.particleSystem1);
			json.WriteFieldValueXAsset("particleSystem2", T10HashAssetType::T10H_ASSET_PARTICLESYSTEM, asset.particleSystem2);
			json.WriteFieldValueXAsset("dismemberment", T10HashAssetType::T10H_ASSET_DISMEMBERMENT, asset.dismemberment);
			json.WriteFieldValueXAsset("weaponAccuracy", T10HashAssetType::T10H_ASSET_WEAPONACCURACY, asset.weaponAccuracy);
			json.WriteFieldValueXAsset("calloutMarkerPing", T10HashAssetType::T10H_ASSET_CALLOUTMARKERPING, asset.calloutMarkerPing);
			json.WriteFieldValueXAsset("unk148", T10HashAssetType::T10H_ASSET_IMAGE, asset.unk148);
			json.WriteFieldValueXAsset("unk150", T10HashAssetType::T10H_ASSET_IMAGE, asset.unk150);
			json.WriteFieldValueXAsset("bundle", T10HashAssetType::T10H_ASSET_SCRIPTBUNDLE, asset.bundle);
			json.WriteFieldValueXHash("characters.unk10", asset.characters.unk10);
			if (asset.characters.unk18) json.WriteFieldValueNumber("characters.unk18", asset.characters.unk18);
			if (asset.characters.unk1c) json.WriteFieldValueNumber("characters.unk1c", asset.characters.unk1c);
			json.WriteFieldNameString("characters");
			json.BeginArray();
			for (size_t i = 0; i < asset.characters.charactersCount; i++) {
				json.BeginObject();
				if (asset.characters.characters) json.WriteFieldValueXAsset("character", T10HashAssetType::T10H_ASSET_CHARACTER, asset.characters.characters[i]);
				if (asset.characters.odds) json.WriteFieldValueNumber("odd", asset.characters.odds[i]);
				json.EndObject();
			}
			json.EndArray();

			if (opt.testDump) {

				json.WriteFieldValueXHash("unk08", asset.unk08);
				json.WriteFieldValueXHash("unk18", asset.unk18);
				json.WriteFieldValueXHash("unk28", asset.unk28);
				json.WriteFieldValueXHash("unk30", asset.unk30);

				// unused?
				json.WriteFieldValueUnknown("unk10", asset.unk10);
				json.WriteFieldValueUnknown("unk20", asset.unk20);

				json.WriteFieldValueUnknown("unk80", asset.unk80);
				json.WriteFieldValueUnknown("unk88", asset.unk88);
				json.WriteFieldValueUnknown("unk90", asset.unk90);
				json.WriteFieldValueUnknown("unka8", asset.unka8);
				json.WriteFieldValueUnknown("unkb0", asset.unkb0);
				json.WriteFieldValueUnknown("unkb8", asset.unkb8);
				json.WriteFieldValueUnknown("unkc0", asset.unkc0);
				json.WriteFieldValueUnknown("unke8", asset.unke8);
				json.WriteFieldValueUnknown("unkf0", asset.unkf0);

				json.WriteFieldValueUnknown("unk100", asset.unk100);
				json.WriteFieldValueUnknown("unk108", asset.unk108);
				json.WriteFieldValueUnknown("unk110", asset.unk110);
				json.WriteFieldValueUnknown("unk118", asset.unk118);
				json.WriteFieldValueUnknown("unk120", asset.unk120);
				json.WriteFieldValueUnknown("unk128", asset.unk128);
				json.WriteFieldValueUnknown("unk130", asset.unk130);
				json.WriteFieldValueUnknown("unk138", asset.unk138);
				json.WriteFieldValueUnknown("unk140", asset.unk140);

			}
			

			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "ai" / "type"
				/ fastfile::GetCurrentContext().ffname
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset.name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump aitype {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorkerBundleList<AiShootStyles>, bo6::T10HashAssetType, Worker> implAiShootStyles{ 
		GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_AISHOOTSTYLESLIST, "aishootstyleslist", "ai/shootstyleslist"
	};
	utils::MapAdder<ImplWorkerBundleList<AiTokenTypes>, bo6::T10HashAssetType, Worker> implAiTokenTypes{ 
		GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_AI_TOKEN_TYPES, "ai_token_types", "ai/token/types" 
	};
	utils::MapAdder<ImplWorkerBundleList<AiTokenStealingDefinitions>, bo6::T10HashAssetType, Worker> implAiTokenStealingDefinitions{
		GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_AI_TOKEN_STEALING_DEFINITIONS, "ai_token_stealing_definitions", "ai/token/stealingdefinitions"
	};
	utils::MapAdder<ImplWorkerBundleList<AiTokenDefinitions>, bo6::T10HashAssetType, Worker> implAiTokenDefinitions{ 
		GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_AI_TOKEN_DEFINITIONS, "ai_token_definitions", "ai/token/definitions" 
	};
	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> impl{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_AITYPE, sizeof(AIType) };
}