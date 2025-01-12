#include <includes.hpp>
#include <utils/utils.hpp>
#include "gsc_gdb.hpp"

namespace tool::gsc::gdb {
	namespace {
		int gscgdbinfo(Process& proc, int argc, const char* argv[]) {
			std::vector<std::filesystem::path> paths{};

			for (size_t i = 2; i < argc; i++) {
				utils::GetFileRecurseExt(argv[i], paths, ".gdb\0.gdbc\0");
			}

			if (paths.empty()) {
				LOG_ERROR("Can't find gdb any file");
				return tool::BASIC_ERROR;
			}

			std::string buffer{};


			std::unordered_set<std::string> cannon{};
			for (auto& path : paths) {
				if (!utils::ReadFile(path, buffer)) {
					LOG_ERROR("Can't read {}", path.string());
					continue;
				}

				auto* obj = reinterpret_cast<GSC_GDB*>(buffer.data());

				if (*reinterpret_cast<decltype(MAGIC)*>(obj->magic) != MAGIC) {
					LOG_ERROR("Can't read {}: Bad magic", path.string());
					continue;
				}

				if (obj->magic != utils::Aligned<uint64_t>(obj->magic)) {
					LOG_ERROR("Not aligned, todo");
					continue;
				}

				LOG_INFO("Read {}...", path.string());

				//LOG_INFO("crc: {:x}, version: {}", obj->source_crc, obj->version);

				auto* lineInfo = reinterpret_cast<GSC_LINEINFO*>(utils::Aligned<uint64_t>(obj->magic + obj->lineinfo_offset));

				for (size_t i = 0; i < obj->lineinfo_count; i++) {
					//LOG_INFO("Line {} -> 0x{:x}", i, lineInfo[i].offset);
				}

				char* strings = reinterpret_cast<char*>(obj->magic + obj->stringtable_offset);

				for (size_t i = 0; i < obj->stringtable_count; i++) {
					//LOG_INFO("Str {:x} {}", hash::HashT89Scr(strings), strings);

					cannon.insert(strings);
					strings += strlen(strings) + 1;
				}
			}

			std::ofstream outstr{ "gdbstrings.txt" };

			if (outstr) {
				for (auto& can : cannon) {
					outstr << can << "\n";
				}
				outstr.close();
			}

			return tool::OK;
		}



		class GSCGDBCompilerOption {
		public:
			bool m_help{};
			const char* outputDir{};
			std::vector<const char*> inputs{};

			bool Compute(const char** args, INT startIndex, INT endIndex) {
				// default values
				for (size_t i = startIndex; i < endIndex; i++) {
					const char* arg = args[i];

					if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
						m_help = true;
					}
					else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
						if (i + 1 == endIndex) {
							LOG_ERROR("Missing value for param: {}!", arg);
							return false;
						}
						outputDir = args[++i];
					}
					else if (*arg == '-') {
						LOG_ERROR("Unknown option: {}!", arg);
						return false;
					}
					else {
						inputs.push_back(arg);
					}
				}
				if (inputs.empty() || !outputDir) {
					LOG_ERROR("Missing input or output dir");
					return false;
				}
				return true;
			}

