#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>
#include <tools/ff/handlers/bo6/bo6_unlinker_scriptbundle.hpp>
namespace {
	using namespace fastfile::handlers::bo6;

	struct ContrailData {
		XHash64 name;
		ParticleSystem* particleSystem1;
		ParticleSystem* particleSystem2;
		scriptbundle::ScriptBundleObjectData bundle;
	};

	static_assert(sizeof(ContrailData) == 0x38);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			ContrailData* asset{ (ContrailData*)ptr };
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXAsset("particleSystem1", T10H_ASSET_PARTICLESYSTEM, asset->particleSystem1);
			json.WriteFieldValueXAsset("particleSystem2", T10H_ASSET_PARTICLESYSTEM, asset->particleSystem2);
			json.WriteFieldNameString("bundle");
			scriptbundle::WriteData(json, asset->bundle);
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / gamePath / "source" / "tables" / "contraildata"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump contraildata {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> impl{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_CONTRAILDATA, sizeof(ContrailData) };
}