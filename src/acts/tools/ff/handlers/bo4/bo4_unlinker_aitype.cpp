#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/utils/data_utils.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	using namespace games::bo4::pool;
	class AiTypeWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
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
			AiType& asset{ *(AiType*)ptr };

			utils::raw_file_extractor::JsonWriter json{};

			json.BeginObject();

			json.WriteFieldNameString("name");
			json.WriteValueHash(asset.name);
			if (asset.animTree) {
				json.WriteFieldNameString("animTree");
				hashutils::AddPrecomputed(hash::Hash64(asset.animTree), asset.animTree, true);
				json.WriteValueString(asset.animTree);
			}
			if (asset.csvInclude) {
				json.WriteFieldNameString("csvInclude");
				json.WriteValueHash(asset.csvInclude);
			}
			json.WriteFieldNameString("category"); json.WriteValueString(GetAICategoryName(asset.category));
			json.WriteFieldNameString("fireMode"); json.WriteValueString(GetAIWeaponFireTypeName(asset.fireMode));
			json.WriteFieldNameString("team"); json.WriteValueString(GetTeamName(asset.team));

			if (asset.species) {
				json.WriteFieldNameString("species");
				json.WriteValueHash(asset.species);
			}
			if (asset.archeType) {
				json.WriteFieldNameString("archeType");
				json.WriteValueHash(asset.archeType);
			}
			if (asset.scoreType) {
				json.WriteFieldNameString("scoreType");
				json.WriteValueHash(asset.scoreType);
			}
			if (asset.serverFlowGraph) {
				json.WriteFieldNameString("serverFlowGraph");
				json.WriteValueHash(asset.serverFlowGraph);
			}
			if (asset.clientFlowGraph) {
				json.WriteFieldNameString("clientFlowGraph");
				json.WriteValueHash(asset.clientFlowGraph);
			}
			if (asset.femaleVersion) {
				json.WriteFieldNameString("femaleVersion");
				json.WriteValueHash(asset.femaleVersion);
			}
			if (asset.movementType) {
				json.WriteFieldNameString("movementType");
				json.WriteValueHash(asset.movementType);
			}
			if (asset.spawnInfluencers[0] || asset.spawnInfluencers[1]) {
				json.WriteFieldNameString("spawnInfluencers");
				json.BeginArray();
				json.WriteValueHash(asset.spawnInfluencers[0]);
				json.WriteValueHash(asset.spawnInfluencers[1]);
				json.EndArray();
			}
			if (asset.compassIcon) {
				json.WriteFieldNameString("compassIcon");
				json.WriteValueHash(asset.compassIcon);
			}
			if (asset.properName) {
				json.WriteFieldNameString("properName");
				json.WriteValueString(fastfile::handlers::bo4::GetScrString(asset.properName));
			}

			if (asset.typeSuffix) {
				json.WriteFieldNameString("typeSuffix");
				json.WriteValueString(fastfile::handlers::bo4::GetScrString(asset.typeSuffix));
			}
			if (asset.spawnerType) {
				json.WriteFieldNameString("spawnerType");
				json.WriteValueString(fastfile::handlers::bo4::GetScrString(asset.spawnerType));
			}
			if (asset.tag1) {
				json.WriteFieldNameString("tag1");
				json.WriteValueString(fastfile::handlers::bo4::GetScrString(asset.tag1));
			}
			if (asset.tag2) {
				json.WriteFieldNameString("tag2");
				json.WriteValueString(fastfile::handlers::bo4::GetScrString(asset.tag2));
			}
			if (asset.tag3) {
				json.WriteFieldNameString("tag3");
				json.WriteValueString(fastfile::handlers::bo4::GetScrString(asset.tag3));
			}
			if (asset.weakPointsTags[0]) {
				json.WriteFieldNameString("weakPointsTags");
				json.BeginArray();
				for (ScrString_t tag : asset.weakPointsTags) {
					if (!tag) break;
					json.WriteValueString(fastfile::handlers::bo4::GetScrString(tag));
				}
				json.EndArray();
			}

			if (asset.numPrimaryWeapons) {
				json.WriteFieldNameString("primaryWeapons");
				json.BeginArray();

				for (size_t i = 0; i < asset.numPrimaryWeapons; i++) {
					json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_WEAPON, asset.primaryWeapons[i])->name);
				}

				json.EndArray();
			}
			if (asset.numCharacters) {
				json.WriteFieldNameString("character");
				json.BeginArray();

				for (size_t i = 0; i < asset.numCharacters; i++) {
					json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_CHARACTER, asset.character[i])->name);
				}

				json.EndArray();
			}
			if (asset.secondaryWeapon) {
				json.WriteFieldNameString("secondaryWeapon");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_WEAPON, asset.secondaryWeapon)->name);
			}
			if (asset.sideArm) {
				json.WriteFieldNameString("sideArm");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_WEAPON, asset.sideArm)->name);
			}
			if (asset.meleeWeapon) {
				json.WriteFieldNameString("meleeWeapon");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_WEAPON, asset.meleeWeapon)->name);
			}
			if (asset.grenadeWeapon) {
				json.WriteFieldNameString("grenadeWeapon");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_WEAPON, asset.grenadeWeapon)->name);
			}

			if (asset.aimTable) {
				json.WriteFieldNameString("aimTable");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_AIMTABLE, asset.aimTable)->name);
			}
			if (asset.shootTable) {
				json.WriteFieldNameString("shootTable");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_SHOOTTABLE, asset.shootTable)->name);
			}
			if (asset.footstepTable) {
				json.WriteFieldNameString("footstepTable");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_FOOTSTEP_TABLE, asset.footstepTable)->name);
			}
			json.WriteFieldNameString("footstepScriptCallback"); json.WriteValueBool(asset.footstepScriptCallback);
			if (asset.footstepFXTable1) {
				json.WriteFieldNameString("footstepFXTable1");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_SURFACEFX_TABLE, asset.footstepFXTable1)->name);
			}
			if (asset.footstepFXTable2) {
				json.WriteFieldNameString("footstepFXTable2");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_SURFACEFX_TABLE, asset.footstepFXTable2)->name);
			}
			if (asset.behaviorTree) {
				json.WriteFieldNameString("behaviorTree");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_BEHAVIORTREE, asset.behaviorTree)->name);
			}
			if (asset.animStateMachine) {
				json.WriteFieldNameString("animStateMachine");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_ANIMSTATEMACHINE, asset.animStateMachine)->name);
			}
			if (asset.animSelectorTable) {
				json.WriteFieldNameString("animSelectorTable");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_ANIMSELECTORTABLESET, asset.animSelectorTable)->name);
			}

			if (asset.primaryAllowedCamos[0]) {
				json.WriteFieldNameString("primaryAllowedCamos");
				json.BeginArray();

				for (const XHash& ref : asset.primaryAllowedCamos) {
					if (!ref) break;
					json.WriteValueHash(ref);
				}

				json.EndArray();
			}

			if (asset.traversalExtents) {
				json.WriteFieldNameString("traversalExtents");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, asset.traversalExtents)->name);
			}
			if (asset.randomName) {
				json.WriteFieldNameString("randomName");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, asset.randomName)->name);
			}
			if (asset.assassinationBundle) {
				json.WriteFieldNameString("assassinationBundle");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, asset.assassinationBundle)->name);
			}
			if (asset.aiFxBundle) {
				json.WriteFieldNameString("aiFxBundle");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, asset.aiFxBundle)->name);
			}
			if (asset.aiNotetrackBoneMappingBundle) {
				json.WriteFieldNameString("aiNotetrackBoneMappingBundle");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, asset.aiNotetrackBoneMappingBundle)->name);
			}
			if (asset.aivsaiMeleeBundles[0]) {
				json.WriteFieldNameString("aivsaiMeleeBundles");
				json.BeginArray();
				for (ScriptBundle* bundle : asset.aivsaiMeleeBundles) {
					if (!bundle) {
						break;
					}

					json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, bundle)->name);
				}
				json.EndArray();
			}

			if (asset.randomNamesCount) {
				json.WriteFieldNameString("randomNames");
				json.BeginArray();

				for (size_t i = 0; i < asset.randomNamesCount; i++) {
					json.WriteValueString(fastfile::handlers::bo4::GetScrString(asset.randomNames[i]));
				}

				json.EndArray();
			}

			if (asset.randomRanksCount) {
				json.WriteFieldNameString("randomRanks");
				json.BeginArray();

				for (size_t i = 0; i < asset.randomNamesCount; i++) {
					json.WriteValueString(fastfile::handlers::bo4::GetScrString(asset.randomRanks[i]));
				}

				json.EndArray();
			}
			if (asset.aiSettingsBundle) {
				json.WriteFieldNameString("aiSettingsBundle");
				json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, asset.aiSettingsBundle)->name);
			}

			json.WriteFieldNameString("ignoresDamageHitLocations"); json.WriteValueBool(asset.ignoresDamageHitLocations);
			json.WriteFieldNameString("ignoreVortices"); json.WriteValueBool(asset.ignoreVortices);
			json.WriteFieldNameString("usesCovernodes"); json.WriteValueBool(asset.usesCovernodes);
			json.WriteFieldNameString("accurateFire"); json.WriteValueBool(asset.accurateFire);
			json.WriteFieldNameString("aggressiveMode"); json.WriteValueBool(asset.aggressiveMode);
			json.WriteFieldNameString("hero"); json.WriteValueBool(asset.hero);
			json.WriteFieldNameString("matureGib"); json.WriteValueBool(asset.matureGib);
			json.WriteFieldNameString("restrictedGib"); json.WriteValueBool(asset.restrictedGib);
			json.WriteFieldNameString("allowFriendlyFire"); json.WriteValueBool(asset.allowFriendlyFire);
			json.WriteFieldNameString("fixedNode"); json.WriteValueBool(asset.fixedNode);
			json.WriteFieldNameString("isPacifist"); json.WriteValueBool(asset.isPacifist);
			json.WriteFieldNameString("threatIgnoreVehicles"); json.WriteValueBool(asset.threatIgnoreVehicles);
			json.WriteFieldNameString("proceduralTraversals"); json.WriteValueBool(asset.proceduralTraversals);
			json.WriteFieldNameString("canFlank"); json.WriteValueBool(asset.canFlank);
			json.WriteFieldNameString("burstCountMin"); json.WriteValueNumber(asset.burstCountMin);
			json.WriteFieldNameString("burstCountMax"); json.WriteValueNumber(asset.burstCountMax);
			json.WriteFieldNameString("fireIntervalmin"); json.WriteValueNumber(asset.fireIntervalmin);
			json.WriteFieldNameString("fireIntervalmax"); json.WriteValueNumber(asset.fireIntervalmax);
			json.WriteFieldNameString("physRadius"); json.WriteValueNumber(asset.physRadius);
			json.WriteFieldNameString("physHeight"); json.WriteValueNumber(asset.physHeight);
			json.WriteFieldNameString("pathEnemyFightDist"); json.WriteValueNumber(asset.pathEnemyFightDist);
			json.WriteFieldNameString("runAndGunDist"); json.WriteValueNumber(asset.runAndGunDist);
			json.WriteFieldNameString("faceEnemyDist"); json.WriteValueNumber(asset.faceEnemyDist);
			json.WriteFieldNameString("coverNodeSearchInterval"); json.WriteValueNumber(asset.coverNodeSearchInterval);
			json.WriteFieldNameString("newThreatDelay"); json.WriteValueNumber(asset.newThreatDelay);
			json.WriteFieldNameString("newPainDelay"); json.WriteValueNumber(asset.newPainDelay);
			json.WriteFieldNameString("defaultGoalRadius"); json.WriteValueNumber(asset.defaultGoalRadius);
			json.WriteFieldNameString("sightDist"); json.WriteValueNumber(asset.sightDist);
			json.WriteFieldNameString("painTolerance"); json.WriteValueNumber(asset.painTolerance);
			json.WriteFieldNameString("animMappingTableDefault"); json.WriteValueNumber(asset.animMappingTableDefault);
			json.WriteFieldNameString("engageMinDist"); json.WriteValueNumber(asset.engageMinDist);
			json.WriteFieldNameString("engageMinFalloffDist"); json.WriteValueNumber(asset.engageMinFalloffDist);
			json.WriteFieldNameString("engageMaxDist"); json.WriteValueNumber(asset.engageMaxDist);
			json.WriteFieldNameString("engageMaxFalloffDist"); json.WriteValueNumber(asset.engageMaxFalloffDist);
			json.WriteFieldNameString("shootAngleThreshold"); json.WriteValueNumber(asset.shootAngleThreshold);
			json.WriteFieldNameString("closeShootAngleThreshold"); json.WriteValueNumber(asset.closeShootAngleThreshold);
			json.WriteFieldNameString("closeShootAngleDistance"); json.WriteValueNumber(asset.closeShootAngleDistance);
			json.WriteFieldNameString("favorTraversalFactor"); json.WriteValueNumber(asset.favorTraversalFactor);
			json.WriteFieldNameString("favorSpecialTraversalFactor"); json.WriteValueNumber(asset.favorSpecialTraversalFactor);
			json.WriteFieldNameString("flankingScoreScale"); json.WriteValueNumber(asset.flankingScoreScale);
			json.WriteFieldNameString("flankingAngle"); json.WriteValueNumber(asset.flankingAngle);
			auto WriteCoverOffset = [&json](const char* name, CoverOffset& off) {
				json.WriteFieldNameString(name);
				json.BeginObject();
				json.WriteFieldNameString("xOffset"); json.WriteValueNumber(off.xOffset);
				json.WriteFieldNameString("yOffset"); json.WriteValueNumber(off.yOffset);
				json.WriteFieldNameString("yawOffset"); json.WriteValueNumber(off.yawOffset);
				json.EndObject();
			};
			WriteCoverOffset("coverCrouch", asset.coverCrouch);
			WriteCoverOffset("coverExposed", asset.coverExposed);
			WriteCoverOffset("coverLeft", asset.coverLeft);
			WriteCoverOffset("coverPillar", asset.coverPillar);
			WriteCoverOffset("coverRight", asset.coverRight);
			WriteCoverOffset("coverStand", asset.coverStand);

			json.WriteFieldNameString("steering");
			json.BeginArray();
			for (AISteering& steer : asset.steering) {
				json.BeginObject();
				json.WriteFieldNameString("turnRate"); json.WriteValueNumber(steer.turnRate);
				json.WriteFieldNameString("maxTurnRateAngle"); json.WriteValueNumber(steer.maxTurnRateAngle);
				json.WriteFieldNameString("sensorSize"); json.WriteValueNumber(steer.sensorSize);
				json.WriteFieldNameString("collisionPenalty"); json.WriteValueNumber(steer.collisionPenalty);
				json.WriteFieldNameString("dodgingPenalty"); json.WriteValueNumber(steer.dodgingPenalty);
				json.WriteFieldNameString("penetrationPenalty"); json.WriteValueNumber(steer.penetrationPenalty);
				json.WriteFieldNameString("sidednessChangingPenalty"); json.WriteValueNumber(steer.sidednessChangingPenalty);
				json.WriteFieldNameString("velocityHysteresis"); json.WriteValueNumber(steer.velocityHysteresis);
				json.EndObject();
			}
			json.EndArray();
			json.WriteFieldNameString("demoLockOn");
			json.BeginObject();
			{
				json.WriteFieldNameString("highlightDistance"); json.WriteValueNumber(asset.demoLockOn.highlightDistance);
				json.WriteFieldNameString("views");
				json.BeginArray();
				for (AIDemoLockOnView& view : asset.demoLockOn.views) {
					json.BeginObject();
					json.WriteFieldNameString("heightOffset"); json.WriteValueNumber(view.heightOffset);
					json.WriteFieldNameString("pitchMin"); json.WriteValueNumber(view.pitchMin);
					json.WriteFieldNameString("pitchMax"); json.WriteValueNumber(view.pitchMax);
					json.EndObject();
				}
				json.EndArray();
			}
			json.EndObject();
			json.WriteFieldNameString("frontQuadrantDegrees"); json.WriteValueNumber(asset.frontQuadrantDegrees);
			json.WriteFieldNameString("backQuadrantDegrees"); json.WriteValueNumber(asset.backQuadrantDegrees);
			json.WriteFieldNameString("leftQuadrantDegrees"); json.WriteValueNumber(asset.leftQuadrantDegrees);
			json.WriteFieldNameString("rightQuadrantDegrees"); json.WriteValueNumber(asset.rightQuadrantDegrees);
			json.WriteFieldNameString("thresholds");
			json.BeginArray();
			for (FaceQuadrantThreshold& threshold : asset.thresholds) {
				json.BeginObject();
				json.WriteFieldNameString("forwardDot"); json.WriteValueNumber(threshold.forwardDot);
				json.WriteFieldNameString("sideDot"); json.WriteValueNumber(threshold.sideDot);
				json.EndObject();
			}
			json.EndArray();
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / "bo4" / "source" / "tables" / "aitype"
				/ fastfile::GetCurrentContext().ffname
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset.name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump aitype {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
			/*
			outFile.replace_extension(".txt");

			utils::OutFileCE os{ outFile };
			utils::data::WriteHex(os, 0, &asset, sizeof(asset));
			*/
		}
	};

	utils::MapAdder<AiTypeWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_AITYPE };
}