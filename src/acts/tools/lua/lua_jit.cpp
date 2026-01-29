#include <includes.hpp>
#include <cli/cli_options.hpp>
#include <core/bytebuffer.hpp>
#include <utils/compress_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/scan_container.hpp>
#include <hook/process.hpp>
#include <hook/error.hpp>
#include <core/memory_allocator.hpp>

namespace {

	struct LuaOption {
		bool help{};
		const char* outdir{};
		bool header{};
	};

	enum LuaJitFlags : byte {
		LJF_IS_BIG_ENDIAN = 1 << 0,
		LJF_IS_STRIPPED = 1 << 1,
		LJF_HAS_FFI = 1 << 2,
		LJF_HAS_FR2 = 1 << 3,

		LJF_MAX_FLAG = 4,
		LJF_MASK = (byte)~(0xFF << (LJF_MAX_FLAG)),
	};

	int luajit(int argc, const char* argv[]) {
		cli::options::CliOptions opts{};
		LuaOption opt{};

		opts.addOption(&opt.help, "show help", "--help", "", "-h");
		opts.addOption(&opt.header, "show header", "--header", "", "-H");
		opts.addOption(&opt.outdir, "Out directory", "--outdir", "", "-o");

		if (!opts.ComputeOptions(2, argc, argv) || opt.help || opts.NotEnoughParam(1)) {
			opts.PrintOptions();
		}

		std::vector<std::filesystem::path> files{};

		utils::GetFileRecurseExt(opts[0], files, ".lua\0.luac\0");

		std::vector<byte> buffer{};
		std::vector<byte> decompresed{};


		for (const std::filesystem::path& path : files) {
			std::string filename{ path.filename().string() };


			if (filename.ends_with(".dec.lua")) continue; // ignore decompiled file

			std::filesystem::path out;
			
			if (opt.outdir) {
				out = path;
				out.replace_extension();
				std::string noextfn{ out.filename().string() };

				uint64_t hash{};
				if (hash::TryHashPattern(noextfn.c_str(), hash)) {
					const char* name{ hashutils::ExtractPtr(hash) };
					if (!name) name = utils::va("hashed/luafile/file_%llx.lua", hash);
					out = std::filesystem::path{ opt.outdir } / name;
				}
				else {
					out = std::filesystem::path{ opt.outdir } / out.filename();
				}
			}
			else {
				out = path;
				out.replace_extension(".dec.lua");
			}

			if (!utils::ReadFile(path, buffer)) {
				LOG_ERROR("Can't open {}", path.string());
				continue;
			}

			LOG_INFO("Decompiling {} into {}", path.string(), out.string());

			std::filesystem::create_directories(out.parent_path());
			core::bytebuffer::ByteBuffer reader{ buffer };

			if (reader.CanRead(1) && reader.Ptr<byte>()[0] == 0x78) {
				// zlib compressed

				int dr{ utils::compress::Decompress(utils::compress::COMP_ZLIB, decompresed, buffer.data(), buffer.size()) };
				if (dr <= 0) {
					LOG_ERROR("Can't decompress: {}", dr);
					continue;
				}


				reader = core::bytebuffer::ByteBuffer{ decompresed };
			}

			uint32_t magic{ reader.Read<uint32_t>() };

			if ((magic & 0x00FFFFFF) != 0x004A4C1B) {
				LOG_ERROR("Bad LJ magic for {}: {:x}", path.string(), magic);
				continue;
			}

			byte flags{ reader.Read<byte>() };

			if (flags & LJF_IS_BIG_ENDIAN) {
				LOG_ERROR("Can't decompile big endian lua scripts");
				continue;
			}

			utils::OutFileCE os{ out };

			if (!os) {
				LOG_ERROR("Can't open output {}", out.string());
				continue;
			}


			if (opt.header) {
				os << "-- file " << path.string() << "\n";
				os << "-- magic 0x" << std::hex << magic << "flags: 0x" << std::hex << (int)flags;

				if (flags & LJF_IS_BIG_ENDIAN) os << " big";
				if (flags & LJF_IS_STRIPPED) os << " stripped";
				if (flags & LJF_HAS_FFI) os << " ffi";
				if (flags & LJF_HAS_FR2) os << " fr2";
				if (flags & ~LJF_MASK) os << " <missing>";
			}


		}

		return tool::OK;
	}
	struct lua_State;
	typedef void* (*lua_Alloc)(void* ud, void* ptr, size_t osize, size_t nsize);
	typedef void* (*lua_Reader)(lua_State* L, void* ud, size_t* sz);

