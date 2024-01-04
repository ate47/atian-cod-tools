#include <includes.hpp>


namespace {

	constexpr auto s_assetPools_off = 0x11E50670;

    struct T9GSCOBJ {
        BYTE magic[8];
        INT32 crc;
        INT32 pad;
        UINT64 name;
        UINT16 string_count;
        UINT16 exports_count;
        UINT16 imports_count;
        UINT16 unk1E;
        UINT32 globalvar_count;
        UINT16 includes_count;
        UINT16 unk26;
        UINT32 loc_28;
        UINT32 start_exports;
        UINT32 string_offset;
        UINT32 includes_table;
        UINT32 exports_tables;
        UINT32 import_tables;
        UINT32 unk_40;
        UINT32 globalvar_offset;
        UINT32 file_size;
        UINT32 unk_4C;
        UINT16 export_size;
        UINT16 unk_52;
        UINT32 unk_54;
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
        int len;
    };


    enum XAssetType : BYTE
    {
        ASSET_TYPE_ZONE = 0,
        ASSET_TYPE_ASSETLIST = 1,
        ASSET_TYPE_PHYSPRESET = 2,
        ASSET_TYPE_PHYSCONSTRAINTS = 3,
        ASSET_TYPE_DESTRUCTIBLEDEF = 4,
        ASSET_TYPE_XANIM = 5,
        ASSET_TYPE_XMODEL = 6,
        ASSET_TYPE_XCOLLISION = 7,
        ASSET_TYPE_XSKELETON = 8,
        ASSET_TYPE_XMODELMESH = 9,
        ASSET_TYPE_MATERIAL = 10,
        ASSET_TYPE_CSDEF = 11,
        ASSET_TYPE_COMPUTESHADERSET = 12,
        ASSET_TYPE_RTSDEF = 13,
        ASSET_TYPE_RAYTRACESHADERSET = 14,
        ASSET_TYPE_TECHSET = 15,
        ASSET_TYPE_IMAGE = 16,
        ASSET_TYPE_SOUND = 17,
        ASSET_TYPE_SOUND_BANK = 18,
        ASSET_TYPE_SOUND_ASSET = 19,
        ASSET_TYPE_SOUND_DUCK = 20,
        ASSET_TYPE_SOUND_ALIAS_MODIFIER = 21,
        ASSET_TYPE_SOUND_ACOUSTICS = 22,
        ASSET_TYPE_COL_MAP = 23,
        ASSET_TYPE_CLIP_MAP = 24,
        ASSET_TYPE_COM_MAP = 25,
        ASSET_TYPE_GAME_MAP = 26,
        ASSET_TYPE_GFX_MAP = 27,
        ASSET_TYPE_FONTICON = 28,
        ASSET_TYPE_LOCALIZEENTRY = 29,
        ASSET_TYPE_GESTURE = 30,
        ASSET_TYPE_GESTURETABLE = 31,
        ASSET_TYPE_CINEMATICMOTION = 32,
        ASSET_TYPE_WEAPON = 33,
        ASSET_TYPE_WEAPONFULL = 34,
        ASSET_TYPE_WEAPONFRONTEND = 35,
        ASSET_TYPE_WEAPONBLUEPRINT = 36,
        ASSET_TYPE_WEAPONSTYLESETTINGS = 37,
        ASSET_TYPE_WEAPONSECONDARYMOVEMENT = 38,
        ASSET_TYPE_WEAPONTUNABLES = 39,
        ASSET_TYPE_CGMEDIATABLE = 40,
        ASSET_TYPE_PLAYERSOUNDSTABLE = 41,
        ASSET_TYPE_PLAYERFXTABLE = 42,
        ASSET_TYPE_SHAREDWEAPONSOUNDS = 43,
        ASSET_TYPE_ATTACHMENT = 44,
        ASSET_TYPE_ATTACHMENTUNIQUE = 45,
        ASSET_TYPE_WEAPONCAMO = 46,
        ASSET_TYPE_WEAPONCAMOBINDING = 47,
        ASSET_TYPE_CUSTOMIZATIONTABLE = 48,
        ASSET_TYPE_CUSTOMIZATIONTABLEFRONTEND = 49,
        ASSET_TYPE_SNDDRIVERGLOBALS = 50,
        ASSET_TYPE_FX = 51,
        ASSET_TYPE_TAGFX = 52,
        ASSET_TYPE_KLF = 53,
        ASSET_TYPE_IMPACTSFXTABLE = 54,
        ASSET_TYPE_IMPACTSOUNDSTABLE = 55,
        ASSET_TYPE_AITYPE = 56,
        ASSET_TYPE_CHARACTER = 57,
        ASSET_TYPE_XMODELALIAS = 58,
        ASSET_TYPE_RAWFILE = 59,
        ASSET_TYPE_RAWFILEPREPROC = 60,
        ASSET_TYPE_RAWTEXTFILE = 61,
        ASSET_TYPE_ANIMTREE = 62,
        ASSET_TYPE_STRINGTABLE = 63,
        ASSET_TYPE_STRUCTUREDTABLE = 64,
        ASSET_TYPE_LEADERBOARDDEF = 65,
        ASSET_TYPE_DDL = 66,
        ASSET_TYPE_GLASSES = 67,
        ASSET_TYPE_SCRIPTPARSETREE = 68,
        ASSET_TYPE_SCRIPTPARSETREEDBG = 69,
        ASSET_TYPE_SCRIPT_USING = 70,
        ASSET_TYPE_SCRIPT_USING_CP = 71,
        ASSET_TYPE_SCRIPT_USING_MP = 72,
        ASSET_TYPE_SCRIPT_USING_WZ = 73,
        ASSET_TYPE_SCRIPT_USING_ZM = 74,
        ASSET_TYPE_KEYVALUEPAIRS = 75,
        ASSET_TYPE_VEHICLE = 76,
        ASSET_TYPE_TRACER = 77,
        ASSET_TYPE_SURFACEFXTABLE = 78,
        ASSET_TYPE_SURFACESOUNDDEF = 79,
        ASSET_TYPE_FOOTSTEPTABLE = 80,
        ASSET_TYPE_ENTITYFXIMPACTS = 81,
        ASSET_TYPE_ENTITYSOUNDIMPACTS = 82,
        ASSET_TYPE_ZBARRIER = 83,
        ASSET_TYPE_VEHICLEFXDEF = 84,
        ASSET_TYPE_VEHICLESOUNDDEF = 85,
        ASSET_TYPE_TYPEINFO = 86,
        ASSET_TYPE_SCRIPTBUNDLE = 87,
        ASSET_TYPE_SCRIPTBUNDLELIST = 88,
        ASSET_TYPE_RUMBLE = 89,
        ASSET_TYPE_BULLETPENETRATION = 90,
        ASSET_TYPE_LOCDMGTABLE = 91,
        ASSET_TYPE_AIMTABLE = 92,
        ASSET_TYPE_SHOOTTABLE = 93,
        ASSET_TYPE_PLAYERGLOBALTUNABLES = 94,
        ASSET_TYPE_OVERHEADCAMERATUNABLES = 95,
        ASSET_TYPE_ANIMSELECTORTABLE = 96,
        ASSET_TYPE_ANIMMAPPINGTABLE = 97,
        ASSET_TYPE_ANIMSTATEMACHINE = 98,
        ASSET_TYPE_BEHAVIORTREE = 99,
        ASSET_TYPE_BEHAVIORSTATEMACHINE = 100,
        ASSET_TYPE_TTF = 101,
        ASSET_TYPE_SANIM = 102,
        ASSET_TYPE_SHELLSHOCK = 103,
        ASSET_TYPE_STATUSEFFECT = 104,
        ASSET_TYPE_CINEMATIC_CAMERA = 105,
        ASSET_TYPE_CINEMATIC_SEQUENCE = 106,
        ASSET_TYPE_SPECTATE_CAMERA = 107,
        ASSET_TYPE_XCAM = 108,
        ASSET_TYPE_BGCACHE = 109,
        ASSET_TYPE_FLAMETABLE = 110,
        ASSET_TYPE_BITFIELD = 111,
        ASSET_TYPE_MAPTABLE = 112,
        ASSET_TYPE_MAPTABLEENTRY = 113,
        ASSET_TYPE_MAPTABLELIST = 114,
        ASSET_TYPE_OBJECTIVE = 115,
        ASSET_TYPE_OBJECTIVELIST = 116,
        ASSET_TYPE_NAVMESH = 117,
        ASSET_TYPE_NAVVOLUME = 118,
        ASSET_TYPE_LASER = 119,
        ASSET_TYPE_BEAM = 120,
        ASSET_TYPE_STREAMERHINT = 121,
        ASSET_TYPE_FLOWGRAPH = 122,
        ASSET_TYPE_POSTFXBUNDLE = 123,
        ASSET_TYPE_LUAFILE = 124,
        ASSET_TYPE_LUAFILEDEBUG = 125,
        ASSET_TYPE_RENDEROVERRIDEBUNDLE = 126,
        ASSET_TYPE_STATICLEVELFXLIST = 127,
        ASSET_TYPE_TRIGGERLIST = 128,
        ASSET_TYPE_PLAYERROLETEMPLATE = 129,
        ASSET_TYPE_PLAYERROLETEMPLATEFRONTEND = 130,
        ASSET_TYPE_PLAYERROLECATEGORYTABLE = 131,
        ASSET_TYPE_PLAYERROLECATEGORY = 132,
        ASSET_TYPE_CHARACTERBODYTYPE = 133,
        ASSET_TYPE_CHARACTERBODYTYPEFRONTEND = 134,
        ASSET_TYPE_PLAYEROUTFIT = 135,
        ASSET_TYPE_GAMETYPETABLE = 136,
        ASSET_TYPE_GAMETYPETABLEENTRY = 137,
        ASSET_TYPE_FEATURE = 138,
        ASSET_TYPE_FEATURETABLE = 139,
        ASSET_TYPE_UNLOCKABLEITEM = 140,
        ASSET_TYPE_UNLOCKABLEITEMTABLE = 141,
        ASSET_TYPE_ENTITYLIST = 142,
        ASSET_TYPE_PLAYLISTS = 143,
        ASSET_TYPE_PLAYLISTGLOBALSETTINGS = 144,
        ASSET_TYPE_PLAYLISTSCHEDULE = 145,
        ASSET_TYPE_MOTIONMATCHINGINPUT = 146,
        ASSET_TYPE_BLACKBOARD = 147,
        ASSET_TYPE_TACTICALQUERY = 148,
        ASSET_TYPE_PLAYERMOVEMENTTUNABLES = 149,
        ASSET_TYPE_HIERARCHICALTASKNETWORK = 150,
        ASSET_TYPE_RAGDOLL = 151,
        ASSET_TYPE_STORAGEFILE = 152,
        ASSET_TYPE_STORAGEFILELIST = 153,
        ASSET_TYPE_CHARMIXER = 154,
        ASSET_TYPE_STOREPRODUCT = 155,
        ASSET_TYPE_STORECATEGORY = 156,
        ASSET_TYPE_STORECATEGORYLIST = 157,
        ASSET_TYPE_RANK = 158,
        ASSET_TYPE_RANKTABLE = 159,
        ASSET_TYPE_PRESTIGE = 160,
        ASSET_TYPE_PRESTIGETABLE = 161,
        ASSET_TYPE_FIRSTPARTYENTITLEMENT = 162,
        ASSET_TYPE_FIRSTPARTYENTITLEMENTLIST = 163,
        ASSET_TYPE_ENTITLEMENT = 164,
        ASSET_TYPE_ENTITLEMENTLIST = 165,
        ASSET_TYPE_SKU = 166,
        ASSET_TYPE_LABELSTORE = 167,
        ASSET_TYPE_LABELSTORELIST = 168,
        ASSET_TYPE_CPU_OCCLUSION_DATA = 169,
        ASSET_TYPE_LIGHTING = 170,
        ASSET_TYPE_DISTRICTS = 171,
        ASSET_TYPE_STREAMERWORLD = 172,
        ASSET_TYPE_TALENT = 173,
        ASSET_TYPE_PLAYERTALENTTEMPLATE = 174,
        ASSET_TYPE_PLAYERANIMATION = 175,
        ASSET_TYPE_UNUSED = 176,
        ASSET_TYPE_TERRAINGFX = 177,
        ASSET_TYPE_HIGHLIGHTREELINFODEFINES = 178,
        ASSET_TYPE_HIGHLIGHTREELPROFILEWEIGHTING = 179,
        ASSET_TYPE_HIGHLIGHTREELSTARLEVELS = 180,
        ASSET_TYPE_DLOGEVENT = 181,
        ASSET_TYPE_RAWSTRING = 182,
        ASSET_TYPE_BALLISTICDESC = 183,
        ASSET_TYPE_STREAMKEY = 184,
        ASSET_TYPE_RENDERTARGETS = 185,
        ASSET_TYPE_DRAWNODES = 186,
        ASSET_TYPE_GROUPLODMODEL = 187,
        ASSET_TYPE_FXLIBRARYVOLUME = 188,
        ASSET_TYPE_ARENASEASONS = 189,
        ASSET_TYPE_SPRAYORGESTUREITEM = 190,
        ASSET_TYPE_SPRAYORGESTURELIST = 191,
        ASSET_TYPE_HWPLATFORM = 192,
        ASSET_TYPE_ATTACHMENTTABLE = 193,
        ASSET_TYPE_NAVINPUT = 194,
        ASSET_TYPE_UIMODELDATASTRUCT = 195,
        ASSET_TYPE_CRAFTICON = 196,
        ASSET_TYPE_CRAFTICONLIST = 197,
        ASSET_TYPE_CRAFTWEAPONSTICKER = 198,
        ASSET_TYPE_CRAFTWEAPONSTICKERLIST = 199,
        ASSET_TYPE_CRAFTBACKGROUND = 200,
        ASSET_TYPE_CRAFTBACKGROUNDLIST = 201,
        ASSET_TYPE_CRAFTMATERIAL = 202,
        ASSET_TYPE_CRAFTMATERIALLIST = 203,
        ASSET_TYPE_CRAFTCATEGORY = 204,
        ASSET_TYPE_CRAFTCATEGORYLIST = 205,
        ASSET_TYPE_CRAFTWEAPONICONTRANSFORM = 206,
        ASSET_TYPE_CRAFTWEAPONICONTRANSFORMLIST = 207,
        ASSET_TYPE_XANIMCURVE = 208,
        ASSET_TYPE_DYNMODEL = 209,
        ASSET_TYPE_VECTORFIELD = 210,
        ASSET_TYPE_WINDDEF = 211,
        ASSET_TYPE_VEHICLEASSEMBLY = 212,
        ASSET_TYPE_MILESTONE = 213,
        ASSET_TYPE_MILESTONETABLE = 214,
        ASSET_TYPE_TRIGGEREFFECTDESC = 215,
        ASSET_TYPE_TRIGGERACTIONS = 216,
        ASSET_TYPE_PLAYERSETTINGS = 217,
        ASSET_TYPE_COMPASSTUNABLES = 218,
        ASSET_TYPE_EXECUTION = 219,
        ASSET_TYPE_SCENARIO = 220,
        ASSET_TYPE_COUNT
    };

