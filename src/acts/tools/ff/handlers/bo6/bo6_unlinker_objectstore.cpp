#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>
namespace {
	using namespace fastfile::handlers::bo6;

	struct StorageFile;
	struct ObjectStoreDDLConfigGroup {
		XHash64 name;
		XHash64 unk8;
	};
	static_assert(sizeof(ObjectStoreDDLConfigGroup) == 0x10);
	struct ObjectStoreDDLConfig {
		XHash64 name;
		int32_t storageFilesCount;
		StorageFile** storageFiles;
	};
	static_assert(sizeof(ObjectStoreDDLConfig) == 0x18);

	struct ObjectStoreProject {
		XHash64 name;
		int32_t storageFilesCount;
		StorageFile** storageFiles;
	};

	static_assert(sizeof(ObjectStoreProject) == 0x18);

	struct ObjectStoreGamemodeUnk11 {
		StorageFile* file;
		uint64_t unk8;
	};
	static_assert(sizeof(ObjectStoreGamemodeUnk11) == 0x10);

	struct ObjectStoreGamemodeUnk1 {
		ObjectStoreGamemodeUnk11* files[10];
	};
	static_assert(sizeof(ObjectStoreGamemodeUnk1) == 0x50);

	struct ObjectStoreGamemodeUnk2 {
		uint64_t name;
		uint64_t unk8;
		StorageFile* file;
	};
	static_assert(sizeof(ObjectStoreGamemodeUnk2) == 0x18);

	struct ObjectStoreGamemode {
		XHash64 name;
		ObjectStoreGamemodeUnk1* unk8;
		int32_t unk18_count;
		ObjectStoreGamemodeUnk2** unk18;
	};
	static_assert(sizeof(ObjectStoreGamemode) == 0x20);

	class ImplWorker : public Worker {

		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			ObjectStoreProject* asset{ (ObjectStoreProject*)ptr };
			BO6JsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXAssetArray("storageFiles", bo6::T10H_ASSET_STORAGEFILE, asset->storageFilesCount, asset->storageFiles);
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "objectstore" / "project"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump objectstore project {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};
	class ImplWorkerDDLCfg : public Worker {

		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			ObjectStoreDDLConfig* asset{ (ObjectStoreDDLConfig*)ptr };
			BO6JsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXAssetArray("storageFiles", bo6::T10H_ASSET_STORAGEFILE, asset->storageFilesCount, asset->storageFiles);
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "objectstore" / "ddlconfig"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump objectstore ddlconfig {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};
	class ImplWorkerDDLCfgGrp : public Worker {

		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			ObjectStoreDDLConfigGroup* asset{ (ObjectStoreDDLConfigGroup*)ptr };
			BO6JsonWriter json{};

			json.BeginObject();
			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXHash("unk8", asset->unk8);
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "objectstore" / "ddlconfig" / "group"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump objectstore ddlconfig group {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};
	
	class ImplWorkerGM : public Worker {

		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			ObjectStoreGamemode* asset{ (ObjectStoreGamemode*)ptr };
			BO6JsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			if (asset->unk8) {
				json.WriteFieldNameString("unk8");
				json.BeginArray();
				for (ObjectStoreGamemodeUnk11* ff : asset->unk8->files) {
					if (!ff) break;

					json.BeginObject();
					json.WriteFieldValueXAsset("storageFile", bo6::T10H_ASSET_STORAGEFILE, ff->file);
					json.WriteFieldValueXHash("unk8", ff->unk8);
					json.EndObject();
				}
				json.EndArray();
			}
			if (asset->unk18) {
				json.WriteFieldNameString("unk18");
				json.BeginArray();
				for (size_t i = 0; i < asset->unk18_count; i++) {
					if (!asset->unk18[i]) {
						json.WriteValueNull();
						continue;
					}
					json.BeginObject();
					json.WriteFieldValueXAsset("storageFile", bo6::T10H_ASSET_STORAGEFILE, asset->unk18[i]->file);
					json.WriteFieldValueXHash("name", asset->unk18[i]->name);
					json.WriteFieldValueNumber("unk8", asset->unk18[i]->unk8);
					json.EndObject();
				}
				json.EndArray();
			}
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "objectstore" / "gamemode"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump objectstore project {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> implp{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_OBJECTSTOREPROJECT, sizeof(ObjectStoreProject) };
	utils::MapAdder<ImplWorkerDDLCfg, bo6::T10HashAssetType, Worker> impldc{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_OBJECTSTOREDDLCONFIG, sizeof(ObjectStoreDDLConfig) };
	utils::MapAdder<ImplWorkerDDLCfgGrp, bo6::T10HashAssetType, Worker> impldcg{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_OBJECTSTOREDDLCONFIGGROUP, sizeof(ObjectStoreDDLConfigGroup) };
	utils::MapAdder<ImplWorkerGM, bo6::T10HashAssetType, Worker> implg{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_OBJECTSTOREGAMEMODE, sizeof(ObjectStoreGamemode) };
}