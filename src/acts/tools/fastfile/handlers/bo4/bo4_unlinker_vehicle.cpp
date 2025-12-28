#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/fastfile/handlers/bo4/bo4_unlinker_scriptbundle.hpp>

namespace {
	using namespace fastfile::handlers::bo4;

	enum VehicleTypes : int16_t {
		VEH_WHEELS_4 = 0x0,
		VEH_MOTORCYCLE = 0x1,
		VEH_TANK = 0x2,
		VEH_PLANE = 0x3,
		VEH_BOAT = 0x4,
		VEH_ARTILLERY = 0x5,
		VEH_HELICOPTER = 0x6,
		NUM_VEHICLE_TYPES = 0x7,
	};

	const char* VehicleTypesNames[]{
		"wheels4",
		"motorcycle",
		"tank",
		"plane",
		"boat",
		"artillery",
		"helicopter",
	};

	const char* VehicleTypesName(VehicleTypes type) {
		if (type >= ARRAYSIZE(VehicleTypesNames)) return utils::va("<invalid:%d>", type);
		return VehicleTypesNames[type];
	}

	struct VehicleDefUnk {
		XHash unk00;
		ScrString_t unk10;
		XHash unk18;
		uint64_t unk28;
		uint64_t unk30;
		uint64_t unk38;
	};

	struct VehicleDefUnk2 {
		XHash unk00;
		XHash unk10;
		XHash unk20;
		ScrString_t unk30;
		uint64_t unk38;
		uint64_t unk40;
		uint64_t unk48;
		uint64_t unk50;
		uint64_t unk58;
		uint64_t unk60;
	};

	struct VehicleDefUnk3 {
		XHash unk00;
		uint64_t unk10;
		uint64_t unk18;
		uint64_t unk20;
		uint64_t unk28;
	};

	struct GraphFloat;

