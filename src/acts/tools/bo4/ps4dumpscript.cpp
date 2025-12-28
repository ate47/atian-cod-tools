#include <includes.hpp>
#include <tools/bo4/dump.hpp>
#include <games/bo4/pool.hpp>

using namespace games::bo4;

struct XAssetPoolEntry {
	uintptr_t pool;
	UINT32 itemSize;
	INT32 itemCount;
	BYTE isSingleton;
	INT32 itemAllocCount;
	uintptr_t freeHead;
};

struct FuncEntry {
	uintptr_t func;
	std::vector<UINT16> vec{};
};

static int ps4reader(Process& _, int argc, const char* argv[]) {
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



	auto bytes = ps4.ReadMemory(pid, base + 0x4A303C0 + sizeof(XAssetPoolEntry) * pool::ASSET_TYPE_SCRIPTPARSETREE, sizeof(XAssetPoolEntry));

	auto* pool = reinterpret_cast<XAssetPoolEntry*>(&bytes[0]);

	auto val = pool->pool;
	auto count = pool->itemAllocCount;

	std::cout << std::hex << val << " : " << std::dec << count << "\n";

	auto poolBytes = ps4.ReadMemory(pid, val, count * sizeof(tool::dump::T8ScriptParseTreeEntry));

	auto* scriptEntry = reinterpret_cast<tool::dump::T8ScriptParseTreeEntry*>(&poolBytes[0]);

	hashutils::ReadDefaultFile();

	std::filesystem::path outdirPath{ outdir };
	auto outdirPathCStr = outdirPath.string();

	std::filesystem::create_directories(outdirPath);

	CHAR nameBuff[MAX_PATH + 1];

	for (size_t i = 0; i < count; i++) {
		const auto& e = scriptEntry[i];

		std::cout << std::dec << i << ": " << hashutils::ExtractTmpScript(e.name) << " -> " << std::hex << e.buffer << " " << e.size << "\n";

		auto scriptBytes = ps4.ReadMemory(pid, e.buffer, e.size);

		sprintf_s(nameBuff, "%s/script_%llx.gscc", outdirPathCStr.c_str(), e.name);

		utils::WriteFile(nameBuff, &scriptBytes[0], scriptBytes.size());
	}


	ps4.Disconnect();




	return tool::OK;
}


static int ps4vtable(Process& _, int argc, const char* argv[]) {
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

	// 1C2A980 gVmOpJumpTable

	auto bytes = ps4.ReadMemory(pid, base + 0x1C2A980, sizeof(uintptr_t) * 0x1000);

	std::map<UINT16, FuncEntry> map{};
	std::map<uintptr_t, UINT16> maplookup{};

	auto* ptrs = reinterpret_cast<uintptr_t*>(&bytes[0]);

	for (size_t i = 0; i < 0x1000; i++) {
		const auto& func = ptrs[i];
		auto& v = maplookup[ptrs[i]];

		if (v == 0 || !i) {
			v = (UINT16) i;
		}
		auto& f = map[v];
		f.func = func - base;
		f.vec.push_back((UINT16)i);
	}

	for (const auto& [key, v] : map) {
		std::cout << std::hex << key << " sub:" << v.func  << ": {";
		for (const auto op : v.vec) {
			std::cout << std::hex << " " << op;
		}
		std::cout << " }\n";
	}


	ps4.Disconnect();




	return tool::OK;
}

ADD_TOOL(ps4_dp, "bo4", " [ip:port]", "dump ps4 scripts", nullptr, ps4reader);
ADD_TOOL(ps4_vt, "bo4", " [ip:port]", "dump ps4 vtable", nullptr, ps4vtable);