#include <includes.hpp>


namespace {
	using tool::gsc::RosettaBlockType;
	using tool::gsc::RosettaOpCodeBlock;

	struct T8ScriptParseTreeEntry {
		UINT64 name;
		UINT64 pad0;
		uintptr_t buffer;
		UINT32 size;
		UINT32 pad02;
	};

	void fillmap(const std::filesystem::path& path, std::vector<std::filesystem::path>& paths) {
		if (std::filesystem::is_directory(path)) {
			for (const auto& sub : std::filesystem::directory_iterator{ path }) {
				fillmap(sub, paths);
			}
			return;
		}

		paths.push_back(path);
	}

	int rosetta(const Process& proc, int argc, const char* argv[]) {

		if (argc < 4) {
			return tool::BAD_USAGE;
		}

		std::ifstream rfile{ argv[2], std::ios::binary };

		if (!rfile) {
			std::cout << "Can't open file '" << argv[2] << "'\n";
		}

		std::map<UINT64, tool::gsc::RosettaFileData> rosettaBlocks{};

		CHAR checkBuff[4] = { 0 };

		rfile.read(checkBuff, 4);

		if (memcmp(checkBuff, "ROSE", 4)) {
			std::cout << "Bad magic for '" << argv[2] << "'\n";
			rfile.close();
			return tool::BASIC_ERROR;
		}

		size_t len = 0;
		size_t lenblk = 0;

		rfile.read(reinterpret_cast<char*>(&len), sizeof(len));

		tool::gsc::T8GSCOBJ header{};

		for (size_t i = 0; i < len; i++) {

			rfile.read(reinterpret_cast<char*>(&header), sizeof(header));
			auto& block = rosettaBlocks[header.name];
			memcpy(&block.header, &header, sizeof(header));

			rfile.read(reinterpret_cast<char*>(&lenblk), sizeof(lenblk));
			for (size_t i = 0; i < lenblk; i++) {
				auto& b = block.blocks.emplace_back();

				rfile.read(reinterpret_cast<char*>(&b), sizeof(b));
			}
		}

		rfile.read(checkBuff, 3);

		if (memcmp(checkBuff, "END", 3)) {
			std::cout << "Bad end magic for '" << argv[2] << "'\n";
			rfile.close();
			return tool::BASIC_ERROR;
		}


		rfile.close();

		std::cout << "Read rosetta index with " << std::dec << rosettaBlocks.size() << " script(s)\n";
		std::filesystem::path locDir = argv[3];

		std::map<UINT16, std::set<UINT16>> map{};
		std::map<UINT16, std::set<UINT16>> maplookup{};


		if (!std::filesystem::exists(locDir)) {
			std::cerr << locDir.string() << " doesn't exist!\n";
			return tool::BASIC_ERROR;
		}

		std::vector<std::filesystem::path> files{};

		fillmap(locDir, files);

		tool::dump::T8GSCSimpleHeader* buffer = NULL;
		SIZE_T size;

		std::cout << "Find " << files.size() << " file(s)\n";

		for (const auto& file : files) {
			if (!utils::ReadFileNotAlign(file, reinterpret_cast<LPVOID&>(buffer), size, false)) {
				std::cerr << "Error when reading " << file.string() << "\n";
				continue;
			}

			//std::cout << "Reading data from " << file.string() << " (" << hashutils::ExtractTmpScript(buffer->name) << ")\n";

			auto blk = rosettaBlocks.find(buffer->name);

			if (blk == rosettaBlocks.end()) {
				std::cerr << "Script " << file.string() << " (" << hashutils::ExtractTmpScript(buffer->name) << ") isn't in the rosetta index\n";
				std::free(buffer);
				buffer = NULL;
				continue;
			}

			auto& bl = blk->second;

			// clear CRC
			bl.header.crc = 0;
			reinterpret_cast<tool::gsc::T8GSCOBJ*>(buffer)->crc = 0;

			if (memcmp(&bl.header, buffer, sizeof(bl.header))) {
				std::cerr << "Script " << file.string() << " (" << hashutils::ExtractTmpScript(buffer->name) << ") has a different header\n";
				std::free(buffer);
				buffer = NULL;
				continue;
			}

			for (auto opco : bl.blocks) {
				if (opco.location >= size) {
					std::cerr << "Bad location: " << std::hex << opco.location << " in " << file.string() << " (" << hashutils::ExtractTmpScript(buffer->name) << ")\n";
				}

				auto val = *reinterpret_cast<UINT16*>(reinterpret_cast<BYTE*>(buffer) + opco.location);
				auto old = opco.opcode;

				map[old].insert(val);
				maplookup[val].insert(old);
			}


			std::free(buffer);
			buffer = NULL;
		}

		std::cout << "Mapping: \n";
		int count = 0;
		for (auto [key, vec] : map) {
			std::cout << std::hex << tool::gsc::opcode::OpCodeName((tool::gsc::opcode::OPCode)(key)) << " = ";
			for (const auto val : vec) {
				std::cout << std::hex << val << " ";
				count++;
			}
			std::cout << "\n";
		}

		std::cout << std::hex << count << " values\n";

		std::cout << "Lookup errors: \n";

		for (auto [key, vec] : maplookup) {
			if (vec.size() == 1) {
				continue;
			}
			std::cout << std::hex << key << " = ";
			for (const auto val : vec) {
				std::cout << std::hex << tool::gsc::opcode::OpCodeName((tool::gsc::opcode::OPCode)(val)) << " ";
			}
			std::cout << "\n";
		}


		return tool::OK;
	}



