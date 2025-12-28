#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo7.hpp>

namespace {
	using namespace fastfile::handlers::bo7;
	union NetConstString {
		const char* str;
		XHash64 hash;
	};

	const char* nameSuffixes[]{
		"level",
		"level_entities",
	};
	const char* typeAbrs[]{
		"mdl",		"mat",		"img",		"rmb",
		"rmg",		"veh",		"particlesystem",
		"loc",		"shk",		"tag",		"sic",
		"hic",		"obj",		"mic",		"wep",
		"hnt",		"anm",		"nms",		"lui",
		"sut",		"cam",		"hol",		"ges",
		"pfb",		"pnv",		"vsn",		"vsf",
		"atr",		"ccr",		"xac",		"cin",
		"bs2",		"cmo",		"pas",		"ait",
		"sbd",		"xcm",		"exx",		"emo",
		"cob",		"stk",		"vcm",		"qed",
		"cur",		"xca",		"tre",		"mov",
		"sev",		"rop",		"eca",		"cnv",
		"cha",		"wma",		"gbp",		"spr",
		"skw",		"pla",		"par",		"vss",
		"gwt",		"csc",		"asc",		"gpr",
		"cmp",		"aid",		"mfx",		"lue",
		"con",		"isk",		"sbe",		"sbs",
		"sbk",		"sbh",		"sbwc",		"hbs",
		"hwc",		"sbc",		"sbscene",
		"sbprecache_vehiclebundle",
		"sbmissionstep",
		"sbmiscmessages",
		"sbgobblegum",
		"sbitemspawnentry",
		"sbtalkinggun",
		"sbmissiondata",
		"sbuiactivityinfo",
		"sbactivitydefinition",
		"sbspawnselectionmaptableentry",
		"void",
	};

