#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>

namespace {
	using namespace fastfile::handlers::bo6;
	class ImplWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct GscObj {
				uint64_t name;
				uint32_t len;
				void* buffer;
			}; static_assert(sizeof(GscObj) == 0x18);
			GscObj* asset{ (GscObj*)ptr };

			std::filesystem::path outFile{ opt.m_output / "bo6" / "gscobj" / std::format("script_{:x}.gscc", asset->name) };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump gscobj {} len: 0x{:x}", outFile.string(), asset->len);

			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10RAssetType, Worker> impl{ GetWorkers(), bo6::T10RAssetType::T10R_ASSET_GSCOBJ };
}