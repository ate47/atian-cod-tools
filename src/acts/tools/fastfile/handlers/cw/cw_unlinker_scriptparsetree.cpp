#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_cw.hpp>
#include <tools/gsc/gsc_decompiler.hpp>

namespace {
	using namespace fastfile::handlers::cw;

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct ScriptParseTree {
				CWXHash name;
				byte* buffer;
				uint32_t len;
			}; static_assert(sizeof(ScriptParseTree) == 0x18);
			ScriptParseTree* asset{ (ScriptParseTree*)ptr };

			std::filesystem::path outFile{ opt.m_output / "cw" / "spt" / std::format("script_{:x}.gscc", asset->name) };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump ScriptParseTree {} len: 0x{:x}", outFile.string(), asset->len);

			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}

			if (!opt.disableScriptsDecomp) {

				tool::gsc::GscDecompilerGlobalContext gdctx{};

				std::filesystem::path outSourceGDB{ opt.m_output / "cw" / "source" / "gdb" };
				std::string outSourceGDBStr{ outSourceGDB.string() };
				gdctx.opt.m_generateGdbData = true;
				gdctx.opt.m_generateGdbBaseData = false;
				gdctx.opt.m_dbgOutputDir = outSourceGDBStr.data();
				gdctx.opt.m_platform = fastfile::GetCurrentContext().gscPlatform;
				gdctx.opt.m_dcomp = true;
				std::filesystem::path outSource{ opt.m_output / "cw" / "source" };
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

	utils::MapAdder<ImplWorker, XAssetType, Worker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_SCRIPTPARSETREE };
}