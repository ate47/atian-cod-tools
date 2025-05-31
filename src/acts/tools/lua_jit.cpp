#include <includes.hpp>
#include <cli/cli_options.hpp>
#include <core/bytebuffer.hpp>
#include <utils/compress_utils.hpp>

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

	ADD_TOOL(luajit, "lib", " [file] (out=file.csv)", "Read luajit file", luajit);

}