#include <includes.hpp>
#include <rapidcsv.h>
#include <tools/fastfile/linkers/linker_bo4.hpp>

namespace {
	using namespace fastfile::linker::bo4;
	using namespace games::bo4::pool;

	struct AIValidations {
		ScrString_t requiredBones[64];
		bool noCosmeticBones;
	};
	struct FaceQuadrantThreshold {
		float forwardDot;
		float sideDot;
	};
	struct CoverOffset {
		int xOffset;
		int yOffset;
		float yawOffset;
	};
	struct AISteering {
		float turnRate;
		float maxTurnRateAngle;
		float sensorSize;
		float collisionPenalty;
		float dodgingPenalty;
		float penetrationPenalty;
		float sidednessChangingPenalty;
		float velocityHysteresis;
	};
	struct AIDemoLockOnView {
		int heightOffset;
		float pitchMin;
		float pitchMax;
	};
	struct AIDemoLockOnValues {
		float highlightDistance;
		AIDemoLockOnView views[2];
	};



	typedef int32_t eAttachment;
	struct WeaponDef;
	struct FootstepTableDef;
	struct SurfaceFXTableDef;
	struct ScriptBundle;
	struct Character;
	struct BehaviorTree;
	struct AnimStateMachine;
	struct AnimSelectorTableSet;
	struct AnimMappingTable;
	struct AimTable;
	struct ShootTable;

	struct AiType {
		XHash name;
		XString animTree;
		XHash csvInclude;
		uint64_t unk28;
		uint64_t unk30;
		WeaponDef* primaryWeapons[4];
		uint32_t numPrimaryWeapons;
		float primaryAttachmentChance[7];
		eAttachment primaryBannedAttachments[64];
		XHash primaryAllowedCamos[100];
		WeaponDef* secondaryWeapon;
		WeaponDef* sideArm;
		WeaponDef* meleeWeapon;
		WeaponDef* grenadeWeapon;
		int grenadeAmmo;
		WeaponDef* ammoPouch;
		AIWeaponFireType fireMode;
		int burstCountMin;
		int burstCountMax;
		int fireIntervalmin;
		int fireIntervalmax;
		team_t team;
		XHash species;
		XHash archeType;
		XHash scoreType;
		ScrString_t typeSuffix;
		AICategory category;
		ScrString_t tag1;
		ScrString_t tag2;
		ScrString_t tag3;
		ScrString_t weakPointsTags[6];
		float physRadius;
		float physHeight;
		bool fixedNode;
		bool isPacifist;
		float pathEnemyFightDist;
		float runAndGunDist;
		float faceEnemyDist;
		int coverNodeSearchInterval;
		int newThreatDelay;
		int newPainDelay;
		float defaultGoalRadius;
		bool threatIgnoreVehicles;
		float sightDist;
		int painTolerance;
		FootstepTableDef* footstepTable;
		bool footstepScriptCallback;
		SurfaceFXTableDef* footstepFXTable1;
		SurfaceFXTableDef* footstepFXTable2;
		float engageMinDist;
		float engageMinFalloffDist;
		float engageMaxDist;
		float engageMaxFalloffDist;
		float frontQuadrantDegrees;
		float backQuadrantDegrees;
		float leftQuadrantDegrees;
		float rightQuadrantDegrees;
		FaceQuadrantThreshold thresholds[5];
		XHash movementType;
		float favorTraversalFactor;
		float favorSpecialTraversalFactor;
		bool ignoresDamageHitLocations;
		bool ignoreVortices;
		bool usesCovernodes;
		bool accurateFire;
		bool aggressiveMode;
		bool hero;
		bool matureGib;
		bool restrictedGib;
		bool allowFriendlyFire;
		ScriptBundle* traversalExtents;
		bool proceduralTraversals;
		float shootAngleThreshold;
		float closeShootAngleThreshold;
		float closeShootAngleDistance;
		CoverOffset coverCrouch;
		CoverOffset coverExposed;
		CoverOffset coverLeft;
		CoverOffset coverPillar;
		CoverOffset coverRight;
		CoverOffset coverStand;
		AISteering steering[3];
		bool canFlank;
		int flankingScoreScale;
		float flankingAngle;
		AIDemoLockOnValues demoLockOn;
		Character* character[8];
		uint32_t numCharacters;
		BehaviorTree* behaviorTree;
		AnimStateMachine* animStateMachine;
		AnimSelectorTableSet* animSelectorTable;
		int animMappingTableDefault;
		AnimMappingTable* animMappingTables[3];
		AimTable* aimTable;
		ShootTable* shootTable;
		ScriptBundle* assassinationBundle;
		ScriptBundle* aivsaiMeleeBundles[5];
		ScriptBundle* aiFxBundle;
		ScriptBundle* aiNotetrackBoneMappingBundle;
		XHash femaleVersion;
		ScrString_t properName;
		ScriptBundle* randomName;
		ScrString_t randomNames[128];
		int32_t randomNamesCount;
		ScrString_t randomRanks[32];
		int32_t randomRanksCount;
		ScrString_t spawnerType;
		XHash spawnInfluencers[2];
		XHash compassIcon;
		GfxImage* tacticalModeIcon;
		XHash serverFlowGraph;
		XHash clientFlowGraph;
		ScriptBundle* aiSettingsBundle;
		AIValidations validations;
		XHash unkef0;
	};


