#include <includes.hpp>

class GscInfoOption {
public:
    bool m_dcomp;
    bool m_dasm;
    bool m_help;
    bool m_verbose;
    bool m_header;
    bool m_imports;
    bool m_strings;
    bool m_gvars;

    std::vector<LPCCH> m_inputFiles{};
    bool Compute(LPCCH* args, INT startIndex, INT endIndex) {
        // default values
        m_dcomp = false;
        m_dasm = false;
        m_help = false;
        m_verbose = true;
        m_header = false;
        m_imports = false;
        m_strings = false;
        m_gvars = false;
        m_inputFiles.clear();

        for (size_t i = startIndex; i < endIndex; i++) {
            LPCCH arg = args[i];

            if (!_strcmpi("-?", arg) || !_strcmpi("--help", arg) || !_strcmpi("-h", arg)) {
                m_help = true;
            }
            else if (!_strcmpi("-g", arg) || !_strcmpi("--gsc", arg)) {
                m_dcomp = true;
            }
            else if (!_strcmpi("-a", arg) || !_strcmpi("--asm", arg)) {
                m_dasm = true;
            }
            else if (!_strcmpi("-s", arg) || !_strcmpi("--silent", arg)) {
                m_verbose = false;
            }
            else if (!_strcmpi("-H", arg) || !_strcmpi("--header", arg)) {
                m_header = true;
            }
            else if (!_strcmpi("-I", arg) || !_strcmpi("--imports", arg)) {
                m_imports = true;
            }
            else if (!_strcmpi("-S", arg) || !_strcmpi("--strings", arg)) {
                m_strings = true;
            }
            else if (!_strcmpi("-G", arg) || !_strcmpi("--gvars", arg)) {
                m_gvars = true;
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
    void PrintHelp(std::ostream& out) {
        out << "-h --help    : Print help\n";
        out << "-g --gsc     : Produce GSC\n";
        out << "-a --asm     : Produce ASM\n";
        out << "-s --silent  : Silent output, only errors\n";
        out << "-H --header  : Write file header\n";
        out << "-I --imports : Write imports\n";
        out << "-S --strings : Write strings\n";
        out << "-G --gvars   : Write gvars\n";
    }
};

int GscInfoHandleData(tool::gsc::T8GSCOBJ* data, size_t size, const char* path, const GscInfoOption& opt) {
    using namespace tool::gsc;
    hashutils::ReadDefaultFile();

    T8GSCOBJContext ctx{};

    // unlink the script and write custom gvar/string ids
    data->PatchCode(ctx);

    char asmfnamebuff[1000];
    snprintf(asmfnamebuff, 1000, "%sasm", path);

    std::ofstream asmout{ asmfnamebuff };

    if (!asmout) {
        std::cerr << "Can't open output file " << asmfnamebuff << "\n";
        return -1;
    }
    if (opt.m_header) {
        asmout
            << "// file size ........... " << size << " Bytes" << std::hex << " (0x" << size << ")\n"
            << "// magic ............... " << *reinterpret_cast<UINT64*>(&data->magic[0]) << "\n"
            << "// vm .................. " << (UINT32)data->magic[7] << "\n"
            << "// crc ................. " << data->crc << "\n"
            << "// name ................ " << hashutils::ExtractTmp("script", data->name) << " (" << data->name << ")" << "\n"
            << "// include_offset ...... " << data->include_offset << "\n"
            << "// string_count ........ " << data->string_count << "\n"
            << "// exports_count ....... " << data->exports_count << "\n"
            << "// ukn20 ............... " << data->ukn20 << "\n"
            << "// string_offset ....... " << data->string_offset << "\n"
            << "// imports_count ....... " << data->imports_count << "\n"
            << "// fixup_count ......... " << data->fixup_count << "\n"
            << "// ukn2c ............... " << data->ukn2c << "\n"
            << "// export_table_offset . " << data->export_table_offset << "\n"
            << "// ukn34 ............... " << data->ukn34 << "\n"
            << "// imports_offset ...... " << data->imports_offset << "\n"
            << "// globalvar_count ..... " << data->globalvar_count << "\n"
            << "// fixup_offset ........ " << data->fixup_offset << "\n"
            << "// globalvar_offset .... " << data->globalvar_offset << "\n"
            << "// script_size ......... " << data->script_size << "\n"
            << "// ukn4c_offset ........ " << data->ukn4c_offset << "\n"
            << "// ukn50 ............... " << data->ukn50 << "\n"
            << "// ukn54 ............... " << data->ukn54 << "\n"
            << "// include_count ....... " << data->include_count << "\n"
            << "// ukn5a ............... " << (int)data->ukn5a << "\n"
            << "// ukn4c_count ......... " << (int)data->ukn4c_count << "\n";
    }

    UINT64 *includes = reinterpret_cast<UINT64*>(&data->magic[data->include_offset]);

    for (size_t i = 0; i < data->include_count; i++) {
        asmout << "#include " << hashutils::ExtractTmp("script", includes[i]) << "\n";
    }
    if (data->include_count) {
        asmout << "\n";
    }


    auto* fixups = reinterpret_cast<T8GSCFixup*>(&data->magic[data->fixup_offset]);

    for (size_t i = 0; i < data->fixup_count; i++) {
        const auto& fixup = fixups[i];
        asmout << std::hex << "#fixup 0x" << fixup.offset << " = 0x" << fixup.address << "\n";
    }

    if (data->fixup_count) {
        asmout << "\n";
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

    auto* exports = reinterpret_cast<T8GSCExport*>(&data->magic[data->export_table_offset]);

    for (size_t i = 0; i < data->exports_count; i++) {
        const auto& exp = exports[i];
        auto asmctx = opcode::asmcontext(&data->magic[exp.address]);

        std::ofstream nullstream;
        nullstream.setstate(std::ios_base::badbit);

        std::ostream& output = opt.m_dasm ? asmout : nullstream;


        exp.DumpFunctionHeader(output, data->magic, ctx, asmctx);

        output << "{ __gscasm{\n";

        exp.DumpAsm(output, data->magic, ctx, asmctx);

        output << "} } \n";

        if (opt.m_dcomp) {
            exp.DumpFunctionHeader(asmout, data->magic, ctx, asmctx);
            asmout << " {\n";

            // decompile ctx

            asmout << "}\n\n";
        }
    }
    auto* ukn4c = reinterpret_cast<UINT64*>(&data->magic[data->ukn4c_offset]);

    for (size_t i = 0; i < data->ukn4c_count; i++) {
        asmout << "ukn4c: "  << hashutils::ExtractTmp("hash", ukn4c[i]) << "\n";
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
                loc[1] = imp->import_namespace;
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
    while (ctx.FindNextLocation()) {
        while (true) {
            auto& base = ctx.Aligned<UINT16>();

            // mark the current location as handled
            ctx.PushLocation().handled = true;

            UINT16 opCode = *(UINT16*)base;
            // pass the opcode
            auto location = ctx.FunctionRelativeLocation();

            base += 2;
         
            const auto* handler = tool::gsc::opcode::LookupOpCode(opCode);


            out << "." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << location << ": ";

            if (opCode & 0x1000) {
                out << std::hex << "FAILURE, FIND errec: " << handler->m_name << "(" << opCode << ")" << "\n";
                opCode &= 0xFFF;
                break;
            }

            out << std::hex << std::setfill('0') << std::setw(sizeof(INT16) << 1) << opCode
                << " "
                << std::setfill(' ') << std::setw(25) << std::left << handler->m_name << std::right
                << " ";

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

void tool::gsc::T8GSCExport::DumpFunctionHeader(std::ostream& asmout, BYTE* gscFile, T8GSCOBJContext& objctx, opcode::asmcontext& ctx) const {
    bool classMember = flags & (T8GSCExportFlags::CLASS_MEMBER | T8GSCExportFlags::CLASS_DESTRUCTOR);
    asmout << "// Namespace "
        << hashutils::ExtractTmp(classMember ? "class" : "namespace", name_space) << std::flush << "/"
        << hashutils::ExtractTmp((flags & T8GSCExportFlags::EVENT) ? "event" : "namespace", callback_event) << std::endl;
    asmout << "// Params " << (int)param_count << ", eflags: 0x" << std::hex << (int)flags;

    if (flags & T8GSCExportFlags::LINKED) {
        asmout << " linked";
    }
    if (flags & T8GSCExportFlags::CLASS_LINKED) {
        asmout << " class_linked";
    }

    asmout << std::endl;
    asmout << std::hex << "// Checksum 0x" << checksum << ", Offset: 0x" << (int)address << std::endl;

    auto size = ctx.FinalSize();
    if (size > 2) { // at least one opcode
        asmout << std::hex << "// Size: 0x" << size << "\n";
    }

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
    if (flags & T8GSCExportFlags::VE) {
        asmout << "ve ";
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

    asmout << "(";

    for (size_t i = 0; i < param_count && i < ctx.m_localvars.size(); i++) {
        if (i) {
            asmout << ", ";
        }

        const auto& lvar = ctx.m_localvars[i];

        asmout << hashutils::ExtractTmp("var", lvar.name) << std::flush;

        if (lvar.flags & tool::gsc::opcode::T8GSCLocalVarFlag::VARIADIC) {
            asmout << "...";
        }
        else if (lvar.flags & tool::gsc::opcode::T8GSCLocalVarFlag::ARRAY_REF) {
            asmout << "&";
        }

        if (lvar.flags & ~(tool::gsc::opcode::T8GSCLocalVarFlag::VARIADIC | tool::gsc::opcode::T8GSCLocalVarFlag::ARRAY_REF)) {
            asmout << " unk flags: " << std::hex << (int)lvar.flags;
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