#include <includes.hpp>

using namespace tool::gsc;

bool GscInfoOption::Compute(LPCCH* args, INT startIndex, INT endIndex) {
    // default values
    for (size_t i = startIndex; i < endIndex; i++) {
        LPCCH arg = args[i];

        if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
            m_help = true;
        }
        else if (!strcmp("-g", arg) || !_strcmpi("--gsc", arg)) {
            m_dcomp = true;
        }
        else if (!strcmp("-a", arg) || !_strcmpi("--asm", arg)) {
            m_dasm = true;
        }
        else if (!strcmp("-s", arg) || !_strcmpi("--silent", arg)) {
            m_verbose = false;
        }
        else if (!strcmp("-H", arg) || !_strcmpi("--header", arg)) {
            m_header = true;
        }
        else if (!strcmp("-I", arg) || !_strcmpi("--imports", arg)) {
            m_imports = true;
        }
        else if (!strcmp("-S", arg) || !_strcmpi("--strings", arg)) {
            m_strings = true;
        }
        else if (!strcmp("-G", arg) || !_strcmpi("--gvars", arg)) {
            m_gvars = true;
        }
        else if (!strcmp("-U", arg) || !_strcmpi("--noincludes", arg)) {
            m_includes = false;
        }
        else if (!strcmp("-X", arg) || !_strcmpi("--exptests", arg)) {
            m_exptests = true;
        }
        else if (!strcmp("-f", arg) || !_strcmpi("--nofunc", arg)) {
            m_func = false;
        }
        else if (!strcmp("-F", arg) || !_strcmpi("--nofuncheader", arg)) {
            m_func_header = false;
        }
        else if (!strcmp("-p", arg) || !_strcmpi("--postfunchead", arg)) {
            m_func_header_post = true;
        }
        else if (!strcmp("-l", arg) || !_strcmpi("--rloc", arg)) {
            m_func_rloc = true;
        }
        else if (!strcmp("-P", arg) || !_strcmpi("--nopatch", arg)) {
            m_patch = false;
        }
        else if (!_strcmpi("--internalblocks", arg)) {
            m_show_internal_blocks = true;
        }
        else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
            if (i + 1 == endIndex) {
                std::cerr << "Missing value for param: " << arg << "!\n";
                return false;
            }
            m_outputDir = args[++i];
        }
        else if (!strcmp("-C", arg) || !_strcmpi("--copyright", arg)) {
            if (i + 1 == endIndex) {
                std::cerr << "Missing value for param: " << arg << "!\n";
                return false;
            }
            m_copyright = args[++i];
        }
        else if (*arg == '-') {
            std::cerr << "Unknown option: " << arg << "!\n";
            return false;
        }
        else {
            m_inputFiles.push_back(arg);
        }
    }
    if (!m_inputFiles.size()) {
        m_inputFiles.push_back("scriptparsetree");
    }
    return true;
}

void GscInfoOption::PrintHelp(std::ostream& out) {
    out << "-h --help          : Print help\n"
        << "-g --gsc           : Produce GSC\n"
        << "-a --asm           : Produce ASM\n"
        << "-o --output [d]    : ASM/GSC output dir, default same.gscasm\n"
        << "-s --silent        : Silent output, only errors\n"
        << "-H --header        : Write file header\n"
        << "-f --nofunc        : No function write\n"
        << "-l --rloc          : Write relative location of the function code\n"
        << "-F --nofuncheader  : No function header\n"
        << "-p --postfunchead  : Write post function header in ASM mode\n"
        << "-I --imports       : Write imports\n"
        << "-S --strings       : Write strings\n"
        << "-G --gvars         : Write gvars\n"
        << "-U --noincludes    : No includes\n"
        << "-X --exptests      : Enable UNK tests\n"
        << "-C --copyright [t] : Set a comment text to put in front of every file\n";
}

