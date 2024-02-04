#include <includes.hpp>
#include "tools/cw/cw.hpp"
#include "tools/gsc.hpp"

char* cw::DecryptString(char* str) {
	// for now I'm using the DLL to call the decrypt function
	if (*reinterpret_cast<BYTE*>(str) == 0x8b) {
		return str + 3; // encryption ptr
	}
	if ((*str & 0xC0) == 0x80) {
		// encrypted
		str[0] = 0;
		str[1] = 0;
	}
	return str;
}

BYTE* cw::DecryptRawBuffer(BYTE* buffer) {
	BYTE* v14;
	UINT64 v15;
	BYTE v16;
	UINT64 v17;
	UINT32 v18;
	UINT32 v19;
	BYTE* v20 = buffer;
	if (!*buffer)
		return buffer + 1;
	v14 = buffer + 1;
	v15 = 0i64;
	v16 = 0;
	v17 = 0xB4A57B0F6DEDABEDui64;
	if (!buffer[1]) {
		buffer = v20;
		*v20 = 0;
		return buffer + 1;
	}
LABEL_61:
	v18 = 0x1518682;
	v19 = 0xE272D3A;
	while (1) {
		if (v18 > 0x3112A9D2) {
			if (v18 > 0x97FA9C76) {
				switch (v18) {
				case 0xA2883D38:
					*v14 = RotateRight8(*v14, v16);
					v19 = 0xA4E5D073;
					break;
				case 0xAA658ADF:
					v19 = 0xFACB37;
					v15 = 0x94D049BB133111EBui64 * ((v15 >> 27) ^ v15);
					break;
				case 0xAA9F41E8:
					v19 = 0x8177CD0;
					v16 = (BYTE)(v15 ^ (v15 >> 31));
					break;
				case 0xB2BB2632:
					v19 = 0x7CDF9DB0;
					v16 = (BYTE)((v15 ^ (v15 >> 57)) & 0x11);
					break;
				}
			}
			else {
				switch (v18) {
				case 0x97FA9C76:
					v19 = 0xAEB82341;
					v16 = (BYTE)(v15 ^ (v15 >> 31));
					break;
				case 0x32728273u:
					v19 = 0x96B5DC3C;
					v15 = 0x94D049BB133111EBui64 * ((v15 >> 30) ^ v15);
					break;
				case 0x33EB2B4Cu:
					v15 = 0xBF58476D1CE4E5B9ui64 * ((v15 >> 30) ^ v15);
					v19 = 0x998EA193;
					break;
				case 0x5F50D735u:
					v17 += 0x56FB4ADEBBC33BEi64;
					v19 = 0x914F0AC;
					v15 = v17;
					break;
				case 0x7C6B3BBAu:
					v17 -= 0x40A7B892E31B1A47i64;
					v19 = 0x1BE47F22;
					v15 = v17;
					break;
				}
			}
		}
		else if (v18 == 823306706) {
			v17 -= 0x1840D2D01DCD60E3i64;
			v19 = 0xDA4ABCC0;
			v15 = v17;
		}
		else if (v18 > 0x4582CC4) {
			switch (v18) {
			case 0x66DED4Bu:
				++v14;
			LABEL_107:
				if (!*v14)
				{
					*buffer = 0;
					return buffer + 1;
				}
				goto LABEL_61;
			case 0xDCD7EDCu:
				*v14 = RotateLeft8(*v14, v16);
				v19 = 0xAB8EE291;
				break;
			case 0x14BFC364u:
				v15 = 0x55F68AAB3374F85i64 * ((v15 >> 47) ^ v15);
				v19 = 0xDA78DE3;
				break;
			case 0x1589813Au:
				v19 = 0x11D181FF;
				v15 = 0x9E3779B97F4A7C15ui64 * ((v15 >> 27) ^ v15);
				break;
			}
		}
		else {
			switch (v18) {
			case 0x4582CC4u:
				v19 = 0x59965B17;
				v15 = 0x7E55A95CD96D727i64 * ((v15 >> 30) ^ v15);
				break;
			case 0x7CA212u:
				v19 = 0xC4F3A9EE;
				v16 = (BYTE)((v15 ^ (v15 >> 31)) & 0x37);
				break;
			case 0x1518682u:
				v17 -= 0x61C8864680B583EBi64;
				v19 = 0x32BAADCE;
				v15 = v17;
				break;
			case 0x2134F43u:
				--v14;
				v19 = 0x7621218;
				break;
			case 0x2136ED6u:
				v19 = 0x427A3A71;
				v15 = 0x4FC09639E4BD6AA6i64 * ((v15 >> 27) ^ v15);
				break;
			case 0x359A2ECu:
				v19 = 0x91DDB2A3;
				v15 = 0x2137F038D1FB387i64 * ((v15 >> 9) ^ v15);
				break;
			}
		}
		v18 ^= v19;
		if (v18 == 0xAFB12217)
			goto LABEL_107;
	}
}

