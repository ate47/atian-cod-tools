#include <includes.hpp>
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include <pool.hpp>

namespace {
    int gschook(Process& _, int argc, const char* argv[]) {
        using namespace tool::gsc::opcode;

        if (argc < 4) {
            return tool::BAD_USAGE;
        }

        std::string mainBuff{};

        if (!utils::ReadFile(argv[2], mainBuff) || mainBuff.size() < sizeof(uint64_t)) {
            LOG_ERROR("Can't read {}", argv[2]);
            return tool::BASIC_ERROR;
        }

        uint64_t magic = *(uint64_t*)mainBuff.data();

        VmInfo* vmInfo{};
        
        if (!IsValidVmMagic(magic, vmInfo)) {
            LOG_ERROR("Unknown magic: {:x}!", magic);
            return false;
        }
        
        auto* readerBuilder = tool::gsc::GetGscReader(vmInfo->vm);

        if (!readerBuilder) {
            LOG_ERROR("No GSC handler available for {}", vmInfo->name);
            return tool::BASIC_ERROR;
        }

        std::shared_ptr<tool::gsc::GSCOBJHandler> handler{ (*readerBuilder)((byte*)mainBuff.data(), mainBuff.size()) };

        if (!handler->IsValidHeader(mainBuff.size())) {
            LOG_ERROR("Invalid header for vm {}", vmInfo->name);
            return tool::BASIC_ERROR;
        }

        std::unique_ptr<tool::gsc::GSCExportReader> exportReader = tool::gsc::CreateExportReader(vmInfo);

        uint64_t name = handler->GetName();

        // read index

        std::unordered_map<uint64_t, std::unordered_set<uint64_t>> dataset{};
        std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::unordered_set<uint64_t>>> used{};

        for (size_t i = 0; i < handler->GetExportsCount(); i++) {
            byte* exp = handler->Ptr(handler->GetExportsOffset() + i * handler->GetExportSize());
            exportReader->SetHandle(exp);

            
            dataset[exportReader->GetNamespace()].insert(exportReader->GetName());
        }

        std::vector<std::filesystem::path> files{};


        utils::GetFileRecurse(argv[3], files, [](const std::filesystem::path& p) {
            auto s = p.string();
            return s.ends_with(".gscc") || s.ends_with(".cscc");
        });

        hashutils::ReadDefaultFile();
        std::string fileBuff{};
        for (const std::filesystem::path& p : files) {
            if (!utils::ReadFile(p, fileBuff)) {
                LOG_WARNING("Can't read {}, ignore", p.string());
                continue;
            }

            handler->file = (byte*)fileBuff.data();
            handler->fileSize = fileBuff.size();

            if (!handler->IsValidHeader(fileBuff.size())) {
                LOG_WARNING("Invalid magic {}, ignore", p.string());
                continue;
            }

            uint64_t* usings = handler->Ptr<uint64_t>(handler->GetIncludesOffset());
            uint64_t* usingsEnd = usings + handler->GetIncludesCount();

            if (std::find(usings, usingsEnd, name) == usingsEnd) {
                continue; // not included
            }
            LOG_INFO("Loading {}...", p.string());


            uintptr_t import_location = reinterpret_cast<uintptr_t>(handler->Ptr(handler->GetImportsOffset()));
            for (size_t i = 0; i < handler->GetImportsCount(); i++) {
                uint64_t name_space;
                uint64_t name;
                size_t impSize;
                byte flags;
                uint16_t numAddress;

                if (vmInfo->flags & VmFlags::VMF_HASH64) {
                    const auto* imp = reinterpret_cast<tool::gsc::IW23GSCImport*>(import_location);
                    name_space = imp->name_space;
                    name = imp->name;
                    flags = imp->flags;
                    numAddress = imp->num_address;
                    impSize = sizeof(*imp);
                }
                else {
                    const auto* imp = reinterpret_cast<tool::gsc::T8GSCImport*>(import_location);
                    name_space = imp->import_namespace;
                    name = imp->name;
                    flags = imp->flags;
                    numAddress = imp->num_address;
                    impSize = sizeof(*imp);
                }

                byte remapedFlags = handler->RemapFlagsImport(flags);


                auto it = dataset.find(name_space);

                if (it != dataset.end() && it->second.contains(name)) {
                    if (!(remapedFlags & tool::gsc::T8GSCImportFlags::DEV_CALL)) {
                        // not a dev call we can check it
                        used[handler->GetName()][name_space].insert(name);
                    }
                    else {
                        LOG_DEBUG("devcall {} {}::{}", hashutils::ExtractTmpScript(handler->GetName()), hashutils::ExtractTmp("namespace", name_space), hashutils::ExtractTmp("function", name));
                    }
                }
                 import_location += impSize + sizeof(uint32_t) * numAddress;
            }
        }


        for (const auto& [script, usedd] : used) {
            for (const auto& [ns, names] : usedd) {
                for (uint64_t name : names) {
                    LOG_INFO("{} {}::{}", hashutils::ExtractTmpScript(script), hashutils::ExtractTmp("namespace", ns), hashutils::ExtractTmp("function", name));
                }
            }
        }


        return tool::OK;
    }


    ADD_TOOL("gschook", "gsc", " [base] [dump]", "find all the linked functions of a script from a dump", nullptr, gschook);
}
