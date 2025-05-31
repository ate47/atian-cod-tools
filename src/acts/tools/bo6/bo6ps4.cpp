#include <includes.hpp>
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include "tools/bo6/bo6.hpp"
#include "tools/utils/ps4_process.hpp"
#include "tools/tools_ui.hpp"
#include "tools/tools_nui.hpp"
#include "tools/gsc.hpp"
#include <core/config.hpp>

const char* poolNames[]{
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
    "shader",
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
    "soundglobalnametable",
    "soundbank",
    "soundbank_merge",
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
    "xanimsetaddon",
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
    "behaviortokenconfig",
    "posematchingtransitionset",
    "posematchingasmconfig",
    "deployableweaponconfig",
    "aianimsettings",
    "parachutedata",
    "downloadgroupset",
    "sndambient",
    "sndambientevent",
    "sndambientelement",
    "sndmix",
    "sndfullocclusion",
    "weatherimpactsfxtable",
    "indexedlisttype",
    "footstepsfxoverride",
};

const char* poolNamesOld2[]{
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
    "shader",
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
    "soundglobalnametable",
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
    "xanimsetaddon",
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
    "behaviortokenconfig",
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
    "sndmasterpresetbundle",
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
    "impactshake",
};

const char* poolNamesRelease[]{
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
    "shader",
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
    "soundglobalnametable",
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
    "xanimsetaddon",
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
    "sndmasterpresetbundle",
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
    "sndvolmods",
    "bonefollowermappinggraph",
    "transientsfxpackage",
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
};

const char* bo6::PoolName(T10AssetType type) {
    return type >= 0 && type < ARRAYSIZE(poolNames) ? poolNames[type] : "<invalid>";
}

bo6::T10AssetType bo6::PoolId(const char* name) {
    for (size_t i = 0; i < ARRAYSIZE(poolNames); i++) {
        if (!_strcmpi(poolNames[i], name)) {
            return (T10AssetType)i;
        }
    }
    return T10_ASSET_COUNT;
}

const char* bo6::PoolNameRelease(bo6::T10RAssetType type) {
    return type >= 0 && type < ARRAYSIZE(poolNamesRelease) ? poolNamesRelease[type] : "<invalid>";
}
bo6::T10RAssetType bo6::PoolIdRelease(const char* name) {
    for (size_t i = 0; i < ARRAYSIZE(poolNamesRelease); i++) {
        if (!_strcmpi(poolNamesRelease[i], name)) {
            return (T10RAssetType)i;
        }
    }
    return T10R_ASSET_COUNT;
}

namespace {
    struct DB_AssetPool {
        uintptr_t m_entries; // void*
        uintptr_t m_freeHead; // void*
        unsigned int m_poolSize;
        unsigned int m_elementSize;
        unsigned int m_loadedPoolSize;
        char __padding[158];
    };
    struct GscObjEntry {
        uint64_t name;
        int len;
        int padc;
        uintptr_t buffer;
    };
    struct LuaFileEntry {
        uint64_t name;
        uint64_t unk8;
        uint64_t unk10;
        uint64_t unk18;
        uint64_t unk20;
        uint64_t unk28;
        uint32_t unk30;
        uint32_t len;
        uint64_t unk38;
        uint64_t buffer;
        uint64_t unk48;
        uint64_t unk50;
    }; static_assert(sizeof(LuaFileEntry) == 0x58);


