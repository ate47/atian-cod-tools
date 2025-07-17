#include <includes.hpp>
#include "gsc_vm.hpp"
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>
#include <actscli.hpp>


namespace tool::gsc::vm {
    namespace {
        std::unordered_map<uint64_t, GscVm*>& GscReaders() {
            static std::unordered_map<uint64_t, GscVm*> gscReaders{};
            return gscReaders;
        }
        std::unordered_map<uint64_t, GscGdb*>& GdbReaders() {
            static std::unordered_map<uint64_t, GscGdb*> gdbReaders{};
            return gdbReaders;
        }
        std::vector<GscVmOpCode*>& GscOpCodes() {
            static std::vector<GscVmOpCode*> gscOpCodes{};
            return gscOpCodes;
        }
    }

    GscVm::GscVm(uint64_t vm, std::function<std::shared_ptr<GSCOBJHandler>(byte*, size_t)> func) : vm(vm), func(func) {
        GscReaders()[vm] = this;
    }

    GscGdb::GscGdb(uint64_t magic, std::function<void(T8GSCOBJContext& ctx, core::bytebuffer::ByteBuffer& dbgReader, std::ostream& asmout)> load) : magic(magic), load(load) {
        GdbReaders()[magic] = this;
    }

    GscVmOpCode::GscVmOpCode(const char* id, std::function<void()> func, bool priv) : func(func), id(id), priv(priv) {
        GscOpCodes().emplace_back(this);
    }

    std::function<std::shared_ptr<GSCOBJHandler>(byte*, size_t)>* GetGscReader(uint64_t vm) {
        auto& gscReaders{ GscReaders() };
        auto it = gscReaders.find(vm);

        if (it == gscReaders.end()) {
            return nullptr;
        }

        return &it->second->func;
    }

    std::function<void(T8GSCOBJContext& ctx, core::bytebuffer::ByteBuffer& dbgReader, std::ostream& asmout)>* GetGdbReader(uint64_t magic) {
        auto& gscReaders{ GdbReaders() };
        auto it = gscReaders.find(magic);

        if (it == gscReaders.end()) {
            return nullptr;
        }

        return &it->second->load;
    }

    void RegisterVmOpCodes() {
        auto& opcodes{ GscOpCodes() };
        if (core::logs::getlevel() <= core::logs::loglevel::LVL_TRACE) {
            std::ostringstream oss{};

            for (GscVmOpCode* opcode : opcodes) {
                oss << " " << opcode->id;
            }

            LOG_LVLF(core::logs::LVL_TRACE, "Registering opcodes for{}", oss.str());
        }
        actslib::profiler::Profiler pl{ "vmReg" };

        // register public
        for (GscVmOpCode* opcode : opcodes) {
            if (!opcode->priv) {
                opcode->func();
            }
        }
        // register private, this is a lazy way to fix private private opcodes vm depending on public vm
        if (!actscli::options().noPrivate) {
            for (GscVmOpCode* opcode : opcodes) {
                if (opcode->priv) {
                    opcode->func();
                }
            }
        }
        pl.Stop();
        LOG_TRACE("Registered opcodes {}ms", pl.GetCurrent().GetMillis());
    }
}