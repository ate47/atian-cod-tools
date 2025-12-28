#include <includes.hpp>
#include "gsc_vm.hpp"
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/gsc_opcodes.hpp>
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
    }

    std::unordered_map<uint64_t, GscVmOpCode*>& GscOpCodes() {
        static std::unordered_map<uint64_t, GscVmOpCode*> gscOpCodes{};
        return gscOpCodes;
    }

    GscVm::GscVm(uint64_t vm, std::function<std::shared_ptr<GSCOBJHandler>(byte*, size_t)> func) : vm(vm), func(func) {
        GscReaders()[vm] = this;
    }

    GscGdb::GscGdb(uint64_t magic, 
        std::function<void(T8GSCOBJContext& ctx, core::bytebuffer::ByteBuffer& dbgReader, std::ostream& asmout)> load,
        std::function<bool(GscDecompilerGDBData* gdb, std::string& data)> saver)
        : magic(magic), load(load), saver(saver) {
        GdbReaders()[magic] = this;
    }

    std::function<std::shared_ptr<GSCOBJHandler>(byte*, size_t)>* GetGscReader(uint64_t vm) {
        auto& gscReaders{ GscReaders() };
        auto it = gscReaders.find(vm);

        if (it == gscReaders.end()) {
            return nullptr;
        }

        return &it->second->func;
    }

    GscGdb* GetGdbReader(uint64_t magic) {
        auto& gscReaders{ GdbReaders() };
        auto it = gscReaders.find(magic);

        if (it == gscReaders.end()) {
            return nullptr;
        }

        return it->second;
    }

    GscVmOpCode::GscVmOpCode(const char* id, std::function<void()> func, const char* deps) : func(func), id(id), deps(deps) {
        std::unordered_map<uint64_t, GscVmOpCode*>& opcodes{ GscOpCodes() };
        GscVmOpCode*& ptr{ opcodes[hash::Hash64(id)] };
        if (ptr) {
            LOG_WARNING("registering twice vm {}", id);
        }
        ptr = this;
    }

    void GscVmOpCode::RegisterAndDeps() {
        if (state == GVOLS_LOADED) {
            return;
        }
        if (state == GVOLS_LOADING) {
            throw std::runtime_error(std::format("Can't register vm opcode {}: recursive loading", id));
        }

        state = GVOLS_LOADING;

        if (deps) {
            std::unordered_map<uint64_t, GscVmOpCode*>& opcodes{ GscOpCodes() };

            auto it{ opcodes.find(hash::Hash64(deps)) };
            if (it == opcodes.end()) {
                throw std::runtime_error(std::format("Can't register vm opcode {}: deps {} doesn't exist", id, deps));
            }

            it->second->RegisterAndDeps();
        }

        // load the values
        func();

        state = GVOLS_LOADED;
    }

    void RegisterVmOpCodes() {
        std::unordered_map<uint64_t, GscVmOpCode*>& opcodes{ GscOpCodes() };
        if (core::logs::getlevel() <= core::logs::loglevel::LVL_TRACE) {
            std::ostringstream oss{};

            for (auto& [name, opcode] : opcodes) {
                oss << " " << opcode->id;
                if (opcode->deps) {
                    oss << "<" << opcode->deps << ">";
                }
            }

            LOG_LVLF(core::logs::LVL_TRACE, "Registering opcodes for{}", oss.str());
        }
        actslib::profiler::Profiler pl{ "vmReg" };

        for (auto& [name, opcode] : opcodes) {
            opcode->RegisterAndDeps();
        }
        pl.Stop();
        LOG_TRACE("Registered opcodes {}ms", pl.GetCurrent().GetMillis());
    }
}