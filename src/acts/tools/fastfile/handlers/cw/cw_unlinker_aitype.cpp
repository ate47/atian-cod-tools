#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_cw.hpp>

namespace {
	using namespace fastfile::handlers::cw;

	enum team_t : uint32_t
	{
		TEAM_FREE = 0x0,
		TEAM_ALLIES = 0x1,
		TEAM_AXIS = 0x2,
		TEAM_CODCASTER = 0x7f,
	};
	const char* TeamName(team_t t) {
		switch (t) {
		case TEAM_FREE: return "FREE";
		case TEAM_ALLIES: return "ALLIES";
		case TEAM_AXIS: return "AXIS";
		case TEAM_CODCASTER: return "CODCASTER";
		default: return utils::va("T(%d)", t);
		}
	}
	const char* FireTypeNames[]{
		"FULLAUTO",
		"BURST",
		"SINGLE_SHOT",
	};
#define STATICARRAY_GET_SAFE(__arr, __idx) ((__idx) < ARRAYSIZE((__arr)) ? __arr[(__idx)] : utils::va("invalid:%d", (__idx)))

	class ImplWorker : public Worker {
		using Worker::Worker;

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
				float unk04;
				float maxTurnRateAngle;
				float unk0c;
				float unk10;
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

			union AiTypeRandomName {
				ScrString_t str;
				CWXHash hash;
			};

			enum AIWeaponFireType : __int32
			{
				AI_WEAPON_FIRE_TYPE_FULLAUTO = 0x0,
				AI_WEAPON_FIRE_TYPE_BURST = 0x1,
				AI_WEAPON_FIRE_TYPE_SINGLE_SHOT = 0x2,
				AI_WEAPON_FIRE_TYPE_COUNT = 0x3,
				AI_WEAPON_FIRE_NUM_INVALID = 0x3,
			};



			struct AiType {
				CWXHash name;
				XString animTree;
				CWXHash csvInclude;
				uint64_t unk28;
				uint64_t unk30;
				uint64_t unk38;
				WeaponDef* primaryWeapons[4];
				uint32_t numPrimaryWeapons;
				float primaryAttachmentChance[7];
				eAttachment primaryBannedAttachments[64];
				CWXHash primaryAllowedCamos[100];
				uint64_t unk490;
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
				uint32_t unk4dc;
				team_t team;
				CWXHash species;
				CWXHash archeType;
				CWXHash scoreType;
				ScrString_t typeSuffix;
				ScrString_t tag1;
				ScrString_t tag2;
				ScrString_t tag3;
				ScrString_t weakPointsTags[6];
				uint64_t unk528;
				uint64_t unk530;
				uint64_t unk538;
				uint64_t unk540;
				uint64_t unk548;
				uint64_t unk550;
				uint64_t unk558;
				uint64_t unk560;
				uint64_t unk568;
				uint64_t unk570;
				uint64_t unk578;
				uint64_t unk580;
				uint64_t unk588;
				uint64_t unk590;
				uint64_t unk598;
				uint64_t unk5a0;
				uint64_t unk5a8;
				FootstepTableDef* footstepTable;
				bool footstepScriptCallback;
				SurfaceFXTableDef* footstepFXTable1;
				SurfaceFXTableDef* footstepFXTable2;
				uint64_t unk5d0;
				uint64_t unk5d8;
				uint64_t unk5e0;
				uint64_t unk5e8;
				uint64_t unk5f0;
				uint64_t unk5f8;
				uint64_t unk600;
				uint64_t unk608;
				uint64_t unk610;
				uint64_t unk618;
				uint64_t unk620;
				uint64_t unk628;
				CWXHash movementType;
				float favorTraversalFactor;
				float favorSpecialTraversalFactor;
				uint32_t unk640;
				bool ignoresDamageHitLocations;
				bool ignoreVortices;
				bool usesCovernodes;
				bool accurateFire;
				uint64_t unk648;
				uint64_t unk650;
				ScriptBundle* traversalExtents;
				uint64_t unk660;
				uint64_t unk668;
				uint64_t unk670;
				uint64_t unk678;
				uint64_t unk680;
				uint64_t unk688;
				uint64_t unk690;
				uint64_t unk698;
				uint64_t unk6a0;
				uint64_t unk6a8;
				uint64_t unk6b0;
				uint64_t unk6b8;
				uint64_t unk6c0;
				uint64_t unk6c8;
				uint64_t unk6d0;
				uint64_t unk6d8;
				AISteering steering[3];
				float unk764;
				float unk768;
				float unk76c;
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
				ScriptBundle* aiWeakpointSettings;
				uint64_t unk870;
				uint64_t unk878; // bool?
				CWXHash unk880;
				ScriptBundle* aiNames;
				ScriptBundle* aiNamesLocalized;
				bool namedHashed;
				uint64_t unk8a0;
				AiTypeRandomName randomNames[256];
				AiTypeRandomName randomRanks[32];
				ScrString_t spawnerType;
				CWXHash spawnInfluencers[2];
				GfxImage* tacticalModeIcon;
				uint64_t unk11c8;
				uint64_t unk11d0;
				uint64_t unk11d8;
				ScriptBundle* aiSettingsBundle;
				AIValidations validations;
				CWXHash unk12f0;
			};