	struct VehicleDef {
		XHash name;
		VehicleTypes type;
		ScrString_t scriptVehicleType;
		XHash archeType;
		ScrString_t scoreType;
		ScrString_t playerDrivenVersion;
		ScrString_t var_409bdb86;
		XHash unk38;
		XHash spawnerType;
		XHash spawnInfluencers[2];
		uint64_t unk78;
		uint64_t unk80;
		uint64_t unk88;
		uint64_t unk90;
		uint64_t unk98;
		uint64_t unka0;
		uint64_t unka8;
		uint64_t unkb0;
		uint64_t unkb8;
		uint64_t unkc0;
		uint64_t unkc8;
		uint64_t unkd0;
		uint64_t unkd8;
		uint64_t unke0;
		uint64_t unke8;
		uint64_t unkf0;
		uint64_t unkf8;
		uint64_t unk100;
		uint64_t unk108;
		uint64_t unk110;
		uint32_t unk118;
		ScrString_t cameraTag;
		uint64_t unk120;
		uint64_t unk128;
		uint64_t unk130;
		uint64_t unk138;
		uint64_t* unk140;
		int32_t unk148;
		uint32_t unk14c;
		uint64_t unk150;
		uint64_t unk158;
		uint64_t unk160;
		uint64_t unk168;
		uint64_t unk170;
		uint64_t unk178;
		uint64_t unk180;
		uint64_t unk188;
		uint64_t unk190;
		uint64_t unk198;
		uint64_t unk1a0;
		uint64_t unk1a8;
		uint64_t unk1b0;
		uint64_t unk1b8;
		uint64_t unk1c0;
		uint64_t unk1c8;
		uint64_t unk1d0;
		uint64_t unk1d8;
		uint64_t unk1e0;
		uint64_t unk1e8;
		uint64_t unk1f0;
		uint64_t unk1f8;
		uint64_t unk200;
		uint64_t unk208;
		uint64_t unk210;
		uint64_t unk218;
		uint64_t unk220;
		uint64_t unk228;
		uint64_t unk230;
		uint64_t unk238;
		uint64_t unk240;
		uint64_t unk248;
		uint64_t unk250;
		uint64_t unk258;
		uint64_t unk260;
		uint64_t unk268;
		uint64_t unk270;
		uint64_t unk278;
		uint64_t unk280;
		uint64_t unk288;
		uint64_t unk290;
		uint64_t unk298;
		uint64_t unk2a0;
		uint64_t unk2a8;
		uint64_t unk2b0;
		uint64_t unk2b8;
		uint64_t unk2c0;
		uint64_t unk2c8;
		uint64_t unk2d0;
		uint64_t unk2d8;
		uint64_t unk2e0;
		uint64_t unk2e8;
		uint64_t unk2f0;
		uint64_t unk2f8;
		uint64_t unk300;
		uint64_t unk308;
		uint64_t unk310;
		uint64_t unk318;
		uint64_t unk320;
		uint64_t unk328;
		uint64_t unk330;
		uint64_t unk338;
		uint64_t unk340;
		uint64_t unk348;
		uint64_t unk350;
		uint64_t unk358;
		uint64_t unk360;
		uint64_t unk368;
		uint64_t unk370;
		uint64_t unk378;
		uint64_t unk380;
		uint64_t unk388;
		uint64_t unk390;
		VehicleDefUnk unk398[16];
		uint64_t unk798;
		VehicleDefUnk2 unk7a0[16];
		uint64_t unke20;
		uint64_t unke28;
		uint64_t unke30;
		VehicleDefUnk3 unke38[5];
		uint64_t unkf28;
		uint64_t unkf30;
		uint64_t unkf38;
		uint64_t unkf40;
		uint64_t unkf48;
		uint64_t unkf50;
		uint64_t unkf58;
		uint64_t unkf60;
		uint64_t unkf68;
		uint64_t unkf70;
		uint64_t unkf78;
		uint64_t unkf80;
		uint64_t unkf88;
		uint64_t unkf90;
		uint64_t unkf98;
		uint64_t unkfa0;
		uint64_t unkfa8;
		uint64_t unkfb0;
		uint64_t unkfb8;
		uint64_t unkfc0;
		uint64_t unkfc8;
		uint64_t unkfd0;
		uint64_t unkfd8;
		uint64_t unkfe0;
		uint64_t unkfe8;
		uint64_t unkff0;
		uint64_t unkff8;
		uint64_t unk1000;
		uint64_t unk1008;
		uint64_t unk1010;
		uint64_t unk1018;
		uint64_t unk1020;
		uint64_t unk1028;
		uint64_t unk1030;
		uint64_t unk1038;
		uint64_t unk1040;
		uint64_t unk1048;
		uint64_t unk1050;
		uint64_t unk1058;
		uint64_t unk1060;
		uint64_t unk1068;
		uint64_t unk1070;
		uint64_t unk1078;
		uint64_t unk1080;
		uint64_t unk1088;
		XHash sndNames[10];
		XHash sndMaterialNames[3];
		float skidSpeedMin;
		float skidSpeedMax;
		float peelSpeedMax;
		XHash futzName;
		float futzBlend;
		VehicleSoundDef* soundDef;
		uint64_t unk1190;
		uint64_t unk1198;
		XHash unk11a0;
		XHash unk11b0;
		XHash unk11c0;
		uint64_t unk11d0;
		const char* animSet;
		uint64_t unk11e0;
		uint64_t unk11e8;
		uint32_t unk11f0;
		ScrString_t unk11f4[4];
		uint64_t unk1208;
		uint64_t unk1210;
		uint64_t unk1218;
		uint64_t unk1220;
		uint64_t unk1228;
		uint64_t unk1230;
		uint64_t unk1238;
		uint64_t unk1240;
		uint64_t unk1248;
		uint32_t unk1250;
		ScrString_t driverHideTag;
		ScrString_t driverOtherHideTags[4];
		ScrString_t unk1268;
		XModel* attachmentModels[4];
		ScrString_t attachmentTags[4];
		XModel* deathAttachmentModels[4];
		ScrString_t deathAttachmentTags[4];
		uint32_t targetingImmunePerk;
		ScrString_t targetTags[4];
		vec3_t aimAssistMins;
		vec3_t aimAssistMaxs;
		ScrString_t unk12fc;
		uint64_t unk1300;
		XModel* model;
		XModel* viewModel;
		XModel* deathModel;
		XModel* enemyModel;
		float modelSwapDelay;
		FxEffectDefHandle exhaustFx;
		ScrString_t exhaustFxTag1;
		ScrString_t exhaustFxTag2;
		FxEffectDefHandle unk1340;
		ScrString_t unk1348;
		FxEffectDefHandle unk1350;
		ScrString_t unk1358;
		ScrString_t unk135c;
		VehicleFxDef* unk1360;
		VehicleFxDef* unk1368[6];
		uint64_t unk1398;
		FxEffectDefHandle deathFx;
		ScrString_t deathFxTag;
		XHash deathFxSound;
		FxEffectDefHandle unk13c0;
		ScrString_t unk13c8;
		XHash unk13d0;
		uint64_t unk13e0;
		FxEffectDefHandle unk13e8;
		ScrString_t unk13f0;
		XHash unk13f8;
		FxEffectDefHandle lightFx[4];
		ScrString_t lightFxTag[4];
		FxEffectDefHandle unk1438[4];
		ScrString_t unk1458[4];
		FxEffectDefHandle unk1468[4];
		ScrString_t unk1488[4];
		FxEffectDefHandle unk1498[4];
		ScrString_t unk14b8[4];
		uint64_t unk14c8;
		uint64_t unk14d0;
		const char* shootshock;
		uint64_t unk14e0;
		uint64_t unk14e8;
		RumbleInfo* unk14f0;
		const char* rumbletype;
		uint64_t unk1500;
		uint64_t unk1508;
		uint64_t unk1510;
		uint64_t unk1518;
		uint64_t unk1520;
		uint64_t unk1528;
		uint64_t unk1530;
		uint64_t unk1538;
		Material* compassIconMaterial;
		uint64_t unk1548;
		uint64_t unk1550;
		const char* steerAxisName;
		uint64_t steerAxis;
		const char* gasAxisName;
		int32_t gasAxis;
		const char* gasButtonName;
		int32_t gasButton;
		const char* reverseBrakeButtonName;
		int32_t reverseBrakeButton;
		const char* handBrakeButtonName;
		int32_t handBrakeButton;
		const char* attackButtonName;
		int32_t attackButton;
		const char* attackSecondaryButtonName;
		int32_t attackSecondaryButton;
		const char* boostButtonName;
		int32_t boostButton;
		const char* unk15d8;
		int32_t unk15e0;
		const char* unk15e8;
		int32_t unk15f0;
		const char* unk15f8;
		int32_t unk1600;
		const char* unk1608;
		int32_t unk1610;
		const char* unk1618;
		int32_t unk1620;
		const char* unk1628;
		int32_t unk1630;
		const char* unk1638;
		int32_t unk1640;
		const char* unk1648;
		int32_t unk1650;
		const char* unk1658;
		int32_t unk1660;
		const char* steerGraphName;
		GraphFloat* steerGraph; // linked by the game
		uint64_t* unk1678;
		int32_t unk1678_count;
		uint32_t unk1684;
		uint64_t unk1688;
		uint64_t unk1690;
		uint64_t unk1698;
		uint64_t unk16a0;
		uint64_t unk16a8;
		RumbleInfo* lightCollisionRumble;
		float heavyCollisionSpeed;
		RumbleInfo* heavyCollisionRumble;
		RumbleInfo* jumpLandingRumble;
		uint64_t unk16d0;
		uint64_t unk16d8;
		uint64_t unk16e0;
		uint64_t unk16e8;
		uint64_t unk16f0;
		uint64_t unk16f8;
		uint64_t unk1700;
		uint64_t unk1708;
		uint64_t unk1710;
		uint64_t unk1718;
		uint64_t unk1720;
		uint64_t unk1728;
		uint64_t unk1730;
		uint64_t unk1738;
		uint64_t unk1740;
		uint64_t unk1748;
		uint64_t unk1750;
		uint64_t unk1758;
		uint64_t unk1760;
		uint64_t unk1768;
		uint64_t unk1770;
		uint64_t unk1778;
		uint64_t unk1780;
		uint64_t unk1788;
		uint64_t unk1790;
		uint64_t unk1798;
		uint64_t unk17a0;
		uint64_t unk17a8;
		uint64_t unk17b0;
		uint64_t unk17b8;
		uint64_t unk17c0;
		uint64_t unk17c8;
		uint64_t unk17d0;
		uint64_t unk17d8;
		uint64_t unk17e0;
		uint64_t unk17e8;
		uint64_t unk17f0;
		uint64_t unk17f8;
		uint64_t unk1800;
		uint64_t unk1808;
		uint64_t unk1810;
		uint64_t unk1818;
		uint64_t unk1820;
		uint64_t unk1828;
		uint64_t unk1830;
		uint64_t unk1838;
		uint64_t unk1840;
		uint64_t unk1848;
		uint64_t unk1850;
		uint64_t unk1858;
		uint64_t unk1860;
		uint64_t unk1868;
		uint64_t unk1870;
		uint64_t unk1878;
		uint64_t unk1880;
		uint64_t unk1888;
		uint64_t unk1890;
		uint64_t unk1898;
		uint64_t unk18a0;
		uint64_t unk18a8;
		uint64_t unk18b0;
		uint64_t unk18b8;
		uint64_t unk18c0;
		uint64_t unk18c8;
		uint64_t unk18d0;
		uint64_t unk18d8;
		uint64_t unk18e0;
		uint64_t unk18e8;
		uint64_t unk18f0;
		uint64_t unk18f8;
		uint64_t unk1900;
		uint64_t unk1908;
		uint64_t unk1910;
		uint64_t unk1918;
		uint64_t unk1920;
		uint64_t unk1928;
		uint64_t unk1930;
		uint64_t unk1938;
		uint64_t unk1940;
		uint64_t unk1948;
		uint64_t unk1950;
		uint64_t unk1958;
		uint64_t unk1960;
		uint64_t unk1968;
		uint64_t unk1970;
		uint64_t unk1978;
		ScriptBundle* killstreakSettings;
		ScriptBundle* scriptBundleSettings;
		ScriptBundle* vehicleRidersBundle;
		ScriptBundle* vehicleRidersRobotBundle;
		ScriptBundle* vehicleRidersWarlordBundle;
		ScriptBundle* assassinationBundle;
		ScriptBundle* shrapnelSettings;
		AnimStateMachine* animStateMachine;
		AnimSelectorTableSet* animSelectorTable;
		AnimMappingTable* animMappingTable;
		uint64_t unk19d0;
		SurfaceSoundDef* vehicleFootstepTable;
		SurfaceFXTableDef* vehicleFootstepFXTable;
		DestructibleDef* destructibleDef;
		GfxImage* tacticalModeIcon;
		uint64_t unk19f8;
		uint64_t unk1a00;
		uint64_t unk1a08;
		scriptbundle::SB_ObjectsArray bundle;
	}; static_assert(sizeof(VehicleDef) == 0x1a30);

