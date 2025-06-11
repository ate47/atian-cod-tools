#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>
#include <utils/compress_utils.hpp>

namespace {
	using namespace fastfile::handlers::bo6;
	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct LuaFile {
				uint64_t name;
				uint64_t unk10_count;
				uint64_t* unk10;
				uint64_t unk18;
				uint64_t unk20;
				LuaFile** unk28;
				uint16_t unk28_count;
				uint16_t unk32;
				int32_t len;
				byte* buffer;
				uint64_t unk40;
				uint64_t unk48;
				uint64_t unk50;
				uint64_t unk58;
			}; static_assert(sizeof(LuaFile) == 0x60);
			LuaFile* asset{ (LuaFile*)ptr };

			const char* n{ hashutils::ExtractPtr(asset->name) };

			if (!n) {
				n = utils::va("hashed/luafile/file_%llx.lua", asset->name);
			}

			std::filesystem::path outFile{ opt.m_output / "bo6" / "luafile" / n };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump luafile {} len: 0x{:x}", outFile.string(), asset->len);
			if (!asset->len && std::filesystem::exists(outFile)) return; // ignore empty files

			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10RAssetType, Worker> impl{ GetWorkers(), bo6::T10RAssetType::T10R_ASSET_LUAFILE };
}
