#include <includes_shared.hpp>
#include "pool.hpp"

namespace games::bo4::pool {
	namespace {
		const char* xassetsNames[] = {
			"physpreset",
			"physconstraints",
			"destructibledef",
			"xanim",
			"xmodel",
			"xmodelmesh",
			"material",
			"computeshaderset",
			"techset",
			"image",
			"sound",
			"col_map",
			"com_map",
			"game_map",
			"gfx_map",
			"fonticon",
			"localizeentry",
			"localize",
			"gesture",
			"gesturetable",
			"weapon",
			"weaponfull",
			"weapontunables",
			"cgmediatable",
			"playersoundstable",
			"playerfxtable",
			"sharedweaponsounds",
			"attachment",
			"attachmentunique",
			"weaponcamo",
			"customizationtable",
			"customizationtable_feimages",
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
			"scriptparsetreeforced",
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
			"animselectortable",
			"animmappingtable",
			"animstatemachine",
			"behaviortree",
			"behaviorstatemachine",
			"ttf",
			"sanim",
			"lightdescription",
			"shellshock",
			"statuseffect",
			"cinematic_camera",
			"cinematic_sequence",
			"spectate_camera",
			"xcam",
			"bgcache",
			"texturecombo",
			"flametable",
			"bitfield",
			"maptable",
			"maptablelist",
			"maptableloadingimages",
			"maptablepreviewimages",
			"maptableentrylevelassets",
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
			"playerrolecategorytable",
			"playerrolecategory",
			"characterbodytype",
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
		};

		const char* bgcacheNames[] = {
			"<invalid>",
			"vehicle",
			"model",
			"aitype",
			"character",
			"xmodelalias",
			"weapon",
			"gesture",
			"gesturetable",
			"zbarrier",
			"rumble",
			"shellshock",
			"statuseffect",
			"xcam",
			"destructible",
			"streamerhint",
			"flowgraph",
			"xanim",
			"sanim",
			"scriptbundle",
			"talent",
			"statusicon",
			"locationselector",
			"menu",
			"material",
			"string",
			"eventstring",
			"moviefile",
			"objective",
			"fx",
			"lui_menu_data",
			"lui_elem",
			"lui_elem_uid",
			"radiant_exploder",
			"soundalias",
			"client_fx",
			"client_tagfxset",
			"client_lui_elem",
			"client_lui_elem_uid",
			"requires_implements",
		};

	}

	XAssetType XAssetIdFromName(const char* name) {
		for (size_t i = 0; i < ARRAYSIZE(xassetsNames); i++) {
			if (!_strcmpi(xassetsNames[i], name)) {
				return (XAssetType)i;
			}
		}

		return ASSET_TYPE_COUNT;
	}

	const char* XAssetNameFromId(XAssetType id) {
		if (id >= 0 && id < ASSET_TYPE_COUNT) {
			return xassetsNames[id];
		}

		return "<invalid>";
	}

	BGCacheTypes BGCacheIdFromName(const char* name) {
		for (size_t i = 0; i < ARRAYSIZE(bgcacheNames); i++) {
			if (!_strcmpi(bgcacheNames[i], name)) {
				return (BGCacheTypes)i;
			}
		}

		return BG_CACHE_TYPE_INVALID;
	}

	const char* BGCacheNameFromId(BGCacheTypes id) {
		if (id >= 0 && id < BG_CACHE_TYPE_COUNT) {
			return bgcacheNames[id];
		}

		return *bgcacheNames;
	}

	std::unordered_map<XAssetType, size_t> xoffsets{
		{ ASSET_TYPE_RAGDOLL, 0xBA0 },
		{ ASSET_TYPE_FLAMETABLE, 0x1B0 },
		{ ASSET_TYPE_XANIM, 0x70 },
		{ ASSET_TYPE_IMAGE, 0x20 },
		{ ASSET_TYPE_KLF, 0x18 },
		{ ASSET_TYPE_PLAYLISTS, 0x10 },
		{ ASSET_TYPE_DLOGEVENT, 8 },
		{ ASSET_TYPE_UNLOCKABLE_ITEM, 8 },
		{ ASSET_TYPE_STORECATEGORY, 8 },
		{ ASSET_TYPE_STORAGEFILE, 8 },
		{ ASSET_TYPE_SOUND, 8 },
		{ ASSET_TYPE_SANIM, 8 },
		{ ASSET_TYPE_PLAYLIST_GLOBAL_SETTINGS, 8 },
		{ ASSET_TYPE_PLAYLIST_SCHEDULE, 8 },
		{ ASSET_TYPE_LOCALIZE_ENTRY, 8 },
	};

	size_t GetAssetNameOffset(XAssetType type) {
		auto it{ xoffsets.find(type) };

		if (it != xoffsets.end()) {
			return it->second;
		}
		return 0;
	}

	XHash* GetAssetName(XAssetType type, void* asset, size_t size) {
		size_t offset{ GetAssetNameOffset(type) };

		if (size && offset + 0x10 > size) {
			return nullptr; // bad ref
		}

		return (XHash*)((byte*)asset + offset);
	}
	
}