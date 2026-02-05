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


	// get a better name for the keys
	std::unordered_map<uint64_t, const char*> knownKeys{
		{ hash::Hash64("aid"), "ai_interaction" },
		{ hash::Hash64("ait"), "ai_type" },
		{ hash::Hash64("anm"), "anim" },
		{ hash::Hash64("asc"), "asset_stream_collection" },
		{ hash::Hash64("atr"), "anim_tree" },
		{ hash::Hash64("bs2"), "blend_space_2d" },
		{ hash::Hash64("cam"), "camera" },
		{ hash::Hash64("ccr"), "client_character" },
		{ hash::Hash64("cha"), "character" },
		{ hash::Hash64("cin"), "cinematic_motion" },
		{ hash::Hash64("cmo"), "camo" },
		{ hash::Hash64("cmp"), "callout_marker_ping" },
		{ hash::Hash64("cnv"), "conversation" },
		{ hash::Hash64("cob"), "carry_object" },
		{ hash::Hash64("con"), "contrail_data" },
		{ hash::Hash64("csc"), "cinematic_scene" },
		{ hash::Hash64("cur"), "curve" },
		{ hash::Hash64("eca"), "esc_asset" },
		{ hash::Hash64("emo"), "emote" },
		{ hash::Hash64("exx"), "execution" },
		{ hash::Hash64("gbp"), "generic_blueprint" },
		{ hash::Hash64("ges"), "gesture" },
		{ hash::Hash64("gpr"), "gameprops" },
		{ hash::Hash64("gwt"), "grapple_weapon_tunables" },
		{ hash::Hash64("hbs"), "scriptbundle_health_bar_settings" },
		{ hash::Hash64("hic"), "head_icon" },
		{ hash::Hash64("hnt"), "hint_string" },
		{ hash::Hash64("hol"), "hud_outline" },
		{ hash::Hash64("hwc"), "hwconfig" },
		{ hash::Hash64("img"), "image" },
		{ hash::Hash64("isk"), "impulse_motion" },
		{ hash::Hash64("loc"), "localized" },
		{ hash::Hash64("lue"), "lui_elems" },
		{ hash::Hash64("lui"), "lui" },
		{ hash::Hash64("mat"), "material" },
		{ hash::Hash64("mdl"), "xmodel" },
		{ hash::Hash64("mft"), "material_float_table" },
		{ hash::Hash64("mfx"), "material_sfx_table" },
		{ hash::Hash64("mic"), "minimap_icon" },
		{ hash::Hash64("mov"), "moving_platform" },
		{ hash::Hash64("nms"), "ai_anim_set" },
		{ hash::Hash64("obj"), "objective" },
		{ hash::Hash64("par"), "parachute_data" },
		{ hash::Hash64("particlesystem"), "particle_system" },
		{ hash::Hash64("pas"), "player_asm" },
		{ hash::Hash64("pfb"), "pbg_postfx_bundle" },
		{ hash::Hash64("pla"), "player_look_at_profile" },
		{ hash::Hash64("pnv"), "pbg_postfx_bundle_nvt" },
		{ hash::Hash64("qed"), "netconststrings" },
		{ hash::Hash64("rmb"), "rumble" },
		{ hash::Hash64("rmg"), "rumble_graph" },
		{ hash::Hash64("rop"), "reactive_operator" },
		{ hash::Hash64("sbactivitydefinition"), "scriptbundle_activity_definition" },
		{ hash::Hash64("sbc"), "scriptbundle_hovercard" },
		{ hash::Hash64("sbd"), "scriptable" },
		{ hash::Hash64("sbe"), "scriptbundle_equipment" },
		{ hash::Hash64("sbgobblegum"), "scriptbundle_gobblegum" },
		{ hash::Hash64("sbh"), "scriptbundle_splash" },
		{ hash::Hash64("sbitemspawnentry"), "scriptbundle_item_spawn_entry" },
		{ hash::Hash64("sbk"), "scriptbundle_killstreak" },
		{ hash::Hash64("sbmiscmessages"), "scriptbundle_misc_message" },
		{ hash::Hash64("sbmissiondata"), "scriptbundle_mission_data" },
		{ hash::Hash64("sbmissionstep"), "scriptbundle_mission_step" },
		{ hash::Hash64("sbprecache_vehiclebundle"), "scriptbundle_vehicle" },
		{ hash::Hash64("sbs"), "scriptbundle_super" },
		{ hash::Hash64("sbscene"), "scriptbundle_scene" },
		{ hash::Hash64("sbspawnselectionmaptableentry"), "scriptbundle_spawn_selection_maptable_entry" },
		{ hash::Hash64("sbtalkinggun"), "scriptbundle_talking_gun" },
		{ hash::Hash64("sbuiactivityinfo"), "scriptbundle_ui_activity_info" },
		{ hash::Hash64("sbwc"), "scriptbundle_weapon_charms_data" },
		{ hash::Hash64("sev"), "sound_event" },
		{ hash::Hash64("shk"), "shock" },
		{ hash::Hash64("sic"), "status_icon" },
		{ hash::Hash64("skw"), "shockwave" },
		{ hash::Hash64("spr"), "spray" },
		{ hash::Hash64("stk"), "sticker" },
		{ hash::Hash64("sut"), "suit" },
		{ hash::Hash64("tag"), "tag" },
		{ hash::Hash64("tre"), "trigger_effect" },
		{ hash::Hash64("vcm"), "vehicle_camo" },
		{ hash::Hash64("veh"), "vehicle" },
		{ hash::Hash64("void"), "void" },
		{ hash::Hash64("vsf"), "vision_suffix" },
		{ hash::Hash64("vrs"), "vehicle_rev_sfx" },
		{ hash::Hash64("vsn"), "vision" },
		{ hash::Hash64("vss"), "volumetric_smoke_settings" },
		{ hash::Hash64("wep"), "weapon" },
		{ hash::Hash64("wma"), "world_model_attachment" },
		{ hash::Hash64("xac"), "anim_curve" },
		{ hash::Hash64("xca"), "xcam" },
		{ hash::Hash64("xcm"), "xcomposite_model" },
	};

	const char* MapKnownKey(const char* keyname) {
		auto it{ knownKeys.find(hash::Hash64(keyname)) };
		if (it != knownKeys.end()) {
			return it->second;
		}
		return keyname;
	}

	typedef uint32_t NetConstStringsType;

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

