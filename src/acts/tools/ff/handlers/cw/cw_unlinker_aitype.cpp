#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/ff/handlers/handler_game_cw.hpp>

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
				uint64_t unk5b0;
				uint64_t unk5b8;
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
				uint64_t unk630;
				uint64_t unk638;
				uint64_t unk640;
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
				uint64_t unk6e0;
				uint64_t unk6e8;
				uint64_t unk6f0;
				uint64_t unk6f8;
				uint64_t unk700;
				uint64_t unk708;
				uint64_t unk710;
				uint64_t unk718;
				uint64_t unk720;
				uint64_t unk728;
				uint64_t unk730;
				uint64_t unk738;
				uint64_t unk740;
				uint64_t unk748;
				uint64_t unk750;
				uint64_t unk758;
				uint64_t unk760;
				uint64_t unk768;
				uint64_t unk770;
				uint64_t unk778;
				uint64_t unk780;
				uint64_t unk788;
				uint64_t unk790;
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
				uint64_t unk878;
				uint64_t unk880;
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
			json.WriteFieldValueXHash("csvInclude", asset.csvInclude);
			json.WriteFieldValueXHash("unk12f0", asset.unk12f0);


			json.WriteFieldValueXHash("species", asset.species);
			json.WriteFieldValueXHash("archeType", asset.archeType);
			json.WriteFieldValueXHash("scoreType", asset.scoreType);
			json.WriteFieldValueString("fireMode", STATICARRAY_GET_SAFE(FireTypeNames, asset.fireMode));
			json.WriteFieldValueString("team", TeamName(asset.team));
			json.WriteFieldValueNumber("burstCountMin", asset.burstCountMin);
			json.WriteFieldValueNumber("burstCountMax", asset.burstCountMax);
			json.WriteFieldValueNumber("fireIntervalmin", asset.fireIntervalmin);
			json.WriteFieldValueNumber("fireIntervalmax", asset.fireIntervalmax);

			if (asset.unk4dc) json.WriteFieldValueNumber("unk4dc", asset.unk4dc); // unused?
			//json.WriteFieldValueHash("unk28", asset.unk28);
			//json.WriteFieldValueHash("unk30", asset.unk30);
			//json.WriteFieldValueHash("unk38", asset.unk38);
			//json.WriteFieldValueXHash("unk11c8", asset.unk11c8);
			//json.WriteFieldValueXHash("unk11d0", asset.unk11d0);
			//json.WriteFieldValueXHash("unk11d8", asset.unk11d8);

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
			json.WriteFieldValueXAsset("footstepFXTable1", cw::ASSET_TYPE_FOOTSTEPTABLE, asset.footstepFXTable1);
			json.WriteFieldValueXAsset("footstepFXTable2", cw::ASSET_TYPE_FOOTSTEPTABLE, asset.footstepFXTable2);
			json.WriteFieldValueXAsset("tacticalModeIcon", cw::ASSET_TYPE_IMAGE, asset.tacticalModeIcon);
			json.WriteFieldValueXAssetArray("character", cw::ASSET_TYPE_charACTER, ARRAYSIZE(asset.character), &asset.character[0]);

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
			LOG_INFO("Dump aitype {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, XAssetType, Worker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_AITYPE };
}