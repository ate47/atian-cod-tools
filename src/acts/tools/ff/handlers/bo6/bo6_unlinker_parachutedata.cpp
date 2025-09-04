#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>
#include <tools/ff/handlers/bo6/bo6_unlinker_scriptbundle.hpp>
namespace {
	using namespace fastfile::handlers::bo6;

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
			BO6JsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXAsset("camo", T10H_ASSET_CAMO, asset->camo);
			json.WriteFieldValueXAsset("viewModel", T10H_ASSET_XMODEL, asset->viewModel);
			json.WriteFieldValueXAsset("worldModel", T10H_ASSET_XMODEL, asset->worldModel);
			json.WriteFieldNameString("bundle");
			scriptbundle::WriteData(json, asset->bundle);
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / "bo6" / "source" / "tables" / "parachutedata"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump parachutedata {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> impl{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_PARACHUTEDATA, sizeof(ParachuteData) };
}