int GscInfoHandleData(tool::gsc::T8GSCOBJ* data, size_t size, const char* path, const GscInfoOption& opt) {
    hashutils::ReadDefaultFile();

    opcode::vminfo* vmInfo;
    if (!opcode::IsValidVm(data->GetVm(), vmInfo)) {
        std::cerr << "Bad vm 0x" << std::hex << (int)data->GetVm() << " for file " << path << "\n";
        return -1;
    }

    T8GSCOBJContext ctx{};

    if (opt.m_patch) {
        // unlink the script and write custom gvar/string ids
        data->PatchCode(ctx);
    }

    char asmfnamebuff[1000];

    if (opt.m_outputDir) {
        LPCCH name = hashutils::ExtractPtr(data->name);
        if (!name) {
            snprintf(asmfnamebuff, 1000, "%s/hashed-%d/script_%llx.gsc", opt.m_outputDir, (int)(data->name % 3) + 1, data->name);
        }
        else {
            snprintf(asmfnamebuff, 1000, "%s/%s", opt.m_outputDir, name);
        }
    }
    else {
        snprintf(asmfnamebuff, 1000, "%sasm", path);
    }

    std::filesystem::path file(asmfnamebuff);

    std::filesystem::create_directories(file.parent_path());

    std::ofstream asmout{ file };

    if (!asmout) {
        std::cerr << "Can't open output file " << asmfnamebuff << "\n";
        return -1;
    }
    if (opt.m_copyright) {
        asmout << "// " << opt.m_copyright << "\n";
    }
    if (opt.m_header) {
        asmout
            << "// " << hashutils::ExtractTmp("script", data->name) << " (" << path << ")" << " (size: " << size << " Bytes / " << std::hex << "0x" << size << ")\n"
            << "// magic .... 0x" << *reinterpret_cast<UINT64*>(&data->magic[0]) 
                << " vm: 0x" << (UINT32)vmInfo->vm << " (" << vmInfo->name << ")"
                << " crc: 0x" << std::hex << data->crc << "\n"
            << std::left << std::setfill(' ')
            << "// size ..... " << std::dec << std::setw(3) << data->script_size << "\n"
            << "// includes . " << std::dec << std::setw(3) << data->include_count << " (offset: 0x" << std::hex << data->include_offset << ")\n"
            << "// strings .. " << std::dec << std::setw(3) << data->string_count << " (offset: 0x" << std::hex << data->string_offset << ")\n"
            << "// exports .. " << std::dec << std::setw(3) << data->exports_count << " (offset: 0x" << std::hex << data->export_table_offset << ")\n"
            << "// imports .. " << std::dec << std::setw(3) << data->imports_count << " (offset: 0x" << std::hex << data->imports_offset << ")\n"
            << "// globals .. " << std::dec << std::setw(3) << data->globalvar_count << " (offset: 0x" << std::hex << data->globalvar_offset << ")\n"
            << "// fixups ... " << std::dec << std::setw(3) << data->fixup_count << " (offset: 0x" << std::hex << data->fixup_offset << ")\n"
            << std::right
            << std::flush;
    }

    if (opt.m_includes) {
        UINT64 *includes = reinterpret_cast<UINT64*>(&data->magic[data->include_offset]);

        for (size_t i = 0; i < data->include_count; i++) {
            asmout << "#include " << hashutils::ExtractTmp("script", includes[i]) << ";\n";
        }
        if (data->include_count) {
            asmout << "\n";
        }
    }

    if (opt.m_exptests) {
        auto* fixups = reinterpret_cast<T8GSCFixup*>(&data->magic[data->fixup_offset]);

        for (size_t i = 0; i < data->fixup_count; i++) {
            const auto& fixup = fixups[i];
            asmout << std::hex << "#fixup 0x" << fixup.offset << " = 0x" << fixup.address << ";\n";
        }

        if (data->fixup_count) {
            asmout << "\n";
        }
    }

    if (opt.m_gvars) {
        uintptr_t gvars_location = reinterpret_cast<uintptr_t>(data->magic) + data->globalvar_offset;

        for (size_t i = 0; i < data->globalvar_count; i++) {
            const auto* globalvar = reinterpret_cast<T8GSCGlobalVar*>(gvars_location);
            asmout << std::hex << "Global var " << hashutils::ExtractTmp("var", globalvar->name) << " " << globalvar->num_address << "\n";

            asmout << "location(s): ";

            const auto* vars = reinterpret_cast<const UINT32*>(&globalvar[1]);
            asmout << std::hex << vars[0];
            for (size_t j = 1; j < globalvar->num_address; j++) {
                asmout << std::hex << "," << vars[j];
            }
            asmout << "\n";
            gvars_location += sizeof(*globalvar) + sizeof(*vars) * globalvar->num_address;
        }
        if (data->globalvar_count) {
            asmout << "\n";
        }
    }

    if (opt.m_strings) {
        uintptr_t str_location = reinterpret_cast<uintptr_t>(data->magic) + data->string_offset;

        for (size_t i = 0; i < data->string_count; i++) {

            const auto* str = reinterpret_cast<T8GSCString*>(str_location);

            asmout << std::hex << "String addr:" << str->string << ", count:" << (int)str->num_address << ", type:" << (int)str->type << "\n";

            LPCH cstr = tool::decrypt::decryptfunc(reinterpret_cast<LPCH>(&data->magic[str->string]));

            asmout << '"' << cstr << "\"\n";

            asmout << "location(s): ";

            const auto* strings = reinterpret_cast<const UINT32*>(&str[1]);
            asmout << std::hex << strings[0];
            for (size_t j = 1; j < str->num_address; j++) {
                asmout << std::hex << "," << strings[j];
            }
            asmout << "\n";
            str_location += sizeof(*str) + sizeof(*strings) * str->num_address;
        }
        if (data->string_count) {
            asmout << "\n";
        }
    }

    if (opt.m_imports) {
        uintptr_t import_location = reinterpret_cast<uintptr_t>(data->magic) + data->imports_offset;

        for (size_t i = 0; i < data->imports_count; i++) {

            const auto* imp = reinterpret_cast<T8GSCImport*>(import_location);
            asmout << std::hex << "import ";

            switch (imp->flags & T8GSCImportFlags::CALLTYPE_MASK) {
            case FUNC_METHOD: asmout << "funcmethod "; break;
            case FUNCTION: asmout << "function "; break;
            case FUNCTION_THREAD: asmout << "function thread "; break;
            case FUNCTION_THREAD_ENDON: asmout << "function threadendon "; break;
            case METHOD: asmout << "method "; break;
            case METHOD_THREAD: asmout << "method thread "; break;
            case METHOD_THREAD_ENDON: asmout << "method threadendon "; break;
            default:
                asmout << "<errorflag:" << std::hex << (imp->flags & 0xF) << "> ";
                break;
            }

            if (imp->flags & T8GSCImportFlags::DEV_CALL) {
                asmout << "devcall ";
            }

            // they both seem unused
            if (imp->flags & T8GSCImportFlags::UKN40) {
                asmout << "ukn40 ";
            }
            if (imp->flags & T8GSCImportFlags::UKN80) {
                asmout << "ukn80 ";
            }

            if ((imp->flags & T8GSCImportFlags::GET_CALL) == 0) {
                // no need for namespace if we are getting the call dynamically (api or inside-code script)
                asmout << hashutils::ExtractTmp("namespace", imp->import_namespace) << std::flush << "::";
            }

            asmout << std::hex << hashutils::ExtractTmp("function", imp->name) << "\n";

            asmout << std::hex << "address: " << imp->num_address <<
                ", params: " << (int)imp->param_count <<
                ", iflags: 0x" << std::hex << (UINT16)(imp->flags) << "\n";

            asmout << "location(s): ";

            const auto* imports = reinterpret_cast<const UINT32*>(&imp[1]);
            asmout << std::hex << imports[0];
            for (size_t j = 1; j < imp->num_address; j++) {
                asmout << std::hex << "," << imports[j];
            }
            asmout << "\n";

            asmout << "--------------\n";

            import_location += sizeof(*imp) + sizeof(*imports) * imp->num_address;
        }
        if (data->imports_count) {
            asmout << "\n";
        }
    }

    if (opt.m_func) {
        // current namespace
        UINT32 currentNSP = 0;

        auto* exports = reinterpret_cast<T8GSCExport*>(&data->magic[data->export_table_offset]);

        for (size_t i = 0; i < data->exports_count; i++) {
            const auto& exp = exports[i];
            if (exp.name_space != currentNSP) {
                currentNSP = exp.name_space;
                asmout << "#namespace " << hashutils::ExtractTmp("namespace", currentNSP) << ";\n" << std::endl;
            }

            auto asmctx = opcode::asmcontext(&data->magic[exp.address], opt, currentNSP, exp, data->GetVm());

            std::ofstream nullstream;
            nullstream.setstate(std::ios_base::badbit);

            std::ostream& output = opt.m_dasm ? asmout : nullstream;

            exp.DumpFunctionHeader(output, data->magic, ctx, asmctx);

            output << " gscasm {\n";

            exp.DumpAsm(output, data->magic, ctx, asmctx);

            output << "}\n";

            std::ostream& outputdecomp = opt.m_dcomp ? asmout : nullstream;

            if (!opt.m_dasm || opt.m_dcomp || opt.m_func_header_post) {
                asmctx.ComputeDefaultParamValue();
                exp.DumpFunctionHeader(outputdecomp, data->magic, ctx, asmctx);
                outputdecomp << std::flush;
                opcode::decompcontext dctx{0, 0, asmctx};
                if (opt.m_dcomp) {
                    if (exp.flags == T8GSCExportFlags::CLASS_VTABLE) {
                        asmctx.m_bcl = &data->magic[exp.address];
                        outputdecomp << " {\n";
                        exp.DumpVTable(outputdecomp, data->magic, ctx, asmctx, dctx);
                        outputdecomp << "}\n";
                    }
                    else {
                        outputdecomp << " "; // padding between block/parameters
                        asmctx.ComputeDevBlocks();
                        asmctx.ComputeSwitchBlocks();
                        asmctx.Dump(outputdecomp, dctx);
                    }
                }
                else {
                    outputdecomp << ";\n";
                }
            }
            asmout << "\n";
        }
    }

    if (opt.m_exptests) {
        auto* ukn4c = reinterpret_cast<UINT64*>(&data->magic[data->ukn4c_offset]);

        for (size_t i = 0; i < data->ukn4c_count; i++) {
            asmout << "ukn4c: " << hashutils::ExtractTmp("hash", ukn4c[i]) << "\n";
        }
    }

    asmout.close();

    return 0;
}

