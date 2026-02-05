#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo6.hpp>
#include <tools/fastfile/handlers/bo7/bo7_unlinker_scriptbundle.hpp>
namespace {
	using namespace fastfile::handlers::bo7;

	struct UnlockFeedConfigTimeInfo {
		uint64_t seconds;
		XHash64 description;
	};

	struct UnlockFeedConfigFilter {
		XHash64 name;
		bool other;
		XHash64 values[24];
	};
	static_assert(sizeof(UnlockFeedConfigFilter) == 0xd0);

	struct UnlockFeedConfig {
		XHash64 name;
		scriptbundle::ScriptBundleObjectData bundle;
		uint64_t timeInfoCount;
		UnlockFeedConfigTimeInfo* timeInfo;
		uint64_t projectsCount;
		const char** projects;
		uint64_t zombiesLootIdsCount;
		uint32_t* zombiesLootIds;
		uint64_t bundlesCount;
		XHash64* bundles;
		uint64_t lootIdsCount;
		uint32_t* lootIds;
		uint64_t filtersCount;
		UnlockFeedConfigFilter* filters;
		uint64_t typesCount;
		XHash64* types;
		uint64_t unlockFeedItemsCount;
		XHash64* unlockFeedItems;
		XHash64 __pad[193];
	};


	static_assert(sizeof(UnlockFeedConfig) == 0x6b0);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			UnlockFeedConfig* asset{ (UnlockFeedConfig*)ptr };
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);

			if (asset->projects) {
				json.WriteFieldNameString("projects");
				json.BeginArray();
				for (size_t i = 0; i < asset->projectsCount; i++) {
					if (asset->projects[i]) {
						json.WriteValueString(asset->projects[i]);
					}
					else {
						json.WriteValueNull();
					}
				}
				json.EndArray();
			}

			if (asset->timeInfo) {
				json.WriteFieldNameString("timeInfo");
				json.BeginArray();
				for (size_t i = 0; i < asset->timeInfoCount; i++) {
					json.BeginObject();
					json.WriteFieldValueNumber("seconds", asset->timeInfo[i].seconds);
					json.WriteFieldValueLocalized("description", asset->timeInfo[i].description);
					json.EndObject();
				}
				json.EndArray();
			}

			if (asset->bundles) {
				json.WriteFieldNameString("bundles");
				json.BeginArray();
				for (size_t i = 0; i < asset->bundlesCount; i++) {
					json.WriteValueHash(asset->bundles[i]);
				}
				json.EndArray();
			}

			if (asset->types) {
				json.WriteFieldNameString("types");
				json.BeginArray();
				for (size_t i = 0; i < asset->typesCount; i++) {
					json.WriteValueHash(asset->types[i]);
				}
				json.EndArray();
			}

			if (asset->unlockFeedItems) {
				json.WriteFieldNameString("unlockFeedItems");
				json.BeginArray();
				for (size_t i = 0; i < asset->unlockFeedItemsCount; i++) {
					json.WriteValueHash(asset->unlockFeedItems[i]);
				}
				json.EndArray();
			}


			if (asset->zombiesLootIds) {
				json.WriteFieldNameString("zombiesLootIds");
				json.BeginArray();
				for (size_t i = 0; i < asset->zombiesLootIdsCount; i++) {
					json.WriteValueNumber(asset->zombiesLootIds[i]);
				}
				json.EndArray();
			}


			if (asset->lootIds) {
				json.WriteFieldNameString("lootIds");
				json.BeginArray();
				for (size_t i = 0; i < asset->lootIdsCount; i++) {
					json.WriteValueNumber(asset->lootIds[i]);
				}
				json.EndArray();
			}

			if (asset->filters) {
				json.WriteFieldNameString("filters");
				json.BeginArray();
				for (size_t i = 0; i < asset->filtersCount; i++) {
					UnlockFeedConfigFilter* data{ &asset->filters[i] };

					json.BeginObject();
					json.WriteFieldValueXHash("name", data->name);
					json.WriteFieldValueBool("other", data->other);
					for (size_t i = 0; i < ARRAYSIZE(data->values); i++) {
						json.WriteFieldValueXHash(utils::va("value%llu", i), data->values[i]);
					}
					json.EndObject();
				}
				json.EndArray();
			}

			json.WriteFieldNameString("bundle");
			scriptbundle::WriteData(json, asset->bundle);

			if (opt.testDump) {

				for (size_t i = 0; i < ARRAYSIZE(asset->__pad); i++) {
					json.WriteFieldValueUnknown(utils::va("unk%x", offsetof(UnlockFeedConfig, __pad) + 8 * i), asset->__pad[i]);
				}
			}
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "unlockfeedconfig"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump timeline {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, HandlerHashedAssetType, Worker> impl{ GetWorkers(), HandlerHashedAssetType::SATH_ASSET_UNLOCKFEEDCONFIG, sizeof(UnlockFeedConfig) };
}