			static_assert(sizeof(AiType) == 0x12f8);
			AiType& asset{ *(AiType*)ptr };

			BOCWJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset.name);
			json.WriteFieldValueXString("animTree", asset.animTree);

			json.WriteFieldValueScrString("typeSuffix", asset.typeSuffix);
			json.WriteFieldValueScrString("spawnerType", asset.spawnerType);
			json.WriteFieldValueScrString("tag1", asset.tag1);
			json.WriteFieldValueScrString("tag2", asset.tag2);
			json.WriteFieldValueScrString("tag3", asset.tag3);
			json.WriteFieldValueScrStringArray("weakPointsTags", ARRAYSIZE(asset.weakPointsTags), asset.weakPointsTags);


			json.WriteFieldValueXHash("species", asset.species);
			json.WriteFieldValueXHash("archeType", asset.archeType);
			json.WriteFieldValueXHash("scoreType", asset.scoreType);
			json.WriteFieldValueXHash("movementType", asset.movementType);
			json.WriteFieldValueString("fireMode", STATICARRAY_GET_SAFE(FireTypeNames, asset.fireMode));
			json.WriteFieldValueString("team", TeamName(asset.team));
			json.WriteFieldValueNumber("burstCountMin", asset.burstCountMin);
			json.WriteFieldValueNumber("burstCountMax", asset.burstCountMax);
			json.WriteFieldValueNumber("fireIntervalmin", asset.fireIntervalmin);
			json.WriteFieldValueNumber("fireIntervalmax", asset.fireIntervalmax);

