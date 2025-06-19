#include <includes.hpp>
#include <tools/utils/ps4_process.hpp>
#include <tools/sp23/sp23.hpp>

namespace {

    const char* assetNames[]{
        "physicslibrary",
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
        "tileshader",
        "libshader",
        "vertexshader",
        "hullshader",
        "domainshader",
        "pixelshader",
        "serializedshader",
        "techset",
        "image",
        "soundglobalvolmod",
        "soundglobalentchannel",
        "soundglobalcontext",
        "soundglobalwhizby",
        "soundglobalbulletcrack",
        "soundglobalperk",
        "soundglobalocclusion",
        "soundglobalsurfaceinfo",
        "soundglobalcurve",
        "soundglobaldoppler",
        "soundglobalshapecone",
        "soundglobalfutz",
        "soundglobalsendeffect",
        "soundglobalnametable",
        "soundglobalfactiontable",
        "soundbank",
        "soundbank_merge",
        "soundbanktransient",
        "col_map",
        "com_map",
        "glass_map",
        "aipaths",
        "navmesh",
        "tacgraph",
        "airegiongraphs",
        "map_ents",
        "map_ents_trzone",
        "fx_map",
        "gfx_map",
        "gfx_map_trzone",
        "iesprofile",
        "lightdef",
        "gradingclut",
        "fogspline",
        "animclass",
        "playeranim",
        "gesture",
        "localize",
        "attachment",
        "weapon",
        "vfx",
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
        "scriptable",
        "vectorfield",
        "particlesimanimation",
        "streaminginfo",
        "laser",
        "gameprops",
        "materialstandard",
        "beam",
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
        "proceduralblendweights",
        "reticle",
        "xanimcurve",
        "coverselector",
        "enemyselector",
        "clientcharacter",
        "clothasset",
        "cinematicmotion",
        "accessory",
        "locdmgtable",
        "bulletpenetration",
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
        "vtm_surfacemapsdata",
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
        "foliagesfxtable",
        "impactsfxtable",
        "aiimpactvfxtable",
        "typeinfo",
        "handplantsfxtable",
        "sndtable",
        "equipmentsfx",
        "soundsubmix",
        "shock",
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
        "sndmodifierset",
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
        "hwconfigvargroup",
        "hwconfigtieredgroup",
        "sndmasterpreset",
        "materialflipbookparams",
        "materialuvdistortionparams",
        "genericblueprint",
        "downloadgroupset",
    };

    const char* GetAssetName(size_t type) {
        return type < ARRAYSIZE(assetNames) ? assetNames[type] : "invalid";
    }

    struct DB_AssetPool {
        uintptr_t m_entries;
        uintptr_t m_freeHead;
        unsigned int m_poolSize;
        unsigned int m_elementSize;
        unsigned int m_loadedPoolSize;
        char __padding[158];
    };
    static_assert(sizeof(DB_AssetPool) == 0xC0);

    struct GscObjEntry {
        uint64_t name;
        int32_t len;
        uintptr_t buffer;
    };
    static_assert(sizeof(GscObjEntry) == 0x18);

    void Cod23CBuf(utils::ps4::PS4Process& ps4, const char* cmd) {
        uintptr_t cbuf{ ps4[0xC0A5AD0] };

        struct CBuf {
            char buff[65536];
            int32_t maxLen;
            int32_t ptr;
        };

        int32_t ptr{ ps4.Read<int32_t>(cbuf + offsetof(CBuf, ptr)) };
        size_t l{ std::strlen(cmd) };
        if (ptr + l >= sizeof(CBuf::buff)) {
            throw std::runtime_error("cbuf error: buffer too small");
        }

        ps4.Write(cbuf + ptr, cmd, l + 1);
        ps4.Notify(std::format("cbuf {}", cmd));
    }

	int mp23ps4test(int argc, const char* argv[]) {
        if (tool::NotEnoughParam(argc, 1)) {
            return tool::BAD_USAGE;
        }

        const char* ipd{ argv[2] };
        utils::ps4::PS4Process ps4{ ipd, "cod23-cod.elf" };


        constexpr size_t poolscount = ARRAYSIZE(assetNames);
        ps4.Notify("Hi");
        // current assettype is good enough
        /*
        {

            LOG_INFO("dump output_mwiii/poolnamesps4.csv");
            utils::OutFileCE an{ "output_mwiii/poolnamesps4.csv", true };
            an << "id,name";
            std::unique_ptr<uintptr_t[]> names{ ps4.ReadArray<uintptr_t>(ps4[0x3266740], poolscount) };

            for (size_t i = 0; i < poolscount; i++) {
                if (!names[i]) break;
                // let's assume the size, it's useless otherwise
                std::unique_ptr<char[]> name{ ps4.ReadArray<char>(names[i], 0x21) };
                name[0x20] = 0;
                an << "\n" << i << "," << name.get();
            }
        }
        //*/


        std::unique_ptr<DB_AssetPool[]> pools{ ps4.ReadArray<DB_AssetPool>(ps4[0x8A18120], poolscount) };

        /*
        {
            LOG_INFO("dump output_mwiii/poolvaluesps4.csv");
            utils::OutFileCE an{ "output_mwiii/poolvaluesps4.csv", true };

            an << "id,name,entries,freeHead,poolSize,elementSize,loadedPoolSize";
            std::unique_ptr<DB_AssetPool[]> pools{ ps4.ReadArray<DB_AssetPool>(ps4[0x8A18120], poolscount) };

            for (size_t i = 0; i < poolscount; i++) {
                an
                    << "\n"
                    << std::dec << i << ","
                    << GetAssetName(i) << ","
                    << std::hex << "0x" << pools[i].m_entries << ","
                    << std::hex << "0x" << pools[i].m_freeHead << ","
                    << std::hex << "0x" << pools[i].m_poolSize << ","
                    << std::hex << "0x" << pools[i].m_elementSize << ","
                    << std::hex << "0x" << pools[i].m_loadedPoolSize
                    ;
            }
        }
        //*/


        /*
        std::filesystem::path outDir{ "output_mwiii/ps4gsc" };
        std::filesystem::create_directories(outDir);
        DB_AssetPool* gscobjPool{ &pools[sp23::ASSET_GSCOBJ] };

        std::unique_ptr<GscObjEntry[]> gscObjPoolEntries{ ps4.ReadArray<GscObjEntry>(gscobjPool->m_entries, gscobjPool->m_loadedPoolSize) };

        for (size_t i = 0; i < gscobjPool->m_loadedPoolSize; i++) {
            GscObjEntry* entry{ &gscObjPoolEntries[i] };

            if (!entry->buffer || entry->len <= 0) {
                continue;
            }

            std::vector<byte> data{ ps4.ReadBuffer(entry->buffer, entry->len) };

            std::filesystem::path outFile{ outDir / std::format("script_{:x}.gscc", entry->name) };
            if (!utils::WriteFile(outFile, data)) {
                LOG_ERROR("Can't dump {}", outFile.string());
            }
            else {
                LOG_INFO("Dump {}", outFile.string());
            }
        }

        //*/

        

		return tool::OK;
	}

    ADD_TOOL(mp23ps4test, "mwiii", " [ps4]", "mp23 ps4 test", mp23ps4test);
}