namespace {
	int rawdecryptcw(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		LPVOID buffer{};
		SIZE_T bufferSize{};

		if (!utils::ReadFileNotAlign(argv[2], buffer, bufferSize, true)) {
			std::cerr << "Can't read " << argv[2] << "\n";
			return tool::BASIC_ERROR;
		}

		BYTE* buff = cw::DecryptRawBuffer((BYTE*)buffer);

		std::cout 
			<< "Buffer:\n"
			<< (LPCCH)buff << "\n";

		std::free(buffer);

		return tool::OK;
	} 

	int cwtestchecksum(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		std::vector<std::filesystem::path> paths{};
		hashutils::ReadDefaultFile();

		utils::GetFileRecurse(argv[2], paths, [](const std::filesystem::path& p) {
			auto s = p.string();
			return s.ends_with(".gscc");
		});

		std::unordered_map<UINT32, std::vector<UINT64>> data{};

		for (const auto& p : paths) {
			LPVOID buffer{};
			LPVOID bufferAlign{};
			size_t size{};
			size_t sizeAlign{};
			if (!utils::ReadFileAlign(p, buffer, bufferAlign, size, sizeAlign)) {
				std::cerr << "Can't read " << p << "\n";
				continue;
			}

			auto* header = reinterpret_cast<tool::gsc::T9GSCOBJ*>(bufferAlign);

			if (!header->exports_count) {
				continue;
			}

			auto* exports = reinterpret_cast<tool::gsc::T8GSCExport*>(&header->magic[header->exports_tables]);

			if (exports->flags != tool::gsc::T9GSCExportFlags::T9_EF_CLASS_VTABLE) {
				std::cerr << "Bad first element : " << hashutils::ExtractTmp("function", exports->name) << " for " << p << "\n";
				continue;
			}

			data[exports->name].emplace_back(header->name);
		}

		for (auto& [key, vec] : data) {
			if (vec.size() > 1) {
				std::cout << hashutils::ExtractTmp("function", key) << ": \n";

				for (const auto& p : vec) {
					std::cout << hashutils::ExtractTmpPath("script", p) << "\n";
				}

			}
		}

		std::cout << "done\n";

		return tool::OK;
	}


