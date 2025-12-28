#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_mw19.hpp>
#include <utils/compress_utils.hpp>

namespace {
	using namespace fastfile::handlers::mw19;
	struct LuaFile {
		XString name;
		uint32_t len;
		byte* buffer;
	};
	static_assert(sizeof(LuaFile) == 0x18);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			LuaFile* asset{ (LuaFile*)ptr };

			const char* n{ ValidateName(asset->name) };

			std::filesystem::path outFile{ opt.m_output / gamePath / "luafile" / n };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump luafile {} len: 0x{:x}", outFile.string(), asset->len);
			if (!asset->len && std::filesystem::exists(outFile)) return; // ignore empty files

			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, HandlerHashedAssetType, Worker> impl{ GetWorkers(), HandlerHashedAssetType::IW8H_ASSET_LUAFILE, sizeof(LuaFile) };
}
