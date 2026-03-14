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

	struct LuaCompOption {
		bool help{};
		bool compressed{};
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

			uint32_t u0{ reader.Read<uint32_t>() };

			uint32_t magic{ u0 & 0x00FFFFFF };

			if (magic != 0x4A4C1B) {
				LOG_ERROR("Bad LJ magic for {}: {:x}", path.string(), magic);
				continue;
			}

			uint32_t version{ u0 >> 24 };

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
				os << "-- magic 0x" << std::hex << magic << " version 0x" << version << " flags: 0x" << (int)flags;

				if (flags & LJF_IS_BIG_ENDIAN) os << " big";
				if (flags & LJF_IS_STRIPPED) os << " stripped";
				if (flags & LJF_HAS_FFI) os << " ffi";
				if (flags & LJF_HAS_FR2) os << " fr2";
				if (flags & ~LJF_MASK) os << " <missing>";
				os << "\n";
			}

			if (!(flags & LJF_IS_STRIPPED)) {
				uint64_t len{ reader.ReadVByte() };
				if (opt.header) {
					std::string chunkName;
					chunkName.resize(len);
					reader.Read(chunkName.data(), len);
					os << "-- chunkname " << chunkName << "\n";
				}
				else {
					reader.Skip(len);
				}
			}

			// read proto


		}

		return tool::OK;
	}
	/*
	0x0 - not enough memory
	0x12 - error in error handling
	0x2a - C++ exception
	0x38 - string length overflow
	0x4f - userdata length overflow
	0x68 - stack overflow
	0x77 - stack overflow (%s)
	0x8b - table overflow
	0x9a - table index is NaN
	0xad - table index is nil
	0xc0 - invalid key to 'next'
	0xd6 - attempt to call a %s value
	0xf1 - attempt to %s %s '%s' (a %s value)
	0x114 - attempt to %s a %s value
	0x12d - attempt to compare %s with %s
	0x14b - attempt to compare two %s values
	0x16c - loop in gettable
	0x17d - loop in settable
	0x18e - call
	0x193 - index
	0x199 - perform arithmetic on
	0x1af - concatenate
	0x1bb - get length of
	0x1c9 - calling '%s' on bad self (%s)
	0x1e7 - bad argument #%d to '%s' (%s)
	0x205 - %s expected, got %s
	0x219 - invalid value
	0x227 - value expected
	0x236 - coroutine expected
	0x249 - nil or table expected
	0x25f - Lua function expected
	0x275 - function or level expected
	0x290 - string/function/table expected
	0x2af - boolean or proxy expected
	0x2c9 - 'for' initial value must be a number
	0x2ee - 'for' limit must be a number
	0x30b - 'for' step must be a number
	0x327 - iw stack frame corruption
	0x341 - no calling environment
	0x358 - attempt to yield across C-call boundary
	0x380 - bad light userdata pointer
	0x39b - bad action while in __gc metamethod
	0x3bf - bad FPU precision (use D3DCREATE_FPU_PRESERVE with DirectX)
	0x3fb - assertion failed!
	0x40d - cannot change a protected metatable
	0x431 - too many results to unpack
	0x44c - reader function must return a string
	0x471 - 'tostring' must return a string to 'print'
	0x49c - index out of range
	0x4af - base out of range
	0x4c1 - level out of range
	0x4d4 - invalid level
	0x4e2 - invalid option
	0x4f1 - invalid option '%s'
	0x505 - invalid format
	0x514 - 'setfenv' cannot change environment of given object
	0x548 - cannot resume running coroutine
	0x568 - cannot resume dead coroutine
	0x585 - cannot resume non-suspended coroutine
	0x5ab - wrong number of arguments to 'insert'
	0x5d1 - invalid value (%s) at index %d in table for 'concat'
	0x606 - invalid order function for sorting
	0x629 - attempt to use a closed file
	0x646 - standard file is closed
	0x65e - unable to generate a unique filename
	0x683 - field '%s' missing in date table
	0x6a4 - unable to dump given function
	0x6c2 - string slice too long
	0x6d8 - missing '[' after '%f' in pattern
	0x6fa - invalid pattern capture
	0x712 - malformed pattern (ends with '%')
	0x734 - malformed pattern (missing ']')
	0x754 - unbalanced pattern
	0x767 - pattern too complex
	0x77b - invalid capture index
	0x791 - too many captures
	0x7a3 - unfinished capture
	0x7b6 - invalid option '%s' to 'format'
	0x7d6 - invalid replacement value (a %s)
	0x7f7 - name conflict for module '%s'
	0x815 - JIT compiler permanently disabled by build option
	0x847 - unknown or malformed optimization flag '%s'
	0x873 - attempt to load chunk with wrong mode
	0x899 - %s near '%s'
	0x8a6 - chunk has too many lines
	0x8bf - chunk has too many syntax levels
	0x8e0 - malformed number
	0x8f1 - unfinished long string
	0x908 - unfinished long comment
	0x920 - unfinished string
	0x932 - invalid escape sequence
	0x94a - invalid long string delimiter
	0x968 - '%s' expected
	0x976 - control structure too long
	0x991 - function or expression too complex
	0x9b4 - chunk has more than %d local variables
	0x9db - main function has more than %d %s
	0x9fd - function at line %d has more than %d %s
	0xa25 - '%s' expected (to close '%s' at line %d)
	0xa4e - function too long for return fixup
	0xa71 - <name> or '...' expected
	0xa8a - function arguments expected
	0xaa6 - unexpected symbol
	0xab8 - cannot use '...' outside a vararg function
	0xae3 - syntax error
	0xaf0 - '=' or 'in' expected
	0xb05 - no loop to break
	0xb16 - no loop to continue
	0xb2a - undefined label '%s'
	0xb3f - duplicate label '%s'
	0xb54 - <goto %s> jumps into the scope of local '%s'
	0xb81 - unfinished xhash
	0xb92 - invalid xhash char
	0xba5 - cannot deduce xhash type in unsecure builds
	0xbd1 - cannot load incompatible bytecode
	0xbf3 - cannot load malformed bytecode
	*/

	int lj_errors(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;

		hook::error::InstallErrorHooks(true);
		hook::module_mapper::Module mod{ true };
		if (!mod.Load(argv[2], true)) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}

		const char* error0{ tool::NotEnoughParam(argc, 2) ? "not enough memory" : argv[3] };

		const char* lj_errmsg{ mod->ScanStringSingle(error0, "ERRMEM").GetPtr<const char*>()};
		const char* current{ lj_errmsg };

		while (*current) {
			LOG_INFO("0x{:x} - {}", (size_t)(current - lj_errmsg), current);

			current += std::strlen(current) + 1;
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
		std::vector<byte>* outDump{};


		int (*lua_loadx)(lua_State* L, lua_Reader reader, void* ud, const char* chunkname, const char* mode) {};
		int (*lj_cf_string_dump)(lua_State* L) {};
		const char* (*lua_tolstring)(lua_State* L, int idx, size_t* len) {};
		int (*writer_buf)(lua_State* L, const void* p, size_t sz, void* ud) {};
		void (*lj_err_throw)(lua_State* L, int status) {};

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
		cli::options::CliOptions opts{};
		LuaCompOption opt{};

		opts.addOption(&opt.help, "show help", "--help", "", "-h");
		opts.addOption(&opt.compressed, "compress result", "--compressed", "", "-z");

		if (!opts.ComputeOptions(2, argc, argv) || opt.help || opts.NotEnoughParam(2)) {
			opts.PrintOptions();
			return tool::OK;
		}

		hook::error::InstallErrorHooks(true);
		hook::module_mapper::Module mod{ true };
		if (!mod.Load(opts[0], true)) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}
		hook::scan_container::ScanContainer scan{ *mod };

		std::filesystem::path in{ opts[1] };
		std::filesystem::path out;
		if (opts.NotEnoughParam(3)) {
			out = in;
			out.replace_extension(".luac");
		}
		else {
			out = opts[3];
		}

		std::vector<byte> input{ utils::ReadFile<std::vector<byte>>(in) };

		ljec_Context ctx{ input };

		lua_State* (*lua_newstate)(lua_Alloc alloc, void* ud, int a3, int a4) =
			scan.ScanSingle("E8 ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 48 8B C8 48 8B D8 E8 ?? ?? ?? ?? BA FF FF FF FF", "lua_newstate")
			.GetRelative<int32_t, decltype(lua_newstate)>(1);

		//void (*luaL_openlibs)(lua_State* L) =
		//	scan.ScanSingle("E8 ?? ?? ?? ?? 48 8B CB E8 ?? ?? ?? ?? 48 8B CB E8 ?? ?? ?? ?? 48 8B CB E8 ?? ?? ?? ?? 48 8B C3", "luaL_openlibs")
		//	.GetRelative<int32_t, decltype(luaL_openlibs)>(1);

		ctx.lua_loadx = scan.ScanSingle("E8 ?? ?? ?? ?? 41 B8 04 00 00 00 8B D0 48 8B CF 48 8B 74 24 50", "lua_loadx")
			.GetRelative<int32_t, decltype(ctx.lua_loadx)>(1);

		ctx.lj_cf_string_dump = scan.ScanSingle("48 89 5C 24 08 57 48 83 EC 30 BA 01 00 00 00 48 8B D9 E8 ?? ?? ?? ?? 48", "lj_cf_string_dump")
			.GetPtr<decltype(ctx.lj_cf_string_dump)>();

		ctx.lua_tolstring = scan.ScanSingle("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 8B 44 24 28 48 8B F1", "lua_tolstring")
			.GetPtr<decltype(ctx.lua_tolstring)>();

		ctx.writer_buf = scan.ScanSingle("4C 8D 05 ?? ?? ?? ?? 89 4C 24 20 48 83 EA 68", "writer_buf")
			.GetRelative<int32_t, decltype(ctx.writer_buf)>(3);


		PatchAntiCheat(scan);
		scan.Save();
		hook::memory::RedirectJmp(ctx.writer_buf, ljec_Writer);

		currentCtx = &ctx;

		lua_State* L = lua_newstate(ljec_Alloc, &ctx, 0, 0);

		if (!L) {
			LOG_ERROR("Can't alloc lua_State");
			return tool::BASIC_ERROR;
		}

		LOG_TRACE("lua_State loaded {}", (void*)L);

		// luaL_openlibs(L); // create issue?

		int r;


		if ((r = ctx.lua_loadx(L, ljec_Reader, &ctx, opts[1], "t")) != 0) {
			const char* msg{ ctx.lua_tolstring(L, -1, nullptr) };
			if (msg) {
				LOG_ERROR("{}", msg);
			}
			LOG_ERROR("Error when compiling {} ({})", ljErrorMsg(r), r);
			return tool::BASIC_ERROR;
		}

		LOG_TRACE("script compiled");

		std::vector<byte> obuff{};
		ctx.outDump = &obuff;
		ctx.lj_cf_string_dump(L);

		if (opt.compressed) {
			size_t compressSize{};
			std::unique_ptr<byte[]> compressBuff{ utils::compress::Compress(utils::compress::COMP_ZLIB | utils::compress::COMP_HIGH_COMPRESSION, obuff.data(), obuff.size(), &compressSize)};

			LOG_TRACE("script compressed (0x{:x} -> 0x{:x})", obuff.size(), compressSize);
			obuff = { &compressBuff[0], &compressBuff[compressSize] };
		}

		if (!utils::WriteFile(out, obuff)) {
			LOG_ERROR("can't write to {}", out.string());
			return tool::BASIC_ERROR;
		}

		LOG_INFO("script compiled to {}", out.string());


		return tool::OK;
	}

	ADD_TOOL(luajit, "lib", " [file] (out=file.csv)", "Read luajit file", luajit);
	ADD_TOOL(lj_errors, "lib", " [exe] (start)", "Dump luajit error from exe", lj_errors);
	ADD_TOOL(ljec, "lib", " [exe] [in] (out)", "Compile Lua jit file using exe compiler", ljec);
}