	int ps4reader100(Process& _, int argc, const char* argv[]) {
		if (argc < 4) {
			return tool::BAD_USAGE;
		}

		auto ipd = argv[2];
		auto outdir = argv[3];

		libdebug::PS4DBG ps4{ ipd };
		ps4.Connect();

		ps4.Notify(210, std::format("Atian Tools {}", actsinfo::VERSION));

		auto procList = ps4.GetProcessList();
		auto proc = procList.FindProcess("eboot.bin");

		if (!proc) {
			std::cerr << "Can't find eboot.bin\n";
			ps4.Disconnect();
			return tool::BASIC_ERROR;
		}

		auto pid = proc->pid;

		auto entries = ps4.GetProcessMaps(proc->pid);
		uint64_t base = 0;

		for (const auto& entry : entries.entries) {
			if (entry->prot == 5) {
				std::cout << "executable base : 0x" << std::hex << entry->start << std::endl;
				base = entry->start;
				break;
			}
		}

		if (!base) {
			std::cerr << "Can't find executable base\n";
			ps4.Disconnect();
			return tool::BASIC_ERROR;
		}

		struct XAssetPool {
			uintptr_t pool; // void*
			unsigned int itemSize;
			int itemCount;
			bool isSingleton;
			int itemAllocCount;
			void* freeHead;
		};

		struct ScriptParseTree {
			UINT64 name;
			uintptr_t unk8;
			uintptr_t buffer; // GSC_OBJ*
			int len;
		};

		auto bytes = ps4.ReadMemory(pid, base + 0x5551D60 + sizeof(XAssetPool) * 67, sizeof(XAssetPool));

		auto* pool = reinterpret_cast<XAssetPool*>(&bytes[0]);

		auto val = pool->pool;
		auto count = pool->itemAllocCount;

		std::cout << std::hex << val << " : " << std::dec << count << "\n";

		auto poolBytes = ps4.ReadMemory(pid, val, count * sizeof(ScriptParseTree));

		auto* scriptEntry = reinterpret_cast<ScriptParseTree*>(&poolBytes[0]);

		hashutils::ReadDefaultFile();

		std::filesystem::path outdirPath{ outdir };
		auto outdirPathCStr = outdirPath.string();

		std::filesystem::create_directories(outdirPath);

		CHAR nameBuff[MAX_PATH + 1];

		std::cout 
			<< std::hex
			<< "spt ... " << pool->pool << "\n"
			<< "count . " << pool->itemAllocCount << "\n"
			<< "size .. " << pool->itemSize << "\n"
			<< "count . " << pool->itemCount << "\n"
			;

		for (size_t i = 0; i < count; i++) {
			const auto& e = scriptEntry[i];

			std::cout << std::dec << i << ": " << hashutils::ExtractTmpScript(e.name) << " -> " << std::hex << e.buffer << " " << e.len << "\n";

			if (!e.buffer || !e.len) {
				continue;
			}

			auto scriptBytes = ps4.ReadMemory(pid, e.buffer, e.len);

			sprintf_s(nameBuff, "%s/script_%llx.gscc", outdirPathCStr.c_str(), e.name);

			utils::WriteFile(nameBuff, &scriptBytes[0], scriptBytes.size());
		}


		ps4.Disconnect();




		return tool::OK;
	}


	int ps4vtable100(Process& _, int argc, const char* argv[]) {
		struct FuncEntry {
			uintptr_t func;
			std::vector<UINT16> vec{};
		};
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		auto ipd = argv[2];

		libdebug::PS4DBG ps4{ ipd };
		ps4.Connect();

		ps4.Notify(210, std::format("Atian Tools {}", actsinfo::VERSION));

		auto procList = ps4.GetProcessList();
		auto proc = procList.FindProcess("eboot.bin");

		if (!proc) {
			std::cerr << "Can't find eboot.bin\n";
			ps4.Disconnect();
			return tool::BASIC_ERROR;
		}

		auto pid = proc->pid;

		auto entries = ps4.GetProcessMaps(proc->pid);
		uint64_t base = 0;

		for (const auto& entry : entries.entries) {
			if (entry->prot == 5) {
				std::cout << "executable base : 0x" << std::hex << entry->start << std::endl;
				base = entry->start;
				break;
			}
		}

		if (!base) {
			std::cerr << "Can't find executable base\n";
			ps4.Disconnect();
			return tool::BASIC_ERROR;
		}

		auto bytes = ps4.ReadMemory(pid, base + 0x21104B0, sizeof(uintptr_t) * 0x1000);

		std::map<UINT16, FuncEntry> map{};
		std::map<uintptr_t, UINT16> maplookup{};

		auto* ptrs = reinterpret_cast<uintptr_t*>(&bytes[0]);

		for (size_t i = 0; i < 0x1000; i++) {
			const auto& func = ptrs[i];
			auto& v = maplookup[ptrs[i]];

			if (v == 0 || !i) {
				v = (UINT16)i;
			}
			auto& f = map[v];
			f.func = func - base;
			f.vec.push_back((UINT16)i);
		}

		for (const auto& [key, v] : map) {
			std::cout << std::hex << key << " sub:" << v.func;
			for (const auto op : v.vec) {
				std::cout << std::hex << ", 0x" << op;
			}
			std::cout << "\n";
		}


		ps4.Disconnect();

		return tool::OK;
	}


