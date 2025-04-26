#include <includes.hpp>
#include <tools/gsc.hpp>
#include <tools/dump.hpp>
#include <hook/module_mapper.hpp>
#include <tools/gsc_decompiler.hpp>

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

	int rosetta(int argc, const char* argv[]) {

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
			if (magicVal == tool::gsc::opcode::VMI_TRE_BASE) {
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

			reader->SetFile((byte*)buffer->magic, 0);


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
			reader->SetFile((byte*)bl.header.data(), 0);
			reader->SetChecksum(0);
			reader->SetFile((byte*)buffer->magic, 0);

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
			std::cout << std::hex << utils::PtrOrElse(tool::gsc::opcode::OpCodeName((tool::gsc::opcode::OPCode)(key)), "null") << " = ";
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
				std::cout << std::hex << utils::PtrOrElse(tool::gsc::opcode::OpCodeName((tool::gsc::opcode::OPCode)(val)), "null") << " ";
			}
			std::cout << "\n";
		}


		return tool::OK;
	}



	int gscfreq(int argc, const char* argv[]) {
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
			std::cout << "Opcode " << utils::PtrOrElse(OpCodeName(freq[i].opcode), "null") << " -> " << std::dec << freq[i].count << "\n";
		}


		return tool::OK;
	}

	int vtabledumread(int argc, const char* argv[]) {
		using namespace tool::gsc::opcode;
		if (tool::NotEnoughParam(argc, 3)) {
			return tool::BAD_USAGE;
		}

		size_t maxOpCode{ std::strtoull(argv[3], nullptr, 16) };

		LOG_TRACE("set maxopcode to {}/0x{:x}", argv[3], maxOpCode);

		std::vector<byte> buffer{};

		if (!utils::ReadFile(argv[2], buffer)) {
			LOG_ERROR("Can't read {}!", argv[2]);
			return tool::BASIC_ERROR;
		}

		if (buffer.size() < maxOpCode * 8) {
			LOG_ERROR("Can't read {}! Too small: 0x{:x} < 0x{:x}", argv[2], buffer.size(), maxOpCode * 8);
			return tool::BASIC_ERROR;
		}

		std::map<uintptr_t, std::vector<size_t>> map{};
		uintptr_t* codes = (uintptr_t*)buffer.data();

		for (size_t i = 0; i < maxOpCode; i++) {
			map[codes[i]].push_back(i);
		}
		{
			utils::OutFileCE os{ argv[4] };

			if (!os) {
				LOG_ERROR("Can't open {}", argv[4]);
				return tool::BASIC_ERROR;
			}

			for (const auto& [loc, map] : map) {
				os << std::hex << std::setfill('0') << std::setw(3) << map[0] << ":" << loc << " -> {";
				for (size_t i = 0; i < map.size(); i++) {
					if (i) {
						os << ", ";
					}
					os << std::hex << "0x" << map[i];
				}
				os << "}\n";
			}

			LOG_INFO("Dump into {}", argv[4]);
		}

		return tool::OK;
	}

	int vtabledumreadexe(int argc, const char* argv[]) {
		using namespace tool::gsc::opcode;
		if (tool::NotEnoughParam(argc, 4)) {
			return tool::BAD_USAGE;
		}

		size_t maxOpCode{ std::strtoull(argv[3], nullptr, 16) };
		uintptr_t loc{ std::strtoull(argv[4], nullptr, 16) };
		uint64_t offset{ tool::NotEnoughParam(argc, 5) ? 0 : std::strtoull(argv[6], nullptr, 16) };

		LOG_TRACE("set maxopcode to {}/0x{:x}", argv[3], maxOpCode);

		std::vector<byte> buffer{};

		hook::module_mapper::Module mod{ true };

		if (!mod.Load(argv[2], false)) {
			LOG_ERROR("Can't read {}!", argv[2]);
			return tool::BASIC_ERROR;
		}

		std::map<uintptr_t, std::vector<size_t>> map{};
		uintptr_t* codes = (uintptr_t*)mod->Get<uintptr_t>(loc);

		for (size_t i = 0; i < maxOpCode; i++) {
			map[codes[i]].push_back(i);
		}
		{
			utils::OutFileCE os{ argv[5] };

			if (!os) {
				LOG_ERROR("Can't open {}", argv[5]);
				return tool::BASIC_ERROR;
			}

			for (const auto& [loc, map] : map) {
				os << std::hex << std::setfill('0') << std::setw(3) << (offset + map[0]) << ":" << mod->Rloc(loc) << " -> {";
				for (size_t i = 0; i < map.size(); i++) {
					if (i) {
						os << ", ";
					}
					os << std::hex << "0x" << (offset + map[i]);
				}
				os << "}\n";
			}

			LOG_INFO("Dump into {}", argv[5]);
		}

		return tool::OK;
	}

	int vtdrexerebase(int argc, const char* argv[]) {
		using namespace tool::gsc::opcode;
		if (tool::NotEnoughParam(argc, 7)) {
			return tool::BAD_USAGE;
		}

		std::filesystem::path exe{ argv[2] };
		size_t maxOpCode{ std::strtoull(argv[3], nullptr, 16) };
		std::filesystem::path output{ argv[4] };
		uintptr_t locori{ std::strtoull(argv[5], nullptr, 16) };
		uintptr_t locdest{ std::strtoull(argv[6], nullptr, 16) };
		tool::gsc::opcode::VMId vm{ tool::gsc::opcode::VMOf(argv[7]) };
		tool::gsc::opcode::Platform plt{ tool::gsc::opcode::PlatformOf(argv[8]) };
		uint64_t start{ tool::NotEnoughParam(argc, 8) ? 0 : std::strtoull(argv[9], nullptr, 16) };
		tool::gsc::opcode::VmInfo* nfo{};

		if (!vm || !tool::gsc::opcode::IsValidVm(vm, nfo)) {
			LOG_ERROR("Invalid vm: {}", argv[7]);
			return tool::BASIC_ERROR;
		}

		LOG_INFO("load {}", exe.string());
		LOG_INFO("vm {}/{}", nfo->name, tool::gsc::opcode::PlatformName(plt));
		LOG_INFO("opc | max: 0x{:x} base:0x{:x}", maxOpCode, start);
		LOG_INFO("loc | ori: 0x{:x} dest:0x{:x}", locori, locdest);


		hook::module_mapper::Module mod{ true };

		// load module
		if (!mod.Load(exe, false)) {
			LOG_ERROR("Can't read {}!", exe.string());
			return tool::BASIC_ERROR;
		}

		std::map<uintptr_t, tool::gsc::opcode::OPCode> funcOpCodes{};
		uintptr_t* codesOri = (uintptr_t*)mod->Get<uintptr_t>(locori);

		for (size_t i = 0; i < maxOpCode; i++) {
			tool::gsc::opcode::OPCode& op{ funcOpCodes[mod->Rloc(codesOri[i])] };

			if (!op) {
				const tool::gsc::opcode::OPCodeInfo* nfo{ tool::gsc::opcode::LookupOpCode(vm, plt, (uint16_t)i) };
				if (nfo) {
					op = nfo->m_id;
				}
			}
		}

		std::map<uintptr_t, std::vector<size_t>> map{};
		uintptr_t* codesDest = (uintptr_t*)mod->Get<uintptr_t>(locdest);

		for (size_t i = 0; i < maxOpCode; i++) {
			map[codesDest[i]].push_back(start + i);
		}
		{
			utils::OutFileCE os{ output, true };

			for (const auto& [loc, ops] : map) {
				os << tool::gsc::opcode::OpCodeName(funcOpCodes[mod->Rloc(loc)]) << ":" << mod->Rloc(loc) << " -> {";
				for (size_t i = 0; i < ops.size(); i++) {
					if (i) {
						os << ", ";
					}
					os << std::hex << "0x" << (ops[i]);
				}
				os << "}\n";
			}

			LOG_INFO("Dump into {}", output.string());
		}

		return tool::OK;
	}

	int vtdrrebase(int argc, const char* argv[]) {
		using namespace tool::gsc::opcode;
		if (tool::NotEnoughParam(argc, 6)) {
			return tool::BAD_USAGE;
		}

		size_t maxOpCode{ std::strtoull(argv[2], nullptr, 16) };
		std::filesystem::path output{ argv[3] };
		std::filesystem::path ori{ argv[4] };
		std::filesystem::path dest{ argv[5] };
		tool::gsc::opcode::VMId vm{ tool::gsc::opcode::VMOf(argv[6]) };
		tool::gsc::opcode::Platform plt{ tool::gsc::opcode::PlatformOf(argv[7]) };
		uint64_t start{ tool::NotEnoughParam(argc, 7) ? 0 : std::strtoull(argv[8], nullptr, 16) };
		tool::gsc::opcode::VmInfo* nfo{};

		if (!vm || !tool::gsc::opcode::IsValidVm(vm, nfo)) {
			LOG_ERROR("Invalid vm: {}", argv[6]);
			return tool::BASIC_ERROR;
		}

		LOG_INFO("vm {}/{}", nfo->name, tool::gsc::opcode::PlatformName(plt));
		LOG_INFO("opc | max: 0x{:x} base:0x{:x}", maxOpCode, start);
		LOG_INFO("loc | ori: {} dest:{}", ori.string(), dest.string());


		std::vector<byte> oribuff{};
		std::vector<byte> destbuff{};


		if (!utils::ReadFile(ori, oribuff)) {
			LOG_ERROR("Can't read {}!", ori.string());
			return tool::BASIC_ERROR;
		}
		if (!utils::ReadFile(dest, destbuff)) {
			LOG_ERROR("Can't read {}!", dest.string());
			return tool::BASIC_ERROR;
		}

		std::map<uintptr_t, tool::gsc::opcode::OPCode> funcOpCodes{};
		uintptr_t* codesOri = (uintptr_t*)oribuff.data();

		for (size_t i = 0; i < maxOpCode; i++) {
			tool::gsc::opcode::OPCode& op{ funcOpCodes[codesOri[i]] };

			if (!op) {
				const tool::gsc::opcode::OPCodeInfo* nfo{ tool::gsc::opcode::LookupOpCode(vm, plt, (uint16_t)i) };
				if (nfo) {
					op = nfo->m_id;
				}
			}
		}

		std::map<uintptr_t, std::vector<size_t>> map{};
		uintptr_t* codesDest = (uintptr_t*)destbuff.data();

		for (size_t i = 0; i < maxOpCode; i++) {
			map[codesDest[i]].push_back(start + i);
		}
		{
			utils::OutFileCE os{ output, true };

			for (const auto& [loc, ops] : map) {
				os << tool::gsc::opcode::OpCodeName(funcOpCodes[loc]) << ":" << loc << " -> {";
				for (size_t i = 0; i < ops.size(); i++) {
					if (i) {
						os << ", ";
					}
					os << std::hex << "0x" << (ops[i]);
				}
				os << "}\n";
			}

			LOG_INFO("Dump into {}", output.string());
		}

		return tool::OK;
	}

	int gscmap(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 4)) return tool::BAD_USAGE;

		std::filesystem::path in{ argv[2] };
		std::filesystem::path out{ argv[3] };
		tool::gsc::opcode::Platform pltIn{ tool::gsc::opcode::PlatformOf(argv[4]) };
		tool::gsc::opcode::Platform pltOut{ tool::gsc::opcode::PlatformOf(argv[5]) };

		if (!pltIn) {
			LOG_ERROR("Invalid platform: ", argv[4]);
			return tool::BASIC_ERROR;
		}
		if (!pltOut) {
			LOG_ERROR("Invalid platform: ", argv[5]);
			return tool::BASIC_ERROR;
		}

		if (pltIn == pltOut) {
			LOG_ERROR("The platforms are the same, nothing to do.");
			return tool::OK;
		}

		std::vector<std::filesystem::path> files{};
		utils::GetFileRecurseExt(in, files, ".gscc\0.cscc\0", true);

		if (files.empty()) {
			LOG_ERROR("Can't find any script in {}", in.string());
			return tool::BASIC_ERROR;
		}

		std::vector<byte> scriptData{};
		std::vector<byte> scriptDataPatched{};
		int r{ tool::OK };
		for (const std::filesystem::path& rel : files) {
			std::filesystem::path pathIn{ in / rel };
			std::filesystem::path pathOut{ out / rel };

			LOG_DEBUG("Loading {}...", pathIn.string());

			if (!utils::ReadFile(pathIn, scriptData) || scriptData.size() < 8) {
				r = tool::BASIC_ERROR;
				LOG_ERROR("Can't read {}", pathIn.string());
				continue;
			}

			uint64_t magic{ *(uint64_t*)scriptData.data() };

			tool::gsc::opcode::VmInfo* nfo{};

			if (!tool::gsc::opcode::IsValidVmMagic(magic, nfo)) {
				LOG_ERROR("Can't read {}: Invalid magic 0x{:x}", pathIn.string(), magic);
				continue;
			}

			std::filesystem::create_directories(pathOut.parent_path());

			tool::gsc::GscDecompilerGlobalContext ctx{};
			std::unordered_map<uint64_t, std::unordered_set<uint32_t>> opcodesLocs{};
			ctx.noDump = true;
			ctx.opcodesLocs = &opcodesLocs;

			scriptDataPatched.clear();
			scriptDataPatched.insert(scriptDataPatched.end(), scriptData.begin(), scriptData.end());
			
			if (tool::gsc::DecompileGsc(scriptData.data(), scriptData.size(), pathIn, ctx) != tool::OK) {
				r = tool::BASIC_ERROR;
				continue;
			}

			bool isU16{ nfo->HasFlag(tool::gsc::opcode::VmFlags::VMF_OPCODE_U16) };

			size_t remapped{};
			bool error{};
			for (auto& [_, ops] : opcodesLocs) {
				for (uint32_t loc : ops) {
					void* ptr{ &scriptDataPatched[loc] };

					uint16_t oldVal;
					if (isU16) {
						oldVal = *(uint16_t*)ptr;
					}
					else {
						oldVal = *(uint8_t*)ptr;
					}

					const tool::gsc::opcode::OPCodeInfo* origin{ tool::gsc::opcode::LookupOpCode(nfo->vmMagic, pltIn, oldVal) };

					if (!origin || !origin->m_id) {
						LOG_ERROR("Can't find origin value for opcodes 0x{:x}", oldVal);
						error = true;
						continue;
					}

					auto [ok, val] = tool::gsc::opcode::GetOpCodeId(nfo->vmMagic, pltOut, origin->m_id);

					if (!ok) {
						LOG_ERROR("Can't find new value for opcodes 0x{:x} ({})", oldVal, origin->m_name);
						error = true;
						continue;
					}

					if (isU16) {
						*(uint16_t*)ptr = val;
					}
					else {
						*(uint8_t*)ptr = (uint8_t)val;
					}

					remapped++;
				}
				break;
			}

			if (error) {
				LOG_ERROR("Can't map {}", pathIn.string());
				continue;
			}

			if (!utils::WriteFile(pathOut, scriptDataPatched)) {
				r = tool::BASIC_ERROR;
				LOG_ERROR("Can't write {}", pathOut.string());
				continue;
			}

			LOG_INFO("mapped {} -> {} ({} opcodes)", pathIn.string(), pathOut.string(), remapped);
		}

		LOG_INFO("done");

		return r;
	}
}

ADD_TOOL(rosetta, "gsc", " [rosetta_file] [compiled script dump]", "Compute the opcodes of a dump using a Rosetta file", rosetta);
ADD_TOOL(gscfreq, "gsc", "", "Frequency of opcodes", gscfreq);
ADD_TOOL(vtdr, "dev_gsc", " [dump] [maxOpCodes] [output]", "Dump opcode vtable", vtabledumread);
ADD_TOOL(vtdrexe, "dev_gsc", " [exe] [maxOpCodes] [loc] [output] (start)", "Dump opcode vtable from exe", vtabledumreadexe);
ADD_TOOL(vtdrrebase, "dev_gsc", " [maxOpCodes] [output] [ori] [dest] [vm] [plt] (start)", "Dump opcode vtable from exe and rebase them", vtdrrebase);
ADD_TOOL(vtdrexerebase, "dev_gsc", " [exe] [maxOpCodes] [output] [locori] [locdest] [vm] [plt] (start)", "Dump opcode vtable from exe and rebase them", vtdrexerebase);
ADD_TOOL(gscmap, "gsc", " [in] [out] [origin] [dest]", "Map a script from one platform to another", gscmap);
