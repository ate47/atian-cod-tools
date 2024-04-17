#include <includes.hpp>
#include "tools/gsc.hpp"
#include "tools/dump.hpp"

namespace {
	using tool::gsc::RosettaBlockType;
	using tool::gsc::RosettaOpCodeBlock;

	struct T8ScriptParseTreeEntry {
		uint64_t name;
		uint64_t pad0;
		uintptr_t buffer;
		uint32_t size;
		uint32_t pad02;
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

	int rosetta(Process& proc, int argc, const char* argv[]) {

		if (argc < 4) {
			return tool::BAD_USAGE;
		}

		std::ifstream rfile{ argv[2], std::ios::binary };

		if (!rfile) {
			std::cout << "Can't open file '" << argv[2] << "'\n";
		}

		std::map<uint64_t, tool::gsc::RosettaFileData> rosettaBlocks{};

		char checkBuff[4] = { 0 };

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

		std::map<uint16_t, std::set<uint16_t>> map{};
		std::map<uint16_t, std::set<uint16_t>> maplookup{};


		if (!std::filesystem::exists(locDir)) {
			std::cerr << locDir.string() << " doesn't exist!\n";
			return tool::BASIC_ERROR;
		}

		std::vector<std::filesystem::path> files{};

		fillmap(locDir, files);

		tool::dump::T8GSCSimpleHeader* buffer = NULL;
		size_t size;

		std::cout << "Find " << files.size() << " file(s)\n";

		for (const auto& file : files) {
			if (!utils::ReadFileNotAlign(file, reinterpret_cast<void*&>(buffer), size, false)) {
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

				auto val = *reinterpret_cast<uint16_t*>(reinterpret_cast<byte*>(buffer) + opco.location);
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



	int gscfreq(Process& proc, int argc, const char* argv[]) {
		using namespace tool::gsc::opcode;
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		byte vm = (byte)std::stol(argv[2], nullptr, 16);

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

		for (uint16_t i = 0; i < 0x1000; i++) {
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

	int vtabledumread(Process& proc, int argc, const char* argv[]) {
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

		std::map<uintptr_t, std::vector<size_t>> map{};
		auto* codes = reinterpret_cast<uintptr_t*>(buffer);

		for (size_t i = 0; i < 0x1000; i++) {
			map[codes[i]].push_back(i);
		}

		for (const auto& [loc, map] : map) {
			std::cout << std::hex << std::setfill('0') << std::setw(3) << map[0] << ":" << loc << " -> {";
			for (size_t i = 0; i < map.size(); i++) {
				if (i) {
					std::cout << ", ";
				}
				std::cout << std::hex << "0x" << map[i];
			}
			std::cout << "}\n";
		}


		std::free(buffer);

		std::cout << "done\n";

		return tool::OK;
	}
}

#ifndef CI_BUILD

// removed from CI build to add some challenge to the skids
ADD_TOOL("rosetta", " [rosetta_file] [compiled script dump]", "Compute the opcodes of a dump using a Rosetta file", nullptr, rosetta);
ADD_TOOL("gscfreq", "", "Frequency of opcodes", nullptr, gscfreq);
ADD_TOOL("vtdr", "", "VTable tests", nullptr, vtabledumread);

#endif