	enum NetConstStringsType : uint32_t {
		NCST_XMODEL = 0, // mdl
		NCST_MATERIAL = 1, // mat
		NCST_IMAGE = 2, // img
		NCST_RUMBLE = 3, // rmb
		NCST_RUMBLE_GRAPH = 4, // rmg
		NCST_VEHICLE = 5, // veh
		NCST_PARTICLE_SYSTEM = 6, // particlesystem
		NCST_LOCALIZED = 7, // loc
		NCST_SHOCK = 8, // shk
		NCST_TAG = 9, // tag
		NCST_STATUS_ICON = 10, // sic
		NCST_HEAD_ICON = 11, // hic
		NCST_OBJECTIVE = 12, // obj
		NCST_MINIMAP_ICON = 13, // mic
		NCST_WEAPON = 14, // wep
		NCST_HINT_STRING = 15, // hnt
		NCST_ANIM = 16, // anm
		NCST_AI_ANIM_SET = 17, // nms
		NCST_LUI = 18, // lui
		NCST_SUIT = 19, // sut
		NCST_CAMERA = 20, // cam
		NCST_HUD_OUTLINE = 21, // hol
		NCST_GESTURE = 22, // ges
		NCST_PBG_POSTFX_BUNDLE = 23, // pfb
		NCST_PBG_POSTFX_BUNDLE_NVT = 24, // pnv
		NCST_VISION = 25, // vsn
		NCST_VISION_SUFFIX = 26, // vsf
		NCST_ANIM_TREE = 27, // atr
		NCST_CLIENT_CHARACTER = 28, // ccr
		NCST_ANIM_CURVE = 29, // xac
		NCST_CINEMATIC_MOTION = 30, // cin
		NCST_BLEND_SPACE_2D = 31, // bs2
		NCST_CAMO = 32, // cmo
		NCST_PLAYER_ASM = 33, // pas
		NCST_AI_TYPE = 34, // ait
		NCST_SCRIPTABLE = 35, // sbd
		NCST_XCOMPOSITE_MODEL = 36, // xcm
		NCST_EXECUTION = 37, // exx
		NCST_EMOTE = 38, // emo
		NCST_CARRY_OBJECT = 39, // cob
		NCST_STICKER = 40, // stk
		NCST_VEHICLE_CAMO = 41, // vcm
		NCST_NETCONSTSTRINGS = 42, // qed
		NCST_CURVE = 43, // cur
		NCST_XCAM = 44, // xca
		NCST_TRIGGER_EFFECT = 45, // tre
		NCST_MOVING_PLATFORM = 46, // mov
		NCST_SOUND_EVENT = 47, // sev
		NCST_REACTIVE_OPERATOR = 48, // rop
		NCST_ESC_ASSET = 49, // eca
		NCST_CONVERSATION = 50, // cnv
		NCST_CHARACTER = 51, // cha
		NCST_WORLD_MODEL_ATTACHMENT = 52, // wma
		NCST_GENERIC_BLUEPRINT = 53, // gbp
		NCST_SPRAY = 54, // spr
		NCST_SHOCKWAVE = 55, // skw
		NCST_PLAYER_LOOK_AT_PROFILE = 56, // pla
		NCST_PARACHUTE_DATA = 57, // par
		NCST_VOLUMETRIC_SMOKE_SETTINGS = 58, // vss
		NCST_GRAPPLE_WEAPON_TUNABLES = 59, // gwt
		NCST_CINEMATIC_SCENE = 60, // csc
		NCST_ASSET_STREAM_COLLECTION = 61, // asc
		NCST_GAMEPROPS = 62, // gpr
		NCST_CALLOUT_MARKER_PING = 63, // cmp
		NCST_AI_INTERACTION = 64, // aid
		NCST_MATERIAL_SFX_TABLE = 65, // mfx
		NCST_LUI_ELEMS = 66, // lue
		NCST_CONTRAIL_DATA = 67, // con
		NCST_IMPULSE_MOTION = 68, // isk
		NCST_SCRIPTBUNDLE_EQUIPMENT = 69, // sbe
		NCST_SCRIPTBUNDLE_SUPER = 70, // sbs
		NCST_SCRIPTBUNDLE_KILLSTREAK = 71, // sbk
		NCST_SCRIPTBUNDLE_SPLASH = 72, // sbh
		NCST_SCRIPTBUNDLE_WEAPON_CHARMS_DATA = 73, // sbwc
		NCST_SCRIPTBUNDLE_HEALTH_BAR_SETTINGS = 74, // hbs
		NCST_HWCONFIG = 75, // hwc
		NCST_SCRIPTBUNDLE_HOVERCARD = 76, // sbc
		NCST_SCRIPTBUNDLE_SCENE = 77, // sbscene
		NCST_SCRIPTBUNDLE_VEHICLE = 78, // sbprecache_vehiclebundle
		NCST_SCRIPTBUNDLE_MISSION_STEP = 79, // sbmissionstep
		NCST_SCRIPTBUNDLE_MISC_MESSAGE = 80, // sbmiscmessages
		NCST_SCRIPTBUNDLE_GOBBLEGUM = 81, // sbgobblegum
		NCST_SCRIPTBUNDLE_ITEM_SPAWN_ENTRY = 82, // sbitemspawnentry
		NCST_SCRIPTBUNDLE_TALKING_GUN = 83, // sbtalkinggun
		NCST_SCRIPTBUNDLE_MISSION_DATA = 84, // sbmissiondata
		NCST_SCRIPTBUNDLE_UI_ACTIVITY_INFO = 85, // sbuiactivityinfo
		NCST_SCRIPTBUNDLE_ACTIVITY_DEFINITION = 86, // sbactivitydefinition
		NCST_SCRIPTBUNDLE_SPAWN_SELECTION_MAPTABLE_ENTRY = 87, // sbspawnselectionmaptableentry
		NCST_VOID = 88, // void
	};

