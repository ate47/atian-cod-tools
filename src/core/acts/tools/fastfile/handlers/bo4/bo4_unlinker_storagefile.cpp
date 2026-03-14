#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/utils/data_utils.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	using namespace games::bo4::pool;
	struct RawFile;

	enum StorageFileType : int32_t {
		STORAGE_FILE_INVALID = -1,
	};
	enum StorageTargetType : int32_t
	{
		STORAGE_TARGET_TYPE_GAMERPROFILE = 0x0,
		STORAGE_TARGET_TYPE_DWUSERSTORAGE = 0x1,
		STORAGE_TARGET_TYPE_MEMORY = 0x2,
		STORAGE_TARGET_TYPE_FASTFILE = 0x3,
		STORAGE_TARGET_TYPE_COUNT = 0x4,
		STORAGE_TARGET_TYPE_FIRST = 0x0,
		STORAGE_TARGET_TYPE_INVALID = 0x4,
	}; enum CampaignMode : __int32
	{
		CAMPAIGNMODE_DEFAULT = 0x0,
		CAMPAIGNMODE_ZOMBIES = 0x1,
		CAMPAIGNMODE_COUNT = 0x2,
		CAMPAIGNMODE_INVALID = 0x3,
	}; enum eNetworkModes : __int32
	{
		MODE_NETWORK_OFFLINE = 0x0,
		MODE_NETWORK_SYSTEMLINK = 0x1,
		MODE_NETWORK_ONLINE = 0x2,
		MODE_NETWORK_COUNT = 0x3,
		MODE_NETWORK_INVALID = 0x3,
	}; enum eGameModes : __int32
	{
		MODE_GAME_MATCHMAKING_PLAYLIST = 0x0,
		MODE_GAME_MATCHMAKING_MANUAL = 0x1,
		MODE_GAME_DEFAULT = 0x2,
		MODE_GAME_LEAGUE = 0x3,
		MODE_GAME_FREERUN = 0x4,
		MODE_GAME_THEATER = 0x5,
		MODE_GAME_COUNT = 0x6,
		MODE_GAME_INVALID = 0x6,
	};

	struct StorageFileInfo {
		const char* nameStr;
		XHash name;
		XHash ddlPath;
		const char** fileNames;
		RawFile** files;
		int32_t size;
		int32_t slots;
		StorageFileType fileType;
		StorageTargetType targetType;
		eModes sessionMode;
		CampaignMode campaignMode;
		eNetworkModes networkMode;
		eGameModes gameMode;
		bool readOnLogin;
		bool readOnly;
		bool sendToServer;
		bool useScratch;
		bool optional;
	};
	static_assert(sizeof(StorageFileInfo) == 0x60);

	struct StorageFileList {
		XHash name;
		uint32_t numStorageFiles;
		StorageFileInfo** storageFiles;
	};
	static_assert(sizeof(StorageFileList) == 0x20);


	class ImplWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			StorageFileInfo& asset{ *(StorageFileInfo*)ptr };

			BO4JsonWriter json{};

			json.BeginObject();

			if (asset.nameStr) {
				const char* h{ utils::va("gamedata/ddl/generated/%s.ddl", asset.nameStr) };
				hashutils::AddPrecomputed(hash::Hash64(h), h);
				hashutils::AddPrecomputed(hash::Hash64(asset.nameStr), asset.nameStr);

				json.WriteFieldValueString("nameStr", asset.nameStr);
			}

			json.WriteFieldValueXHash("name", asset.name);
			json.WriteFieldValueXHash("ddlPath", asset.ddlPath);

			json.WriteFieldNameString("files");
			json.BeginArray();
			for (size_t i = 0; i < asset.slots; i++) {
				json.BeginObject();
				if (asset.fileNames && asset.fileNames[i]) {
					hashutils::AddPrecomputed(hash::Hash64(asset.fileNames[i]), asset.fileNames[i]);
					json.WriteFieldValueString("fileName", asset.fileNames[i]);
				}
				if (asset.files) {
					json.WriteFieldValueXAsset("assetName", games::bo4::pool::ASSET_TYPE_RAWFILE, asset.files[i]);
				}
				json.EndObject();
			}
			json.EndArray();

			json.WriteFieldValueNumber("size", asset.size);
			json.WriteFieldValueBool("readOnLogin", asset.readOnLogin);
			json.WriteFieldValueBool("readOnly", asset.readOnly);
			json.WriteFieldValueBool("sendToServer", asset.sendToServer);
			json.WriteFieldValueBool("useScratch", asset.useScratch);
			json.WriteFieldValueBool("optional", asset.optional);

			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / "bo4" / "source" / "tables" / "storagefile"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset.name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump storagefile {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping storagefile {}", outFile.string());
			}
		}
	};

	class ImplListWorker : public Worker {

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			StorageFileList* asset{ (StorageFileList*)ptr };
			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "storagefile" / "list"
				/ std::format("{}.csv", hashutils::ExtractTmp("file", asset->name)) };

			std::filesystem::create_directories(outFile.parent_path());

			utils::OutFileCE os{ outFile };
			if (!os) {
				LOG_ERROR("Error when dumping storagefilelist {}", outFile.string());
				return;
			}
			LOG_OPT_INFO("Dump storagefilelist {}", outFile.string());

			os << "name";

			for (size_t i = 0; i < asset->numStorageFiles; i++) {
				XHash* h{ games::bo4::pool::GetAssetName(ASSET_TYPE_STORAGEFILE, asset->storageFiles[i]) };
				os << "\n";
				if (h) {
					os << "#" << hashutils::ExtractTmp("hash", *h);
				}
				else {
					os << "null";
				}
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_STORAGEFILE };
	utils::MapAdder<ImplListWorker, games::bo4::pool::XAssetType, Worker> impll{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_STORAGEFILELIST };
}