			/*
			// unused?
			json.WriteFieldValueXHash("csvInclude", asset.csvInclude);
			json.WriteFieldValueUnknown("unk28", asset.unk28);
			json.WriteFieldValueUnknown("unk4dc", asset.unk4dc); 
			json.WriteFieldValueXHash("unk12f0", asset.unk12f0);

			json.WriteFieldValueUnknown("unk30", asset.unk30);
			json.WriteFieldValueUnknown("unk38", asset.unk38);
			json.WriteFieldValueUnknown("unk528", asset.unk528);
			json.WriteFieldValueUnknown("unk530", asset.unk530);
			json.WriteFieldValueUnknown("unk538", asset.unk538);
			json.WriteFieldValueUnknown("unk540", asset.unk540);
			json.WriteFieldValueUnknown("unk548", asset.unk548);
			json.WriteFieldValueUnknown("unk550", asset.unk550);
			json.WriteFieldValueUnknown("unk558", asset.unk558);
			json.WriteFieldValueUnknown("unk560", asset.unk560);
			json.WriteFieldValueUnknown("unk568", asset.unk568);
			json.WriteFieldValueUnknown("unk570", asset.unk570);
			json.WriteFieldValueUnknown("unk578", asset.unk578);
			json.WriteFieldValueUnknown("unk580", asset.unk580);
			json.WriteFieldValueUnknown("unk588", asset.unk588);
			json.WriteFieldValueUnknown("unk590", asset.unk590);
			json.WriteFieldValueUnknown("unk598", asset.unk598);
			json.WriteFieldValueUnknown("unk5a0", asset.unk5a0);
			json.WriteFieldValueUnknown("unk5a8", asset.unk5a8);

			json.WriteFieldValueUnknown("unk5d0", asset.unk5d0);
			json.WriteFieldValueUnknown("unk5d8", asset.unk5d8);
			json.WriteFieldValueUnknown("unk5e0", asset.unk5e0);
			json.WriteFieldValueUnknown("unk5e8", asset.unk5e8);
			json.WriteFieldValueUnknown("unk5f0", asset.unk5f0);
			json.WriteFieldValueUnknown("unk5f8", asset.unk5f8);
			json.WriteFieldValueUnknown("unk600", asset.unk600);
			json.WriteFieldValueUnknown("unk608", asset.unk608);
			json.WriteFieldValueUnknown("unk610", asset.unk610);
			json.WriteFieldValueUnknown("unk618", asset.unk618);
			json.WriteFieldValueUnknown("unk620", asset.unk620);
			json.WriteFieldValueUnknown("unk628", asset.unk628);
			json.WriteFieldValueUnknown("unk648", asset.unk648);
			json.WriteFieldValueUnknown("unk650", asset.unk650);

			json.WriteFieldValueUnknown("unk660", asset.unk660);
			json.WriteFieldValueUnknown("unk668", asset.unk668);
			json.WriteFieldValueUnknown("unk670", asset.unk670);
			json.WriteFieldValueUnknown("unk678", asset.unk678);
			json.WriteFieldValueUnknown("unk680", asset.unk680);
			json.WriteFieldValueUnknown("unk688", asset.unk688);
			json.WriteFieldValueUnknown("unk690", asset.unk690);
			json.WriteFieldValueUnknown("unk698", asset.unk698);
			json.WriteFieldValueUnknown("unk6a0", asset.unk6a0);
			json.WriteFieldValueUnknown("unk6a8", asset.unk6a8);
			json.WriteFieldValueUnknown("unk6b0", asset.unk6b0);
			json.WriteFieldValueUnknown("unk6b8", asset.unk6b8);
			json.WriteFieldValueUnknown("unk6c0", asset.unk6c0);
			json.WriteFieldValueUnknown("unk6c8", asset.unk6c8);
			json.WriteFieldValueUnknown("unk6d0", asset.unk6d0);
			json.WriteFieldValueUnknown("unk6d8", asset.unk6d8);
			json.WriteFieldValueUnknown("unk640", asset.unk640);
			if (asset.unk870) json.WriteFieldValueUnknown("unk870", asset.unk870); // null?
			json.WriteFieldValueNumber("unk764", asset.unk764);
			json.WriteFieldValueNumber("unk768", asset.unk768);
			json.WriteFieldValueNumber("unk76c", asset.unk76c);
			json.WriteFieldValueUnknown("unk878", asset.unk878);
			json.WriteFieldValueUnknown("unk11c8", asset.unk11c8);
			json.WriteFieldValueUnknown("unk11d0", asset.unk11d0);
			json.WriteFieldValueUnknown("unk11d8", asset.unk11d8);
			*/


