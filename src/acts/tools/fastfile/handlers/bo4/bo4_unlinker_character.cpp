#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/utils/data_utils.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	using namespace games::bo4::pool;


	enum CharacterSkeleton : uint32_t {
		CHARACTER_SKEL_BASE = 0x0,
		CHARACTER_SKEL_SCALED80 = 0x1,
		CHARACTER_SKEL_PROP80 = 0x2,
		NUM_CHARACTER_SKEL_TYPES = 0x3,
	};

	enum CharacterVoice : uint32_t {
		CHARACTER_VOICE_AMERICAN = 0x0,
		CHARACTER_VOICE_RUSSIAN = 0x1,
		CHARACTER_VOICE_UNITA = 0x2,
		CHARACTER_VOICE_CUBAN = 0x3,
		CHARACTER_VOICE_MUJAHIDEEN = 0x4,
		CHARACTER_VOICE_PDF = 0x5,
		CHARACTER_VOICE_PMC = 0x6,
		CHARACTER_VOICE_ISI = 0x7,
		CHARACTER_VOICE_DIGBAT = 0x8,
		CHARACTER_VOICE_YEMENI = 0x9,
		CHARACTER_VOICE_TERRORIST = 0xA,
		CHARACTER_VOICE_LAPD = 0xB,
		CHARACTER_VOICE_SECRETSERVICE = 0xC,
		CHARACTER_VOICE_MP = 0xD,
		CHARACTER_VOICE_CHINESE = 0xE,
		NUM_CHARACTER_VOICE_TYPES = 0xF,
	};

	const char* characterSkeletonNames[]{
		"base",
		"scaled80",
		"prop80",
	};
	const char* characterVoiceNames[]{
		"american",
		"russian",
		"unita",
		"cuban",
		"mujahideen",
		"pdf",
		"pmc",
		"isi",
		"digbat",
		"yemeni",
		"terrorist",
		"lapd",
		"secretservice",
		"mp",
		"chinese",
	};

	const char* CharacterSkeletonName(CharacterSkeleton id) {
		if (id >= ARRAYSIZE(characterSkeletonNames)) {
			return "unknown";
		}
		return characterSkeletonNames[id];
	}

	const char* CharacterVoiceName(CharacterVoice id) {
		if (id >= ARRAYSIZE(characterVoiceNames)) {
			return "unknown";
		}
		return characterVoiceNames[id];
	}

	struct Character {
		XHash name;
		CharacterSkeleton skeleton;
		CharacterVoice voice;
		ScriptBundle* destructibleDef;
		ScriptBundle* gibDef;
		ScriptBundle* fxDef;
		ScriptBundle* deathFxDef;
		ScriptBundle* altFxDef1;
		ScriptBundle* altFxDef2;
		ScriptBundle* altFxDef3;
		ScriptBundle* altFxDef4;
		ScriptBundle* altFxDef5;
		ScriptBundle* altFxDef6;
		XModel* body;
		XModelAlias* bodyAlias;
		XModel* head;
		XModelAlias* headAlias;
		XModelAlias* headAliasZombie;
		XModel* hat;
		XModelAlias* hatAlias;
		XModel* gear;
		XModelAlias* gearAlias;
		XModel* viewModel;
		XModel* shadowCharacter;
		XModel* damagedTorso_Clean;
		XModelAlias* damagedTorso_Clean_Alias;
		XModel* damagedTorso_RightArmGone;
		XModelAlias* damagedTorso_RightArmGone_Alias;
		XModel* damagedTorso_LeftArmGone;
		XModelAlias* damagedTorso_LeftArmGone_Alias;
		XModel* damagedTorso_GutsGone;
		XModelAlias* damagedTorso_GutsGone_Alias;
		XModel* damagedTorso_HeadGone;
		XModelAlias* damagedTorso_HeadGone_Alias;
		XModel* damagedLegs_Clean;
		XModelAlias* damagedLegs_Clean_Alias;
		XModel* damagedLegs_RightLegGone;
		XModelAlias* damagedLegs_RightLegGone_Alias;
		XModel* damagedLegs_LeftLegGone;
		XModelAlias* damagedLegs_LeftLegGone_Alias;
		XModel* damagedLegs_NoLegs;
		XModelAlias* damagedLegs_NoLegs_Alias;
		XModel* chestGear_Right;
		bool chestGear_Right_Required;
		XModel* chestGear_Left;
		bool chestGear_Left_Required;
		XModel* frontPack;
		bool frontPack_Required;
		XModel* frontPackLow;
		bool frontPackLow_Required;
		XModel* backPack;
		bool backPack_Required;
		XModel* rearGear_Right;
		bool rearGear_Right_Required;
		XModel* rearGear_Left;
		bool rearGear_Left_Required;
		XModel* rearGearSide_Right;
		bool rearGearSide_Right_Required;
		XModel* rearGearSide_Left;
		bool rearGearSide_Left_Required;
		XModel* sideGear_Right;
		bool sideGear_Right_Required;
		XModel* sideGear_Left;
		bool sideGear_Left_Required;
		XModel* frontGear_Right;
		bool frontGear_Right_Required;
		XModel* frontGear_Left;
		bool frontGear_Left_Required;
		XModel* helmet;
		bool helmet_Required;
		XModel* misc1;
		bool misc1_Required;
		XModel* misc2;
		bool misc2_Required;
		XModel* misc3;
		bool misc3_Required;
		RagdollDef* ragdollDef;
		const char* eyeGlowFX;
		const char* eyeGlowBone;
		int headImpactType;
		int bodyImpactType;
	};

	static_assert(sizeof(Character) == 0x280);

	class ImplWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {

			Character* asset{ (Character*)ptr };
			// refs
			constexpr size_t postWeaponDefNameField = offsetof(Character, name) + sizeof(Character::name);
			constexpr size_t testLen = sizeof(Character) - postWeaponDefNameField;

			if (utils::data::IsNulled(&((byte*)ptr)[postWeaponDefNameField], testLen)) {
				LOG_INFO("ignore empty character {}", hashutils::ExtractTmp("file", asset->name.name));
				return;
			}


			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "character"
				/ fastfile::GetCurrentContext().ffname
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name.name)) };
			std::filesystem::create_directories(outFile.parent_path());

			BO4JsonWriter json{};

			LOG_INFO("Dump character {}", outFile.string());


			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueString("skeleton", CharacterSkeletonName(asset->skeleton));
			json.WriteFieldValueString("voice", CharacterVoiceName(asset->voice));
			json.WriteFieldValueXString("eyeGlowFX", asset->eyeGlowFX);
			json.WriteFieldValueXString("eyeGlowBone", asset->eyeGlowBone);
			json.WriteFieldValueNumber("headImpactType", asset->headImpactType);
			json.WriteFieldValueNumber("bodyImpactType", asset->bodyImpactType);
			json.WriteFieldValueXAsset("destructibleDef", ASSET_TYPE_SCRIPTBUNDLE, asset->destructibleDef);
			json.WriteFieldValueXAsset("gibDef", ASSET_TYPE_SCRIPTBUNDLE, asset->gibDef);
			json.WriteFieldValueXAsset("fxDef", ASSET_TYPE_SCRIPTBUNDLE, asset->fxDef);
			json.WriteFieldValueXAsset("deathFxDef", ASSET_TYPE_SCRIPTBUNDLE, asset->deathFxDef);
			json.WriteFieldValueXAsset("altFxDef1", ASSET_TYPE_SCRIPTBUNDLE, asset->altFxDef1);
			json.WriteFieldValueXAsset("altFxDef2", ASSET_TYPE_SCRIPTBUNDLE, asset->altFxDef2);
			json.WriteFieldValueXAsset("altFxDef3", ASSET_TYPE_SCRIPTBUNDLE, asset->altFxDef3);
			json.WriteFieldValueXAsset("altFxDef4", ASSET_TYPE_SCRIPTBUNDLE, asset->altFxDef4);
			json.WriteFieldValueXAsset("altFxDef5", ASSET_TYPE_SCRIPTBUNDLE, asset->altFxDef5);
			json.WriteFieldValueXAsset("altFxDef6", ASSET_TYPE_SCRIPTBUNDLE, asset->altFxDef6);
			json.WriteFieldValueXAsset("ragdollDef", ASSET_TYPE_RAGDOLL, asset->ragdollDef);

			json.WriteFieldValueXAsset("body", ASSET_TYPE_XMODEL, asset->body);
			json.WriteFieldValueXAsset("bodyAlias", ASSET_TYPE_XMODELALIAS, asset->bodyAlias);
			json.WriteFieldValueXAsset("head", ASSET_TYPE_XMODEL, asset->head);
			json.WriteFieldValueXAsset("headAlias", ASSET_TYPE_XMODELALIAS, asset->headAlias);
			json.WriteFieldValueXAsset("headAliasZombie", ASSET_TYPE_XMODELALIAS, asset->headAliasZombie);
			json.WriteFieldValueXAsset("hat", ASSET_TYPE_XMODEL, asset->hat);
			json.WriteFieldValueXAsset("hatAlias", ASSET_TYPE_XMODELALIAS, asset->hatAlias);
			json.WriteFieldValueXAsset("gear", ASSET_TYPE_XMODEL, asset->gear);
			json.WriteFieldValueXAsset("gearAlias", ASSET_TYPE_XMODELALIAS, asset->gearAlias);
			json.WriteFieldValueXAsset("viewModel", ASSET_TYPE_XMODEL, asset->viewModel);
			json.WriteFieldValueXAsset("shadowCharacter", ASSET_TYPE_XMODEL, asset->shadowCharacter);

			json.WriteFieldValueXAsset("damagedTorso_Clean", ASSET_TYPE_XMODEL, asset->damagedTorso_Clean);
			json.WriteFieldValueXAsset("damagedTorso_Clean_Alias", ASSET_TYPE_XMODELALIAS, asset->damagedTorso_Clean_Alias);
			json.WriteFieldValueXAsset("damagedTorso_RightArmGone", ASSET_TYPE_XMODEL, asset->damagedTorso_RightArmGone);
			json.WriteFieldValueXAsset("damagedTorso_RightArmGone_Alias", ASSET_TYPE_XMODELALIAS, asset->damagedTorso_RightArmGone_Alias);
			json.WriteFieldValueXAsset("damagedTorso_LeftArmGone", ASSET_TYPE_XMODEL, asset->damagedTorso_LeftArmGone);
			json.WriteFieldValueXAsset("damagedTorso_LeftArmGone_Alias", ASSET_TYPE_XMODELALIAS, asset->damagedTorso_LeftArmGone_Alias);
			json.WriteFieldValueXAsset("damagedTorso_GutsGone", ASSET_TYPE_XMODEL, asset->damagedTorso_GutsGone);
			json.WriteFieldValueXAsset("damagedTorso_GutsGone_Alias", ASSET_TYPE_XMODELALIAS, asset->damagedTorso_GutsGone_Alias);
			json.WriteFieldValueXAsset("damagedTorso_HeadGone", ASSET_TYPE_XMODEL, asset->damagedTorso_HeadGone);
			json.WriteFieldValueXAsset("damagedTorso_HeadGone_Alias", ASSET_TYPE_XMODELALIAS, asset->damagedTorso_HeadGone_Alias);
			json.WriteFieldValueXAsset("damagedLegs_Clean", ASSET_TYPE_XMODEL, asset->damagedLegs_Clean);
			json.WriteFieldValueXAsset("damagedLegs_Clean_Alias", ASSET_TYPE_XMODELALIAS, asset->damagedLegs_Clean_Alias);
			json.WriteFieldValueXAsset("damagedLegs_RightLegGone", ASSET_TYPE_XMODEL, asset->damagedLegs_RightLegGone);
			json.WriteFieldValueXAsset("damagedLegs_RightLegGone_Alias", ASSET_TYPE_XMODELALIAS, asset->damagedLegs_RightLegGone_Alias);
			json.WriteFieldValueXAsset("damagedLegs_LeftLegGone", ASSET_TYPE_XMODEL, asset->damagedLegs_LeftLegGone);
			json.WriteFieldValueXAsset("damagedLegs_LeftLegGone_Alias", ASSET_TYPE_XMODELALIAS, asset->damagedLegs_LeftLegGone_Alias);
			json.WriteFieldValueXAsset("damagedLegs_NoLegs", ASSET_TYPE_XMODEL, asset->damagedLegs_NoLegs);
			json.WriteFieldValueXAsset("damagedLegs_NoLegs_Alias", ASSET_TYPE_XMODELALIAS, asset->damagedLegs_NoLegs_Alias);

			json.WriteFieldValueXAsset("chestGear_Right", ASSET_TYPE_XMODEL, asset->chestGear_Right);
			if (asset->chestGear_Right_Required) json.WriteFieldValueBool("chestGear_Right_Required", asset->chestGear_Right_Required);
			json.WriteFieldValueXAsset("chestGear_Left", ASSET_TYPE_XMODEL, asset->chestGear_Left);
			if (asset->chestGear_Left_Required) json.WriteFieldValueBool("chestGear_Left_Required", asset->chestGear_Left_Required);
			json.WriteFieldValueXAsset("frontPack", ASSET_TYPE_XMODEL, asset->frontPack);
			if (asset->frontPack_Required) json.WriteFieldValueBool("frontPack_Required", asset->frontPack_Required);
			json.WriteFieldValueXAsset("frontPackLow", ASSET_TYPE_XMODEL, asset->frontPackLow);
			if (asset->frontPackLow_Required) json.WriteFieldValueBool("frontPackLow_Required", asset->frontPackLow_Required);
			json.WriteFieldValueXAsset("backPack", ASSET_TYPE_XMODEL, asset->backPack);
			if (asset->backPack_Required) json.WriteFieldValueBool("backPack_Required", asset->backPack_Required);
			json.WriteFieldValueXAsset("rearGear_Right", ASSET_TYPE_XMODEL, asset->rearGear_Right);
			if (asset->rearGear_Right_Required) json.WriteFieldValueBool("rearGear_Right_Required", asset->rearGear_Right_Required);
			json.WriteFieldValueXAsset("rearGear_Left", ASSET_TYPE_XMODEL, asset->rearGear_Left);
			if (asset->rearGear_Left_Required) json.WriteFieldValueBool("rearGear_Left_Required", asset->rearGear_Left_Required);
			json.WriteFieldValueXAsset("rearGearSide_Right", ASSET_TYPE_XMODEL, asset->rearGearSide_Right);
			if (asset->rearGearSide_Right_Required) json.WriteFieldValueBool("rearGearSide_Right_Required", asset->rearGearSide_Right_Required);
			json.WriteFieldValueXAsset("rearGearSide_Left", ASSET_TYPE_XMODEL, asset->rearGearSide_Left);
			if (asset->rearGearSide_Left_Required) json.WriteFieldValueBool("rearGearSide_Left_Required", asset->rearGearSide_Left_Required);
			json.WriteFieldValueXAsset("sideGear_Right", ASSET_TYPE_XMODEL, asset->sideGear_Right);
			if (asset->sideGear_Right_Required) json.WriteFieldValueBool("sideGear_Right_Required", asset->sideGear_Right_Required);
			json.WriteFieldValueXAsset("sideGear_Left", ASSET_TYPE_XMODEL, asset->sideGear_Left);
			if (asset->sideGear_Left_Required) json.WriteFieldValueBool("sideGear_Left_Required", asset->sideGear_Left_Required);
			json.WriteFieldValueXAsset("frontGear_Right", ASSET_TYPE_XMODEL, asset->frontGear_Right);
			if (asset->frontGear_Right_Required) json.WriteFieldValueBool("frontGear_Right_Required", asset->frontGear_Right_Required);
			json.WriteFieldValueXAsset("frontGear_Left", ASSET_TYPE_XMODEL, asset->frontGear_Left);
			if (asset->frontGear_Left_Required) json.WriteFieldValueBool("frontGear_Left_Required", asset->frontGear_Left_Required);
			json.WriteFieldValueXAsset("helmet", ASSET_TYPE_XMODEL, asset->helmet);
			if (asset->helmet_Required) json.WriteFieldValueBool("helmet_Required", asset->helmet_Required);
			json.WriteFieldValueXAsset("misc1", ASSET_TYPE_XMODEL, asset->misc1);
			if (asset->misc1_Required) json.WriteFieldValueBool("misc1_Required", asset->misc1_Required);
			json.WriteFieldValueXAsset("misc2", ASSET_TYPE_XMODEL, asset->misc2);
			if (asset->misc2_Required) json.WriteFieldValueBool("misc2_Required", asset->misc2_Required);
			json.WriteFieldValueXAsset("misc3", ASSET_TYPE_XMODEL, asset->misc3);
			if (asset->misc3_Required) json.WriteFieldValueBool("misc3_Required", asset->misc3_Required);

			json.EndObject();

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_CHARACTER };
}