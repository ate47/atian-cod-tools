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

		static void AddGscDBGHeader(BO4LinkContext& ctx, std::vector<byte>& buffer, std::string& preproc, std::vector<byte>& dbgbuffer, const std::filesystem::path& path) {
			if (buffer.size() < sizeof(tool::gsc::T8GSCOBJ)) {
				LOG_ERROR("Can't read compiled gsc header for {}", path.string());
				return;
			}
			tool::gsc::T8GSCOBJ& obj{ *(tool::gsc::T8GSCOBJ*)buffer.data() };
			struct ScriptParseTreeDBG {
				XHash name;
				int32_t gdbLen;
				int32_t srcLen;
				void* gdb;
				const char* src;
			}; static_assert(sizeof(ScriptParseTreeDBG) == 0x28);

			ctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_SCRIPTPARSETREEDBG, fastfile::linker::data::POINTER_NEXT);

			ctx.data.PushStream(XFILE_BLOCK_TEMP);
			ScriptParseTreeDBG spt{};
			spt.name.name = obj.name;
			if (preproc.size()) {
				spt.src = (const char*)fastfile::linker::data::POINTER_NEXT;
				spt.srcLen = (int32_t)preproc.size();
			}
			if (dbgbuffer.size()) {
				spt.gdb = (void*)fastfile::linker::data::POINTER_NEXT;
				spt.gdbLen = (int32_t)dbgbuffer.size();
			}
			ctx.data.WriteData(spt);

			ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
			if (dbgbuffer.size()) {
				ctx.data.Align(0x20);
				void* ptr{ ctx.data.AllocDataPtr<void>(dbgbuffer.size() + 1) };
				std::memcpy(ptr, dbgbuffer.data(), dbgbuffer.size());
			}

			if (preproc.size()) {
				ctx.data.Align<char>();
				void* ptr{ ctx.data.AllocDataPtr<void>(preproc.size() + 1) };
				std::memcpy(ptr, preproc.data(), preproc.size());
			}
			ctx.data.PopStream();

			ctx.data.PopStream();
			LOG_INFO("Added asset scriptparsetreedbg {} (hash_{:x})", path.string(), obj.name);
		}

		void Compute(BO4LinkContext& ctx) override {
			bool cfgGenDBG{ ctx.linkCtx.zone.GetConfigBool("gsc.gendbg", false) };
			bool cfgDev{ ctx.linkCtx.zone.GetConfigBool("gsc.dev", false) };
			bool cfgNoDevCallInline{ ctx.linkCtx.zone.GetConfigBool("gsc.noDevCallInline", false) };
			bool genDevBlockAsComment{ ctx.linkCtx.zone.GetConfigBool("gsc.devBlockAsComment", false) };

			std::vector<XHash> forcedServerScripts{};
			std::vector<XHash> forcedClientScripts{};

			for (const char*& scriptparsetreeName : ctx.linkCtx.zone.assets["scriptparsetree"]) {
				const char* scriptparsetreeNameCfg{ scriptparsetreeName };
				bool forced{ *scriptparsetreeNameCfg == '!' };
				if (forced) scriptparsetreeNameCfg++;
				std::filesystem::path scriptName{ scriptparsetreeNameCfg };
				std::filesystem::path path{ ctx.linkCtx.input / scriptparsetreeNameCfg };
				LOG_TRACE("Processing {} ({})", scriptName.string(), path.string());

				std::vector<byte> buffer{};

				bool isCsc;

				if (scriptName.extension() == ".cscc") {
					if (!utils::ReadFile(path, buffer)) {
						LOG_ERROR("Can't read {}", path.string());
						ctx.error = true;
						continue;
					}
					scriptName.replace_extension(".csc");
					isCsc = true;
				}
				else if (scriptName.extension() == ".gscc") {
					if (!utils::ReadFile(path, buffer)) {
						LOG_ERROR("Can't read {}", path.string());
						ctx.error = true;
						continue;
					}
					scriptName.replace_extension(".gsc");
					isCsc = false;
				}
				else {
					// compile file
					acts::compiler::CompilerConfig cfg{};
					std::string snp{ scriptName.string() };
					cfg.name = hashutils::CleanPath(snp.data());
					ctx.HashPathName(scriptName);
					cfg.platform = GetGSCPlatform(ctx.linkCtx.opt.platform);
					cfg.vm = tool::gsc::opcode::VMI_T8_36; // read cfg?
					cfg.detourType = acts::compiler::DETOUR_ACTS;
					cfg.computeDevOption = cfgDev;
					std::string preprocOutput{};
					std::vector<byte> dbgdata{};
					if (cfgGenDBG) {
						cfg.preprocOutput = &preprocOutput;
					}
					cfg.dbgoutput = &dbgdata;
					cfg.noDevCallInline = cfgNoDevCallInline;
					cfg.processorOpt.devBlockAsComment = genDevBlockAsComment;
					cfg.processorOpt.cwd = ctx.linkCtx.input;
					cfg.clientScript = scriptName.extension() == ".csc";
					cfg.baseDir = ctx.linkCtx.input;
					isCsc = cfg.clientScript;
					std::unordered_map<std::string, std::vector<std::string>> bgcacheCompiled{};
					cfg.precache = &bgcacheCompiled;
					cfg.useModToolOpCodes = true;
					cfg.processorOpt.defines.insert(std::format("_FF_GEN_{}", ctx.linkCtx.ffname));

					std::unordered_set<std::string> hashes{};
					cfg.hashes = &hashes;

					try {
						acts::compiler::CompileGsc(path, buffer, cfg);
					}
					catch (std::runtime_error& re) {
						LOG_ERROR("Can't compile {}: {}", path.string(), re.what());
						ctx.error = true;
						continue;
					}

					for (const std::string& str : hashes) {
						char* v{ hashutils::CleanPath(utils::CloneString(str.data())) };
						ctx.linkCtx.RegisterHash(hash::Hash64(str.data()), str);
						ctx.linkCtx.RegisterHash(hash::HashT89Scr(str.data()), str);
					}

					for (auto& [bgtype, entries] : bgcacheCompiled) {
						games::bo4::pool::BGCacheTypes type{ games::bo4::pool::BGCacheIdFromName(bgtype.c_str()) };
						if (type == games::bo4::pool::BGCacheTypes::BG_CACHE_TYPE_INVALID) {
							LOG_ERROR("Can't compile {}: Invalid precache asset type {}", path.string(), bgtype);
							ctx.error = true;
							continue;
						}

						std::unordered_set<uint64_t>& bg{ ctx.bgcache[type] };

						for (const std::string& entry : entries) {
							uint64_t hash{ ctx.HashXHash(entry.c_str()) };
							LOG_DEBUG("add {}::{} ({:x}) to bgcache", games::bo4::pool::BGCacheNameFromId(type), entry, hash);
							bg.insert(hash);
						}
					}

					LOG_INFO("Compiled {} ({})", path.string(), cfg.name);
					if (preprocOutput.size() || dbgdata.size()) {
						AddGscDBGHeader(ctx, buffer, preprocOutput, dbgdata, path);
					}
				}
				if (forced) {
					tool::gsc::T8GSCOBJ& obj{ *(tool::gsc::T8GSCOBJ*)buffer.data() };

					// add forced script
					if (isCsc) {
						forcedClientScripts.emplace_back(obj.name);
					}
					else {
						forcedServerScripts.emplace_back(obj.name);
					}
				}

				AddGscHeader(ctx, buffer, scriptName);
			}

			if (forcedClientScripts.size() || forcedServerScripts.size()) {
				struct ScriptParseTreeForced {
					XHash name;
					uint32_t gscCount;
					uint32_t cscCount;
					XHash* gscScripts;
					XHash* cscScripts;
				};
				ctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_SCRIPTPARSETREEFORCED, fastfile::linker::data::POINTER_NEXT);

				ctx.data.PushStream(XFILE_BLOCK_TEMP);
				ScriptParseTreeForced header{};
				header.name.name = ctx.ffnameHash;
				header.gscCount = (uint32_t)forcedServerScripts.size();
				header.cscCount = (uint32_t)forcedClientScripts.size();
				if (forcedServerScripts.size()) header.gscScripts = (XHash*)fastfile::linker::data::POINTER_NEXT;
				if (forcedClientScripts.size()) header.cscScripts = (XHash*)fastfile::linker::data::POINTER_NEXT;

				// todo: implement gdb compiler
				ctx.data.WriteData(header);

				ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
				ctx.data.Align(8);
				ctx.data.WriteData(forcedServerScripts.data(), forcedServerScripts.size() * sizeof(forcedServerScripts[0]));
				ctx.data.Align(8);
				ctx.data.WriteData(forcedClientScripts.data(), forcedClientScripts.size() * sizeof(forcedClientScripts[0]));
				ctx.data.PopStream();

				ctx.data.PopStream();
				LOG_INFO("Added asset scriptparsetreeforced {} (hash_{:x})", ctx.linkCtx.ffname, ctx.ffnameHash);
			}
		}
	};

	utils::ArrayAdder<ScriptParseTreeWorker, LinkerWorker> impl{ GetWorkers() };
}