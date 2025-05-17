#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	class ScriptParseTreeWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct LuaFile {
				XHash name;
				uint32_t len;
				byte* buffer;
			}; static_assert(sizeof(LuaFile) == 0x20);
			LuaFile* asset{ (LuaFile*)ptr };

			const char* n{ hashutils::ExtractPtr(asset->name.name) };

			if (!n) {
				n = utils::va("hashed/%llx.lua", asset->name.name);
			}

			std::filesystem::path outFile{ opt.m_output / "bo4" / "luafile" / n };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump lua file {} 0x{:x}", outFile.string(), asset->len);
			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ScriptParseTreeWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_LUAFILE };
}