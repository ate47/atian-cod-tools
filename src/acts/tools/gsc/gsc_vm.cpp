#include <includes.hpp>
#include "gsc_vm.hpp"
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/gsc_opcodes.hpp>
#include <actscli.hpp>

// opcodes
#include <tools/gsc/opcodes/gsc_opcodes_acts.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_gscbin.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_iw8.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_iw9.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_jup_8a.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_jup_8b.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t6_01.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t7_1a.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t7_1b.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t7_1c.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t8_31.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t8_34.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t8_35.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t8_36.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t9_37.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t9_38.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t10_06.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t10_07.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t10_0b.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t10_0c.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t10_0d.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t10_0e.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t10_0f.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t10_10.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t10_11.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t10_12.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t10_13.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_t10_14.hpp>
#include <tools/gsc/opcodes/gsc_opcodes_sat_14.hpp>

// vm
#include <tools/gsc/vm/gsc_vm_gscbin.hpp>
#include <tools/gsc/vm/gsc_vm_acts.hpp>
#include <tools/gsc/vm/gsc_vm_jup.hpp>
#include <tools/gsc/vm/gsc_vm_t6.hpp>
#include <tools/gsc/vm/gsc_vm_t7.hpp>
#include <tools/gsc/vm/gsc_vm_t8.hpp>
#include <tools/gsc/vm/gsc_vm_t9.hpp>
#include <tools/gsc/vm/gsc_vm_t10.hpp>

// gdb
#include <tools/gsc/gdb/gdb_acts.hpp>
#include <tools/gsc/gdb/gdb_t78.hpp>


namespace tool::gsc::vm {
    namespace {
        template<typename HandlerName>
        class GscVmImpl : public GscVm {
            using GscVm::GscVm;

            std::shared_ptr<GSCOBJHandler> NewHandler(byte* file, size_t fileSize) const override {
                return std::make_shared<HandlerName>(file, fileSize);
            }
        };

        class GscVmData {
        public:
            std::unordered_map<uint64_t, std::shared_ptr<GscVm>> gscReaders{};
            std::unordered_map<uint64_t, std::shared_ptr<GscGdb>> gdbReaders{};

            GscVmData() {
                // register VM readers
                RegisterGSCOBJHandler<ACTSF1GSCOBJHandler>(VMI_ACTS_F1);
                RegisterGSCOBJHandler<IWGSCOBJHandler>(VMI_IW_GSCBIN);

                RegisterGSCOBJHandler<MW23GSCOBJHandler>(VMI_JUP_8A);
                RegisterGSCOBJHandler<MW23BGSCOBJHandler>(VMI_JUP_8B);

                RegisterGSCOBJHandler<T7GSCOBJHandler>(VMI_T7_1C);
                RegisterGSCOBJHandler<T71BGSCOBJHandler>(VMI_T7_1B);
                RegisterGSCOBJHandler<T71AGSCOBJHandler>(VMI_T7_1A);

                RegisterGSCOBJHandler<T8GSCOBJHandler>(VMI_T8_36);
                RegisterGSCOBJHandler<T835GSCOBJHandler>(VMI_T8_35);
                RegisterGSCOBJHandler<T834GSCOBJHandler>(VMI_T8_34);
                RegisterGSCOBJHandler<T831GSCOBJHandler>(VMI_T8_31);

                RegisterGSCOBJHandler<T9GSCOBJHandler>(VMI_T9_38);
                RegisterGSCOBJHandler<T937GSCOBJHandler>(VMI_T9_37);

                RegisterGSCOBJHandler<T1006GSCOBJHandler>(VMI_T10_06);
                RegisterGSCOBJHandler<T1007GSCOBJHandler>(VMI_T10_07);
                RegisterGSCOBJHandler<T100BGSCOBJHandler>(VMI_T10_0B);
                RegisterGSCOBJHandler<T100CGSCOBJHandler>(VMI_T10_0C);
                RegisterGSCOBJHandler<T1010GSCOBJHandler<VMI_T10_0D>>(VMI_T10_0D);
                RegisterGSCOBJHandler<T1010GSCOBJHandler<VMI_T10_0E>>(VMI_T10_0E);
                RegisterGSCOBJHandler<T1010GSCOBJHandler<VMI_T10_0F>>(VMI_T10_0F);
                RegisterGSCOBJHandler<T1010GSCOBJHandler<VMI_T10_10>>(VMI_T10_10);
                RegisterGSCOBJHandler<T1010GSCOBJHandler<VMI_T10_11>>(VMI_T10_11);
                RegisterGSCOBJHandler<T1010GSCOBJHandler<VMI_T10_12>>(VMI_T10_12);
                RegisterGSCOBJHandler<T1010GSCOBJHandler<VMI_T10_13>>(VMI_T10_13);
                RegisterGSCOBJHandler<T1010GSCOBJHandler<VMI_T10_14>>(VMI_T10_14);

                // register GBD data
                RegisterGDBOBJHandler<GscGdbActs>();
                RegisterGDBOBJHandler<GscGdbT78<VMI_DBG_T7_12>>();
                RegisterGDBOBJHandler<GscGdbT78<VMI_DBG_T7_13>>();
                RegisterGDBOBJHandler<GscGdbT78<VMI_DBG_T8_21>>();
            }