	enum ljError : int {
		LUA_OK = 0,
		LUA_YIELD = 1,
		LUA_ERRRUN = 2,
		LUA_ERRSYNTAX = 3,
		LUA_ERRMEM = 4,
		LUA_ERRERR = 5,
	};

	const char* ljErrors[]{
		"ok",
		"yield",
		"run error",
		"syntax error",
		"memory error",
		"error",
	};

	const char* ljErrorMsg(int err) {
		if (err < 0 || err > ARRAYSIZE(ljErrors)) {
			return "unknown";
		}

		return ljErrors[err];
	}

	class ljec_Context {
		core::bytebuffer::ByteBuffer reader;
		core::memory_allocator::MemoryAllocator alloc{};
	public:
		utils::OutFileCE* outDump{};

		ljec_Context(std::vector<byte>& file) : reader(file) {
		}

		void* Alloc(void* ptr, size_t osize, size_t nsize) {
			if (ptr) {
				alloc.Free(ptr);
			}

			void* nv{ alloc.Alloc<byte>(nsize) };

			//LOG_TRACE("ljec_Alloc(0x{:x}) -> {}", nsize, nv);
			return nv;
		}

		void* Reader(lua_State* L, size_t* sz) {
			// read all
			size_t len{ reader.Remaining() };
			*sz = len;
			if (!len) {
				return nullptr;
			}
			return reader.ReadPtr<byte>(len);
		}

		int Write(lua_State* L, const void* p, size_t sz) {
			if (!outDump) throw std::runtime_error("no file to dump");

			utils::WriteValue(*outDump, p, sz);

			return 0;
		}
	};

	static ljec_Context* currentCtx{};

	void* ljec_Alloc(void* ud, void* ptr, size_t osize, size_t nsize) {
		return currentCtx->Alloc(ptr, osize, nsize);
	}

	void* ljec_Reader(lua_State* L, void* ud, size_t* sz) {
		return currentCtx->Reader(L, sz);
	}

	int ljec_Writer(lua_State* L, const void* p, size_t sz, void* ud) {
		return currentCtx->Write(L, p, sz);
	}

	void PatchAntiCheat(hook::scan_container::ScanContainer& scan) {
		size_t patches{};
		struct PatchInfo {
			const char* sig;
			uint32_t len;
		} patchInfo[]{
			{ "49 3B C2 0F 82 ?? ?? ?? ?? 48 8B 84 24 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 49 3B C2 0F 87 ?? ?? ?? ?? 48 8B 84 24 ?? ?? ?? ?? 80 78 FB E8 74 ??", 46 },
			{ "48 3b c1 0f 82 ?? ?? ?? ?? 48 8b 44 24 ?? 48 ?? ?? ?? ?? ?? ?? 48 3b c1 0f ?? ?? ?? ?? ?? 48 8b 44 24 ?? 80 78 fb e8 74 ??", 40 },
		};

		for (PatchInfo& info : patchInfo) {
			for (hook::library::ScanResult& res : scan.Scan(info.sig, "AC patch")) {
				// patch the whole if and goto the end 
				byte patch[]{
					0xe9, 0xFF, 0xFF, 0xFF, 0xFF // jmp XXXX
				};
				*(uint32_t*)&patch[1] = (uint32_t)res.Get<byte>(info.len) + info.len + 1 - 5;
				hook::process::WriteMemSafe(res.location, patch, sizeof(patch));
				patches++;
			}
		}
		LOG_TRACE("patched {} lua func(s)", patches);
	}