			void PrintHelp() {
				LOG_INFO("-h --help              : Print help");
				LOG_INFO("-o --output            : Output directory");
			}
		};

		std::string ReadEscapedString(const std::string& line, size_t start, size_t* out) {
			while (start < line.length() && isspace(line[start])) {
				start++;
			}
			if (start == line.length()) {
				LOG_WARNING("invalid string, find EOF, excepted \"");
				if (out) *out = std::string::npos;
				return {};
			}
			if (line[start] != '"') {
				LOG_WARNING("invalid string, find {}, excepted \"", line[start]);
				if (out) *out = std::string::npos;
				return {};
			}
			start++; // str start
			auto newStr = std::make_unique<char[]>(line.length() - start);
			auto* newStrWriter = &newStr[0];

			// format string
			size_t i = start;
			for (; i < line.length() && line[i] != '"'; i++) {
				if (line[i] != '\\') {
					*(newStrWriter++) = line[i];
					continue; // default case
				}

				i++;

				if (i == line.length()) {
					LOG_WARNING("bad format, \\ before end");
					*(newStrWriter++) = '\\';
					continue;
				}

				switch (line[i]) {
				case 'n':
					*(newStrWriter++) = '\n';
					break;
				case 't':
					*(newStrWriter++) = '\t';
					break;
				case 'r':
					*(newStrWriter++) = '\r';
					break;
				case 'b':
					*(newStrWriter++) = '\b';
					break;
				default:
					*(newStrWriter++) = line[i];
					break;
				}
			}
			*(newStrWriter++) = 0; // end char

			if (i == line.length()) {
				LOG_WARNING("invalid string, find {}, excepted \"", line[start]);
				if (out) *out = std::string::npos;
				return {};
			}

			if (out) *out = i + 1;

			return &newStr[0];
		}

		uint32_t ReadLineNumber(const std::string& line, size_t start, size_t* end = nullptr) {
			char buff[14];
			size_t endVal{ line.find(' ', start) };

			if (endVal == std::string::npos) {
				endVal = line.length();
			}
			if (end) {
				*end = endVal;
			}
			size_t len{ (endVal - start) < 13 ? (endVal - start) : 13 }; // 13 = log8(1<<32) + 2 (0x/0)

			memcpy(buff, line.data() + start, len);
			buff[len] = 0;


			try {
				if (buff[0] == '0') {
					if (buff[1] == 'x') {
						// base 16
						return std::strtoul(buff + 2, nullptr, 16);
					}
					else {
						// base 8
						return std::strtoul(buff + 1, nullptr, 8);
					}
				}
				else {
					// base 10
					if (buff[0] == '-') {
						return -std::strtol(buff + 1, nullptr, 10);
					}
					else {
						return std::strtol(buff, nullptr, 10);
					}
				}
			}
			catch (std::exception& e) {
				LOG_WARNING("Can't read number {}: {}", buff, e.what());
			}

			return 0;
		}

	}

	bool tool::gsc::gdb::ACTS_GSC_GDB::ReadFrom(const std::filesystem::path& input) {
		std::ifstream is{ input };


		if (!is) {
			LOG_ERROR("Can't read file '{}'", input.string());
			return false;
		}

		utils::CloseEnd ce{ [&is] { is.close(); } };

		std::string line{};

		size_t lineidx{};

		while (is.good() && std::getline(is, line)) {
			lineidx++;
			if (line.empty() || line[0] == '#') {
				continue;
			}

			size_t split{ line.find(' ') };

			if (split == std::string::npos) {
				LOG_WARNING("Invalid line {}: '{}'", lineidx, line);
				continue;
			}

			std::string_view sw{ line.begin(), line.begin() + split };

			if (sw == "NAME") {
				if (name) {
					LOG_WARNING("A name was specified twice, line {}", lineidx);
				}
				else {
					nameStr = line.c_str() + split + 1;
					name = hash::HashPattern(nameStr.c_str());
					if (name) {
						nameHashed = true;
					}
					else {
						name = hash::Hash64(nameStr.c_str());
					}
				}
			}
			else if (sw == "VERSION") {
				if (version) {
					LOG_WARNING("A version was specified twice, line {}", lineidx);
				}
				else {
					version = ReadLineNumber(line, split + 1);
				}
			}
			else if (sw == "CHECKSUM") {
				if (crc) {
					LOG_WARNING("A checksum was specified twice, line {}", lineidx);
				}
				else {
					crc = ReadLineNumber(line, split + 1);
				}
			}
			else if (sw == "STRING") {
				// read string
				size_t numbers{};
				std::string str{ ReadEscapedString(line, split + 1, &numbers) };

				if (numbers == std::string::npos) {
					continue;
				}

				uint32_t offset;
				while (numbers + 1 < line.length() && (offset = ReadLineNumber(line, numbers + 1, &numbers))) {
					strings[offset] = str;
				}
			}
			else if (sw != "DEVBLOCK" && sw != "LAZYLINK") {
				LOG_WARNING("Invalid option at line {}: '{}'", lineidx, sw);
				continue;
			}

		}

		if (!name) {
			LOG_ERROR("Can't compile {}: missing filename", input.string());
			return false;
		}


		return true;
	}

	int gscgdbc(Process& proc, int argc, const char* argv[]) {
		GSCGDBCompilerOption opt{};
		if (!opt.Compute(argv, 2, argc) || opt.m_help) {
			opt.PrintHelp();
			return tool::OK;
		}

		std::vector<std::filesystem::path> inputs{};

		for (const char* in : opt.inputs) {
			utils::GetFileRecurse(in, inputs, [](const std::filesystem::path& p) {
				auto s = p.string();
				return s.ends_with(".gdb");
			});
		}

		if (inputs.empty()) {
			LOG_ERROR("No file to compile");
			return tool::BASIC_ERROR;
		}

		std::filesystem::path outDir{ opt.outputDir ? opt.outputDir : "." };

		for (const std::filesystem::path& input : inputs) {
			ACTS_GSC_GDB gdb{ input };

			if (!gdb.ReadFrom(input)) {
				continue;
			}

			std::filesystem::path outFir{ outDir / utils::va("%s%sgdb", gdb.nameHashed ? ".gsc" : "", gdb.nameStr.c_str()) };

			if (gdb.nameHashed) {
				outFir = outDir / utils::va("hashed/%s.gscgdb", gdb.nameStr.c_str());
			}
			else {
				outFir = outDir / utils::va("%sgdb", gdb.nameStr.c_str());
			}

			LOG_INFO("name ..... {}", hashutils::ExtractTmpScript(gdb.name));
			LOG_INFO("version .. 0x{:x}", gdb.version);
			LOG_INFO("Checksum . 0x{:x}", gdb.crc);
			LOG_INFO("strings");
			for (auto& [floc, str] : gdb.strings) {
				LOG_INFO("0x{:x} -> {}", floc, str);
			}

			LOG_INFO("Compiling to {}", outFir.string());

			std::vector<byte> data{};

			utils::Allocate(data, sizeof(tool::gsc::gdb::GSC_GDB));

			// todo: write fixup strings


			tool::gsc::gdb::GSC_GDB* ggdb{ reinterpret_cast<tool::gsc::gdb::GSC_GDB*>(data.data()) };
			*reinterpret_cast<uint64_t*>(ggdb->magic) = tool::gsc::gdb::MAGIC;
			ggdb->source_crc = gdb.crc;
			ggdb->version = gdb.version;

			utils::WriteFile(outFir, data.data(), data.size());
		}

		return tool::OK;
	}


	ADD_TOOL(gscgdbd, "gsc", "", "decompile GSC GDB file", nullptr, gscgdbinfo);
	ADD_TOOL(gscgdbc, "gsc", " [dir]", "compile GSC GDB file", nullptr, gscgdbc);
}