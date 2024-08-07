#pragma once

namespace sp23 {
	enum AssetType : int {
		ASSET_PHYSICSLIBRARY = 0x0,
		ASSET_PHYSICSSFXEVEN = 0x1,
		ASSET_PHYSICSVFXEVEN = 0x2,
		ASSET_PHYSICSASSET = 0x3,
		ASSET_PHYSICSFXPIPEL = 0x4,
		ASSET_PHYSICSFXSHAPE = 0x5,
		ASSET_PHYSICSDEBUGDA = 0x6,
		ASSET_XANIM = 0x7,
		ASSET_XMODELSURFS = 0x8,
		ASSET_XMODEL = 0x9,
		ASSET_MAYHEM = 0xA,
		ASSET_MATERIAL = 0xB,
		ASSET_COMPUTESHADER = 0xC,
		ASSET_TILESHADER = 0xD,
		ASSET_LIBSHADER = 0xE,
		ASSET_VERTEXSHADER = 0xF,
		ASSET_HULLSHADER = 0x10,
		ASSET_DOMAINSHADER = 0x11,
		ASSET_PIXELSHADER = 0x12,
		ASSET_SERIALIZEDSHAD = 0x13,
		ASSET_TECHSET = 0x14,
		ASSET_IMAGE = 0x15,
		ASSET_SOUNDGLOBALVOL = 0x16,
		ASSET_SOUNDGLOBALENT = 0x17,
		ASSET_SOUNDGLOBALCON = 0x18,
		ASSET_SOUNDGLOBALWHI = 0x19,
		ASSET_SOUNDGLOBALBUL = 0x1A,
		ASSET_SOUNDGLOBALPER = 0x1B,
		ASSET_SOUNDGLOBALOCC = 0x1C,
		ASSET_SOUNDGLOBALSUR = 0x1D,
		ASSET_SOUNDGLOBALCUR = 0x1E,
		ASSET_SOUNDGLOBALDOP = 0x1F,
		ASSET_SOUNDGLOBALSHA = 0x20,
		ASSET_SOUNDGLOBALFUT = 0x21,
		ASSET_SOUNDGLOBALSEN = 0x22,
		ASSET_SOUNDGLOBALNAM = 0x23,
		ASSET_SOUNDGLOBALFAC = 0x24,
		ASSET_SOUNDBANK = 0x25,
		ASSET_SOUNDBANKMERGE = 0x26,
		ASSET_SOUNDBANKTRANS = 0x27,
		ASSET_COLMAP = 0x28,
		ASSET_COMMAP = 0x29,
		ASSET_GLASSMAP = 0x2A,
		ASSET_AIPATHS = 0x2B,
		ASSET_NAVMESH = 0x2C,
		ASSET_TACGRAPH = 0x2D,
		ASSET_AIREGIONGRAPHS = 0x2E,
		ASSET_MAPENTS = 0x2F,
		ASSET_MAPENTSTRZONE = 0x30,
		ASSET_FXMAP = 0x31,
		ASSET_GFXMAP = 0x32,
		ASSET_GFXMAPTRZONE = 0x33,
		ASSET_IESPROFILE = 0x34,
		ASSET_LIGHTDEF = 0x35,
		ASSET_GRADINGCLUT = 0x36,
		ASSET_FOGSPLINE = 0x37,
		ASSET_ANIMCLASS = 0x38,
		ASSET_PLAYERANIM = 0x39,
		ASSET_NK_8A41D00 = 0x3A,
		ASSET_LOCALIZE = 0x3B,
		ASSET_ATTACHMENT = 0x3C,
		ASSET_WEAPON = 0x3D,
		ASSET_VFX = 0x3E,
		ASSET_IMPACTFX = 0x3F,
		ASSET_SURFACEFX = 0x40,
		ASSET_AITYPE = 0x41,
		ASSET_charACTER = 0x42,
		ASSET_XMODELALIAS = 0x43,
		ASSET_RAWFILE = 0x44,
		ASSET_GSCOBJ = 0x45,
		ASSET_GSCGDB = 0x46,
		ASSET_STRINGTABLE = 0x47,
		ASSET_DDL = 0x48,
		ASSET_TRACER = 0x49,
		ASSET_VEHICLE = 0x4A,
		ASSET_NETCONSTSTRING = 0x4B,
		ASSET_LUAFILE = 0x4C,
		ASSET_SCRIPTABLE = 0x4D,
		ASSET_VECTORFIELD = 0x4E,
		ASSET_PARTICLESIMANI = 0x4F,
		ASSET_STREAMINGINFO = 0x50,
		ASSET_LASER = 0x51,
		ASSET_GAMEPROPS = 0x52,
		ASSET_MATERIALSTANDA = 0x53,
		ASSET_BEAM = 0x54,
		ASSET_TTF = 0x55,
		ASSET_SUIT = 0x56,
		ASSET_SUITANIMPACKAG = 0x57,
		ASSET_CAMERA = 0x58,
		ASSET_HUDOUTLINE = 0x59,
		ASSET_RUMBLE = 0x5A,
		ASSET_RUMBLEGRAPH = 0x5B,
		ASSET_ANIMPKG = 0x5C,
		ASSET_SFXPKG = 0x5D,
		ASSET_VFXPKG = 0x5E,
		ASSET_FOOTSTEPVFX = 0x5F,
		ASSET_BEHAVIORTREE = 0x60,
		ASSET_BEHAVIORSEQUEN = 0x61,
		ASSET_SIGHTCONFIG = 0x62,
		ASSET_SIGHTCONFIGTEM = 0x63,
		ASSET_AIANIMSET = 0x64,
		ASSET_AIASM = 0x65,
		ASSET_PROCEDURALBONE = 0x66,
		ASSET_DYNAMICBONES = 0x67,
		ASSET_PROCEDURALBLEN = 0x68,
		ASSET_RETICLE_0 = 0x69,
		ASSET_XANIMCURVE = 0x6A,
		ASSET_COVERSELECTOR = 0x6B,
		ASSET_ENEMYSELECTOR = 0x6C,
		ASSET_CLIENTcharACTE_1 = 0x6D,
		ASSET_CLOTHASSET = 0x6E,
		ASSET_CINEMATICMOTIO = 0x6F,
		ASSET_ACCESSORY = 0x70,
		ASSET_LOCDMGTABLE = 0x71,
		ASSET_BULLETPENETRAT = 0x72,
		ASSET_SCRIPTBUNDLE = 0x73,
		ASSET_BLENDSPACE2D = 0x74,
		ASSET_XCAM = 0x75,
		ASSET_CAMO_0 = 0x76,
		ASSET_XCOMPOSITEMODE = 0x77,
		ASSET_XMODELDETAILCO = 0x78,
		ASSET_STREAMTREEOVER = 0x79,
		ASSET_KEYVALUEPAIRS = 0x7A,
		ASSET_STTERRAIN = 0x7B,
		ASSET_VHMDATA = 0x7C,
		ASSET_VTMSURFACEMAPS = 0x7D,
		ASSET_NK_8A41CB0 = 0x7E,
		ASSET_EXECUTION = 0x7F,
		ASSET_CARRYOBJECT = 0x80,
		ASSET_SOUNDBANKLIST = 0x81,
		ASSET_WEAPONACCURACY = 0x82,
		ASSET_DECALVOLUMEMAT = 0x83,
		ASSET_DECALVOLUMEMAS = 0x84,
		ASSET_DYNENTITYLIST = 0x85,
		ASSET_FXMAPTRZONE = 0x86,
		ASSET_VOLUMETRICHEIG = 0x87,
		ASSET_DLOGSCHEMA = 0x88,
		ASSET_EDGELIST = 0x89,
		ASSET_STANDALONEUMBR = 0x8A,
		ASSET_XBONESET = 0x8B,
		ASSET_RAGDOLLASSET = 0x8C,
		ASSET_PHYSICSBONEGRA = 0x8D,
		ASSET_CURVE = 0x8E,
		ASSET_SKELETONCONSTR = 0x8F,
		ASSET_TRIGGEREFFECT = 0x90,
		ASSET_WEAPONTRIGGER = 0x91,
		ASSET_VOLUMETRICCLOU = 0x92,
		ASSET_CODCASTERDATA = 0x93,
		ASSET_WATERSYSTEM = 0x94,
		ASSET_WATERBUOYANCY = 0x95,
		ASSET_KEYBINDS_0 = 0x96,
		ASSET_CALLOUTMARKERP_1 = 0x97,
		ASSET_LIGHTSTATE = 0x98,
		ASSET_RADIANTTELEMET = 0x99,
		ASSET_AIMARKUP = 0x9A,
		ASSET_AIMARKUPGENERA = 0x9B,
		ASSET_SCENARIO = 0x9C,
		ASSET_AIINTERACTION = 0x9D,
		ASSET_MAPVOXELIZEDST = 0x9E,
		ASSET_WATERVIS = 0x9F,
		ASSET_GAMETYPE_0 = 0xA0,
		ASSET_GAMETYPETABLE = 0xA1,
		ASSET_SNDMODIFIER = 0xA2,
		ASSET_WEAPONBLUEPRIN = 0xA3,
		ASSET_ATTACHMENTBLUE = 0xA4,
		ASSET_MOVINGPLATFORM = 0xA5,
		ASSET_HWCONFIG = 0xA6,
		ASSET_TIMELAPSESKY = 0xA7,
		ASSET_HWCONFIGURATOR = 0xA8,
		ASSET_OBJECTIVEDATA = 0xA9,
		ASSET_FONT = 0xAA,
		ASSET_MOTIONMATCHING = 0xAB,
		ASSET_MOTIONMATCHING_0 = 0xAC,
		ASSET_GAMETYPESTATDA = 0xAD,
		ASSET_FONTICON = 0xAE,
		ASSET_CALLOUTMARKERP_2 = 0xAF,
		ASSET_HWCONFIGVAR = 0xB0,
		ASSET_ZIVART = 0xB1,
		ASSET_MOVIE = 0xB2,
		ASSET_MAPINFO = 0xB3,
		ASSET_MAPTABLE = 0xB4,
		ASSET_ACHIEVEMENT_0 = 0xB5,
		ASSET_ACHIEVEMENTLIS = 0xB6,
		ASSET_MATERIALDEBUGD = 0xB7,
		ASSET_SCRIPTABLEVARI = 0xB8,
		ASSET_LEAGUEPLAYSEAS = 0xB9,
		ASSET_SETTINGCONTEXT = 0xBA,
		ASSET_AIEVENTLIST = 0xBB,
		ASSET_SOUNDEVENT = 0xBC,
		ASSET_CALLOUTMARKERP_3 = 0xBD,
		ASSET_PROJECT = 0xBE,
		ASSET_PROJECTTABLE = 0xBF,
		ASSET_GAMEMODE = 0xC0,
		ASSET_SNDASSET = 0xC1,
		ASSET_GFXUMBRATOME = 0xC2,
		ASSET_AUDIOVISUALIZE = 0xC3,
		ASSET_MATERIALANIMAT_0 = 0xC4,
		ASSET_NAMEPLATESETTI = 0xC5,
		ASSET_REACTIVEAUDIOP = 0xC6,
		ASSET_REACTIVEVFXPAC = 0xC7,
		ASSET_MATERIALSFXTAB = 0xC8,
		ASSET_FOOTSTEPSFXTAB = 0xC9,
		ASSET_REACTIVESTAGES = 0xCA,
		ASSET_FOLIAGESFXTABL = 0xCB,
		ASSET_IMPACTSFXTABLE = 0xCC,
		ASSET_AIIMPACTVFXTAB = 0xCD,
		ASSET_TYPEINFO = 0xCE,
		ASSET_HANDPLANTSFXTA = 0xCF,
		ASSET_SNDTABLE = 0xD0,
		ASSET_EQUIPMENTSFX = 0xD1,
		ASSET_SOUNDSUBMIX = 0xD2,
		ASSET_SHOCK = 0xD3,
		ASSET_STORAGEFILE = 0xD4,
		ASSET_ECSASSET = 0xD5,
		ASSET_TRACKERFOOTSTE = 0xD6,
		ASSET_PLAYERSPAWNSET = 0xD7,
		ASSET_PLAYERSPAWNINF = 0xD8,
		ASSET_SOUNDSPEAKERMA = 0xD9,
		ASSET_REVERBPRESET = 0xDA,
		ASSET_AISHOOTSTYLESL = 0xDB,
		ASSET_OPERATORLIST = 0xDC,
		ASSET_OPERATOR = 0xDD,
		ASSET_OPERATORSKIN = 0xDE,
		ASSET_DISMEMBERMENT = 0xDF,
		ASSET_CONVERSATION = 0xE0,
		ASSET_XANIMNODE = 0xE1,
		ASSET_SNDMODIFIERSET = 0xE2,
		ASSET_SNDCURVE = 0xE3,
		ASSET_TTLOS = 0xE4,
		ASSET_MATERIALTINTAN = 0xE5,
		ASSET_MATERIALUVANIM = 0xE6,
		ASSET_MATERIALCAMOAN = 0xE7,
		ASSET_MATERIALANIMAT = 0xE8,
		ASSET_IMPACTFXTABLE = 0xE9,
		ASSET_IMPACTTYPETOIM = 0xEA,
		ASSET_REACTIVEOPERAT = 0xEB,
		ASSET_WEATHERVOLUME = 0xEC,
		ASSET_VEHICLETRICK = 0xED,
		ASSET_REACTIVEAUDIOP_2 = 0xEE,
		ASSET_AMBIENTSFXPACK = 0xEF,
		ASSET_OBJECTSTOREPRO = 0xF0,
		ASSET_OBJECTSTOREGAM = 0xF1,
		ASSET_PROCEDURALBONE_2 = 0xF2,
		ASSET_HWCONFIGVARGRO = 0xF3,
		ASSET_HWCONFIGTIERED = 0xF4,
		ASSET_SNDMASTERPRESE = 0xF5,
		ASSET_GENERICBLUEPRI = 0xF6,
		ASSET_COUNT = 0xF7,
	};