int GscInfoFile(const std::filesystem::path& path, const GscInfoOption& opt) {
    if (std::filesystem::is_directory(path)) {
        // directory
        auto ret = 0;
        for (const auto& sub : std::filesystem::directory_iterator{path}) {
            auto lret = GscInfoFile(sub.path(), opt);
            if (!ret) {
                ret = lret;
            }
        }
        return ret;
    }
    auto pathname = path.generic_string();

    if (!(
        pathname.ends_with(".gscc") || pathname.ends_with(".cscc")
            || pathname.ends_with(".gscbin") || pathname.ends_with(".cscbin")
        )) {
        return 0;
    }
    if (opt.m_verbose) {
        std::cout << "Reading " << pathname << "\n";
    }

    LPVOID buffer = NULL;
    size_t size;
    LPVOID bufferNoAlign = NULL;
    size_t sizeNoAlign;
    if (!utils::ReadFileAlign(path, bufferNoAlign, buffer, sizeNoAlign, size)) {
        std::cerr << "Can't read file data for " << path << "\n";
        return -1;
    }

    if (size < sizeof(tool::gsc::T8GSCOBJ)) {
        std::cerr << "Bad header, file size: " << std::hex << size << "/" << sizeof(tool::gsc::T8GSCOBJ) << " for " << path << "\n";
        std::free(bufferNoAlign);
        return -1;
    }

    auto ret = GscInfoHandleData(reinterpret_cast<tool::gsc::T8GSCOBJ*>(buffer), size, pathname.c_str(), opt);
    std::free(bufferNoAlign);
    return ret;
}

