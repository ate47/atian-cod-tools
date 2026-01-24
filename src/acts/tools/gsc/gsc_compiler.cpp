#include <includes.hpp>
#include <tools/gsc/compiler/gsc_compiler_parser.hpp>
#include <tools/gsc/compiler/gsc_compiler_script_object.hpp>
#include <tools/gsc/gsc_compiler.hpp>

namespace tool::gsc::compiler {
    using namespace tool::gsc::opcode;

    
    class GscCompilerOption {
    public:
        bool m_help{};
        const char* m_preproc{};
        const char* m_outFileName{ "compiled" };
        std::vector<const char*> m_inputFiles{};
        CompilerConfig config{};
        int32_t crcServer{};
        int32_t crcClient{};
        bool oneFilePerComp{};
        bool hashFile{};
        const char* nameServer{ "" };
        const char* nameClient{ "" };
        const char* fileNameSpaceServer{ "" };
        const char* fileNameSpaceClient{ "" };

        bool Compute(const char** args, INT startIndex, INT endIndex) {
            // default values
            for (size_t i = startIndex; i < endIndex; i++) {
                const char* arg = args[i];

                if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
                    m_help = true;
                }
                else if (!strcmp("-p", arg) || !_strcmpi("--platform", arg)) {
                    if (i + 1 == endIndex) {
                        LOG_ERROR("Missing value for param: {}!", arg);
                        return false;
                    }
                    config.platform = PlatformOf(args[++i]);

                    if (!config.platform) {
                        LOG_ERROR("Unknown platform: {}!", args[i]);
                        return false;
                    }
                }
                else if (!strcmp("-g", arg) || !_strcmpi("--game", arg)) {
                    if (i + 1 == endIndex) {
                        LOG_ERROR("Missing value for param: {}!", arg);
                        return false;
                    }
                    VmInfo* out{};

                    if (!IsValidVm(VMOf(args[++i]), out)) {
                        LOG_ERROR("Unknown game: {}!", args[i]);
                        return false;
                    }

                    config.vm = (VMId)out->vmMagic;
                }
                else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
                    if (i + 1 == endIndex) {
                        LOG_ERROR("Missing value for param: {}!", arg);
                        return false;
                    }
                    m_outFileName = args[++i];
                }
                else if (!strcmp("-d", arg) || !_strcmpi("--dbg", arg)) {
                    config.computeDevOption = true;
                }
                else if (!strcmp("-f", arg) || !_strcmpi("--file", arg)) {
                    oneFilePerComp = true;
                }
                else if (!strcmp("-O", arg) || !_strcmpi("--obfuscate", arg)) {
                    config.obfuscate = true;
                }
                else if (!_strcmpi("--dev-block-as-comment", arg)) {
                    config.processorOpt.devBlockAsComment = true;
                    LOG_WARNING("{} used, this message is just here to remind you that you're stupid.", arg);
                }
                else if (!_strcmpi("--preproc", arg)) {
                    if (i + 1 == endIndex) {
                        LOG_ERROR("Missing value for param: {}!", arg);
                        return false;
                    }
                    m_preproc = args[++i];
                }
                else if (!strcmp("-c", arg) || !_strcmpi("--csc", arg)) {
                    LOG_WARNING("{} option not required anymore", arg);
                }
                else if (!_strcmpi("--crc", arg)) {
                    if (i + 1 == endIndex) {
                        LOG_ERROR("Missing value for param: {}!", arg);
                        return false;
                    }
                    try {
                        crcServer = std::strtol(args[++i], nullptr, 16);
                    }
                    catch (std::exception& e) {
                        LOG_ERROR("Invalid crc for {}: {} / {}!", arg, args[i], e.what());
                        return false;
                    }
                }
                else if (!_strcmpi("--gen-crc", arg)) {
                    config.noDefaultChecksum = true;
                }
                else if (!_strcmpi("--crc-client", arg)) {
                    if (i + 1 == endIndex) {
                        LOG_ERROR("Missing value for param: {}!", arg);
                        return false;
                    }
                    try {
                        crcClient = std::strtol(args[++i], nullptr, 16);
                    }
                    catch (std::exception& e) {
                        LOG_ERROR("Invalid crc for {}: {} / {}!", arg, args[i], e.what());
                        return false;
                    }
                }
                else if (!_strcmpi("--name", arg)) {
                    if (i + 1 == endIndex) {
                        LOG_ERROR("Missing value for param: {}!", arg);
                        return false;
                    }
                    nameServer = args[++i];
                }
                else if (!_strcmpi("--name-client", arg)) {
                    if (i + 1 == endIndex) {
                        LOG_ERROR("Missing value for param: {}!", arg);
                        return false;
                    }
                    nameClient = args[++i];
                }
                else if (!_strcmpi("--namespace", arg)) {
                    if (i + 1 == endIndex) {
                        LOG_ERROR("Missing value for param: {}!", arg);
                        return false;
                    }
                    fileNameSpaceServer = args[++i];
                }
                else if (!_strcmpi("--namespace-client", arg)) {
                    if (i + 1 == endIndex) {
                        LOG_ERROR("Missing value for param: {}!", arg);
                        return false;
                    }
                    fileNameSpaceClient = args[++i];
                }
                else if (!_strcmpi("--define-as-constxpr", arg)) {
                    config.processorOpt.noDefineExpr = true;
                }
                else if (!_strcmpi("--no-devcall-inline", arg)) {
                    config.noDevCallInline = true;
                }
                else if (!_strcmpi("--mod-tool", arg)) {
                    config.useModToolOpCodes = true;
                }
                else if (!_strcmpi("--detour", arg)) {
                    if (i + 1 == endIndex) {
                        LOG_ERROR("Missing value for param: {}!", arg);
                        return false;
                    }
                    const char* dt = args[++i];
                    if (!_strcmpi(dt, "acts")) {
                        config.detourType = DETOUR_ACTS;
                    }
                    else if (!_strcmpi(dt, "gsic")) {
                        config.detourType = DETOUR_GSIC;
                    }
                    else if (_strcmpi(dt, "none")) {
                        LOG_ERROR("Invalid value value for param '{}': {}!", arg, dt);
                        return false;
                    }
                }
                else if (*arg == '-') {
                    if (arg[1] == 'D' && arg[2] && config.processorOpt.AddDefineConfig(arg + 2)) {
                        continue;
                    }

                    LOG_ERROR("Unknown option: {}!", arg);
                    return false;
                }
                else {
                    m_inputFiles.push_back(arg);
                }
            }
            if (!m_inputFiles.size()) {
                m_inputFiles.push_back(".");
            }
            if (!config.vm) {
                LOG_ERROR("No game set, please set a game using --game [game]");
                return false;
            }
            return true;
        }

        void PrintHelp() {
            LOG_INFO("-h --help              : Print help");
            {
                std::ostringstream games;

                size_t idx{};
                for (const auto& [vm, vmInfo] : tool::gsc::opcode::GetVMMaps()) {
                    if (idx++) games << ",";
                    games << " '" << vmInfo.internalName << "'";
                }

                LOG_INFO("-g --game [g]          : Set game, values:{}.", games.str());
            }
            {
                std::ostringstream platforms;

                size_t idx{};

                for (size_t i = 1; i < tool::gsc::opcode::PLATFORM_COUNT; i++) {
                    const char* plt = tool::gsc::opcode::PlatformIdName((tool::gsc::opcode::Platform)i);
                    if (i) platforms << ",";
                    platforms << " '" << plt << "'";
                }

                LOG_INFO("-p --platform [p]      : Set platform, values:{}.", platforms.str());
            }
            LOG_INFO("-d --dbg               : Add dev options");
            LOG_INFO("-o --output [f]        : Set output file (without extension), default: 'compiled'");
            LOG_INFO("-D[name]               : Define variable");
            LOG_INFO("-c --csc               : Build client script with csc files");
            LOG_INFO("-f --file              : Compile each file inside an independant one");
            LOG_INFO("--detour [t]           : Set the detour compilation type ('none' / 'acts' / 'gsic') default: 'none'");
            LOG_INFO("--crc [c]              : Set the crc for the server script");
            LOG_INFO("--crc-client [c]       : Set the crc for the client script");
            LOG_INFO("--name [n]             : Set the name for the server script");
            LOG_INFO("--name-client [n]      : Set the name for the client script");
            LOG_INFO("--namespace [n]        : Set the file namespace for the server script");
            LOG_INFO("--namespace-client [n] : Set the file namespace for the client script");
            LOG_INFO("--dev-block-as-comment : Consider /# #/ as comment markers");
            LOG_INFO("--define-as-constxpr   : Consider #define as constexpr");
            LOG_INFO("--no-devcall-inline    : Do not automatically inline dev calls in /# #/ blocks");
            LOG_INFO("-O --obfuscate         : Obfuscate some parts of the code");
            LOG_INFO("--mod-tool             : use mod tool opcodes (if available)");
            LOG_DEBUG("--preproc [f]         : Export preproc result into f");
        }
    };


    int gscc(int argc, const char* argv[]) {
        GscCompilerOption opt;
        if (!opt.Compute(argv, 2, argc) || opt.m_help) {
            opt.PrintHelp();
            return 0;
        }

        VmInfo* vmInfo{ opt.config.GetVm() };
        auto* readerBuilder = tool::gsc::GetGscReader(vmInfo->vmMagic);

        if (!readerBuilder) {
            LOG_ERROR("No GSC handler available for {}", vmInfo->name);
            return tool::BASIC_ERROR;
        }

        std::shared_ptr<tool::gsc::GSCOBJHandler> handler {(*readerBuilder)(nullptr, 0)};

        auto produceFile = [&opt, &handler](bool client, std::vector<std::filesystem::path>& inputs) -> int {
            opt.config.clientScript = client;
            VmInfo* vmInfo{ opt.config.GetVm() };

            opt.config.checksum = client ? opt.crcClient : opt.crcServer;

            if (!opt.config.checksum && !opt.config.noDefaultChecksum) {
                opt.config.checksum = (int32_t)handler->GetDefaultChecksum(client);
            }

            const char* fn = opt.config.name;
            // client ? opt.nameClient : opt.nameServer;
            const char* fileNameStr = fn && *fn ? fn : nullptr;

            if (!fileNameStr) {
                fileNameStr = handler->GetDefaultName(client);
            }

            if (client) {
                if (opt.fileNameSpaceClient && *opt.fileNameSpaceClient) opt.config.fileName = opt.fileNameSpaceClient;
            }
            else {
                if (opt.fileNameSpaceServer && *opt.fileNameSpaceServer) opt.config.fileName = opt.fileNameSpaceServer;
            }


            std::vector<std::filesystem::path> files{};
            for (const std::filesystem::path& file : inputs) {
                std::filesystem::path ext = file.extension();
                if (client) {
                    if (ext != ".csc" && ext != ".gcsc") {
                        continue;
                    }
                }
                else {
                    if (ext != ".gsc" && ext != ".gcsc") {
                        continue;
                    }
                }
                files.emplace_back(file);
            }

            if (files.empty()) return tool::OK; // nothing to compile
            std::vector<byte> data{};

            LOG_TRACE("Compile tree");

            std::string preprocout{};
            if (opt.m_preproc) {
                opt.config.preprocOutput = &preprocout;
            }

            std::unordered_set<std::string> hashes{};

            if (opt.hashFile) {
                opt.config.hashes = &hashes;
            }

            std::vector<byte> dbgdata{};
            opt.config.dbgoutput = &dbgdata;

            CompileGsc(inputs, data, opt.config);

            if (opt.m_preproc) {
                utils::WriteFile(opt.m_preproc, *opt.config.preprocOutput);
            }
            
            const char* outFile{ utils::va("%s.%s", opt.m_outFileName, client ? "cscc" : "gscc")};
            const char* dbgOutFile{ utils::va("%s.%s", opt.m_outFileName, client ? "csc.gdb" : "gsc.gdb")};
            std::filesystem::path outPath{ outFile };

            if (outPath.has_parent_path()) {
                std::filesystem::create_directories(outPath.parent_path());
            }

            if (!utils::WriteFile(outPath, (const void*)data.data(), data.size())) {
                LOG_ERROR("Error when writing out file {}", outFile);
                return tool::BASIC_ERROR;
            }

            LOG_INFO("Done into {} ({}/{})", outFile, vmInfo->codeName, PlatformName(opt.config.platform));

            if (!dbgdata.empty()) {
                if (!utils::WriteFile(dbgOutFile, (const void*)dbgdata.data(), dbgdata.size())) {
                    LOG_ERROR("Error when writing out dbg file {}", dbgOutFile);
                    return tool::BASIC_ERROR;
                }
                LOG_INFO("Debug file dump into {}", dbgOutFile);
            }

            if (opt.hashFile) {
                const char* outFileHash{ utils::va("%s.hash", outFile) };
                utils::OutFileCE hos{ outFileHash };
                if (!hos) {
                    LOG_ERROR("Can't open hash file {}", outFileHash);
                    return tool::BASIC_ERROR;
                }
                else {
                    for (const std::string& str : hashes) {
                        hos << str << "\n";
                    }
                    LOG_INFO("Hashes dumped into {}", outFileHash);
                }
            }

            return tool::OK;
        };

        if (opt.oneFilePerComp) {
            // create one per file
            char outnameDir[0x200]{};
            std::filesystem::path outDir{ opt.m_outFileName };
            int ret{ tool::OK };
            std::vector<std::filesystem::path> inputs{};
            std::vector<std::filesystem::path> singleInputs{};
            size_t doneFiles{};
            for (const char* file : opt.m_inputFiles) {
                std::filesystem::path base{ file };
                inputs.clear();
                utils::GetFileRecurse(base, inputs);

                for (const std::filesystem::path& path : inputs) {
                    std::filesystem::path ext{ path.extension() };
                    if (ext != ".gsc" && ext != ".csc" && ext != ".gcsc" && ext != ".gcsc") continue;
                    doneFiles++;

                    std::filesystem::path trueFileName{ std::filesystem::relative(path, base) };
                    std::filesystem::path outFile{ outDir / trueFileName };


                    std::string outFileName{ outFile.string() };
                    LOG_INFO("Compiling {} into {}", trueFileName.string(), outFileName);
                    outFile.replace_extension();
                    std::string outFileNameNE{ outFile.string() };

                    opt.config.name = outFileName.data();
                    std::string fileNamespace;
                    if (vmInfo->HasFlag(VmFlags::VMF_FULL_FILE_NAMESPACE)) {
                        fileNamespace = outFileName;
                    }
                    else {
                        fileNamespace = outFile.filename().string();
                    }
                    opt.config.fileName = fileNamespace.data();
                    opt.m_outFileName = outFileNameNE.data();

                    singleInputs.clear();
                    singleInputs.push_back(path);

                    try {
                        int sret;
                        
                        if (ext == ".gsc" || ext == ".gcsc") {
                            sret = produceFile(false, singleInputs);
                            if (sret != tool::OK) ret = sret;
                        }

                        if (ext == ".csc" || ext == ".gcsc") {
                            sret = produceFile(true, singleInputs);
                            if (sret != tool::OK) ret = sret;
                        }
                    }
                    catch (std::runtime_error& err) {
                        LOG_ERROR("Error when compiling: {}", err.what());
                        ret = tool::BASIC_ERROR;
                    }
                }

            }
            if (!ret && doneFiles) {
                LOG_ERROR("No file produced");
                return tool::BASIC_ERROR;
            }
            return ret;
        }
        else {
            std::vector<std::filesystem::path> inputs{};

            for (const char* file : opt.m_inputFiles) {
                utils::GetFileRecurse(file, inputs);
            }

            // build csc/gsc
            opt.config.name = opt.nameServer;
            int ret = produceFile(false, inputs);

            if (ret) {
                return ret;
            }
            opt.config.name = opt.nameClient;
            return produceFile(true, inputs);
        }
    }
    class ACTSErrorListener : public ConsoleErrorListener {
        InputInfo& m_info;
    public:
        ACTSErrorListener(InputInfo& info) : m_info(info) {
        }

        void syntaxError(Recognizer* recognizer, Token* offendingSymbol, size_t line, size_t charPositionInLine,
            const std::string& msg, std::exception_ptr e) override {
            m_info.container.PrintLineMessage(core::logs::LVL_ERROR, line, charPositionInLine, msg);
        }
    };


    void CompileGsc(const std::vector<std::filesystem::path>& files, std::vector<byte>& data, CompilerConfig& config) {
        VmInfo* vmInfo{ config.GetVm() };
        InputInfo info{};
        for (const std::filesystem::path& file : files) {
            if (!info.container.AppendFile(file)) {
                throw std::runtime_error(std::format("Can't read file '{}'", file.string()));
            }
        }

        core::preprocessor::PreProcessorOption& popt = config.processorOpt;
        popt.AddDefine("_SUPPORTS_GCSC");
        if (config.detourType) {
            popt.AddDefine("_SUPPORTS_DETOURS");
        }
        popt.AddDefine(utils::UpperCase(utils::va("_%s", vmInfo->codeName)));
        popt.AddDefine(utils::MapString(utils::va("_%s", PlatformName(config.platform)), [](char c) -> char { return isspace(c) ? '_' : std::toupper(c); }));

        if (tool::gsc::opcode::HasOpCode(config.vm, config.platform, OPCODE_T8C_GetLazyFunction)) {
            popt.AddDefine("_SUPPORTS_LAZYLINK");
        }

        if (config.clientScript) {
            popt.AddDefine("_CSC");
        }
        else {
            popt.AddDefine("_GSC");
        }

        if (!popt.ApplyPreProcessor(info.container.data,
            [&info](core::logs::loglevel lvl, size_t line, const std::string& message) { info.container.PrintLineMessage(lvl, line, 0, message); })) {
            throw std::runtime_error("Error when applying preprocessor on data");
        }

        if (config.preprocOutput) {
            *config.preprocOutput = info.container.data;
        }

        ANTLRInputStream is{ info.container.data };

        std::unique_ptr<ACTSErrorListener> errList = std::make_unique<ACTSErrorListener>(info);

        gscLexer lexer{ &is };
        lexer.addErrorListener(&*errList);
        CommonTokenStream tokens{ &lexer };

        tokens.fill();
        gscParser parser{ &tokens };

        parser.removeErrorListeners();

        parser.addErrorListener(&*errList);

        gscParser::ProgContext* prog = parser.prog();

        auto* readerBuilder = tool::gsc::GetGscReader(vmInfo->vmMagic);

        if (!readerBuilder) {
            throw std::runtime_error(std::format("No GSC handler available for {}", vmInfo->name));
        }

        std::shared_ptr<tool::gsc::GSCOBJHandler> handler{ (*readerBuilder)(nullptr, 0) };

        CompileObject obj{ config, config.clientScript ? FILE_CSC : FILE_GSC, info, handler };

        size_t error = parser.getNumberOfSyntaxErrors();
        if (error) {
            throw std::runtime_error(std::format("{} error(s) detected, abort", error));
        }

        LOG_TRACE("Parse tree");

        if (!ParseProg(prog, parser, obj)) {
            throw std::runtime_error("Error when parsing the object");
        }


        RegisterOpCodesMap();

        if (!obj.Compile(data, config.dbgoutput)) {
            throw std::runtime_error("Error when compiling the object");
        }
    }

    ADD_TOOL(gscc, "gsc", "", "GSC compiler", gscc);
}