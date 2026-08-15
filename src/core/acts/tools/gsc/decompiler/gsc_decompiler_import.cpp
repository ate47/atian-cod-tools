#include <includes.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_import.hpp>
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/gsc_iw.hpp>
#include <acts_vm.hpp>

namespace tool::gsc {
    using namespace opcode;

    struct H32T8GSCImportReader : GSCImportReader {
        T8GSCImport* imp{};

        void SetHandle(void* handle) override { imp = (T8GSCImport*)handle; }

        uint64_t GetName() override { return imp->name; }
        uint64_t GetNamespace() override { return imp->import_namespace; }
        uint8_t GetFlags() override { return imp->flags; }
        size_t SizeOf() override { return sizeof(*imp); }
        uint8_t GetParamCount() override { return imp->param_count; }
        uint16_t GetNumAddresses() override { return imp->num_address; }
        uint32_t* GetAddresses() override { return (uint32_t*)&imp[1]; }
        void SetName(uint64_t name) override { imp->name = name; }
        void SetNamespace(uint64_t nameSpace) override { imp->import_namespace = nameSpace; }
        void SetFlags(uint8_t flags) override { imp->flags = flags; }
        void SetParamCount(uint8_t paramCount) override { imp->param_count = paramCount; }

        void ReadGeneric(IW23GSCImport& imp) override {
            imp = IW23GSCImport{
                .name = this->imp->name,
                .name_space = this->imp->import_namespace,
                .num_address = this->imp->num_address,
                .param_count = this->imp->param_count,
                .flags = this->imp->flags,
            };
        }
    };

    struct H64IW23GSCImportReader : GSCImportReader {
        IW23GSCImport* imp{};

        void SetHandle(void* handle) override { imp = (IW23GSCImport*)handle; };

        uint64_t GetName() override { return imp->name; }
        uint64_t GetNamespace() override { return imp->name_space; }
        uint8_t GetFlags() override { return imp->flags; }
        size_t SizeOf() override { return sizeof(*imp); }
        uint8_t GetParamCount() override { return imp->param_count; }
        uint16_t GetNumAddresses() override { return imp->num_address; }
        uint32_t* GetAddresses() override { return (uint32_t*)&imp[1]; }
        void SetName(uint64_t name) override { imp->name = name; }
        void SetNamespace(uint64_t nameSpace) override { imp->name_space = nameSpace; }
        void SetFlags(uint8_t flags) override { imp->flags = flags; }
        void SetParamCount(uint8_t paramCount) override { imp->param_count = paramCount; }
        void ReadGeneric(IW23GSCImport& imp) override { imp = *this->imp; }
    };

    std::unique_ptr<GSCImportReader> CreateImportReader(tool::gsc::opcode::VmInfo* vmInfo) {
        if (vmInfo->HasFlag(VmFlags::VMF_IW_LIKE)) {
            return std::make_unique<H64IW23GSCImportReader>();
        } else {
            return std::make_unique<H32T8GSCImportReader>();
        }
    }

} // namespace tool::gsc