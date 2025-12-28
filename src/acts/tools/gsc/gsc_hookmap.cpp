#include <includes.hpp>
#include <core/memory_allocator.hpp>
#include <tools/bo4/dump.hpp>
#include "tools/gsc/gsc.hpp"

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
        
        auto* readerBuilder = tool::gsc::GetGscReader(vmInfo->vmMagic);

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
        uint64_t checksum{};

        if (handler->HasFlag(tool::gsc::GOHF_NOTIFY_CRC) || handler->HasFlag(tool::gsc::GOHF_NOTIFY_CRC_STRING)) {
            checksum = handler->GetChecksum();
        }

        std::unordered_map<uint64_t, std::unordered_set<uint64_t>> dataset{};
        std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::unordered_set<uint64_t>>> used{};

        for (size_t i = 0; i < handler->GetExportsCount(); i++) {
            byte* exp = handler->Ptr(handler->GetExportsOffset() + i * handler->GetExportSize());
            exportReader->SetHandle(exp);


            dataset[exportReader->GetNamespace()].insert(exportReader->GetName());
            if (!vmInfo->HasFlag(VmFlags::VMF_NO_FILE_NAMESPACE)) {
                dataset[exportReader->GetFileNamespace()].insert(exportReader->GetName());
            }
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

            handler->SetFile((byte*)fileBuff.data(), fileBuff.size());


            if (!handler->IsValidHeader(fileBuff.size())) {
                LOG_TRACE("Invalid magic {}, ignore {:x} != {:x}", p.string(), magic, *(uint64_t*)fileBuff.data());
                continue;
            }
            if (handler->GetName() == name) {
                continue; // same file
            }

            uint64_t* usings = handler->Ptr<uint64_t>(handler->GetIncludesOffset());
            uint64_t* usingsEnd = usings + handler->GetIncludesCount();

            if (std::find(usings, usingsEnd, name) == usingsEnd) {
                continue; // not included
            }
            LOG_INFO("Loading {} ({})...", p.string(), hashutils::ExtractTmpScript(handler->GetName()));


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

        LOG_INFO("**** Required data ****");

        if (checksum) {
            LOG_INFO("crc .. 0x{:x} / {}", checksum, checksum);
        }
        LOG_INFO("name . {} ({:x})", hashutils::ExtractTmpScript(name), name);

        if (!used.empty()) {
            LOG_INFO("links:");

            for (const auto& [script, usedd] : used) {
                for (const auto& [ns, names] : usedd) {
                    for (uint64_t name : names) {
                        LOG_INFO("{}::{} (from {})", hashutils::ExtractTmp("namespace", ns), hashutils::ExtractTmp("function", name), hashutils::ExtractTmpScript(script));
                    }
                }
            }
        }


        return tool::OK;
    }

    int gsclerr(Process& _, int argc, const char* argv[]) {
        using namespace tool::gsc::opcode;

        if (argc < 3) {
            return tool::BAD_USAGE;
        }

        std::vector<std::filesystem::path> files{};

        for (size_t i = 2; i < argc; i++) {
            utils::GetFileRecurse(argv[2], files, [](const std::filesystem::path& p) {
                auto s = p.string();
                return s.ends_with(".gscc") || s.ends_with(".cscc");
            });
        }

        if (files.empty()) {
            LOG_ERROR("No files");
            return tool::BASIC_ERROR;
        }

        // vm -> script
        struct ScriptData {
            uint64_t name{};
            uint64_t vmMagic{};
            void* bufferData{};
            size_t bufferDataLen{};
            std::unordered_map<uint64_t, uint64_t> exports{};
            std::unordered_map<uint64_t, uint64_t> exportsFNS{};
            bool dsResolved{};
            std::filesystem::path fileSys{};
        };
        std::unordered_map<VmInfo*, std::unordered_map<uint64_t, ScriptData>> dataset{};

        core::memory_allocator::MemoryAllocator alloc{};

        std::string mainBuff{};

        hashutils::ReadDefaultFile();
        LOG_INFO("Reading dump...");


        for (const std::filesystem::path& file : files) {
            if (!utils::ReadFile(file, mainBuff) || mainBuff.size() < sizeof(uint64_t)) {
                LOG_ERROR("{} : Can't read", file.string());
                continue;
            }

            uint64_t magic = *(uint64_t*)mainBuff.data();

            VmInfo* vmInfo{};

            if (!IsValidVmMagic(magic, vmInfo)) {
                LOG_ERROR("{} : Unknown magic: {:x}!", file.string(), magic);
                continue;
            }

            auto* readerBuilder = tool::gsc::GetGscReader(vmInfo->vmMagic);

            if (!readerBuilder) {
                LOG_ERROR("{} : No GSC handler available for {}", file.string(), vmInfo->name);
                continue;
            }

            std::shared_ptr<tool::gsc::GSCOBJHandler> handler{ (*readerBuilder)((byte*)mainBuff.data(), mainBuff.size()) };

            if (!handler->IsValidHeader(mainBuff.size())) {
                LOG_ERROR("{} : Invalid header for vm {}", file.string(), vmInfo->name);
                continue;
            }

            uint64_t name = handler->GetName();
            void* ptr = alloc.Alloc(mainBuff);

            ScriptData& sc = dataset[vmInfo][name];

            sc.name = name;
            sc.bufferData = ptr;
            sc.bufferDataLen = mainBuff.length();
            sc.vmMagic = vmInfo->vmMagic;
            sc.fileSys = file;
            LOG_TRACE("Loaded {} ({})", file.string(), hashutils::ExtractTmpScript(name));
        }

        if (dataset.empty()) {
            LOG_ERROR("No script");
            return tool::BASIC_ERROR;
        }

        for (auto& [vmInfo, scs] : dataset) {
            LOG_INFO("Searching linking issues for {}...", vmInfo->name);

            auto* readerBuilder = tool::gsc::GetGscReader(vmInfo->vmMagic);

            if (!readerBuilder) {
                LOG_ERROR("No GSC handler available for {}", vmInfo->name);
                continue;
            }

            std::shared_ptr<tool::gsc::GSCOBJHandler> handler{ (*readerBuilder)(nullptr, 0) };
            std::unique_ptr<tool::gsc::GSCExportReader> exportReader = tool::gsc::CreateExportReader(vmInfo);

            for (auto& [name, sc] : scs) {
                handler->SetFile((byte*)sc.bufferData, sc.bufferDataLen);

                uint64_t* usings = handler->Ptr<uint64_t>(handler->GetIncludesOffset());
                uint64_t* usingsEnd = usings + handler->GetIncludesCount();

                auto ResolveDataSet = [&handler, &exportReader](ScriptData& d) {
                    if (d.dsResolved) {
                        return; // already resolved
                    }

                    handler->SetFile((byte*)d.bufferData, d.bufferDataLen);

                    byte* exports = handler->Ptr(handler->GetExportsOffset());

                    // resolve the exports dataset
                    for (size_t i = 0; i < handler->GetExportsCount(); i++) {
                        exportReader->SetHandle(exports + handler->GetExportSize() * i);

                        //d.exportsFNS[exportReader->GetFileNamespace()] = exportReader->GetName();
                        d.exports[exportReader->GetNamespace()] = exportReader->GetName();
                    }

                    d.dsResolved = true;
                };

                // resolve current scripts, can be done during reading??
                ResolveDataSet(sc);

                for (; usings != usingsEnd; usings++) {
                    auto iti = scs.find(*usings);

                    if (iti == scs.end()) {
                        LOG_ERROR("{} : The include '{}' is missing", sc.fileSys.string(), hashutils::ExtractTmpScript(*usings));
                        continue; // can't explore
                    }

                    ScriptData& d = iti->second;

                    ResolveDataSet(d);
                }

                handler->SetFile((byte*)sc.bufferData, sc.bufferDataLen);
                
                // read imports

                uintptr_t import_location = reinterpret_cast<uintptr_t>(handler->Ptr(handler->GetImportsOffset()));
                for (size_t i = 0; i < handler->GetImportsCount(); i++) {
                    uint64_t name_space;
                    uint64_t name;
                    size_t impSize;
                    byte flags;
                    uint16_t numAddress;

                    if (vmInfo->HasFlag(VmFlags::VMF_HASH64)) {
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

                    if (!(remapedFlags & tool::gsc::T8GSCImportFlags::DEV_CALL)) {
                        // ignore dev call

                        // fixme: the mwiii vm is using file namespace, it is not considered

                        int calltype = remapedFlags & tool::gsc::T8GSCImportFlags::CALLTYPE_MASK;

                       
                        // todo: split script/engine function in remapedFlags or allow to use a dump
                        //       of the engine functions.


                    }

                    import_location += impSize + sizeof(uint32_t) * numAddress;
                }
            }

        }


        return tool::OK;
    }

    ADD_TOOL(gschook, "gsc", " [base] [dump]", "find all the linked functions of a script from a dump", nullptr, gschook);
#ifndef CI_BUILD
    ADD_TOOL(gsclerr, "gsc", " [dump]", "find all the link errors in a gsc dump", nullptr, gsclerr);
#endif
}
