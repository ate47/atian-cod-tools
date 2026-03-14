#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_cw.hpp>

namespace {
	using namespace fastfile::handlers::cw;

	class ImplWorker : public Worker {
		const char* type;
	public:
		ImplWorker(const char* type) : type(type) {};

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct ScriptUsing {
				CWXHash name;
				CWXHash serverName;
				CWXHash clientName;
			}; static_assert(sizeof(ScriptUsing) == 0x18);
			ScriptUsing* asset{ (ScriptUsing*)ptr };

			const char* name{ hashutils::ExtractPtr(asset->name) };

			std::filesystem::path outFile;

			if (name) {
				outFile = opt.m_output / "cw" / "source" / "tables" / "usings" / type / std::format("{}.json", name);
			}
			else {
				const char* ext;
				if (asset->serverName) {
					ext = "gsc";
				}
				else {
					ext = "csc";
				}

				outFile = opt.m_output / "cw" / "source" / "tables" / "usings" / type / "hashed" / std::format("file_{:x}.{}.json", asset->name, ext);
			}
			utils::raw_file_extractor::JsonWriter json{};

			json.BeginObject();
			json.WriteFieldNameString("name"); json.WriteValueHash(asset->name);
			if (asset->serverName) { json.WriteFieldNameString("serverName"); json.WriteValueHash(asset->serverName); }
			if (asset->clientName) { json.WriteFieldNameString("clientName"); json.WriteValueHash(asset->clientName); }
			json.EndObject();

			std::filesystem::create_directories(outFile.parent_path());
			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Can't write to {}", outFile.string());
				return;
			}
			LOG_OPT_INFO("Dump Using {} {}", type, outFile.string());
		}
	};

	utils::MapAdder<ImplWorker, XAssetType, Worker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_SCRIPT_USING, "common" };
	utils::MapAdder<ImplWorker, XAssetType, Worker> implcp{ GetWorkers(), XAssetType::ASSET_TYPE_SCRIPT_USING_CP, "cp" };
	utils::MapAdder<ImplWorker, XAssetType, Worker> implmp{ GetWorkers(), XAssetType::ASSET_TYPE_SCRIPT_USING_MP, "mp" };
	utils::MapAdder<ImplWorker, XAssetType, Worker> implwz{ GetWorkers(), XAssetType::ASSET_TYPE_SCRIPT_USING_WZ, "wz" };
	utils::MapAdder<ImplWorker, XAssetType, Worker> implzm{ GetWorkers(), XAssetType::ASSET_TYPE_SCRIPT_USING_ZM, "zm" };
}