int DumpInfoFileData(tool::gsc::T8GSCOBJ* data, size_t size, const char* path, std::unordered_map<UINT64, LPCCH>& dataset) {
    // name
    dataset.insert({ data->name, "script" });


    UINT64* includes = reinterpret_cast<UINT64*>(&data->magic[data->include_offset]);
    for (size_t i = 0; i < data->include_count; i++) {
        dataset.insert({ includes[i], "script" });
    }

    uintptr_t gvars_location = reinterpret_cast<uintptr_t>(data->magic) + data->globalvar_offset;
    for (size_t i = 0; i < data->globalvar_count; i++) {
        const auto* globalvar = reinterpret_cast<tool::gsc::T8GSCGlobalVar*>(gvars_location);
        dataset.insert({ globalvar->name, "var" });

        gvars_location += sizeof(*globalvar) + sizeof(UINT32) * globalvar->num_address;
    }


    auto* exports = reinterpret_cast<tool::gsc::T8GSCExport*>(&data->magic[data->export_table_offset]);
    for (size_t i = 0; i < data->exports_count; i++) {
        const auto& exp = exports[i];
        dataset.insert({ exp.name_space, "namespace" });
        dataset.insert({ exp.name, "function" });

        if (exp.flags & tool::gsc::T8GSCExportFlags::EVENT) {
            dataset.insert({ exp.callback_event, "event" });
        }
    }

    return 0;
}

int DumpInfoFile(const std::filesystem::path& path, std::unordered_map<UINT64, LPCCH>& dataset) {
    if (std::filesystem::is_directory(path)) {
        // directory
        auto ret = 0;
        for (const auto& sub : std::filesystem::directory_iterator{ path }) {
            auto lret = DumpInfoFile(sub.path(), dataset);
            if (!ret) {
                ret = lret;
            }
        }
        return ret;
    }
    auto pathname = path.generic_string();

    if (!(
        pathname.ends_with(".gscc") || pathname.ends_with(".cscc")
        || pathname.ends_with(".gscbin") || pathname.ends_with(".cscbin")
        )) {
        return 0;
    }

    std::cout << "Reading " << pathname << "\n";

    LPVOID buffer = NULL;
    size_t size;
    LPVOID bufferNoAlign = NULL;
    size_t sizeNoAlign;
    if (!utils::ReadFileAlign(path, bufferNoAlign, buffer, sizeNoAlign, size)) {
        std::cerr << "Can't read file data for " << path << "\n";
        return -1;
    }

    if (size < sizeof(tool::gsc::T8GSCOBJ)) {
        std::cerr << "Bad header, file size: " << std::hex << size << "/" << sizeof(tool::gsc::T8GSCOBJ) << " for " << path << "\n";
        std::free(bufferNoAlign);
        return -1;
    }

    auto ret = DumpInfoFileData(reinterpret_cast<tool::gsc::T8GSCOBJ*>(buffer), size, pathname.c_str(), dataset);
    std::free(bufferNoAlign);
    return ret;
}

