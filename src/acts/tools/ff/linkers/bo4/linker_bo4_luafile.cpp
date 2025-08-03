#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>
#include <compiler/preprocessor.hpp>
#include <hksclua.hpp>

namespace fastfile::linker::bo4 {
	class LuaFileWorker : public LinkerWorker {
	public:
		LuaFileWorker() : LinkerWorker("LuaFile") {}

		void Compute(BO4LinkContext& ctx) override {
			const char* preprocDefs{ ctx.linkCtx.zone.GetConfig("lua.defs", "") };

			for (fastfile::zone::AssetData& assval : ctx.linkCtx.zone.assets["luafile"]) {
				std::filesystem::path rfpath{ assval.value };
				assval.handled = true;
				std::filesystem::path path{ ctx.linkCtx.input / rfpath };
				std::string pathstr{ path.string() };

				std::string luaFile{};

				if (!utils::ReadFile(path, luaFile)) {
					LOG_ERROR("Can't read {}", pathstr);
					ctx.error = true;
					continue;
				}
				acts::compiler::preprocessor::PreProcessorOption prepopt{};
				prepopt.cwd = ctx.linkCtx.input;
				prepopt.AddDefine(std::format("_FF_GEN_{}", ctx.linkCtx.ffname));
				prepopt.AddDefineListConfig(preprocDefs);

				if (!prepopt.ApplyPreProcessor(luaFile, pathstr.data())) {
					LOG_ERROR("Error when applying preprocessor on data {}", pathstr);
					ctx.error = true;
					continue;
				}

				hksc_StateSettings settings;
				hksc_State* H;

				hksI_StateSettings(&settings);
				H = hksI_newstate(&settings);
				if (!H) {
					LOG_ERROR("Cannot create luafile state: not enough memory");
					ctx.error = true;
					continue;
				}

				utils::CloseEnd ceH{ [H] { hksI_close(H); } };

				std::vector<byte> outluac{};

				lua_setmode(H, HKSC_MODE_SOURCE);
				lua_setbytecodestrippinglevel(H, BYTECODE_STRIPPING_ALL);
				int status{ 
					hksI_parser_buffer(H, luaFile.data(), luaFile.size(), pathstr.data(), [](hksc_State* H, void* ud) -> int {
						return lua_dump(H, [](hksc_State* H, const void* p, size_t size, void* u) -> int {
							std::vector<byte>* outluac = (std::vector<byte>*)u;

							utils::WriteValue(*outluac, p, size);

							return 0;
						}, ud);
					}, &outluac)
				};

				if (status) {
					LOG_ERROR("Can't compile lua {}: {}", pathstr, lua_geterror(H));
					ctx.error = true;
					continue;
				}

				struct LuaFile {
					XHash name;
					uint32_t len;
					byte* buffer;
				}; static_assert(sizeof(LuaFile) == 0x20);

				ctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_LUAFILE, fastfile::linker::data::POINTER_NEXT);

				ctx.data.PushStream(XFILE_BLOCK_TEMP);
				LuaFile lf{};

				lf.name.name = ctx.HashPathName(rfpath);
				lf.buffer = (byte*)fastfile::linker::data::POINTER_NEXT;
				lf.len = (uint32_t)outluac.size();
				outluac.push_back(0);
				ctx.data.WriteData(lf);

				ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
				ctx.data.Align(0x10);
				ctx.data.WriteData(outluac.data(), outluac.size());
				ctx.data.PopStream();

				ctx.data.PopStream();

				LOG_INFO("Added asset luafile {} (hash_{:x})", rfpath.string(), lf.name.name);
			}
		}
	};

	utils::ArrayAdder<LuaFileWorker, LinkerWorker> impl{ GetWorkers() };
}