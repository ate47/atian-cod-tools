#include <includes.hpp>
#include <hook/module_mapper.hpp>
#include "mw19.hpp"


namespace mw19 {
    namespace {
        const char* IW19PoolNames[]{
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
            "libshader",
            "vertexshader",
            "hullshader",
            "domainshader",
            "pixelshader",
            "techset",
            "image",
            "soundglobals",
            "soundbank",
            "soundbanktransient",
            "col_map",
            "com_map",
            "glass_map",
            "aipaths",
            "navmesh",
            "tacgraph",
            "map_ents",
            "fx_map",
            "gfx_map",
            "gfx_map_trzone",
            "iesprofile",
            "lightdef",
            "gradingclut",
            "ui_map",
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
            "mptype",
            "character",
            "xmodelalias",
            "rawfile",
            "scriptfile",
            "scriptdebugdata",
            "stringtable",
            "leaderboarddef",
            "virtualleaderboarddef",
            "ddl",
            "tracer",
            "vehicle",
            "addon_map_ents",
            "netconststrings",
            "luafile",
            "scriptable",
            "equipsndtable",
            "vectorfield",
            "particlesimanimation",
            "streaminginfo",
            "laser",
            "ttf",
            "suit",
            "suitanimpackage",
            "camera",
            "hudoutline",
            "spaceshiptarget",
            "rumble",
            "rumblegraph",
            "animpkg",
            "sfxpkg",
            "vfxpkg",
            "footstepvfx",
            "behaviortree",
            "aianimset",
            "aiasm",
            "proceduralbones",
            "dynamicbones",
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
            "streamkey",
            "streamtreeoverride",
            "keyvaluepairs",
            "stterrain",
            "nativescriptpatch",
            "collisiontile",
            "execution",
            "carryobject",
            "soundbanklist",
            "decalvolumematerial",
            "decalvolumemask",
            "dynentitylist",
            "fx_map_trzone",
            "dlogschema",
            "edgelist",
            "triggereffect",
            "weapontrigger",
            "dwidata",
        };

	    int mw19_exe(int argc, const char* argv[]) {
		    std::filesystem::path exe{ argv[2] };

		    hook::module_mapper::Module mod{ true };

		    LOG_INFO("Loading module {}", exe.string());
		    if (!mod.Load(exe) || !mod) {
			    LOG_ERROR("Can't load module");
			    return tool::BASIC_ERROR;
		    }

		    LOG_INFO("Loaded");



		    auto* pools{ mod->Get<uintptr_t>(0x7221160) };
		    {
			    std::filesystem::path pf{ exe.parent_path() / "pools19.cpp" };
			    std::ofstream osf{ pf };
			    if (!osf) {
				    LOG_ERROR("Can't open {}", pf.string());
			    }
			    else {
				    utils::CloseEnd osfce{ osf };
				    osf << "enum IW19PoolType : int {\n";
				    for (size_t idx = 0; idx < mw19::IW19_ASSETTYPE_COUNT; idx++) {
					    if (!pools[idx]) break;
					    const char* cc = mod->Rebase<const char>(pools[idx]);
					    char* ccu = utils::UpperCase(utils::CloneString(cc));
					    osf << "    IW19_ASSETTYPE_" << ccu << " = " << std::dec << idx << ", // 0x" << std::hex << idx << "\n";
				    }
				    osf << "};\n";

				    osf << "const char* IW19PoolNames[] {\n";
				    for (size_t idx = 0; idx < mw19::IW19_ASSETTYPE_COUNT; idx++) {
					    if (!pools[idx]) break;
					    const char* cc = mod->Rebase<const char>(pools[idx]);
					    osf << "    \"" << cc << "\",\n";
				    }
				    osf << "};\n";
			    }
			    LOG_INFO("Created {}", pf.string());
		    }

		    return tool::OK;
	    }


    }


    const char* PoolName(IW19PoolType type) {
        return type >= 0 && type < ARRAYSIZE(IW19PoolNames) ? IW19PoolNames[type] : "<invalid>";
    }

    IW19PoolType PoolId(const char* name) {
        for (size_t i = 0; i < ARRAYSIZE(IW19PoolNames); i++) {
            if (!_strcmpi(IW19PoolNames[i], name)) {
                return (IW19PoolType)i;
            }
        }
        return IW19_ASSETTYPE_COUNT;
    }
    ADD_TOOL(mw19_exe, "mw19", "[exe]", "Map mw19 exe in memory", mw19_exe);
}