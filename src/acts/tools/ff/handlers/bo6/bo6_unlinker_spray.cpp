#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>
#include <tools/ff/handlers/bo6/bo6_unlinker_scriptbundle.hpp>
namespace {
	using namespace fastfile::handlers::bo6;

	class ImplWorker : public Worker {

		using Worker::Worker;
		struct Spray {
			XHash64 name;
			Material* material;
			scriptbundle::ScriptBundleObjectData bundle;
		};
		static_assert(sizeof(Spray) == 0x30);

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			Spray* asset{ (Spray*)ptr };
			BO6JsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXAsset("material", T10R_ASSET_MATERIAL, asset->material);
			json.WriteFieldNameString("bundle");
			scriptbundle::WriteData(json, asset->bundle);
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / "bo6" / "source" / "tables" / "spray"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump spray {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10RAssetType, Worker> impl{ GetWorkers(), bo6::T10RAssetType::T10R_ASSET_SPRAY };
}