	int ps4setmode100(Process& _, int argc, const char* argv[]) {
		struct FuncEntry {
			uintptr_t func;
			std::vector<UINT16> vec{};
		};
		if (argc < 4) {
			return tool::BAD_USAGE;
		}

		auto ipd = argv[2];
		auto mode = atoi(argv[3]);

		if (mode < 0 || mode >= 4) {
			return tool::BAD_USAGE;
		}

		libdebug::PS4DBG ps4{ ipd };
		ps4.Connect();

		ps4.Notify(210, std::format("Atian Tools {}", actsinfo::VERSION));

		auto procList = ps4.GetProcessList();
		auto proc = procList.FindProcess("eboot.bin");

		if (!proc) {
			std::cerr << "Can't find eboot.bin\n";
			ps4.Disconnect();
			return tool::BASIC_ERROR;
		}

		auto pid = proc->pid;

		auto entries = ps4.GetProcessMaps(proc->pid);
		uint64_t base = 0;

		for (const auto& entry : entries.entries) {
			if (entry->prot == 5) {
				std::cout << "executable base : 0x" << std::hex << entry->start << std::endl;
				base = entry->start;
				break;
			}
		}

		if (!base) {
			std::cerr << "Can't find executable base\n";
			ps4.Disconnect();
			return tool::BASIC_ERROR;
		}

		constexpr auto sSessionModeState_off = 0xB594B58;

		auto bytes = ps4.ReadMemory(pid, base + sSessionModeState_off, sizeof(uint32_t));

		auto sSessionModeState = *reinterpret_cast<uint32_t*>(bytes.data());
		auto currentMode = sSessionModeState << 28 >> 28;

		std::cout << "Current mode: " << currentMode << std::hex << " (0x" << sSessionModeState << ")\n";

		*reinterpret_cast<uint32_t*>(bytes.data()) = (sSessionModeState & ~0xF) | mode & 0xF;

		ps4.WriteMemory(pid, base + sSessionModeState_off, bytes);

		auto bytes2 = ps4.ReadMemory(pid, base + sSessionModeState_off, sizeof(int));

		auto newMode = *reinterpret_cast<uint32_t*>(bytes.data()) << 28 >> 28;

		std::cout << "Current mode: " << newMode << std::hex << " (0x" << *reinterpret_cast<uint32_t*>(bytes.data()) << ")\n";

		ps4.Disconnect();

		return tool::OK;
	}
}
#ifndef CI_BUILD

ADD_TOOL("tcrccw", "", "test crc (cw)", nullptr, cwtestchecksum);
ADD_TOOL("rawdecryptcw", "", " raw decrypt (cw)", nullptr, rawdecryptcw);

ADD_TOOL("ps4_dpcw100", " [ip:port]", "dump ps4 scripts (CW)", nullptr, ps4reader100);
ADD_TOOL("ps4_vtcw100", " [ip:port]", "dump ps4 vtable (CW)", nullptr, ps4vtable100);
ADD_TOOL("ps4_sm100", " [ip:port] [mode=0,1,2,3]", "set mode (CW)", nullptr, ps4setmode100);

#endif