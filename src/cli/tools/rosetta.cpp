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


}

#ifndef CI_BUILD

// removed from CI build to add some challenge to the skids
ADD_TOOL("rosetta", " [rosetta_file] [compiled script dump]", "Compute the opcodes of a dump using a Rosetta file", false, rosetta);

#endif
