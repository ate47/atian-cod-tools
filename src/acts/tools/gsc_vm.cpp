#include <includes.hpp>
#include "gsc_vm.hpp"
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>
#include <decrypt.hpp>


namespace tool::gsc::vm {
    namespace {
        std::unordered_map<byte, GscVm*>& GscReaders() {
            static std::unordered_map<byte, GscVm*> gscReaders{};
            return gscReaders;
        }
        std::vector<GscVmOpCode*>& GscOpCodes() {
            static std::vector<GscVmOpCode*> gscOpCodes{};
            return gscOpCodes;
        }
    }

    GscVm::GscVm(byte vm, std::function<std::shared_ptr<GSCOBJHandler>(byte*, size_t)> func) : vm(vm), func(func) {
        GscReaders()[vm] = this;
    }

    GscVmOpCode::GscVmOpCode(const char* id, std::function<void()> func, bool priv) : func(func), id(id), priv(priv) {
        GscOpCodes().emplace_back(this);
    }

    std::function<std::shared_ptr<GSCOBJHandler>(byte*, size_t)>* GetGscReader(byte vm) {
        auto& gscReaders{ GscReaders() };
        auto it = gscReaders.find(vm);

        if (it == gscReaders.end()) {
            return nullptr;
        }

        return &it->second->func;
    }
    void RegisterVmOpCodes() {
        auto& opcodes{ GscOpCodes() };
        if (alogs::getlevel() <= alogs::loglevel::LVL_TRACE) {
            std::ostringstream oss{};

            for (GscVmOpCode* opcode : opcodes) {
                oss << " " << opcode->id;
            }

            alogs::log(alogs::LVL_TRACE, std::format("Registering opcodes for{}", oss.str()));
        }

        // register public
        for (GscVmOpCode* opcode : opcodes) {
            if (!opcode->priv) {
                opcode->func();
            }
        }
        // register private, this is a lazy way to fix private private opcodes vm depending on public vm
        for (GscVmOpCode* opcode : opcodes) {
            if (opcode->priv) {
                opcode->func();
            }
        }
    }
}