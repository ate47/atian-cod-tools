#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>
#include <tools/utils/raw_file_extractor.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	using namespace games::bo4::pool;
	class ImplWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct WeaponDefObj21329beb {
				uintptr_t model; // XModel* 
				uint64_t unk8;
				uint64_t unk10;
				uint64_t unk18;
				uint64_t unk20;
				uint64_t unk28;
				uint64_t unk30;
				uint64_t unk38;
				uint64_t unk40;
			};
			enum eModes : int {
				MODE_ZOMBIES = 0x0,
				MODE_MULTIPLAYER = 0x1,
				MODE_CAMPAIGN = 0x2,
				MODE_WARZONE = 0x3,
				MODE_COUNT = 0x4,
				MODE_INVALID = 0x4,
				MODE_FIRST = 0x0,
			};

			struct WeaponDef {
				XHash name;
				XHash baseWeapon;
				XHash displayname;
				XHash unk30;
				XHash unk40;
				XHash unk50;
				XHash unk60;
				XHash unk70;
				XHash streamkey;
				XHash unk90;
				XHash sounda0;
				XHash soundb0;
				XHash soundc0;
				XHash firesounddistant;
				XHash firesound;
				XHash firesoundplayer;
				XHash sound100;
				XHash sound110;
				XHash sound120;
				XHash sound130;
				XHash sound140;
				XHash sound150;
				XHash sound160;
				XHash sound170;
				XHash sound180;
				XHash sound190;
				XHash sound1a0;
				XHash sound1b0;
				XHash sound1c0;
				XHash sound1d0;
				XHash sound1e0;
				XHash sound1f0;
				XHash sound200;
				XHash sound210;
				XHash sound220;
				XHash sound230;
				XHash sound240;
				XHash sound250;
				XHash sound260;
				XHash sound270;
				XHash sound280;
				XHash sound290;
				XHash sound2a0;
				XHash sound2b0;
				XHash sound2c0;
				XHash sound2d0;
				XHash sound2e0;
				XHash sound2f0;
				XHash sound300;
				XHash sound310;
				XHash sound320;
				XHash sound330;
				XHash sound340;
				XHash sound350;
				XHash sound360;
				XHash sound370;
				XHash sound380;
				XHash gadgetreadysound;
				XHash var_1f7ccc3b;
				XHash sound3b0;
				XHash sound3c0;
				XHash sound3d0;
				XHash sound3e0;
				XHash sound3f0;
				XHash sound400;
				XHash sound410;
				XHash gadgetreadysoundplayer;
				XHash var_fb22040b;
				XHash sound440;
				XHash sound450;
				XHash sound460;
				XHash sound470;
				XHash sound480;
				XHash sound490;
				XHash sound4a0;
				XHash sound4b0;
				XHash sound4c0;
				XHash sound4d0;
				XHash sound4e0;
				XHash sound4f0;
				XHash sound500;
				XHash sound510;
				XHash sound520;
				XHash sound530;
				XHash sound540;
				XHash sound550;
				XHash sound560;
				XHash sound570;
				XHash sound580;
				XHash sound590;
				XHash sound5a0;
				XHash sound5b0;
				XHash sound5c0;
				XHash sound5d0;
				XHash sound5e0;
				XHash sound5f0;
				XHash sound600;
				XHash sound610;
				XHash sound620;
				XHash sound630;
				XHash sound640;
				XHash sound650;
				XHash sound660;
				XHash sound670;
				XHash sound680;
				XHash sound690;
				XHash sound6a0;
				XHash sound6b0;
				XHash sound6c0;
				XHash lockonseekersearchsound;
				XHash lockonseekerlockedsound;
				XHash lockontargetlockedsound;
				XHash lockontargetfiredonsound;
				XHash sound710;
				XHash sound720;
				XHash sound730;
				XHash sound740;
				XHash var_8a03df2b;
				XHash var_2f3ca476;
				XHash var_5c29f743;
				XHash projexplosionsound;
				XHash projexplosionsoundplayer;
				XHash projsmokestartsound;
				XHash projsmokeloopsound;
				XHash projsmokeendsound;
				XHash sound7d0;
				XHash sound7e0;
				XHash sound7f0;
				XHash sound800;
				XHash sound810;
				XHash sound820;
				XHash sound830;
				XHash sound840;
				XHash sound850;
				XHash sound860;
				XHash hitsound;
				XHash sound880;
				XHash sound890;
				uint64_t unk8a0;
				uint64_t unk8a8;
				const char* unk8b0;
				uintptr_t* unk8b8; // XAnim**
				uintptr_t* unk8c0; // XAnim**
				ScrString_t* unk8c8;
				ScrString_t* unk8d0;
				ScrString_t* unk8d8;
				uint64_t unk8e0;
				uintptr_t unk8e8; // XCam*
				uintptr_t unk8f0; // XCam*
				uintptr_t unk8f8; // XCam*
				uint64_t unk900;
				uint64_t unk908;
				uint64_t unk910;
				uint64_t unk918;
				uint64_t unk920;
				uint64_t unk928;
				uintptr_t unk930; // TagFxSet*
				vec2_t* accuracyGraphKnots[2];
				uint64_t unk948;
				uint64_t unk950;
				uint64_t unk958;
				uint64_t unk960;
				uint64_t unk968;
				int32_t var_6566504b;
				uint32_t unk974;
				uint64_t unk978;
				uint64_t unk980[4];
				uint64_t unk9a0;
				uint64_t unk9a8;
				uint64_t unk9b0;
				uint64_t unk9b8;
				uint64_t unk9c0;
				uint64_t unk9c8;
				uint64_t unk9d0;
				uint64_t unk9d8;
				uint64_t unk9e0;
				uint64_t unk9e8;
				uint64_t unk9f0;
				uint64_t unk9f8;
				uint64_t unka00;
				uint64_t unka08;
				uint64_t unka10;
				uint64_t unka18;
				uint64_t unka20;
				uintptr_t weaponCamo; // WeaponCamo*
				uint64_t unka30;
				uint64_t unka38;
				uintptr_t attachments; // WeaponAttachmentPtr*
				uintptr_t attachmentUniques; // WeaponAttachmentUniquePtr*
				uint64_t unka50;
				uintptr_t tunables1; // WeaponTunables*
				uintptr_t tunables2; // WeaponTunables*
				uintptr_t unka68; // XAnim*
				uintptr_t viewmodel; // XModel*
				uintptr_t frontendmodel; // XModel*
				uintptr_t unka80; // XModel*
				uint64_t unka88;
				uint64_t unka90;
				uint64_t unka98;
				uint64_t unkaa0;
				uint64_t unkaa8;
				uint64_t unkab0;
				uint64_t unkab8;
				uint64_t unkac0;
				uint64_t unkac8;
				uint64_t unkad0;
				uint64_t unkad8;
				uint64_t unkae0;
				uint64_t unkae8;
				uint64_t unkaf0;
				uint64_t unkaf8;
				uint64_t unkb00;
				uint64_t unkb08;
				uint64_t unkb10;
				uint64_t unkb18;
				uint64_t unkb20;
				uint64_t unkb28;
				uint64_t unkb30;
				uint64_t unkb38;
				uint64_t unkb40;
				uint64_t unkb48;
				uint64_t unkb50;
				uint64_t unkb58;
				uint64_t unkb60;
				uint64_t unkb68;
				uint64_t unkb70;
				uint64_t unkb78;
				uint64_t unkb80;
				uint64_t unkb88;
				uint64_t unkb90;
				uint64_t unkb98;
				uint64_t unkba0;
				uint64_t unkba8;
				uint64_t unkbb0;
				uint64_t unkbb8;
				uint64_t unkbc0;
				uintptr_t unkbc8; // FxEffectDefHandle 
				uintptr_t unkbd0; // FxEffectDefHandle 
				uintptr_t unkbd8; // FxEffectDefHandle 
				uintptr_t unkbe0; // FxEffectDefHandle 
				uintptr_t unkbe8; // FxEffectDefHandle 
				uintptr_t viewPersistentEffectSet; // TagFxSet*
				uintptr_t worldPersistentEffectSet; // TagFxSet*
				uintptr_t enemyDeathFxSet; // TagFxSet*
				uintptr_t unkc08; // FxEffectDefHandle 
				uintptr_t unkc10; // SharedWeaponSoundsPtr 
				uint64_t unkc18;
				uintptr_t tagFXSetFirstPerson[4]; // TagFxSet* 
				uintptr_t tagFXSetThirdPerson[4]; // TagFxSet* 
				uintptr_t gadgetIconAvailable; // GfxImageHandle 
				uintptr_t gadgetIconUnavailable; // GfxImageHandle 
				uintptr_t unkc70; // GfxImageHandle 
				uintptr_t unkc78; // GfxImageHandle 
				uintptr_t unkc80; // FxEffectDefHandle 
				uintptr_t unkc88; // FxEffectDefHandle 
				uintptr_t unkc90; // FxEffectDefHandle 
				uintptr_t unkc98; // FxEffectDefHandle 
				uintptr_t unkca0; // FxEffectDefHandle 
				uintptr_t unkca8; // FxEffectDefHandle 
				uintptr_t unkcb0; // GfxImageHandle 
				uintptr_t unkcb8; // GfxImageHandle 
				uint64_t unkcc0;
				uintptr_t worldModel; // XModel**
				uintptr_t unkcd0; // XModel**
				uintptr_t stowedmodel; // XModel**
				uintptr_t clipmodel; // XModel**
				XHash* unkce8;
				XHash* unkcf0;
				uintptr_t unkcf8; // GfxImageHandle*
				uintptr_t unkd00; // GfxImageHandle*
				uintptr_t unkd08; // GfxImageHandle*
				XHash* unkd10;
				uint64_t unkd18;
				uint64_t unkd20;
				uintptr_t unkd28; // XModel* 
				uintptr_t var_22082a57; // XModel* 
				uintptr_t unkd38; // GfxImageHandle 
				uintptr_t unkd40; // GfxImageHandle 
				uintptr_t unkd48; // GfxImageHandle 
				uintptr_t unkd50; // GfxImageHandle 
				uintptr_t unkd58; // GfxImageHandle 
				uintptr_t unkd60; // MaterialHandle 
				uintptr_t unkd68; // MaterialHandle 
				uintptr_t unkd70; // MaterialHandle 
				uintptr_t unkd78; // MaterialHandle 
				uintptr_t unkd80; // MaterialHandle 
				uintptr_t unkd88; // GfxImageHandle 
				uintptr_t unkd90; // GfxImageHandle 
				uintptr_t unkd98; // GfxImageHandle 
				uintptr_t unkda0; // GfxImageHandle 
				uintptr_t unkda8; // GfxImageHandle 
				uintptr_t unkdb0; // GfxImageHandle 
				uintptr_t unkdb8; // GfxImageHandle 
				uintptr_t unkdc0; // RumbleInfoPtr 
				uintptr_t unkdc8; // RumbleInfoPtr 
				uintptr_t unkdd0; // RumbleInfoPtr 
				uintptr_t unkdd8; // RumbleInfoPtr 
				uintptr_t unkde0; // RumbleInfoPtr 
				uintptr_t unkde8; // RumbleInfoPtr 
				uintptr_t unkdf0; // RumbleInfoPtr 
				uintptr_t unkdf8; // RumbleInfoPtr 
				uintptr_t unke00; // RumbleInfoPtr 
				uintptr_t unke08; // RumbleInfoPtr 
				uintptr_t unke10; // RumbleInfoPtr 
				uintptr_t unke18; // RumbleInfoPtr 
				uintptr_t unke20; // RumbleInfoPtr 
				uintptr_t unke28; // RumbleInfoPtr 
				uintptr_t unke30; // LaserDefPtr 
				uintptr_t unke38; // LaserDefPtr 
				uintptr_t unke40; // LaserDefPtr 
				uintptr_t flameTableFirstPerson; // FlameTablePtr 
				uintptr_t flameTableThirdPerson; // FlameTablePtr 
				uintptr_t unke58; // BeamDef* 
				uintptr_t unke60; // BeamDef* 
				uintptr_t unke68; // BeamDef* 
				uintptr_t unke70; // BeamDef* 
				uintptr_t unke78; // BeamDef* 
				uintptr_t unke80; // BeamDef* 
				uintptr_t unke88; // BeamDef* 
				uintptr_t unke90; // GfxImageHandle 
				uintptr_t unke98; // GfxImageHandle 
				uintptr_t unkea0; // GfxImageHandle 
				uintptr_t unkea8; // BallisticDesc* 
				uintptr_t unkeb0; // GfxImageHandle 
				uintptr_t weaponHeadObjective; // Objective* 
				uintptr_t crateObjective; // Objective* 
				uintptr_t projectilemodel; // XModel* 
				uintptr_t projectileModelEnemy; // XModel* 
				uintptr_t unked8; // FxEffectDefHandle 
				uintptr_t unkee0; // FxEffectDefHandle 
				uintptr_t unkee8; // FxEffectDefHandle 
				uintptr_t unkef0; // FxEffectDefHandle 
				uintptr_t var_4bcd08b0; // XModel* 
				uintptr_t unkf00; // FxEffectDefHandle 
				uintptr_t unkf08; // FxEffectDefHandle 
				uintptr_t unkf10; // FxEffectDefHandle 
				uintptr_t unkf18; // FxEffectDefHandle 
				uintptr_t underwaterExplosionSurfaceFX[3]; // FxEffectDefHandle 
				uintptr_t unkf38; // FxEffectDefHandle 
				uintptr_t unkf40; // FxEffectDefHandle 
				uintptr_t unkf48; // FxEffectDefHandle 
				uintptr_t unkf50; // FxEffectDefHandle 
				uintptr_t unkf58; // FxEffectDefHandle 
				uintptr_t unkf60; // FxEffectDefHandle 
				uintptr_t unkf68; // FxEffectDefHandle 
				uintptr_t unkf70; // FxEffectDefHandle 
				uintptr_t var_96850284; // TagFxSet* 
				uintptr_t var_26f68e75; // TagFxSet* 
				uintptr_t unkf88; // FxEffectDefHandle 
				uintptr_t unkf90; // FxEffectDefHandle 
				uintptr_t unkf98; // FxEffectDefHandle 
				uintptr_t unkfa0; // FxEffectDefHandle 
				uint64_t unkfa8;
				uint64_t unkfb0;
				uintptr_t unkfb8; // FxImpactTablePtr 
				uintptr_t unkfc0; // FxImpactTablePtr 
				uintptr_t unkfc8; // FxImpactTablePtr 
				uintptr_t unkfd0; // FxImpactTablePtr 
				uintptr_t unkfd8; // FxImpactTablePtr 
				uintptr_t unkfe0; // SoundsImpactTable* 
				uintptr_t unkfe8; // SoundsImpactTable* 
				uintptr_t unkff0; // SoundsImpactTable* 
				uintptr_t unkff8; // SoundsImpactTable* 
				uintptr_t unk1000; // SoundsImpactTable* 
				uintptr_t unk1008; // ScriptBundle* 
				uintptr_t unk1010; // ScriptBundle* 
				uintptr_t unk1018; // ScriptBundle* 
				uintptr_t customsettings; // ScriptBundle* 
				uintptr_t shrapnelsettings; // ScriptBundle* 
				uintptr_t var_2e4a8800; // ScriptBundle* 
				uintptr_t unk1038; // GfxImageHandle 
				uintptr_t unk1040; // GfxImageHandle 
				uintptr_t unk1048; // GfxImageHandle 
				uintptr_t var_8456d4d; // ScriptBundle* 
				uint64_t unk1058;
				uint64_t unk1060;
				uint64_t unk1068;
				uint64_t unk1070;
				uint64_t unk1078;
				uint64_t unk1080;
				uint64_t unk1088;
				uint64_t unk1090;
				uint64_t unk1098;
				uint64_t unk10a0;
				uint64_t unk10a8;
				uint64_t unk10b0;
				uint64_t unk10b8;
				uint64_t unk10c0;
				uint64_t unk10c8;
				uint64_t unk10d0;
				uint64_t unk10d8;
				uint64_t unk10e0;
				uint64_t unk10e8;
				uint64_t unk10f0;
				uint64_t unk10f8;
				uint64_t unk1100;
				uint64_t unk1108;
				uint64_t unk1110;
				uint64_t unk1118;
				uint64_t unk1120;
				uint64_t unk1128;
				uint64_t unk1130;
				uint64_t unk1138;
				uint64_t unk1140;
				uint64_t unk1148;
				uint64_t unk1150;
				uint64_t unk1158;
				uint64_t unk1160;
				uint64_t unk1168;
				uint64_t unk1170;
				uint64_t unk1178;
				uint32_t unk1180;
				ScrString_t unk1184;
				ScrString_t unk1188;
				ScrString_t unk118c;
				ScrString_t unk1190;
				ScrString_t unk1194;
				ScrString_t unk1198;
				ScrString_t unk119c;
				ScrString_t unk11a0;
				ScrString_t unk11a4;
				ScrString_t unk11a8;
				ScrString_t unk11ac;
				ScrString_t unk11b0;
				ScrString_t unk11b4;
				uint64_t unk11b8;
				uint64_t unk11c0;
				uint64_t unk11c8;
				uint64_t unk11d0;
				uint64_t unk11d8;
				uint64_t unk11e0;
				uint64_t unk11e8;
				uint64_t unk11f0;
				uint64_t unk11f8;
				uint64_t unk1200;
				uint64_t unk1208;
				uint64_t unk1210;
				uint64_t unk1218;
				uint64_t unk1220;
				uint64_t unk1228;
				uint64_t unk1230;
				uint64_t unk1238;
				uint64_t unk1240;
				uint64_t unk1248;
				uint64_t unk1250;
				uint64_t unk1258;
				uint64_t unk1260;
				uint64_t unk1268;
				uint64_t unk1270;
				uint64_t unk1278;
				uint64_t unk1280;
				uint64_t unk1288;
				uint64_t unk1290;
				uint64_t unk1298;
				uint64_t unk12a0;
				uint64_t unk12a8;
				uint64_t unk12b0;
				uint64_t unk12b8;
				uint64_t unk12c0;
				uint64_t unk12c8;
				uint64_t unk12d0;
				uint64_t unk12d8;
				uint64_t unk12e0;
				uint64_t unk12e8;
				uint64_t unk12f0;
				uint64_t unk12f8;
				uint32_t unk1300;
				float weaponheadobjectiveheight;
				uint64_t unk1308;
				uint64_t unk1310;
				uint64_t unk1318;
				uint64_t unk1320;
				uint64_t unk1328;
				uint64_t unk1330;
				uint64_t unk1338;
				eModes sessionMode;
				uint32_t unk1344;
				uint64_t unk1348;
				int32_t var_5b73038c;
				uint32_t unk1354;
				int itemIndex;
				uint32_t unk135c;
				uint64_t unk1360;
				uint32_t unk1368;
				int32_t weaponstarthitpoints;
				uint64_t unk1370;
				int32_t soundrattlerangemin;
				int32_t soundrattlerangemax;
				uint32_t unk1380;
				int accuracyGraphKnotCount[2];
				uint64_t unk1390;
				uint64_t unk1398;
				uint64_t unk13a0;
				uint64_t unk13a8;
				uint32_t unk13b0;
				uint32_t grenadeweapon;
				uint32_t unk13b8;
				uint32_t dualwieldweapon;
				uint32_t unk13c0;
				uint32_t dualWieldWeaponIndex;
				uint32_t altWeaponIndex;
				uint32_t unk13cc;
				uint32_t unk13d0;
				byte var_50d2316b;
				byte var_83b1dc1;
				uint16_t unk13d6;
				byte unk13d8;
				bool bDrawOffhandModelInHand;
				uint16_t unk13da;
				bool lockonseekersearchsoundloops;
				bool lockonseekerlockedsoundloops;
				bool lockontargetlockedsoundloops;
				bool lockontargetfiredonsoundloops;
				uint32_t unk13e0;
				byte var_965cc0b3;
				byte unk13e5;
				uint16_t unk13e6;
				uint16_t unk13e8;
				byte forcedamageshellshockandrumble;
				byte unk13eb;
				uint32_t unk13ec;
				uint32_t unk13f0;
				byte var_2cf49821;
				byte var_d2751f9d;
				byte var_554be9f7;
				byte unk13f7;
				uint64_t unk13f8;
				uint64_t unk1400;
				uint64_t unk1408;
				uint64_t unk1410;
				uint64_t unk1418;
				uint64_t unk1420;
				uint64_t unk1428;
				WeaponDefObj21329beb var_21329beb[4];
			};
			static_assert(sizeof(WeaponDef) == 0x1550);

			WeaponDef* asset{ (WeaponDef*)ptr };

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "weapon" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name.name)) };
			std::filesystem::create_directories(outFile.parent_path());

			tool::utils::raw_file_extractor::JsonWriter json{};

			LOG_INFO("Dump weapon {}", outFile.string());

			json.BeginObject();
			{
				auto AddXHash = [&json](const char* name, uint64_t hash, bool onlyValue = false) {
					if (hash) {
						if (!onlyValue) {
							json.WriteFieldNameString(name);
						}
						json.WriteValueHash(hash);
					}
					else if (onlyValue) {
						json.WriteValueHash(hash);
					}
				};
				auto AddXAssetRef = [&json, &AddXHash](const char* name, XAssetType type, uintptr_t ptr, bool onlyValue = false) {
					if (!ptr) return; // ignore
					if ((ptr & 0xF000000000000000ull) || (ptr < 0x10000)) {
						// ref, error

						json.WriteFieldNameString(name);
						json.WriteValueString(utils::va("<invalidref:0x%llx>", ptr));
						return;
					}

					size_t off{ GetAssetNameOffset(type) };

					XHash& hash{ *reinterpret_cast<XHash*>(ptr + off) };
					AddXHash(name, hash, onlyValue);
				};
				AddXHash("name", asset->name);
				AddXHash("baseWeapon", asset->baseWeapon);
				AddXHash("displayname", asset->displayname);
				AddXHash("unk30", asset->unk30);
				AddXHash("unk40", asset->unk40);
				AddXHash("unk50", asset->unk50);
				AddXHash("unk60", asset->unk60);
				AddXHash("unk70", asset->unk70);
				AddXHash("streamkey", asset->streamkey);
				AddXHash("unk90", asset->unk90);
				AddXHash("sounda0", asset->sounda0);
				AddXHash("soundb0", asset->soundb0);
				AddXHash("soundc0", asset->soundc0);
				AddXHash("firesounddistant", asset->firesounddistant);
				AddXHash("firesound", asset->firesound);
				AddXHash("firesoundplayer", asset->firesoundplayer);
				AddXHash("sound100", asset->sound100);
				AddXHash("sound110", asset->sound110);
				AddXHash("sound120", asset->sound120);
				AddXHash("sound130", asset->sound130);
				AddXHash("sound140", asset->sound140);
				AddXHash("sound150", asset->sound150);
				AddXHash("sound160", asset->sound160);
				AddXHash("sound170", asset->sound170);
				AddXHash("sound180", asset->sound180);
				AddXHash("sound190", asset->sound190);
				AddXHash("sound1a0", asset->sound1a0);
				AddXHash("sound1b0", asset->sound1b0);
				AddXHash("sound1c0", asset->sound1c0);
				AddXHash("sound1d0", asset->sound1d0);
				AddXHash("sound1e0", asset->sound1e0);
				AddXHash("sound1f0", asset->sound1f0);
				AddXHash("sound200", asset->sound200);
				AddXHash("sound210", asset->sound210);
				AddXHash("sound220", asset->sound220);
				AddXHash("sound230", asset->sound230);
				AddXHash("sound240", asset->sound240);
				AddXHash("sound250", asset->sound250);
				AddXHash("sound260", asset->sound260);
				AddXHash("sound270", asset->sound270);
				AddXHash("sound280", asset->sound280);
				AddXHash("sound290", asset->sound290);
				AddXHash("sound2a0", asset->sound2a0);
				AddXHash("sound2b0", asset->sound2b0);
				AddXHash("sound2c0", asset->sound2c0);
				AddXHash("sound2d0", asset->sound2d0);
				AddXHash("sound2e0", asset->sound2e0);
				AddXHash("sound2f0", asset->sound2f0);
				AddXHash("sound300", asset->sound300);
				AddXHash("sound310", asset->sound310);
				AddXHash("sound320", asset->sound320);
				AddXHash("sound330", asset->sound330);
				AddXHash("sound340", asset->sound340);
				AddXHash("sound350", asset->sound350);
				AddXHash("sound360", asset->sound360);
				AddXHash("sound370", asset->sound370);
				AddXHash("sound380", asset->sound380);
				AddXHash("gadgetreadysound", asset->gadgetreadysound);
				AddXHash("var_1f7ccc3b", asset->var_1f7ccc3b);
				AddXHash("sound3b0", asset->sound3b0);
				AddXHash("sound3c0", asset->sound3c0);
				AddXHash("sound3d0", asset->sound3d0);
				AddXHash("sound3e0", asset->sound3e0);
				AddXHash("sound3f0", asset->sound3f0);
				AddXHash("sound400", asset->sound400);
				AddXHash("sound410", asset->sound410);
				AddXHash("gadgetreadysoundplayer", asset->gadgetreadysoundplayer);
				AddXHash("var_fb22040b", asset->var_fb22040b);
				AddXHash("sound440", asset->sound440);
				AddXHash("sound450", asset->sound450);
				AddXHash("sound460", asset->sound460);
				AddXHash("sound470", asset->sound470);
				AddXHash("sound480", asset->sound480);
				AddXHash("sound490", asset->sound490);
				AddXHash("sound4a0", asset->sound4a0);
				AddXHash("sound4b0", asset->sound4b0);
				AddXHash("sound4c0", asset->sound4c0);
				AddXHash("sound4d0", asset->sound4d0);
				AddXHash("sound4e0", asset->sound4e0);
				AddXHash("sound4f0", asset->sound4f0);
				AddXHash("sound500", asset->sound500);
				AddXHash("sound510", asset->sound510);
				AddXHash("sound520", asset->sound520);
				AddXHash("sound530", asset->sound530);
				AddXHash("sound540", asset->sound540);
				AddXHash("sound550", asset->sound550);
				AddXHash("sound560", asset->sound560);
				AddXHash("sound570", asset->sound570);
				AddXHash("sound580", asset->sound580);
				AddXHash("sound590", asset->sound590);
				AddXHash("sound5a0", asset->sound5a0);
				AddXHash("sound5b0", asset->sound5b0);
				AddXHash("sound5c0", asset->sound5c0);
				AddXHash("sound5d0", asset->sound5d0);
				AddXHash("sound5e0", asset->sound5e0);
				AddXHash("sound5f0", asset->sound5f0);
				AddXHash("sound600", asset->sound600);
				AddXHash("sound610", asset->sound610);
				AddXHash("sound620", asset->sound620);
				AddXHash("sound630", asset->sound630);
				AddXHash("sound640", asset->sound640);
				AddXHash("sound650", asset->sound650);
				AddXHash("sound660", asset->sound660);
				AddXHash("sound670", asset->sound670);
				AddXHash("sound680", asset->sound680);
				AddXHash("sound690", asset->sound690);
				AddXHash("sound6a0", asset->sound6a0);
				AddXHash("sound6b0", asset->sound6b0);
				AddXHash("sound6c0", asset->sound6c0);
				AddXHash("lockonseekersearchsound", asset->lockonseekersearchsound);
				AddXHash("lockonseekerlockedsound", asset->lockonseekerlockedsound);
				AddXHash("lockontargetlockedsound", asset->lockontargetlockedsound);
				AddXHash("lockontargetfiredonsound", asset->lockontargetfiredonsound);
				AddXHash("sound710", asset->sound710);
				AddXHash("sound720", asset->sound720);
				AddXHash("sound730", asset->sound730);
				AddXHash("sound740", asset->sound740);
				AddXHash("var_8a03df2b", asset->var_8a03df2b);
				AddXHash("var_2f3ca476", asset->var_2f3ca476);
				AddXHash("var_5c29f743", asset->var_5c29f743);
				AddXHash("projexplosionsound", asset->projexplosionsound);
				AddXHash("projexplosionsoundplayer", asset->projexplosionsoundplayer);
				AddXHash("projsmokestartsound", asset->projsmokestartsound);
				AddXHash("projsmokeloopsound", asset->projsmokeloopsound);
				AddXHash("projsmokeendsound", asset->projsmokeendsound);
				AddXHash("sound7d0", asset->sound7d0);
				AddXHash("sound7e0", asset->sound7e0);
				AddXHash("sound7f0", asset->sound7f0);
				AddXHash("sound800", asset->sound800);
				AddXHash("sound810", asset->sound810);
				AddXHash("sound820", asset->sound820);
				AddXHash("sound830", asset->sound830);
				AddXHash("sound840", asset->sound840);
				AddXHash("sound850", asset->sound850);
				AddXHash("sound860", asset->sound860);
				AddXHash("hitsound", asset->hitsound);
				AddXHash("sound880", asset->sound880);
				AddXHash("sound890", asset->sound890);

				AddXAssetRef("tunables1", ASSET_TYPE_WEAPON_TUNABLES, asset->tunables1);
				AddXAssetRef("tunables2", ASSET_TYPE_WEAPON_TUNABLES, asset->tunables2);
				AddXAssetRef("weaponCamo", ASSET_TYPE_WEAPON_CAMO, asset->weaponCamo);
				AddXAssetRef("xcam_8e8", ASSET_TYPE_XCAM, asset->unk8e8);
				AddXAssetRef("xcam_8f0", ASSET_TYPE_XCAM, asset->unk8f0);
				AddXAssetRef("xcam_8f8", ASSET_TYPE_XCAM, asset->unk8f8);
				AddXAssetRef("attachments", ASSET_TYPE_ATTACHMENT, asset->attachments);
				AddXAssetRef("attachmentUniques", ASSET_TYPE_ATTACHMENT_UNIQUE, asset->attachmentUniques);
				AddXAssetRef("xanim_a68", ASSET_TYPE_ATTACHMENT_UNIQUE, asset->unka68);
				AddXAssetRef("viewmodel", ASSET_TYPE_XMODEL, asset->viewmodel);
				AddXAssetRef("frontendmodel", ASSET_TYPE_XMODEL, asset->frontendmodel);
				AddXAssetRef("model_a80", ASSET_TYPE_XMODEL, asset->unka80);

				AddXAssetRef("fx_bc8", ASSET_TYPE_FX, asset->unkbc8);
				AddXAssetRef("fx_bd0", ASSET_TYPE_FX, asset->unkbd0);
				AddXAssetRef("fx_bd8", ASSET_TYPE_FX, asset->unkbd8);
				AddXAssetRef("fx_be0", ASSET_TYPE_FX, asset->unkbe0);
				AddXAssetRef("fx_be8", ASSET_TYPE_FX, asset->unkbe8);

				AddXAssetRef("viewPersistentEffectSet", ASSET_TYPE_TAGFX, asset->viewPersistentEffectSet);
				AddXAssetRef("worldPersistentEffectSet", ASSET_TYPE_TAGFX, asset->worldPersistentEffectSet);
				AddXAssetRef("enemyDeathFxSet", ASSET_TYPE_TAGFX, asset->enemyDeathFxSet);
				AddXAssetRef("fx_c08", ASSET_TYPE_FX, asset->unkc08);
				AddXAssetRef("sharedWeaponSounds_c08", ASSET_TYPE_SHAREDWEAPONSOUNDS, asset->unkc10);
				json.WriteFieldNameString("tagFXSetFirstPerson");
				json.BeginArray();
				for (size_t i = 0; i < 4; i++) {
					AddXAssetRef("", ASSET_TYPE_TAGFX, asset->tagFXSetFirstPerson[i], true);
				}
				json.EndArray();
				json.WriteFieldNameString("tagFXSetThirdPerson");
				json.BeginArray();
				for (size_t i = 0; i < 4; i++) {
					AddXAssetRef("", ASSET_TYPE_TAGFX, asset->tagFXSetThirdPerson[i], true);
				}
				json.EndArray();
				AddXAssetRef("gadgetIconAvailable", ASSET_TYPE_IMAGE, asset->gadgetIconAvailable);
				AddXAssetRef("gadgetIconUnavailable", ASSET_TYPE_IMAGE, asset->gadgetIconUnavailable);
				AddXAssetRef("image_c70", ASSET_TYPE_IMAGE, asset->unkc70);
				AddXAssetRef("image_c78", ASSET_TYPE_IMAGE, asset->unkc78);
				AddXAssetRef("fx_c80", ASSET_TYPE_FX, asset->unkc80);
				AddXAssetRef("fx_c88", ASSET_TYPE_FX, asset->unkc88);
				AddXAssetRef("fx_c90", ASSET_TYPE_FX, asset->unkc90);
				AddXAssetRef("fx_c98", ASSET_TYPE_FX, asset->unkc98);
				AddXAssetRef("fx_ca0", ASSET_TYPE_FX, asset->unkca0);
				AddXAssetRef("fx_ca8", ASSET_TYPE_FX, asset->unkca8);
				AddXAssetRef("image_cb0", ASSET_TYPE_IMAGE, asset->unkcb0);
				AddXAssetRef("image_cb8", ASSET_TYPE_IMAGE, asset->unkcb8);
				uintptr_t worldModel; // XModel**
				uintptr_t unkcd0; // XModel**
				uintptr_t stowedmodel; // XModel**
				uintptr_t clipmodel; // XModel**
				uintptr_t unkcf8; // GfxImageHandle*
				uintptr_t unkd00; // GfxImageHandle*
				uintptr_t unkd08; // GfxImageHandle*
				AddXAssetRef("unkd28", ASSET_TYPE_XMODEL, asset->unkd28);
				AddXAssetRef("var_22082a57", ASSET_TYPE_XMODEL, asset->var_22082a57);
				AddXAssetRef("image_d38", ASSET_TYPE_IMAGE, asset->unkd38);
				AddXAssetRef("image_d40", ASSET_TYPE_IMAGE, asset->unkd40);
				AddXAssetRef("image_d48", ASSET_TYPE_IMAGE, asset->unkd48);
				AddXAssetRef("image_d50", ASSET_TYPE_IMAGE, asset->unkd50);
				AddXAssetRef("image_d58", ASSET_TYPE_IMAGE, asset->unkd58);
				AddXAssetRef("material_d60", ASSET_TYPE_MATERIAL, asset->unkd60);
				AddXAssetRef("material_d68", ASSET_TYPE_MATERIAL, asset->unkd68);
				AddXAssetRef("material_d70", ASSET_TYPE_MATERIAL, asset->unkd70);
				AddXAssetRef("material_d78", ASSET_TYPE_MATERIAL, asset->unkd78);
				AddXAssetRef("material_d80", ASSET_TYPE_MATERIAL, asset->unkd80);
				AddXAssetRef("image_d88", ASSET_TYPE_IMAGE, asset->unkd88);
				AddXAssetRef("image_d90", ASSET_TYPE_IMAGE, asset->unkd90);
				AddXAssetRef("image_d98", ASSET_TYPE_IMAGE, asset->unkd98);
				AddXAssetRef("image_da0", ASSET_TYPE_IMAGE, asset->unkda0);
				AddXAssetRef("image_da8", ASSET_TYPE_IMAGE, asset->unkda8);
				AddXAssetRef("image_db0", ASSET_TYPE_IMAGE, asset->unkdb0);
				AddXAssetRef("image_db8", ASSET_TYPE_IMAGE, asset->unkdb8);
				AddXAssetRef("rumble_dc0", ASSET_TYPE_RUMBLE, asset->unkdc0);
				AddXAssetRef("rumble_dc8", ASSET_TYPE_RUMBLE, asset->unkdc8);
				AddXAssetRef("rumble_dd0", ASSET_TYPE_RUMBLE, asset->unkdd0);
				AddXAssetRef("rumble_dd8", ASSET_TYPE_RUMBLE, asset->unkdd8);
				AddXAssetRef("rumble_de0", ASSET_TYPE_RUMBLE, asset->unkde0);
				AddXAssetRef("rumble_de8", ASSET_TYPE_RUMBLE, asset->unkde8);
				AddXAssetRef("rumble_df0", ASSET_TYPE_RUMBLE, asset->unkdf0);
				AddXAssetRef("rumble_df8", ASSET_TYPE_RUMBLE, asset->unkdf8);
				AddXAssetRef("rumble_e00", ASSET_TYPE_RUMBLE, asset->unke00);
				AddXAssetRef("rumble_e08", ASSET_TYPE_RUMBLE, asset->unke08);
				AddXAssetRef("rumble_e10", ASSET_TYPE_RUMBLE, asset->unke10);
				AddXAssetRef("rumble_e18", ASSET_TYPE_RUMBLE, asset->unke18);
				AddXAssetRef("rumble_e20", ASSET_TYPE_RUMBLE, asset->unke20);
				AddXAssetRef("rumble_e28", ASSET_TYPE_RUMBLE, asset->unke28);
				AddXAssetRef("laser_e30", ASSET_TYPE_LASER, asset->unke30);
				AddXAssetRef("laser_e38", ASSET_TYPE_LASER, asset->unke38);
				AddXAssetRef("laser_e40", ASSET_TYPE_LASER, asset->unke40);
				AddXAssetRef("flameTableFirstPerson", ASSET_TYPE_FLAMETABLE, asset->flameTableFirstPerson);
				AddXAssetRef("flameTableThirdPerson", ASSET_TYPE_FLAMETABLE, asset->flameTableThirdPerson);
				AddXAssetRef("beam_e58", ASSET_TYPE_BEAM, asset->unke58);
				AddXAssetRef("beam_e60", ASSET_TYPE_BEAM, asset->unke60);
				AddXAssetRef("beam_e68", ASSET_TYPE_BEAM, asset->unke68);
				AddXAssetRef("beam_e70", ASSET_TYPE_BEAM, asset->unke70);
				AddXAssetRef("beam_e78", ASSET_TYPE_BEAM, asset->unke78);
				AddXAssetRef("beam_e80", ASSET_TYPE_BEAM, asset->unke80);
				AddXAssetRef("beam_e88", ASSET_TYPE_BEAM, asset->unke88);
				AddXAssetRef("image_e90", ASSET_TYPE_IMAGE, asset->unke90);
				AddXAssetRef("image_e98", ASSET_TYPE_IMAGE, asset->unke98);
				AddXAssetRef("image_ea0", ASSET_TYPE_IMAGE, asset->unkea0);
				AddXAssetRef("ballisticDesc_ea8", ASSET_TYPE_BALLISTICDESC, asset->unkea8);
				AddXAssetRef("image_eb0", ASSET_TYPE_IMAGE, asset->unkeb0);
				AddXAssetRef("weaponHeadObjective", ASSET_TYPE_OBJECTIVE, asset->weaponHeadObjective);
				AddXAssetRef("crateObjective", ASSET_TYPE_OBJECTIVE, asset->crateObjective);
				AddXAssetRef("projectilemodel", ASSET_TYPE_XMODEL, asset->projectilemodel);
				AddXAssetRef("projectileModelEnemy", ASSET_TYPE_XMODEL, asset->projectileModelEnemy);
				AddXAssetRef("fx_ed8", ASSET_TYPE_FX, asset->unked8);
				AddXAssetRef("fx_ee0", ASSET_TYPE_FX, asset->unkee0);
				AddXAssetRef("fx_ee8", ASSET_TYPE_FX, asset->unkee8);
				AddXAssetRef("fx_ef0", ASSET_TYPE_FX, asset->unkef0);
				AddXAssetRef("var_4bcd08b0", ASSET_TYPE_XMODEL, asset->var_4bcd08b0);
				AddXAssetRef("fx_f00", ASSET_TYPE_FX, asset->unkf00);
				AddXAssetRef("fx_f08", ASSET_TYPE_FX, asset->unkf08);
				AddXAssetRef("fx_f10", ASSET_TYPE_FX, asset->unkf10);
				AddXAssetRef("fx_f18", ASSET_TYPE_FX, asset->unkf18);
				json.WriteFieldNameString("underwaterExplosionSurfaceFX");
				json.BeginArray();
				for (size_t i = 0; i < 3; i++) {
					AddXAssetRef("", ASSET_TYPE_FX, asset->underwaterExplosionSurfaceFX[i], true);
				}
				json.EndArray();
				AddXAssetRef("fx_f38", ASSET_TYPE_FX, asset->unkf38);
				AddXAssetRef("fx_f40", ASSET_TYPE_FX, asset->unkf40);
				AddXAssetRef("fx_f48", ASSET_TYPE_FX, asset->unkf48);
				AddXAssetRef("fx_f50", ASSET_TYPE_FX, asset->unkf50);
				AddXAssetRef("fx_f58", ASSET_TYPE_FX, asset->unkf58);
				AddXAssetRef("fx_f60", ASSET_TYPE_FX, asset->unkf60);
				AddXAssetRef("fx_f68", ASSET_TYPE_FX, asset->unkf68);
				AddXAssetRef("fx_f70", ASSET_TYPE_FX, asset->unkf70);
				AddXAssetRef("var_96850284", ASSET_TYPE_TAGFX, asset->var_96850284);
				AddXAssetRef("var_26f68e75", ASSET_TYPE_TAGFX, asset->var_26f68e75);
				AddXAssetRef("fx_f88", ASSET_TYPE_FX, asset->unkf88);
				AddXAssetRef("fx_f90", ASSET_TYPE_FX, asset->unkf90);
				AddXAssetRef("fx_f98", ASSET_TYPE_FX, asset->unkf98);
				AddXAssetRef("fx_fa0", ASSET_TYPE_FX, asset->unkfa0);
				uintptr_t unkfb8; // FxImpactTablePtr 
				uintptr_t unkfc0; // FxImpactTablePtr 
				uintptr_t unkfc8; // FxImpactTablePtr 
				uintptr_t unkfd0; // FxImpactTablePtr 
				uintptr_t unkfd8; // FxImpactTablePtr 
				uintptr_t unkfe0; // SoundsImpactTable* 
				uintptr_t unkfe8; // SoundsImpactTable* 
				uintptr_t unkff0; // SoundsImpactTable* 
				uintptr_t unkff8; // SoundsImpactTable* 
				uintptr_t unk1000; // SoundsImpactTable* 
				AddXAssetRef("scriptbundle_1008", ASSET_TYPE_SCRIPTBUNDLE, asset->unk1008);
				AddXAssetRef("scriptbundle_1010", ASSET_TYPE_SCRIPTBUNDLE, asset->unk1010);
				AddXAssetRef("scriptbundle_1018", ASSET_TYPE_SCRIPTBUNDLE, asset->unk1018);
				AddXAssetRef("customsettings", ASSET_TYPE_SCRIPTBUNDLE, asset->customsettings);
				AddXAssetRef("shrapnelsettings", ASSET_TYPE_SCRIPTBUNDLE, asset->shrapnelsettings);
				AddXAssetRef("var_2e4a8800", ASSET_TYPE_SCRIPTBUNDLE, asset->var_2e4a8800);
				AddXAssetRef("image_1038", ASSET_TYPE_IMAGE, asset->unk1038);
				AddXAssetRef("image_1040", ASSET_TYPE_IMAGE, asset->unk1040);
				AddXAssetRef("image_1048", ASSET_TYPE_IMAGE, asset->unk1048);
				AddXAssetRef("var_8456d4d", ASSET_TYPE_SCRIPTBUNDLE, asset->var_8456d4d);

			}
			json.EndObject();

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_WEAPON };
}