	enum JupScrVarType : byte {
		TYPE_UNDEFINED = 0x0,
		TYPE_POINTER = 0x1,
		TYPE_STRING = 0x2,
		TYPE_ISTRING = 0x3,
		TYPE_VECTOR = 0x4,
		TYPE_FLOAT = 0x5,
		TYPE_INTEGER = 0x6,
		TYPE_UNK7 = 0x7,
		TYPE_HASH = 0x8,
		TYPE_UNK9 = 0x9,
		TYPE_DVarHash = 0xA,
		TYPE_UNKb = 0xB,
		TYPE_UNKc = 0xC,
		TYPE_UNKd = 0xD,
		TYPE_UNKe = 0xE,
		TYPE_FUNCTION = 0xF,
		TYPE_BUILTIN_FUNCTION = 0x10,
		TYPE_BUILTIN_METHOD = 0x11,
		TYPE_UNK12 = 0x12,
		TYPE_ANIMATION = 0x13,
		TYPE_UNK14 = 0x14,
		TYPE_UNK15 = 0x15,
		TYPE_UNK16 = 0x16,
		TYPE_UNK17 = 0x17,
		TYPE_UNK18 = 0x18,
		TYPE_UNK19 = 0x19,
		TYPE_UNK1a = 0x1A,
		TYPE_STRUCT = 0x1B,
		TYPE_ENTITY = 0x1D,
		TYPE_ARRAY = 0x1E,
	};


	const char* AssetTypeName(AssetType type);
	AssetType AssetTypeFromName(const char* name);
}