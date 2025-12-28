#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_cw.hpp>

namespace {
	using namespace fastfile::handlers::cw;

	class ImplWorker : public Worker {
		bool decrypt;
		const char* type;
		const char* ext;
	public:
		ImplWorker(bool decrypt, const char* type, const char* ext) : decrypt(decrypt), type(type), ext(ext) {};

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct RawFile {
				CWXHash name;
				uint32_t len;
				byte* data;
			}; static_assert(sizeof(RawFile) == 0x18);
			RawFile* asset{ (RawFile*)ptr };

			if (decrypt) {
				asset->data = cw::DecryptRawBuffer(asset->data);
				asset->len = asset->len ? asset->len - 1 : 0;
			}

			const char* name{ hashutils::ExtractPtr(asset->name) };
			if (!name) {
				name = utils::va("hashed/%s/hash_%llx%s", type, asset->name, ext);
			}
			std::filesystem::path outFile{ opt.m_output / "cw" / "source" / name };

			std::filesystem::create_directories(outFile.parent_path());
			if (!utils::WriteFile(outFile, asset->data, asset->len)) {
				LOG_ERROR("Can't write to {}", outFile.string());
				return;
			}
			LOG_INFO("Dump {} {}", type, outFile.string());
		}
	};

	utils::MapAdder<ImplWorker, XAssetType, Worker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_RAWFILE, false, "rawfile", ".raw" };
	utils::MapAdder<ImplWorker, XAssetType, Worker> implpp{ GetWorkers(), XAssetType::ASSET_TYPE_RAWFILEPREPROC, true, "rawfilepreproc", ".rawpp" };
	utils::MapAdder<ImplWorker, XAssetType, Worker> impltf{ GetWorkers(), XAssetType::ASSET_TYPE_RAWTEXTFILE, true, "rawtextfile", ".txt" };
}