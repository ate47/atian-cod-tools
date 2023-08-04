#include "dump.hpp"

struct ScriptParseTreeEntry {
public:
    UINT64 name;
    UINT64 pad0;
    UINT64 buffer;
    UINT32 size;
    UINT32 pad02;
};

int tool::dump::poolscripts(const process& proc, int argc, const char* argv[]) {
    uintptr_t poolPtr = proc.ReadMemory<uintptr_t>(proc[OFFSET_XASSET_SCRIPTPARSETREE]);
    INT32 poolSize = proc.ReadMemory<INT32>(proc[OFFSET_XASSET_SCRIPTPARSETREE + 0x14]);
    ScriptParseTreeEntry* buffer = new ScriptParseTreeEntry[poolSize];

    std::cout << std::hex << "pool: " << poolPtr << ", elements: " << std::dec << poolSize << "\n";

    if (!proc.ReadMemory(buffer, poolPtr, sizeof *buffer * poolSize)) {
        std::cerr << "Can't read pool data\n";
        return -1;
    }

    LPCCH outFile;
    if (argc == 2) {
        outFile = "pool.csv";
    }
    else {
        outFile = argv[2];
    }

    std::ofstream out{ outFile, std::ios::out };

    if (!out) {
        std::cerr << "Can't open output file '" << outFile << "'\n";
        return -1;
    }

    // csv header
    out << "index,name,size,buffer\n";

    for (size_t i = 0; i < poolSize; i++) {
        const auto& ref = buffer[i];
        out << i << "," << hashutils::ExtractTmp("script", ref.name) << "," << ref.size << "," << std::hex << ref.buffer << std::dec << "\n";
    }


    out.close();
    delete[] buffer;

    return 0;
}
