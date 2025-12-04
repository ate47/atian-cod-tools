#include <includes.hpp>
#include <tools/bo7/bo7.hpp>
#include <core/bytebuffer.hpp>
#include <hook/module_mapper.hpp>
#include <games/cod/asset_names.hpp>


namespace tool::bo7 {
    namespace {
        const char* poolNames[]{
            "physicssfxeventasset",
            "physicsvfxeventasset",
            "physicsasset",
            "physicsfxpipeline",
            "physicsfxshape",
            "physicsdebugdata",
            "xanim",
            "xmodelsurfs",
            "xmodel",
            "mayhem",
            "material",
            "computeshader",
            "libshader",
            "techset",
            "image",
            "soundglobalvolmod",
            "soundglobalentchannel",
            "soundglobalcontext",
            "soundglobalwhizby",
            "soundglobalbulletcrack",
            "soundglobalocclusion",
            "soundglobalsurfaceinfo",
            "soundglobalcurve",
            "soundglobaldoppler",
            "soundglobalnametable",
            "soundglobalweaponreflect",
            "soundbank",
            "soundbanktransient",
            "col_map",
            "com_map",
            "world_event_data",
            "glass_map",
            "aipaths",
            "navmesh",
            "tacgraph",
            "map_game_objects",
            "map_fx_objects",
            "map_ents",
            "map_ents_trzone",
            "fx_map",
            "gfx_map",
            "gfx_map_trzone",
            "iesprofile",
            "lightdef",
            "gradingclut",
            "fogspline",
            "pbgbloom",
            "pbgcurve",
            "pbgcolor",
            "pbgeffects",
            "atmosphericfog",
            "volumetricfog",
            "pbgpostfxbundle",
            "pbgpostfxbundlenvt",
            "gesture",
            "localize",
            "attachment",
            "weapon",
            "particlesystem",
            "impactfx",
            "surfacefx",
            "aitype",
            "character",
            "xmodelalias",
            "rawfile",
            "gscobj",
            "gscgdb",
            "stringtable",
            "ddl",
            "tracer",
            "vehicle",
            "netconststrings",
            "luafile",
            "luadebugfile",
            "scriptable",
            "vectorfield",
            "particlesimanimation",
            "streaminginfo",
            "laser",
            "gameprops",
            "materialstandard",
            "ttf",
            "suit",
            "suitanimpackage",
            "camera",
            "hudoutline",
            "rumble",
            "rumblegraph",
            "animpkg",
            "sfxpkg",
            "vfxpkg",
            "footstepvfx",
            "behaviortree",
            "behaviorsequencer",
            "sightconfig",
            "sightconfigtemplate",
            "aianimset",
            "aiasm",
            "proceduralbones",
            "dynamicbones",
            "reticle",
            "xanimcurve",
            "coverselector",
            "battlechattertree",
            "battlechatterevent",
            "enemyselector",
            "clientcharacter",
            "clothasset",
            "cinematicmotion",
            "accessory",
            "locdmgtable",
            "bullet_penetration",
            "scriptbundle",
            "blendspace2d",
            "xcam",
            "camo",
            "xcompositemodel",
            "xmodeldetailcollision",
            "streamtreeoverride",
            "keyvaluepairs",
            "stterrain",
            "vhmdata",
            "vtmdata",
            "collisiontile",
            "execution",
            "carryobject",
            "soundbanklist",
            "weaponaccuracy",
            "decalvolumematerial",
            "decalvolumemask",
            "dynentitylist",
            "fx_map_trzone",
            "volumetricheightmap",
            "dlogschema",
            "edgelist",
            "standaloneumbratome",
            "xboneset",
            "ragdollasset",
            "physicsbonegraph",
            "curve",
            "skeletonconstraints",
            "triggereffect",
            "weapontrigger",
            "volumetriccloud",
            "codcasterdata",
            "watersystem",
            "waterbuoyancy",
            "keybinds",
            "calloutmarkerping",
            "lightstate",
            "radianttelemetry",
            "aimarkup",
            "aimarkup_generated",
            "scenario",
            "ai_interaction",
            "mapvoxelizedstateformat",
            "watervis",
            "gametype",
            "gametypetable",
            "sndmodifier",
            "weaponblueprint",
            "attachmentblueprint",
            "movingplatform",
            "hwconfig",
            "timelapsesky",
            "hwconfiguratorinfo",
            "objectivedata",
            "font",
            "motionmatchingfeatures",
            "motionmatchingset",
            "gametypestatdata",
            "fonticon",
            "calloutmarkerpingwheel",
            "hwconfigvar",
            "zivart",
            "movie",
            "mapinfo",
            "maptable",
            "globalmaptable",
            "achievement",
            "achievementlist",
            "materialdebugdata",
            "scriptablevariant",
            "leagueplayseasons",
            "settingcontext",
            "ai_eventlist",
            "soundevent",
            "calloutmarkerpingmetadata",
            "project",
            "projecttable",
            "gamemode",
            "sndasset",
            "gfxumbratome",
            "audiovisualizer",
            "materialanimationparams",
            "nameplatesettings",
            "reactiveaudioparams",
            "reactivevfxpackage",
            "materialsfxtable",
            "footstepsfxtable",
            "reactivestageset",
            "foliageimpactsfxtable",
            "impactsfxtable",
            "aiimpactvfxtable",
            "typeinfo",
            "handplantsfxtable",
            "sndtable",
            "equipmentsfx",
            "soundsubmix",
            "storagefile",
            "ecsasset",
            "trackerfootstepfx",
            "playerspawnsettings",
            "playerspawninfluencer",
            "soundspeakermap",
            "reverbpreset",
            "aishootstyleslist",
            "operatorlist",
            "operator",
            "operatorskin",
            "dismemberment",
            "conversation",
            "xanimnode",
            "sndcurve",
            "ttlos",
            "materialtintanimationparams",
            "materialuvanimationparams",
            "materialcamoanimationparams",
            "materialanimation",
            "impactfxtable",
            "impacttypetoimpactfxtable",
            "reactiveoperator",
            "weathervolume",
            "vehicletrick",
            "reactiveaudiopackage",
            "ambientsfxpackage",
            "objectstoreproject",
            "objectstoregamemode",
            "proceduralbonelodsettings",
            "genericblueprint",
            "hwconfigvargroup",
            "hwconfigtieredgroup",
            "soundcone",
            "sndmasterpreset",
            "sndfilter",
            "sndfutz",
            "xanimtree",
            "socialplayerprofile",
            "localizeassetentrydev",
            "mldeformer",
            "mldeformerbinding",
            "xanimparammodifier",
            "aispline",
            "emote",
            "worldmodelattachment",
            "materialflipbookparams",
            "materialuvdistortionparams",
            "chroma",
            "vehicletunables",
            "sndweaponreflectiondeltatweaks",
            "sndadsr",
            "sndbreathstate",
            "vehicleblueprint",
            "charcollbounds",
            "weather",
            "weatherstate",
            "weathermap",
            "weathertile",
            "sndtimescale",
            "sndreverb",
            "aiobstacle",
            "spray",
            "sndocclusion",
            "sndadsrzone",
            "xanimset",
            "xanimsetstate",
            "xanimsetalias",
            "xanimsetparameters",
            "grounddeformationvis",
            "aibcsoundaliastable",
            "airoutine",
            "aiactionsettings",
            "sndacoustics",
            "scriptbrushmodel",
            "weathersurfacetypemapping",
            "compassdata",
            "shockwave",
            "bonetoggleparams",
            "playerlookatprofile",
            "weatherenvironmentsfxpackage",
            "ai_token_collections",
            "ai_token_definitions",
            "ai_token_stealing_definitions",
            "ai_token_types",
            "posematchingtransitionset",
            "posematchingasmconfig",
            "deployableweaponconfig",
            "aianimsettings",
            "parachutedata",
            "downloadgroupset",
            "sndambient",
            "sndambientelement",
            "sndfullocclusion",
            "weatherimpactsfxtable",
            "indexedlisttype",
            "footstepsfxoverride",
            "volumetricsmokesettings",
            "timeline",
            "radar",
            "ikparameters",
            "indoorexclusiontile",
            "contraildata",
            "vehleanoutanimcurveset",
            "visionset",
            "objectstoretitleidgroup",
            "sndzoneweather",
            "weatherprecipitationsfxpackage",
            "gobo",
            "footstepsurfacetable",
            "weatherdistantsfxpackage",
            "hyperpose",
            "iktarget",
            "sndmusicset",
            "physicsmateriallibrary",
            "physicsbodyqualitylibrary",
            "physicsmotionpropertieslibrary",
            "physicsglobaltypecompendium",
            "impulsemotion",
            "weatherwind",
            "bestplaycam",
            "aidebugdata",
            "bonefollowermappinggraph",
            "soundbankpackage",
            "perlinnoise",
            "botbtconstants",
            "dismembermenttype",
            "deatheffect",
            "weathersoundthresholds",
            "sndocclusionsetting",
            "weaponsndreflectionzone",
            "weaponsndreflections",
            "objectstoreddlconfig",
            "objectstoreddlconfiggroup",
            "grappleweapontunables",
            "unlockfeedconfig",
            "cinematicscene",
            "customdamagepattern",
            "objective",
            "codcasterteamcustomization",
            "hyperposeasmasset",
            "weapontunables",
            "bulletpenetrationset",
            "lensflare",
            "mlmodel",
            "assetstreamcollection",
            "sndvolmod",
            "importsettingconfig",
            "omnvar",
            "whizbysfx",
            "sndcontext",
            "bulletcracksfx",
            "heatmapradar",
            "unlockfeeditem",
            "codcastergameplaystat",
            "sndchannels",
            "omnvarlist",
            "materialfloattable",
            "hyperposegamefeatures",
            "xanimpayload",
            "grenadeutilityspec",
            "spatialcinematics",
            "footstepdecaldescription",
            "vehiclerevsfx",
            "shock",
            "spikeprofilerconfig",
            "spikeprofilersettings",
        };
    }

