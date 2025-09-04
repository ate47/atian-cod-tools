#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>
namespace {
	using namespace fastfile::handlers::bo6;

	struct ObjectStoreDDLConfig;
	struct StorageFileUnk {
		uint64_t unk0;
		const char* unk8;
		const char* unk10;
	};
	static_assert(sizeof(StorageFileUnk) == 0x18);

	struct StorageFile {
		XHash64 name;
		const char* nameStr;
		const char* config;
		const char* configPrestige;
		const char* ddlName;
		const char* category;
		uint32_t unk30;
		int32_t objectStoreDDLConfigsCount;
		ObjectStoreDDLConfig** objectStoreDDLConfigs;
		int32_t unk48_count;
		StorageFileUnk** unk48;
		int32_t unk58_count;
		StorageFileUnk** unk58;
		int32_t operatorsCount;
		XHash64* operators;
		uint64_t unk70;
		uint64_t unk78;
		uint64_t unk80;
		uint64_t unk88;
		const char* env;
		uint64_t unk98;
	};

	static_assert(sizeof(StorageFile) == 0xA0);

	class ImplWorker : public Worker {

		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			StorageFile* asset{ (StorageFile*)ptr };
			BO6JsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXString("nameStr", asset->nameStr);
			json.WriteFieldValueXString("config", asset->config);
			json.WriteFieldValueXString("configPrestige", asset->configPrestige);
			json.WriteFieldValueXString("ddlName", asset->ddlName);
			json.WriteFieldValueXString("category", asset->category);
			json.WriteFieldValueXString("env", asset->env);
			json.WriteFieldValueXAssetArray("objectStoreDDLConfigs", bo6::T10H_ASSET_OBJECTSTOREDDLCONFIG, asset->objectStoreDDLConfigsCount, asset->objectStoreDDLConfigs);

			if (asset->operators) {
				json.WriteFieldNameString("operators");
				json.BeginArray();
				for (size_t i = 0; i < asset->operatorsCount; i++) {
					json.WriteValueHash(asset->operators[i]);
				}
				json.EndArray();
			}

			auto WriteStorageFileInfo = [&json](const char* name, StorageFileUnk** array, size_t count) {
				if (!array) return;
				json.WriteFieldNameString(name);
				json.BeginArray();
				for (size_t i = 0; i < count; i++) {
					if (!array[i]) {
						json.WriteValueNull();
						continue;
					}
					json.BeginObject();

					json.WriteFieldValueXHash("unk0", array[i]->unk0);
					json.WriteFieldValueXString("unk8", array[i]->unk8);
					json.WriteFieldValueXString("unk10", array[i]->unk10);
					json.EndObject();

				}
				json.EndArray();
			};
			WriteStorageFileInfo("unk48", asset->unk48, asset->unk48_count);
			WriteStorageFileInfo("unk58", asset->unk58, asset->unk58_count);

			if (opt.testDump) {
				json.WriteFieldValueNumber("unk30", asset->unk30);
				json.WriteFieldValueUnknown("unk70", asset->unk70);
				json.WriteFieldValueUnknown("unk78", asset->unk78);
				json.WriteFieldValueUnknown("unk80", asset->unk80);
				json.WriteFieldValueUnknown("unk88", asset->unk88);
				json.WriteFieldValueUnknown("unk98", asset->unk98);
			}
			
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / "bo6" / "source" / "tables" / "storagefile"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump storagefile {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> impl{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_STORAGEFILE, sizeof(StorageFile) };
}