    int ps4dumpbo6(Process& _, int argc, const char* argv[]) {
        if (argc < 4) {
            return tool::BAD_USAGE;
        }

        utils::ps4::PS4Process ps4{ argv[2] };

        for (size_t i = 3; i < argc; i++) {
            const char* poolName{ argv[i] };

            if (!_strcmpi(poolName, "pools")) {
                //const char* poolnames[311]
                constexpr size_t poolsCount = bo6::T10_ASSET_COUNT;
                auto pools = ps4.ReadArray<DB_AssetPool>(ps4[0x98FEFA0], poolsCount);
                auto names = ps4.ReadArray<uint64_t>(ps4[0x3206610], poolsCount);

                std::ofstream os{ "bo6pools.csv" };

                if (!os) {
                    LOG_ERROR("Can't open bo6pools.csv");
                    return tool::BASIC_ERROR;
                }

                utils::CloseEnd ce{ os };

                os << "id,name,itemsize,count,alloc";
            
                for (size_t i = 0; i < poolsCount; i++) {
                    std::string name{ ps4.ReadString(names[i], 128) }; // we can go unsafe because we are in the middle of the eboot
                    auto& pool{ pools[i] };
                    os << "\n" << std::dec << i << "," << name << "," << std::hex << pool.m_elementSize << "," << pool.m_poolSize << "," << pool.m_loadedPoolSize;
                }

                LOG_INFO("Pools dumped into bo6pools.csv");
                continue;
            }

            if (!_strcmpi(poolName, "gscobj")) {
                auto pool = ps4.ReadObject<DB_AssetPool>(ps4[0x98FEFA0] + sizeof(DB_AssetPool) * bo6::T10_ASSET_GSCOBJ);


                LOG_INFO("Pool: {:x}, count: {}/{}, len 0x{:x}", pool->m_entries, pool->m_loadedPoolSize, pool->m_poolSize, pool->m_elementSize);
                auto objs = ps4.ReadArray<GscObjEntry>(pool->m_entries, pool->m_loadedPoolSize);

                std::filesystem::path gsc{ "gsc" };

                std::filesystem::create_directories(gsc);

                for (size_t i = 0; i < pool->m_loadedPoolSize; i++) {
                    auto& obj = objs[i];

                    if (!obj.len || !obj.buffer) {
                        LOG_WARNING("Ignore {:x}", obj.name);
                        continue;
                    }

                    auto bytes3 = ps4.ReadBuffer(obj.buffer, obj.len);

                    if (!utils::WriteFile(gsc / utils::va("script_%llx.gscc", obj.name), bytes3.data(), bytes3.size())) {
                        LOG_ERROR("Error when writting {:x}", obj.name);
                    }
                    else {
                        LOG_INFO("Dump script_{:x}.gscc", obj.name);
                    }

                }
                continue;
            }

            if (!_strcmpi(poolName, "luafile")) {
                auto pool = ps4.ReadObject<DB_AssetPool>(ps4[0x98FEFA0] + sizeof(DB_AssetPool) * bo6::T10_ASSET_LUAFILE);


                LOG_INFO("Pool: {:x}, count: {}/{}, len 0x{:x}", pool->m_entries, pool->m_loadedPoolSize, pool->m_poolSize, pool->m_elementSize);
                auto objs = ps4.ReadArray<LuaFileEntry>(pool->m_entries, pool->m_loadedPoolSize);

                std::filesystem::path gsc{ "lua" };

                std::filesystem::create_directories(gsc);

                for (size_t i = 0; i < pool->m_loadedPoolSize; i++) {
                    auto& obj = objs[i];

                    if (!obj.len || !obj.buffer) {
                        LOG_WARNING("Ignore {:x}", obj.name);
                        continue;
                    }
                    
                    auto bytes3 = ps4.ReadBuffer(obj.buffer, obj.len);
                    
                    if (!utils::WriteFile(gsc / utils::va("%llx.lua", obj.name), bytes3.data(), bytes3.size())) {
                        LOG_ERROR("Error when writting {:x}", obj.name);
                    }
                    else {
                        LOG_INFO("Dump {:x}.lua", obj.name);
                    }

                }
                continue;
            }

            LOG_ERROR("Can't dump pool {}", poolName);
        }

        ps4.Notify("Pools dumped");

        return tool::OK;
    }