    const char* PoolName(SatHashAssetType type) {
        static std::unordered_map<uint32_t, const char*> maps{};
        if (maps.empty()) {
            for (const char* s : poolNames) {
                maps[PoolId(s)] = s;
            }
        }
        auto it{ maps.find(type) };
        if (it == maps.end()) {
            return hashutils::ExtractTmp("hash", type);
        }
        return it->second;
    }

    SatHashAssetType PoolId(const char* name) {
        return (SatHashAssetType)hash::HashX32(name);
    }

    constexpr const char* defaultBoLuaFuncObject = "default";
    static struct {
        std::map<const char*, std::map<uint64_t, void*>> funcs{};
        const char* currentFunc{ defaultBoLuaFuncObject };

        void Cleanup() {
            funcs.clear();
            boLuaData.currentFunc = defaultBoLuaFuncObject;
        }
    } boLuaData;

    void NullStub() {}
    template<typename T, T val>
    T ReturnStub() { return val; }

    void BoLoadLuaPop(void* vm, size_t count) {
        boLuaData.currentFunc = defaultBoLuaFuncObject;
    }
    void BoLoadLuaPush(void* vm, int a2, const char* category) {
        boLuaData.currentFunc = category;
    }

    void BoLoadLuaFunc(void* vm, uint64_t hash, void* func) {
        boLuaData.funcs[boLuaData.currentFunc][hash] = func;
    }
    void BoLoadLuaFuncStr(void* vm, const char* str, void* func) {
        uint64_t hash{ hash::Hash64A(str) };
        hashutils::AddPrecomputed(hash, str, true);
        BoLoadLuaFunc(vm, hash, func);
    }