int tool::gsc::dumpdataset(const process& proc, int argc, const char* argv[]) {
    hashutils::ReadDefaultFile();
    // scriptparsetree] [output=dataset.txt]
    LPCCH inputFile = "scriptparsetree";
    LPCCH outputFile = "dataset.csv";
    if (argc > 2) {
        inputFile = argv[2];
        if (argc > 3) {
            inputFile = argv[3];
        }
    }

    std::unordered_map<UINT64, LPCCH> dataset{};

    int ret = DumpInfoFile(inputFile, dataset);
    if (ret) {
        return ret;
    }

    std::ofstream out{ outputFile };
    if (!out) {
        std::cerr << "Can't open output file\n";
    }

    out << "type,name\n";

    for (const auto& [hash, type] : dataset) {
        out << std::hex << type << "," << hashutils::ExtractTmp(type, hash) << "\n";
    }
    out.close();
    return 0;
}

tool::gsc::T8GSCOBJContext::T8GSCOBJContext() {}

// apply ~ to ref to avoid using 0, 1, 2 which might already be used

UINT32 tool::gsc::T8GSCOBJContext::GetGlobalVarName(UINT16 gvarRef) {
    auto f = m_gvars.find(gvarRef);
    if (f == m_gvars.end()) {
        return 0;
    }
    return f->second;
}

LPCCH tool::gsc::T8GSCOBJContext::GetStringValue(UINT32 stringRef) {
    auto f = m_stringRefs.find(stringRef);
    if (f == m_stringRefs.end()) {
        return NULL;
    }
    return f->second;
}

UINT16 tool::gsc::T8GSCOBJContext::AddGlobalVarName(UINT32 value) {
    UINT16 id = ((UINT16)m_gvars.size());
    m_gvars[id] = value;
    return id;
}

UINT32 tool::gsc::T8GSCOBJContext::AddStringValue(LPCCH value) {
    UINT32 id = ((UINT32)m_stringRefs.size());
    m_stringRefs[id] = value;
    return id;
}


void tool::gsc::T8GSCOBJ::PatchCode(T8GSCOBJContext& ctx) {
    // patching imports unlink the script refs to write namespace::import_name instead of the address
    uintptr_t import_location = reinterpret_cast<uintptr_t>(&magic[0]) + imports_offset;
    for (size_t i = 0; i < imports_count; i++) {

        const auto* imp = reinterpret_cast<T8GSCImport*>(import_location);

        const auto* imports = reinterpret_cast<const UINT32*>(&imp[1]);
        for (size_t j = 0; j < imp->num_address; j++) {
            UINT32* loc;
            switch (imp->flags & 0xF) {
            case 1:
                loc = (UINT32*)(magic + ((imports[j] + 2 + 7) & ~7));
                break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                // here the game fix function calls with a bad number of params,
                // but for the decomp/dasm we don't care because we only mind about
                // what we'll find on the stack.
                *(BYTE*)(magic + imports[j] + 2) = imp->param_count;
                loc = (UINT32*)(magic + ((imports[j] + 2 + 1 + 7) & ~7));
                break;
            default:
                loc = nullptr;
                break;
            }
            if (loc) {
                loc[0] = imp->name;

                if (imp->flags & T8GSCImportFlags::GET_CALL) {
                    // no need for namespace if we are getting the call dynamically (api or inside-code script)
                    loc[1] = 0xc1243180; // ""
                }
                else {
                    loc[1] = imp->import_namespace;
                }

            }
        }
        import_location += sizeof(*imp) + sizeof(*imports) * imp->num_address;
    }

    uintptr_t gvars_location = reinterpret_cast<uintptr_t>(magic) + globalvar_offset;
    for (size_t i = 0; i < globalvar_count; i++) {
        const auto* globalvar = reinterpret_cast<T8GSCGlobalVar*>(gvars_location);
        UINT16 ref = ctx.AddGlobalVarName(globalvar->name);

        const auto* vars = reinterpret_cast<const UINT32*>(&globalvar[1]);
        for (size_t j = 0; j < globalvar->num_address; j++) {
            // no align, no opcode to pass, directly the fucking location, cool.
            *(UINT16*)(magic + vars[j]) = ref;
        }
        gvars_location += sizeof(*globalvar) + sizeof(*vars) * globalvar->num_address;
    }

    uintptr_t str_location = reinterpret_cast<uintptr_t>(magic) + string_offset;
    for (size_t i = 0; i < string_count; i++) {

        const auto* str = reinterpret_cast<T8GSCString*>(str_location);
        LPCH cstr = tool::decrypt::decryptfunc(reinterpret_cast<LPCH>(&magic[str->string]));
        UINT32 ref = ctx.AddStringValue(cstr);

        const auto* strings = reinterpret_cast<const UINT32*>(&str[1]);
        for (size_t j = 0; j < str->num_address; j++) {
            // no align too....
            *(UINT32*)(magic + strings[j]) = ref;
        }
        str_location += sizeof(*str) + sizeof(*strings) * str->num_address;
    }
}