    int ps4cbufbo6(Process& _, int argc, const char* argv[]) {
        if (argc < 4) {
            return tool::BAD_USAGE;
        }

        const char* ipd = argv[2];
        const char* cmd = argv[3];

        LOG_TRACE("Send {} -> {}", ipd, cmd);

        libdebug::PS4DBG ps4{ ipd };
        try {
            ps4.Connect();

            LOG_TRACE("Connected to {}", ipd);



            auto procList = ps4.GetProcessList();
            auto proc = procList.FindProcess("eboot.bin");

            if (!proc) {
                LOG_ERROR("Can't find eboot.bin");
                for (auto& p : procList.processes) {
                    LOG_TRACE("{}", p->name);
                }
                ps4.Disconnect();
                return tool::BASIC_ERROR;
            }

            auto pid = proc->pid;

            auto entries = ps4.GetProcessMaps(proc->pid);

            uint64_t base = 0;

            LOG_TRACE("Connected to {}", ipd);
            for (const auto& entry : entries.entries) {
                if (entry->prot == 5) {
                    LOG_INFO("executable base : 0x{:x}", entry->start);
                    base = entry->start;
                    break;
                }
            }
            LOG_TRACE("Connected to {}", ipd);

            if (!base) {
                LOG_ERROR("Can't find executable base");
                ps4.Disconnect();
                return tool::BASIC_ERROR;
            }

            uint64_t cbuf1 = base + 0x4D6C350;
            uint64_t cbuf2 = cbuf1 + 0x10004;

            std::vector<byte> data{};

            LOG_TRACE("Write string");
            utils::WriteString(data, cmd);

            ps4.WriteMemory(pid, cbuf1, data);

            data.clear();
            LOG_TRACE("Write len");
            utils::WriteValue<uint32_t>(data, (uint32_t)std::strlen(cmd));

            ps4.WriteMemory(pid, cbuf2, data);


            ps4.Notify(utils::ps4::PS4N_MESSAGE, std::format("cbuf {}", cmd));
        }
        catch (std::exception& res) {
            LOG_ERROR("{}", res.what());
        }
        ps4.Disconnect();

        return tool::OK;
    }
    int ps4repbo6(Process& _, int argc, const char* argv[]) {
        if (argc < 4) {
            return tool::BAD_USAGE;
        }

        const char* ipd = argv[2];
        const char* file = argv[3];

        std::string buff{};

        if (!utils::ReadFile(file, buff)) {
            LOG_ERROR("Can't read {}", file);
            return tool::BASIC_ERROR;
        }

        tool::gsc::GscObj24* script{ (tool::gsc::GscObj24*)buff.data() };

        if (*(uint64_t*)script != 0xa0d43534706) {
            LOG_ERROR("Invalid script magic");
            return tool::BASIC_ERROR;
        }

        uint64_t name = script->name;

        LOG_INFO("Loaded script {} -> name = script_{:x}", file, name);


        libdebug::PS4DBG ps4{ ipd };
        try {
            ps4.Connect();

            LOG_TRACE("Connected to {}", ipd);



            auto procList = ps4.GetProcessList();
            auto proc = procList.FindProcess("eboot.bin");

            if (!proc) {
                LOG_ERROR("Can't find eboot.bin");
                for (auto& p : procList.processes) {
                    LOG_TRACE("{}", p->name);
                }
                ps4.Disconnect();
                return tool::BASIC_ERROR;
            }

            auto pid = proc->pid;

            auto entries = ps4.GetProcessMaps(proc->pid);

            uint64_t base = 0;

            LOG_TRACE("Connected to {}", ipd);
            for (const auto& entry : entries.entries) {
                if (entry->prot == 5) {
                    LOG_INFO("executable base : 0x{:x}", entry->start);
                    base = entry->start;
                    break;
                }
            }
            LOG_TRACE("Connected to {}", ipd);

            if (!base) {
                LOG_ERROR("Can't find executable base");
                ps4.Disconnect();
                return tool::BASIC_ERROR;
            }


            uintptr_t poolIdx = base + 0x98FEFA0 + sizeof(DB_AssetPool) * 69; // 69 = gscobj
            auto bytes = ps4.ReadMemory(pid, poolIdx, sizeof(DB_AssetPool));

            auto* pool = reinterpret_cast<DB_AssetPool*>(bytes.data());


            LOG_INFO("Pool: {:x}, count: {}/{}, len 0x{:x}", pool->m_entries, pool->m_loadedPoolSize, pool->m_poolSize, pool->m_elementSize);
            auto bytes2 = ps4.ReadMemory(pid, pool->m_entries, sizeof(GscObjEntry) * pool->m_loadedPoolSize);

            auto* objs = reinterpret_cast<GscObjEntry*>(bytes2.data());

            bool replaced{};
            for (size_t i = 0; i < pool->m_loadedPoolSize; i++) {
                auto& obj = objs[i];

                if (obj.name != name) {
                    continue;
                }

                if (!obj.buffer) {
                    LOG_ERROR("Empty buffer");
                    break;
                }

                if (obj.len < buff.size()) {
                    LOG_ERROR("Buffer too small, can't remplace {} < {}", obj.len, buff.size());
                    break;
                }

                std::vector<byte> data{};

                data.insert(data.begin(), (char*)buff.data(), (char*)buff.data() + buff.size());

                ps4.WriteMemory(pid, obj.buffer, data);

                LOG_INFO("Script replaced {}/{} bytes", buff.size(), obj.len);
                replaced = true;
                break;
            }
            if (!replaced) {
                LOG_ERROR("Can't find {:x}", name);
            }
            ps4.Notify(utils::ps4::PS4N_MESSAGE, "script injected");

        }
        catch (std::exception& res) {
            LOG_ERROR("{}", res.what());
        }
        ps4.Disconnect();

        return tool::OK;
    }

