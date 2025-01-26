#include <includes.hpp>
#include <tools/ff/fastfile_handlers.hpp>

namespace {
	enum XAssetType : byte {
		ASSET_TYPE_PHYSPRESET = 0x0,
		ASSET_TYPE_PHYSCONSTRAINTS = 0x1,
		ASSET_TYPE_DESTRUCTIBLEDEF = 0x2,
		ASSET_TYPE_XANIMPARTS = 0x3,
		ASSET_TYPE_XMODEL = 0x4,
		ASSET_TYPE_XMODELMESH = 0x5,
		ASSET_TYPE_MATERIAL = 0x6,
		ASSET_TYPE_COMPUTE_SHADER_SET = 0x7,
		ASSET_TYPE_TECHNIQUE_SET = 0x8,
		ASSET_TYPE_IMAGE = 0x9,
		ASSET_TYPE_SOUND = 0xA,
		ASSET_TYPE_CLIPMAP = 0xB,
		ASSET_TYPE_COMWORLD = 0xC,
		ASSET_TYPE_GAMEWORLD = 0xD,
		ASSET_TYPE_GFXWORLD = 0xE,
		ASSET_TYPE_FONTICON = 0xF,
		ASSET_TYPE_LOCALIZE_ENTRY = 0x10,
		ASSET_TYPE_LOCALIZE_LIST = 0x11,
		ASSET_TYPE_WEAPON = 0x12,
		ASSET_TYPE_WEAPON_FULL = 0x13,
		ASSET_TYPE_CGMEDIA = 0x14,
		ASSET_TYPE_PLAYERSOUNDS = 0x15,
		ASSET_TYPE_PLAYERFX = 0x16,
		ASSET_TYPE_SHAREDWEAPONSOUNDS = 0x17,
		ASSET_TYPE_ATTACHMENT = 0x18,
		ASSET_TYPE_ATTACHMENT_UNIQUE = 0x19,
		ASSET_TYPE_WEAPON_CAMO = 0x1A,
		ASSET_TYPE_CUSTOMIZATION_TABLE = 0x1B,
		ASSET_TYPE_CUSTOMIZATION_TABLE_FE_IMAGES = 0x1C,
		ASSET_TYPE_CUSTOMIZATION_TABLE_COLOR = 0x1D,
		ASSET_TYPE_SNDDRIVER_GLOBALS = 0x1E,
		ASSET_TYPE_FX = 0x1F,
		ASSET_TYPE_TAGFX = 0x20,
		ASSET_TYPE_NEW_LENSFLARE_DEF = 0x21,
		ASSET_TYPE_IMPACT_FX = 0x22,
		ASSET_TYPE_IMPACT_SOUND = 0x23,
		ASSET_TYPE_AITYPE = 0x24,
		ASSET_TYPE_CHARACTER = 0x25,
		ASSET_TYPE_XMODELALIAS = 0x26,
		ASSET_TYPE_RAWFILE = 0x27,
		ASSET_TYPE_STRINGTABLE = 0x28,
		ASSET_TYPE_STRUCTURED_TABLE = 0x29,
		ASSET_TYPE_LEADERBOARD = 0x2A,
		ASSET_TYPE_DDL = 0x2B,
		ASSET_TYPE_GLASSES = 0x2C,
		ASSET_TYPE_SCRIPTPARSETREE = 0x2D,
		ASSET_TYPE_SCRIPTPARSETREEDBG = 0x2E,
		ASSET_TYPE_KEYVALUEPAIRS = 0x2F,
		ASSET_TYPE_VEHICLEDEF = 0x30,
		ASSET_TYPE_TRACER = 0x31,
		ASSET_TYPE_SURFACEFX_TABLE = 0x32,
		ASSET_TYPE_SURFACESOUNDDEF = 0x33,
		ASSET_TYPE_FOOTSTEP_TABLE = 0x34,
		ASSET_TYPE_ENTITYFXIMPACTS = 0x35,
		ASSET_TYPE_ENTITYSOUNDIMPACTS = 0x36,
		ASSET_TYPE_ZBARRIER = 0x37,
		ASSET_TYPE_VEHICLEFXDEF = 0x38,
		ASSET_TYPE_VEHICLESOUNDDEF = 0x39,
		ASSET_TYPE_TYPEINFO = 0x3A,
		ASSET_TYPE_SCRIPTBUNDLE = 0x3B,
		ASSET_TYPE_SCRIPTBUNDLELIST = 0x3C,
		ASSET_TYPE_RUMBLE = 0x3D,
		ASSET_TYPE_BULLETPENETRATION = 0x3E,
		ASSET_TYPE_LOCDMGTABLE = 0x3F,
		ASSET_TYPE_AIMTABLE = 0x40,
		ASSET_TYPE_ANIMSELECTORTABLESET = 0x41,
		ASSET_TYPE_ANIMMAPPINGTABLE = 0x42,
		ASSET_TYPE_ANIMSTATEMACHINE = 0x43,
		ASSET_TYPE_BEHAVIORTREE = 0x44,
		ASSET_TYPE_BEHAVIORSTATEMACHINE = 0x45,
		ASSET_TYPE_TTF = 0x46,
		ASSET_TYPE_SANIM = 0x47,
		ASSET_TYPE_LIGHT_DESCRIPTION = 0x48,
		ASSET_TYPE_SHELLSHOCK = 0x49,
		ASSET_TYPE_CINEMATIC_CAMERA = 0x4A,
		ASSET_TYPE_XCAM = 0x4B,
		ASSET_TYPE_BG_CACHE = 0x4C,
		ASSET_TYPE_TEXTURE_COMBO = 0x4D,
		ASSET_TYPE_FLAMETABLE = 0x4E,
		ASSET_TYPE_BITFIELD = 0x4F,
		ASSET_TYPE_ATTACHMENT_COSMETIC_VARIANT = 0x50,
		ASSET_TYPE_MAPTABLE = 0x51,
		ASSET_TYPE_MAPTABLE_LOADING_IMAGES = 0x52,
		ASSET_TYPE_MAPTABLEENTRY_LEVEL_ASSETS = 0x53,
		ASSET_TYPE_MEDAL = 0x54,
		ASSET_TYPE_MEDALTABLE = 0x55,
		ASSET_TYPE_OBJECTIVE = 0x56,
		ASSET_TYPE_OBJECTIVE_LIST = 0x57,
		ASSET_TYPE_NAVMESH = 0x58,
		ASSET_TYPE_NAVVOLUME = 0x59,
		ASSET_TYPE_BINARYHTML = 0x5A,
		ASSET_TYPE_LASER = 0x5B,
		ASSET_TYPE_BEAM = 0x5C,
		ASSET_TYPE_STREAMER_HINT = 0x5D,
		ASSET_TYPE_FLOWGRAPH = 0x5E,
		ASSET_TYPE_POSTFXBUNDLE = 0x5F,
		ASSET_TYPE_LUAFILE = 0x60,
		ASSET_TYPE_LUAFILE_DBG = 0x61,
		ASSET_TYPE_RENDEROVERRIDEBUNDLE = 0x62,
		ASSET_TYPE_STATIC_LEVEL_FX_LIST = 0x63,
		ASSET_TYPE_TRIGGER_LIST = 0x64,
		ASSET_TYPE_PLAYER_ROLE_TEMPLATE = 0x65,
		ASSET_TYPE_PLAYER_ROLE_CATEGORY_TABLE = 0x66,
		ASSET_TYPE_PLAYER_ROLE_CATEGORY = 0x67,
		ASSET_TYPE_CHARACTER_BODY_TYPE = 0x68,
		ASSET_TYPE_GAMETYPETABLE = 0x69,
		ASSET_TYPE_UNLOCKABLE_ITEM_TABLE = 0x6A,
		ASSET_TYPE_ENTITY_LIST = 0x6B,
		ASSET_TYPE_PLAYLISTS = 0x6C,
		ASSET_TYPE_PLAYLIST_GLOBAL_SETTINGS = 0x6D,
		ASSET_TYPE_PLAYLIST_EVENT = 0x6E,
		ASSET_TYPE_PLAYLIST_EVENT_SCHEDULE = 0x6F,
		ASSET_TYPE_MOTION_MATCHING_INPUT = 0x70,
		ASSET_TYPE_BLACKBOARD = 0x71,
		ASSET_TYPE_TACTICALQUERY = 0x72,
		ASSET_TYPE_PMOVE = 0x73,
		ASSET_TYPE_HIERARCHICAL_TASK_NETWORK = 0x74,
		ASSET_TYPE_RAGDOLL = 0x75,
		ASSET_TYPE_STORAGEFILE = 0x76,
		ASSET_TYPE_STORAGEFILELIST = 0x77,
		ASSET_TYPE_CHARMIXER = 0x78,
		ASSET_TYPE_COUNT = 0x79,
		ASSET_TYPE_ASSETLIST = 0x79,
		ASSET_TYPE_REPORT = 0x7A,
		ASSET_TYPE_FULL_COUNT = 0x7B,
	};