	static_assert(sizeof(AiType) == 0xf00);


	class AITypeTypeGetter : public BO4TypeGetter {
	public:
		using BO4TypeGetter::Load;

		template<typename CF = core::config::ConfigGeneric<BO4TypeGetter>>
		static void Load(const char* path, CF& base, FaceQuadrantThreshold& ref, FaceQuadrantThreshold defaultValue) {
			CF sub{ base.GetSubVal(path) };
			if (!sub) return;
			sub.Load("forwardDot", ref.forwardDot);
			sub.Load("sideDot", ref.sideDot);
		}

		template<typename CF = core::config::ConfigGeneric<BO4TypeGetter>>
		static void Load(const char* path, CF& base, AIDemoLockOnValues& ref, AIDemoLockOnValues defaultValue) {
			CF sub{ base.GetSubVal(path) };
			if (!sub) return;
			sub.Load("highlightDistance", ref.highlightDistance);
			sub.Load("views", ref.views);
		}

		template<typename CF = core::config::ConfigGeneric<BO4TypeGetter>>
		static void Load(const char* path, CF& base, AIDemoLockOnView& ref, AIDemoLockOnView defaultValue) {
			CF sub{ base.GetSubVal(path) };
			if (!sub) return;
			sub.Load("heightOffset", ref.heightOffset);
			sub.Load("pitchMin", ref.pitchMin);
			sub.Load("pitchMax", ref.pitchMax);
		}

		template<typename CF = core::config::ConfigGeneric<BO4TypeGetter>>
		static void Load(const char* path, CF& base, CoverOffset& ref, CoverOffset defaultValue) {
			CF sub{ base.GetSubVal(path) };
			if (!sub) return;
			sub.Load("xOffset", ref.xOffset);
			sub.Load("yOffset", ref.yOffset);
			sub.Load("yawOffset", ref.yawOffset);
		}

		template<typename CF = core::config::ConfigGeneric<BO4TypeGetter>>
		static void Load(const char* path, CF& base, AISteering& ref, AISteering defaultValue) {
			CF sub{ base.GetSubVal(path) };
			if (!sub) return;
			sub.Load("turnRate", ref.turnRate);
			sub.Load("maxTurnRateAngle", ref.maxTurnRateAngle);
			sub.Load("sensorSize", ref.sensorSize);
			sub.Load("collisionPenalty", ref.collisionPenalty);
			sub.Load("dodgingPenalty", ref.dodgingPenalty);
			sub.Load("penetrationPenalty", ref.penetrationPenalty);
			sub.Load("sidednessChangingPenalty", ref.sidednessChangingPenalty);
			sub.Load("velocityHysteresis", ref.velocityHysteresis);
		}
	};