int tool::gsc::T8GSCExport::DumpAsm(std::ostream& out, BYTE* gscFile, T8GSCOBJContext& objctx, opcode::asmcontext& ctx) const {
    // main reading loop
    while (ctx.FindNextLocation()) {
        while (true) {
            auto& base = ctx.Aligned<UINT16>();

            // mark the current location as handled
            auto& loc = ctx.PushLocation();
            loc.handled = true;

            if (ctx.m_lastOpCodeBase == -1) {
                ctx.m_lastOpCodeBase = loc.rloc;
            }

            UINT16 opCode = *(UINT16*)base;

            const auto* handler = ctx.LookupOpCode(opCode);


            out << "." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << loc.rloc << ": ";

            if (opCode & 0x1000) {
                out << std::hex << "FAILURE, FIND errec: " << handler->m_name << "(" << opCode << ")" << "\n";
                opCode &= 0xFFF;
                break;
            }

            out << std::hex << std::setfill('0') << std::setw(sizeof(INT16) << 1) << opCode
                << " "
                << std::setfill(' ') << std::setw(25) << std::left << handler->m_name << std::right
                << " ";


            // pass the opcode
            base += 2;

            // update asmcontext::WritePadding if you change the format

            auto ret = handler->Dump(out, opCode, ctx, objctx);

            if (ret) {
                break;
            }
        }
    }
    // no more location, we can assume the final size
    // maybe we don't end on a return/end, to check?
    out << "// final size: 0x" << std::hex << ctx.FinalSize() << "\n";

    return 0;
}