	int ljec(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

		hook::error::InstallErrorHooks(true);
		hook::module_mapper::Module mod{ true };
		if (!mod.Load(argv[2], true)) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}
		hook::scan_container::ScanContainer scan{ *mod };

		PatchAntiCheat(scan);

		std::filesystem::path in{ argv[3] };
		std::filesystem::path out;
		if (tool::NotEnoughParam(argc, 3)) {
			out = in;
			out.replace_extension(".luac");
		}
		else {
			out = argv[4];
		}

		std::vector<byte> input{ utils::ReadFile<std::vector<byte>>(in) };


		lua_State* (*lua_newstate)(lua_Alloc alloc, void* ud, int a3, int a4) =
			scan.ScanSingle("E8 ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 48 8B C8 48 8B D8 E8 ?? ?? ?? ?? BA FF FF FF FF", "lua_newstate")
			.GetRelative<int32_t, decltype(lua_newstate)>(1);

		//void (*luaL_openlibs)(lua_State* L) =
		//	scan.ScanSingle("E8 ?? ?? ?? ?? 48 8B CB E8 ?? ?? ?? ?? 48 8B CB E8 ?? ?? ?? ?? 48 8B CB E8 ?? ?? ?? ?? 48 8B C3", "luaL_openlibs")
		//	.GetRelative<int32_t, decltype(luaL_openlibs)>(1);

		int (*lua_loadx)(lua_State * L, lua_Reader reader, void* ud, const char* chunkname, const char* mode) =
			scan.ScanSingle("E8 ?? ?? ?? ?? 41 B8 04 00 00 00 8B D0 48 8B CF 48 8B 74 24 50", "lua_loadx")
			.GetRelative<int32_t, decltype(lua_loadx)>(1);

		int (*lj_cf_string_dump)(lua_State * L) =
			scan.ScanSingle("48 89 5C 24 08 57 48 83 EC 30 BA 01 00 00 00 48 8B D9 E8 ?? ?? ?? ?? 48", "lj_cf_string_dump")
			.GetPtr<decltype(lj_cf_string_dump)>();

		const char* (*lua_tolstring)(lua_State * L, int idx, size_t * len) =
			scan.ScanSingle("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 8B 44 24 28 48 8B F1", "lua_tolstring")
			.GetPtr<decltype(lua_tolstring)>();

		int (*writer_buf)(struct lua_State* L, const void* p, size_t sz, void* ud) =
			scan.ScanSingle("4C 8D 05 ?? ?? ?? ?? 89 4C 24 20 48 83 EA 68", "writer_buf")
			.GetRelative<int32_t, decltype(writer_buf)>(3);


		// 
		scan.Save();
		hook::memory::RedirectJmp(writer_buf, ljec_Writer);

		ljec_Context ctx{ input };
		currentCtx = &ctx;

		lua_State* L = lua_newstate(ljec_Alloc, &ctx, 0, 0);

		if (!L) {
			LOG_ERROR("Can't alloc lua_State");
			return tool::BASIC_ERROR;
		}

		LOG_TRACE("lua_State loaded {}", (void*)L);

		// luaL_openlibs(L); // create issue?

		int r;


		if ((r = lua_loadx(L, ljec_Reader, &ctx, argv[3], "t")) != 0) {
			LOG_ERROR("Error when compiling {} ({})", ljErrorMsg(r), r);
			LOG_ERROR("{}", lua_tolstring(L, -1, nullptr));
			return tool::BASIC_ERROR;
		}

		LOG_TRACE("script compiled, dump");
		{
			utils::OutFileCE os{ out, true, std::ios::binary };
			ctx.outDump = &os;
			lj_cf_string_dump(L);
			utils::WriteValue<byte>(os, 0);
		}

		LOG_INFO("script compiled to {}", out.string());


		return tool::OK;
	}

	ADD_TOOL(luajit, "lib", " [file] (out=file.csv)", "Read luajit file", luajit);
	ADD_TOOL(ljec, "lib", " [exe] [in] (out)", "Compile Lua jit file using exe compiler", ljec);
}