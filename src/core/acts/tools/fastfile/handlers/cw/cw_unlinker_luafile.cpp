#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_cw.hpp>

namespace {
	using namespace fastfile::handlers::cw;

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct LuaFile
			{
				CWXHash name;
				uint32_t len;
				byte* buffer;
			}; static_assert(sizeof(LuaFile) == 0x18);
			LuaFile* asset{ (LuaFile*)ptr };

			const char* name{ hashutils::ExtractPtr(asset->name) };

			if (!name) name = utils::va("hashed/file_%llx.lua", asset->name);

			std::filesystem::path outFile{ opt.m_output / "cw" / "luafile" / name };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump luafile {} len: 0x{:x}", outFile.string(), asset->len);

			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, XAssetType, Worker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_LUAFILE };
}