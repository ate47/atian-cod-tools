#include <includes.hpp>
#include "cw.hpp"
#include "pool_cod2020.hpp"

namespace {

    const char* poolNames[]{
        "physpreset",
        "physconstraints",
        "destructibledef",
        "xanim",
        "xmodel",
        "xcollision",
        "xskeleton",
        "xmodelmesh",
        "material",
        "csdef",
        "computeshaderset",
        "rtsdef",
        "raytraceshaderset",
        "techset",
        "image",
        "sound",
        "sound_bank",
        "sound_asset",
        "sound_duck",
        "sound_alias_modifier",
        "sound_acoustics",
        "col_map",
        "clip_map",
        "com_map",
        "game_map",
        "gfx_map",
        "fonticon",
        "localizeentry",
        "gesture",
        "gesturetable",
        "cinematicmotion",
        "weapon",
        "weaponfull",
        "weaponfrontend",
        "weaponstylesettings",
        "weaponsecondarymovement",
        "weapontunables",
        "cgmediatable",
        "playersoundstable",
        "playerfxtable",
        "sharedweaponsounds",
        "attachment",
        "attachmentunique",
        "weaponcamo",
        "weaponcamobinding",
        "customizationtable",
        "customizationtablefrontend",
        "snddriverglobals",
        "fx",
        "tagfx",
        "klf",
        "impactsfxtable",
        "impactsoundstable",
        "aitype",
        "character",
        "xmodelalias",
        "rawfile",
        "animtree",
        "stringtable",
        "structuredtable",
        "leaderboarddef",
        "ddl",
        "glasses",
        "scriptparsetree",
        "scriptparsetreedbg",
        "script_using",
        "keyvaluepairs",
        "vehicle",
        "tracer",
        "surfacefxtable",
        "surfacesounddef",
        "footsteptable",
        "entityfximpacts",
        "entitysoundimpacts",
        "zbarrier",
        "vehiclefxdef",
        "vehiclesounddef",
        "typeinfo",
        "scriptbundle",
        "scriptbundlelist",
        "rumble",
        "bulletpenetration",
        "locdmgtable",
        "aimtable",
        "shoottable",
        "playerglobaltunables",
        "overheadcameratunables",
        "animselectortable",
        "animmappingtable",
        "animstatemachine",
        "behaviortree",
        "behaviorstatemachine",
        "ttf",
        "sanim",
        "shellshock",
        "statuseffect",
        "cinematic_camera",
        "cinematic_sequence",
        "spectate_camera",
        "xcam",
        "bgcache",
        "flametable",
        "bitfield",
        "maptable",
        "maptablelist",
        "maptableloadingimages",
        "maptablepreviewimages",
        "objective",
        "objectivelist",
        "navmesh",
        "navvolume",
        "laser",
        "beam",
        "streamerhint",
        "flowgraph",
        "postfxbundle",
        "luafile",
        "luafiledebug",
        "renderoverridebundle",
        "staticlevelfxlist",
        "triggerlist",
        "playerroletemplate",
        "playerroletemplatefrontend",
        "playerrolecategorytable",
        "playerrolecategory",
        "characterbodytype",
        "characterbodytypefrontend",
        "playeroutfit",
        "gametypetable",
        "feature",
        "featuretable",
        "unlockableitem",
        "unlockableitemtable",
        "entitylist",
        "playlists",
        "playlistglobalsettings",
        "playlistschedule",
        "motionmatchinginput",
        "blackboard",
        "tacticalquery",
        "playermovementtunables",
        "hierarchicaltasknetwork",
        "ragdoll",
        "storagefile",
        "storagefilelist",
        "charmixer",
        "storeproduct",
        "storecategory",
        "storecategorylist",
        "rank",
        "ranktable",
        "prestige",
        "prestigetable",
        "firstpartyentitlement",
        "firstpartyentitlementlist",
        "entitlement",
        "entitlementlist",
        "sku",
        "labelstore",
        "labelstorelist",
        "cpu_occlusion_data",
        "lighting",
        "streamerworld",
        "talent",
        "playertalenttemplate",
        "playeranimation",
        "<unused>",
        "terraingfx",
        "highlightreelinfodefines",
        "highlightreelprofileweighting",
        "highlightreelstarlevels",
        "dlogevent",
        "rawstring",
        "ballisticdesc",
        "streamkey",
        "rendertargets",
        "drawnodes",
        "grouplodmodel",
        "fxlibraryvolume",
        "arenaseasons",
        "sprayorgestureitem",
        "sprayorgesturelist",
        "hwplatform",
        "attachmenttable",
        "navinput",
        "uimodeldatastruct",
        "crafticon",
        "crafticonlist",
        "craftbackground",
        "craftbackgroundlist",
        "craftmaterial",
        "craftmateriallist",
        "craftcategory",
        "craftcategorylist",
        "craftweaponicontransform",
        "craftweaponicontransformlist",
        "xanimcurve",
        "dynmodel",
        "vectorfield",
        "winddef",
        "vehicleassembly",
        "milestone",
        "milestonetable",
        "triggereffectdesc",
        "triggeractions",
        "playersettings",
        "compasstunables",
    };

    struct XAssetPool {
        uintptr_t pool; // void*
        unsigned int itemSize;
        int itemCount;
        bool isSingleton;
        int itemAllocCount;
        void* freeHead;
    };
    struct ScriptParseTree {
        uint64_t name;
        uintptr_t buffer; // GSC_OBJ*
        int32_t len;
    };

    int wpscod2020(Process& proc, int argc, const char* argv[]) {
        std::filesystem::path output{ tool::NotEnoughParam(argc, 1) ? "scriptparsetree_cod2020" : argv[2] };

        std::filesystem::create_directories(output);

        uintptr_t poolsPtr{ cw::ScanPool(proc) };

        auto pool{ proc.ReadMemoryObjectEx<XAssetPool>(cw::ScanPool(proc) + sizeof(XAssetPool) * cw::alpha::ASSET_TYPE_SCRIPTPARSETREE) };

        LOG_INFO("Find pool scriptparsetree -> 0x{:x} (0x{:x} * 0x{:x}/0x{:x} element(s)", pool->pool, pool->itemSize, pool->itemAllocCount, pool->itemCount);

        if (pool->itemSize != sizeof(ScriptParseTree)) {
            LOG_ERROR("Invalid item size 0x{:x}", pool->itemSize);
            return tool::BASIC_ERROR;
        }


        auto entries{ proc.ReadMemoryArrayEx<ScriptParseTree>(pool->pool, pool->itemSize) };

        for (size_t i = 0; i < pool->itemAllocCount; i++) {
            ScriptParseTree& e{ entries[i] };

            if (e.name < 0x1000000000 || e.buffer == 0 || e.len == 0) continue; // ignore bad entries

            std::filesystem::path of{ output / std::format("script_{:x}.gscc", e.name) };
            try {
                auto buffer{ proc.ReadMemoryArrayEx<byte>(e.buffer, e.len) };

                if (utils::WriteFile(of, buffer.get(), e.len)) {
                    LOG_INFO("Dump {}", of.string());
                }
                else {
                    LOG_ERROR("Error when dumping {}", of.string());
                }
            }
            catch (std::runtime_error& e) {
                LOG_ERROR("Error when dumping {}: {}", of.string(), e.what());
            }
        }


        LOG_INFO("Dump into {}", output.string());

        return tool::OK;
    }


    ADD_TOOL(wpscod2020, "cw", " [output=scriptparsetree_cod2020]", "write pooled scripts (cwa)", L"CoD2020.exe", wpscod2020);
}