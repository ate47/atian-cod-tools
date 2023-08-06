#include <includes.hpp>

int GscInfoHandleData(tool::gsc::T8GSCOBJ* data, size_t size, const char* path) {
    hashutils::ReadDefaultFile();

    data->PatchCode();

    char asmfnamebuff[1000];
    snprintf(asmfnamebuff, 1000, "%sasm", path);

    std::ofstream asmout{ asmfnamebuff };

    if (!asmout) {
        std::cerr << "Can't open output file " << asmfnamebuff << "\n";
        return -1;
    }

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

    UINT64 *includes = reinterpret_cast<UINT64*>(&data->magic[data->include_offset]);

    for (size_t i = 0; i < data->include_count; i++) {
        asmout << "#include " << hashutils::ExtractTmp("script", includes[i]) << "\n";
    }


    auto* fixups = reinterpret_cast<tool::gsc::T8GSCFixup*>(&data->magic[data->fixup_offset]);

    for (size_t i = 0; i < data->fixup_count; i++) {
        const auto& fixup = fixups[i];
        asmout << std::hex << "#fixup 0x" << fixup.offset << " = 0x" << fixup.address << "\n";
    }

    uintptr_t gvars_location = reinterpret_cast<uintptr_t>(data->magic) + data->globalvar_offset;

    for (size_t i = 0; i < data->globalvar_count; i++) {
        const auto* globalvar = reinterpret_cast<tool::gsc::T8GSCGlobalVar*>(gvars_location);
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

    uintptr_t str_location = reinterpret_cast<uintptr_t>(data->magic) + data->string_offset;

    for (size_t i = 0; i < data->string_count; i++) {

        const auto* str = reinterpret_cast<tool::gsc::T8GSCString*>(str_location);

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

    uintptr_t import_location = reinterpret_cast<uintptr_t>(data->magic) + data->imports_offset;

    for (size_t i = 0; i < data->imports_count; i++) {

        const auto* imp = reinterpret_cast<tool::gsc::T8GSCImport*>(import_location);
        asmout << std::hex << "-- import " << hashutils::ExtractTmp("namespace", imp->import_namespace) << std::flush;
        asmout << std::hex << "::" << hashutils::ExtractTmp("function", imp->name) << " --\n";
        asmout << std::hex << "address: " << imp->num_address <<
            ", params: " << (int)imp->param_count <<
            ", flags: " << std::hex << (UINT16)(imp->flags) << "\n";

        asmout << "location(s): ";

        const auto* imports = reinterpret_cast<const UINT32*>(&imp[1]);
        asmout << std::hex << imports[0];
        for (size_t j = 1; j < imp->num_address; j++) {
            asmout << std::hex << "," << imports[j];
        }
        asmout << "\n";

        asmout << "--------------\n";

        str_location += sizeof(*imp) + sizeof(*imports) * imp->num_address;
    }

    auto* exports = reinterpret_cast<tool::gsc::T8GSCExport*>(&data->magic[data->export_table_offset]);

    for (size_t i = 0; i < data->exports_count; i++) {
        const auto& exp = exports[i];
        asmout << "// Namespace " << hashutils::ExtractTmp("namespace", exp.name_space) << std::endl;
        asmout << "// Params " << (int)exp.param_count << ", Flags: 0x" << std::hex << (int)exp.flags << std::endl;
        asmout << std::hex << "// Checksum 0x" << exp.checksum << ", Offset: 0x" << (int)exp.address << std::endl;

        asmout << "function ";
        if (exp.flags & tool::gsc::T8GSCExportFlags::AUTOEXEC) {
            asmout << "autoexec ";
        }
        if (exp.flags & tool::gsc::T8GSCExportFlags::EVENT) {
            asmout << "event<" << hashutils::ExtractTmp("event", exp.callback_event) << "> " << std::flush;
        }
        if (exp.flags & tool::gsc::T8GSCExportFlags::PRIVATE) {
            asmout << "private ";
        }
        if (exp.flags & tool::gsc::T8GSCExportFlags::UKN08) {
            asmout << "unk08 ";
        }
        if (exp.flags & tool::gsc::T8GSCExportFlags::UKN10) {
            asmout << "unk10 ";
        }
        if (exp.flags & tool::gsc::T8GSCExportFlags::UKN20) {
            asmout << "ukn20 ";
        }

        asmout << hashutils::ExtractTmp("function", exp.name) << "() {}\n";

    }
    auto* ukn4c = reinterpret_cast<UINT64*>(&data->magic[data->ukn4c_offset]);

    for (size_t i = 0; i < data->ukn4c_count; i++) {
        asmout << "ukn4c: "  << hashutils::ExtractTmp("hash", ukn4c[i]) << "\n";
    }

    asmout.close();

    return 0;
}

int GscInfoFile(const std::filesystem::path& path) {
    if (std::filesystem::is_directory(path)) {
        // directory
        auto ret = 0;
        for (const auto& sub : std::filesystem::directory_iterator{path}) {
            auto lret = GscInfoFile(sub.path());
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
    if (!utils::ReadFile(path, buffer, size)) {
        std::cerr << "Can't read file data for " << path << "\n";
        return -1;
    }

    if (size < sizeof(tool::gsc::T8GSCOBJ)) {
        std::cerr << "Bad header, file size: " << std::hex << size << "/" << sizeof(tool::gsc::T8GSCOBJ) << " for " << path << "\n";
        std::free(buffer);
        return -1;
    }

    auto ret = GscInfoHandleData(reinterpret_cast<tool::gsc::T8GSCOBJ*>(buffer), size, pathname.c_str());
    std::free(buffer);
    return ret;
}

int tool::gsc::gscinfo(const process& proc, int argc, const char* argv[]) {
    if (argc == 2) {
        return GscInfoFile("scriptparsetree");
    }

    auto ret = 0;
    for (size_t i = 2; i < argc; i++) {
        auto lret = GscInfoFile(argv[i]);
        if (!ret) {
            ret = lret;
        }
    }
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
    if (!utils::ReadFile(path, buffer, size)) {
        std::cerr << "Can't read file data for " << path << "\n";
        return -1;
    }

    if (size < sizeof(tool::gsc::T8GSCOBJ)) {
        std::cerr << "Bad header, file size: " << std::hex << size << "/" << sizeof(tool::gsc::T8GSCOBJ) << " for " << path << "\n";
        std::free(buffer);
        return -1;
    }

    auto ret = DumpInfoFileData(reinterpret_cast<tool::gsc::T8GSCOBJ*>(buffer), size, pathname.c_str(), dataset);
    std::free(buffer);
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

void tool::gsc::T8GSCOBJ::PatchCode() {
}