#define __STRUCTTXT(name, def) def \
constexpr const char* name = #def;


	__STRUCTTXT(NCSInfoStr,
		struct NCSInfo {
		const char* precache;
		const char* name;
		SatAssetType type;
		const char* bundleCategory;
		uint32_t unk20;
		bool unk24;
	};
	static_assert(sizeof(NCSInfo) == 0x28);
		)

#undef __STRUCTTXT

	class NetConstStringNameFinder {

		NCSInfo* info{};
		size_t infoCount{};
	public:

		void Init(fastfile::FastFileOption& opt) {
			info = nullptr;
			hook::module_mapper::Module& mod{ opt.GetGameModule() };

			hook::scan_container::ScanContainer& scan{ mod.GetScanContainer() };

			void (*InitNCSInfo)() {};
			NCSInfo* s_netConstStringTypeAssetData{};
			try {

				InitNCSInfo = scan.ScanSingle("48 89 5C 24 10 48 89 7C 24 18 55 48 8B EC 48 83 EC 20 48 8D", "InitNCSInfo").GetPtr<void(*)()>();
				s_netConstStringTypeAssetData = scan.ScanSingle("48 89 05 ?? ?? ?? ?? C6 45 10 00 E8 ?? ?? ?? ?? 33 FF", "s_netConstStringTypeAssetData").GetRelative<int32_t, NCSInfo*>(3);
			}
			catch (std::runtime_error& err) {
				LOG_WARNING("Can't load NSC string data: {}", err.what());
				return;
			}
			LOG_TRACE("InitNCSInfo: {}", hook::library::CodePointer{ InitNCSInfo });
			LOG_TRACE("s_netConstStringTypeAssetData: {}", hook::library::CodePointer{ s_netConstStringTypeAssetData });
			
			try {
				InitNCSInfo();
			}
			catch (...) {
				LOG_WARNING("Can't load NSC string data: severe error when calling InitNCSInfo()");
				return;
			}


			info = s_netConstStringTypeAssetData;
			std::filesystem::path dir{ opt.m_output / gamePath / "code"};
			std::filesystem::create_directories(dir);
			std::filesystem::path nscCsv{ dir / std::format("{}_ncs.csv", gameDumpId) };
			std::filesystem::path nscCpp{ dir / std::format("{}_ncs.cpp", gameDumpId) };
			std::filesystem::path nschpp{ dir / std::format("{}_ncs.hpp", gameDumpId) };
			utils::OutFileCE nscCsvOs{ nscCsv, true };
			utils::OutFileCE nscCppOs{ nscCpp, true };
			utils::OutFileCE nschppOs{ nschpp, true };
			
			auto& assetNames{ fastfile::handlers::bo7::GetAssetNames() };

			nscCsvOs << "id,name,type,precache,bundleCategory,unk24";
			nscCppOs 
				<< "#include \"" << gameDumpId << "_ncs.hpp\"\n\n"
				<< "NCSInfo s_netConstStringTypeAssetData[] {";

			nschppOs
				<< "#include \"" << gameDumpId << "_ids.hpp\"\n\n"
				<< NCSInfoStr << "\n\n"
				<< "enum NetConstStringsType : uint32_t {";
			size_t i;
			for (i = 0; ; i++) {
				if (info[i].name <= **mod || info[i].name > (*mod)[0x20000000]) {
					break; // invalid name
				}

				nscCsvOs
					<< "\n" << std::dec << i
					<< "," << utils::PtrOrElse(info[i].name, "null")
					<< "," << assetNames.GetTypeName(info[i].type)
					<< "," << utils::PtrOrElse(info[i].precache, "null")
					<< "," << utils::PtrOrElse(info[i].bundleCategory, "null")
					<< "," << (info[i].unk24 ? "true" : "false")
					;

				nscCppOs 
					<< "\n    { .precache = \"" << info[i].precache << "\""
					<< ", .name = \"" << info[i].name << "\""
					<< ", .type = " << hppPrefix << assetNames.GetCppName(info[i].type)
					<< ", .bundleCategory = \"" << utils::PtrOrElse(info[i].bundleCategory, "null") << "\""
					<< ", .unk20 = " << info[i].unk20
					<< ", .unk24 = " << (info[i].unk24 ? "true" : "false")
					<< " },";
				nschppOs << "\n    NCST_" << core::strings::GetCppIdentifier(MapKnownKey(info[i].name)) << " = 0x" << std::hex << i << ", // " << info[i].name << " " << std::dec << i << " " << assetNames.GetTypeName(info[i].type);
				
			}
			nschppOs << "\n    NCST_COUNT = 0x" << std::hex << i << ", // " << std::dec << i;
			infoCount = i;
			nschppOs << "\n};\n";
			nscCppOs << "\n};\n";
		}

		const char* GetTypeName(NetConstStringsType type) {
			if (info && type <= infoCount) {
				if (type == infoCount) {
					return "void";
				}
				return MapKnownKey(info[type].name);
			}
			return utils::va("%d", (int)type);
		}

		void LoadHash(const char* ffname) {
			if (!ffname) {
				// common ones
				for (const char* nameSuffix : nameSuffixes) {
					for (size_t i = 0; i < infoCount; i++) {
						const char* str{ utils::va("ncs_%s_%s", info[i].name, nameSuffix)};
						hashutils::AddPrecomputed(hash::HashIWAsset(str), str, true);
					}
				}
			}
			else {
				// ff specific
				for (size_t i = 0; i < infoCount; i++) {
					const char* str{ utils::va("ncs_%s_%s", info[i].name, ffname) };
					hashutils::AddPrecomputed(hash::HashIWAsset(str), str, true);
				}
			}
		}
	};

	class ImplWorker : public Worker {
		using Worker::Worker;

		NetConstStringNameFinder finder{};

		void PreLoadWorker(fastfile::FastFileContext* ctx) override {
			if (!ctx) {
				finder.Init(fastfile::GetCurrentOptions());
			}
			finder.LoadHash(ctx ? ctx->ffname : nullptr);
		}

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			NetConstStrings* asset{ (NetConstStrings*)ptr };
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueString("type", finder.GetTypeName(asset->type));
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
			LOG_OPT_INFO("Dump netconststrings {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, SatHashAssetType, Worker> impl{ GetWorkers(), SatHashAssetType::SATH_ASSET_NETCONSTSTRINGS, sizeof(NetConstStrings) };
}