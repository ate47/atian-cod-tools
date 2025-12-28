#include <includes.hpp>
#include <tools/gsc/gsc_export.hpp>
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/gsc_iw.hpp>
#include <acts_vm.hpp>


namespace tool::gsc {
    using namespace opcode;


    struct H32T7GSCExportReader : GSCExportReader {
        T7GSCExport* exp{};

        void SetHandle(void* handle) override { exp = (T7GSCExport*)handle; };
        uint64_t GetName() override { return exp->name; };
        uint64_t GetNamespace() override { return exp->name_space; };
        uint64_t GetFileNamespace() override { return 0; };
        uint64_t GetChecksum() override { return exp->checksum; };
        uint32_t GetAddress() override { return exp->address; };
        uint8_t GetParamCount() override { return exp->param_count; };
        uint8_t GetFlags() override { return exp->flags; };
        size_t SizeOf() override { return sizeof(*exp); };
    };

    struct H32GSCExportReader : GSCExportReader {
        T8GSCExport* exp{};

        void SetHandle(void* handle) override { exp = (T8GSCExport*)handle; };
        uint64_t GetName() override { return exp->name; };
        uint64_t GetNamespace() override { return exp->name_space; };
        uint64_t GetFileNamespace() override { return exp->callback_event; };
        uint64_t GetChecksum() override { return exp->checksum; };
        uint32_t GetAddress() override { return exp->address; };
        uint8_t GetParamCount() override { return exp->param_count; };
        uint8_t GetFlags() override { return exp->flags; };
        size_t SizeOf() override { return sizeof(*exp); };
    };
    struct H64GSCExportReader : GSCExportReader {
        IW23GSCExport* exp{};

        void SetHandle(void* handle) override { exp = (IW23GSCExport*)handle; };
        uint64_t GetName() override { return exp->name; };
        uint64_t GetNamespace() override { return exp->name_space; };
        uint64_t GetFileNamespace() override { return exp->file_name_space; };
        uint64_t GetChecksum() override { return exp->checksum; };
        uint32_t GetAddress() override { return exp->address; };
        uint8_t GetParamCount() override { return exp->param_count; };
        uint8_t GetFlags() override { return exp->flags; };
        size_t SizeOf() override { return sizeof(*exp); };
    };
    struct BINGSCExportReader : GSCExportReader {
        tool::gsc::iw::BINGSCExport* exp{};

        void SetHandle(void* handle) override { exp = (tool::gsc::iw::BINGSCExport*)handle; };
        uint64_t GetName() override { return exp->name; };
        uint64_t GetNamespace() override { return exp->name_space; };
        uint64_t GetFileNamespace() override { return exp->file_name_space; };
        uint64_t GetChecksum() override { return exp->checksum; };
        uint32_t GetAddress() override { return exp->address; };
        uint8_t GetParamCount() override { return exp->param_count; };
        uint8_t GetFlags() override { return exp->flags; };
        uint32_t GetSize() { return exp->size; };
        size_t SizeOf() override { return sizeof(*exp); };
    };

    struct H64CERGSCExportReader : GSCExportReader {
        IW24GSCExport* exp{};

        void SetHandle(void* handle) override { exp = (IW24GSCExport*)handle; };
        uint64_t GetName() override { return exp->name; };
        uint64_t GetNamespace() override { return exp->name_space; };
        uint64_t GetFileNamespace() override { return exp->file_name_space; };
        uint64_t GetChecksum() override { return 0; };
        uint32_t GetAddress() override { return exp->address; };
        uint8_t GetParamCount() override { return exp->param_count; };
        uint8_t GetFlags() override { return exp->flags; };
        size_t SizeOf() override { return sizeof(*exp); };
    };

    struct ACTSExportReader : GSCExportReader {
        acts::vm::ScriptExport* exp{};

        void SetHandle(void* handle) override { exp = (acts::vm::ScriptExport*)handle; };
        uint64_t GetName() override { return exp->name; };
        uint64_t GetNamespace() override { return exp->name_space; };
        uint64_t GetFileNamespace() override { return exp->data; };
        uint64_t GetChecksum() override { return exp->checksum; };
        uint32_t GetAddress() override { return exp->address; };
        uint8_t GetParamCount() override { return exp->param_count; };
        uint8_t GetFlags() override { return exp->flags; };
        size_t SizeOf() override { return sizeof(*exp); };
    };

    struct H64CER2GSCExportReader : GSCExportReader {
        IW24GSCExport2* exp{};

        void SetHandle(void* handle) override { exp = (IW24GSCExport2*)handle; };
        uint64_t GetName() override { return exp->name; };
        uint64_t GetNamespace() override { return exp->name_space; };
        uint64_t GetFileNamespace() override { return exp->file_name_space; };
        uint64_t GetChecksum() override { return exp->checksum; };
        uint32_t GetAddress() override { return exp->address; };
        uint8_t GetParamCount() override { return exp->param_count; };
        uint8_t GetFlags() override { return exp->flags; };
        size_t SizeOf() override { return sizeof(*exp); };
    };


    std::unique_ptr<GSCExportReader> CreateExportReader(VmInfo* vmInfo) {
        if (vmInfo->HasFlag(VmFlags::VMF_HASH_ACTS)) {
            return std::make_unique<ACTSExportReader>();
        }
        if (vmInfo->HasFlag(VmFlags::VMF_GSCBIN)) {
            return std::make_unique<BINGSCExportReader>();
        }
        if (vmInfo->HasFlag(VmFlags::VMF_HASH64 | VmFlags::VMF_EXPORT_NOCHECKSUM)) {
            return std::make_unique<H64CERGSCExportReader>();
        }
        else if (vmInfo->HasFlag(VmFlags::VMF_HASH64)) {
            if (vmInfo->HasFlag(VmFlags::VMF_EXPORT_CRC32)) {
                return std::make_unique<H64CER2GSCExportReader>();
            }
            else {
                return std::make_unique<H64GSCExportReader>();
            }
        }
        else if (vmInfo->flags & VmFlags::VMF_NO_FILE_NAMESPACE) {
            return std::make_unique<H32T7GSCExportReader>();
        }
        else {
            return std::make_unique<H32GSCExportReader>();
        }
    }
}
