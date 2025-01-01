#include <includes.hpp>
#include "tools/cw/cw.hpp"
#include "tools/cw/pool_cod2020.hpp"
#include "tools/gsc.hpp"

char* cw::DecryptString(char* str) {
	// for now I'm using the DLL to call the decrypt function
	if (*reinterpret_cast<byte*>(str) == 0x8b) {
		return str + 3; // encryption ptr
	}
	if ((*str & 0xC0) == 0x80) {
		// encrypted
		str[0] = 0;
		str[1] = 0;
	}
	return str;
}

byte* cw::DecryptRawBuffer(byte* buffer) {
	byte* v14;
	uint64_t v15;
	byte v16;
	uint64_t v17;
	uint32_t v18;
	uint32_t v19;
	byte* v20 = buffer;
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
					v16 = (byte)(v15 ^ (v15 >> 31));
					break;
				case 0xB2BB2632:
					v19 = 0x7CDF9DB0;
					v16 = (byte)((v15 ^ (v15 >> 57)) & 0x11);
					break;
				}
			}
			else {
				switch (v18) {
				case 0x97FA9C76:
					v19 = 0xAEB82341;
					v16 = (byte)(v15 ^ (v15 >> 31));
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
				v16 = (byte)((v15 ^ (v15 >> 31)) & 0x37);
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
uintptr_t cw::ScanPool(Process& proc) {
	uintptr_t curr = proc.Scan("48 8D 05 ? ? ? ? 48 C1 E2 ? 48 03 D0");
	if (!curr) {
		throw std::runtime_error("Can't find xasset pool");
	}
	int32_t delta = proc.ReadMemory<int32_t>(curr + 3);
	if (!delta) {
		throw std::runtime_error("Can't find xasset pool delta");
	}
	return curr + 7 + delta;
}
int cw::InjectScriptCW(Process& proc, const char* script, const char* target, const char* replaced, std::string& notify) {

	uint64_t targetHash = hash::Hash64Pattern(target);
	uint64_t replacedHash = hash::Hash64Pattern(replaced);

	LOG_DEBUG("script: {}", script);
	LOG_DEBUG("target: {} (script_{:x})", target, targetHash);
	LOG_DEBUG("replaced: {} (script_{:x})", replaced, replacedHash);

	std::filesystem::path scriptPath = script;
	std::string scriptBuffStr{};

	if (!utils::ReadFile(scriptPath, scriptBuffStr)) {
		notify = std::format("Can't read '{}'", scriptPath.string());
		return tool::BASIC_ERROR;
	}

	void* scriptBuffer = scriptBuffStr.data();
	size_t scriptSize = scriptBuffStr.size();

	if (scriptSize < 8 || *reinterpret_cast<uint64_t*>(scriptBuffer) != cw::GSC_MAGIC) {
		notify = "Not a valid compiled Black Ops Cold War script (BAD MAGIC)";
		return tool::BASIC_ERROR;
	}

	XAssetPool sptPool{};

	uintptr_t poolLoc = cw::ScanPool(proc);

	proc.WriteLocation(std::cout << "pool: ", poolLoc) << "\n";

	if (!proc.ReadMemory(&sptPool, poolLoc + sizeof(sptPool) * cw::ASSET_TYPE_SCRIPTPARSETREE, sizeof(sptPool))) {
		notify = "Can't read SPT pool";
		return tool::BASIC_ERROR;
	}

	auto entries = std::make_unique<ScriptParseTree[]>(sptPool.itemAllocCount);

	if (!proc.ReadMemory(&entries[0], sptPool.pool, sptPool.itemAllocCount * sizeof(entries[0]))) {
		notify = "Can't read SPT pool entries";
		return tool::BASIC_ERROR;
	}

	auto* scriptEntry = reinterpret_cast<ScriptParseTree*>(&entries[0]);

	auto* end = scriptEntry + sptPool.itemAllocCount;

	auto* targetEntry = std::find_if(scriptEntry, end, [targetHash](const auto& ent) { return ent.name == targetHash; });

	uintptr_t replacedEntry = 0;
	ScriptParseTree* replacedEntryH = NULL;

	for (size_t i = 0; i < sptPool.itemAllocCount; i++) {
		if (scriptEntry[i].name == replacedHash) {
			replacedEntryH = scriptEntry + i;
			replacedEntry = sptPool.pool + sizeof(*scriptEntry) * i;
			break;
		}
	}

	if (targetEntry == end) {
		notify = std::format("Can't find target script '{}'", target);
		return tool::BASIC_ERROR;
	}
	if (!replacedEntry) {
		notify = std::format("Can't find replaced script '{}'", replaced);
		return tool::BASIC_ERROR;
	}
	tool::gsc::T9GSCOBJ replacerScriptHeader{};
	if (!proc.ReadMemory(&replacerScriptHeader, replacedEntryH->buffer, sizeof(tool::gsc::T9GSCOBJ))) {
		notify = "Can't read replacer header";
		return tool::BASIC_ERROR;
	}

	tool::gsc::T9GSCOBJ scriptHeader{};
	if (!proc.ReadMemory(&scriptHeader, targetEntry->buffer, sizeof(tool::gsc::T9GSCOBJ))) {
		notify = "Can't read target header";
		return tool::BASIC_ERROR;
	}
	auto includesOffset = targetEntry->buffer + scriptHeader.includes_table;
	auto includes = std::make_unique<uint64_t[]>(scriptHeader.includes_count);
	if (!proc.ReadMemory(&includes[0], includesOffset, sizeof(uint64_t) * scriptHeader.includes_count)) {
		notify = "Can't read includes";
		return tool::BASIC_ERROR;
	}

	auto includesEnd = &includes[0] + scriptHeader.includes_count;

	LOG_DEBUG("{} -> {:x} ({})", target, targetEntry->buffer, targetEntry->name);
	LOG_DEBUG("{} -> {:x} ({})", replaced, replacedEntryH->buffer, replacedEntryH->name);

	auto hookId = std::find(&includes[0], includesEnd, replacedHash);
	if (hookId == includesEnd) {
		// need to add the include

		// insert the new include
		if (!proc.WriteMemory(includesOffset + sizeof(uint64_t) * scriptHeader.includes_count, &replacedHash, sizeof(replacedHash))) {
			notify = "Error when patching includes";
			return tool::BASIC_ERROR;
		}

		// correct the include count
		uint16_t newIncludeCount = scriptHeader.includes_count + 1;
		if (!proc.WriteMemory(targetEntry->buffer + offsetof(tool::gsc::T9GSCOBJ, includes_count), &newIncludeCount, sizeof(newIncludeCount))) {
			notify = "Error when patching includes count";
			return tool::BASIC_ERROR;
		}

		if (scriptHeader.name == targetHash) {
			LOG_DEBUG("Hook inserted into {}", target);
		}
		else {
			LOG_DEBUG("Hook inserted into script_{:x}", scriptHeader.name);
		}
	}

	// fixup crc & name
	reinterpret_cast<tool::gsc::T9GSCOBJ*>(scriptBuffer)->crc = replacerScriptHeader.crc;
	reinterpret_cast<tool::gsc::T9GSCOBJ*>(scriptBuffer)->name = replacerScriptHeader.name;

	auto loc = proc.AllocateMemory(scriptSize + 0xF, PAGE_READWRITE);

	if (!loc) {
		notify = "Can't allocate memory";
		return tool::BASIC_ERROR;
	}

	auto locAligned = (loc + 0xF) & ~0xF;
	LOG_DEBUG("Allocating script at 0x{:x} (0x{:x})", locAligned, loc);

	if (!proc.WriteMemory(locAligned, scriptBuffer, scriptSize)) {
		auto err = GetLastError();
		notify = std::format("Error when writing script: 0x{:x}", err);
		proc.FreeMemory(loc, scriptSize + 0xF);
		return tool::BASIC_ERROR;
	}

	if (!proc.WriteMemory(replacedEntry + offsetof(ScriptParseTree, buffer), &locAligned, sizeof(locAligned))) {
		notify = std::format("Error when patching SPT entry to 0x{:x}", locAligned);
		proc.FreeMemory(loc, scriptSize + 0xF);
		return tool::BASIC_ERROR;
	}

	notify = std::format("{} injected at {:x}", script, locAligned);
	return tool::OK;
}
int cw::InjectScriptCWAlpha(Process& proc, const char* script, const char* target, const char* replaced, std::string& notify) {
	uint64_t targetHash = hash::Hash64Pattern(target);
	uint64_t replacedHash = hash::Hash64Pattern(replaced);

	LOG_DEBUG("script: {}", script);
	LOG_DEBUG("target: {} (script_{:x})", target, targetHash);
	LOG_DEBUG("replaced: {} (script_{:x})", replaced, replacedHash);

	std::filesystem::path scriptPath = script;
	std::string scriptBuffStr{};

	if (!utils::ReadFile(scriptPath, scriptBuffStr)) {
		notify = std::format("Can't read '{}'", scriptPath.string());
		return tool::BASIC_ERROR;
	}

	void* scriptBuffer = scriptBuffStr.data();
	size_t scriptSize = scriptBuffStr.size();

	if (scriptSize < 8 || *reinterpret_cast<uint64_t*>(scriptBuffer) != cw::GSC_MAGIC_37) {
		notify = "Not a valid compiled Black Ops Cold War script (BAD MAGIC)";
		return tool::BASIC_ERROR;
	}

	XAssetPool sptPool{};

	uintptr_t poolLoc = cw::ScanPool(proc);

	proc.WriteLocation(std::cout << "pool: ", poolLoc) << "\n";

	if (!proc.ReadMemory(&sptPool, poolLoc + sizeof(sptPool) * cw::alpha::ASSET_TYPE_SCRIPTPARSETREE, sizeof(sptPool))) {
		notify = "Can't read SPT pool";
		return tool::BASIC_ERROR;
	}

	auto entries = std::make_unique<ScriptParseTreeAlpha[]>(sptPool.itemAllocCount);

	if (!proc.ReadMemory(&entries[0], sptPool.pool, sptPool.itemAllocCount * sizeof(entries[0]))) {
		notify = "Can't read SPT pool entries";
		return tool::BASIC_ERROR;
	}

	auto* scriptEntry = reinterpret_cast<ScriptParseTreeAlpha*>(&entries[0]);

	auto* end = scriptEntry + sptPool.itemAllocCount;

	auto* targetEntry = std::find_if(scriptEntry, end, [targetHash](const auto& ent) { return ent.name == targetHash; });

	uintptr_t replacedEntry = 0;
	ScriptParseTreeAlpha* replacedEntryH = NULL;

	for (size_t i = 0; i < sptPool.itemAllocCount; i++) {
		if (scriptEntry[i].name == replacedHash) {
			replacedEntryH = scriptEntry + i;
			replacedEntry = sptPool.pool + sizeof(*scriptEntry) * i;
			break;
		}
	}

	if (targetEntry == end) {
		notify = std::format("Can't find target script '{}'", target);
		return tool::BASIC_ERROR;
	}
	if (!replacedEntry) {
		notify = std::format("Can't find replaced script '{}'", replaced);
		return tool::BASIC_ERROR;
	}
	tool::gsc::T937GSCOBJ replacerScriptHeader{};
	if (!proc.ReadMemory(&replacerScriptHeader, replacedEntryH->buffer, sizeof(tool::gsc::T937GSCOBJ))) {
		notify = "Can't read replacer header";
		return tool::BASIC_ERROR;
	}

	tool::gsc::T937GSCOBJ scriptHeader{};
	if (!proc.ReadMemory(&scriptHeader, targetEntry->buffer, sizeof(tool::gsc::T937GSCOBJ))) {
		notify = "Can't read target header";
		return tool::BASIC_ERROR;
	}
	auto includesOffset = targetEntry->buffer + scriptHeader.includes_table;
	auto includes = std::make_unique<uint64_t[]>(scriptHeader.includes_count);
	if (!proc.ReadMemory(&includes[0], includesOffset, sizeof(uint64_t) * scriptHeader.includes_count)) {
		notify = "Can't read includes";
		return tool::BASIC_ERROR;
	}

	auto includesEnd = &includes[0] + scriptHeader.includes_count;

	LOG_DEBUG("{} -> {:x} ({})", target, targetEntry->buffer, targetEntry->name);
	LOG_DEBUG("{} -> {:x} ({})", replaced, replacedEntryH->buffer, replacedEntryH->name);

	auto hookId = std::find(&includes[0], includesEnd, replacedHash);
	if (hookId == includesEnd) {
		// need to add the include

		// insert the new include
		if (!proc.WriteMemory(includesOffset + sizeof(uint64_t) * scriptHeader.includes_count, &replacedHash, sizeof(replacedHash))) {
			notify = "Error when patching includes";
			return tool::BASIC_ERROR;
		}

		// correct the include count
		uint16_t newIncludeCount = scriptHeader.includes_count + 1;
		if (!proc.WriteMemory(targetEntry->buffer + offsetof(tool::gsc::T937GSCOBJ, includes_count), &newIncludeCount, sizeof(newIncludeCount))) {
			notify = "Error when patching includes count";
			return tool::BASIC_ERROR;
		}

		if (scriptHeader.name == targetHash) {
			LOG_DEBUG("Hook inserted into {}", target);
		}
		else {
			LOG_DEBUG("Hook inserted into script_{:x}", scriptHeader.name);
		}
	}

	// fixup crc & name
	reinterpret_cast<tool::gsc::T937GSCOBJ*>(scriptBuffer)->crc = replacerScriptHeader.crc;
	reinterpret_cast<tool::gsc::T937GSCOBJ*>(scriptBuffer)->name = replacerScriptHeader.name;

	auto loc = proc.AllocateMemory(scriptSize + 0xF, PAGE_READWRITE);

	if (!loc) {
		notify = "Can't allocate memory";
		return tool::BASIC_ERROR;
	}

	auto locAligned = (loc + 0xF) & ~0xF;
	LOG_DEBUG("Allocating script at 0x{:x} (0x{:x})", locAligned, loc);

	if (!proc.WriteMemory(locAligned, scriptBuffer, scriptSize)) {
		auto err = GetLastError();
		notify = std::format("Error when writing script: 0x{:x}", err);
		proc.FreeMemory(loc, scriptSize + 0xF);
		return tool::BASIC_ERROR;
	}

	if (!proc.WriteMemory(replacedEntry + offsetof(ScriptParseTreeAlpha, buffer), &locAligned, sizeof(locAligned))) {
		notify = std::format("Error when patching SPT entry to 0x{:x}", locAligned);
		proc.FreeMemory(loc, scriptSize + 0xF);
		return tool::BASIC_ERROR;
	}

	notify = std::format("{} injected at {:x}", script, locAligned);
	return tool::OK;
}

namespace {
	int rawdecryptcw(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		void* buffer{};
		size_t bufferSize{};

		if (!utils::ReadFileNotAlign(argv[2], buffer, bufferSize, true)) {
			std::cerr << "Can't read " << argv[2] << "\n";
			return tool::BASIC_ERROR;
		}

		byte* buff = cw::DecryptRawBuffer((byte*)buffer);

		std::cout 
			<< "Buffer:\n"
			<< (const char*)buff << "\n";

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

		std::unordered_map<uint32_t, std::vector<uint64_t>> data{};

		for (const auto& p : paths) {
			void* buffer{};
			void* bufferAlign{};
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

		ps4.Notify(210, std::format("Atian Tools {}", core::actsinfo::VERSION));

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
			uint64_t name;
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

		char nameBuff[MAX_PATH + 1];

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
			std::vector<uint16_t> vec{};
		};
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		auto ipd = argv[2];

		libdebug::PS4DBG ps4{ ipd };
		ps4.Connect();

		ps4.Notify(210, std::format("Atian Tools {}", core::actsinfo::VERSION));

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

		std::map<uint16_t, FuncEntry> map{};
		std::map<uintptr_t, uint16_t> maplookup{};

		auto* ptrs = reinterpret_cast<uintptr_t*>(&bytes[0]);

		for (size_t i = 0; i < 0x1000; i++) {
			const auto& func = ptrs[i];
			auto& v = maplookup[ptrs[i]];

			if (v == 0 || !i) {
				v = (uint16_t)i;
			}
			auto& f = map[v];
			f.func = func - base;
			f.vec.push_back((uint16_t)i);
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
			std::vector<uint16_t> vec{};
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

		ps4.Notify(210, std::format("Atian Tools {}", core::actsinfo::VERSION));

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
	int cwdllgt(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}
		const char* gametype{ argv[2] };
		const char* map{ argc < 4 ? nullptr : argv[3] };

		ProcessModule& powrprof{ proc["powrprof.dll"] };

		if (!powrprof) {
			LOG_ERROR("Can't find DLL");
			return tool::BASIC_ERROR;
		}
		LOG_INFO("Set gametype {}...", gametype);
		{
			ProcessModuleExport& ACTS_EXPORT_SetLobbyGameType{ powrprof["ACTS_EXPORT_SetLobbyGameType"] };

			if (!ACTS_EXPORT_SetLobbyGameType) {
				LOG_ERROR("Can't find ACTS_EXPORT_SetLobbyGameType");
				return tool::BASIC_ERROR;
			}

			size_t gtlen;
			uintptr_t gtptr{ proc.AllocateString(gametype, &gtlen) };
			if (!gtptr) {
				LOG_ERROR("Can't allocate string");
				return tool::BASIC_ERROR;
			}

			HANDLE thr{ proc.Exec(ACTS_EXPORT_SetLobbyGameType.m_location, gtptr) };

			if (thr == INVALID_HANDLE_VALUE || !thr) {
				std::cerr << "Can't call ACTS_EXPORT_SetLobbyGameType\n";
				proc.FreeMemory(gtptr, gtlen);
				return tool::BASIC_ERROR;
			}

			WaitForSingleObject(thr, INFINITE);
			CloseHandle(thr);
			proc.FreeMemory(gtptr, gtlen);

		}

		if (map) {
			LOG_INFO("Set map {}...", map);
			ProcessModuleExport& ACTS_EXPORT_SetLobbyMap{ powrprof["ACTS_EXPORT_SetLobbyMap"] };
			
			if (!ACTS_EXPORT_SetLobbyMap) {
				LOG_ERROR("Can't find ACTS_EXPORT_SetLobbyMap");
				return tool::BASIC_ERROR;
			}

			size_t mplen;
			uintptr_t mpptr{ proc.AllocateString(map, &mplen) };
			if (!mpptr) {
				LOG_ERROR("Can't allocate string");
				return tool::BASIC_ERROR;
			}

			HANDLE thr{ proc.Exec(ACTS_EXPORT_SetLobbyMap.m_location, mpptr) };

			if (thr == INVALID_HANDLE_VALUE || !thr) {
				std::cerr << "Can't call ACTS_EXPORT_SetLobbyMap\n";
				proc.FreeMemory(mpptr, mplen);
				return tool::BASIC_ERROR;
			}

			WaitForSingleObject(thr, INFINITE);
			CloseHandle(thr);
			proc.FreeMemory(mpptr, mplen);
		}
		LOG_INFO("Done");

		return tool::OK;
	}
	
	int cwdllac(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}
		const char* action{ argv[2] };
		const char* value{ argc > 3 ? argv[3] : nullptr };

		ProcessModule& powrprof{ proc["powrprof.dll"] };

		if (!powrprof) {
			LOG_ERROR("Can't find DLL");
			return tool::BASIC_ERROR;
		}

		ProcessModuleExport& exp{ powrprof[action] };

		if (!exp) {
			LOG_ERROR("Can't find {}", action);
			return tool::BASIC_ERROR;
		}

		size_t vllen{};
		uintptr_t vlptr{};
		if (value) {
			vlptr = proc.AllocateString(value, &vllen);
			if (!vlptr) {
				LOG_ERROR("Can't allocate string");
				return tool::BASIC_ERROR;
			}
		}

		HANDLE thr{ proc.Exec(exp.m_location, vlptr) };

		if (thr == INVALID_HANDLE_VALUE || !thr) {
			LOG_ERROR("Can't call {}", action);
			proc.FreeMemory(vlptr, vllen);
			return tool::BASIC_ERROR;
		}

		WaitForSingleObject(thr, INFINITE);
		CloseHandle(thr);
		proc.FreeMemory(vlptr, vllen);

		LOG_INFO("Done");

		return tool::OK;
	}
}

ADD_TOOL(cwdllgt, "cw", " [gametype] (map)", "set gametype", L"BlackOpsColdWar.exe", cwdllgt);
ADD_TOOL(cwdllac, "cw", " [action] (param str)", "run dll action", L"BlackOpsColdWar.exe", cwdllac);

#ifndef CI_BUILD

ADD_TOOL(tcrccw, "cw", "", "test crc (cw)", nullptr, cwtestchecksum);
ADD_TOOL(rawdecryptcw, "cw", "", " raw decrypt (cw)", nullptr, rawdecryptcw);

ADD_TOOL(ps4_dpcw100, "cw", " [ip:port]", "dump ps4 scripts (CW)", nullptr, ps4reader100);
ADD_TOOL(ps4_vtcw100, "cw", " [ip:port]", "dump ps4 vtable (CW)", nullptr, ps4vtable100);
ADD_TOOL(ps4_sm100, "cw", " [ip:port] [mode=0,1,2,3]", "set mode (CW)", nullptr, ps4setmode100);

#endif