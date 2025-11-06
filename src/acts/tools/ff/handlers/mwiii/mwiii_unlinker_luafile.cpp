#include <includes.hpp>
#include <tools/ff/handlers/handler_game_mwiii.hpp>
#include <utils/compress_utils.hpp>

namespace {
	using namespace fastfile::handlers::mwiii;
	struct LuaFile {
		XHash64 name;
		uint32_t len;
		byte* buffer;
	};
	static_assert(sizeof(LuaFile) == 0x18);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			LuaFile* asset{ (LuaFile*)ptr };

			const char* n{ hashutils::ExtractPtr(asset->name) };

			if (!n) {
				n = utils::va("hashed/luafile/file_%llx.lua", asset->name);
			}

			std::filesystem::path outFile{ opt.m_output / gamePath / "luafile" / n };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump luafile {} len: 0x{:x}", outFile.string(), asset->len);
			if (!asset->len && std::filesystem::exists(outFile)) return; // ignore empty files

			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, HandlerHashedAssetType, Worker> impl{ GetWorkers(), HandlerHashedAssetType::JUPH_ASSET_LUAFILE, sizeof(LuaFile) };
}
