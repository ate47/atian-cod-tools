#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>
#include <tools/gsc_decompiler.hpp>

namespace {
	using namespace fastfile::handlers::bo6;
	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			struct GscObj {
				uint64_t name;
				uint32_t len;
				byte* buffer;
			}; static_assert(sizeof(GscObj) == 0x18);
			GscObj* asset{ (GscObj*)ptr };

			std::filesystem::path outFile{ opt.m_output / "bo6" / "gscobj" / std::format("script_{:x}.gscc", asset->name) };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump gscobj {} len: 0x{:x}", outFile.string(), asset->len);

			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}

			if (!opt.disableScriptsDecomp) {
				tool::gsc::GscDecompilerGlobalContext gdctx{};

				std::filesystem::path outSourceGDB{ opt.m_output / "bo6" / "source" / "gdb" };
				std::string outSourceGDBStr{ outSourceGDB.string() };
				gdctx.opt.m_generateGdbData = true;
				gdctx.opt.m_generateGdbBaseData = false;
				gdctx.opt.m_dbgOutputDir = outSourceGDBStr.data();
				gdctx.opt.m_platform = tool::gsc::opcode::PLATFORM_PC; // todo: use header type
				gdctx.opt.m_dcomp = true;
				std::filesystem::path outSource{ opt.m_output / "bo6" / "source" };
				std::string outSourceStr{ outSource.string() };
				gdctx.opt.m_outputDir = outSourceStr.data();

				std::vector<byte> scriptData;
				byte* alignedAlloc{ utils::Aligned(asset->buffer, 0x20) };

				if (asset->buffer != alignedAlloc) {
					// we clone the buffer to align it
					utils::Allocate(scriptData, asset->len + 0x20);
					alignedAlloc = utils::Aligned(scriptData.data(), 0x20);
					std::memcpy(alignedAlloc, asset->buffer, asset->len);
				}

				int r{ tool::gsc::DecompileGsc(alignedAlloc, asset->len, outFile, gdctx) };

				if (r) {
					LOG_ERROR("Error when decompiling script");
				}
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10RAssetType, Worker> impl{ GetWorkers(), bo6::T10RAssetType::T10R_ASSET_GSCOBJ };
}