    int bo7_lua_dump(int argc, const char* argv[]) {
        if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

        std::filesystem::path exe{ argv[2] };

        hook::module_mapper::Module mod{ true };

        LOG_INFO("Loading module {}", exe.string());
        if (!mod.Load(exe) || !mod) {
            LOG_ERROR("Can't load module");
            return tool::BASIC_ERROR;
        }

        LOG_INFO("Loaded");

        boLuaData.Cleanup();

        // remove lua things if false it creates some useless calls
        hook::memory::RedirectJmp(mod->ScanSingle("E8 ?? ?? ?? ?? 48 63 5D ?? 83 F8", "BoLuaUnkReturn").GetRelative<int32_t>(1), ReturnStub<bool, true>); // 88F5390

        // registry function
        hook::memory::RedirectJmp(mod->ScanSingle("E8 ?? ?? ?? ?? 48 8B 9C 24 ?? ?? ?? ?? 48 8D 4C 24 ?? E8 ?? ?? ?? ?? 48 8B 8D", "BoLoadLuaPop").GetRelative<int32_t>(1), BoLoadLuaPop); // 88F5390
        hook::memory::RedirectJmp(mod->ScanSingle("E8 ?? ?? ?? ?? BA FE FF FF FF 48 8B CE E8 ?? ?? ?? ?? 48 8B BC 24", "BoLoadLuaPush").GetRelative<int32_t>(1), BoLoadLuaPush); // 88F5390

        void* stub{ mod->ScanSingle("E8 ?? ?? ?? ?? 48 BA EA C8 1A 47 1B 60 F9 08", "BoLoadLuaFunc").GetRelative<int32_t>(1) }; // BoLoadLuaFunc(luastate*, 0xhash, func)
        void* stubStr{ mod->ScanSingle("E8 ?? ?? ?? ?? 4C 8D 05 ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 49 8B CD", "BoLoadLuaFuncStr").GetRelative<int32_t>(1) }; // BoLoadLuaFuncStr(luastate*, "str", func)

        LOG_INFO("redirect hash . {}", hook::library::CodePointer{ stub });
        LOG_INFO("redirect str .. {}", hook::library::CodePointer{ stubStr });

        hook::memory::RedirectJmp(
            stub,
            BoLoadLuaFunc
        );

        hook::memory::RedirectJmp(
            stubStr,
            BoLoadLuaFuncStr
        );

        // 71b36f0bb44547d4 / bb69baec8be93d50 ?
        mod->ScanSingle("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 48 89 4C 24 ?? 57 41 54 41 55 41 56 41 57 48 83 EC ?? 4C 8D 05 ?? ?? ?? ?? BA EE D8 FF FF E8")
            .GetPtr<void(*)(void*)>()(nullptr);// load_funcs(luastate*)

        utils::OutFileCE os{ argv[3], true };

        LOG_INFO("Dumping functions in {}", argv[3]);

        os << "object,name,offset";

        for (auto& [cat, funcs] : boLuaData.funcs) {
            for (auto& [name, func] : funcs) {
                os << "\n" << cat << "," << hashutils::ExtractTmp("hash", name) << "," << hook::library::CodePointer{ func };
            }
        }

        boLuaData.Cleanup();


        return tool::OK;
    }