int tool::gsc::T8GSCExport::DumpVTable(std::ostream& out, BYTE* gscFile, T8GSCOBJContext& objctx, opcode::asmcontext& ctx, opcode::decompcontext& dctxt) const {
    using namespace tool::gsc::opcode;
    UINT16 code = *(UINT16*)ctx.Aligned<UINT16>();
    // main reading loop
    const auto* ccp = ctx.LookupOpCode(code);

/*
 * Start
 .00000000: 000d CheckClearParams
 .00000002: 000e PreScriptCall
 .00000004: 0eca ScriptFunctionCall        params: 0 spawnstruct
 .00000010: 0b16 GetZero
 .00000012: 0eef GetGlobalObject           classes
 .00000016: 05c0 EvalFieldVariableRef      cct_shared_warning
 .0000001c: 04ce EvalArrayRef
 .0000001e: 0c18 SetVariableField
*/
    
    if (!ccp || ccp->m_id != OPCODE_CheckClearParams) {
        dctxt.WritePadding(out) << "Bad opcode: " << std::hex << code << ", expected CheckClearParams\n";
        return -1;
    }

    ctx.m_bcl += 2;

    const auto* preScriptCall = ctx.LookupOpCode(code = *(UINT16*)ctx.Aligned<UINT16>());


    if (!preScriptCall || preScriptCall->m_id != OPCODE_PreScriptCall) {
        dctxt.WritePadding(out) << "Bad opcode: " << std::hex << code << ", expected PreScriptCall\n";
        return -1;
    }

    ctx.m_bcl += 2;

    const auto* spawnStruct = ctx.LookupOpCode(code = *(UINT16*)ctx.Aligned<UINT16>());

    
    if (!spawnStruct || (spawnStruct->m_id != OPCODE_ScriptFunctionCall && spawnStruct->m_id != OPCODE_CallBuiltinFunction)) {
        dctxt.WritePadding(out) << "Bad opcode: " << std::hex << code << ", expected ScriptFunctionCall\n";
        return -1;
    }

    ctx.m_bcl += 2 + 1;
    ctx.Aligned<UINT64>() += 8; // assume that we have a spawnstruct

    ctx.Aligned<UINT16>() += 2; // GetZero

    ctx.Aligned<UINT16>() += 2; // GetGlobalObject
    ctx.Aligned<UINT16>() += 2; // - classes

    ctx.Aligned<UINT16>() += 2; // EvalFieldVariableRef

    auto& clsName = ctx.Aligned<UINT32>();

    UINT32 name = *(UINT32*)clsName; // __vtable

    auto& cls = objctx.m_classes[name];

    clsName += 4;

    ctx.Aligned<UINT16>() += 2; // EvalArrayRef
    ctx.Aligned<UINT16>() += 2; // SetVariableField

    while (true) {
        auto& func = ctx.Aligned<UINT16>();

        UINT16 opcode = *(UINT16*)func;
        func += 2;

        const auto* funcOpCode = ctx.LookupOpCode(opcode);

        if (!funcOpCode) {
            dctxt.WritePadding(out) << "Bad opcode: " << std::hex << opcode << "\n";
            return -1;
        }
        if (funcOpCode->m_id == OPCODE_End) {
            break; // end
        }
        if (funcOpCode->m_id != OPCODE_GetResolveFunction) {
            dctxt.WritePadding(out) << "Bad opcode: " << std::hex << opcode << ", excepted GetResolveFunction or End\n";
            return -1;
        }

        auto& getFuncBase = ctx.Aligned<UINT64>();
        UINT32 methodName = ((UINT32*)getFuncBase)[0];
        UINT32 methodClsName = ((UINT32*)getFuncBase)[1];
        getFuncBase += 8;

        auto& uidCodeBase = ctx.Aligned<UINT16>();

        UINT16 uidCodeOp = *(UINT16*)uidCodeBase;

        const auto* uidCodeOpCode = ctx.LookupOpCode(uidCodeOp);

        uidCodeBase += 2;

        if (!uidCodeOpCode) {
            dctxt.WritePadding(out) << "Bad opcode: " << std::hex << uidCodeOp << ", excepted Getter\n";
            return -1;
        }

        UINT64 uid;

        switch (uidCodeOpCode->m_id) {
        case OPCODE_GetZero: // INT32
            uid = 0;
            break;
        case OPCODE_GetNegUnsignedShort: // UINT16
            uid = -*(UINT16*)ctx.Aligned<UINT16>();
            ctx.m_bcl += 2;
            break;
        case OPCODE_GetNegByte: // UINT8
            uid = -*(UINT8*)ctx.Aligned<UINT8>();
            ctx.m_bcl++;
            break;
        case OPCODE_GetByte: // BYTE
            uid = *(BYTE*)ctx.Aligned<BYTE>();
            ctx.m_bcl++;
            break;
        case OPCODE_GetInteger: // INT32
            uid = *(INT32*)ctx.Aligned<INT32>();
            ctx.m_bcl += 4;
            break;
        case OPCODE_GetLongInteger: // INT64
            uid = *(INT64*)ctx.Aligned<INT64>();
            ctx.m_bcl += 8;
            break;
        case OPCODE_GetUnsignedInteger: // UINT32
            uid = *(UINT32*)ctx.Aligned<UINT32>();
            ctx.m_bcl += 4;
            break;
        case OPCODE_GetUnsignedShort: // UINT16
            uid = *(UINT16*)ctx.Aligned<UINT16>();
            ctx.m_bcl += 2;
            break;
        default:
            dctxt.WritePadding(out) << "Bad opcode: " << std::hex << uidCodeOpCode->m_id << ", excepted Getter\n";
            return -1;
        }

        if (methodClsName == name) {
            cls.m_methods.emplace(methodName);
        }
        else {
            cls.m_superClass.emplace(methodClsName);
        }
        auto& mtd = cls.m_vtable[uid];
        mtd.name = methodName;
        mtd.nsp = methodClsName;
        dctxt.WritePadding(out) << "0x" << std::hex << std::setfill('0') << std::setw(sizeof(uid)) << uid
            << " -> &" << hashutils::ExtractTmp("class", methodClsName) << std::flush 
            << "::" << hashutils::ExtractTmp("function", methodName) << ";" << std::endl;

        ctx.Aligned<UINT16>() += 2; // GetZero

        ctx.Aligned<UINT16>() += 2; // EvalGlobalObjectFieldVariable
        ctx.Aligned<UINT16>() += 2; // - gvar
        ctx.Aligned<UINT32>() += 4; // - ref
        ctx.Aligned<UINT16>() += 2; // EvalArray
        ctx.Aligned<UINT16>() += 2; // CastFieldObject
        ctx.Aligned<UINT16>() += 2; // EvalFieldVariableRef
        ctx.Aligned<UINT32>() += 4; // - ref
        ctx.Aligned<UINT16>() += 2; // EvalArrayRef
        ctx.Aligned<UINT16>() += 2; // SetVariableField

    }
/*
* Field
.000002f4: 086c GetResolveFunction        &cct_shared_warning::__constructor
.00000300: 0a9a GetInteger                674154906
.00000308: 056d GetZero
.0000030a: 07f7 EvalGlobalObjectFieldVariable classes.cct_shared_warning
.00000314: 09e4 EvalArray
.00000316: 05e3 CastFieldObject
.00000318: 0af5 EvalFieldVariableRef      __vtable
.00000320: 0887 EvalArrayRef
.00000322: 00f3 SetVariableField
* End
End
*/
    dctxt.WritePadding(out) << "// class " << hashutils::ExtractTmp("class", name) << std::flush;
    if (cls.m_superClass.size()) {
        out << " : ";
        for (auto it = cls.m_superClass.begin(); it != cls.m_superClass.end(); it++) {
            if (it != cls.m_superClass.begin()) {
                out << ", ";
            }
            out << hashutils::ExtractTmp("class", *it) << std::flush;
        }
    }
    out << "\n";
    return 0;
}

