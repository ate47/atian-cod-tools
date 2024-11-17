#include <includes.hpp>
#include <compatibility/xensik_gscbin.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>
#include <tools/gsc_iw.hpp>


// GSCBIN

namespace {
    using namespace tool::gsc;
    using namespace tool::gsc::iw;
    using namespace tool::gsc::opcode;

    struct FakeLinkHeader {
        // what is the easiest way to convert a stream based script loading to a linked script loading?
        // idk, but today it'll be to read the stream based script into a fake linked script and then
        // reading it. (It's dumb, yeah)
        uint64_t name;
        size_t exports_table;
        size_t exports_count;

        compatibility::xensik::gscbin::GscBinHeader binHeader{};
    };

    class IWGSCOBJHandler : public GSCOBJHandler {
    public:

        FakeLinkHeader fakeHeader{};
        std::vector<byte> fakeLinked{};
        IWGSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_IW_BIN) {}

        void DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt) override {
            compatibility::xensik::gscbin::GscBinHeader& data{ fakeHeader.binHeader };
            asmout
                << "// bytecode . 0x" << std::hex << data.bytecodeLen << "\n"
                << "// buffer ... 0x" << std::hex << data.len << " / Compressed: 0x" << data.compressedLen << "\n"
                ;

        }

        int PreLoadCode() override {
            compatibility::xensik::gscbin::GscBinHeader& header{ Ref<compatibility::xensik::gscbin::GscBinHeader>() };

            auto decompressedData{ std::make_unique<byte[]>(header.len) };

            uLongf sizef = (uLongf)header.len;
            uLongf sizef2{ header.compressedLen };
            int ret;
            if (header.len && (ret = uncompress2(decompressedData.get(), &sizef, reinterpret_cast<const Bytef*>(header.GetBuffer()), &sizef2) < 0)) {
                std::string fileName{ originalFile ? originalFile->string() : "<unk>" };
                throw std::runtime_error(utils::va("Can't decompress file %s: %s", fileName.c_str(), zError(ret)));
            }

            std::vector<tool::gsc::iw::BINGSCExport> functions{};

            int64_t sizeRead{ sizef };
            uint32_t locByteCode{ 1 };

            uint32_t start{ sizeof(FakeLinkHeader) };

            while (sizeRead > 0 && locByteCode < header.bytecodeLen) {
                functions.emplace_back();
                tool::gsc::iw::BINGSCExport& func{ functions[functions.size() - 1] };

                func.address = start + locByteCode;
                func.size = *(uint32_t*)&decompressedData[sizeRead];
                sizeRead -= sizeof(uint32_t);
                locByteCode += func.size;
                uint32_t idRef = *(uint32_t*)&decompressedData[sizeRead];
                sizeRead -= sizeof(uint32_t);
                if (idRef) {
                    const char* nameRef{ utils::va("ref_%x", idRef) };
                    uint64_t hash{ hash::Hash64(nameRef) };
                    hashutils::AddPrecomputed(hash, nameRef);
                    func.name = hash;
                }
                else {
                    const char* nameStr{ (const char*)&decompressedData[sizeRead] };
                    size_t nameStrLen{ std::strlen(nameStr) };
                    sizeRead -= nameStrLen + 1;
                    uint64_t hash{ hash::Hash64(nameStr) };
                    hashutils::AddPrecomputed(hash, nameStr);
                    func.name = hash;
                }
            }

            fakeLinked.clear();
            fakeLinked.reserve(header.len + sizeof(functions[0]) * functions.size());

            size_t bytecode{ utils::WriteValue(fakeLinked, header.GetByteCode(), header.bytecodeLen)};
            size_t exports{ utils::WriteValue(fakeLinked, functions.data(), sizeof(functions[0]) * functions.size()) };

            fakeHeader.exports_count = functions.size();
            fakeHeader.exports_table = exports;

            if (originalFile) {
                std::string fn = originalFile->string();

                if (originalFile->has_extension()) {
                    size_t len = originalFile->extension().string().length();
                    fn = fn.substr(0, fn.length() - len);
                }

                fn = fn + ".gsc";

                const char* name{ fn.data() };
                hashutils::Add(name);
                fakeHeader.name = hash::Hash64(name);
            }
            fakeHeader.binHeader = header;
            SetFile(fakeLinked.data(), fakeLinked.size());

            return GSCOBJHandler::PreLoadCode();
        }

        uint64_t GetName() override {
            return fakeHeader.name;
        }
        uint16_t GetExportsCount() override {
            return (uint16_t)fakeHeader.exports_count;
        }
        uint32_t GetExportsOffset() override {
            return (uint32_t)fakeHeader.exports_table;
        }
        uint16_t GetIncludesCount() override {
            return 0;
        }
        uint32_t GetIncludesOffset() override {
            return 0;
        }
        uint16_t GetImportsCount() override {
            return 0;
        }
        uint32_t GetImportsOffset() override {
            return 0;
        }
        uint16_t GetGVarsCount() override {
            return 0;
        }
        uint32_t GetGVarsOffset() override {
            return 0;
        }
        uint16_t GetStringsCount() override {
            return 0;
        }
        uint32_t GetStringsOffset() override {
            return 0;
        }
        uint16_t GetDevStringsCount() override {
            return 0;
        }
        uint32_t GetDevStringsOffset() override {
            return 0;
        }
        size_t GetHeaderSize() override {
            return sizeof(compatibility::xensik::gscbin::GscBinHeader);
        }
        char* DecryptString(char* str) override {
            return str;
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(compatibility::xensik::gscbin::GscBinHeader) && Ref<uint32_t>() == compatibility::xensik::gscbin::GSCBIN_MAGIC;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return 0;
        };
        uint32_t GetAnimTreeSingleOffset() override {
            return 0;
        };
        uint16_t GetAnimTreeDoubleCount() override {
            return 0;
        };
        uint32_t GetAnimTreeDoubleOffset() override {
            return 0;
        };

        // no name
        void SetName(uint64_t name) override {}
        void SetNameString(uint32_t name) override {}

        void SetHeader() override {
            Ref<uint32_t>() = compatibility::xensik::gscbin::GSCBIN_MAGIC;
        }
        void SetExportsCount(uint16_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetExportsOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetIncludesCount(uint16_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetIncludesOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetImportsCount(uint16_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetImportsOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetStringsCount(uint16_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetStringsOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetDevStringsCount(uint16_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetDevStringsOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetGVarsCount(uint16_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetGVarsOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetFileSize(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetCSEGOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetCSEGSize(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        uint32_t GetCSEGOffset() override {
            return 0;
        }
        uint32_t GetCSEGSize() override {
            return 0;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {}
        void SetAnimTreeSingleOffset(uint32_t val) override {}
        void SetAnimTreeDoubleCount(uint16_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetAnimTreeDoubleOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        size_t GetImportSize() override {
            return sizeof(tool::gsc::T8GSCImport);
        }
        size_t GetExportSize() override {
            return sizeof(tool::gsc::T7GSCExport);
        }
        size_t GetStringSize() override {
            return sizeof(tool::gsc::T8GSCString);
        }
        size_t GetGVarSize() override {
            return 0;
        }
        size_t GetAnimTreeSingleSize() override {
            return 0;
        }
        size_t GetAnimTreeDoubleSize() override {
            return sizeof(tool::gsc::T7GscAnimTree);
        }
        void WriteExport(byte* data, const tool::gsc::IW23GSCExport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::T7GSCExport*>(data);
            imp.name = (uint32_t)item.name;
            imp.name_space = (uint32_t)item.name_space;
            imp.checksum = (uint32_t)item.checksum;
            imp.flags = item.flags;
            imp.address = item.address;
            imp.param_count = item.param_count;
        }
        void WriteImport(byte* data, const tool::gsc::IW23GSCImport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::T8GSCImport*>(data);
            imp.name = (uint32_t)item.name;
            imp.import_namespace = (uint32_t)item.name_space;
            imp.flags = item.flags;
            imp.num_address = item.num_address;
            imp.param_count = item.param_count;
        }
        void WriteGVar(byte* data, const tool::gsc::T8GSCGlobalVar& item) override {}
        void WriteString(byte* data, const tool::gsc::T8GSCString& item) override {
            *reinterpret_cast<tool::gsc::T8GSCString*>(data) = item;
        }
        void WriteAnimTreeSingle(byte* data, const tool::gsc::GSC_USEANIMTREE_ITEM& item) override { }
        void WriteAnimTreeDouble(byte* data, const tool::gsc::GSC_ANIMTREE_ITEM& item) override { }
        int64_t GetDefaultChecksum(bool client) override {
            return 0x1234567;
        }
        void SetChecksum(uint64_t val) override {

        }
        uint32_t GetChecksum() override {
            return 0;
        }
        const char* GetDefaultName(bool client) override {
            return "";
        }

        bool IsVTableImportFlags(byte flags) override {
            return false;
        }
        byte GetVTableImportFlags() override {
            return 0;
        }
    };
}

REGISTER_GSC_VM(VMI_IW_GSCBIN, IWGSCOBJHandler);