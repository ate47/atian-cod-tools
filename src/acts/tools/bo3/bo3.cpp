#include <includes.hpp>
#include "pools.hpp"
#include "bo3.hpp"
#include "tools/gsc.hpp"

namespace bo3 {
    int InjectScriptBO3(Process& proc, const char* script, const char* replaced, std::string& notify) {
		using namespace bo3::pool;
		LOG_DEBUG("script: {}", script);
		LOG_DEBUG("replaced: {}", replaced);

		std::filesystem::path scriptPath = script;
		std::string scriptBuffStr{};

		if (!utils::ReadFile(scriptPath, scriptBuffStr)) {
			notify = std::format("Can't read '{}'", scriptPath.string());
			return tool::BASIC_ERROR;
		}

		void* scriptBuffer = scriptBuffStr.data();
		size_t scriptSize = scriptBuffStr.size();

		if (scriptSize < 8 || *reinterpret_cast<uint64_t*>(scriptBuffer) != bo3::GSC_MAGIC) {
			notify = "Not a valid compiled Black Ops Cold War script (BAD MAGIC)";
			return tool::BASIC_ERROR;
		}

		pool::T7XAssetPool sptPool{};

		uintptr_t poolLoc = proc[bo3::pool::xassetpools];

		proc.WriteLocation(std::cout << "pool: ", poolLoc) << "\n";

		if (!proc.ReadMemory(&sptPool, poolLoc + sizeof(sptPool) * bo3::pool::T7_ASSET_TYPE_SCRIPTPARSETREE, sizeof(sptPool))) {
			notify = "Can't read SPT pool";
			return tool::BASIC_ERROR;
		}

		auto entries = std::make_unique<T7ScriptParseTree[]>(sptPool.itemAllocCount);

		if (!proc.ReadMemory(&entries[0], sptPool.pool, sptPool.itemAllocCount * sizeof(entries[0]))) {
			notify = "Can't read SPT pool entries";
			return tool::BASIC_ERROR;
		}

		T7ScriptParseTree* scriptEntry = reinterpret_cast<bo3::pool::T7ScriptParseTree*>(&entries[0]);

		T7ScriptParseTree* end = scriptEntry + sptPool.itemAllocCount;

		uintptr_t replacedEntry = 0;
		bo3::pool::T7ScriptParseTree* replacedEntryH = NULL;

		for (size_t i = 0; i < sptPool.itemAllocCount; i++) {
			if (!_strcmpi(proc.ReadStringTmp(scriptEntry[i].name, ""), replaced)) {
				replacedEntryH = scriptEntry + i;
				replacedEntry = sptPool.pool + sizeof(*scriptEntry) * i;
				break;
			}
		}

		if (!replacedEntry) {
			notify = std::format("Can't find replaced script '{}'", replaced);
			return tool::BASIC_ERROR;
		}
		tool::gsc::T7GSCOBJ replacerScriptHeader{};
		if (!proc.ReadMemory(&replacerScriptHeader, replacedEntryH->script, sizeof(tool::gsc::T7GSCOBJ))) {
			notify = "Can't read replacer header";
			return tool::BASIC_ERROR;
		}

		LOG_DEBUG("{} -> {:x} ({})", replaced, replacedEntryH->script, proc.ReadStringTmp(replacedEntryH->name, "<err>"));

		// fixup crc & name
		tool::gsc::T7GSCOBJ* obj{ (tool::gsc::T7GSCOBJ*)scriptBuffer };
		obj->source_crc = replacerScriptHeader.source_crc;

		if (std::strcmp(replaced, (const char*)(obj->magic + obj->name_offset))) {
			// we need to replace the name
			size_t nameLen{ std::strlen(replaced) + 1 };

			obj->name_offset = (uint32_t)scriptBuffStr.size();
			scriptBuffStr.resize(scriptBuffStr.size() + nameLen);

			scriptBuffer = scriptBuffStr.data();
			scriptSize = scriptBuffStr.size();

			// add the string
			std::memcpy(&scriptBuffStr[obj->name_offset], replaced, nameLen);
		}

		uintptr_t loc = proc.AllocateMemory(scriptSize + 0x1F, PAGE_READWRITE);

		if (!loc) {
			notify = "Can't allocate memory";
			return tool::BASIC_ERROR;
		}

		uintptr_t locAligned = (loc + 0x1F) & ~0x1F;
		LOG_DEBUG("Allocating script at 0x{:x} (0x{:x})", locAligned, loc);

		if (!proc.WriteMemory(locAligned, scriptBuffer, scriptSize)) {
			auto err = GetLastError();
			notify = std::format("Error when writing script: 0x{:x}", err);
			proc.FreeMemory(loc, scriptSize + 0xF);
			return tool::BASIC_ERROR;
		}

		if (!proc.WriteMemory(replacedEntry + offsetof(T7ScriptParseTree, script), &locAligned, sizeof(locAligned))) {
			notify = std::format("Error when patching SPT entry to 0x{:x}", locAligned);
			proc.FreeMemory(loc, scriptSize + 0xF);
			return tool::BASIC_ERROR;
		}

		notify = std::format("{} injected at {:x}", script, locAligned);
		return tool::OK;
    }
}
namespace {
    int injectbo3(Process& proc, int argc, const char* argv[]) {
        if (argc < 4) {
            return tool::BAD_USAGE;
        }

        const char* script = argv[2];
        const char* replaced = argv[3];

        std::string notify{};
        int ret = bo3::InjectScriptBO3(proc, script, replaced, notify);

        if (!notify.empty()) {
            if (ret) {
                LOG_ERROR("{}", notify);
            }
            else {
                LOG_INFO("{}", notify);
            }
        }

        return ret;
    }
}
ADD_TOOL(injectbo3, "bo3", " (script) (replace)", "inject script (bo3)", L"BlackOps3.exe", injectbo3);