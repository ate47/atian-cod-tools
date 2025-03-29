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
				void* buffer;
				uint32_t len;
			}; static_assert(sizeof(ScriptParseTree) == 0x20);

			ctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_SCRIPTPARSETREE, fastfile::linker::data::POINTER_NEXT);

			ctx.data.PushStream(XFILE_BLOCK_TEMP);
			ScriptParseTree spt{};
			spt.name.name = obj.name;
			spt.buffer = (void*)fastfile::linker::data::POINTER_NEXT;
			spt.len = (uint32_t)buffer.size();
			ctx.data.WriteData(spt);

			ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
			ctx.data.Align(0x20);
			ctx.data.WriteData(buffer.data(), buffer.size());
			ctx.data.WriteData<byte>(0);
			ctx.data.PopStream();

			ctx.data.PopStream();
			LOG_INFO("Added asset scriptparsetree {} (hash_{:x})", path.string(), obj.name);
		}

		static tool::gsc::opcode::Platform GetGSCPlatform(fastfile::FastFilePlatform plt) {
			switch (plt) {
			case fastfile::XFILE_PC: return tool::gsc::opcode::PLATFORM_PC;
			case fastfile::XFILE_PLAYSTATION: return tool::gsc::opcode::PLATFORM_PLAYSTATION;
			case fastfile::XFILE_XBOX: return tool::gsc::opcode::PLATFORM_XBOX;
			case fastfile::XFILE_DEV: return tool::gsc::opcode::PLATFORM_PC_ALPHA;
			default: throw std::runtime_error("Invalid pltform for this ff");
			}
		}

		static void AddGscDBGHeader(BO4LinkContext& ctx, std::vector<byte>& buffer, std::string& preproc, const std::filesystem::path& path) {
			if (buffer.size() < sizeof(tool::gsc::T8GSCOBJ)) {
				LOG_ERROR("Can't read compiled gsc header for {}", path.string());
				return;
			}
			tool::gsc::T8GSCOBJ& obj{ *(tool::gsc::T8GSCOBJ*)buffer.data() };
			struct ScriptParseTreeDBG
			{
				XHash name;
				int gdbLen;
				int srcLen;
				void* gdb;
				const char* src;
			}; static_assert(sizeof(ScriptParseTreeDBG) == 0x28);

			ctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_SCRIPTPARSETREEDBG, fastfile::linker::data::POINTER_NEXT);

			ctx.data.PushStream(XFILE_BLOCK_TEMP);
			ScriptParseTreeDBG spt{};
			spt.name.name = obj.name;
			spt.src = (const char*)fastfile::linker::data::POINTER_NEXT;
			spt.srcLen = (uint32_t)preproc.size();
			// todo: implement gdb compiler
			ctx.data.WriteData(spt);

			ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
			ctx.data.Align<char>();
			preproc.push_back(0);
			ctx.data.WriteData(preproc.data(), preproc.size());
			ctx.data.PopStream();

			ctx.data.PopStream();
			LOG_INFO("Added asset scriptparsetreedbg {} (hash_{:x})", path.string(), obj.name);
		}

		void Compute(BO4LinkContext& ctx) override {
			std::vector<std::filesystem::path> scripts{};
			std::filesystem::path scriptDir{ ctx.linkCtx.input / "scripts" };
			utils::GetFileRecurseExt(scriptDir, scripts, ".csc\0.gsc\0", true);

			bool cfgGenDBG{ ctx.ffConfig.GetBool("gsc.gendbg", false) };
			bool cfgDev{ ctx.ffConfig.GetBool("gsc.dev", false) };
			bool cfgNoDevCallInline{ ctx.ffConfig.GetBool("gsc.noDevCallInline", false) };
			bool genDevBlockAsComment{ ctx.ffConfig.GetBool("gsc.devBlockAsComment", false) };

			for (const std::filesystem::path& sub : scripts) {
				std::filesystem::path path{ scriptDir / sub };
				std::filesystem::path scriptName{ std::filesystem::path{"scripts"} / sub };
				LOG_TRACE("Processing {} ({})", scriptName.string(), path.string());

				// compile file
				acts::compiler::CompilerConfig cfg{};
				std::string snp{ scriptName.string() };
				cfg.name = hashutils::CleanPath(snp.data());
				cfg.platform = GetGSCPlatform(ctx.linkCtx.opt.platform);
				cfg.vm = tool::gsc::opcode::VMI_T8_36; // read cfg?
				cfg.detourType = acts::compiler::DETOUR_ACTS;
				cfg.computeDevOption = cfgDev;
				std::string preprocOutput{};
				if (cfgGenDBG) {
					cfg.preprocOutput = &preprocOutput;
				}
				cfg.noDevCallInline = cfgNoDevCallInline;
				cfg.processorOpt.devBlockAsComment = genDevBlockAsComment;
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
				if (cfgGenDBG) {
					AddGscDBGHeader(ctx, buffer, preprocOutput, path);
				}
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