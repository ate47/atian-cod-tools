#include <includes.hpp>
#include <rapidcsv.h>
#include <tools/fastfile/linkers/linker_bo4.hpp>

namespace {
	using namespace fastfile::linker::bo4;
	using namespace games::bo4::pool;


	enum CharacterSkeleton : uint32_t {
		CHARACTER_SKEL_BASE = 0x0,
		CHARACTER_SKEL_SCALED80 = 0x1,
		CHARACTER_SKEL_PROP80 = 0x2,
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
	};

	struct ScriptBundle;
	struct XModel;
	struct XModelAlias;
	struct RagdollDef;

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

	class XAssetLinkerImpl : public XAssetLinker {
	public:
		using XAssetLinker::XAssetLinker;

		void Compute(BO4LinkContext& ctx, const char* id, BO4FFContext& ff) override {

			std::filesystem::path path{ ctx.linkCtx.input / id };
			std::filesystem::path rfpath{ path.filename() };
			rfpath.replace_extension();

			BO4Json objCfg{ path };

			if (!objCfg.SyncConfig(false)) {
				LOG_ERROR("Can't read {}", path.string());
				ctx.error = true;
				return;
			}

			ff.data.PushStream(XFILE_BLOCK_TEMP);
			Character& character{ ff.data.AllocStreamRef<Character>() };

			std::string rfpathStr{ rfpath.string() };
			std::string assetName{ objCfg.GetString("name", rfpathStr.c_str()) };
			ctx.AddXHash(assetName, character.name);

			objCfg.Load("skeleton", character.skeleton);
			objCfg.Load("voice", character.voice);
			objCfg.Load("chestGear_Right_Required", character.chestGear_Right_Required);
			objCfg.Load("chestGear_Left_Required", character.chestGear_Left_Required);
			objCfg.Load("frontPack_Required", character.frontPack_Required);
			objCfg.Load("frontPackLow_Required", character.frontPackLow_Required);
			objCfg.Load("backPack_Required", character.backPack_Required);
			objCfg.Load("rearGear_Right_Required", character.rearGear_Right_Required);
			objCfg.Load("rearGear_Left_Required", character.rearGear_Left_Required);
			objCfg.Load("rearGearSide_Right_Required", character.rearGearSide_Right_Required);
			objCfg.Load("rearGearSide_Left_Required", character.rearGearSide_Left_Required);
			objCfg.Load("sideGear_Right_Required", character.sideGear_Right_Required);
			objCfg.Load("sideGear_Left_Required", character.sideGear_Left_Required);
			objCfg.Load("frontGear_Right_Required", character.frontGear_Right_Required);
			objCfg.Load("frontGear_Left_Required", character.frontGear_Left_Required);
			objCfg.Load("helmet_Required", character.helmet_Required);
			objCfg.Load("misc1_Required", character.misc1_Required);
			objCfg.Load("misc2_Required", character.misc2_Required);
			objCfg.Load("misc3_Required", character.misc3_Required);
			objCfg.Load("headImpactType", character.headImpactType);
			objCfg.Load("bodyImpactType", character.bodyImpactType);

			ff.data.PushStream(XFILE_BLOCK_VIRTUAL);

			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("destructibleDef"), character.destructibleDef, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("gibDef"), character.gibDef, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("fxDef"), character.fxDef, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("deathFxDef"), character.deathFxDef, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("altFxDef1"), character.altFxDef1, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("altFxDef2"), character.altFxDef2, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("altFxDef3"), character.altFxDef3, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("altFxDef4"), character.altFxDef4, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("altFxDef5"), character.altFxDef5, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("altFxDef6"), character.altFxDef6, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("body"), character.body, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODELALIAS, objCfg.GetCString("bodyAlias"), character.bodyAlias, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("head"), character.head, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODELALIAS, objCfg.GetCString("headAlias"), character.headAlias, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODELALIAS, objCfg.GetCString("headAliasZombie"), character.headAliasZombie, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("hat"), character.hat, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODELALIAS, objCfg.GetCString("hatAlias"), character.hatAlias, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("gear"), character.gear, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODELALIAS, objCfg.GetCString("gearAlias"), character.gearAlias, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("viewModel"), character.viewModel, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("shadowCharacter"), character.shadowCharacter, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("damagedTorso_Clean"), character.damagedTorso_Clean, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODELALIAS, objCfg.GetCString("damagedTorso_Clean_Alias"), character.damagedTorso_Clean_Alias, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("damagedTorso_RightArmGone"), character.damagedTorso_Clean, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODELALIAS, objCfg.GetCString("damagedTorso_RightArmGone_Alias"), character.damagedTorso_Clean_Alias, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("damagedTorso_LeftArmGone"), character.damagedTorso_Clean, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODELALIAS, objCfg.GetCString("damagedTorso_LeftArmGone_Alias"), character.damagedTorso_Clean_Alias, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("damagedTorso_GutsGone"), character.damagedTorso_Clean, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODELALIAS, objCfg.GetCString("damagedTorso_GutsGone_Alias"), character.damagedTorso_Clean_Alias, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("damagedTorso_HeadGone"), character.damagedTorso_Clean, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODELALIAS, objCfg.GetCString("damagedTorso_HeadGone_Alias"), character.damagedTorso_Clean_Alias, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("damagedLegs_Clean"), character.damagedTorso_Clean, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODELALIAS, objCfg.GetCString("damagedLegs_Clean_Alias"), character.damagedTorso_Clean_Alias, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("damagedLegs_RightLegGone"), character.damagedTorso_Clean, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODELALIAS, objCfg.GetCString("damagedLegs_RightLegGone_Alias"), character.damagedTorso_Clean_Alias, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("damagedLegs_LeftLegGone"), character.damagedTorso_Clean, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODELALIAS, objCfg.GetCString("damagedLegs_LeftLegGone_Alias"), character.damagedTorso_Clean_Alias, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("damagedLegs_NoLegs"), character.damagedTorso_Clean, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODELALIAS, objCfg.GetCString("damagedLegs_NoLegs_Alias"), character.damagedTorso_Clean_Alias, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("chestGear_Right"), character.chestGear_Right, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("chestGear_Left"), character.chestGear_Left, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("frontPack"), character.frontPack, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("frontPackLow"), character.frontPackLow, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("backPack"), character.backPack, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("rearGear_Right"), character.rearGear_Right, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("rearGear_Left"), character.rearGear_Left, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("rearGearSide_Right"), character.rearGearSide_Right, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("rearGearSide_Left"), character.rearGearSide_Left, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("sideGear_Right"), character.sideGear_Right, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("sideGear_Left"), character.sideGear_Left, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("frontGear_Right"), character.frontGear_Right, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("frontGear_Left"), character.frontGear_Left, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("helmet"), character.helmet, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("misc1"), character.misc1, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("misc2"), character.misc2, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_XMODEL, objCfg.GetCString("misc3"), character.misc3, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_RAGDOLL, objCfg.GetCString("ragdollDef"), character.ragdollDef, false, &ff);

			ff.data.AddXString(character.eyeGlowFX, objCfg.GetCString("eyeGlowFX"));
			ff.data.AddXString(character.eyeGlowBone, objCfg.GetCString("eyeGlowBone"));
			
			ff.data.PopStream();

			ff.data.PopStream();

			LOG_INFO("Added asset character {} (hash_{:x})", assetName, character.name.name);
		}
	};

	utils::MapAdder<XAssetLinkerImpl, XAssetType, XAssetLinker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_CHARACTER };
}