	class VehicleDefWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			VehicleDef* asset{ (VehicleDef*)ptr };

			const char* n{ hashutils::ExtractTmp("file", asset->name.name) };
			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "vehicle" / std::format("{}.json", n) };
			std::filesystem::create_directories(outFile.parent_path());

			BO4JsonWriter json{};

			LOG_INFO("Dump vehicle {}", outFile.string());
			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueString("type", VehicleTypesName(asset->type));
			
			json.WriteFieldValueScrString("scriptVehicleType", asset->scriptVehicleType);
			json.WriteFieldValueScrString("scoreType", asset->scoreType);
			json.WriteFieldValueScrString("playerDrivenVersion", asset->playerDrivenVersion);
			json.WriteFieldValueScrString(hashutils::ExtractTmp("var", 0x409bdb86), asset->var_409bdb86);
			json.WriteFieldValueScrString("cameraTag", asset->cameraTag);
			json.WriteFieldValueXHash("archeType", asset->archeType);
			json.WriteFieldValueXHash("unk38", asset->unk38);
			json.WriteFieldValueXHash("spawnerType", asset->spawnerType);
			json.WriteFieldValueXHash("spawnInfluencers1", asset->spawnInfluencers[0]);
			json.WriteFieldValueXHash("spawnInfluencers2", asset->spawnInfluencers[1]);
			json.WriteFieldValueXAsset("exhaustFx", games::bo4::pool::ASSET_TYPE_FX, asset->exhaustFx);
			json.WriteFieldValueScrString("exhaustFxTag1", asset->exhaustFxTag1);
			json.WriteFieldValueScrString("exhaustFxTag2", asset->exhaustFxTag2);

			json.WriteFieldValueXAsset("unk13c0", games::bo4::pool::ASSET_TYPE_FX, asset->unk13c0);
			json.WriteFieldValueScrString("unk13c8", asset->unk13c8);
			json.WriteFieldValueXHash("unk13d0", asset->unk13d0);
			json.WriteFieldValueXAsset("unk13e8", games::bo4::pool::ASSET_TYPE_FX, asset->unk13e8);
			json.WriteFieldValueScrString("unk13f0", asset->unk13f0);
			json.WriteFieldValueXHash("unk13f8", asset->unk13f8);
			json.WriteFieldValueXAsset("unk1340", games::bo4::pool::ASSET_TYPE_FX, asset->unk1340);
			json.WriteFieldValueXAsset("unk1350", games::bo4::pool::ASSET_TYPE_FX, asset->unk1350);


			json.WriteFieldValueXAsset("killstreakSettings", games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, asset->killstreakSettings);
			json.WriteFieldValueXAsset("scriptBundleSettings", games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, asset->scriptBundleSettings);
			json.WriteFieldValueXAsset("vehicleRidersBundle", games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, asset->vehicleRidersBundle);
			json.WriteFieldValueXAsset("vehicleRidersRobotBundle", games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, asset->vehicleRidersRobotBundle);
			json.WriteFieldValueXAsset("vehicleRidersWarlordBundle", games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, asset->vehicleRidersWarlordBundle);
			json.WriteFieldValueXAsset("shrapnelSettings", games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, asset->shrapnelSettings);
			json.WriteFieldValueXAsset("assassinationBundle", games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, asset->assassinationBundle);

			json.WriteFieldValueXAsset("deathFx", games::bo4::pool::ASSET_TYPE_FX, asset->deathFx);
			json.WriteFieldValueScrString("deathFxTag", asset->deathFxTag);
			json.WriteFieldValueXHash("deathFxSound", asset->deathFxSound);
			json.WriteFieldValueXAssetArray("lightFx", games::bo4::pool::ASSET_TYPE_FX, ARRAYSIZE(asset->lightFx), &asset->lightFx[0]);
			json.WriteFieldValueScrStringArray("lightFxTag", ARRAYSIZE(asset->lightFxTag), &asset->lightFxTag[0]);
			json.WriteFieldValueXAssetArray("unk1438", games::bo4::pool::ASSET_TYPE_FX, ARRAYSIZE(asset->unk1438), &asset->unk1438[0]);
			json.WriteFieldValueScrStringArray("unk1458", ARRAYSIZE(asset->unk1458), &asset->unk1458[0]);
			json.WriteFieldValueXAssetArray("unk1468", games::bo4::pool::ASSET_TYPE_FX, ARRAYSIZE(asset->unk1468), &asset->unk1468[0]);
			json.WriteFieldValueScrStringArray("unk1488", ARRAYSIZE(asset->unk1488), &asset->unk1488[0]);
			json.WriteFieldValueXAssetArray("unk1498", games::bo4::pool::ASSET_TYPE_FX, ARRAYSIZE(asset->unk1498), &asset->unk1498[0]);
			json.WriteFieldValueScrStringArray("unk14b8", ARRAYSIZE(asset->unk14b8), &asset->unk14b8[0]);

			json.WriteFieldValueScrString("unk1268", asset->unk1268);
			json.WriteFieldValueScrString("unk12fc", asset->unk12fc);
			json.WriteFieldValueScrString("driverHideTag", asset->driverHideTag);
			json.WriteFieldValueScrStringArray("unk11f4", ARRAYSIZE(asset->unk11f4), &asset->unk11f4[0]);
			json.WriteFieldValueScrStringArray("driverOtherHideTags", ARRAYSIZE(asset->driverOtherHideTags), &asset->driverOtherHideTags[0]);
			json.WriteFieldValueScrStringArray("attachmentTags", ARRAYSIZE(asset->attachmentTags), &asset->attachmentTags[0]);
			json.WriteFieldValueScrStringArray("deathAttachmentTags", ARRAYSIZE(asset->deathAttachmentTags), &asset->deathAttachmentTags[0]);
			json.WriteFieldValueScrStringArray("targetTags", ARRAYSIZE(asset->targetTags), &asset->targetTags[0]);


			json.WriteFieldValueXHashArray("sndNames", ARRAYSIZE(asset->sndNames), asset->sndNames);
			json.WriteFieldValueXHashArray("sndMaterialNames", ARRAYSIZE(asset->sndMaterialNames), asset->sndMaterialNames);
			json.WriteFieldNameString("skidSpeedMin"); json.WriteValueNumber(asset->skidSpeedMin);
			json.WriteFieldNameString("skidSpeedMax"); json.WriteValueNumber(asset->skidSpeedMax);
			json.WriteFieldNameString("peelSpeedMax"); json.WriteValueNumber(asset->peelSpeedMax);
			json.WriteFieldValueXHash("futzName", asset->futzName);
			json.WriteFieldValueXAsset("model", games::bo4::pool::ASSET_TYPE_XMODEL, asset->model);
			json.WriteFieldValueXAsset("viewModel", games::bo4::pool::ASSET_TYPE_XMODEL, asset->viewModel);
			json.WriteFieldValueXAsset("deathModel", games::bo4::pool::ASSET_TYPE_XMODEL, asset->deathModel);
			json.WriteFieldValueXAsset("enemyModel", games::bo4::pool::ASSET_TYPE_XMODEL, asset->enemyModel);
			json.WriteFieldValueXAssetArray("deathAttachmentModels", games::bo4::pool::ASSET_TYPE_XMODEL, ARRAYSIZE(asset->attachmentModels), &asset->attachmentModels[0]);
			json.WriteFieldValueXAssetArray("deathAttachmentModels", games::bo4::pool::ASSET_TYPE_XMODEL, ARRAYSIZE(asset->deathAttachmentModels), &asset->deathAttachmentModels[0]);
			json.WriteFieldValueXAsset("soundDef", games::bo4::pool::ASSET_TYPE_VEHICLESOUNDDEF, asset->soundDef);
			json.WriteFieldValueXAsset("compassIconMaterial", games::bo4::pool::ASSET_TYPE_MATERIAL, asset->compassIconMaterial);
			json.WriteFieldValueXAsset("lightCollisionRumble", games::bo4::pool::ASSET_TYPE_RUMBLE, asset->lightCollisionRumble);
			json.WriteFieldValueXAsset("heavyCollisionRumble", games::bo4::pool::ASSET_TYPE_RUMBLE, asset->heavyCollisionRumble);
			json.WriteFieldValueXAsset("jumpLandingRumble", games::bo4::pool::ASSET_TYPE_RUMBLE, asset->jumpLandingRumble);
			json.WriteFieldValueXAsset("animStateMachine", games::bo4::pool::ASSET_TYPE_ANIMSTATEMACHINE, asset->animStateMachine);
			json.WriteFieldValueXAsset("animSelectorTable", games::bo4::pool::ASSET_TYPE_ANIMSELECTORTABLESET, asset->animSelectorTable);
			json.WriteFieldValueXAsset("animMappingTable", games::bo4::pool::ASSET_TYPE_ANIMMAPPINGTABLE, asset->animMappingTable);
			json.WriteFieldValueXAsset("vehicleFootstepTable", games::bo4::pool::ASSET_TYPE_SURFACESOUNDDEF, asset->vehicleFootstepTable);
			json.WriteFieldValueXAsset("vehicleFootstepFXTable", games::bo4::pool::ASSET_TYPE_SURFACEFX_TABLE, asset->vehicleFootstepFXTable);
			json.WriteFieldValueXAsset("destructibleDef", games::bo4::pool::ASSET_TYPE_DESTRUCTIBLEDEF, asset->destructibleDef);
			json.WriteFieldValueXAsset("tacticalModeIcon", games::bo4::pool::ASSET_TYPE_IMAGE, asset->tacticalModeIcon);

			if (asset->animSet && *asset->animSet) {
				json.WriteFieldValueString("animSet", asset->animSet);
			}

			constexpr const char* EMPTY_BUTTON_NAME = "none";
			
			if (asset->steerAxisName && strcmp(EMPTY_BUTTON_NAME, asset->steerAxisName)) {
				json.WriteFieldValueString("steerAxisName", asset->steerAxisName);
			}
			if (asset->gasAxisName && strcmp(EMPTY_BUTTON_NAME, asset->gasAxisName)) {
				json.WriteFieldValueString("gasAxisName", asset->gasAxisName);
			}
			if (asset->gasButtonName && strcmp(EMPTY_BUTTON_NAME, asset->gasButtonName)) {
				json.WriteFieldValueString("gasButtonName", asset->gasButtonName);
			}
			if (asset->reverseBrakeButtonName && strcmp(EMPTY_BUTTON_NAME, asset->reverseBrakeButtonName)) {
				json.WriteFieldValueString("reverseBrakeButtonName", asset->reverseBrakeButtonName);
			}
			if (asset->handBrakeButtonName && strcmp(EMPTY_BUTTON_NAME, asset->handBrakeButtonName)) {
				json.WriteFieldValueString("handBrakeButtonName", asset->handBrakeButtonName);
			}
			if (asset->attackButtonName && strcmp(EMPTY_BUTTON_NAME, asset->attackButtonName)) {
				json.WriteFieldValueString("attackButtonName", asset->attackButtonName);
			}
			if (asset->attackSecondaryButtonName && strcmp(EMPTY_BUTTON_NAME, asset->attackSecondaryButtonName)) {
				json.WriteFieldValueString("attackSecondaryButtonName", asset->attackSecondaryButtonName);
			}
			if (asset->boostButtonName && strcmp(EMPTY_BUTTON_NAME, asset->boostButtonName)) {
				json.WriteFieldValueString("boostButtonName", asset->boostButtonName);
			}
			if (asset->unk15d8 && strcmp(EMPTY_BUTTON_NAME, asset->unk15d8)) {
				json.WriteFieldValueString("unk15d8", asset->unk15d8);
			}
			if (asset->unk15e8 && strcmp(EMPTY_BUTTON_NAME, asset->unk15e8)) {
				json.WriteFieldValueString("unk15e8", asset->unk15e8);
			}
			if (asset->unk15f8 && strcmp(EMPTY_BUTTON_NAME, asset->unk15f8)) {
				json.WriteFieldValueString("unk15f8", asset->unk15f8);
			}
			if (asset->unk1608 && strcmp(EMPTY_BUTTON_NAME, asset->unk1608)) {
				json.WriteFieldValueString("unk1608", asset->unk1608);
			}
			if (asset->unk1618 && strcmp(EMPTY_BUTTON_NAME, asset->unk1618)) {
				json.WriteFieldValueString("unk1618", asset->unk1618);
			}
			if (asset->unk1628 && strcmp(EMPTY_BUTTON_NAME, asset->unk1628)) {
				json.WriteFieldValueString("unk1628", asset->unk1628);
			}
			if (asset->unk1638 && strcmp(EMPTY_BUTTON_NAME, asset->unk1638)) {
				json.WriteFieldValueString("unk1638", asset->unk1638);
			}
			if (asset->unk1648 && strcmp(EMPTY_BUTTON_NAME, asset->unk1648)) {
				json.WriteFieldValueString("unk1648", asset->unk1648);
			}
			if (asset->unk1658 && strcmp(EMPTY_BUTTON_NAME, asset->unk1658)) {
				json.WriteFieldValueString("unk1658", asset->unk1658);
			}
			if (asset->steerGraphName && *asset->steerGraphName) {
				json.WriteFieldValueString("steerGraphName", asset->steerGraphName);
			}
			if (asset->shootshock && *asset->shootshock) {
				json.WriteFieldValueString("shootshock", asset->shootshock);
			}
			if (asset->rumbletype && *asset->rumbletype) {
				json.WriteFieldValueString("rumbletype", asset->rumbletype);
			}

			

			bool error{};
			json.WriteFieldNameString("bundle");
			scriptbundle::WriteObject(json, asset->bundle, error);

			if (error) return;

			json.EndObject();


			if (!json.WriteToFile(outFile) || error) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<VehicleDefWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_VEHICLEDEF };
}