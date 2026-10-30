#include <includes.hpp>
#include <tools/mcp/mcp_tool.hpp>
#include <tools/gsc/gsc_opcodes.hpp>
#include <tools/gsc/gsc_decompiler.hpp>
#include <tools/gsc/gsc_vm.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_export.hpp>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace {
    using tool::gsc::opcode::OPCode;
    using tool::gsc::opcode::Platform;
    using tool::gsc::opcode::VmInfo;

    std::string list_gsc_vm(const rapidjson::Value& args, bool& isError) {
        rapidjson::StringBuffer sb{};
        rapidjson::Writer<rapidjson::StringBuffer> writer{ sb };

        writer.StartArray();
        for (const std::pair<const uint64_t, tool::gsc::opcode::VmInfo>& vm : tool::gsc::opcode::GetVMMaps()) {
            const tool::gsc::opcode::VmInfo& info{ vm.second };
            if (!info.vmMagic || !info.internalName || !info.name) {
                continue;
            }
            writer.StartObject();
            writer.Key("internalName");
            writer.String(info.internalName);
            writer.Key("name");
            writer.String(info.name);
            writer.Key("platforms");
            writer.StartArray();
            for (size_t i = 1; i < tool::gsc::opcode::PLATFORM_COUNT; i++) {
                tool::gsc::opcode::Platform plt{ (tool::gsc::opcode::Platform)i };
                if (info.HasPlatform(plt)) {
                    writer.String(tool::gsc::opcode::PlatformIdName(plt));
                }
            }
            writer.EndArray();
            writer.EndObject();
        }
        writer.EndArray();
        return std::string{ sb.GetString(), sb.GetSize() };
    }

    // Get a string argument, empty if missing
    const char* GetStringArg(const rapidjson::Value& args, const char* key) {
        if (!args.IsObject() || !args.HasMember(key) || !args[key].IsString()) {
            return nullptr;
        }
        return args[key].GetString();
    }

    // Find a VM by its internal name
    VmInfo* FindVmByInternalName(const char* internalName) {
        for (const std::pair<const uint64_t, VmInfo>& vm : tool::gsc::opcode::GetVMMaps()) {
            if (vm.second.vmMagic && vm.second.internalName && !_strcmpi(vm.second.internalName, internalName)) {
                VmInfo* info{};
                if (tool::gsc::opcode::IsValidVm(vm.first, info, false)) {
                    return info;
                }
            }
        }
        return nullptr;
    }

    // Read the 'vm' (internalName) and 'platform' arguments
    bool ReadVmPlatform(const rapidjson::Value& args, VmInfo*& info, Platform& plt, std::string& err) {
        const char* vmName{ GetStringArg(args, "vm") };
        if (!vmName) {
            err = "Missing 'vm' argument";
            return false;
        }
        info = FindVmByInternalName(vmName);
        if (!info) {
            err = std::format("Unknown vm '{}', use list_gsc_vm to get the internalName", vmName);
            return false;
        }
        const char* pltName{ GetStringArg(args, "platform") };
        if (!pltName) {
            err = "Missing 'platform' argument";
            return false;
        }
        plt = tool::gsc::opcode::PlatformOf(pltName);
        if (plt == tool::gsc::opcode::PLATFORM_UNKNOWN) {
            err = std::format("Unknown platform '{}', use list_gsc_vm to get the platforms", pltName);
            return false;
        }
        return true;
    }

    bool ReadOpCodeValue(const rapidjson::Value& val, VmInfo* info, uint16_t& out) {
        if (!val.IsUint() || val.GetUint() >= info->maxOpCode) {
            return false;
        }
        out = (uint16_t)val.GetUint();
        return true;
    }

    std::string ToJson(rapidjson::StringBuffer& sb) { return std::string{ sb.GetString(), sb.GetSize() }; }

    std::string list_gsc_opcode_names(const rapidjson::Value& args, bool& isError) {
        const char* filter{ GetStringArg(args, "filter") };
        std::string lowerFilter{ filter ? filter : "" };
        std::transform(lowerFilter.begin(), lowerFilter.end(), lowerFilter.begin(), [](unsigned char c) {
            return (char)std::tolower(c);
        });

        rapidjson::StringBuffer sb{};
        rapidjson::Writer<rapidjson::StringBuffer> writer{ sb };

        writer.StartArray();
        for (size_t i = 0; i < tool::gsc::opcode::OPCODE_COUNT; i++) {
            const char* name{ tool::gsc::opcode::OpCodeName((OPCode)i) };
            if (!name) {
                continue;
            }
            if (!lowerFilter.empty()) {
                std::string lowerName{ name };
                std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char c) {
                    return (char)std::tolower(c);
                });
                if (lowerName.find(lowerFilter) == std::string::npos) {
                    continue;
                }
            }
            writer.String(name);
        }
        writer.EndArray();
        return ToJson(sb);
    }

    std::string get_gsc_vm_opcodes(const rapidjson::Value& args, bool& isError) {
        VmInfo* info{};
        Platform plt{};
        std::string err;
        if (!ReadVmPlatform(args, info, plt, err)) {
            isError = true;
            return err;
        }

        std::map<std::string, std::vector<uint16_t>> opcodes{};
        std::unordered_map<Platform, std::unordered_map<OPCode, std::unordered_set<uint16_t>>>::const_iterator pltIt{
            info->opcodemappltlookup.find(info->RemapSamePlatform(plt))
        };
        if (pltIt != info->opcodemappltlookup.end()) {
            for (const std::pair<const OPCode, std::unordered_set<uint16_t>>& op : pltIt->second) {
                if (op.first == tool::gsc::opcode::OPCODE_Undefined) {
                    continue;
                }
                const char* name{ tool::gsc::opcode::OpCodeName(op.first) };
                if (!name) {
                    continue;
                }
                std::vector<uint16_t>& values{ opcodes[name] };
                values.assign(op.second.begin(), op.second.end());
                std::sort(values.begin(), values.end());
            }
        }

        rapidjson::StringBuffer sb{};
        rapidjson::Writer<rapidjson::StringBuffer> writer{ sb };
        writer.StartArray();
        for (const std::pair<const std::string, std::vector<uint16_t>>& op : opcodes) {
            writer.StartObject();
            writer.Key("opcode");
            writer.String(op.first.c_str());
            writer.Key("values");
            writer.StartArray();
            for (uint16_t value : op.second) {
                writer.Uint(value);
            }
            writer.EndArray();
            writer.EndObject();
        }
        writer.EndArray();
        return ToJson(sb);
    }

    std::string dump_gsc_vm_opcodes(const rapidjson::Value& args, bool& isError) {
        VmInfo* info{};
        Platform plt{};
        std::string err;
        if (!ReadVmPlatform(args, info, plt, err)) {
            isError = true;
            return err;
        }

        // same format as the dop tool: VM,PLATFORM,OPCODE,VALUES
        std::string vmName{ info->codeName };
        std::string pltName{ tool::gsc::opcode::PlatformName(plt) };
        std::transform(vmName.begin(), vmName.end(), vmName.begin(), [](unsigned char c) {
            return (char)std::toupper(c);
        });
        std::transform(pltName.begin(), pltName.end(), pltName.begin(), [](unsigned char c) {
            return (char)std::toupper(c);
        });

        std::map<std::string, std::vector<uint16_t>> opcodes{};
        std::unordered_set<uint16_t> defined{};
        std::unordered_map<Platform, std::unordered_map<OPCode, std::unordered_set<uint16_t>>>::const_iterator pltIt{
            info->opcodemappltlookup.find(info->RemapSamePlatform(plt))
        };
        if (pltIt != info->opcodemappltlookup.end()) {
            for (const std::pair<const OPCode, std::unordered_set<uint16_t>>& op : pltIt->second) {
                // removed values are registered as undefined
                if (op.first == tool::gsc::opcode::OPCODE_Undefined) {
                    continue;
                }
                const char* name{ tool::gsc::opcode::OpCodeName(op.first) };
                std::vector<uint16_t>& values{ opcodes[name ? name : "<error>"] };
                values.assign(op.second.begin(), op.second.end());
                std::sort(values.begin(), values.end());
                defined.insert(op.second.begin(), op.second.end());
            }
        }

        std::string csv{ "VM,PLATFORM,OPCODE,VALUES" };
        for (const std::pair<const std::string, std::vector<uint16_t>>& op : opcodes) {
            csv += std::format("\n{},{},{},", vmName, pltName, op.first);
            for (size_t i = 0; i < op.second.size(); i++) {
                csv += std::format("{}0x{:x}", i ? ";" : "", op.second[i]);
            }
        }
        if (defined.size() < info->maxOpCode) {
            csv += std::format("\n{},{},Undefined,", vmName, pltName);
            bool first{ true };
            for (size_t i = 0; i <= info->maxOpCode; i++) {
                if (!defined.contains((uint16_t)i)) {
                    csv += std::format("{}0x{:x}", first ? "" : ";", i);
                    first = false;
                }
            }
        }

        const char* output{ GetStringArg(args, "output") };
        if (!output) {
            return csv;
        }
        if (!utils::WriteFile(output, csv)) {
            isError = true;
            return std::format("Can't open the output file '{}'", output);
        }
        return std::format("{} opcode(s) dumped into '{}'", opcodes.size(), output);
    }

    std::string set_gsc_vm_opcodes(const rapidjson::Value& args, bool& isError) {
        VmInfo* info{};
        Platform plt{};
        std::string err;
        if (!ReadVmPlatform(args, info, plt, err)) {
            isError = true;
            return err;
        }
        if (!args.HasMember("opcodes") || !args["opcodes"].IsArray()) {
            isError = true;
            return "Missing 'opcodes' array argument";
        }

        // validate everything first to not apply a partial change
        std::vector<std::pair<OPCode, uint16_t>> toRegister{};
        for (const rapidjson::Value& entry : args["opcodes"].GetArray()) {
            const char* opName{ GetStringArg(entry, "opcode") };
            if (!opName) {
                isError = true;
                return "Missing 'opcode' name in an opcodes entry";
            }
            OPCode op{ tool::gsc::opcode::OpCodeFromName(opName) };
            if (op == tool::gsc::opcode::OPCODE_Undefined) {
                isError = true;
                return std::format("Unknown opcode '{}', use list_gsc_opcode_names to get the names", opName);
            }
            if (!entry.HasMember("values") || !entry["values"].IsArray()) {
                isError = true;
                return std::format("Missing 'values' array for opcode '{}'", opName);
            }
            for (const rapidjson::Value& valueJson : entry["values"].GetArray()) {
                uint16_t value;
                if (!ReadOpCodeValue(valueJson, info, value)) {
                    isError = true;
                    return std::format("Invalid value for opcode '{}' for vm", opName);
                }
                toRegister.emplace_back(op, value);
            }
        }

        info->AddPlatform(plt);
        for (const std::pair<OPCode, uint16_t>& reg : toRegister) {
            info->RegisterOpCode(plt, reg.first, reg.second);
        }
        return std::format("{} opcode value(s) registered", toRegister.size());
    }

    std::string remove_gsc_vm_opcodes(const rapidjson::Value& args, bool& isError) {
        VmInfo* info{};
        Platform plt{};
        std::string err;
        if (!ReadVmPlatform(args, info, plt, err)) {
            isError = true;
            return err;
        }
        if (!args.HasMember("values") || !args["values"].IsArray()) {
            isError = true;
            return "Missing 'values' array argument";
        }

        std::vector<uint16_t> values{};
        for (const rapidjson::Value& valueJson : args["values"].GetArray()) {
            uint16_t value;
            if (!ReadOpCodeValue(valueJson, info, value)) {
                isError = true;
                return "Invalid value for vm";
            }
            values.push_back(value);
        }

        for (uint16_t value : values) {
            // an undefined opcode makes the value unusable
            info->RegisterOpCode(plt, tool::gsc::opcode::OPCODE_Undefined, value);
        }
        return std::format("{} opcode value(s) removed", values.size());
    }

    // Collect the .gsc/.csc files of a path (a file or a directory)
    bool CollectScriptFiles(
        const char* pathStr, std::vector<std::filesystem::path>& files, std::filesystem::path& base, std::string& err
    ) {
        std::filesystem::path path{ pathStr };
        std::error_code ec;
        if (!std::filesystem::exists(path, ec)) {
            err = std::format("'{}' doesn't exist", pathStr);
            return false;
        }
        if (std::filesystem::is_regular_file(path, ec)) {
            // an explicit file is used whatever its extension is
            base = path.parent_path();
            files.push_back(path);
            return true;
        }
        base = path;
        // the ends are \0 separated, compiled scripts use gscc/cscc
        utils::GetFileRecurseExt(path, files, ".gsc\0.csc\0.gscc\0.cscc\0");
        return true;
    }

    struct ExportFilterDef {
        std::string function;
        std::string nameSpace;
        std::string script;
    };

    // Set the export filters of the decompiler for the vm of a script magic, return false if the vm is unknown
    bool SetExportFilters(
        tool::gsc::GscDecompilerGlobalContext& gdctx, uint64_t magic, const std::vector<ExportFilterDef>& filters
    ) {
        VmInfo* vmInfo{};
        if (!tool::gsc::opcode::IsValidVmMagic(magic, vmInfo)) {
            return false;
        }
        gdctx.exportFilters.clear();
        for (const ExportFilterDef& filter : filters) {
            // the decompiler searches name, name/namespace or name/namespace/script
            tool::gsc::NameLocated loc{};
            loc.name = vmInfo->HashField(filter.function);
            if (!filter.nameSpace.empty()) {
                loc.name_space = vmInfo->HashField(filter.nameSpace);
            }
            if (!filter.script.empty()) {
                loc.script = vmInfo->HashPath(filter.script);
            }
            gdctx.exportFilters.insert(loc);
        }
        return true;
    }

    std::string decompile_gsc_file(const rapidjson::Value& args, bool& isError) {
        const char* path{ GetStringArg(args, "path") };
        if (!path) {
            isError = true;
            return "Missing 'path' argument";
        }

        std::vector<std::filesystem::path> files{};
        std::filesystem::path base{};
        std::string err{};
        if (!CollectScriptFiles(path, files, base, err)) {
            isError = true;
            return err;
        }

        tool::gsc::GscDecompilerGlobalContext gdctx{};
        gdctx.noDump = true;
        gdctx.opt.m_platform = tool::gsc::opcode::PLATFORM_PC;

        const char* pltName{ GetStringArg(args, "platform") };
        if (pltName) {
            Platform plt{ tool::gsc::opcode::PlatformOf(pltName) };
            if (plt == tool::gsc::opcode::PLATFORM_UNKNOWN) {
                isError = true;
                return std::format("Unknown platform '{}'", pltName);
            }
            gdctx.opt.m_platform = plt;
        }

        const char* mode{ GetStringArg(args, "mode") };
        bool status{};
        if (!mode || !_strcmpi(mode, "decompile")) {
            gdctx.opt.m_dcomp = true;
        } else if (!_strcmpi(mode, "asm")) {
            gdctx.opt.m_dasm = true;
        } else if (!_strcmpi(mode, "status")) {
            status = true;
        } else if (!_strcmpi(mode, "both")) {
            gdctx.opt.m_dcomp = true;
            gdctx.opt.m_dasm = true;
        } else {
            isError = true;
            return "Invalid 'mode', expecting decompile, asm, both or status";
        }
        gdctx.opt.m_header = args.HasMember("header") && args["header"].IsBool() && args["header"].GetBool();
        gdctx.opt.m_show_opcode_values =
            !(args.HasMember("hideOpcodeValues") && args["hideOpcodeValues"].IsBool() &&
              args["hideOpcodeValues"].GetBool());

        // exports: [ { function, namespace?, script? } ], the hashes depend on the vm of each file
        std::vector<ExportFilterDef> exportFilters{};
        if (args.HasMember("exports")) {
            if (!args["exports"].IsArray()) {
                isError = true;
                return "'exports' must be an array";
            }
            for (const rapidjson::Value& entry : args["exports"].GetArray()) {
                const char* function{ GetStringArg(entry, "function") };
                if (!function) {
                    isError = true;
                    return "Missing 'function' in an exports entry";
                }
                const char* nameSpace{ GetStringArg(entry, "namespace") };
                const char* script{ GetStringArg(entry, "script") };
                if (script && !nameSpace) {
                    isError = true;
                    return "A 'script' filter requires a 'namespace' in the exports entry";
                }
                exportFilters.push_back({ function, nameSpace ? nameSpace : "", script ? script : "" });
            }
        }

        // the vm is read from the magic of each file
        std::string output{};
        if (!status) {
            gdctx.opt.udWriteAsmFunction = &output;
            gdctx.opt.WriteAsmFunction = [](const char* str, size_t len, void* ud) {
                ((std::string*)ud)->append(str, len);
            };
        }

        // the errors of the decompiler (missing opcodes, severe problems), listed per file
        struct DecompError {
            std::string message;
            tool::gsc::NameLocated origin;
        };
        std::vector<DecompError> decompErrors{};
        gdctx.opt.udWriteErrorFunction = &decompErrors;
        gdctx.opt.WriteErrorFunction = [](const char* str, size_t len, tool::gsc::NameLocated& origin, void* ud) {
            ((std::vector<DecompError>*)ud)->push_back({ std::string{ str, len }, origin });
        };

        // maxErrors: stop the loop when this number of errors is reached (0 = no limit)
        size_t maxErrors{};
        if (args.HasMember("maxErrors")) {
            if (!args["maxErrors"].IsUint()) {
                isError = true;
                return "'maxErrors' must be a positive integer";
            }
            maxErrors = args["maxErrors"].GetUint();
        }

        size_t totalErrors{};
        size_t processed{};
        bool stoppedEarly{};

        rapidjson::StringBuffer sb{};
        rapidjson::Writer<rapidjson::StringBuffer> writer{ sb };

        writer.StartObject();
        writer.Key("files");
        writer.StartArray();
        for (const std::filesystem::path& file : files) {
            if (maxErrors && totalErrors >= maxErrors) {
                stoppedEarly = true;
                break;
            }
            processed++;
            output.clear();
            decompErrors.clear();
            std::string fileErr{};

            std::string buffer{};
            void* bufferAlign{};
            size_t size{};
            if (!utils::ReadFileAlign(file, buffer, bufferAlign, size) || size < 8) {
                fileErr = "Can't read the file";
            } else if (!exportFilters.empty() && !SetExportFilters(gdctx, *(uint64_t*)bufferAlign, exportFilters)) {
                fileErr = std::format("No vm for the magic 0x{:x}", *(uint64_t*)bufferAlign);
            } else {
                try {
                    if (tool::gsc::DecompileGsc(
                            (byte*)bufferAlign,
                            size,
                            std::filesystem::relative(file, base),
                            gdctx
                        ) != tool::OK) {
                        fileErr = "Decompilation error, check the platform and the vm opcodes";
                    }
                } catch (std::runtime_error& e) {
                    fileErr = std::format("Decompilation error: {}", e.what());
                }
            }

            totalErrors += decompErrors.size() + (fileErr.empty() ? 0 : 1);
            if (status && fileErr.empty() && decompErrors.empty()) {
                continue; // status mode only lists the files with errors
            }

            writer.StartObject();
            writer.Key("path");
            writer.String(file.string().c_str());
            writer.Key("success");
            writer.Bool(fileErr.empty());
            if (!fileErr.empty()) {
                writer.Key("error");
                writer.String(fileErr.c_str());
            }
            writer.Key("errors");
            writer.StartArray();
            for (const DecompError& decompError : decompErrors) {
                writer.StartObject();
                writer.Key("message");
                writer.String(decompError.message.c_str(), (rapidjson::SizeType)decompError.message.size());
                writer.Key("function");
                writer.String(hashutils::ExtractTmp("function", decompError.origin.name));
                writer.Key("namespace");
                writer.String(hashutils::ExtractTmp("namespace", decompError.origin.name_space));
                writer.Key("script");
                writer.String(hashutils::ExtractTmp("script", decompError.origin.script));
                writer.EndObject();
            }
            writer.EndArray();
            if (!status) {
                writer.Key("output");
                writer.String(output.c_str(), (rapidjson::SizeType)output.size());
            }
            writer.EndObject();
        }
        writer.EndArray();
        writer.Key("processed");
        writer.Uint64(processed);
        writer.Key("totalFiles");
        writer.Uint64(files.size());
        writer.Key("totalErrors");
        writer.Uint64(totalErrors);
        writer.Key("stoppedEarly");
        writer.Bool(stoppedEarly);
        writer.EndObject();
        return ToJson(sb);
    }

    // An opened script, the handler points to the buffer
    struct ScriptFile {
        std::string buffer{};
        void* bufferAlign{};
        size_t size{};
        std::shared_ptr<tool::gsc::GSCOBJHandler> handler{};
        VmInfo* vmInfo{};
        uint64_t magic{};
    };

    // Open a script, the vm is read from its magic
    bool OpenScript(const std::filesystem::path& file, ScriptFile& script, std::string& err) {
        if (!utils::ReadFileAlign(file, script.buffer, script.bufferAlign, script.size) || script.size < 8) {
            err = "Can't read the file";
            return false;
        }
        script.magic = *(uint64_t*)script.bufferAlign;
        tool::gsc::vm::GscVm* reader{ tool::gsc::vm::GetGscReader(script.magic) };
        if (!reader || !tool::gsc::opcode::IsValidVmMagic(script.magic, script.vmInfo)) {
            err = std::format("No vm for the magic 0x{:x}", script.magic);
            return false;
        }
        try {
            script.handler = reader->NewHandler((byte*)script.bufferAlign, script.size);
            if (!script.handler->IsValidHeader(script.size)) {
                script.handler = nullptr;
                err = "Invalid header";
                return false;
            }
        } catch (std::runtime_error& e) {
            script.handler = nullptr;
            err = std::format("Can't read the script: {}", e.what());
            return false;
        }
        return true;
    }

    struct ScriptCountDef {
        const char* name;
        uint32_t (*getCount)(tool::gsc::GSCOBJHandler& handler);
    };

    // The counts of the script header, usable in the search filters
    const ScriptCountDef scriptCounts[]{
        { "includes", [](tool::gsc::GSCOBJHandler& h) -> uint32_t { return h.GetIncludesCount(); } },
        { "exports", [](tool::gsc::GSCOBJHandler& h) -> uint32_t { return h.GetExportsCount(); } },
        { "imports", [](tool::gsc::GSCOBJHandler& h) -> uint32_t { return h.GetImportsCount(); } },
        { "strings", [](tool::gsc::GSCOBJHandler& h) -> uint32_t { return h.GetStringsCount(); } },
        { "devStrings", [](tool::gsc::GSCOBJHandler& h) -> uint32_t { return h.GetDevStringsCount(); } },
        { "globalVariables", [](tool::gsc::GSCOBJHandler& h) -> uint32_t { return h.GetGVarsCount(); } },
        { "animTreesSingle", [](tool::gsc::GSCOBJHandler& h) -> uint32_t { return h.GetAnimTreeSingleCount(); } },
        { "animTreesDouble", [](tool::gsc::GSCOBJHandler& h) -> uint32_t { return h.GetAnimTreeDoubleCount(); } },
        { "tokens", [](tool::gsc::GSCOBJHandler& h) -> uint32_t { return h.GetTokensCount(); } },
    };

    // Loop over the exports of a script, return false if the export table is invalid
    bool ForEachExport(const ScriptFile& script, const std::function<void(tool::gsc::GSCExportReader&)>& callback) {
        std::unique_ptr<tool::gsc::GSCExportReader> exp{ tool::gsc::CreateExportReader(script.vmInfo) };
        tool::gsc::GSCOBJHandler& handler{ *script.handler };
        size_t exportSize{ exp->SizeOf() };
        size_t exportCount{ handler.GetExportsCount() };
        if (handler.GetExportsOffset() + exportCount * exportSize > handler.GetFileSize()) {
            return false;
        }
        for (size_t i = 0; i < exportCount; i++) {
            exp->SetHandle(handler.Ptr(handler.GetExportsOffset() + i * exportSize));
            callback(*exp);
        }
        return true;
    }

    std::string get_gsc_script_data(const rapidjson::Value& args, bool& isError) {
        const char* path{ GetStringArg(args, "path") };
        if (!path) {
            isError = true;
            return "Missing 'path' argument";
        }

        std::vector<std::filesystem::path> files{};
        std::filesystem::path base{};
        std::string err{};
        if (!CollectScriptFiles(path, files, base, err)) {
            isError = true;
            return err;
        }

        hashutils::ReadDefaultFile();

        rapidjson::StringBuffer sb{};
        rapidjson::Writer<rapidjson::StringBuffer> writer{ sb };

        writer.StartArray();
        for (const std::filesystem::path& file : files) {
            std::string fileErr{};
            ScriptFile script{};
            OpenScript(file, script, fileErr);
            std::shared_ptr<tool::gsc::GSCOBJHandler>& handler{ script.handler };
            VmInfo* vmInfo{ script.vmInfo };
            uint64_t magic{ script.magic };

            writer.StartObject();
            writer.Key("path");
            writer.String(file.string().c_str());
            writer.Key("success");
            writer.Bool(fileErr.empty());
            if (!fileErr.empty()) {
                writer.Key("error");
                writer.String(fileErr.c_str());
            }
            if (handler) {
                assert(vmInfo != nullptr);
                writer.Key("vm");
                writer.String(vmInfo->internalName);
                writer.Key("magic");
                writer.String(std::format("0x{:x}", magic).c_str());
                writer.Key("fileSize");
                writer.Uint(handler->GetFileSize());
                writer.Key("name");
                writer.String(hashutils::ExtractTmp("script", handler->GetName()));
                writer.Key("checksum");
                writer.Uint(handler->GetChecksum());
                writer.Key("includes");
                writer.StartObject();
                writer.Key("count");
                writer.Uint(handler->GetIncludesCount());
                writer.Key("offset");
                writer.Uint(handler->GetIncludesOffset());
                writer.EndObject();
                writer.Key("exports");
                writer.StartObject();
                writer.Key("count");
                writer.Uint(handler->GetExportsCount());
                writer.Key("offset");
                writer.Uint(handler->GetExportsOffset());
                writer.EndObject();
                writer.Key("imports");
                writer.StartObject();
                writer.Key("count");
                writer.Uint(handler->GetImportsCount());
                writer.Key("offset");
                writer.Uint(handler->GetImportsOffset());
                writer.EndObject();
                writer.Key("strings");
                writer.StartObject();
                writer.Key("count");
                writer.Uint(handler->GetStringsCount());
                writer.Key("offset");
                writer.Uint(handler->GetStringsOffset());
                writer.EndObject();
                writer.Key("devStrings");
                writer.StartObject();
                writer.Key("count");
                writer.Uint(handler->GetDevStringsCount());
                writer.Key("offset");
                writer.Uint(handler->GetDevStringsOffset());
                writer.EndObject();
                writer.Key("globalVariables");
                writer.StartObject();
                writer.Key("count");
                writer.Uint(handler->GetGVarsCount());
                writer.Key("offset");
                writer.Uint(handler->GetGVarsOffset());
                writer.EndObject();
                writer.Key("animTreesSingle");
                writer.StartObject();
                writer.Key("count");
                writer.Uint(handler->GetAnimTreeSingleCount());
                writer.Key("offset");
                writer.Uint(handler->GetAnimTreeSingleOffset());
                writer.EndObject();
                writer.Key("animTreesDouble");
                writer.StartObject();
                writer.Key("count");
                writer.Uint(handler->GetAnimTreeDoubleCount());
                writer.Key("offset");
                writer.Uint(handler->GetAnimTreeDoubleOffset());
                writer.EndObject();
                writer.Key("tokens");
                writer.StartObject();
                writer.Key("count");
                writer.Uint(handler->GetTokensCount());
                writer.Key("offset");
                writer.Uint(handler->GetTokensOffset());
                writer.EndObject();
                writer.Key("cseg");
                writer.StartObject();
                writer.Key("offset");
                writer.Uint(handler->GetCSEGOffset());
                writer.Key("size");
                writer.Uint(handler->GetCSEGSize());
                writer.EndObject();

                writer.Key("exportList");
                writer.StartArray();
                ForEachExport(script, [&writer](tool::gsc::GSCExportReader& exp) {
                    writer.StartObject();
                    writer.Key("name");
                    writer.String(hashutils::ExtractTmp("function", exp.GetName()));
                    writer.Key("namespace");
                    writer.String(hashutils::ExtractTmp("namespace", exp.GetNamespace()));
                    writer.Key("fileNamespace");
                    writer.String(hashutils::ExtractTmp("namespace", exp.GetFileNamespace()));
                    writer.Key("checksum");
                    writer.String(std::format("0x{:x}", exp.GetChecksum()).c_str());
                    writer.Key("address");
                    writer.Uint(exp.GetAddress());
                    writer.Key("size");
                    writer.Uint(exp.GetSize());
                    writer.Key("paramCount");
                    writer.Uint(exp.GetParamCount());
                    writer.Key("flags");
                    writer.Uint(exp.GetFlags());
                    writer.EndObject();
                });
                writer.EndArray();
            }
            writer.EndObject();
        }
        writer.EndArray();
        return ToJson(sb);
    }

    // Compare two hashes, the top bits are ignored
    bool HashEquals(uint64_t a, uint64_t b) { return (a & hash::MASK60) == (b & hash::MASK60); }

    std::string search_gsc_scripts(const rapidjson::Value& args, bool& isError) {
        const char* path{ GetStringArg(args, "path") };
        if (!path) {
            isError = true;
            return "Missing 'path' argument";
        }
        const char* namespaceFilter{ GetStringArg(args, "namespace") };
        const char* functionFilter{ GetStringArg(args, "function") };
        const char* scriptFilter{ GetStringArg(args, "script") };

        // counts: { "<count name>": { "min": n, "max": n } }
        struct CountFilter {
            const ScriptCountDef* def;
            uint32_t min;
            uint32_t max;
        };
        std::vector<CountFilter> countFilters{};
        if (args.HasMember("counts")) {
            const rapidjson::Value& counts{ args["counts"] };
            if (!counts.IsObject()) {
                isError = true;
                return "'counts' must be an object";
            }
            for (rapidjson::Value::ConstMemberIterator it = counts.MemberBegin(); it != counts.MemberEnd(); ++it) {
                const ScriptCountDef* def{};
                for (const ScriptCountDef& countDef : scriptCounts) {
                    if (!_strcmpi(countDef.name, it->name.GetString())) {
                        def = &countDef;
                        break;
                    }
                }
                if (!def) {
                    isError = true;
                    return std::format("Unknown count '{}'", it->name.GetString());
                }
                CountFilter filter{ def, 0, UINT32_MAX };
                if (!it->value.IsObject()) {
                    isError = true;
                    return std::format("Count '{}' must be an object with min and/or max", def->name);
                }
                if (it->value.HasMember("min")) {
                    if (!it->value["min"].IsUint()) {
                        isError = true;
                        return std::format("Invalid min for '{}'", def->name);
                    }
                    filter.min = it->value["min"].GetUint();
                }
                if (it->value.HasMember("max")) {
                    if (!it->value["max"].IsUint()) {
                        isError = true;
                        return std::format("Invalid max for '{}'", def->name);
                    }
                    filter.max = it->value["max"].GetUint();
                }
                countFilters.push_back(filter);
            }
        }

        std::vector<std::filesystem::path> files{};
        std::filesystem::path base{};
        std::string err{};
        if (!CollectScriptFiles(path, files, base, err)) {
            isError = true;
            return err;
        }

        hashutils::ReadDefaultFile();

        size_t scanned{};
        size_t failed{};
        rapidjson::StringBuffer sb{};
        rapidjson::Writer<rapidjson::StringBuffer> writer{ sb };

        writer.StartObject();
        writer.Key("matches");
        writer.StartArray();
        for (const std::filesystem::path& file : files) {
            ScriptFile script{};
            std::string fileErr{};
            if (!OpenScript(file, script, fileErr)) {
                failed++;
                continue;
            }
            scanned++;
            tool::gsc::GSCOBJHandler& handler{ *script.handler };

            // the hashes depend on the vm, so the names are hashed for each script
            if (scriptFilter && !HashEquals(handler.GetName(), script.vmInfo->HashPath(scriptFilter))) {
                continue;
            }

            bool countsOk{ true };
            for (const CountFilter& filter : countFilters) {
                uint32_t count{ filter.def->getCount(handler) };
                if (count < filter.min || count > filter.max) {
                    countsOk = false;
                    break;
                }
            }
            if (!countsOk) {
                continue;
            }

            // exports matching both the namespace and the function filters
            struct ExportMatch {
                uint64_t name;
                uint64_t nameSpace;
                uint32_t address;
            };
            std::vector<ExportMatch> exportMatches{};
            bool exportFilter{ namespaceFilter || functionFilter };
            if (exportFilter) {
                uint64_t namespaceHash{ namespaceFilter ? script.vmInfo->HashField(namespaceFilter) : 0 };
                uint64_t functionHash{ functionFilter ? script.vmInfo->HashField(functionFilter) : 0 };
                ForEachExport(script, [&](tool::gsc::GSCExportReader& exp) {
                    if (functionFilter && !HashEquals(exp.GetName(), functionHash)) {
                        return;
                    }
                    if (namespaceFilter && !HashEquals(exp.GetNamespace(), namespaceHash) &&
                        !HashEquals(exp.GetFileNamespace(), namespaceHash)) {
                        return;
                    }
                    exportMatches.push_back({ exp.GetName(), exp.GetNamespace(), exp.GetAddress() });
                });
                if (exportMatches.empty()) {
                    continue;
                }
            }

            writer.StartObject();
            writer.Key("path");
            writer.String(file.string().c_str());
            writer.Key("vm");
            writer.String(script.vmInfo->internalName);
            writer.Key("name");
            writer.String(hashutils::ExtractTmp("script", handler.GetName()));
            writer.Key("counts");
            writer.StartObject();
            for (const ScriptCountDef& countDef : scriptCounts) {
                writer.Key(countDef.name);
                writer.Uint(countDef.getCount(handler));
            }
            writer.EndObject();
            if (exportFilter) {
                writer.Key("exports");
                writer.StartArray();
                for (const ExportMatch& match : exportMatches) {
                    writer.StartObject();
                    writer.Key("name");
                    writer.String(hashutils::ExtractTmp("function", match.name));
                    writer.Key("namespace");
                    writer.String(hashutils::ExtractTmp("namespace", match.nameSpace));
                    writer.Key("address");
                    writer.Uint(match.address);
                    writer.EndObject();
                }
                writer.EndArray();
            }
            writer.EndObject();
        }
        writer.EndArray();
        writer.Key("scanned");
        writer.Uint64(scanned);
        writer.Key("failed");
        writer.Uint64(failed);
        writer.EndObject();
        return ToJson(sb);
    }

    void WriteOpCodeName(rapidjson::Writer<rapidjson::StringBuffer>& writer, OPCode op) {
        const char* name{ tool::gsc::opcode::OpCodeName(op) };
        if (name) {
            writer.String(name);
        } else {
            writer.Null();
        }
    }

    std::string get_gsc_vm_info(const rapidjson::Value& args, bool& isError) {
        const char* vmName{ GetStringArg(args, "vm") };
        if (!vmName) {
            isError = true;
            return "Missing 'vm' argument";
        }
        const VmInfo* info{ FindVmByInternalName(vmName) };
        if (!info) {
            isError = true;
            return std::format("Unknown vm '{}', use list_gsc_vm to get the internalName", vmName);
        }

        rapidjson::StringBuffer sb{};
        rapidjson::Writer<rapidjson::StringBuffer> writer{ sb };

        writer.StartObject();
        writer.Key("internalName");
        writer.String(info->internalName);
        writer.Key("name");
        writer.String(info->name);
        writer.Key("codeName");
        writer.String(info->codeName);
        writer.Key("magic");
        writer.String(std::format("0x{:x}", info->vmMagic).c_str());
        writer.Key("gdbMagic");
        writer.String(std::format("0x{:x}", info->gdbMagic).c_str());
        writer.Key("compilerHookFunctionName");
        writer.String(std::format("0x{:x}", info->compilerHookFunctionName).c_str());
        writer.Key("maxOpCode");
        writer.Uint(info->maxOpCode);
        writer.Key("modToolFlag");
        writer.Uint(info->modToolFlag);
        writer.Key("opaqueStringCount");
        writer.Uint64(info->opaqueStringCount);
        writer.Key("devCallsCount");
        writer.Uint64(info->devCallsNames.size());

        writer.Key("flags");
        writer.StartArray();
        for (const std::pair<tool::gsc::opcode::VmFlags, std::string_view>& flag :
             magic_enum::enum_entries<tool::gsc::opcode::VmFlags>()) {
            if (flag.first != tool::gsc::opcode::VMF_NONE && info->HasFlag(flag.first)) {
                std::string_view flagName{ flag.second };
                flagName.remove_prefix(flagName.starts_with("VMF_") ? 4 : 0);
                writer.String(flagName.data(), (rapidjson::SizeType)flagName.size());
            }
        }
        writer.EndArray();

        writer.Key("platforms");
        writer.StartArray();
        for (size_t i = 1; i < tool::gsc::opcode::PLATFORM_COUNT; i++) {
            Platform plt{ (Platform)i };
            if (!info->HasPlatform(plt)) {
                continue;
            }
            writer.StartObject();
            writer.Key("id");
            writer.String(tool::gsc::opcode::PlatformIdName(plt));
            writer.Key("name");
            writer.String(tool::gsc::opcode::PlatformName(plt));
            writer.Key("bigEndian");
            writer.Bool(info->IsPlatformBigEndian(plt));
            Platform same{ info->RemapSamePlatform(plt) };
            if (same != plt) {
                writer.Key("sameOpCodesAs");
                writer.String(tool::gsc::opcode::PlatformIdName(same));
            }
            std::unordered_map<Platform, std::unordered_map<OPCode, std::unordered_set<uint16_t>>>::const_iterator
                pltOps{ info->opcodemappltlookup.find(plt) };
            writer.Key("opcodeCount");
            writer.Uint64(pltOps == info->opcodemappltlookup.end() ? 0 : pltOps->second.size());
            writer.EndObject();
        }
        writer.EndArray();

        writer.Key("hashes");
        writer.StartArray();
        for (const std::pair<const char, tool::gsc::opcode::VmHashFunc>& hash : info->hashesFunc) {
            writer.StartObject();
            writer.Key("type");
            writer.String(std::string(1, hash.first).c_str());
            writer.Key("opcode");
            WriteOpCodeName(writer, hash.second.opCode);
            writer.Key("size");
            writer.Int(hash.second.size);
            writer.EndObject();
        }
        writer.EndArray();

        std::map<std::string, const tool::gsc::opcode::GlobalVariableDef*> globals{};
        for (const std::pair<const uint64_t, tool::gsc::opcode::GlobalVariableDef>& gvar : info->globalvars) {
            globals[gvar.second.name] = &gvar.second;
        }
        writer.Key("globalVariables");
        writer.StartArray();
        for (const std::pair<const std::string, const tool::gsc::opcode::GlobalVariableDef*>& gvar : globals) {
            writer.StartObject();
            writer.Key("name");
            writer.String(gvar.first.c_str());
            writer.Key("getOpCode");
            WriteOpCodeName(writer, gvar.second->getOpCode);
            writer.Key("refOpCode");
            WriteOpCodeName(writer, gvar.second->refOpCode);
            writer.EndObject();
        }
        writer.EndArray();

        std::map<std::string, const tool::gsc::opcode::FunctionOperator*> funcs{};
        for (const std::pair<const uint64_t, tool::gsc::opcode::FunctionOperator>& func : info->opFuncs) {
            funcs[func.second.name] = &func.second;
        }
        writer.Key("operatorFunctions");
        writer.StartArray();
        for (const std::pair<const std::string, const tool::gsc::opcode::FunctionOperator*>& func : funcs) {
            writer.StartObject();
            writer.Key("name");
            writer.String(func.first.c_str());
            writer.Key("usage");
            writer.String(func.second->usage ? func.second->usage : "");
            writer.Key("opcode");
            WriteOpCodeName(writer, func.second->opCode);
            writer.Key("flags");
            writer.Int(func.second->flags);
            writer.Key("minParam");
            writer.Int(func.second->minParam);
            writer.Key("maxParam");
            writer.Int(func.second->maxParam);
            writer.EndObject();
        }
        writer.EndArray();

        std::set<std::string> dataTypes{};
        for (const std::pair<const uint64_t, const char*>& dt : info->dataType) {
            dataTypes.insert(dt.second);
        }
        writer.Key("dataTypes");
        writer.StartArray();
        for (const std::string& dt : dataTypes) {
            writer.String(dt.c_str());
        }
        writer.EndArray();

        writer.EndObject();
        return ToJson(sb);
    }

    ADD_MCP_TOOL(
        get_gsc_vm_info,
        "Get all the information about a vm (names, magic, flags, platforms, hash opcodes, global variables, "
        "operator functions, datatypes). Use get_gsc_vm_opcodes for the opcode values.",
        R"({"type":"object","properties":{"vm":{"type":"string","description":"vm internalName"}},"required":["vm"]})",
        get_gsc_vm_info
    );
    ADD_MCP_TOOL(
        list_gsc_vm, "List the GSC VM and their platforms. Returns an array of { internalName, name, platforms[] }.",
        nullptr, list_gsc_vm
    );

    ADD_MCP_TOOL(
        list_gsc_opcode_names,
        "List the opcode names usable in the other opcode tools. Optional 'filter' is a case insensitive substring.",
        R"({"type":"object","properties":{"filter":{"type":"string"}}})", list_gsc_opcode_names
    );
    ADD_MCP_TOOL(
        get_gsc_vm_opcodes, "Get the opcodes defined by a vm for a platform. Returns an array of { opcode, values[] }.",
        R"({"type":"object","properties":{"vm":{"type":"string","description":"vm internalName"},"platform":{"type":"string"}},"required":["vm","platform"]})",
        get_gsc_vm_opcodes
    );
    ADD_MCP_TOOL(
        dump_gsc_vm_opcodes,
        "Dump the opcodes of a vm for a platform in the actsop csv format (VM,PLATFORM,OPCODE,VALUES, values in hex, "
        "the values not used are in the Undefined line). The csv is returned, or written into 'output' if it is "
        "set.",
        R"({"type":"object","properties":{"vm":{"type":"string","description":"vm internalName"},"platform":{"type":"string"},"output":{"type":"string","description":"optional file to write the dump into"}},"required":["vm","platform"]})",
        dump_gsc_vm_opcodes
    );
    ADD_MCP_TOOL(
        set_gsc_vm_opcodes,
        "Add or replace opcode values in a vm for a platform. 'opcodes' is an array of { opcode (name from "
        "list_gsc_opcode_names), values[] (integers) }. A value already used is replaced.",
        R"({"type":"object","properties":{"vm":{"type":"string","description":"vm internalName"},"platform":{"type":"string"},"opcodes":{"type":"array","items":{"type":"object","properties":{"opcode":{"type":"string"},"values":{"type":"array","items":{"type":"integer","minimum":0,"maximum":65535}}},"required":["opcode","values"]}}},"required":["vm","platform","opcodes"]})",
        set_gsc_vm_opcodes
    );
    ADD_MCP_TOOL(
        remove_gsc_vm_opcodes, "Remove opcode values from a vm for a platform, 'values' is an array of integers.",
        R"({"type":"object","properties":{"vm":{"type":"string","description":"vm internalName"},"platform":{"type":"string"},"values":{"type":"array","items":{"type":"integer","minimum":0,"maximum":65535}}},"required":["vm","platform","values"]})",
        remove_gsc_vm_opcodes
    );
    ADD_MCP_TOOL(
        decompile_gsc_file,
        "Decompile or disassemble the compiled GSC/CSC files of a path (a file or a directory, searched "
        "recursively) with the current vm opcodes. The vm is read from the magic of each file. The platform is pc by "
        "default, mode is decompile (default), asm, both or status (only the files with errors, without the output). "
        "'exports' restricts the output to some functions, each "
        "entry is { function, namespace?, script? } with exact names (script requires namespace), they are hashed "
        "for the vm of each file. 'maxErrors' stops the loop over the files once this number of errors is reached. "
        "Returns "
        "{ files: [ { path, success, error?, errors[] ({ message, function, namespace, script } of the decompiler "
        "errors, missing opcodes...), output } ], processed, "
        "totalFiles, totalErrors, stoppedEarly }.",
        R"({"type":"object","properties":{"path":{"type":"string","description":"path of a compiled gsc/csc file or of a directory"},"exports":{"type":"array","items":{"type":"object","properties":{"function":{"type":"string"},"namespace":{"type":"string"},"script":{"type":"string"}},"required":["function"]}},"platform":{"type":"string"},"mode":{"type":"string","enum":["decompile","asm","both","status"]},"maxErrors":{"type":"integer","minimum":0},"header":{"type":"boolean"},"hideOpcodeValues":{"type":"boolean"}},"required":["path"]})",
        decompile_gsc_file
    );
    ADD_MCP_TOOL(
        search_gsc_scripts,
        "Search the compiled GSC/CSC files of a path (a file or a directory, searched recursively). Filters, all "
        "optional and combined with AND: 'script' (script name/path), 'function' and 'namespace' (an export of the "
        "script, the namespace can also be the file namespace) and 'counts' ({ includes|exports|imports|strings|"
        "devStrings|globalVariables|animTreesSingle|animTreesDouble|tokens: { min?, max? } }). The names must be "
        "exact, they are hashed with the hash of the vm of each script. Returns { matches: [ { path, vm, name, counts, "
        "exports? (the matching functions) } ], scanned, failed }.",
        R"({"type":"object","properties":{"path":{"type":"string","description":"path of a compiled gsc/csc file or of a directory"},"script":{"type":"string","description":"exact script name"},"function":{"type":"string","description":"exact function name"},"namespace":{"type":"string","description":"exact namespace name"},"counts":{"type":"object","additionalProperties":{"type":"object","properties":{"min":{"type":"integer","minimum":0},"max":{"type":"integer","minimum":0}}}}},"required":["path"]})",
        search_gsc_scripts
    );
    ADD_MCP_TOOL(
        get_gsc_script_data,
        "Read the header data of the compiled GSC/CSC files of a path (a file or a directory, searched "
        "recursively). The vm is read from the magic of each file. Returns an array of { path, success, error?, vm, "
        "magic, fileSize, name, checksum, and the count/offset of includes, exports, imports, strings, "
        "devStrings, globalVariables, animTreesSingle, animTreesDouble, tokens, plus cseg and exportList (the "
        "functions: "
        "name, namespace, fileNamespace (strings, or type_hash when unknown), checksum, address, size, "
        "paramCount, flags) }.",
        R"({"type":"object","properties":{"path":{"type":"string","description":"path of a compiled gsc/csc file or of a directory"}},"required":["path"]})",
        get_gsc_script_data
    );
} // namespace
