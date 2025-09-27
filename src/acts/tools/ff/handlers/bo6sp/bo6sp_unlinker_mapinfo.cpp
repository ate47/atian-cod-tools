#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6_sp.hpp>
#include <tools/ff/handlers/bo6sp/bo6sp_unlinker_scriptbundle.hpp>

namespace {
	using namespace fastfile::handlers::bo6sp;
	
	struct MapSpawn {
		uint64_t unk0;
		PlayerSpawnSettings* settings;
	};
	static_assert(sizeof(MapSpawn) == 0x10);

	struct MapInfo {
		XHash64 name;
		const char* nameStr;
		const char* gamemodes;
		const char* scriptsPrefix;
		const char* loadingImage;
		GfxImage* previewImage;
		const char* unk30;
		const char* unk38;
		float unk40[3];
		float unk4c[3];
		float unk_34e5be4982c1723;
		uint32_t unk5c;
		uint64_t unk60;
		uint64_t unk68;
		uint64_t unk70;
		uint64_t spawnsCount;
		MapSpawn* spawns;
		scriptbundle::ScriptBundleObjectData bundle;
	};
	static_assert(sizeof(MapInfo) == 0xa8);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			MapInfo* asset{ (MapInfo*)ptr };
			BO6JsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			
			json.WriteFieldValueXString("nameStr", asset->nameStr);
			json.WriteFieldValueXString("gamemodes", asset->gamemodes);
			json.WriteFieldValueXString("scriptsPrefix", asset->scriptsPrefix);
			json.WriteFieldValueXString("loadingImage", asset->loadingImage);
			json.WriteFieldValueXAsset("previewImage", bo6::T10HashAssetType::T10H_ASSET_IMAGE, asset->previewImage);
			json.WriteFieldValueXString("unk30", asset->unk30);
			json.WriteFieldValueXString("unk38", asset->unk38);

			json.WriteFieldValueString("unk40", std::format("{} {} {}", asset->unk40[0], asset->unk40[1], asset->unk40[2]));
			json.WriteFieldValueString("unk4c", std::format("{} {} {}", asset->unk4c[0], asset->unk4c[1], asset->unk4c[2]));
			json.WriteFieldValueNumber(0x34e5be4982c1723, asset->unk_34e5be4982c1723);

			if (opt.testDump) {
				json.WriteFieldValueUnknown("unk60", asset->unk60);
				json.WriteFieldValueUnknown("unk68", asset->unk68);
				json.WriteFieldValueUnknown("unk70", asset->unk70);
			}

			if (asset->spawns) {
				json.WriteFieldNameString("spawns");
				json.BeginArray();
				for (size_t i = 0; i < asset->spawnsCount; i++) {
					json.BeginObject();
					json.WriteFieldValueUnknown("unk0", asset->spawns[i].unk0);
					json.WriteFieldValueXAsset("settings", bo6::T10HashAssetType::T10H_ASSET_PLAYERSPAWNSETTINGS, asset->spawns[i].settings);
					json.EndObject();
				}
				json.EndArray();
			}
			json.WriteFieldNameString("bundle");
			scriptbundle::WriteData(json, asset->bundle);
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / "bo6sp" / "source" / "tables" / "mapinfo"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump mapinfo {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> impl{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_MAPINFO, sizeof(MapInfo) };
}