    int bo7_ncs_dump(int argc, const char* argv[]) {
        if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

        std::filesystem::path exe{ argv[2] };

        hook::module_mapper::Module mod{ true };

        LOG_INFO("Loading module {}", exe.string());
        if (!mod.Load(exe) || !mod) {
            LOG_ERROR("Can't load module");
            return tool::BASIC_ERROR;
        }

        games::cod::asset_names::AssetNames<SatHashAssetType, SatAssetType> names{ "physicssfxeventasset", "string", bo7::PoolId };

        names.InitMap(*mod);

        LOG_INFO("Loaded");

        void* stub{ mod->ScanSingle("E8 ?? ?? ?? ?? 48 BA EA C8 1A 47 1B 60 F9 08", "BoLoadLuaFunc").GetRelative<int32_t>(1) }; // BoLoadLuaFunc(luastate*, 0xhash, func)
        void* stubStr{ mod->ScanSingle("E8 ?? ?? ?? ?? 4C 8D 05 ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 49 8B CD", "BoLoadLuaFuncStr").GetRelative<int32_t>(1) }; // BoLoadLuaFuncStr(luastate*, "str", func)

        // init data
        mod->ScanSingle("40 55 53 56 57 41 56 41 57 48 8D 6C 24 D1 48 81 EC ?? ?? ?? ?? 48 B8")
            .GetPtr<void(*)()>()();

        struct NCSInfo {
            const char* precache;
            const char* name;
            SatAssetType type;
            const char* bundleCategory;
            uint32_t unk20;
            bool unk24;
        };
        static_assert(sizeof(NCSInfo) == 0x28);



        NCSInfo* nsci{ (NCSInfo*)&mod->ScanSingle("48 8D 3D ?? ?? ?? ?? BE ?? ?? ?? ?? 4C 8D 25 ?? ?? ?? ?? 4C 8D 2D").GetRelative<int32_t>(3)[-(int)offsetof(NCSInfo, name)] };
        LOG_INFO("load info at {}", hook::library::CodePointer{ nsci });

        utils::OutFileCE os{ argv[3], true };
        os << "id,name,type,precache,bundleCategory,unk24";

        for (size_t i = 0; i < 88; i++) {
            os
                << "\n" << std::dec << i
                << "," << utils::PtrOrElse(nsci[i].name, "null")
                << "," << names.GetTypeName(nsci[i].type)
                << "," << utils::PtrOrElse(nsci[i].precache, "null")
                << "," << utils::PtrOrElse(nsci[i].bundleCategory, "null")
                << "," << (nsci[i].unk24 ? "true" : "false")
                ;
        }

        LOG_INFO("dumped into {}", argv[3]);

        return tool::OK;
    }


