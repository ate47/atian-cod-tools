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
		const char* teamNames[]{
			"TEAM_FREE",
			"TEAM_ALLIES",
			"TEAM_AXIS",
			"TEAM_THREE",
			"TEAM_FOUR",
			"TEAM_FIVE",
			"TEAM_SIX",
			"TEAM_SEVEN",
			"TEAM_EIGHT",
			"TEAM_NINE",
			"TEAM_TEN",
			"TEAM_ELEVEN",
			"TEAM_TWELVE",
			"TEAM_THIRTEEN",
			"TEAM_FOURTEEN",
			"TEAM_FIFTEEN",
			"TEAM_SIXTEEN",
			"TEAM_SEVENTEEN",
			"TEAM_EIGHTEEN",
			"TEAM_NINETEEN",
			"TEAM_TWENTY",
			"TEAM_TWENTYONE",
			"TEAM_TWENTYTWO",
			"TEAM_TWENTYTHREE",
			"TEAM_TWENTYFOUR",
			"TEAM_TWENTYFIVE",
			"TEAM_TWENTYSIX",
			"TEAM_TWENTYSEVEN",
			"TEAM_TWENTYEIGHT",
			"TEAM_TWENTYNINE",
			"TEAM_THIRTY",
			"TEAM_THIRTYONE",
			"TEAM_THIRTYTWO",
			"TEAM_THIRTYTHREE",
			"TEAM_THIRTYFOUR",
			"TEAM_THIRTYFIVE",
			"TEAM_THIRTYSIX",
			"TEAM_THIRTYSEVEN",
			"TEAM_THIRTYEIGHT",
			"TEAM_THIRTYNINE",
			"TEAM_FOURTY",
			"TEAM_FOURTYONE",
			"TEAM_FOURTYTWO",
			"TEAM_FOURTYTHREE",
			"TEAM_FOURTYFOUR",
			"TEAM_FOURTYFIVE",
			"TEAM_FOURTYSIX",
			"TEAM_FOURTYSEVEN",
			"TEAM_FOURTYEIGHT",
			"TEAM_FOURTYNINE",
			"TEAM_FIFTY",
			"TEAM_FIFTYONE",
			"TEAM_FIFTYTWO",
			"TEAM_FIFTYTHREE",
			"TEAM_FIFTYFOUR",
			"TEAM_FIFTYFIVE",
			"TEAM_FIFTYSIX",
			"TEAM_FIFTYSEVEN",
			"TEAM_FIFTYEIGHT",
			"TEAM_FIFTYNINE",
			"TEAM_SIXTY",
			"TEAM_SIXTYONE",
			"TEAM_SIXTYTWO",
			"TEAM_SIXTYTHREE",
			"TEAM_SIXTYFOUR",
			"TEAM_SIXTYFIVE",
			"TEAM_SIXTYSIX",
			"TEAM_SIXTYSEVEN",
			"TEAM_SIXTYEIGHT",
			"TEAM_SIXTYNINE",
			"TEAM_SEVENTY",
			"TEAM_SEVENTYONE",
			"TEAM_SEVENTYTWO",
			"TEAM_SEVENTYTHREE",
			"TEAM_SEVENTYFOUR",
			"TEAM_SEVENTYFIVE",
			"TEAM_SEVENTYSIX",
			"TEAM_SEVENTYSEVEN",
			"TEAM_SEVENTYEIGHT",
			"TEAM_SEVENTYNINE",
			"TEAM_EIGHTY",
			"TEAM_EIGHTYONE",
			"TEAM_EIGHTYTWO",
			"TEAM_EIGHTYTHREE",
			"TEAM_EIGHTYFOUR",
			"TEAM_EIGHTYFIVE",
			"TEAM_EIGHTYSIX",
			"TEAM_EIGHTYSEVEN",
			"TEAM_EIGHTYEIGHT",
			"TEAM_EIGHTYNINE",
			"TEAM_NINETY",
			"TEAM_NINETYONE",
			"TEAM_NINETYTWO",
			"TEAM_NINETYTHREE",
			"TEAM_NINETYFOUR",
			"TEAM_NINETYFIVE",
			"TEAM_NINETYSIX",
			"TEAM_NINETYSEVEN",
			"TEAM_NINETYEIGHT",
			"TEAM_NINETYNINE",
			"TEAM_ONEHUNDRED",
			"TEAM_ONEHUNDRED_ONE",
			"TEAM_ONEHUNDRED_TWO",
			"TEAM_ONEHUNDRED_THREE",
			"TEAM_ONEHUNDRED_FOUR",
			"TEAM_ONEHUNDRED_FIVE",
			"TEAM_ONEHUNDRED_SIX",
			"TEAM_ONEHUNDRED_SEVEN",
			"TEAM_ONEHUNDRED_EIGHT",
			"TEAM_ONEHUNDRED_NINE",
			"TEAM_ONEHUNDRED_TEN",
			"TEAM_ONEHUNDRED_ELEVEN",
			"TEAM_ONEHUNDRED_TWELVE",
			"TEAM_ONEHUNDRED_THIRTEEN",
			"TEAM_ONEHUNDRED_FOURTEEN",
			"TEAM_ONEHUNDRED_FIFTEEN",
			"TEAM_ONEHUNDRED_SIXTEEN",
			"TEAM_ONEHUNDRED_SEVENTEEN",
			"TEAM_ONEHUNDRED_EIGHTEEN",
			"TEAM_ONEHUNDRED_NINETEEN",
			"TEAM_ONEHUNDRED_TWENTY",
			"TEAM_ONEHUNDRED_TWENTYONE",
			"TEAM_ONEHUNDRED_TWENTYTWO",
			"TEAM_ONEHUNDRED_TWENTYTHREE",
			"TEAM_ONEHUNDRED_TWENTYFOUR",
			"TEAM_NEUTRAL",
			"TEAM_WORLD",
			"TEAM_SPECTATOR",
			"TEAM_INVALID",
		};
		const char* aiWeaponFireTypeNames[]{
			"fullauto",
			"burst",
			"singleshot"
		};
		const char* aiCategoryNames[]{
			"none",
			"civilian",
			"assault",
			"cqb",
			"rpg",
			"sniper",
			"suppressor",
			"riot_shield",
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
		{ ASSET_TYPE_WEAPON_TUNABLES, 0x40 },
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
		{ ASSET_TYPE_ARENASEASONS, 8 },
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

	const char* GetTeamName(team_t team) {
		if (team >= 0 && team < ARRAYSIZE(teamNames)) {
			return teamNames[team];
		}

		return "<invalid>";
	}
	team_t GetTeam(const char* name) {
		for (size_t i = 0; i < ARRAYSIZE(teamNames); i++) {
			if (!_strcmpi(teamNames[i], name)) {
				return (team_t)i;
			}
		}

		return TEAM_INVALID;
	}
	const char* GetAIWeaponFireTypeName(AIWeaponFireType type) {
		if (type >= 0 && type < ARRAYSIZE(aiWeaponFireTypeNames)) {
			return aiWeaponFireTypeNames[type];
		}

		return "<invalid>";
	}
	AIWeaponFireType GetAIWeaponFireType(const char* name) {
		for (size_t i = 0; i < ARRAYSIZE(aiWeaponFireTypeNames); i++) {
			if (!_strcmpi(aiWeaponFireTypeNames[i], name)) {
				return (AIWeaponFireType)i;
			}
		}

		return AI_WEAPON_FIRE_NUM_INVALID;
	}
	const char* GetAICategoryName(AICategory type) {
		if (type >= 0 && type < ARRAYSIZE(aiCategoryNames)) {
			return aiCategoryNames[type];
		}

		return "<invalid>";
	}
	AICategory GetAICategory(const char* name) {
		for (size_t i = 0; i < ARRAYSIZE(aiCategoryNames); i++) {
			if (!_strcmpi(aiCategoryNames[i], name)) {
				return (AICategory)i;
			}
		}

		return AI_CATEGORY_COUNT;
	}
}