	class BO4DevFFHandler : public fastfile::FFHandler {
	public:
		BO4DevFFHandler() : fastfile::FFHandler("bo4dev", "Test bo4 dev") {
		}

		void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
			std::filesystem::path out{ opt.m_output / "bo4dev" / ctx.ffname };
			std::filesystem::create_directories(out);

			struct XAsset_0 {
				XAssetType type;
				void* header;
			}; static_assert(sizeof(XAsset_0) == 0x10);


			struct ScriptStringList {
				int count;
				const char** strings;
			};

			struct XAssetList_0 {
				ScriptStringList stringList;
				int assetCount;
				XAsset_0* assets;
			};

			XAssetList_0& assetList{ *reader.ReadPtr<XAssetList_0>() };

			if (assetList.stringList.count) {
				reader.Align<void*>();
				assetList.stringList.strings = reader.ReadPtr<const char*>(assetList.stringList.count);

				for (size_t i = 0; i < assetList.stringList.count; i++) {
					if (fastfile::IsSame(assetList.stringList.strings[i], -1)) {
						assetList.stringList.strings[i] = reader.ReadString();
					}
					else {

						assetList.stringList.strings[i] = "";
					}
				}
			}

			{
				std::filesystem::path outStrings{ out / "strings.txt" };
				utils::OutFileCE os{ outStrings };
				if (!os) {
					throw std::runtime_error(std::format("Can't open {}", outStrings.string()));
				}
				for (size_t i = 0; i < assetList.stringList.count; i++) {
					os << assetList.stringList.strings[i] << "\n";
				}
				LOG_INFO("Dump strings into {}", outStrings.string());
			}

