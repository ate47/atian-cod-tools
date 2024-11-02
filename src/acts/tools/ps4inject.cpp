#include <includes.hpp>
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include <pool.hpp>

namespace {
	struct XAssetPoolEntry {
		uintptr_t pool;
		uint32_t itemSize;
		int32_t itemCount;
		byte isSingleton;
		int32_t itemAllocCount;
		uintptr_t freeHead;
	};
	struct T9ScriptParseTree {
		uint64_t name;
		uintptr_t unk8;
		uintptr_t buffer; // GSC_OBJ*
		int len;
	};

	int ps4inject(Process& _, int argc, const char* argv[]) {
		if (argc < 6) {
			return tool::BAD_USAGE;
		}

		auto ipd = argv[2];
		auto script = argv[3];
		auto target = argv[4];
		auto replaced = argv[5];

		auto targetHash = hash::Hash64Pattern(target);
		auto replacedHash = hash::Hash64Pattern(replaced);

		std::cout <<
			"Ip: " << ipd << "\n"
			"script: " << script << "\n"
			"target: " << target << " (script_" << std::hex << targetHash << ")\n"
			"replaced: " << replaced << " (script_" << std::hex << replacedHash << ")\n"
			;

		std::filesystem::path scriptPath = script;
		void* scriptBuffer = NULL;
		size_t scriptSize = 0;

		if (!utils::ReadFileNotAlign(scriptPath, scriptBuffer, scriptSize, false)) {
			std::cerr << "Can't read '" << scriptPath.string() << "'\n";
			return tool::BASIC_ERROR;
		}

		if (scriptSize >= (1ull << ((sizeof(uint32_t) << 3) - 1))) {
			std::cerr << "Size too large: " << std::hex << scriptSize << "/" << (1ull << ((sizeof(uint32_t) << 3) - 1)) << "\n";
			std::free(scriptBuffer);
			return tool::BASIC_ERROR;
		}

		libdebug::PS4DBG ps4{ ipd };
		try {
			ps4.Connect();
			//ps4.Notify(210, std::format("Atian Tools {}", actsinfo::VERSION));

			auto procList = ps4.GetProcessList();
			auto proc = procList.FindProcess("eboot.bin");

			if (!proc) {
				ps4.Notify(222, "Please start the game");
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
				std::free(scriptBuffer);
				return tool::BASIC_ERROR;
			}



			uintptr_t poolIdx = base + 0x4A303C0 + sizeof(XAssetPoolEntry) * pool::ASSET_TYPE_SCRIPTPARSETREE;
			auto bytes = ps4.ReadMemory(pid, poolIdx, sizeof(XAssetPoolEntry));

			auto* pool = reinterpret_cast<XAssetPoolEntry*>(bytes.data());

			auto val = pool->pool;
			auto count = pool->itemAllocCount;

			std::cout << std::hex << "SPT: 0x" << val << " : " << std::dec << count << "\n";

			auto poolBytes = ps4.ReadMemory(pid, val, count * sizeof(tool::dump::T8ScriptParseTreeEntry));

			auto* scriptEntry = reinterpret_cast<tool::dump::T8ScriptParseTreeEntry*>(&poolBytes[0]);

			auto* end = scriptEntry + count;

			auto* targetEntry = std::find_if(scriptEntry, end, [targetHash](const auto& ent) { return ent.name == targetHash; });

			uintptr_t replacedEntry = 0;
			tool::dump::T8ScriptParseTreeEntry* replacedEntryH = NULL;

			for (size_t i = 0; i < count; i++) {
				if (scriptEntry[i].name == replacedHash) {
					replacedEntryH = scriptEntry + i;
					replacedEntry = val + sizeof(*scriptEntry) * i;
					break;
				}
			}

			if (targetEntry == end) {
				auto err = std::format("Can't find target script '{}'", target);
				ps4.Notify(222, err);
				std::cerr << err << "\n";
				ps4.Disconnect();
				std::free(scriptBuffer);
				return tool::BASIC_ERROR;
			}
			if (!replacedEntry) {
				auto err = std::format("Can't find replaced script '{}'", replaced);
				std::cerr << err << "\n";
				ps4.Notify(222, err);
				ps4.Disconnect();
				std::free(scriptBuffer);
				return tool::BASIC_ERROR;
			}
			auto replacerScriptHeaderVal = ps4.ReadMemory(pid, replacedEntryH->buffer, sizeof(tool::gsc::T8GSCOBJ));
			auto replacerScriptHeader = reinterpret_cast<tool::gsc::T8GSCOBJ*>(replacerScriptHeaderVal.data());

			auto targetScriptHeader = ps4.ReadMemory(pid, targetEntry->buffer, sizeof(tool::gsc::T8GSCOBJ));
			auto scriptHeader = reinterpret_cast<tool::gsc::T8GSCOBJ*>(targetScriptHeader.data());
			auto includesOffset = targetEntry->buffer + scriptHeader->include_offset;
			auto targetScriptIncludes = ps4.ReadMemory(pid, includesOffset, sizeof(uint64_t) * scriptHeader->include_count);
			auto includes = reinterpret_cast<uint64_t*>(targetScriptIncludes.data());
			auto includesEnd = includes + scriptHeader->include_count;

			std::cout << target << " -> " << std::hex << targetEntry->buffer << "(" << targetEntry->name << ")\n";
			std::cout << replaced << " -> " << std::hex << replacedEntryH->buffer << "(" << replacedEntryH->name << ")\n";

			auto hookId = std::find(includes, includesEnd, replacedHash);
			if (hookId == includesEnd) {
				// need to add the include

				// insert the new include
				std::vector<byte> includeVal{};
				includeVal.insert(includeVal.begin(), (uint8_t*)&replacedHash, (uint8_t*)(&replacedHash + 1));
				ps4.WriteMemory(pid, includesOffset + sizeof(uint64_t) * scriptHeader->include_count, includeVal);

				includeVal.clear();

				// correct the include count
				uint16_t newIncludeCount = scriptHeader->include_count + 1;
				includeVal.insert(includeVal.begin(), (uint8_t*)&newIncludeCount, (uint8_t*)(&newIncludeCount + 1));
				ps4.WriteMemory(pid, targetEntry->buffer + offsetof(tool::gsc::T8GSCOBJ, include_count), includeVal);
				std::cout << "Hook inserted into ";
				if (scriptHeader->name == targetHash) {
					std::cout << target;
				}
				else {
					std::cout << std::hex << scriptHeader->name;
				}
				std::cout << "\n";
			}

		
			auto loc = ps4.AllocateMemory(pid, (int) scriptSize + 15);
			auto locAligned = (loc + 15) & ~15;

			if (!loc) {
				std::cerr << "Can't allocate memory\n";
			}
			else {
				std::cout << "Allocating script...\n";

				// fixup crc & name
				reinterpret_cast<tool::gsc::T8GSCOBJ*>(scriptBuffer)->crc = replacerScriptHeader->crc;
				reinterpret_cast<tool::gsc::T8GSCOBJ*>(scriptBuffer)->name = replacerScriptHeader->name;

				std::vector<uint8_t> scriptBytes{};
				scriptBytes.insert(scriptBytes.begin(), (uint8_t*)scriptBuffer, (uint8_t*)scriptBuffer + scriptSize);
				ps4.WriteMemory(pid, locAligned, scriptBytes);

				std::cout << "Script allocated at " << std::hex << locAligned << "\n";

				std::vector<uint8_t> entryBytes{};
				entryBytes.insert(entryBytes.begin(), (uint8_t*)&locAligned, (uint8_t*)(&locAligned + 1));
				ps4.WriteMemory(pid, replacedEntry + offsetof(tool::dump::T8ScriptParseTreeEntry, buffer), entryBytes);

				ps4.Notify(222, std::format("{} injected", script));
			}
		}
		catch (const char* res) {
			std::cerr << res << "\n";
		}
		ps4.Disconnect();
		std::free(scriptBuffer);



		return tool::OK;
	}

