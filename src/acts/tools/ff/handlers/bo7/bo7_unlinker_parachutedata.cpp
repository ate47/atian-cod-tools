#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>
#include <tools/ff/handlers/bo7/bo7_unlinker_scriptbundle.hpp>
namespace {
	using namespace fastfile::handlers::bo7;

	struct ParachuteData {
		XHash64 name;
		Camo* camo;
		XModel* viewModel;
		XModel* worldModel;
		scriptbundle::ScriptBundleObjectData bundle;
	};
	static_assert(sizeof(ParachuteData) == 0x40);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			ParachuteData* asset{ (ParachuteData*)ptr };
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXAsset("camo", SATH_ASSET_CAMO, asset->camo);
			json.WriteFieldValueXAsset("viewModel", SATH_ASSET_XMODEL, asset->viewModel);
			json.WriteFieldValueXAsset("worldModel", SATH_ASSET_XMODEL, asset->worldModel);
			json.WriteFieldNameString("bundle");
			scriptbundle::WriteData(json, asset->bundle);
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "parachutedata"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump parachutedata {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, HandlerHashedAssetType, Worker> impl{ GetWorkers(), HandlerHashedAssetType::SATH_ASSET_PARACHUTEDATA, sizeof(ParachuteData) };
}