	const char* typeNames[] {
		"xmodel",
		"material",
		"image",
		"rumble",
		"rumble_graph",
		"vehicle",
		"particle_system",
		"localized",
		"shock",
		"tag",
		"status_icon",
		"head_icon",
		"objective",
		"minimap_icon",
		"weapon",
		"hint_string",
		"anim",
		"ai_anim_set",
		"lui",
		"suit",
		"camera",
		"hud_outline",
		"gesture",
		"pbg_postfx_bundle",
		"pbg_postfx_bundle_nvt",
		"vision",
		"vision_suffix",
		"anim_tree",
		"client_character",
		"anim_curve",
		"cinematic_motion",
		"blend_space_2d",
		"camo",
		"player_asm",
		"ai_type",
		"scriptable",
		"xcomposite_model",
		"execution",
		"emote",
		"carry_object",
		"sticker",
		"vehicle_camo",
		"netconststrings",
		"curve",
		"xcam",
		"trigger_effect",
		"moving_platform",
		"sound_event",
		"reactive_operator",
		"esc_asset",
		"conversation",
		"character",
		"world_model_attachment",
		"generic_blueprint",
		"spray",
		"shockwave",
		"player_look_at_profile",
		"parachute_data",
		"volumetric_smoke_settings",
		"grapple_weapon_tunables",
		"cinematic_scene",
		"asset_stream_collection",
		"gameprops",
		"callout_marker_ping",
		"ai_interaction",
		"material_sfx_table",
		"lui_elems",
		"contrail_data",
		"impulse_motion",
		"scriptbundle_equipment",
		"scriptbundle_super",
		"scriptbundle_killstreak",
		"scriptbundle_splash",
		"scriptbundle_weapon_charms_data",
		"scriptbundle_health_bar_settings",
		"hwconfig",
		"scriptbundle_hovercard",
		"scriptbundle_scene",
		"scriptbundle_vehicle",
		"scriptbundle_mission_step",
		"scriptbundle_misc_message",
		"scriptbundle_gobblegum",
		"scriptbundle_item_spawn_entry",
		"scriptbundle_talking_gun",
		"scriptbundle_mission_data",
		"scriptbundle_ui_activity_info",
		"scriptbundle_activity_definition",
		"scriptbundle_spawn_selection_maptable_entry",
		"void",
	};

	enum NetConstStringsSource : byte {
		NCSS_CODE = 0,
		NCSS_GLOBAL = 1,
		NCSS_INGAME = 2,
		NCSS_LEVEL = 3,
		NCSS_UNK4 = 4,
		NCSS_VOID = 5,
	};

	const char* sourceNames[]{
		"code",
		"global",
		"ingame",
		"level",
		"unk4",
		"void",
	};

	struct NetConstStrings {
		uint64_t name;
		NetConstStringsType type;
		NetConstStringsSource source;
		bool isHashed;
		uint16_t unk0e;
		bool localized;
		uint32_t stringsCount;
		uint32_t checksum;
		uint32_t unk1c;
		NetConstString* strings;
	}; static_assert(sizeof(NetConstStrings) == 0x28);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void GenDefaultXHashes(fastfile::FastFileContext* ctx) override {
			if (!ctx) {
				// common ones
				for (const char* nameSuffix : nameSuffixes) {
					for (const char* typeAbr : typeAbrs) {
						const char* str{ utils::va("ncs_%s_%s", typeAbr, nameSuffix) };
						hashutils::AddPrecomputed(hash::HashIWAsset(str), str, true);
					}
				}
			}
			else {
				// ff specific
				for (const char* typeAbr : typeAbrs) {
					const char* str{ utils::va("ncs_%s_%s", typeAbr, ctx->ffname) };
					hashutils::AddPrecomputed(hash::HashIWAsset(str), str, true);
				}
			}
		}

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			NetConstStrings* asset{ (NetConstStrings*)ptr };
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			if (asset->type >= 0 && asset->type < ARRAYSIZE(typeNames)) {
				json.WriteFieldValueString("type", typeNames[asset->type]);
			}
			else {
				json.WriteFieldValueNumber<uint32_t>("type", asset->type);
			}
			if (asset->source >= 0 && asset->source < ARRAYSIZE(sourceNames)) {
				json.WriteFieldValueString("source", sourceNames[asset->source]);
			}
			else {
				json.WriteFieldValueNumber<uint32_t>("source", asset->source);
			}
			json.WriteFieldValueBool("isHashed", asset->isHashed);
			json.WriteFieldValueBool("localized", asset->localized);
			json.WriteFieldValueNumber<uint32_t>("checksum", asset->checksum);
			if (opt.testDump) {
				json.WriteFieldValueUnknown("unk0e", asset->unk0e); // empty?
				json.WriteFieldValueUnknown("unk1c", asset->unk1c); // empty?
			}

			json.WriteFieldNameString("strings");
			json.BeginArray();
			if (asset->isHashed) {
				for (size_t i = 0; i < asset->stringsCount; i++) {
					json.WriteValueHash(asset->strings[i].hash);
				}
			}
			else {
				for (size_t i = 0; i < asset->stringsCount; i++) {
					json.WriteValueString(asset->strings[i].str);
				}
			}
			json.EndArray();
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "netconststrings"
				/ fastfile::GetCurrentContext().ffname
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump netconststrings {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, SatHashAssetType, Worker> impl{ GetWorkers(), SatHashAssetType::SATH_ASSET_NETCONSTSTRINGS, sizeof(NetConstStrings) };
}