	int dumppoolcw(const Process& proc, int argc, const char* argv[]) {

		LPCCH outFile;
		if (argc == 2) {
			outFile = "scriptparsetree_cw";
		}
		else {
			outFile = argv[2];
		}

		std::filesystem::create_directories(outFile);

		int gObjFileInfoCount[2];

		if (!proc.ReadMemory(gObjFileInfoCount, proc[0xF6F5FD0], sizeof(gObjFileInfoCount))) {
			std::cerr << "Can't read gObjFileInfoCount\n";
			return tool::BASIC_ERROR;
		}

		hashutils::ReadDefaultFile();

		auto elements = std::make_unique<tool::dump::T8ObjFileInfo[]>(800ull * 2 * sizeof(tool::dump::T8ObjFileInfo));

		if (!proc.ReadMemory(&elements[0], proc[0xF6EC9D0], sizeof(elements[0]) * 800 * 2)) {
			std::cerr << "Can't read gObjFileInfo\n";
			return tool::BASIC_ERROR;
		}

		T9GSCOBJ headerTmp{};

		CHAR namebuff[MAX_PATH + 10];
        std::cout << "dump using linked scripts\n";

		for (size_t i = 0; i < scriptinstance::SI_COUNT; i++) {
			for (size_t j = 0; j < gObjFileInfoCount[i]; j++) {
				auto& elem = elements[i * 800 + j];

				if (!proc.ReadMemory(&headerTmp, elem.activeVersion, sizeof(headerTmp))) {
					std::cerr << "Can't read elem at " << j << "\n";
					continue;
				}

				auto file = std::make_unique<BYTE[]>(headerTmp.file_size);

				if (!proc.ReadMemory(&file[0], elem.activeVersion, headerTmp.file_size)) {
					std::cerr << "Can't read file elem at " << j << "\n";
					continue;
				}

				sprintf_s(namebuff, "%s/script_%llx.gscc", outFile, headerTmp.name);

				if (utils::WriteFile(namebuff, &file[0], headerTmp.file_size)) {
					std::cout << "- " << hashutils::ExtractTmpScript(headerTmp.name) << " -> " << namebuff << "\n";
				}
				else {
					std::cerr << "Error when writting " << namebuff << "\n";
				}
			}
		}
		
        XAssetPool sptPool{};

        if (!proc.ReadMemory(&sptPool, proc[s_assetPools_off] + sizeof(sptPool) * ASSET_TYPE_SCRIPTPARSETREE, sizeof(sptPool))) {
            std::cerr << "Can't read SPT pool\n";
            return tool::BASIC_ERROR;
        }
		
        auto entries = std::make_unique<ScriptParseTree[]>(sptPool.itemAllocCount);

        if (!proc.ReadMemory(&entries[0], sptPool.pool, sptPool.itemAllocCount * sizeof(entries[0]))) {
            std::cerr << "Can't read SPT pool entries\n";
            return tool::BASIC_ERROR;
        }

        std::cout << "dump using pool\n";

        for (size_t i = 0; i < sptPool.itemAllocCount; i++) {
            auto& e = entries[i];

            if (!proc.ReadMemory(&headerTmp, e.buffer, sizeof(headerTmp))) {
                continue;
            }

            auto file = std::make_unique<BYTE[]>(headerTmp.file_size);

            if (!proc.ReadMemory(&file[0], e.buffer, headerTmp.file_size)) {
                std::cerr << "Can't read file elem at " << i << "\n";
                continue;
            }

            sprintf_s(namebuff, "%s/script_%llx.gscc", outFile, headerTmp.name);

            if (utils::WriteFile(namebuff, &file[0], headerTmp.file_size)) {
                std::cout << "- " << hashutils::ExtractTmpScript(headerTmp.name) << " -> " << namebuff << "\n";
            }
            else {
                std::cerr << "Error when writting " << namebuff << "\n";
            }
        }


		return tool::OK;
	}