void tool::gsc::T8GSCExport::DumpFunctionHeader(std::ostream& asmout, BYTE* gscFile, T8GSCOBJContext& objctx, opcode::asmcontext& ctx) const {
    bool classMember = flags & (T8GSCExportFlags::CLASS_MEMBER | T8GSCExportFlags::CLASS_DESTRUCTOR);

    if (ctx.m_opt.m_func_header) {
        asmout << "// Namespace "
            << hashutils::ExtractTmp(classMember ? "class" : "namespace", name_space) << std::flush << "/"
            << hashutils::ExtractTmp((flags & T8GSCExportFlags::EVENT) ? "event" : "namespace", callback_event) << std::endl;
        asmout << "// Params " << (int)param_count << ", eflags: 0x" << std::hex << (int)flags;

        if (flags == T8GSCExportFlags::CLASS_VTABLE) {
            asmout << " vtable";
        }
        else {
            if (flags & T8GSCExportFlags::LINKED) {
                asmout << " linked";
            }
            if (flags & T8GSCExportFlags::CLASS_LINKED) {
                asmout << " class_linked";
            }
        }

        asmout << std::endl;
        asmout << std::hex << "// Checksum 0x" << checksum << ", Offset: 0x" << (int)address << std::endl;

        auto size = ctx.FinalSize();
        if (size > 2) { // at least one opcode
            asmout << std::hex << "// Size: 0x" << size << "\n";
        }
    }

    if (flags == T8GSCExportFlags::CLASS_VTABLE) {
        asmout << "vtable " << hashutils::ExtractTmp("class", name);
    } else {
        asmout << "function ";
        if (flags & T8GSCExportFlags::AUTOEXEC) {
            asmout << "autoexec ";
        }
        if (flags & T8GSCExportFlags::EVENT) {
            asmout << "event<" << hashutils::ExtractTmp("event", callback_event) << "> " << std::flush;
        }
        if (flags & T8GSCExportFlags::PRIVATE) {
            asmout << "private ";
        }

        if (flags & T8GSCExportFlags::CLASS_DESTRUCTOR) {
            const auto* cls = hashutils::ExtractTmp("class", name_space);
            asmout << cls << "::~";
        }
        else if (classMember) {
            asmout << hashutils::ExtractTmp("class", name_space)
                << std::flush << "::";
        }

        asmout << hashutils::ExtractTmp("function", name);
    }

    asmout << std::flush << "(";

    // local var size = <empty>, <params>, <localvars> so we need to check that we have at least param_count + 1
    if (ctx.m_localvars.size() > param_count) {
        for (size_t i = 0; i < param_count; i++) {
            if (i) {
                asmout << ", ";
            }

            // -1 to avoid the <empty> object, -1 because we are in reverse order
            const auto& lvar = ctx.m_localvars[ctx.m_localvars.size() - i - 2];

            asmout << hashutils::ExtractTmp("var", lvar.name) << std::flush;

            if (lvar.flags & tool::gsc::opcode::T8GSCLocalVarFlag::VARIADIC) {
                asmout << "...";
            }
            else if (lvar.flags & tool::gsc::opcode::T8GSCLocalVarFlag::ARRAY_REF) {
                asmout << "&";
            }

            if (lvar.flags & ~(tool::gsc::opcode::T8GSCLocalVarFlag::VARIADIC | tool::gsc::opcode::T8GSCLocalVarFlag::ARRAY_REF)) {
                asmout << " (unk flags: " << std::hex << (int)lvar.flags << ")";
            }
            if (lvar.defaultValueNode) {
                asmout << " = ";
                opcode::decompcontext dctx = { 0, 0, ctx };
                lvar.defaultValueNode->Dump(asmout, dctx);
            }
        }
    }
    asmout << ")";
}

int tool::gsc::gscinfo(const process& proc, int argc, const char* argv[]) {
    GscInfoOption opt{};

    if (!opt.Compute(argv, 2, argc) || opt.m_help) {
        opt.PrintHelp(std::cout);
        return 0;
    }

    bool computed = false;
    auto ret = 0;
    for (const auto& file : opt.m_inputFiles) {
        auto lret = GscInfoFile(file, opt);
        if (!ret) {
            ret = lret;
        }
    }
    return ret;
}
