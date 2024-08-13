#include <includes.hpp>
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include <pool.hpp>
#include "tools/bo6/bo6.hpp"

namespace {
    struct DB_AssetPool
    {
        uintptr_t m_entries; // void*
        uintptr_t m_freeHead; // void*
        unsigned int m_poolSize;
        unsigned int m_elementSize;
        unsigned int m_loadedPoolSize;
        char __padding[158];
    };
    struct GscObjEntry {
        uint64_t name;
        int len;
        int padc;
        uintptr_t buffer;
    };

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
                LOG_ERROR("Can't find eboot.bin");
                for (auto& p : procList.processes) {
                    LOG_TRACE("{}", p->name);
                }
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

            uintptr_t poolIdx = base + 0x98FEFA0 + sizeof(DB_AssetPool) * 69; // 69 = gscobj
            auto bytes = ps4.ReadMemory(pid, poolIdx, sizeof(DB_AssetPool));

            auto* pool = reinterpret_cast<DB_AssetPool*>(bytes.data());


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

    int ps4cbufbo6(Process& _, int argc, const char* argv[]) {
        if (argc < 4) {
            return tool::BAD_USAGE;
        }

        const char* ipd = argv[2];
        const char* cmd = argv[3];

        LOG_TRACE("Send {} -> {}", ipd, cmd);

        libdebug::PS4DBG ps4{ ipd };
        try {
            ps4.Connect();

            LOG_TRACE("Connected to {}", ipd);



            auto procList = ps4.GetProcessList();
            auto proc = procList.FindProcess("eboot.bin");

            if (!proc) {
                LOG_ERROR("Can't find eboot.bin");
                for (auto& p : procList.processes) {
                    LOG_TRACE("{}", p->name);
                }
                ps4.Disconnect();
                return tool::BASIC_ERROR;
            }

            auto pid = proc->pid;

            auto entries = ps4.GetProcessMaps(proc->pid);

            uint64_t base = 0;

            LOG_TRACE("Connected to {}", ipd);
            for (const auto& entry : entries.entries) {
                if (entry->prot == 5) {
                    LOG_INFO("executable base : 0x{:x}", entry->start);
                    base = entry->start;
                    break;
                }
            }
            LOG_TRACE("Connected to {}", ipd);

            if (!base) {
                LOG_ERROR("Can't find executable base");
                ps4.Disconnect();
                return tool::BASIC_ERROR;
            }

            uint64_t cbuf1 = base + 0x4D6C350;
            uint64_t cbuf2 = cbuf1 + 0x10004;

            std::vector<byte> data{};

            LOG_TRACE("Write string");
            utils::WriteString(data, cmd);

            ps4.WriteMemory(pid, cbuf1, data);

            data.clear();
            LOG_TRACE("Write len");
            utils::WriteValue<uint32_t>(data, (uint32_t)std::strlen(cmd));

            ps4.WriteMemory(pid, cbuf2, data);

            ps4.Notify(210, std::format("cbuf {}", cmd));
        }
        catch (std::exception& res) {
            LOG_ERROR("{}", res.what());
        }
        ps4.Disconnect();

        return tool::OK;
    }
    int ps4repbo6(Process& _, int argc, const char* argv[]) {
        if (argc < 4) {
            return tool::BAD_USAGE;
        }

        const char* ipd = argv[2];
        const char* file = argv[3];

        std::string buff{};

        if (!utils::ReadFile(file, buff)) {
            LOG_ERROR("Can't read {}", file);
            return tool::BASIC_ERROR;
        }

        tool::gsc::GscObj24* script{ (tool::gsc::GscObj24*)buff.data() };

        if (*(uint64_t*)script != 0xa0d43534706) {
            LOG_ERROR("Invalid script magic");
            return tool::BASIC_ERROR;
        }

        uint64_t name = script->name;

        LOG_INFO("Loaded script {} -> name = script_{:x}", file, name);


        libdebug::PS4DBG ps4{ ipd };
        try {
            ps4.Connect();

            LOG_TRACE("Connected to {}", ipd);



            auto procList = ps4.GetProcessList();
            auto proc = procList.FindProcess("eboot.bin");

            if (!proc) {
                LOG_ERROR("Can't find eboot.bin");
                for (auto& p : procList.processes) {
                    LOG_TRACE("{}", p->name);
                }
                ps4.Disconnect();
                return tool::BASIC_ERROR;
            }

            auto pid = proc->pid;

            auto entries = ps4.GetProcessMaps(proc->pid);

            uint64_t base = 0;

            LOG_TRACE("Connected to {}", ipd);
            for (const auto& entry : entries.entries) {
                if (entry->prot == 5) {
                    LOG_INFO("executable base : 0x{:x}", entry->start);
                    base = entry->start;
                    break;
                }
            }
            LOG_TRACE("Connected to {}", ipd);

            if (!base) {
                LOG_ERROR("Can't find executable base");
                ps4.Disconnect();
                return tool::BASIC_ERROR;
            }


            uintptr_t poolIdx = base + 0x98FEFA0 + sizeof(DB_AssetPool) * 69; // 69 = gscobj
            auto bytes = ps4.ReadMemory(pid, poolIdx, sizeof(DB_AssetPool));

            auto* pool = reinterpret_cast<DB_AssetPool*>(bytes.data());


            LOG_INFO("Pool: {:x}, count: {}/{}, len 0x{:x}", pool->m_entries, pool->m_loadedPoolSize, pool->m_poolSize, pool->m_elementSize);
            auto bytes2 = ps4.ReadMemory(pid, pool->m_entries, sizeof(GscObjEntry) * pool->m_loadedPoolSize);

            auto* objs = reinterpret_cast<GscObjEntry*>(bytes2.data());

            for (size_t i = 0; i < pool->m_loadedPoolSize; i++) {
                auto& obj = objs[i];

                if (obj.name != name) {
                    continue;
                }

                if (!obj.buffer) {
                    LOG_ERROR("Empty buffer");
                    break;
                }

                if (obj.len < buff.size()) {
                    LOG_ERROR("Buffer too small, can't remplace {} < {}", obj.len, buff.size());
                    break;
                }

                std::vector<byte> data{};

                data.insert(data.begin(), (char*)buff.data(), (char*)buff.data() + buff.size());

                ps4.WriteMemory(pid, obj.buffer, data);

                LOG_INFO("Script replaced");
                break;
            }

        }
        catch (std::exception& res) {
            LOG_ERROR("{}", res.what());
        }
        ps4.Disconnect();

        return tool::OK;
    }

    
    ADD_TOOL("ps4cbufbo6", "dev", " [ip:port] [cmd]", "", nullptr, ps4cbufbo6);
    ADD_TOOL("ps4dumpbo6", "dev", " [ip:port]", "", nullptr, ps4dumpbo6);
    ADD_TOOL("ps4repbo6", "dev", " [ip:port] [file]", "", nullptr, ps4repbo6);
}
