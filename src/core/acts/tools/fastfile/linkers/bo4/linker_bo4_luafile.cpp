#include <includes.hpp>
#include <tools/fastfile/linkers/linker_bo4.hpp>
#include <core/preprocessor.hpp>
#include <hksclua.hpp>

namespace {
	using namespace fastfile::linker::bo4;

	class XAssetLinkerImpl : public XAssetLinker {
	public:
		using XAssetLinker::XAssetLinker;

		void Compute(BO4LinkContext& ctx, const char* id, BO4FFContext& ff) override {
			const char* preprocDefs{ ctx.linkCtx.zone.GetConfig("lua.defs", "") };

			std::filesystem::path rfpath{ id };
			std::filesystem::path path{ ctx.linkCtx.input / rfpath };
			std::string pathstr{ path.string() };

			std::string luaFile{};

			if (!utils::ReadFile(path, luaFile)) {
				LOG_ERROR("Can't read {}", pathstr);
				ctx.error = true;
				return;
			}
			core::preprocessor::PreProcessorOption prepopt{};
			prepopt.cwd = ctx.linkCtx.input;
			prepopt.AddDefine(std::format("_FF_GEN_{}", ff.ffname));
			prepopt.AddDefineListConfig(preprocDefs);

			if (!prepopt.ApplyPreProcessor(luaFile, pathstr.data())) {
				LOG_ERROR("Error when applying preprocessor on data {}", pathstr);
				ctx.error = true;
				return;
			}

			hksc_StateSettings settings;
			hksc_State* H;

			hksI_StateSettings(&settings);
			H = hksI_newstate(&settings);
			if (!H) {
				LOG_ERROR("Cannot create luafile state: not enough memory");
				ctx.error = true;
				return;
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
				return;
			}

			struct LuaFile {
				XHash name;
				uint32_t len;
				byte* buffer;
			}; static_assert(sizeof(LuaFile) == 0x20);

			ff.data.PushStream(XFILE_BLOCK_TEMP);
			LuaFile& lf{ ff.data.AllocStreamRef<LuaFile>() };

			lf.name.name = ctx.HashPathName(rfpath);
			lf.buffer = (byte*)fastfile::linker::memory::POINTER_NEXT;
			lf.len = (uint32_t)outluac.size();
			outluac.push_back(0);

			ff.data.PushStream(XFILE_BLOCK_VIRTUAL);
			ff.data.Align(0x10);
			ff.data.WriteStream(outluac.data(), outluac.size());
			ff.data.PopStream();

			ff.data.PopStream();

			LOG_INFO("Added asset luafile {} (hash_{:x})", rfpath.string(), lf.name.name);
		}
	};

	utils::MapAdder<XAssetLinkerImpl, XAssetType, XAssetLinker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_LUAFILE };
}