	using BO4JsonAI = core::config::ConfigDocument<AITypeTypeGetter>;

	class XAssetLinkerImpl : public XAssetLinker {
	public:
		using XAssetLinker::XAssetLinker;

		void Compute(BO4LinkContext& ctx, const char* id, BO4FFContext& ff) override {

			std::filesystem::path path{ ctx.linkCtx.input / id };
			std::filesystem::path rfpath{ path.filename() };
			rfpath.replace_extension();

			BO4JsonAI objCfg{ path };

			if (!objCfg.SyncConfig(false)) {
				LOG_ERROR("Can't aitype read {}", path.string());
				ctx.error = true;
				return;
			}

			ff.data.PushStream(XFILE_BLOCK_TEMP);
			AiType& aitype{ ff.data.AllocStreamRef<AiType>() };

			std::string rfpathStr{ rfpath.string() };
			std::string assetName{ objCfg.GetString("name", rfpathStr.c_str()) };
			ctx.AddXHash(assetName, aitype.name);
			objCfg.Load("csvInclude", aitype.csvInclude);
			objCfg.Load("species", aitype.species);
			objCfg.Load("archeType", aitype.archeType);
			objCfg.Load("scoreType", aitype.scoreType);
			objCfg.Load("movementType", aitype.movementType);
			objCfg.Load("femaleVersion", aitype.femaleVersion);
			objCfg.Load("compassIcon", aitype.compassIcon);
			objCfg.Load("serverFlowGraph", aitype.serverFlowGraph);
			objCfg.Load("clientFlowGraph", aitype.clientFlowGraph);
			objCfg.Load("unkef0", aitype.unkef0);

			ctx.AddXHashArray("spawnInfluencers", objCfg.GetVal("spawnInfluencers", 0, objCfg.base), aitype.spawnInfluencers, ACTS_ARRAYSIZE(aitype.spawnInfluencers));
			ctx.AddXHashArray("primaryAllowedCamos", objCfg.GetVal("primaryAllowedCamos", 0, objCfg.base), aitype.primaryAllowedCamos, ACTS_ARRAYSIZE(aitype.primaryAllowedCamos));

			ctx.AddScrString(objCfg.GetCString("typeSuffix"), aitype.typeSuffix, ff);
			ctx.AddScrString(objCfg.GetCString("tag1"), aitype.tag1, ff);
			ctx.AddScrString(objCfg.GetCString("tag2"), aitype.tag2, ff);
			ctx.AddScrString(objCfg.GetCString("tag3"), aitype.tag3, ff);
			ctx.AddScrString(objCfg.GetCString("properName"), aitype.properName, ff);
			ctx.AddScrString(objCfg.GetCString("spawnerType"), aitype.spawnerType, ff);

			ctx.AddScrStringArray("weakPointsTags", objCfg.GetVal("weakPointsTags", 0, objCfg.base), aitype.weakPointsTags, ACTS_ARRAYSIZE(aitype.weakPointsTags), ff);
			aitype.randomNamesCount = (int32_t)ctx.AddScrStringArray("randomNames", objCfg.GetVal("randomNames", 0, objCfg.base), aitype.randomNames, ACTS_ARRAYSIZE(aitype.randomNames), ff);
			aitype.randomRanksCount = (int32_t)ctx.AddScrStringArray("randomRanks", objCfg.GetVal("randomRanks", 0, objCfg.base), aitype.randomRanks, ACTS_ARRAYSIZE(aitype.randomRanks), ff);

			objCfg.Load("accurateFire", aitype.accurateFire);
			objCfg.Load("aggressiveMode", aitype.aggressiveMode);
			objCfg.Load("allowFriendlyFire", aitype.allowFriendlyFire);
			objCfg.Load("canFlank", aitype.canFlank);
			objCfg.Load("fixedNode", aitype.fixedNode);
			objCfg.Load("footstepScriptCallback", aitype.footstepScriptCallback);
			objCfg.Load("hero", aitype.hero);
			objCfg.Load("ignoresDamageHitLocations", aitype.ignoresDamageHitLocations);
			objCfg.Load("ignoreVortices", aitype.ignoreVortices);
			objCfg.Load("isPacifist", aitype.isPacifist);
			objCfg.Load("matureGib", aitype.matureGib);
			objCfg.Load("proceduralTraversals", aitype.proceduralTraversals);
			objCfg.Load("restrictedGib", aitype.restrictedGib);
			objCfg.Load("threatIgnoreVehicles", aitype.threatIgnoreVehicles);
			objCfg.Load("usesCovernodes", aitype.usesCovernodes);

			objCfg.Load("backQuadrantDegrees", aitype.backQuadrantDegrees);
			objCfg.Load("closeShootAngleDistance", aitype.closeShootAngleDistance);
			objCfg.Load("closeShootAngleThreshold", aitype.closeShootAngleThreshold);
			objCfg.Load("defaultGoalRadius", aitype.defaultGoalRadius);
			objCfg.Load("engageMaxDist", aitype.engageMaxDist);
			objCfg.Load("engageMaxFalloffDist", aitype.engageMaxFalloffDist);
			objCfg.Load("engageMinDist", aitype.engageMinDist);
			objCfg.Load("engageMinFalloffDist", aitype.engageMinFalloffDist);
			objCfg.Load("faceEnemyDist", aitype.faceEnemyDist);
			objCfg.Load("favorSpecialTraversalFactor", aitype.favorSpecialTraversalFactor);
			objCfg.Load("favorTraversalFactor", aitype.favorTraversalFactor);
			objCfg.Load("flankingAngle", aitype.flankingAngle);
			objCfg.Load("frontQuadrantDegrees", aitype.frontQuadrantDegrees);
			objCfg.Load("leftQuadrantDegrees", aitype.leftQuadrantDegrees);
			objCfg.Load("pathEnemyFightDist", aitype.pathEnemyFightDist);
			objCfg.Load("physHeight", aitype.physHeight);
			objCfg.Load("physRadius", aitype.physRadius);
			objCfg.Load("rightQuadrantDegrees", aitype.rightQuadrantDegrees);
			objCfg.Load("runAndGunDist", aitype.runAndGunDist);
			objCfg.Load("shootAngleThreshold", aitype.shootAngleThreshold);
			objCfg.Load("sightDist", aitype.sightDist);
			objCfg.Load("primaryAttachmentChance", aitype.primaryAttachmentChance);

			objCfg.Load("animMappingTableDefault", aitype.animMappingTableDefault);
			objCfg.Load("burstCountMax", aitype.burstCountMax);
			objCfg.Load("burstCountMin", aitype.burstCountMin);
			objCfg.Load("coverNodeSearchInterval", aitype.coverNodeSearchInterval);
			objCfg.Load("fireIntervalmax", aitype.fireIntervalmax);
			objCfg.Load("fireIntervalmin", aitype.fireIntervalmin);
			objCfg.Load("flankingScoreScale", aitype.flankingScoreScale);
			objCfg.Load("grenadeAmmo", aitype.grenadeAmmo);
			objCfg.Load("newPainDelay", aitype.newPainDelay);
			objCfg.Load("newThreatDelay", aitype.newThreatDelay);
			objCfg.Load("painTolerance", aitype.painTolerance);
			objCfg.Load("unk28", aitype.unk28);
			objCfg.Load("unk30", aitype.unk30);

			objCfg.Load("category", aitype.category);
			objCfg.Load("fireMode", aitype.fireMode);
			objCfg.Load("team", aitype.team);
			objCfg.Load<int32_t>("primaryBannedAttachments", aitype.primaryBannedAttachments);

			objCfg.Load("demoLockOn", aitype.demoLockOn);
			objCfg.Load("coverCrouch", aitype.coverCrouch);
			objCfg.Load("coverExposed", aitype.coverExposed);
			objCfg.Load("coverLeft", aitype.coverLeft);
			objCfg.Load("coverPillar", aitype.coverPillar);
			objCfg.Load("coverRight", aitype.coverRight);
			objCfg.Load("coverStand", aitype.coverStand);
			objCfg.Load("steering", aitype.steering);
			objCfg.Load("thresholds", aitype.thresholds);

			objCfg.Load("validations.noCosmeticBones", aitype.validations.noCosmeticBones, true);

			ctx.AddScrStringArray("validations.requiredBones", objCfg.GetVal("validations.requiredBones", 0, objCfg.base), aitype.validations.requiredBones, ACTS_ARRAYSIZE(aitype.validations.requiredBones), ff);


			ff.data.PushStream(XFILE_BLOCK_VIRTUAL);

			ff.data.AddXString(aitype.animTree, assetName.data());

			aitype.numPrimaryWeapons = 
				(uint32_t)ctx.LinkAssetArray(
					XAssetType::ASSET_TYPE_WEAPON, "primaryWeapons", objCfg.GetVal("primaryWeapons", 0, objCfg.base), 
					aitype.primaryWeapons, &ff
				);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_WEAPON, objCfg.GetCString("secondaryWeapon"), aitype.secondaryWeapon, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_WEAPON, objCfg.GetCString("sideArm"), aitype.sideArm, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_WEAPON, objCfg.GetCString("meleeWeapon"), aitype.meleeWeapon, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_WEAPON, objCfg.GetCString("grenadeWeapon"), aitype.grenadeWeapon, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_WEAPON, objCfg.GetCString("ammoPouch"), aitype.ammoPouch, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_FOOTSTEP_TABLE, objCfg.GetCString("footstepTable"), aitype.footstepTable, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SURFACEFX_TABLE, objCfg.GetCString("footstepFXTable1"), aitype.footstepFXTable1, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SURFACEFX_TABLE, objCfg.GetCString("footstepFXTable2"), aitype.footstepFXTable2, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("traversalExtents"), aitype.traversalExtents, false, &ff);

