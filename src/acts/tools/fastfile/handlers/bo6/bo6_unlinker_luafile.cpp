#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo6.hpp>
#include <utils/compress_utils.hpp>

namespace {
	using namespace fastfile::handlers::bo6;
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
		uint32_t unk60_count;
		void* unk60;
	};
	static_assert(sizeof(LuaFile) == 0x68);

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

			std::vector<byte> decompressedFile{};

			size_t len{ (size_t)asset->len };
			byte* buffer{ asset->buffer };

			//int end{ utils::compress::DecompressAll(utils::compress::COMP_ZLIB, decompressedFile, buffer, len) };
			//
			//if (end < 0) {
			//	LOG_ERROR("Error when decompressing {}, using compressed file: {}", outFile.string(), utils::compress::DecompressResultName(end));
			//}
			//else {
			//	buffer = decompressedFile.data();
			//	len = decompressedFile.size();
			//}

			if (!utils::WriteFile(outFile, buffer, len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> impl{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_LUAFILE, sizeof(LuaFile) };
}
