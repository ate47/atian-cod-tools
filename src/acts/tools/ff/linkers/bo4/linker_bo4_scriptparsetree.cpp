#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>
#include <tools/gsc.hpp>
#include <compiler/gsc_compiler.hpp>

namespace fastfile::linker::bo4 {
	class ScriptParseTreeWorker : public LinkerWorker {
	public:
		ScriptParseTreeWorker() : LinkerWorker("ScriptParseTree") {}

		static void AddGscHeader(BO4LinkContext& ctx, std::vector<byte>& buffer, const std::filesystem::path& path) {
			if (buffer.size() < sizeof(tool::gsc::T8GSCOBJ)) {
				LOG_ERROR("Can't read compiled gsc header for {}", path.string());
				return;
			}
			tool::gsc::T8GSCOBJ& obj{ *(tool::gsc::T8GSCOBJ*)buffer.data() };

			struct ScriptParseTree {
				XHash name;
				uintptr_t buffer;
				uint32_t len;
			}; static_assert(sizeof(ScriptParseTree) == 0x20);
			
			ScriptParseTree& spt{ utils::Allocate<ScriptParseTree>(ctx.assetData) };

			spt.name.hash = obj.name;
			spt.buffer = fastfile::ALLOC_PTR;
			spt.len = (uint32_t)buffer.size();

			// write script header
			buffer.push_back(0); // the game is reading (len + 1) so we add a byte at the end
			utils::WriteValue(ctx.assetData, buffer.data(), buffer.size());
			ctx.assets.emplace_back(games::bo4::pool::ASSET_TYPE_SCRIPTPARSETREE, fastfile::ALLOC_PTR);
			LOG_INFO("Added asset scriptparsetree {} (hash_{:x})", path.string(), spt.name.hash);
		}

		void Compute(BO4LinkContext& ctx) override {
			std::vector<std::filesystem::path> scripts{};
			std::filesystem::path scriptDir{ ctx.linkCtx.input / "scripts" };
			utils::GetFileRecurseExt(scriptDir, scripts, ".csc\0.gsc\0", true);

			for (const std::filesystem::path& sub : scripts) {
				std::filesystem::path path{ scriptDir / sub };
				std::filesystem::path scriptName{ std::filesystem::path{"scripts"} / sub };
				LOG_TRACE("Processing {} ({})", scriptName.string(), path.string());

				// compile file
				acts::compiler::CompilerConfig cfg{};
				std::string snp{ scriptName.string() };
				cfg.name = hashutils::CleanPath(snp.data());
				cfg.platform = tool::gsc::opcode::PLATFORM_PC;
				cfg.vm = tool::gsc::opcode::VMI_T8;
				cfg.detourType = acts::compiler::DETOUR_ACTS;
				cfg.clientScript = scriptName.extension() == ".csc";
				cfg.processorOpt.defines.insert(std::format("_FF_GEN_{}", ctx.linkCtx.ffname));

				std::vector<byte> buffer{};
				try {
					acts::compiler::CompileGsc(path, buffer, cfg);
				}
				catch (std::runtime_error& re) {
					LOG_ERROR("Can't compile {}: {}", path.string(), re.what());
					ctx.error = true;
					continue;
				}


				LOG_INFO("Compiled {} ({})", path.string(), cfg.name);
				AddGscHeader(ctx, buffer, path);
				// TODO: generate and add ScriptParseTreeDBG header
			}

			scripts.clear();
			utils::GetFileRecurseExt(ctx.linkCtx.input, scripts, ".cscc\0.gscc\0");
			for (const std::filesystem::path& path : scripts) {
				std::vector<byte> buffer{};
				if (!utils::ReadFile(path, buffer)) {
					LOG_ERROR("Can't read {}", path.string());
					ctx.error = true;
					continue;
				}
				AddGscHeader(ctx, buffer, path);
			}
		}
	};

	utils::ArrayAdder<ScriptParseTreeWorker, LinkerWorker> impl{ GetWorkers() };
}