			aitype.numCharacters =
				(uint32_t)ctx.LinkAssetArray(
					XAssetType::ASSET_TYPE_CHARACTER, "character", objCfg.GetVal("character", 0, objCfg.base),
					aitype.character, &ff
				);

			ctx.LinkAsset(XAssetType::ASSET_TYPE_BEHAVIORTREE, objCfg.GetCString("behaviorTree"), aitype.behaviorTree, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_ANIMSTATEMACHINE, objCfg.GetCString("animStateMachine"), aitype.animStateMachine, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_ANIMSELECTORTABLESET, objCfg.GetCString("animSelectorTable"), aitype.animSelectorTable, false, &ff);
			ctx.LinkAssetArray(XAssetType::ASSET_TYPE_ANIMMAPPINGTABLE, "animMappingTables", objCfg.GetVal("animMappingTables", 0, objCfg.base), aitype.animMappingTables, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_AIMTABLE, objCfg.GetCString("aimTable"), aitype.aimTable, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SHOOTTABLE, objCfg.GetCString("shootTable"), aitype.shootTable, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("assassinationBundle"), aitype.assassinationBundle, false, &ff);
			ctx.LinkAssetArray(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, "aivsaiMeleeBundles", objCfg.GetVal("aivsaiMeleeBundles", 0, objCfg.base), aitype.aivsaiMeleeBundles, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("aiFxBundle"), aitype.aiFxBundle, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("aiNotetrackBoneMappingBundle"), aitype.aiNotetrackBoneMappingBundle, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("randomName"), aitype.randomName, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_SCRIPTBUNDLE, objCfg.GetCString("aiSettingsBundle"), aitype.aiSettingsBundle, false, &ff);

			ff.data.PopStream();

			ff.data.PopStream();

			LOG_INFO("Added asset aitype {} (hash_{:x})", assetName, aitype.name.name);
		}
	};

	utils::MapAdder<XAssetLinkerImpl, XAssetType, XAssetLinker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_AITYPE };
}