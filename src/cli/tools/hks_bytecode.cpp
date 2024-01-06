#include <includes.hpp>

namespace {
	enum LuaType {
		LT_NIL = 0,
		LT_BOOLEAN,
		LT_LIGHTUSERDATA,
		LT_NUMBER,
		LT_STRING,
		LT_TABLE,
		LT_FUNCTION,
		LT_USERDATA,
		LT_THREAD,
		LT_IFUNCTION,
		LT_CFUNCTION,
		LT_UI64,
		LT_STRUCT,
		LT_XHASH,

		LT_BADTYPE = 0xFFFF
	};

	LuaType ReadLuaType(LPCCH type) {
		static std::unordered_map<std::string, LuaType> luaTypeMap{
			{ "TNIL", LT_NIL },
			{ "TBOOLEAN", LT_BOOLEAN },
			{ "TLIGHTUSERDATA", LT_LIGHTUSERDATA },
			{ "TNUMBER", LT_NUMBER },
			{ "TSTRING", LT_STRING },
			{ "TTABLE", LT_TABLE },
			{ "TFUNCTION", LT_FUNCTION },
			{ "TUSERDATA", LT_USERDATA },
			{ "TTHREAD", LT_THREAD },
			{ "TIFUNCTION", LT_IFUNCTION },
			{ "TCFUNCTION", LT_CFUNCTION },
			{ "TUI64", LT_UI64 },
			{ "TSTRUCT", LT_STRUCT },
			{ "TXHASH", LT_XHASH },
		};

		auto it = luaTypeMap.find(type);

		return it == luaTypeMap.end() ? LT_BADTYPE : it->second;
	}


	struct CLIOption {
		bool m_help = false;
		bool m_verbose = true;
		bool m_header = false;

		LPCCH m_outputDir{};
		std::vector<LPCCH> m_inputFiles{};

		bool Compute(LPCCH* args, INT startIndex, INT endIndex) {
			// default values
			for (size_t i = startIndex; i < endIndex; i++) {
				LPCCH arg = args[i];

				if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
					m_help = true;
				}
				else if (!strcmp("-s", arg) || !_strcmpi("--silent", arg)) {
					m_verbose = false;
				}
				else if (!strcmp("-H", arg) || !_strcmpi("--header", arg)) {
					m_header = true;
				}
				else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					m_outputDir = args[++i];
				}

				else if (*arg == '-') {
					std::cerr << "Unknown option: " << arg << "!\n";
					return false;
				}
				else {
					m_inputFiles.push_back(arg);
				}
			}
			if (!m_inputFiles.size()) {
				m_inputFiles.push_back("scriptparsetree");
			}
			return true;
		}

		void PrintHelp(std::ostream& out) {
			out << "-h --help          : Print help\n"
				<< "-o --output [d]    : Output dir\n";
		}
	};

	bool HandleByteCode(BYTE* buffer, size_t size, const std::filesystem::path& origin, CLIOption& opt) {
		constexpr auto minimumSize = 14 // header
			+ 4 // numTypeObjects
			+ 4 // typeId
			+ 4 // nameSize
			;
		if (size < minimumSize || *reinterpret_cast<UINT32*>(buffer) != 0x61754C1B) {
			std::cerr << "Bad magic : 0x" << std::hex << *reinterpret_cast<UINT32*>(buffer) << "\n";
			return false;
		}

		if (buffer[4] != 0x51) {
			std::cerr << "Bad version : 0x" << std::hex << (int)buffer[4] << "\n";
			return false;
		}

		// header is 14 bytes
		size_t loc = 14;

		std::filesystem::path outputFile;
		const auto fn = origin.filename().string();
		if (opt.m_outputDir) {
			outputFile = std::filesystem::path{ opt.m_outputDir } / std::format("{}.dec.lua", fn);
		}
		else {
			outputFile = origin.parent_path() / std::format("{}.dec.lua", fn);
		}

		std::filesystem::create_directories(outputFile.parent_path());

		std::ofstream out{ outputFile };

		if (!out) {
			std::cerr << "Can't create output file " << outputFile.string() << "\n";
			return false;
		}

		out << "-- file: " << origin.string() << "\n";

		UINT32 numTypeObjects = *reinterpret_cast<UINT32*>(buffer + loc);
		loc += 4;

		out << "-- types (" << std::dec << numTypeObjects << "):\n";

		std::unordered_map<UINT32, LuaType> typeIds{};

		for (size_t i = 0; i < numTypeObjects; i++) {
			if (loc + 8 >= size) {
				std::cerr << "Bad type declaration (not enough bytes)\n";
				out.close();
				return false;
			}

			UINT32 typeId = *reinterpret_cast<UINT32*>(buffer + loc);
			loc += 4;

			UINT32 typeSize = *reinterpret_cast<UINT32*>(buffer + loc);
			loc += 4;

			if (typeSize > 32 || loc + typeSize >= size || buffer[loc + typeSize - 1]) {
				std::cerr << "Bad text buffer\n";
				out.close();
				return false;
			}

			auto* name = reinterpret_cast<LPCCH>(buffer + loc);
			auto luaType = ReadLuaType(name);

			if (luaType == LT_BADTYPE) {
				std::cerr << "Unknown Lua type: " << name << "\n";
				out.close();
				return false;
			}

			typeIds[typeId] = luaType;

			out << "-- " << std::dec << std::setw(2) << std::setfill(' ') << typeId << " : " << name << "\n";
			loc += typeSize;
		}



		out.close();

		return true;
	}

	int hks_bytecode(Process& proc, int argc, const char* argv[]) {
		CLIOption opt{};

		if (!opt.Compute(argv, 2, argc) || opt.m_help || !opt.m_inputFiles.size()) {
			opt.PrintHelp(std::cout);
			return tool::OK;
		}

		std::vector<std::filesystem::path> paths{};

		for (const auto* file : opt.m_inputFiles) {
			utils::GetFileRecurse(file, paths, [](const std::filesystem::path& f) {
				const auto str = f.string();
				return str.ends_with(".luac") || str.ends_with(".lua");
			});
		}

		if (!paths.size()) {
			std::cerr << "No input file\n";
			return tool::BASIC_ERROR;
		}

		LPVOID buffer{};
		SIZE_T size{};

		for (const auto& path : paths) {
			std::cout << "Reading " << path.string() << "\n";

			if (!utils::ReadFileNotAlign(path, buffer, size)) {
				std::cerr << "Can't read file\n";
				continue;
			}

			if (!HandleByteCode(reinterpret_cast<BYTE*>(buffer), size, path, opt)) {
				std::cerr << "Error when reading\n";
			}

			std::free(buffer);
			buffer = NULL;
		}
		std::cout << "Done\n";

		return tool::OK;
	}
}

ADD_TOOL("hks", " [file]", "Lua Havok Script tests", nullptr, hks_bytecode);