			json.WriteFieldValueNumber("favorTraversalFactor", asset.favorTraversalFactor);
			json.WriteFieldValueNumber("favorSpecialTraversalFactor", asset.favorSpecialTraversalFactor);
			json.WriteFieldValueBool("ignoresDamageHitLocations", asset.ignoresDamageHitLocations);
			json.WriteFieldValueBool("ignoreVortices", asset.ignoreVortices);
			json.WriteFieldValueBool("usesCovernodes", asset.usesCovernodes);
			json.WriteFieldValueBool("accurateFire", asset.accurateFire);
			json.WriteFieldNameString("steering");
			json.BeginArray();
			for (AISteering& steer : asset.steering) {
				json.BeginObject();
				json.WriteFieldValueNumber("turnRate", steer.turnRate);
				json.WriteFieldValueNumber("unk14", steer.unk04);
				json.WriteFieldValueNumber("maxTurnRateAngle", steer.maxTurnRateAngle);
				json.WriteFieldValueNumber("unk1c", steer.unk0c);
				json.WriteFieldValueNumber("unk10", steer.unk10);
				json.WriteFieldValueNumber("sensorSize", steer.sensorSize);
				json.WriteFieldValueNumber("collisionPenalty", steer.collisionPenalty);
				json.WriteFieldValueNumber("dodgingPenalty", steer.dodgingPenalty);
				json.WriteFieldValueNumber("penetrationPenalty", steer.penetrationPenalty);
				json.WriteFieldValueNumber("sidednessChangingPenalty", steer.sidednessChangingPenalty);
				json.WriteFieldValueNumber("velocityHysteresis", steer.velocityHysteresis);
				json.EndObject();
			}
			json.EndArray();
			json.WriteFieldValueBool("canFlank", asset.canFlank);
			json.WriteFieldValueNumber("flankingScoreScale", asset.flankingScoreScale);
			json.WriteFieldValueNumber("flankingAngle", asset.flankingAngle);
			json.WriteFieldNameString("demoLockOn");
			json.BeginObject();
			{
				json.WriteFieldValueNumber("highlightDistance", asset.demoLockOn.highlightDistance);
				json.WriteFieldNameString("views");
				json.BeginArray();
				for (AIDemoLockOnView& view : asset.demoLockOn.views) {
					json.BeginObject();
					json.WriteFieldValueNumber("heightOffset", view.heightOffset);
					json.WriteFieldValueNumber("pitchMin", view.pitchMin);
					json.WriteFieldValueNumber("pitchMax", view.pitchMax);
					json.EndObject();
				}
				json.EndArray();
			}
			json.EndObject();


			json.WriteFieldValueXHash("unk880", asset.unk880);

			if (asset.primaryAllowedCamos[0]) {
				json.WriteFieldNameString("primaryAllowedCamos");
				json.BeginArray();
				for (CWXHash& ai : asset.primaryAllowedCamos) {
					if (!ai) break;
					json.WriteValueHash(ai);
				}
				json.EndArray();
			}
			if (asset.spawnInfluencers[0]) {
				json.WriteFieldNameString("spawnInfluencers");
				json.BeginArray();
				for (CWXHash& ai : asset.spawnInfluencers) {
					if (!ai) break;
					json.WriteValueHash(ai);
				}
				json.EndArray();
			}
			json.WriteFieldValueXAssetArray("primaryWeapons", cw::ASSET_TYPE_WEAPON, ARRAYSIZE(asset.primaryWeapons), &asset.primaryWeapons[0]);
			json.WriteFieldValueXAsset("secondaryWeapon", cw::ASSET_TYPE_WEAPON, asset.secondaryWeapon);
			json.WriteFieldValueXAsset("sideArm", cw::ASSET_TYPE_WEAPON, asset.sideArm);
			json.WriteFieldValueXAsset("meleeWeapon", cw::ASSET_TYPE_WEAPON, asset.meleeWeapon);
			json.WriteFieldValueXAsset("grenadeWeapon", cw::ASSET_TYPE_WEAPON, asset.grenadeWeapon);
			json.WriteFieldValueXAsset("ammoPouch", cw::ASSET_TYPE_WEAPON, asset.ammoPouch);
			json.WriteFieldValueNumber("grenadeAmmo", asset.grenadeAmmo);


			json.WriteFieldValueXAsset("traversalExtents", cw::ASSET_TYPE_SCRIPTBUNDLE, asset.traversalExtents);
			json.WriteFieldValueXAsset("aiNames", cw::ASSET_TYPE_SCRIPTBUNDLE, asset.aiNames);
			json.WriteFieldValueXAsset("aiNamesLocalized", cw::ASSET_TYPE_SCRIPTBUNDLE, asset.aiNamesLocalized);
			json.WriteFieldValueXAsset("assassinationBundle", cw::ASSET_TYPE_SCRIPTBUNDLE, asset.assassinationBundle);
			json.WriteFieldValueXAssetArray("aivsaiMeleeBundles", cw::ASSET_TYPE_SCRIPTBUNDLE, ARRAYSIZE(asset.aivsaiMeleeBundles), &asset.aivsaiMeleeBundles[0]);
			json.WriteFieldValueXAsset("aiFxBundle", cw::ASSET_TYPE_SCRIPTBUNDLE, asset.aiFxBundle);
			json.WriteFieldValueXAsset("aiNotetrackBoneMappingBundle", cw::ASSET_TYPE_SCRIPTBUNDLE, asset.aiNotetrackBoneMappingBundle);
			json.WriteFieldValueXAsset("aiWeakpointSettings", cw::ASSET_TYPE_SCRIPTBUNDLE, asset.aiWeakpointSettings);
			json.WriteFieldValueXAsset("aiSettingsBundle", cw::ASSET_TYPE_SCRIPTBUNDLE, asset.aiSettingsBundle);
			
