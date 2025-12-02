#include <includes.hpp>
#include <tools/ff/handlers/bo4/bo4_unlinker_weapon.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/utils/data_utils.hpp>

namespace fastfile::handlers::bo4::weapon {
	using namespace games::bo4::pool;
	class ImplWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			WeaponDef* asset{ (WeaponDef*)ptr };

			// refs
			constexpr size_t postWeaponDefNameField = offsetof(WeaponDef, name) + sizeof(WeaponDef::name);
			constexpr size_t testLen = sizeof(WeaponDef) - postWeaponDefNameField;
			
			if (utils::data::IsNulled(&((byte*)ptr)[postWeaponDefNameField], testLen)) {
				LOG_INFO("ignore empty weapondef {}", hashutils::ExtractTmp("file", asset->name.name));
				return;
			}

			const char* baseWeaponStr{ hashutils::ExtractPtr(asset->baseWeapon) };

			if (baseWeaponStr) {
				// load default hashes
				static const char* suffixes[]{
					"_wz", "_zm", "_mp", "_cp", // gamemode values
					"_gold", "_operator", // wz weapons
					"_tunables", "_tunables_wz", "_tunables_zm", "_tunables_mp", // tunables
				};
				static const char* prefixes[]{
					"weapon/" // localize
				};

				for (const char* suffix : suffixes) {
					const char* s{ utils::va("%s%s", baseWeaponStr, suffix) };
					hashutils::AddPrecomputed(hash::Hash64(s), s, true);
				}

				for (const char* prefix : prefixes) {
					const char* s{ utils::va("%s%s", prefix, baseWeaponStr) };
					hashutils::AddPrecomputed(hash::Hash64(s), s, true);
				}
			}

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "weapon" 
				/ fastfile::GetCurrentContext().ffname
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name.name)) };
			std::filesystem::create_directories(outFile.parent_path());

			BO4JsonWriter json{};

			LOG_INFO("Dump weapon {}", outFile.string());


			json.BeginObject();
			{
				
				json.WriteFieldValueXHash("name", asset->name);
				json.WriteFieldValueXHash("baseWeapon", asset->baseWeapon);
				json.WriteFieldValueXHash("displayname", asset->displayname);

				json.WriteFieldValueXHash("altWeapon", asset->altWeapon);
				json.WriteFieldValueXHash("weapon40", asset->weapon40);
				json.WriteFieldValueXHash("weapon50", asset->weapon50);
				json.WriteFieldValueXHash("xhash_60", asset->unk60);
				json.WriteFieldValueXHash("weapon70", asset->weapon70);
				if (asset->luigiReticleWidget && *asset->luigiReticleWidget) {
					json.WriteFieldValueString("luigiReticleWidget", asset->luigiReticleWidget);
				}
				json.WriteFieldValueXHash("dualwieldWeaponName", asset->dualwieldWeaponName);
				json.WriteFieldValueXHash("grenadeWeaponName", asset->grenadeWeaponName);
				json.WriteFieldValueXHash("weapon730Name", asset->weapon730Name);
				json.WriteFieldValueXHash("weapon880", asset->weapon880);
				json.WriteFieldValueXHash("weapon890", asset->weapon890);
				json.WriteFieldValueXHash("streamkey", asset->streamkey);

				json.WriteFieldValueScrStringArray("scrstr_8c8", ARRAYSIZE(*asset->unk8c8), *asset->unk8c8);
				json.WriteFieldValueScrStringArray("scrstr_8d0", ARRAYSIZE(*asset->unk8d0), *asset->unk8d0);
				json.WriteFieldValueScrStringArray("scrstr_8d8", ARRAYSIZE(*asset->unk8d8), *asset->unk8d8);

				json.WriteFieldValueScrString("worldModelTagRight", asset->worldModelTagRight);
				json.WriteFieldValueScrString("worldModelTagLeft", asset->worldModelTagLeft);
				json.WriteFieldValueScrString("scrstr_118c", asset->unk118c);
				json.WriteFieldValueScrString("scrstr_1190", asset->unk1190);
				json.WriteFieldValueScrString("stowedModelTag", asset->stowedModelTag);
				json.WriteFieldValueScrString("scrstr_1198", asset->unk1198);
				json.WriteFieldValueScrString("scrstr_119c", asset->unk119c);
				json.WriteFieldValueScrString("scrstr_11a0", asset->unk11a0);
				json.WriteFieldValueScrString("scrstr_11a4", asset->unk11a4);
				json.WriteFieldValueScrString("scrstr_11a8", asset->unk11a8);
				json.WriteFieldValueScrString("scrstr_11ac", asset->unk11ac);
				json.WriteFieldValueScrString("scrstr_11b0", asset->unk11b0);
				json.WriteFieldValueScrString("scrstr_11b4", asset->unk11b4);

				json.WriteFieldValueNumber("weaponstarthitpoints", asset->weaponstarthitpoints);
				json.WriteFieldValueNumber("soundrattlerangemin", asset->soundrattlerangemin);
				json.WriteFieldValueNumber("soundrattlerangemax", asset->soundrattlerangemax);
				//json.WriteFieldValueNumber("grenadeweapon", asset->grenadeweapon);
				//json.WriteFieldValueNumber("dualwieldweapon", asset->dualwieldweapon);
				//json.WriteFieldValueNumber("dualWieldWeaponIndex", asset->dualWieldWeaponIndex);
				//json.WriteFieldValueNumber("altWeaponIndex", asset->altWeaponIndex);
				//json.WriteFieldValueNumber("itemIndex", asset->itemIndex);
				json.WriteFieldValueNumber(0x5b73038c, asset->var_5b73038c);
				json.WriteFieldValueNumber(0x6566504b, asset->var_6566504b);
				json.WriteFieldValueNumber("weaponheadobjectiveheight", asset->weaponheadobjectiveheight);
				json.WriteFieldValueString("sessionMode", GetEModeName(asset->sessionMode));
				json.WriteFieldValueBool("bDrawOffhandModelInHand", asset->bDrawOffhandModelInHand);
				json.WriteFieldValueBool("lockonseekersearchsoundloops", asset->lockonseekersearchsoundloops);
				json.WriteFieldValueBool("lockonseekerlockedsoundloops", asset->lockonseekerlockedsoundloops);
				json.WriteFieldValueBool("lockontargetlockedsoundloops", asset->lockontargetlockedsoundloops);
				json.WriteFieldValueBool("lockontargetfiredonsoundloops", asset->lockontargetfiredonsoundloops);
				json.WriteFieldValueBool("forcedamageshellshockandrumble", asset->forcedamageshellshockandrumble);
				json.WriteFieldValueBool(0x965cc0b3, asset->var_965cc0b3);
				json.WriteFieldValueBool(0x50d2316b, asset->var_50d2316b);
				json.WriteFieldValueBool(0x83b1dc1, asset->var_83b1dc1);
				json.WriteFieldValueBool(0x2cf49821, asset->var_2cf49821);
				json.WriteFieldValueBool(0xd2751f9d, asset->var_d2751f9d);
				json.WriteFieldValueBool(0x554be9f7, asset->var_554be9f7);
				if (asset->accuracyGraphKnotCount[0] || asset->accuracyGraphKnotCount[1]) {
					json.WriteFieldNameString("accuracyGraphKnot");
					json.BeginArray();
					for (size_t i = 0; i < ARRAYSIZE(asset->accuracyGraphKnotCount); i++) {
						int32_t count{ asset->accuracyGraphKnotCount[i] };

						json.BeginArray();
						for (size_t j = 0; j < count; j++) {
							json.WriteValueString(std::format("{} {}", asset->accuracyGraphKnots[i][j][0], asset->accuracyGraphKnots[i][j][1]));
						}
						json.EndArray();
					}
					json.EndArray();
				}
				// empty?
				json.WriteFieldValueXHashArray("unkce8", asset->var_5b73038c, asset->unkce8);
				json.WriteFieldValueXHashArray("unkcf0", asset->var_5b73038c, asset->unkcf0);
				json.WriteFieldValueXHashArray("unkd10", asset->var_5b73038c, asset->unkd10);


				json.WriteFieldValueXHash("sound90", asset->sound90);
				json.WriteFieldValueXHash("sounda0", asset->sounda0);
				json.WriteFieldValueXHash("soundb0", asset->soundb0);
				json.WriteFieldValueXHash("soundc0", asset->soundc0);
				json.WriteFieldValueXHash("firesounddistant", asset->firesounddistant);
				json.WriteFieldValueXHash("firesound", asset->firesound);
				json.WriteFieldValueXHash("firesoundplayer", asset->firesoundplayer);
				json.WriteFieldValueXHash("sound100", asset->sound100);
				json.WriteFieldValueXHash("sound110", asset->sound110);
				json.WriteFieldValueXHash("sound120", asset->sound120);
				json.WriteFieldValueXHash("sound130", asset->sound130);
				json.WriteFieldValueXHash("sound140", asset->sound140);
				json.WriteFieldValueXHash("sound150", asset->sound150);
				json.WriteFieldValueXHash("sound160", asset->sound160);
				json.WriteFieldValueXHash("sound170", asset->sound170);
				json.WriteFieldValueXHash("sound180", asset->sound180);
				json.WriteFieldValueXHash("sound190", asset->sound190);
				json.WriteFieldValueXHash("sound1a0", asset->sound1a0);
				json.WriteFieldValueXHash("sound1b0", asset->sound1b0);
				json.WriteFieldValueXHash("sound1c0", asset->sound1c0);
				json.WriteFieldValueXHash("sound1d0", asset->sound1d0);
				json.WriteFieldValueXHash("sound1e0", asset->sound1e0);
				json.WriteFieldValueXHash("sound1f0", asset->sound1f0);
				json.WriteFieldValueXHash("sound200", asset->sound200);
				json.WriteFieldValueXHash("sound210", asset->sound210);
				json.WriteFieldValueXHash("sound220", asset->sound220);
				json.WriteFieldValueXHash("sound230", asset->sound230);
				json.WriteFieldValueXHash("sound240", asset->sound240);
				json.WriteFieldValueXHash("sound250", asset->sound250);
				json.WriteFieldValueXHash("sound260", asset->sound260);
				json.WriteFieldValueXHash("sound270", asset->sound270);
				json.WriteFieldValueXHash("sound280", asset->sound280);
				json.WriteFieldValueXHash("sound290", asset->sound290);
				json.WriteFieldValueXHash("sound2a0", asset->sound2a0);
				json.WriteFieldValueXHash("sound2b0", asset->sound2b0);
				json.WriteFieldValueXHash("sound2c0", asset->sound2c0);
				json.WriteFieldValueXHash("sound2d0", asset->sound2d0);
				json.WriteFieldValueXHash("sound2e0", asset->sound2e0);
				json.WriteFieldValueXHash("sound2f0", asset->sound2f0);
				json.WriteFieldValueXHash("sound300", asset->sound300);
				json.WriteFieldValueXHash("sound310", asset->sound310);
				json.WriteFieldValueXHash("sound320", asset->sound320);
				json.WriteFieldValueXHash("sound330", asset->sound330);
				json.WriteFieldValueXHash("sound340", asset->sound340);
				json.WriteFieldValueXHash("sound350", asset->sound350);
				json.WriteFieldValueXHash("sound360", asset->sound360);
				json.WriteFieldValueXHash("sound370", asset->sound370);
				json.WriteFieldValueXHash("sound380", asset->sound380);
				json.WriteFieldValueXHash("gadgetreadysound", asset->gadgetreadysound);
				json.WriteFieldValueXHash(0x1f7ccc3b, asset->var_1f7ccc3b);
				json.WriteFieldValueXHash("sound3b0", asset->sound3b0);
				json.WriteFieldValueXHash("sound3c0", asset->sound3c0);
				json.WriteFieldValueXHash("sound3d0", asset->sound3d0);
				json.WriteFieldValueXHash("sound3e0", asset->sound3e0);
				json.WriteFieldValueXHash("sound3f0", asset->sound3f0);
				json.WriteFieldValueXHash("sound400", asset->sound400);
				json.WriteFieldValueXHash("sound410", asset->sound410);
				json.WriteFieldValueXHash("gadgetreadysoundplayer", asset->gadgetreadysoundplayer);
				json.WriteFieldValueXHash(0xfb22040b, asset->var_fb22040b);
				json.WriteFieldValueXHash("sound440", asset->sound440);
				json.WriteFieldValueXHash("sound450", asset->sound450);
				json.WriteFieldValueXHash("sound460", asset->sound460);
				json.WriteFieldValueXHash("sound470", asset->sound470);
				json.WriteFieldValueXHash("sound480", asset->sound480);
				json.WriteFieldValueXHash("sound490", asset->sound490);
				json.WriteFieldValueXHash("sound4a0", asset->sound4a0);
				json.WriteFieldValueXHash("sound4b0", asset->sound4b0);
				json.WriteFieldValueXHash("sound4c0", asset->sound4c0);
				json.WriteFieldValueXHash("sound4d0", asset->sound4d0);
				json.WriteFieldValueXHash("sound4e0", asset->sound4e0);
				json.WriteFieldValueXHash("sound4f0", asset->sound4f0);
				json.WriteFieldValueXHash("sound500", asset->sound500);
				json.WriteFieldValueXHash("sound510", asset->sound510);
				json.WriteFieldValueXHash("sound520", asset->sound520);
				json.WriteFieldValueXHash("sound530", asset->sound530);
				json.WriteFieldValueXHash("sound540", asset->sound540);
				json.WriteFieldValueXHash("sound550", asset->sound550);
				json.WriteFieldValueXHash("sound560", asset->sound560);
				json.WriteFieldValueXHash("sound570", asset->sound570);
				json.WriteFieldValueXHash("sound580", asset->sound580);
				json.WriteFieldValueXHash("sound590", asset->sound590);
				json.WriteFieldValueXHash("sound5a0", asset->sound5a0);
				json.WriteFieldValueXHash("sound5b0", asset->sound5b0);
				json.WriteFieldValueXHash("sound5c0", asset->sound5c0);
				json.WriteFieldValueXHash("sound5d0", asset->sound5d0);
				json.WriteFieldValueXHash("sound5e0", asset->sound5e0);
				json.WriteFieldValueXHash("sound5f0", asset->sound5f0);
				json.WriteFieldValueXHash("sound600", asset->sound600);
				json.WriteFieldValueXHash("sound610", asset->sound610);
				json.WriteFieldValueXHash("sound620", asset->sound620);
				json.WriteFieldValueXHash("sound630", asset->sound630);
				json.WriteFieldValueXHash("sound640", asset->sound640);
				json.WriteFieldValueXHash("sound650", asset->sound650);
				json.WriteFieldValueXHash("sound660", asset->sound660);
				json.WriteFieldValueXHash("sound670", asset->sound670);
				json.WriteFieldValueXHash("sound680", asset->sound680);
				json.WriteFieldValueXHash("sound690", asset->sound690);
				json.WriteFieldValueXHash("sound6a0", asset->sound6a0);
				json.WriteFieldValueXHash("sound6b0", asset->sound6b0);
				json.WriteFieldValueXHash("sound6c0", asset->sound6c0);
				json.WriteFieldValueXHash("lockonseekersearchsound", asset->lockonseekersearchsound);
				json.WriteFieldValueXHash("lockonseekerlockedsound", asset->lockonseekerlockedsound);
				json.WriteFieldValueXHash("lockontargetlockedsound", asset->lockontargetlockedsound);
				json.WriteFieldValueXHash("lockontargetfiredonsound", asset->lockontargetfiredonsound);
				json.WriteFieldValueXHash("sound710", asset->sound710);
				json.WriteFieldValueXHash(0x8a03df2b, asset->var_8a03df2b);
				json.WriteFieldValueXHash(0x2f3ca476, asset->var_2f3ca476);
				json.WriteFieldValueXHash(0x5c29f743, asset->var_5c29f743);
				json.WriteFieldValueXHash("projexplosionsound", asset->projexplosionsound);
				json.WriteFieldValueXHash("projexplosionsoundplayer", asset->projexplosionsoundplayer);
				json.WriteFieldValueXHash("projsmokestartsound", asset->projsmokestartsound);
				json.WriteFieldValueXHash("projsmokeloopsound", asset->projsmokeloopsound);
				json.WriteFieldValueXHash("projsmokeendsound", asset->projsmokeendsound);
				json.WriteFieldValueXHash("sound7d0", asset->sound7d0);
				json.WriteFieldValueXHash("sound7e0", asset->sound7e0);
				json.WriteFieldValueXHash("sound7f0", asset->sound7f0);
				json.WriteFieldValueXHash("sound800", asset->sound800);
				json.WriteFieldValueXHash("sound810", asset->sound810);
				json.WriteFieldValueXHash("sound820", asset->sound820);
				json.WriteFieldValueXHash("sound830", asset->sound830);
				json.WriteFieldValueXHash("sound840", asset->sound840);
				json.WriteFieldValueXHash("sound850", asset->sound850);
				json.WriteFieldValueXHash("sound860", asset->sound860);
				json.WriteFieldValueXHash("hitsound", asset->hitsound);

				json.WriteFieldValueXAsset("tunables1", ASSET_TYPE_WEAPON_TUNABLES, asset->tunables1);
				json.WriteFieldValueXAsset("tunables2", ASSET_TYPE_WEAPON_TUNABLES, asset->tunables2);
				json.WriteFieldValueXAsset("weaponCamo", ASSET_TYPE_WEAPON_CAMO, asset->weaponCamo);
				json.WriteFieldValueXAsset("cacWeaponXCam", ASSET_TYPE_XCAM, asset->cacWeaponXCam);
				json.WriteFieldValueXAsset("cacAttachmentsXCam", ASSET_TYPE_XCAM, asset->cacAttachmentsXCam);
				json.WriteFieldValueXAsset("weaponIconXCam", ASSET_TYPE_XCAM, asset->weaponIconXCam);
				json.WriteFieldValueXAssetArray("attachments", ASSET_TYPE_ATTACHMENT, ARRAYSIZE(*asset->attachments), asset->attachments);
				json.WriteFieldValueXAssetArray("attachmentUniques", ASSET_TYPE_ATTACHMENT_UNIQUE, ARRAYSIZE(*asset->attachmentUniques), asset->attachmentUniques);
				json.WriteFieldValueXAsset("xanim_a68", ASSET_TYPE_XANIM, asset->unka68);
				json.WriteFieldValueXAsset("sharedWeaponSounds", ASSET_TYPE_SHAREDWEAPONSOUNDS, asset->sharedWeaponSounds);
				json.WriteFieldValueXAsset("ballisticDesc", ASSET_TYPE_BALLISTICDESC, asset->ballisticDesc);
				json.WriteFieldValueXAsset("weaponHeadObjective", ASSET_TYPE_OBJECTIVE, asset->weaponHeadObjective);
				json.WriteFieldValueXAsset("crateObjective", ASSET_TYPE_OBJECTIVE, asset->crateObjective);
				json.WriteFieldValueXAsset("flameTableFirstPerson", ASSET_TYPE_FLAMETABLE, asset->flameTableFirstPerson);
				json.WriteFieldValueXAsset("flameTableThirdPerson", ASSET_TYPE_FLAMETABLE, asset->flameTableThirdPerson);

				if (asset->gestureTables && **asset->gestureTables) {
					json.WriteFieldNameString("gestureTables");
					json.BeginArray();
					for (GestureTable* table : *asset->gestureTables) {
						if (!table) {
							break;
						}
						json.WriteValueXAsset(ASSET_TYPE_GESTURE_TABLE, table);
					}
					json.EndArray();
				}
				if (asset->tracers && **asset->tracers) {
					json.WriteFieldNameString("tracers");
					json.BeginArray();
					for (TracerDef* def : *asset->tracers) {
						if (!def) {
							break;
						}
						json.WriteValueXAsset(ASSET_TYPE_TRACER, def);
					}
					json.EndArray();
				}
				if (asset->statusEffects && asset->statusEffects[0].effect) {
					json.WriteFieldNameString("statusEffects");
					json.BeginArray();
					for (WeaponDefStatusEffect& effect : asset->statusEffects) {
						if (utils::data::IsNulled(effect)) {
							break;
						}
						json.BeginObject();
						json.WriteFieldValueXAsset("effect", ASSET_TYPE_STATUS_EFFECT, effect.effect);
						json.WriteFieldValueUnknown("statusEffects:unk8", effect.unk8);
						json.WriteFieldValueUnknown("statusEffects:unk10", effect.unk10);
						json.WriteFieldValueUnknown("statusEffects:unk18", effect.unk18);
						json.EndObject();
					}
					json.EndArray();
				}

				json.WriteFieldValueXAsset("scriptbundle_1008", ASSET_TYPE_SCRIPTBUNDLE, asset->unk1008);
				json.WriteFieldValueXAsset("turretAnimSettings", ASSET_TYPE_SCRIPTBUNDLE, asset->turretAnimSettings);
				json.WriteFieldValueXAsset("killcamSettings", ASSET_TYPE_SCRIPTBUNDLE, asset->killcamSettings);
				json.WriteFieldValueXAsset("customsettings", ASSET_TYPE_SCRIPTBUNDLE, asset->customsettings);
				json.WriteFieldValueXAsset("shrapnelsettings", ASSET_TYPE_SCRIPTBUNDLE, asset->shrapnelsettings);
				json.WriteFieldValueXAsset(0x2e4a8800, ASSET_TYPE_SCRIPTBUNDLE, asset->var_2e4a8800);
				json.WriteFieldValueXAsset(0x8456d4d, ASSET_TYPE_SCRIPTBUNDLE, asset->var_8456d4d);

				json.WriteFieldValueXAsset("handModel", ASSET_TYPE_XMODEL, asset->handModel);
				json.WriteFieldValueXAsset("model_d28", ASSET_TYPE_XMODEL, asset->unkd28);
				json.WriteFieldValueXAsset(0x22082a57, ASSET_TYPE_XMODEL, asset->var_22082a57);
				json.WriteFieldValueXAsset("projectilemodel", ASSET_TYPE_XMODEL, asset->projectilemodel);
				json.WriteFieldValueXAsset("projectileModelEnemy", ASSET_TYPE_XMODEL, asset->projectileModelEnemy);
				json.WriteFieldValueXAsset(0x4bcd08b0, ASSET_TYPE_XMODEL, asset->var_4bcd08b0);
				json.WriteFieldValueXAssetArray("viewmodel", ASSET_TYPE_XMODEL, asset->var_5b73038c, asset->viewmodel);
				json.WriteFieldValueXAssetArray("frontendmodel", ASSET_TYPE_XMODEL, asset->var_5b73038c, asset->frontendmodel);
				json.WriteFieldValueXAssetArray("worldModel", ASSET_TYPE_XMODEL, asset->var_5b73038c, asset->worldModel);
				json.WriteFieldValueXAssetArray("models_cd0", ASSET_TYPE_XMODEL, asset->var_5b73038c, asset->unkcd0);
				json.WriteFieldValueXAssetArray("stowedmodel", ASSET_TYPE_XMODEL, asset->var_5b73038c, asset->stowedmodel);
				json.WriteFieldValueXAssetArray("clipmodel", ASSET_TYPE_XMODEL, asset->var_5b73038c, asset->clipmodel);

				json.WriteFieldValueXAsset("gadgetIconAvailable", ASSET_TYPE_IMAGE, asset->gadgetIconAvailable);
				json.WriteFieldValueXAsset("gadgetIconUnavailable", ASSET_TYPE_IMAGE, asset->gadgetIconUnavailable);
				json.WriteFieldValueXAsset("image_c70", ASSET_TYPE_IMAGE, asset->unkc70);
				json.WriteFieldValueXAsset("image_c78", ASSET_TYPE_IMAGE, asset->unkc78);
				json.WriteFieldValueXAsset("reticleCenter", ASSET_TYPE_IMAGE, asset->reticleCenter);
				json.WriteFieldValueXAsset("reticleSide", ASSET_TYPE_IMAGE, asset->reticleSide);
				json.WriteFieldValueXAssetArray("images_cf8", ASSET_TYPE_IMAGE, asset->var_5b73038c, asset->unkcf8);
				json.WriteFieldValueXAssetArray("images_d00", ASSET_TYPE_IMAGE, asset->var_5b73038c, asset->unkd00);
				json.WriteFieldValueXAssetArray("images_d08", ASSET_TYPE_IMAGE, asset->var_5b73038c, asset->unkd08);
				json.WriteFieldValueXAsset("inventoryIcon", ASSET_TYPE_IMAGE, asset->inventoryIcon);
				json.WriteFieldValueXAsset("inventoryIconZm", ASSET_TYPE_IMAGE, asset->inventoryIconZm);
				json.WriteFieldValueXAsset("hudIcon", ASSET_TYPE_IMAGE, asset->hudIcon);
				json.WriteFieldValueXAsset("reticleSeekingLockOn", ASSET_TYPE_IMAGE, asset->reticleSeekingLockOn);
				json.WriteFieldValueXAsset("reticleLockOn", ASSET_TYPE_IMAGE, asset->reticleLockOn);
				json.WriteFieldValueXAsset("image_d88", ASSET_TYPE_IMAGE, asset->unkd88);
				json.WriteFieldValueXAsset("image_d90", ASSET_TYPE_IMAGE, asset->unkd90);
				json.WriteFieldValueXAsset("image_d98", ASSET_TYPE_IMAGE, asset->unkd98);
				json.WriteFieldValueXAsset("image_da0", ASSET_TYPE_IMAGE, asset->unkda0);
				json.WriteFieldValueXAsset("image_da8", ASSET_TYPE_IMAGE, asset->unkda8);
				json.WriteFieldValueXAsset("image_db0", ASSET_TYPE_IMAGE, asset->unkdb0);
				json.WriteFieldValueXAsset("image_db8", ASSET_TYPE_IMAGE, asset->unkdb8);
				json.WriteFieldValueXAsset("image_e90", ASSET_TYPE_IMAGE, asset->unke90);
				json.WriteFieldValueXAsset("image_e98", ASSET_TYPE_IMAGE, asset->unke98);
				json.WriteFieldValueXAsset("image_ea0", ASSET_TYPE_IMAGE, asset->unkea0);
				json.WriteFieldValueXAsset("image_eb0", ASSET_TYPE_IMAGE, asset->unkeb0);
				json.WriteFieldValueXAsset("image_1038", ASSET_TYPE_IMAGE, asset->unk1038);
				json.WriteFieldValueXAsset("image_1040", ASSET_TYPE_IMAGE, asset->unk1040);
				json.WriteFieldValueXAsset("image_1048", ASSET_TYPE_IMAGE, asset->unk1048);

				json.WriteFieldValueXAsset("fireRumble", ASSET_TYPE_RUMBLE, asset->fireRumble);
				json.WriteFieldValueXAsset("rumble_dc8", ASSET_TYPE_RUMBLE, asset->unkdc8);
				json.WriteFieldValueXAsset("rumble_dd0", ASSET_TYPE_RUMBLE, asset->unkdd0);
				json.WriteFieldValueXAsset("meleeImpactRumble", ASSET_TYPE_RUMBLE, asset->meleeImpactRumble);
				json.WriteFieldValueXAsset("rumble_de0", ASSET_TYPE_RUMBLE, asset->unkde0);
				json.WriteFieldValueXAsset("rumble_de8", ASSET_TYPE_RUMBLE, asset->unkde8);
				json.WriteFieldValueXAsset("rumble_df0", ASSET_TYPE_RUMBLE, asset->unkdf0);
				json.WriteFieldValueXAsset("rumble_df8", ASSET_TYPE_RUMBLE, asset->unkdf8);
				json.WriteFieldValueXAsset("rumble_e00", ASSET_TYPE_RUMBLE, asset->unke00);
				json.WriteFieldValueXAsset("rumble_e08", ASSET_TYPE_RUMBLE, asset->unke08);
				json.WriteFieldValueXAsset("rumble_e10", ASSET_TYPE_RUMBLE, asset->unke10);
				json.WriteFieldValueXAsset("rumble_e18", ASSET_TYPE_RUMBLE, asset->unke18);
				json.WriteFieldValueXAsset("rumble_e20", ASSET_TYPE_RUMBLE, asset->unke20);
				json.WriteFieldValueXAsset("rumble_e28", ASSET_TYPE_RUMBLE, asset->unke28);

				json.WriteFieldValueXAsset("laser_e30", ASSET_TYPE_LASER, asset->unke30);
				json.WriteFieldValueXAsset("laser_e38", ASSET_TYPE_LASER, asset->unke38);
				json.WriteFieldValueXAsset("laser_e40", ASSET_TYPE_LASER, asset->unke40);

				json.WriteFieldValueXAsset("beam_e58", ASSET_TYPE_BEAM, asset->unke58);
				json.WriteFieldValueXAsset("beam_e60", ASSET_TYPE_BEAM, asset->unke60);
				json.WriteFieldValueXAsset("beam_e68", ASSET_TYPE_BEAM, asset->unke68);
				json.WriteFieldValueXAsset("beam_e70", ASSET_TYPE_BEAM, asset->unke70);
				json.WriteFieldValueXAsset("beam_e78", ASSET_TYPE_BEAM, asset->unke78);
				json.WriteFieldValueXAsset("beam_e80", ASSET_TYPE_BEAM, asset->unke80);
				json.WriteFieldValueXAsset("beam_e88", ASSET_TYPE_BEAM, asset->unke88);


				json.WriteFieldValueXAsset("reticlePivotLocked", ASSET_TYPE_MATERIAL, asset->reticlePivotLocked);
				json.WriteFieldValueXAsset("reticlePivotInvalid", ASSET_TYPE_MATERIAL, asset->reticlePivotInvalid);
				json.WriteFieldValueXAsset("reticlePivotTarget", ASSET_TYPE_MATERIAL, asset->reticlePivotTarget);
				json.WriteFieldValueXAsset("reticlePivotSides", ASSET_TYPE_MATERIAL, asset->reticlePivotSides);
				json.WriteFieldValueXAsset("reticlePivotLine", ASSET_TYPE_MATERIAL, asset->reticlePivotLine);

				json.WriteFieldValueXAsset("fx_bc8", ASSET_TYPE_FX, asset->unkbc8);
				json.WriteFieldValueXAsset("fx_bd0", ASSET_TYPE_FX, asset->unkbd0);
				json.WriteFieldValueXAsset("fx_bd8", ASSET_TYPE_FX, asset->unkbd8);
				json.WriteFieldValueXAsset("fx_be0", ASSET_TYPE_FX, asset->unkbe0);
				json.WriteFieldValueXAsset("fx_be8", ASSET_TYPE_FX, asset->unkbe8);
				json.WriteFieldValueXAsset("fx_c08", ASSET_TYPE_FX, asset->unkc08);
				json.WriteFieldValueXAsset("fx_c80", ASSET_TYPE_FX, asset->unkc80);
				json.WriteFieldValueXAsset("fx_c88", ASSET_TYPE_FX, asset->unkc88);
				json.WriteFieldValueXAsset("fx_c90", ASSET_TYPE_FX, asset->unkc90);
				json.WriteFieldValueXAsset("fx_c98", ASSET_TYPE_FX, asset->unkc98);
				json.WriteFieldValueXAsset("fx_ca0", ASSET_TYPE_FX, asset->unkca0);
				json.WriteFieldValueXAsset("fx_ca8", ASSET_TYPE_FX, asset->unkca8);
				json.WriteFieldValueXAsset("fx_ed8", ASSET_TYPE_FX, asset->unked8);
				json.WriteFieldValueXAsset("fx_ee0", ASSET_TYPE_FX, asset->unkee0);
				json.WriteFieldValueXAsset("fx_ee8", ASSET_TYPE_FX, asset->unkee8);
				json.WriteFieldValueXAsset("fx_ef0", ASSET_TYPE_FX, asset->unkef0);
				json.WriteFieldValueXAsset("fx_f00", ASSET_TYPE_FX, asset->unkf00);
				json.WriteFieldValueXAsset("fx_f08", ASSET_TYPE_FX, asset->unkf08);
				json.WriteFieldValueXAsset("fx_f10", ASSET_TYPE_FX, asset->unkf10);
				json.WriteFieldValueXAsset("fx_f18", ASSET_TYPE_FX, asset->unkf18);
				json.WriteFieldValueXAssetArray("underwaterExplosionSurfaceFX", ASSET_TYPE_FX, ARRAYSIZE(asset->underwaterExplosionSurfaceFX), asset->underwaterExplosionSurfaceFX);
				json.WriteFieldValueXAsset("fx_f38", ASSET_TYPE_FX, asset->unkf38);
				json.WriteFieldValueXAsset("fx_f40", ASSET_TYPE_FX, asset->unkf40);
				json.WriteFieldValueXAsset("fx_f48", ASSET_TYPE_FX, asset->unkf48);
				json.WriteFieldValueXAsset("fx_f50", ASSET_TYPE_FX, asset->unkf50);
				json.WriteFieldValueXAsset("fx_f58", ASSET_TYPE_FX, asset->unkf58);
				json.WriteFieldValueXAsset("fx_f60", ASSET_TYPE_FX, asset->unkf60);
				json.WriteFieldValueXAsset("fx_f68", ASSET_TYPE_FX, asset->unkf68);
				json.WriteFieldValueXAsset("fx_f70", ASSET_TYPE_FX, asset->unkf70);
				json.WriteFieldValueXAsset("fx_f88", ASSET_TYPE_FX, asset->unkf88);
				json.WriteFieldValueXAsset("fx_f90", ASSET_TYPE_FX, asset->unkf90);
				json.WriteFieldValueXAsset("fx_f98", ASSET_TYPE_FX, asset->unkf98);
				json.WriteFieldValueXAsset("fx_fa0", ASSET_TYPE_FX, asset->unkfa0);

				json.WriteFieldValueXAsset("viewPersistentEffectSet", ASSET_TYPE_TAGFX, asset->viewPersistentEffectSet);
				json.WriteFieldValueXAsset("worldPersistentEffectSet", ASSET_TYPE_TAGFX, asset->worldPersistentEffectSet);
				json.WriteFieldValueXAsset("enemyDeathFxSet", ASSET_TYPE_TAGFX, asset->enemyDeathFxSet);
				json.WriteFieldValueXAsset("tagFXSet930", ASSET_TYPE_TAGFX, asset->tagFXSet930);
				json.WriteFieldValueXAssetArray("tagFXSetFirstPerson", ASSET_TYPE_TAGFX, ARRAYSIZE(asset->tagFXSetFirstPerson), asset->tagFXSetFirstPerson);
				json.WriteFieldValueXAssetArray("tagFXSetThirdPerson", ASSET_TYPE_TAGFX, ARRAYSIZE(asset->tagFXSetThirdPerson), asset->tagFXSetThirdPerson);
				json.WriteFieldValueXAsset(0x96850284, ASSET_TYPE_TAGFX, asset->var_96850284);
				json.WriteFieldValueXAsset(0x26f68e75, ASSET_TYPE_TAGFX, asset->var_26f68e75);

				json.WriteFieldValueXAsset("normalImpactFX", ASSET_TYPE_IMPACT_FX, asset->normalImpactFX);
				json.WriteFieldValueXAsset("exitImpactFX", ASSET_TYPE_IMPACT_FX, asset->exitImpactFX);
				json.WriteFieldValueXAsset("bounceImpactFX", ASSET_TYPE_IMPACT_FX, asset->bounceImpactFX);
				json.WriteFieldValueXAsset("dudImpactFX", ASSET_TYPE_IMPACT_FX, asset->dudImpactFX);
				json.WriteFieldValueXAsset("fxImpact_fd8", ASSET_TYPE_IMPACT_FX, asset->unkfd8);

				json.WriteFieldValueXAsset("normalImpactSounds", ASSET_TYPE_IMPACT_SOUND, asset->normalImpactSounds);
				json.WriteFieldValueXAsset("exitImpactSounds", ASSET_TYPE_IMPACT_SOUND, asset->exitImpactSounds);
				json.WriteFieldValueXAsset("bounceImpactSounds", ASSET_TYPE_IMPACT_SOUND, asset->bounceImpactSounds);
				json.WriteFieldValueXAsset("dudImpactSounds", ASSET_TYPE_IMPACT_SOUND, asset->dudImpactSounds);
				json.WriteFieldValueXAsset("soundsImpact_1000", ASSET_TYPE_IMPACT_SOUND, asset->unk1000);


				if (asset->var_21329beb[0].model) {
					json.WriterFieldNameHash(0x21329beb);
					json.BeginArray();
					for (WeaponDefObj21329beb& arr : asset->var_21329beb) {
						if (!arr.model) break;
						json.BeginObject();
						json.WriteFieldValueXAsset("model", ASSET_TYPE_XMODEL, arr.model);
						json.EndObject();
					}

					json.EndArray();
				}


				if (opt.testDump) {
					/*
					json.WriteFieldValueUnknown("unk8a0", asset->unk8a0);
					json.WriteFieldValueUnknown("unk8a8", asset->unk8a8);
					json.WriteFieldValueUnknown("unk8e0", asset->unk8e0);
					json.WriteFieldValueUnknown("unk900", asset->unk900);
					json.WriteFieldValueUnknown("unk908", asset->unk908);
					json.WriteFieldValueUnknown("unk910", asset->unk910);
					json.WriteFieldValueUnknown("unk918", asset->unk918);
					json.WriteFieldValueUnknown("unk920", asset->unk920);
					json.WriteFieldValueUnknown("unk928", asset->unk928);
					json.WriteFieldValueUnknown("unk948", asset->unk948);
					json.WriteFieldValueUnknown("unk950", asset->unk950);
					json.WriteFieldValueUnknown("unk958", asset->unk958);
					json.WriteFieldValueUnknown("unk960", asset->unk960);
					json.WriteFieldValueUnknown("unk968", asset->unk968);
					json.WriteFieldValueUnknown("unk974", asset->unk974);
					json.WriteFieldValueUnknown("unk980", asset->unk980); // 4
					json.WriteFieldValueUnknown("unk9a0", asset->unk9a0);
					json.WriteFieldValueUnknown("unk9a8", asset->unk9a8);
					json.WriteFieldValueUnknown("unk9b0", asset->unk9b0);
					json.WriteFieldValueUnknown("unk9b8", asset->unk9b8);
					json.WriteFieldValueUnknown("unk9c0", asset->unk9c0);
					json.WriteFieldValueUnknown("unk9c8", asset->unk9c8);
					json.WriteFieldValueUnknown("unk9d0", asset->unk9d0);
					json.WriteFieldValueUnknown("unk9d8", asset->unk9d8);
					json.WriteFieldValueUnknown("unk9e0", asset->unk9e0);
					json.WriteFieldValueUnknown("unk9e8", asset->unk9e8);
					json.WriteFieldValueUnknown("unk9f0", asset->unk9f0);
					json.WriteFieldValueUnknown("unk9f8", asset->unk9f8);
					json.WriteFieldValueUnknown("unka00", asset->unka00);
					json.WriteFieldValueUnknown("unka08", asset->unka08);
					json.WriteFieldValueUnknown("unka10", asset->unka10);
					json.WriteFieldValueUnknown("unka18", asset->unka18);
					json.WriteFieldValueUnknown("unka20", asset->unka20);
					json.WriteFieldValueUnknown("unka30", asset->unka30);
					json.WriteFieldValueUnknown("unka38", asset->unka38);
					json.WriteFieldValueUnknown("unka50", asset->unka50);
					json.WriteFieldValueUnknown("unka88", asset->unka88);
					json.WriteFieldValueUnknown("unka90", asset->unka90);
					json.WriteFieldValueUnknown("unka98", asset->unka98);
					json.WriteFieldValueUnknown("unkaa0", asset->unkaa0);
					json.WriteFieldValueUnknown("unkaa8", asset->unkaa8);
					json.WriteFieldValueUnknown("unkab0", asset->unkab0);
					json.WriteFieldValueUnknown("unkab8", asset->unkab8);
					json.WriteFieldValueUnknown("unkac0", asset->unkac0);
					json.WriteFieldValueUnknown("unkac8", asset->unkac8);
					json.WriteFieldValueUnknown("unkad0", asset->unkad0);
					json.WriteFieldValueUnknown("unkad8", asset->unkad8);
					json.WriteFieldValueUnknown("unkae0", asset->unkae0);
					json.WriteFieldValueUnknown("unkae8", asset->unkae8);
					json.WriteFieldValueUnknown("unkaf0", asset->unkaf0);
					json.WriteFieldValueUnknown("unkaf8", asset->unkaf8);
					json.WriteFieldValueUnknown("unkb00", asset->unkb00);
					json.WriteFieldValueUnknown("unkb08", asset->unkb08);
					json.WriteFieldValueUnknown("unkb10", asset->unkb10);
					json.WriteFieldValueUnknown("unkb18", asset->unkb18);
					json.WriteFieldValueUnknown("unkb20", asset->unkb20);
					json.WriteFieldValueUnknown("unkb28", asset->unkb28);
					json.WriteFieldValueUnknown("unkb30", asset->unkb30);
					json.WriteFieldValueUnknown("unkb38", asset->unkb38);
					json.WriteFieldValueUnknown("unkb40", asset->unkb40);
					json.WriteFieldValueUnknown("unkb48", asset->unkb48);
					json.WriteFieldValueUnknown("unkb50", asset->unkb50);
					json.WriteFieldValueUnknown("unkb58", asset->unkb58);
					json.WriteFieldValueUnknown("unkb60", asset->unkb60);
					json.WriteFieldValueUnknown("unkb68", asset->unkb68);
					json.WriteFieldValueUnknown("unkb70", asset->unkb70);
					json.WriteFieldValueUnknown("unkb78", asset->unkb78);
					json.WriteFieldValueUnknown("unkb80", asset->unkb80);
					json.WriteFieldValueUnknown("unkb88", asset->unkb88);
					json.WriteFieldValueUnknown("unkb90", asset->unkb90);
					json.WriteFieldValueUnknown("unkb98", asset->unkb98);
					json.WriteFieldValueUnknown("unkba0", asset->unkba0);
					json.WriteFieldValueUnknown("unkba8", asset->unkba8);
					json.WriteFieldValueUnknown("unkbb0", asset->unkbb0);
					json.WriteFieldValueUnknown("unkbb8", asset->unkbb8);
					json.WriteFieldValueUnknown("unkbc0", asset->unkbc0);
					json.WriteFieldValueUnknown("unkc18", asset->unkc18);
					json.WriteFieldValueUnknown("unkcc0", asset->unkcc0);
					json.WriteFieldValueUnknown("unkd18", asset->unkd18);
					json.WriteFieldValueUnknown("unkd20", asset->unkd20);
					json.WriteFieldValueUnknown("unkfa8", asset->unkfa8);
					json.WriteFieldValueUnknown("unkfb0", asset->unkfb0);
					
					json.WriteFieldValueNumber("unk1058", asset->unk1058);
					json.WriteFieldValueNumber("unk105c", asset->unk105c);
					json.WriteFieldValueNumber("unk1060", asset->unk1060);
					json.WriteFieldValueNumber("unk1064", asset->unk1064);
					json.WriteFieldValueNumber("unk1068", asset->unk1068);
					json.WriteFieldValueNumber("unk106c", asset->unk106c);
					json.WriteFieldValueUnknown("unk1070", asset->unk1070);
					json.WriteFieldValueNumber("unk1074", asset->unk1074);
					json.WriteFieldValueNumber("unk1078", asset->unk1078);
					json.WriteFieldValueUnknown("unk107c", asset->unk107c);
					json.WriteFieldValueNumber("unk1080", asset->unk1080);
					json.WriteFieldValueUnknown("unk1084", asset->unk1084);
					json.WriteFieldValueNumber("unk1088", asset->unk1088);
					json.WriteFieldValueNumber("unk108c", asset->unk108c);
					json.WriteFieldValueNumber("unk1090", asset->unk1090);
					json.WriteFieldValueUnknown("unk1094", asset->unk1094);
					json.WriteFieldValueUnknown("unk1098", asset->unk1098);
					json.WriteFieldValueNumber("unk10a0", asset->unk10a0);
					json.WriteFieldValueUnknown("unk10a4", asset->unk10a4);
					json.WriteFieldValueNumber("unk10a8", asset->unk10a8);
					json.WriteFieldValueNumber("unk10ac", asset->unk10ac);
					json.WriteFieldValueUnknown("unk10b0", asset->unk10b0);
					json.WriteFieldValueUnknown("unk10b8", asset->unk10b8);
					json.WriteFieldValueUnknown("unk10c0", asset->unk10c0);
					json.WriteFieldValueNumber("unk10c4", asset->unk10c4);
					json.WriteFieldValueNumber("unk10c8", asset->unk10c8);
					json.WriteFieldValueNumber("unk10cc", asset->unk10cc);
					json.WriteFieldValueNumber("unk10d0", asset->unk10d0);
					json.WriteFieldValueNumber("unk10d4", asset->unk10d4);
					json.WriteFieldValueNumber("unk10d8", asset->unk10d8);
					json.WriteFieldValueNumber("unk10dc", asset->unk10dc);
					json.WriteFieldValueNumber("unk10e0", asset->unk10e0);
					json.WriteFieldValueNumber("unk10e4", asset->unk10e4);
					json.WriteFieldValueNumber("unk10e8", asset->unk10e8);
					json.WriteFieldValueNumber("unk10ec", asset->unk10ec);
					json.WriteFieldValueNumber("unk10f0", asset->unk10f0);
					json.WriteFieldValueNumber("unk10f4", asset->unk10f4);
					json.WriteFieldValueNumber("unk10f8", asset->unk10f8);
					json.WriteFieldValueNumber("unk10fc", asset->unk10fc);
					json.WriteFieldValueNumber("unk1100", asset->unk1100);
					json.WriteFieldValueUnknown("unk1104", asset->unk1104);
					json.WriteFieldValueNumber("unk1108", asset->unk1108);
					json.WriteFieldValueUnknown("unk110c", asset->unk110c);
					json.WriteFieldValueUnknown("unk1110", asset->unk1110);
					json.WriteFieldValueNumber("unk1114", asset->unk1114);
					json.WriteFieldValueNumber("unk1118", asset->unk1118);
					json.WriteFieldValueUnknown("unk111c", asset->unk111c);



					json.WriteFieldValueUnknown("unk1120", asset->unk1120);
					json.WriteFieldValueUnknown("unk1128", asset->unk1128);
					json.WriteFieldValueUnknown("unk1130", asset->unk1130);
					json.WriteFieldValueUnknown("unk1138", asset->unk1138);
					json.WriteFieldValueUnknown("unk1140", asset->unk1140);
					json.WriteFieldValueUnknown("unk1148", asset->unk1148);
					json.WriteFieldValueUnknown("unk1150", asset->unk1150);
					json.WriteFieldValueUnknown("unk1158", asset->unk1158);
					json.WriteFieldValueUnknown("unk1160", asset->unk1160);
					json.WriteFieldValueUnknown("unk1168", asset->unk1168);
					json.WriteFieldValueUnknown("unk1170", asset->unk1170);
					json.WriteFieldValueUnknown("unk1178", asset->unk1178);
					json.WriteFieldValueUnknown("unk1180", asset->unk1180);
					json.WriteFieldValueUnknown("unk11b8", asset->unk11b8);
					json.WriteFieldValueUnknown("unk11c0", asset->unk11c0);
					json.WriteFieldValueUnknown("unk11c8", asset->unk11c8);
					json.WriteFieldValueUnknown("unk11d0", asset->unk11d0);
					json.WriteFieldValueUnknown("unk11d8", asset->unk11d8);
					json.WriteFieldValueUnknown("unk11e0", asset->unk11e0);
					json.WriteFieldValueUnknown("unk11e8", asset->unk11e8);
					json.WriteFieldValueUnknown("unk11f0", asset->unk11f0);
					json.WriteFieldValueUnknown("unk11f8", asset->unk11f8);
					json.WriteFieldValueUnknown("unk1200", asset->unk1200);
					json.WriteFieldValueUnknown("unk1208", asset->unk1208);
					json.WriteFieldValueUnknown("unk1210", asset->unk1210);
					json.WriteFieldValueUnknown("unk1218", asset->unk1218);
					json.WriteFieldValueUnknown("unk1220", asset->unk1220);
					json.WriteFieldValueUnknown("unk1228", asset->unk1228);
					json.WriteFieldValueUnknown("unk1230", asset->unk1230);
					json.WriteFieldValueUnknown("unk1238", asset->unk1238);
					json.WriteFieldValueUnknown("unk1240", asset->unk1240);
					json.WriteFieldValueUnknown("unk1248", asset->unk1248);
					json.WriteFieldValueUnknown("unk1250", asset->unk1250);
					json.WriteFieldValueUnknown("unk1258", asset->unk1258);
					json.WriteFieldValueUnknown("unk1260", asset->unk1260);
					json.WriteFieldValueUnknown("unk1268", asset->unk1268);
					json.WriteFieldValueUnknown("unk1270", asset->unk1270);
					json.WriteFieldValueUnknown("unk1278", asset->unk1278);
					json.WriteFieldValueUnknown("unk1280", asset->unk1280);
					json.WriteFieldValueUnknown("unk1288", asset->unk1288);
					json.WriteFieldValueUnknown("unk1290", asset->unk1290);
					json.WriteFieldValueUnknown("unk1298", asset->unk1298);
					json.WriteFieldValueUnknown("unk12a0", asset->unk12a0);
					json.WriteFieldValueUnknown("unk12a8", asset->unk12a8);
					json.WriteFieldValueUnknown("unk12b0", asset->unk12b0);
					json.WriteFieldValueUnknown("unk12b8", asset->unk12b8);
					json.WriteFieldValueUnknown("unk12c0", asset->unk12c0);
					json.WriteFieldValueUnknown("unk12c8", asset->unk12c8);
					json.WriteFieldValueUnknown("unk12d0", asset->unk12d0);
					json.WriteFieldValueUnknown("unk12d8", asset->unk12d8);
					json.WriteFieldValueUnknown("unk12e0", asset->unk12e0);
					json.WriteFieldValueUnknown("unk12e8", asset->unk12e8);
					json.WriteFieldValueUnknown("unk12f0", asset->unk12f0);
					json.WriteFieldValueUnknown("unk12f8", asset->unk12f8);
					json.WriteFieldValueUnknown("unk1300", asset->unk1300);
					json.WriteFieldValueUnknown("unk1308", asset->unk1308);
					json.WriteFieldValueUnknown("unk1310", asset->unk1310);
					json.WriteFieldValueUnknown("unk1318", asset->unk1318);
					json.WriteFieldValueUnknown("unk1320", asset->unk1320);
					json.WriteFieldValueUnknown("unk1328", asset->unk1328);
					json.WriteFieldValueUnknown("unk1330", asset->unk1330);
					json.WriteFieldValueUnknown("unk1338", asset->unk1338);
					json.WriteFieldValueUnknown("unk1344", asset->unk1344);
					json.WriteFieldValueUnknown("unk1348", asset->unk1348);
					json.WriteFieldValueUnknown("unk1354", asset->unk1354);
					json.WriteFieldValueUnknown("unk135c", asset->unk135c);
					json.WriteFieldValueUnknown("unk1360", asset->unk1360);
					json.WriteFieldValueUnknown("unk1368", asset->unk1368);
					json.WriteFieldValueUnknown("unk1370", asset->unk1370);
					json.WriteFieldValueUnknown("unk1380", asset->unk1380);
					json.WriteFieldValueUnknown("unk1390", asset->unk1390);
					json.WriteFieldValueUnknown("unk1398", asset->unk1398);
					json.WriteFieldValueUnknown("unk13a0", asset->unk13a0);
					json.WriteFieldValueUnknown("unk13a8", asset->unk13a8);
					json.WriteFieldValueUnknown("unk13b0", asset->unk13b0);
					json.WriteFieldValueUnknown("unk13c0", asset->unk13c0);
					json.WriteFieldValueUnknown("unk13cc", asset->unk13cc);
					json.WriteFieldValueUnknown("unk13d6", asset->unk13d6);
					json.WriteFieldValueUnknown("unk13d8", asset->unk13d8);
					json.WriteFieldValueUnknown("unk13da", asset->unk13da);
					json.WriteFieldValueUnknown("unk13e0", asset->unk13e0);
					json.WriteFieldValueUnknown("unk13e5", asset->unk13e5);
					json.WriteFieldValueUnknown("unk13e6", asset->unk13e6);
					json.WriteFieldValueUnknown("unk13e8", asset->unk13e8);
					json.WriteFieldValueUnknown("unk13eb", asset->unk13eb);
					json.WriteFieldValueUnknown("unk13ec", asset->unk13ec);
					json.WriteFieldValueUnknown("unk13f0", asset->unk13f0);
					json.WriteFieldValueUnknown("unk13f7", asset->unk13f7);
					json.WriteFieldValueUnknown("unk13f8", asset->unk13f8);
					json.WriteFieldValueUnknown("unk1400", asset->unk1400);
					json.WriteFieldValueUnknown("unk1408", asset->unk1408);
					json.WriteFieldValueUnknown("unk1410", asset->unk1410);
					json.WriteFieldValueUnknown("unk1418", asset->unk1418);
					json.WriteFieldValueUnknown("unk1420", asset->unk1420);
					json.WriteFieldValueUnknown("unk1428", asset->unk1428);
					*/
					if (!utils::data::IsNulled(asset->var_21329beb)) {
						json.WriterFieldNameHash(0x21329beb);
						json.BeginArray();
						for (WeaponDefObj21329beb& arr : asset->var_21329beb) {
							json.BeginObject();
							json.WriteFieldValueXAsset("model", ASSET_TYPE_XMODEL, arr.model);

							if (opt.testDump) {
								json.WriteFieldValueUnknown("arr:unk8", arr.unk8);
								json.WriteFieldValueUnknown("arr:unk10", arr.unk10);
								json.WriteFieldValueUnknown("arr:unk18", arr.unk18);
								json.WriteFieldValueUnknown("arr:unk20", arr.unk20);
								json.WriteFieldValueUnknown("arr:unk28", arr.unk28);
								json.WriteFieldValueUnknown("arr:unk30", arr.unk30);
								json.WriteFieldValueUnknown("arr:unk38", arr.unk38);
								json.WriteFieldValueUnknown("arr:unk40", arr.unk40);
							}
							json.EndObject();
						}

						json.EndArray();
					}

				}
				/*
				
				WeaponDefObj21329beb var_21329beb[4];
				*/
			}
			json.EndObject();

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_WEAPON };
}