    typedef uint32_t ScrString_t;

    struct {
        std::unordered_map<ScrString_t, std::string> strings{};

        void Clear() {
            strings.clear();
        }
    } constStrings;

    static ScrString_t* ConstStrings_SL_GetString(ScrString_t* ret, const char* string, int user, bool cst) {
        if (!string) {
            *ret = 0;
            return ret;
        }

        ScrString_t id{ (ScrString_t)(constStrings.strings.size() + 1) };
        constStrings.strings[id] = string;
        *ret = id;
        return ret;
    }


    // Get the cpp identifier for a pool name
    const char* GetCppIdentifier(std::string& str) {
        return utils::MapString(str.data(), [](char c) -> char {
            if (!(c == '_' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))) {
                return '_'; // remap bad char
            }

            return c;
        });
    }

    int bo7_cststr_dump(int argc, const char* argv[]) {
        if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

        std::filesystem::path exe{ argv[2] };

        hook::module_mapper::Module mod{ true };

        LOG_INFO("Loading module {}", exe.string());
        if (!mod.Load(exe) || !mod) {
            LOG_ERROR("Can't load module");
            return tool::BASIC_ERROR;
        }
        void(*SL_RegisterConstStrings)() { mod->ScanSingle("E8 ?? ?? ?? ?? 48 B8 94 0A 26 27 C3 45 FA FD", "SL_RegisterConstStrings").GetRelative<int32_t, void(*)()>(1) };
        if (!hook::library::ScanMatch(SL_RegisterConstStrings, "48 83 EC 28 33 D2 48 8D 0D ?? ?? ?? ?? 41 B8 ?? ?? ?? ??")) {
            throw std::runtime_error("Can't match register info");
        }
        void* SL_GetString{ mod->ScanSingle("E8 ?? ?? ?? ?? 8B 18 49 8B CC", "SL_GetString").GetRelative<int32_t, void*>(1) };
        hook::memory::RedirectJmp(SL_GetString, ConstStrings_SL_GetString);

        ScrString_t* scrConstStrings{ hook::library::GetRelative<int32_t, ScrString_t*>(SL_RegisterConstStrings, 9) };
        uint32_t count{ *(uint32_t*)((byte*)SL_RegisterConstStrings + 15) / sizeof(ScrString_t) };

        LOG_INFO("stub: {}, ptr: {}, count:{}", hook::library::CodePointer{ SL_RegisterConstStrings },hook::library::CodePointer{ scrConstStrings }, count);
        
        if (count > 10000) {
            throw std::runtime_error("Invalid count");
        }

        constStrings.Clear();

        SL_RegisterConstStrings();

        utils::OutFileCE os{ argv[3], true };
        os 
            << 
                "typedef uint32_t ScrString_t;\n"
                "struct ConstScrStrings {\n";

        std::unordered_map<uint64_t, size_t> counts{};

        for (size_t i = 0; i < count; i++) {
            ScrString_t scr{ scrConstStrings[i] };

            auto it{ constStrings.strings.find(scr) };

            if (it == constStrings.strings.end()) {
                utils::Padding(os, 1) << "uint32_t unk" << std::hex << (i * sizeof(scrConstStrings[i])) << ";\n";
                continue;
            }
            
            const char* def{ GetCppIdentifier(it->second) };
            size_t cnt{ counts[hash::Hash64A(def)]++ };

            utils::Padding(os, 1) << "ScrString_t ";
            if (*def) {
                os << "field_" << def;
            }
            else {
                os << "empty";
            }
            if (cnt) os << "_" << cnt;
            os << ";\n";
        }
        os << "};\n";

        LOG_INFO("dumped into {}", argv[3]);
        constStrings.Clear();

        return tool::OK;
    }

    ADD_TOOL(bo7_lua_dump, "bo7", " [exe] [func]", "Dump lua function data from an exe dump", bo7_lua_dump);
    ADD_TOOL(bo7_ncs_dump, "bo7", " [exe] [dump]", "Dump netconststrings data from an exe dump", bo7_ncs_dump);
    ADD_TOOL(bo7_cststr_dump, "bo7", " [exe] [dump]", "Dump scrstring struct data from an exe dump", bo7_cststr_dump);
}