			json.WriteFieldValueXAsset("aimTable", cw::ASSET_TYPE_AIMTABLE, asset.aimTable);
			json.WriteFieldValueXAsset("shootTable", cw::ASSET_TYPE_SHOOTTABLE, asset.shootTable);
			json.WriteFieldValueNumber("animMappingTableDefault", asset.animMappingTableDefault);
			json.WriteFieldValueXAssetArray("animMappingTables", cw::ASSET_TYPE_ANIMMAPPINGTABLE, ARRAYSIZE(asset.animMappingTables), &asset.animMappingTables[0]);
			json.WriteFieldValueXAsset("animSelectorTable", cw::ASSET_TYPE_ANIMSELECTORTABLE, asset.animSelectorTable);
			json.WriteFieldValueXAsset("animStateMachine", cw::ASSET_TYPE_ANIMSTATEMACHINE, asset.animStateMachine);
			json.WriteFieldValueXAsset("behaviorTree", cw::ASSET_TYPE_BEHAVIORTREE, asset.behaviorTree);
			json.WriteFieldValueXAsset("footstepTable", cw::ASSET_TYPE_FOOTSTEPTABLE, asset.footstepTable);
			json.WriteFieldValueBool("footstepScriptCallback", asset.footstepScriptCallback);
			json.WriteFieldValueXAsset("footstepFXTable1", cw::ASSET_TYPE_SURFACEFXTABLE, asset.footstepFXTable1);
			json.WriteFieldValueXAsset("footstepFXTable2", cw::ASSET_TYPE_SURFACEFXTABLE, asset.footstepFXTable2);
			json.WriteFieldValueXAsset("tacticalModeIcon", cw::ASSET_TYPE_IMAGE, asset.tacticalModeIcon);
			json.WriteFieldValueXAssetArray("character", cw::ASSET_TYPE_CHARACTER, ARRAYSIZE(asset.character), &asset.character[0]);

			json.WriteFieldValueBool("nameHashed", asset.namedHashed);
			if (asset.unk8a0) json.WriteFieldValueHash("unk8a0", asset.unk8a0);
			if (asset.namedHashed) {
				if (asset.randomNames[0].hash) {
					json.WriteFieldNameString("randomNames");
					json.BeginArray();
					for (AiTypeRandomName& ai : asset.randomNames) {
						if (!ai.str) break;
						json.WriteValueHash(ai.hash);
					}
					json.EndArray();
				}
				if (asset.randomRanks[0].hash) {
					json.WriteFieldNameString("randomRanks");
					json.BeginArray();
					for (AiTypeRandomName& ai : asset.randomRanks) {
						if (!ai.str) break;
						json.WriteValueHash(ai.hash);
					}
					json.EndArray();
				}
			}
			else {
				if (asset.randomNames[0].str) {
					json.WriteFieldNameString("randomNames");
					json.BeginArray();
					for (AiTypeRandomName& ai : asset.randomNames) {
						if (!ai.str) break;
						json.WriteValueString(GetScrString(ai.str));
					}
					json.EndArray();
				}
				if (asset.randomRanks[0].str) {
					json.WriteFieldNameString("randomRanks");
					json.BeginArray();
					for (AiTypeRandomName& ai : asset.randomRanks) {
						if (!ai.str) break;
						json.WriteValueString(GetScrString(ai.str));
					}
					json.EndArray();
				}
			}

			json.WriteFieldValueBool("noCosmeticBones", asset.validations.noCosmeticBones);
			json.WriteFieldValueScrStringArray("requiredBones", ARRAYSIZE(asset.validations.requiredBones), &asset.validations.requiredBones[0]);
			json.EndObject();

			std::filesystem::path outFile{
				opt.m_output / "cw" / "source" / "tables" / "aitype"
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

	utils::MapAdder<ImplWorker, XAssetType, Worker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_AITYPE };
}