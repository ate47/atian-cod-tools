#include <includes.hpp>
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include <pool.hpp>
#include "tools/bo6/bo6.hpp"

namespace {

    int ps4dumpbo6(Process& _, int argc, const char* argv[]) {
        if (argc < 3) {
            return tool::BAD_USAGE;
        }

        const char* ipd = argv[2];

        libdebug::PS4DBG ps4{ ipd };
        try {
            ps4.Connect();



            auto procList = ps4.GetProcessList();
            auto proc = procList.FindProcess("eboot.bin");

            if (!proc) {
                ps4.Notify(222, "Please start the game");
                LOG_ERROR("Can't find eboot.bin");
                ps4.Disconnect();
                return tool::BASIC_ERROR;
            }

            auto pid = proc->pid;

            auto entries = ps4.GetProcessMaps(proc->pid);

            uint64_t base = 0;

            for (const auto& entry : entries.entries) {
                if (entry->prot == 5) {
                    LOG_INFO("executable base : 0x{:x}", entry->start);
                    base = entry->start;
                    break;
                }
            }

            if (!base) {
                LOG_ERROR("Can't find executable base");
                ps4.Disconnect();
                return tool::BASIC_ERROR;
            }
            struct DB_AssetPool
            {
                uintptr_t m_entries; // void*
                uintptr_t m_freeHead; // void*
                unsigned int m_poolSize;
                unsigned int m_elementSize;
                unsigned int m_loadedPoolSize;
                char __padding[158];
            };

            uintptr_t poolIdx = base + 0x98FEFA0 + sizeof(DB_AssetPool) * 69; // 69 = gscobj
            auto bytes = ps4.ReadMemory(pid, poolIdx, sizeof(DB_AssetPool));

            auto* pool = reinterpret_cast<DB_AssetPool*>(bytes.data());

            struct GscObjEntry {
                uint64_t name;
                int len;
                int padc;
                uintptr_t buffer;
            };

            LOG_INFO("Pool: {:x}, count: {}/{}, len 0x{:x}", pool->m_entries, pool->m_loadedPoolSize, pool->m_poolSize, pool->m_elementSize);
            auto bytes2 = ps4.ReadMemory(pid, pool->m_entries, sizeof(GscObjEntry) * pool->m_loadedPoolSize);

            auto* objs = reinterpret_cast<GscObjEntry*>(bytes2.data());

            std::filesystem::path gsc = "gsc";

            std::filesystem::create_directories(gsc);

            for (size_t i = 0; i < pool->m_loadedPoolSize; i++) {
                auto& obj = objs[i];

                if (!obj.len || !obj.buffer) {
                    LOG_WARNING("Ignore {:x}", obj.name);
                    continue;
                }

                auto bytes3 = ps4.ReadMemory(pid, obj.buffer, obj.len);

                if (!utils::WriteFile(gsc / utils::va("script_%llx.gscc", obj.name), bytes3.data(), bytes3.size())) {
                    LOG_ERROR("Error when writting {:x}", obj.name);
                }
                else {
                    LOG_INFO("Dump script_{:x}.gscc", obj.name);
                }

            }

            ps4.Notify(210, "test");
        }
        catch (const char* res) {
            LOG_ERROR("{}", res);
        }
        ps4.Disconnect();

        return tool::OK;
    }


    ADD_TOOL("ps4dumpbo6", "dev", " [ip:port]", "", nullptr, ps4dumpbo6);
}