            template<typename GSCOBJHandlerType>
            void RegisterGSCOBJHandler(uint64_t magic) {
                std::shared_ptr<GscVm> vm{ std::make_shared<GscVmImpl<GSCOBJHandlerType>>(magic) };
                gscReaders[magic] = vm;
            }

            template<typename GSCGDBType>
            void RegisterGDBOBJHandler() {
                std::shared_ptr<GscGdb> gdb{ std::make_shared<GSCGDBType>() };
                gdbReaders[gdb->magic] = gdb;
            }
        };

        GscVmData& VmData() {
            static GscVmData data{};
            return data;
        }
    }



    tool::gsc::vm::GscVm* GetGscReader(uint64_t vm) {
        auto& gscReaders{ VmData().gscReaders };
        auto it = gscReaders.find(vm);

        if (it == gscReaders.end()) {
            return nullptr;
        }

        return it->second.get();
    }

    GscGdb* GetGdbReader(uint64_t magic) {
        auto& gdbReaders{ VmData().gdbReaders };
        auto it = gdbReaders.find(magic);

        if (it == gdbReaders.end()) {
            return nullptr;
        }

        return it->second.get();
    }

    void RegisterVmOpCodes() {
        using namespace tool::gsc::opcode;
        // registry
        OpCodeActsRegistry::OpCode();

        OpCodeGscBinRegistry::OpCode();
        OpCodeIW8Registry::OpCode();
        OpCodeIW9Registry::OpCode();

        OpCodeJup8ARegistry::OpCode();
        OpCodeJup8BRegistry::OpCode();

        OpCodeT601Registry::OpCode();

        OpCodeT71ARegistry::OpCode();
        OpCodeT71BRegistry::OpCode();
        OpCodeT71CRegistry::OpCode();

        OpCodeT831Registry::OpCode();
        OpCodeT834Registry::OpCode();
        OpCodeT835Registry::OpCode();
        OpCodeT836Registry::OpCode();

        OpCodeT937Registry::OpCode();
        OpCodeT938Registry::OpCode();

        OpCodeT1006Registry::OpCode();
        OpCodeT1007Registry::OpCode();
        OpCodeT100BRegistry::OpCode();
        OpCodeT100CRegistry::OpCode();
        OpCodeT100DRegistry::OpCode();
        OpCodeT100ERegistry::OpCode();
        OpCodeT100FRegistry::OpCode();
        OpCodeT1010Registry::OpCode();
        OpCodeT1011Registry::OpCode();
        OpCodeT1012Registry::OpCode();
        OpCodeT1013Registry::OpCode();
        OpCodeT1014Registry::OpCode();

        OpCodeSat14Registry::OpCode();
    }
}