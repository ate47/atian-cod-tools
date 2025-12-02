#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo7.hpp>
#include <tools/ff/handlers/bo7/bo7_unlinker_scriptbundle.hpp>
namespace {
	using namespace fastfile::handlers::bo7;
	struct Spray {
		XHash64 name;
		Material* material;
		scriptbundle::ScriptBundleObjectData bundle;
	};
	static_assert(sizeof(Spray) == 0x30);

	class ImplWorker : public Worker {

		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			Spray* asset{ (Spray*)ptr };
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXAsset("material", SATH_ASSET_MATERIAL, asset->material);
			json.WriteFieldNameString("bundle");
			scriptbundle::WriteData(json, asset->bundle);
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "spray"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump spray {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, HandlerHashedAssetType, Worker> impl{ GetWorkers(), HandlerHashedAssetType::SATH_ASSET_SPRAY, sizeof(Spray) };
}