	const char* ReadTmpStr(const Process& proc, uintptr_t location) {
		static CHAR tmp_buff[0x1000];

		if (proc.ReadString(tmp_buff, location, sizeof(tmp_buff)) < 0) {
			sprintf_s(tmp_buff, "<invalid:%llx>", location);
		}
		return tmp_buff;
	}

	int dpnamescw(const Process& proc, int argc, const char* argv[]) {

		auto loc = proc[0xD7C8D90];

		XAssetPool pool{};
		CHAR tmp_buff[0x50];

		int id = 0;

		std::cout << "id,name,itemSize,itemCount,itemAllocCount\n";

		while (true) {
			auto addr = proc.ReadMemory<uintptr_t>(loc + id * 8);
			if (!addr || proc.ReadString(tmp_buff, addr, sizeof(tmp_buff)) < 0) {
				break;
			}
			if (!proc.ReadMemory(&pool, proc[s_assetPools_off] + sizeof(pool) * id, sizeof(pool))) {
				break;
			}

			std::cout << std::dec << id << "," << tmp_buff << "," << std::hex << pool.itemSize << "," << pool.itemCount << "," << pool.itemAllocCount << "\n";

			id++;
		}


		return tool::OK;
	}

}
ADD_TOOL("wpscw", "", "write pooled scripts (cw)", L"BlackOpsColdWar.exe", dumppoolcw);
ADD_TOOL("dpncw", "", "dump pool names (cw)", L"BlackOpsColdWar.exe", dpnamescw);