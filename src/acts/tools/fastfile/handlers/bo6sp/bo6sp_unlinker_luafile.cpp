#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo6_sp.hpp>
#include <utils/compress_utils.hpp>

namespace {
	using namespace fastfile::handlers::bo6sp;
	struct LuaFile {
		uint64_t name;
		uint64_t unk10_count;
		uint64_t* unk10;
		uint64_t unk18;
		uint64_t unk20;
		uint64_t unk28;
		LuaFile** unk30;
		uint16_t unk30_count;
		uint16_t unk3a;
		int32_t len;
		byte* buffer;
		uint32_t unk50_count;
		void* unk50;
	};
	static_assert(sizeof(LuaFile) == 0x58);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			LuaFile* asset{ (LuaFile*)ptr };

			const char* n{ hashutils::ExtractPtr(asset->name) };

			if (!n) {
				n = utils::va("hashed/luafile/file_%llx.lua", asset->name);
			}

			std::filesystem::path outFile{ opt.m_output / "bo6sp" / "luafile" / n };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump luafile {} len: 0x{:x}", outFile.string(), asset->len);
			if (!asset->len && std::filesystem::exists(outFile)) return; // ignore empty files

			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> impl{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_LUAFILE, sizeof(LuaFile) };
}