    struct {
        HWND titleLabel{};
        HWND filePathEdit{};
        HWND filePathEditLabel{};
        HWND ps4Edit{};
        HWND ps4EditLabel{};
        HWND filePathButton{};
        HWND injectButton{};
        HWND cbuffEdit{};
        HWND cbuffEditLabel{};
        HWND cbuffButton{};
        HWND notificationLabel{};
        std::wstring lastNotif{};
    } info{};

    void SetNotif(const std::string& msg) {
        info.lastNotif = utils::StrToWStr(msg);

        SendMessage(info.notificationLabel, WM_SETTEXT, (WPARAM)0, (LPARAM)info.lastNotif.data());
    }

    int Render(HWND window, HINSTANCE hInstance) {
        info.titleLabel = CreateWindowExW(
            0,
            L"STATIC",
            L"Black Ops 6 Beta tools",
            SS_CENTER | WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.notificationLabel = CreateWindowExW(
            0,
            L"STATIC",
            info.lastNotif.c_str(),
            SS_CENTER | WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.filePathEdit = CreateWindowExW(
            0,
            L"EDIT",
            L"",
            WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.filePathEditLabel = CreateWindowExW(
            0,
            L"STATIC",
            L"GSC File : ",
            SS_RIGHT | WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );
        info.ps4Edit = CreateWindowExW(
            0,
            L"EDIT",
            L"",
            WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.ps4EditLabel = CreateWindowExW(
            0,
            L"STATIC",
            L"PS4 : ",
            SS_RIGHT | WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.filePathButton = CreateWindowExW(
            0,
            L"BUTTON",
            L"...",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.injectButton = CreateWindowExW(
            0,
            L"BUTTON",
            L"Inject GSC script",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.cbuffEdit = CreateWindowExW(
            0,
            L"EDIT",
            L"",
            WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.cbuffEditLabel = CreateWindowExW(
            0,
            L"STATIC",
            L"Command : ",
            SS_RIGHT | WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.cbuffButton = CreateWindowExW(
            0,
            L"BUTTON",
            L"Send command",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );


        if (
            info.filePathEdit == NULL
            || info.filePathEditLabel == NULL
            || info.filePathButton == NULL
            || info.injectButton == NULL
            || info.ps4Edit == NULL
            || info.ps4EditLabel == NULL
            || info.titleLabel == NULL
            || info.cbuffEdit == NULL
            || info.cbuffEditLabel == NULL
            || info.cbuffButton == NULL
            ) {
            return -1;
        }

        SendMessage(info.filePathEdit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
        SendMessage(info.ps4Edit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
        SendMessage(info.cbuffEdit, EM_SETLIMITTEXT, (WPARAM)0x1000, (LPARAM)0);
        

        std::wstring gscPath = utils::StrToWStr(core::config::GetString("ui.bo6.path", ""));
        std::wstring ps4Path = utils::StrToWStr(core::config::GetString("ui.ps4.ipd", ""));
        std::wstring cbufcfg = utils::StrToWStr(core::config::GetString("ui.bo6.cbuf", ""));
        SendMessage(info.filePathEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)gscPath.c_str());
        SendMessage(info.ps4Edit, WM_SETTEXT, (WPARAM)0, (LPARAM)ps4Path.c_str());
        SendMessage(info.cbuffEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)cbufcfg.c_str());
        return 0;
    }

    LRESULT Update(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_COMMAND) {
            if (HIWORD(wParam) == EN_CHANGE) {
                if (info.filePathEdit == (HWND)lParam) {

                    wchar_t path[MAX_PATH + 1];

                    if (SUCCEEDED(GetWindowTextW(info.filePathEdit, &path[0], ARRAYSIZE(path)))) {
                        core::config::SetString("ui.bo6.path", utils::WStrToStr(path));
                        core::config::SaveConfig();
                    }
                }
                else if (info.ps4Edit == (HWND)lParam) {
                    wchar_t path[MAX_PATH + 1];

                    if (SUCCEEDED(GetWindowTextW(info.ps4Edit, &path[0], ARRAYSIZE(path)))) {
                        core::config::SetString("ui.ps4.ipd", utils::WStrToStr(path));
                        core::config::SaveConfig();
                    }
                }
                else if (info.cbuffEdit == (HWND)lParam) {
                    wchar_t path[MAX_PATH + 1];

                    if (SUCCEEDED(GetWindowTextW(info.cbuffEdit, &path[0], ARRAYSIZE(path)))) {
                        core::config::SetString("ui.bo6.cbuf", utils::WStrToStr(path));
                        core::config::SaveConfig();
                    }
                }
            }
            else if (wParam == BN_CLICKED) {
                if (info.filePathButton == (HWND)lParam) {
                    // Open file

                    OPENFILENAMEW ofn;
                    WCHAR szFile[MAX_PATH + 1] = { 0 };

                    // Initialize OPENFILENAME
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = tool::ui::window().hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile);
                    ofn.lpstrFilter = L"Compiled GSC file (.gscc)\0*.gscc\0All\0*.*\0";
                    ofn.lpstrTitle = L"Open GSC file";
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    if (GetOpenFileNameW(&ofn) == TRUE) {
                        core::config::SetString("ui.bo6.path", utils::WStrToStr(ofn.lpstrFile));
                        core::config::SaveConfig();
                        SendMessage(info.filePathEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)ofn.lpstrFile);
                    }
                    return 1;
                }
                else if (info.injectButton == (HWND)lParam) {
                    std::string filePath = utils::WStrToStr(tool::ui::GetWindowTextVal(info.filePathEdit));
                    std::string ps4Path = utils::WStrToStr(tool::ui::GetWindowTextVal(info.ps4Edit));

                    core::config::SetString("ui.bo6.path", filePath);
                    core::config::SetString("ui.ps4.ipd", ps4Path);
                    core::config::SaveConfig();
                    try {
                        SetNotif("");
                        std::string buff{};

                        if (!utils::ReadFile(filePath, buff)) {
                            SetNotif(std::format("Can't read {}", filePath));
                            return TRUE;
                        }

                        tool::gsc::GscObj24* script{ (tool::gsc::GscObj24*)buff.data() };

                        uint64_t magic = *(uint64_t*)script;

                        if (magic != 0xa0d43534706) {
                            tool::gsc::opcode::VmInfo* nfo{};
                            if (tool::gsc::opcode::IsValidVmMagic(magic, nfo)) {
                                SetNotif(std::format("This script is for {}, not Black Ops 6", nfo->name));
                            }
                            else {
                                SetNotif(std::format("Invalid script magic {:x}, are you sure to use a compiled bo6 script?", magic));
                            }
                            return TRUE;
                        }

                        uint64_t name = script->name;

                        utils::ps4::PS4Process ps4{ ps4Path };

                        auto pool = ps4.ReadObject<DB_AssetPool>(ps4[0x98FEFA0] + sizeof(DB_AssetPool) * bo6::T10_ASSET_GSCOBJ);


                        LOG_INFO("Pool: {:x}, count: {}/{}, len 0x{:x}", pool->m_entries, pool->m_loadedPoolSize, pool->m_poolSize, pool->m_elementSize);
                        auto objs = ps4.ReadArray<GscObjEntry>(pool->m_entries, pool->m_loadedPoolSize);

                        for (size_t i = 0; i < pool->m_loadedPoolSize; i++) {
                            auto& obj = objs[i];

                            if (obj.name != name) {
                                continue;
                            }

                            if (!obj.buffer) {
                                SetNotif("Empty buffer");
                                return TRUE;
                            }

                            if (obj.len < buff.size()) {
                                SetNotif(std::format("Buffer too small, can't remplace {} < {}", obj.len, buff.size()));
                                return TRUE;
                            }

                            auto scriptTarget = ps4.ReadObject<tool::gsc::GscObj24>(obj.buffer);

                            if (scriptTarget->checksum != script->checksum) {
                                SetNotif("Find target script, but the checksum doesn't match");
                                return TRUE;
                            }

                            ps4.Write(obj.buffer, buff.data(), buff.size());

                            SetNotif("Script injected");
                            ps4.Notify("Script injected");
                            return TRUE;
                        }
                        SetNotif("Can't find hook script");
                    }
                    catch (std::exception& e) {
                        SetNotif(std::format("Exception: {}", e.what()));
                    }
                    return TRUE;
                }
                else if (info.cbuffButton == (HWND)lParam) {
                    SetNotif("");
                    std::string ps4Path = utils::WStrToStr(tool::ui::GetWindowTextVal(info.ps4Edit));
                    std::string cmd = utils::WStrToStr(tool::ui::GetWindowTextVal(info.cbuffEdit));

                    core::config::SetString("ui.bo6.cbuf", cmd);
                    core::config::SaveConfig();
                    try {
                        utils::ps4::PS4Process ps4{ ps4Path };

                        uint64_t cbuf1 = ps4[0x4D6C350];
                        uint64_t cbuf2 = cbuf1 + 0x10004;

                        ps4.Write(cbuf1, cmd.data(), cmd.size() + 1);
                        ps4.Write<uint32_t>(cbuf2, (uint32_t)cmd.size());


                        ps4.Notify(std::format("cbuf {}", cmd));
                    }
                    catch (std::exception& e) {
                        SetNotif(std::format("Exception: {}", e.what()));
                    }
                    return TRUE;
                }
            }
        }
        else if (uMsg == WM_CTLCOLORSTATIC) {
            if (lParam == (LPARAM)info.filePathEdit
                || lParam == (LPARAM)info.filePathEditLabel
                || lParam == (LPARAM)info.filePathButton
                || lParam == (LPARAM)info.ps4Edit
                || lParam == (LPARAM)info.ps4EditLabel
                || lParam == (LPARAM)info.cbuffEdit
                || lParam == (LPARAM)info.cbuffEditLabel
                || lParam == (LPARAM)info.cbuffButton
                || lParam == (LPARAM)info.injectButton
                || lParam == (LPARAM)info.notificationLabel
                || lParam == (LPARAM)info.titleLabel
                ) {
                return 0;
            }
        }
        return 1;
    }
    void Resize(int width, int height) {
        int y{ height / 2 - 28 * 4 };
        SetWindowPos(info.titleLabel, NULL, 0, y - 68, width, 60, SWP_SHOWWINDOW);


        SetWindowPos(info.ps4Edit, NULL, width / 2 - 250, y, 500, 24, SWP_SHOWWINDOW);
        SetWindowPos(info.ps4EditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
        y += 28;


        // GSC TOOL
        y += 20;

        SetWindowPos(info.filePathEdit, NULL, width / 2 - 250, y, 410, 24, SWP_SHOWWINDOW);
        SetWindowPos(info.filePathEditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
        SetWindowPos(info.filePathButton, NULL, width / 2 + 164, y, 86, 24, SWP_SHOWWINDOW);
        y += 28;

        SetWindowPos(info.injectButton, NULL, width / 2 - 250, y, 500, 24, SWP_SHOWWINDOW);
        y += 28;

        // CBUFF TOOL
        y += 20;

        SetWindowPos(info.cbuffEdit, NULL, width / 2 - 250, y, 500, 24, SWP_SHOWWINDOW);
        SetWindowPos(info.cbuffEditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
        y += 28;
        SetWindowPos(info.cbuffButton, NULL, width / 2 - 250, y, 500, 24, SWP_SHOWWINDOW);
        y += 28;

        // Notif
        y += 20;
        SetWindowPos(info.notificationLabel, NULL, 0, y, width, 48, SWP_SHOWWINDOW);

        tool::ui::window().SetTitleFont(info.titleLabel);
    }

    void bo6_tools() {
        tool::nui::NuiUseDefaultWindow dw{};
        static char gscFileIn[MAX_PATH + 1]{ 0 };
        static char cbuffIn[0x100]{ 0 };
        static char ps4In[0x50]{ 0 };
        static std::string notif{};

        static std::once_flag of{};

        bool c = false;
        std::call_once(of, [&c] {
            std::string injGsc = core::config::GetString("ui.bo6.path");
            std::string injCbuff = core::config::GetString("ui.bo6.cbuf");
            std::string injPs4 = core::config::GetString("ui.ps4.ipd");

            snprintf(gscFileIn, sizeof(gscFileIn), "%s", injGsc.data());
            snprintf(cbuffIn, sizeof(cbuffIn), "%s", injCbuff.data());
            snprintf(ps4In, sizeof(ps4In), "%s", injPs4.data());
            c = true;
           });

        ImGui::SeparatorText("BO6 PS4 utilitites");

        if (ImGui::InputText("PS4 IP", ps4In, sizeof(ps4In))) {
            core::config::SetString("ui.ps4.ipd", ps4In);
            c = true;
        }

        ImGui::Spacing();
        ImGui::SeparatorText("cbuff");

        if (ImGui::InputText("Command", cbuffIn, sizeof(cbuffIn))) {
            core::config::SetString("ui.bo6.cbuf", cbuffIn);
            c = true;
        }
        if (ImGui::Button("Send command")) {
            try {
                std::string cmd = cbuffIn;

                utils::ps4::PS4Process ps4{ ps4In };

                uint64_t cbuf1 = ps4[0x4D6C350];
                uint64_t cbuf2 = cbuf1 + 0x10004;

                ps4.Write(cbuf1, cmd.data(), cmd.size() + 1);
                ps4.Write<uint32_t>(cbuf2, (uint32_t)cmd.size());


                ps4.Notify(std::format("cbuf {}", cmd));
                notif = "";
            }
            catch (std::exception& e) {
                notif = std::format("Exception: {}", e.what());
            }
        }

        ImGui::Spacing();
        ImGui::SeparatorText("GSC injection");

        if (ImGui::InputText("GSC File", gscFileIn, sizeof(gscFileIn))) {
            core::config::SetString("ui.bo6.path", gscFileIn);
            c = true;
        }
        if (ImGui::Button("Open file...")) {
            // Open file

            OPENFILENAMEW ofn;
            WCHAR szFile[MAX_PATH + 1] = { 0 };

            // Initialize OPENFILENAME
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"Compiled GSC file (.gscc, .gsic, .gscobj)\0*.gscc;*.gsic;*.gscobj\0All\0*.*\0";
            ofn.lpstrTitle = L"Open GSC file";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileNameW(&ofn) == TRUE) {
                std::string injGsc = utils::WStrToStr(ofn.lpstrFile);
                core::config::SetString("ui.bo6.path", injGsc);
                snprintf(gscFileIn, sizeof(gscFileIn), "%s", injGsc.data());
                c = true;
            }
        }


        if (ImGui::Button("Inject PS4 Script")) {
            std::string file{};

            std::string filePath = gscFileIn;

            try {
                if (!utils::ReadFile(filePath, file)) {
                    throw std::runtime_error(std::format("Can't read '{}'", filePath));
                }

                if (file.size() >= 4 && !memcmp("GSC", file.data(), 4)) {
                    throw std::runtime_error("GSCBIN format not supported");
                }

                if (file.size() < 0x20) {
                    throw std::runtime_error(std::format("Invalid gsc file '{}'", filePath));
                }

                uint64_t magic = *reinterpret_cast<uint64_t*>(file.data());

                tool::gsc::opcode::VmInfo* nfo{};
                if (!tool::gsc::opcode::IsValidVmMagic(magic, nfo)) {
                    notif = (std::format("Invalid magic: 0x{:x}", magic));
                }
                else if (nfo->vmMagic == tool::gsc::opcode::VMI_T10_06) {
                    // bo6 injector

                    try {
                        tool::gsc::GscObj24* script{ (tool::gsc::GscObj24*)file.data() };

                        uint64_t name = script->name;

                        utils::ps4::PS4Process ps4{ ps4In };

                        auto pool = ps4.ReadObject<bo6::DB_AssetPool>(ps4[0x98FEFA0] + sizeof(bo6::DB_AssetPool) * bo6::T10_ASSET_GSCOBJ);


                        LOG_INFO("Pool: {:x}, count: {}/{}, len 0x{:x}", pool->m_entries, pool->m_loadedPoolSize, pool->m_poolSize, pool->m_elementSize);
                        auto objs = ps4.ReadArray<bo6::GscObjEntry>(pool->m_entries, pool->m_loadedPoolSize);

                        size_t i;
                        for (i = 0; i < pool->m_loadedPoolSize; i++) {
                            auto& obj = objs[i];

                            if (obj.name != name) {
                                continue;
                            }

                            if (!obj.buffer) {
                                throw std::runtime_error("Empty buffer");
                            }

                            if (obj.len < file.size()) {
                                throw std::runtime_error(utils::va("Buffer too small, can't remplace %llu < %llu", (size_t)obj.len, file.size()));
                            }

                            auto scriptTarget = ps4.ReadObject<tool::gsc::GscObj24>(obj.buffer);

                            if (scriptTarget->checksum != script->checksum) {
                                throw std::runtime_error("Find target script, but the checksum doesn't match");
                            }

                            ps4.Write(obj.buffer, file.data(), file.size());

                            notif = ("Script injected");
                            ps4.Notify("Script injected");
                        }
                        if (i == pool->m_loadedPoolSize) {
                            notif = ("Can't find hook script");
                        }
                    }
                    catch (std::exception& e) {
                        notif = (std::format("Exception: {}", e.what()));
                    }
                }
                else {
                    notif = (std::format("PS4 injector not implemented for VM: {}", nfo->name));
                }
            }
            catch (std::exception& e) {
                notif = std::format("Exception: {}", e.what());
            }
        }

        if (!notif.empty()) {
            ImGui::Separator();

            ImGui::Text("%s", notif.data());
        }

        if (c) tool::nui::SaveNextConfig();
    }
    
    ADD_TOOL(ps4cbufbo6, "bo6", " [ip:port] [cmd]", "", nullptr, ps4cbufbo6);
    ADD_TOOL(ps4dumpbo6, "bo6", " [ip:port]", "", nullptr, ps4dumpbo6);
    ADD_TOOL(ps4repbo6, "bo6", " [ip:port] [file]", "", nullptr, ps4repbo6);
    ADD_TOOL_UI(bo6_tools, L"BO6 PS4", Render, Update, Resize);
    ADD_TOOL_NUI(bo6_tools, "BO6 PS4", bo6_tools);
}