	int gscfreq(const Process& proc, int argc, const char* argv[]) {
		using namespace tool::gsc::opcode;
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		BYTE vm = (BYTE)std::stol(argv[2], nullptr, 16);

		VmInfo* info;

		if (!IsValidVm(vm, info)) {
			std::cerr << "Bad vm: " << std::hex << (int)vm << "\n";
			return tool::BASIC_ERROR;
		}

		std::cout << "Reading " << info->name << "\n";

		struct Count {
			OPCode opcode{};
			int count{};
		};

		Count freq[OPCODE_COUNT];

		for (size_t i = 0; i < OPCODE_COUNT; i++) {
			freq[i].count = 0;
			freq[i].opcode = (OPCode)i;
		}

		for (UINT16 i = 0; i < 0x1000; i++) {
			auto* ci = LookupOpCode(vm, PLATFORM_PC, i);

			freq[(size_t)ci->m_id].count++;
		}

		std::sort(std::begin(freq), std::end(freq), [](const Count& a, const Count& b) { return a.count > b.count; });

		for (size_t i = 0; i < OPCODE_COUNT; i++) {
			if (!freq[i].count) {
				continue;
			}
			std::cout << "Opcode " << OpCodeName(freq[i].opcode) << " -> " << std::dec << freq[i].count << "\n";
		}


		return tool::OK;
	}

	int vtabledumread(const Process& proc, int argc, const char* argv[]) {
		using namespace tool::gsc::opcode;
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		PVOID buffer = NULL;
		size_t bufferSize = 0;

		if (!utils::ReadFileNotAlign(argv[2], buffer, bufferSize)) {
			std::cerr << "Can't read " << argv[2] << "!\n";
			return tool::BASIC_ERROR;
		}

		struct OpCode {
			uintptr_t val1;
			uintptr_t val2;
			uintptr_t val3;
			uintptr_t val4;
		};

		struct OpCodeCount {
			int val1;
			int val2;
			int val3;
			int val4;
		};

		auto* codes = reinterpret_cast<OpCode*>(buffer);

		std::map<uintptr_t, OpCodeCount> map{};

		for (size_t i = 0; i < 0xFFF; i++) {
			auto& c = codes[i];

			map[c.val1].val1++;
			map[c.val2].val2++;
			map[c.val3].val3++;
			map[c.val4].val4++;
		}

		for (const auto& [loc, count] : map) {
			std::cout << std::hex << loc << " -> " 
				<< std::dec << count.val1 << "/" << count.val2 << "/" << count.val3 << "/" << count.val4 << "\n";
		}


		std::free(buffer);

		std::cout << "done\n";

		return tool::OK;
	}
}

#ifndef CI_BUILD

// removed from CI build to add some challenge to the skids
ADD_TOOL("rosetta", " [rosetta_file] [compiled script dump]", "Compute the opcodes of a dump using a Rosetta file", false, rosetta);
ADD_TOOL("gscfreq", "", "Frequency of opcodes", false, gscfreq);
ADD_TOOL("vtdr", "", "VTable tests", false, vtabledumread);

#endif
