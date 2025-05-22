#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>
#include <tools/ff/handlers/bo4/bo4_unlinker_scriptbundle.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	struct PlaylistMap {
		XHash name;
		XHash base;
		byte mapPlatformMask;
	};
	static_assert(sizeof(PlaylistMap) == 0x28);

	enum PlaylistRuleTypes : uint32_t {
		PLAYLIST_RULE_TYPE_GAMETYPE_SETTINGS = 0x0,
		PLAYLIST_RULE_TYPE_SET_DVAR = 0x1,
		PLAYLIST_RULE_TYPE_EXEC_COMMAND = 0x2,
		PLAYLIST_RULE_TYPE_COUNT = 0x3,
	};

	static const char* PlaylistRuleTypesNames[]{
		"GAMETYPE_SETTINGS",
		"SET_DVAR",
		"EXEC_COMMAND"
	};
	struct PlaylistRule30 {
		uint64_t unk0;
		uint64_t unk8;
		const char* unk10;
	};

	struct PlaylistRule40 {
		XHash unk0;
		uint64_t unk10;
	};

	struct PlaylistRule {
		PlaylistRuleTypes type;
		uint32_t unk4;
		XHash name;
		const char* value;
		int32_t environmentMask;
		byte platformMask;
		int32_t unk30_count;
		PlaylistRule30* unk30;
		uint32_t unk40_count;
		PlaylistRule40* unk40;
		uint32_t utcStartTime;
		uint32_t utcEndTime;
	};
	static_assert(sizeof(PlaylistRule) == 0x50);

	struct PlaylistGameType {
		XHash name;
		XHash description;
		XHash gametype;
		PlaylistRule* rules;
		int32_t numRules;
	};
	static_assert(sizeof(PlaylistGameType) == 0x40);

	struct PlaylistRotation {
		PlaylistMap* map;
		PlaylistGameType* gametype;
		uint32_t weight;
		bool isFree;
	};
	static_assert(sizeof(PlaylistRotation) == 0x18);
	struct PlaylistEntryUnkE0 {
		int32_t unk0;
		int32_t unk4;
		uint32_t unk8;
		const char* name;
	};
	static_assert(sizeof(PlaylistEntryUnkE0) == 0x18);

	struct PlaylistEntry {
		XHash name;
		eModes mainMode;
		XHash description;
		XHash unique_name;
		GfxImage* image;
		GfxImage* imageBackground;
		GfxImage* imageBackgroundFocus;
		GfxImage* imageTileLarge;
		GfxImage* imageTileSmall;
		GfxImage* imageTileSideInfo;
		int32_t minPartySize;
		int32_t maxPartySize;
		int32_t maxLocalPlayers;
		int32_t maxPlayers;
		int32_t minPlayers;
		int32_t minPlayersToCreate;
		uint32_t searchType;
		uint32_t minUserTier;
		uint32_t maxUserTier;
		uint32_t parkingPlaylist;
		bool isCustomMatch;
		uint32_t unlockXp;
		uint32_t unlockPLevel;
		int32_t environmentMask;
		byte platformMask;
		const char* dlcName;
		bool hideifmissingdlc;
		bool disableGuests;
		bool excludePublicLobby;
		bool customMutation;
		bool unkb4;
		bool isSpectreRising;
		bool isQuickplayCard;
		int32_t arenaSlot;
		int32_t unkbc;
		XHash unkc0;
		XHash visibleProfileVar;
		PlaylistEntryUnkE0* unke0;
		int32_t unke0_count;
		PlaylistRule* rules;
		int32_t rules_count;
		uint32_t unkfc;
		PlaylistRotation* rotations;
		int32_t rotations_count;
		int32_t id;
		bool isNewGameOrResumeGame;
		byte unk111;
		uint16_t unk112;
		uint32_t unk114;
		scriptbundle::SB_ObjectsArray bundle;
	};
	static_assert(sizeof(PlaylistEntry) == 0x138);

	struct PlaylistGroup {
		XHash name;
		eModes lobbyMainMode;
		XHash description;
		XHash icon;
		uint32_t environmentMask;
		bool hidden;
		PlaylistEntry* entries;
		int32_t entries_count;
	};
	static_assert(sizeof(PlaylistGroup) == 0x50);


	struct __declspec(align(8)) Playlists {
		uint32_t unk0;
		uint32_t unk4;
		uint64_t unk8;
		XHash name;
		PlaylistMap** maps;
		int32_t maps_count;
		PlaylistGameType** gametypes;
		int32_t gametypes_count;
		PlaylistGroup* categories;
		int32_t categories_count;
	};
	static_assert(sizeof(Playlists) == 0x50);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			Playlists* asset{ (Playlists*)ptr };

			std::filesystem::path outDir{ opt.m_output / "bo4" / "source" / "tables" / "playlists" / hashutils::ExtractTmp("file", asset->name) };
			std::filesystem::create_directories(outDir);
			LOG_INFO("Dump playlists {}", outDir.string());
			{

				BO4JsonWriter json{};

				std::filesystem::path outFile{ outDir / "playlists.json" };
				json.BeginObject();
				json.WriteFieldValueXHash("name", asset->name);
				json.WriteFieldValueNumber("unk0", asset->unk0);
				json.WriteFieldValueNumber("unk4", asset->unk4);
				json.WriteFieldValueNumber("unk8", asset->unk8);
				json.EndObject();


				if (!json.WriteToFile(outFile)) {
					LOG_ERROR("Error when dumping {}", outFile.string());
				}
			}

			{
				BO4JsonWriter json{};

				std::filesystem::path outFile{ outDir / "maps.json" };

				json.BeginArray();
				for (size_t i = 0; i < asset->maps_count; i++) {
					PlaylistMap* map{ asset->maps[i] };
					if (!map) {
						json.WriteValueNull();
						continue;
					}
					json.BeginObject();
					json.WriteFieldValueXHash("name", map->name);
					json.WriteFieldValueXHash("base", map->base);
					json.WriteFieldValueNumber("mapPlatformMask", (int)map->mapPlatformMask);

					json.EndObject();
				}
				json.EndArray();

				if (!json.WriteToFile(outFile)) {
					LOG_ERROR("Error when dumping {}", outFile.string());
				}
			}
			auto WritePlayListRule = [](BO4JsonWriter& json, PlaylistRule* rule) {
				json.BeginObject();
				const char* type;
				if (rule->type < ARRAYSIZE(PlaylistRuleTypesNames)) {
					type = PlaylistRuleTypesNames[rule->type];
				}
				else {
					type = utils::va("unk:%d", rule->type);
				}
				json.WriteFieldValueString("type", type);
				json.WriteFieldValueXHash("name", rule->name);
				if (rule->value) json.WriteFieldValueString("value", rule->value);
				json.WriteFieldValueNumber("platformMask", (int)rule->platformMask);
				json.WriteFieldValueNumber("environmentMask", rule->environmentMask);
				json.WriteFieldValueNumber("utcStartTime", rule->utcStartTime);
				json.WriteFieldValueNumber("utcEndTime", rule->utcEndTime);


				if (rule->unk30_count) {
					json.WriteFieldNameString("unk30");
					json.BeginArray();
					for (size_t k = 0; k < rule->unk30_count; k++) {
						PlaylistRule30* v{ rule->unk30 + k };
						json.BeginObject();
						json.WriteFieldValueNumber("unk0", v->unk0);
						json.WriteFieldValueNumber("unk8", v->unk8);
						if (v->unk10) json.WriteFieldValueString("unk10", v->unk10);
						json.EndObject();
					}
					json.EndArray();
				}

				if (rule->unk40) {
					json.WriteFieldNameString("unk40");
					json.BeginArray();
					for (size_t k = 0; k < rule->unk40_count; k++) {
						PlaylistRule40* v{ rule->unk40 + k };
						json.BeginObject();
						json.WriteFieldValueXHash("unk0", v->unk0);
						json.WriteFieldValueNumber("unk10", v->unk10);
						json.EndObject();
					}
					json.EndArray();
				}

				json.EndObject();
			};

			{
				BO4JsonWriter json{};

				std::filesystem::path outFile{ outDir / "gametypes.json" };
				json.BeginArray();
				for (size_t i = 0; i < asset->gametypes_count; i++) {
					PlaylistGameType* gametype{ asset->gametypes[i] };
					if (!gametype) {
						json.WriteValueNull();
						continue;
					}

					json.BeginObject();
					json.WriteFieldValueXHash("name", gametype->name);
					json.WriteFieldValueXHash("description", gametype->description);
					json.WriteFieldValueXHash("gametype", gametype->gametype);

					json.WriteFieldNameString("rules");
					json.BeginArray();
					for (size_t j = 0; j < gametype->numRules; j++) {
						WritePlayListRule(json, gametype->rules + j);
					}
					json.EndArray();
					json.EndObject();
				}
				json.EndArray();

				if (!json.WriteToFile(outFile)) {
					LOG_ERROR("Error when dumping {}", outFile.string());
				}
			}

			for (size_t i = 0; i < asset->categories_count; i++) {
				PlaylistGroup* group{ asset->categories + i };

				BO4JsonWriter json{};

				std::filesystem::path outFile{ outDir / "categories" / std::format("{}.json", hashutils::ExtractTmp("file", group->name)) };
				std::filesystem::create_directories(outFile.parent_path());
				json.BeginObject();
				json.WriteFieldValueXHash("name", group->name);
				json.WriteFieldValueXHash("description", group->description);
				json.WriteFieldValueXHash("icon", group->icon);
				json.WriteFieldValueString("lobbyMainMode", GetEModeName(group->lobbyMainMode));

				json.WriteFieldValueBool("hidden", group->hidden);
				json.WriteFieldValueNumber("environmentMask", group->environmentMask);
				
				json.WriteFieldNameString("entries");
				json.BeginArray();
				for (size_t j = 0; j < group->entries_count; j++) {
					PlaylistEntry* entry{ group->entries + j };
					json.BeginObject();

					json.WriteFieldValueXHash("name", entry->name);
					json.WriteFieldValueXHash("description", entry->description);
					json.WriteFieldValueXHash("unique_name", entry->unique_name);
					json.WriteFieldValueXHash("visibleProfileVar", entry->visibleProfileVar);

					if (entry->dlcName) json.WriteFieldValueString("dlcName", entry->dlcName);
					json.WriteFieldValueString("lobbyMainMode", GetEModeName(entry->mainMode));

					json.WriteFieldValueXAsset("image", games::bo4::pool::ASSET_TYPE_IMAGE, entry->image);
					json.WriteFieldValueXAsset("imageBackground", games::bo4::pool::ASSET_TYPE_IMAGE, entry->imageBackground);
					json.WriteFieldValueXAsset("imageBackgroundFocus", games::bo4::pool::ASSET_TYPE_IMAGE, entry->imageBackgroundFocus);
					json.WriteFieldValueXAsset("imageTileLarge", games::bo4::pool::ASSET_TYPE_IMAGE, entry->imageTileLarge);
					json.WriteFieldValueXAsset("imageTileSmall", games::bo4::pool::ASSET_TYPE_IMAGE, entry->imageTileSmall);
					json.WriteFieldValueXAsset("imageTileSideInfo", games::bo4::pool::ASSET_TYPE_IMAGE, entry->imageTileSideInfo);

					json.WriteFieldValueNumber("id", entry->id);
					json.WriteFieldValueNumber("arenaSlot", entry->arenaSlot);
					json.WriteFieldValueNumber("minPartySize", entry->minPartySize);
					json.WriteFieldValueNumber("maxPartySize", entry->maxPartySize);
					json.WriteFieldValueNumber("maxLocalPlayers", entry->maxLocalPlayers);
					json.WriteFieldValueNumber("maxPlayers", entry->maxPlayers);
					json.WriteFieldValueNumber("minPlayers", entry->minPlayers);
					json.WriteFieldValueNumber("minPlayersToCreate", entry->minPlayersToCreate);
					json.WriteFieldValueNumber("searchType", entry->searchType);
					json.WriteFieldValueNumber("minUserTier", entry->minUserTier);
					json.WriteFieldValueNumber("maxUserTier", entry->maxUserTier);
					json.WriteFieldValueNumber("parkingPlaylist", entry->parkingPlaylist);
					json.WriteFieldValueNumber("unlockXp", entry->unlockXp);
					json.WriteFieldValueNumber("unlockPLevel", entry->unlockPLevel);
					json.WriteFieldValueNumber("environmentMask", entry->environmentMask);
					json.WriteFieldValueNumber("platformMask", entry->platformMask);
					
					json.WriteFieldValueBool("isCustomMatch", entry->isCustomMatch);
					json.WriteFieldValueBool("hideifmissingdlc", entry->hideifmissingdlc);
					json.WriteFieldValueBool("disableGuests", entry->disableGuests);
					json.WriteFieldValueBool("excludePublicLobby", entry->excludePublicLobby);
					json.WriteFieldValueBool("customMutation", entry->customMutation);
					json.WriteFieldValueBool("isSpectreRising", entry->isSpectreRising);
					json.WriteFieldValueBool("isQuickplayCard", entry->isQuickplayCard);
					json.WriteFieldValueBool("isNewGameOrResumeGame", entry->isNewGameOrResumeGame);

					// unk
					if (entry->unk111) json.WriteFieldValueNumber("unk111", (int)entry->unk111);
					if (entry->unk112) json.WriteFieldValueNumber("unk112", entry->unk112);
					if (entry->unk114) json.WriteFieldValueNumber("unk114", entry->unk114);
					// unused?
					if (entry->unkbc) json.WriteFieldValueNumber("unkbc", entry->unkbc);
					if (entry->unkfc) json.WriteFieldValueNumber("unkfc", entry->unkfc);
					if (entry->unkc0) json.WriteFieldValueXHash("unkc0", entry->unkc0);
					if (entry->unkb4) json.WriteFieldValueBool("unkb4", entry->unkb4);


					/*

				struct PlaylistEntry {
					PlaylistRotation* rotations;
					int32_t rotations_count;
				};
				static_assert(sizeof(PlaylistEntry) == 0x138);
	*/
					// tbh I don't know
					if (entry->unke0_count) {
						json.WriteFieldNameString("unke0");
						json.BeginArray();
						for (size_t k = 0; k < entry->unke0_count; k++) {
							PlaylistEntryUnkE0* unke0{ entry->unke0 + k };
							json.BeginObject();

							if (unke0->name) json.WriteFieldValueString("name", unke0->name);

							if (unke0->unk0) json.WriteFieldValueNumber("unk0", unke0->unk0);
							if (unke0->unk4) json.WriteFieldValueNumber("unk4", unke0->unk4);
							if (unke0->unk8) json.WriteFieldValueNumber("unk8", unke0->unk8);

							json.EndObject();
						}
						json.EndArray();
					}

					json.WriteFieldNameString("rules");
					json.BeginArray();
					for (size_t k = 0; k < entry->rules_count; k++) {
						WritePlayListRule(json, entry->rules + k);
					}
					json.EndArray();

					json.WriteFieldNameString("rotations");
					json.BeginArray();
					for (size_t k = 0; k < entry->rotations_count; k++) {
						PlaylistRotation* rotation{ entry->rotations + k };
						json.BeginObject();

						if (rotation->map) json.WriteFieldValueXHash("map", rotation->map->name);
						if (rotation->gametype) json.WriteFieldValueXHash("gametype", rotation->gametype->name);
						json.WriteFieldValueNumber("weight", rotation->weight);
						json.WriteFieldValueBool("isFree", rotation->isFree);

						json.EndObject();
					}
					json.EndArray();


					json.WriteFieldNameString("bundle");
					bool err{};
					scriptbundle::WriteObject(json, entry->bundle, err);
					if (err) {
						LOG_ERROR("Error when dumping bundle");
						return;
					}

					json.EndObject();

				}
				json.EndArray();

				json.EndObject();

				if (!json.WriteToFile(outFile)) {
					LOG_ERROR("Error when dumping {}", outFile.string());
				}
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_PLAYLISTS, true };
}