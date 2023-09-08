#include "gscBaseVisitor.h"
#include "gscLexer.h"
#include "gscParser.h"
#include "gscVisitor.h"
#include "antlr4-runtime.h"

#include <includes.hpp>

class GscCompilerOption {
public:
    bool m_help = false;
    std::vector<LPCCH> m_inputFiles{};

    bool Compute(LPCCH* args, INT startIndex, INT endIndex) {
        // default values
        for (size_t i = startIndex; i < endIndex; i++) {
            LPCCH arg = args[i];

            if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
                m_help = true;
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
            m_inputFiles.push_back(".");
        }
        return true;
    }

    void PrintHelp(std::ostream& out) {
        out << "-h --help          : Print help\n";
    }
};

enum GscFileType {
    FILE_GSC,
    FILE_CSC
};

class GscFile {
public:
    LPCCH filename;
    GscFileType type;
    size_t start;
    LPCH buffer;
    size_t size;

    ~GscFile() {
        std::free(buffer);
    }
};

int gsc::compiler::compiler(const Process& proc, int argc, const char* argv[]) {
    GscCompilerOption opt;
    if (!opt.Compute(argv, 2, argc) || opt.m_help) {
        opt.PrintHelp(std::cout);
        return 0;
    }

    std::vector<GscFile> files{};
    std::string gscData{};
    std::string cscData{};

    for (const auto& file : opt.m_inputFiles) {
        auto s = strlen(file);

        GscFileType type;
        size_t start;
        if (s < 4) {
            continue;
        }
        if (!strncmp(&file[s - 4], ".gsc", 4)) {
            type = FILE_GSC;
            start = gscData.size();
        }
        else if (!strncmp(&file[s - 4], ".csc", 4)) {
            type = FILE_GSC;
            start = cscData.size();
        }
        else {
            continue; // not a known file type, ignore
        }

        auto& dt = files.emplace_back(file, type, start);

        if (!utils::ReadFileNotAlign(file, reinterpret_cast<LPVOID&>(dt.buffer), dt.size, true)) {
            std::cerr << "Can't read file " << file << "\n";
            return tool::BASIC_ERROR;
        }


        switch (type)
        {
        case FILE_GSC:
            gscData = gscData + dt.buffer + "\n";
            break;
        case FILE_CSC:
            cscData = cscData + dt.buffer + "\n";
            break;
        default:
            break;
        }
    }

    antlr4::ANTLRInputStream is{ gscData };

    gscLexer lexer{ &is };
    antlr4::CommonTokenStream tokens{ &lexer };

    tokens.fill();

    gscParser parser{ &tokens };
    antlr4::tree::ParseTree* tree = parser.prog();

    std::cout << tree->toStringTree(&parser) << std::endl;



    return 0;
}
