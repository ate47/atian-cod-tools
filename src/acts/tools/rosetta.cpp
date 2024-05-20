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

		if (memcmp(checkBuff, "ROS2", 4)) {
			std::cout << "Bad magic for '" << argv[2] << "'\n";
			rfile.close();
			return tool::BASIC_ERROR;
		}

		uint64_t len{};
		uint64_t lenblk{};

		rfile.read(reinterpret_cast<char*>(&len), sizeof(len));

		std::string headerStr{};

		for (size_t i = 0; i < len; i++) {
			rfile.read(reinterpret_cast<char*>(&lenblk), sizeof(lenblk));
			headerStr.resize(lenblk);
			rfile.read(reinterpret_cast<char*>(headerStr.data()), lenblk);
			tool::dump::T8GSCSimpleHeader* header = reinterpret_cast<tool::dump::T8GSCSimpleHeader*>(headerStr.data());

			auto& block = rosettaBlocks[header->name];
			block.header.resize(lenblk);
			memcpy(block.header.data(), header, lenblk);

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

		tool::dump::T8GSCSimpleHeader* buffer{};
		size_t size{};

		std::cout << "Find " << files.size() << " file(s)\n";

		std::unordered_map<byte, std::shared_ptr<tool::gsc::GSCOBJHandler>> handlers{};

		for (const auto& file : files) {
			if (!utils::ReadFileNotAlign(file, reinterpret_cast<void*&>(buffer), size, false)) {
				std::cerr << "Error when reading " << file.string() << "\n";
				continue;
			}

			//std::cout << "Reading data from " << file.string() << " (" << hashutils::ExtractTmpScript(buffer->name) << ")\n";

			byte vm;

			auto magicVal = *reinterpret_cast<uint64_t*>(buffer) & ~0xFF00000000000000;
			if (magicVal == 0xa0d43534780) {
				// Treyarch GSC file, use revision
				vm = buffer->magic[7];
			}
			else {
				LOG_ERROR("Bad magic 0x{:x}", *reinterpret_cast<uint64_t*>(buffer->magic));
				continue;
			}

			tool::gsc::GSCOBJHandler* reader = [&handlers, vm, buffer, size]() -> tool::gsc::GSCOBJHandler* {

				auto hit = handlers.find(vm);

				if (hit == handlers.end()) {
					auto bld = tool::gsc::GetGscReader(vm);

					if (!bld) {
						LOG_ERROR("Can't create reader for vm {:x}", (int)vm);
						return nullptr;
					}

					return &*(handlers[vm] = (*bld)(buffer->magic, size));
				}
				else {
					return &*(hit->second);
				}
			}();

			if (!reader) {
				continue;
			}

			reader->file = buffer->magic;


			auto blk = rosettaBlocks.find(reader->GetName());

			if (blk == rosettaBlocks.end()) {
				std::cerr << "Script " << file.string() << " (" << hashutils::ExtractTmpScript(reader->GetName()) << ") isn't in the rosetta index\n";
				std::free(buffer);
				buffer = NULL;
				continue;
			}

			auto& bl = blk->second;

			// clear CRC
			reader->SetChecksum(0);
			reader->file = (byte*)bl.header.data();
			reader->SetChecksum(0);
			reader->file = buffer->magic;

			byte* b1 = (byte*)bl.header.data();
			byte* b2 = (byte*)reader->Ptr();
			if (memcmp(b1, b2, reader->GetHeaderSize())) {
				std::cerr << "Script " << file.string() << " (" << hashutils::ExtractTmpScript(reader->GetName()) << ") has a different header\n";
				std::cerr << "b1:";
				for (size_t i = 0; i < reader->GetHeaderSize(); i++) {
					std::cerr << " " << std::hex << std::setw(2) << std::setfill('0') << (int)b1[i];
				}
				std::cerr << "\n";
				std::cerr << "b2:";
				for (size_t i = 0; i < reader->GetHeaderSize(); i++) {
					std::cerr << " " << std::hex << std::setw(2) << std::setfill('0') << (int)b2[i];
				}
				std::cerr << "\n";
				std::free(buffer);
				buffer = NULL;
				continue;
			}

			for (auto opco : bl.blocks) {
				if (opco.location >= size) {
					std::cerr << "Bad location: " << std::hex << opco.location << " in " << file.string() << " (" << hashutils::ExtractTmpScript(reader->GetName()) << ")\n";
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
ADD_TOOL("rosetta", "gsc", " [rosetta_file] [compiled script dump]", "Compute the opcodes of a dump using a Rosetta file", nullptr, rosetta);
ADD_TOOL("gscfreq", "gsc", "", "Frequency of opcodes", nullptr, gscfreq);
ADD_TOOL("vtdr", "gsc", "", "VTable tests", nullptr, vtabledumread);

#endif