	int ps4t8cee(Process& _, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		auto ipd = argv[2];

		libdebug::PS4DBG ps4{ ipd };
		try {
			ps4.Connect();
			//ps4.Notify(210, std::format("Atian Tools {}", actsinfo::VERSION));

			auto procList = ps4.GetProcessList();
			auto proc = procList.FindProcess("eboot.bin");

			if (!proc) {
				ps4.Notify(222, "Please start the game");
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



			uintptr_t poolIdx = base + 0x4A303C0 + sizeof(XAssetPoolEntry) * pool::ASSET_TYPE_SCRIPTPARSETREE;
			auto bytes = ps4.ReadMemory(pid, poolIdx, sizeof(XAssetPoolEntry));

			auto* pool = reinterpret_cast<XAssetPoolEntry*>(bytes.data());

			auto val = pool->pool;
			auto count = pool->itemAllocCount;

			std::cout << std::hex << "SPT: 0x" << val << " : " << std::dec << count << "\n";

			auto poolBytes = ps4.ReadMemory(pid, val, count * sizeof(tool::dump::T8ScriptParseTreeEntry));

			auto* scriptEntry = reinterpret_cast<tool::dump::T8ScriptParseTreeEntry*>(&poolBytes[0]);

			auto* end = scriptEntry + count;

			constexpr uint64_t zm_utility = hash::Hash64("scripts/zm_common/zm_utility");
			constexpr uint64_t zm_utilityGsc = hash::Hash64(".gsc", zm_utility);
			constexpr uint64_t zm_utilityCsc = hash::Hash64(".csc", zm_utility);
			constexpr uint64_t is_ee_enabled = hash::HashT89Scr("is_ee_enabled");

			auto* targetGscEntry = std::find_if(scriptEntry, end, [](const auto& ent) { return ent.name == zm_utilityGsc; });
			auto* targetCscEntry = std::find_if(scriptEntry, end, [](const auto& ent) { return ent.name == zm_utilityCsc; });

			if (targetGscEntry == end && targetCscEntry == end) {
				auto err = std::format("Can't find custom EE target scripts");
				ps4.Notify(222, err);
				std::cerr << err << "\n";
				ps4.Disconnect();
				return tool::BASIC_ERROR;
			}

			// CheckClearParams 0x000d
			// GetByte 0x0889 0x01
			// Align 0x00
			// Return 0x0757
			std::vector<byte> payload{
				0x0d, 0x00,
				0x89, 0x08, 0x01,
				0x00,
				0x57, 0x07
			};

			auto replaceScript = [pid, &ps4, &payload](tool::dump::T8ScriptParseTreeEntry* entry) {
				auto buffer = ps4.ReadMemory(pid, entry->buffer, entry->size);
				auto* header = reinterpret_cast<tool::gsc::T8GSCOBJ*>(buffer.data());

				auto* exports = reinterpret_cast<tool::gsc::T8GSCExport*>(buffer.data() + header->export_table_offset);

				for (size_t i = 0; i < header->exports_count; i++) {
					auto& e = exports[i];

					if (e.name != is_ee_enabled) {
						continue;
					}
					// patch function

					ps4.WriteMemory(pid, entry->buffer + e.address, payload);
					return true;
				}
			
				return false;
			};

			if (targetGscEntry && !replaceScript(targetGscEntry)) {
				std::cerr << "Can't find zm_utility::is_ee_enabled() inside GSC script\n";
				ps4.Disconnect();
				return tool::BASIC_ERROR;
			}
			if (targetCscEntry && !replaceScript(targetCscEntry)) {
				std::cerr << "Can't find zm_utility::is_ee_enabled() inside CSC script\n";
				ps4.Disconnect();
				return tool::BASIC_ERROR;
			}


			std::string msg = "Custom EE injected, leave the zombies mode to erase.";
			ps4.Notify(222, msg);
			std::cout << msg << "\n";
		}
		catch (const char* res) {
			std::cerr << res << "\n";
		}
		ps4.Disconnect();

		return tool::OK;
	}

	int ps4injectcw100(Process& _, int argc, const char* argv[]) {
		if (argc < 6) {
			return tool::BAD_USAGE;
		}

		auto ipd = argv[2];
		auto script = argv[3];
		auto target = argv[4];
		auto replaced = argv[5];

		auto targetHash = hash::Hash64Pattern(target);
		auto replacedHash = hash::Hash64Pattern(replaced);

		std::cout <<
			"Ip: " << ipd << "\n"
			"script: " << script << "\n"
			"target: " << target << " (script_" << std::hex << targetHash << ")\n"
			"replaced: " << replaced << " (script_" << std::hex << replacedHash << ")\n"
			;

		std::filesystem::path scriptPath = script;
		void* scriptBuffer = NULL;
		size_t scriptSize = 0;

		if (!utils::ReadFileNotAlign(scriptPath, scriptBuffer, scriptSize, false)) {
			std::cerr << "Can't read '" << scriptPath.string() << "'\n";
			return tool::BASIC_ERROR;
		}

		if (scriptSize >= (1ull << ((sizeof(uint32_t) << 3) - 1))) {
			std::cerr << "Size too large: " << std::hex << scriptSize << "/" << (1ull << ((sizeof(uint32_t) << 3) - 1)) << "\n";
			std::free(scriptBuffer);
			return tool::BASIC_ERROR;
		}

		libdebug::PS4DBG ps4{ ipd };
		try {
			ps4.Connect();
			//ps4.Notify(210, std::format("Atian Tools {}", actsinfo::VERSION));

			auto procList = ps4.GetProcessList();
			auto proc = procList.FindProcess("eboot.bin");

			if (!proc) {
				ps4.Notify(222, "Please start the game");
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
				std::free(scriptBuffer);
				return tool::BASIC_ERROR;
			}



			uintptr_t poolIdx = base + 0x5551D60 + sizeof(XAssetPoolEntry) * 67; // 67 = spt cw
			auto bytes = ps4.ReadMemory(pid, poolIdx, sizeof(XAssetPoolEntry));

			auto* pool = reinterpret_cast<XAssetPoolEntry*>(bytes.data());

			auto val = pool->pool;
			auto count = pool->itemAllocCount;

			std::cout << std::hex << "SPT: 0x" << val << " : " << std::dec << count << "\n";

			auto poolBytes = ps4.ReadMemory(pid, val, count * sizeof(T9ScriptParseTree));

			auto* scriptEntry = reinterpret_cast<T9ScriptParseTree*>(&poolBytes[0]);

			auto* end = scriptEntry + count;

			auto* targetEntry = std::find_if(scriptEntry, end, [targetHash](const auto& ent) { return ent.name == targetHash; });

			uintptr_t replacedEntry = 0;
			T9ScriptParseTree* replacedEntryH = NULL;

			for (size_t i = 0; i < count; i++) {
				if (scriptEntry[i].name == replacedHash) {
					replacedEntryH = scriptEntry + i;
					replacedEntry = val + sizeof(*scriptEntry) * i;
					break;
				}
			}

			if (targetEntry == end) {
				auto err = std::format("Can't find target script '{}'", target);
				ps4.Notify(222, err);
				std::cerr << err << "\n";
				ps4.Disconnect();
				std::free(scriptBuffer);
				return tool::BASIC_ERROR;
			}
			if (!replacedEntry) {
				auto err = std::format("Can't find replaced script '{}'", replaced);
				std::cerr << err << "\n";
				ps4.Notify(222, err);
				ps4.Disconnect();
				std::free(scriptBuffer);
				return tool::BASIC_ERROR;
			}
			auto replacerScriptHeaderVal = ps4.ReadMemory(pid, replacedEntryH->buffer, sizeof(tool::gsc::T937GSCOBJ));
			auto replacerScriptHeader = reinterpret_cast<tool::gsc::T937GSCOBJ*>(replacerScriptHeaderVal.data());

			auto targetScriptHeader = ps4.ReadMemory(pid, targetEntry->buffer, sizeof(tool::gsc::T937GSCOBJ));
			auto scriptHeader = reinterpret_cast<tool::gsc::T937GSCOBJ*>(targetScriptHeader.data());
			auto includesOffset = targetEntry->buffer + scriptHeader->includes_table;
			auto targetScriptIncludes = ps4.ReadMemory(pid, includesOffset, sizeof(uint64_t) * scriptHeader->includes_count);
			auto includes = reinterpret_cast<uint64_t*>(targetScriptIncludes.data());
			auto includesEnd = includes + scriptHeader->includes_count;

			std::cout << target << " -> " << std::hex << targetEntry->buffer << "(" << targetEntry->name << ")\n";
			std::cout << replaced << " -> " << std::hex << replacedEntryH->buffer << "(" << replacedEntryH->name << ")\n";

			auto hookId = std::find(includes, includesEnd, replacedHash);
			if (hookId == includesEnd) {
				// need to add the include

				// insert the new include
				std::vector<byte> includeVal{};
				includeVal.insert(includeVal.begin(), (uint8_t*)&replacedHash, (uint8_t*)(&replacedHash + 1));
				ps4.WriteMemory(pid, includesOffset + sizeof(uint64_t) * scriptHeader->includes_count, includeVal);

				includeVal.clear();

				// correct the include count
				uint16_t newIncludeCount = scriptHeader->includes_count + 1;
				includeVal.insert(includeVal.begin(), (uint8_t*)&newIncludeCount, (uint8_t*)(&newIncludeCount + 1));
				ps4.WriteMemory(pid, targetEntry->buffer + offsetof(tool::gsc::T937GSCOBJ, includes_count), includeVal);
				std::cout << "Hook inserted into ";
				if (scriptHeader->name == targetHash) {
					std::cout << target;
				}
				else {
					std::cout << std::hex << scriptHeader->name;
				}
				std::cout << "\n";
			}


			auto loc = ps4.AllocateMemory(pid, (int)scriptSize + 15);
			auto locAligned = (loc + 15) & ~15;

			if (!loc) {
				std::cerr << "Can't allocate memory\n";
			}
			else {
				std::cout << "Allocating script...\n";

				// fixup crc & name
				reinterpret_cast<tool::gsc::T937GSCOBJ*>(scriptBuffer)->crc = replacerScriptHeader->crc;
				reinterpret_cast<tool::gsc::T937GSCOBJ*>(scriptBuffer)->name = replacerScriptHeader->name;

				std::vector<uint8_t> scriptBytes{};
				scriptBytes.insert(scriptBytes.begin(), (uint8_t*)scriptBuffer, (uint8_t*)scriptBuffer + scriptSize);
				ps4.WriteMemory(pid, locAligned, scriptBytes);

				std::cout << "Script allocated at " << std::hex << locAligned << "\n";

				std::vector<uint8_t> entryBytes{};
				entryBytes.insert(entryBytes.begin(), (uint8_t*)&locAligned, (uint8_t*)(&locAligned + 1));
				ps4.WriteMemory(pid, replacedEntry + offsetof(T9ScriptParseTree, buffer), entryBytes);

				ps4.Notify(222, std::format("{} injected", script));
			}
		}
		catch (const char* res) {
			std::cerr << res << "\n";
		}
		ps4.Disconnect();
		std::free(scriptBuffer);



		return tool::OK;
	}
}

ADD_TOOL(ps4_t8cee, "bo4", " [ip:port]", "Enable EEs in custom mutation/offline/casual (BO4 / PS4 126)", nullptr, ps4t8cee);
ADD_TOOL(ps4_inject, "bo4", " [ip:port] [script] [target] [replaced]", "Inject script (BO4 / PS4 126)", nullptr, ps4inject);
ADD_TOOL(ps4_injectcw100, "cw", " [ip:port] [script] [target] [replaced]", "Inject script (CW / PS4 100)", nullptr, ps4injectcw100);