			if (!assetList.assetCount) {
				LOG_TRACE("not asset");
				return;
			}
			//reader.Align<void*>();
			LOG_TRACE("assets: 0x{:x}", reader.Loc());
			assetList.assets = reader.ReadPtr<XAsset_0>(assetList.assetCount);

			std::filesystem::path outAssets{ out / "assets.csv" };
			utils::OutFileCE osa{ outAssets };
			if (!osa) {
				throw std::runtime_error(std::format("Can't open {}", outAssets.string()));
			}

			osa << "type,name";

			auto ReadString = [&reader](const char* str) -> const char* {
				if (fastfile::IsSame(str, -1)) {
					return reader.ReadString();
				}
				else {
					return "<empty>";
				}
			};

			for (size_t i = 0; i < assetList.assetCount; i++) {
				XAsset_0& asset{ assetList.assets[i] };

				switch (asset.type) {
				case ASSET_TYPE_PHYSPRESET: {
					reader.Align<void*>();
					byte* assetData{ reader.ReadPtr<byte>(0x78) };
					const char* name{ ReadString((const char*)&assetData[0]) };
					const char* sndAliasPrefix{ ReadString((const char*)&assetData[0x28]) };

					osa << "\n" "ASSET_TYPE_PHYSPRESET," << name;
					break;
				}
				case ASSET_TYPE_IMAGE: {
					reader.Align<void*>();
					byte* assetData{ reader.ReadPtr<byte>(0x120) };
					const char* name{ ReadString((const char*)&assetData[0x10]) };

					osa << "\n" "ASSET_TYPE_IMAGE," << name;
					// load pixels
					// TODO:
					break;
				}
				default:
					throw std::runtime_error(std::format("Missing implementation for asset type 0x{:x}", (int)asset.type));
				}
			}
		}
	};

	utils::ArrayAdder<BO4DevFFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}
