#include <includes.hpp>
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include <pool.hpp>
#include <offsets.hpp>
#include <scriptinstance.hpp>

using namespace tool::dump;

int poolscripts(Process& proc, int argc, const char* argv[]) {
    uintptr_t poolPtr = proc.ReadMemory<uintptr_t>(proc[offset::XASSET_SCRIPTPARSETREE]);
    int32_t poolSize = proc.ReadMemory<int32_t>(proc[offset::XASSET_SCRIPTPARSETREE + 0x14]);
    T8ScriptParseTreeEntry* buffer = new T8ScriptParseTreeEntry[poolSize];

    std::cout << std::hex << "pool: " << poolPtr << ", elements: " << std::dec << poolSize << "\n";

    if (!proc.ReadMemory(buffer, poolPtr, sizeof *buffer * poolSize)) {
        std::cerr << "Can't read pool data\n";
        delete[] buffer;
        return -1;
    }

    // cache reading to avoid writing empty file
    hashutils::ReadDefaultFile();

    const char* outFile;
    if (argc == 2) {
        outFile = "pool.csv";
    }
    else {
        outFile = argv[2];
    }

    std::ofstream out{ outFile, std::ios::out };

    if (!out) {
        std::cerr << "Can't open output file '" << outFile << "'\n";
        delete[] buffer;
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

int writepoolscripts(Process& proc, int argc, const char* argv[]) {
    uintptr_t poolPtr = proc.ReadMemory<uintptr_t>(proc[offset::XASSET_SCRIPTPARSETREE]);
    int32_t poolSize = proc.ReadMemory<int32_t>(proc[offset::XASSET_SCRIPTPARSETREE + 0x14]);
    T8ScriptParseTreeEntry* buffer = new T8ScriptParseTreeEntry[poolSize];
    std::cout << std::hex << "pool: " << poolPtr << ", elements: " << std::dec << poolSize << "\n";

    if (!proc.ReadMemory(buffer, poolPtr, sizeof * buffer * poolSize)) {
        std::cerr << "Can't read pool data\n";
        delete[] buffer;
        return -1;
    }

    // cache reading to avoid writing empty file
    hashutils::ReadDefaultFile();

    const char* outFile;
    if (argc == 2) {
        outFile = "scriptparsetree";
    }
    else {
        outFile = argv[2];
    }

    int readFile = 0;
    int readFileOrigin[3] = {0,0,0};
    std::error_code ec;

    size_t allocated = 0x1000;
    void* storage = std::malloc(allocated);
    char nameBuffer[1000];

    for (size_t i = 0; i < poolSize; i++) {
        const auto& ref = buffer[i];

        if (allocated < ref.size) {
            void* ns = std::realloc(storage, ref.size + 100);
            if (ns) {
                allocated = ref.size + 100;
                storage = ns;
            }
            else {
                std::cerr << "Can't allocate buffer for address " << std::hex << ref.buffer << " of size " << std::dec << ref.size << "\n";
                continue; // bad size?
            }
        }

        sprintf_s(nameBuffer, "%s/script_%llx.gscc", outFile, ref.name);

        if (!proc.ReadMemory(storage, ref.buffer, ref.size)) {
            std::cerr << "Can't read pooled buffer at address " << std::hex << ref.buffer << " of size " << std::dec << ref.size << " for file " << nameBuffer << "\n";
            continue;
        }

        std::filesystem::path file(nameBuffer);
        
        std::filesystem::create_directories(file.parent_path(), ec);
        if (!std::filesystem::exists(file, ec)) {
            readFile++;
            readFileOrigin[0]++;
        }
        if (!utils::WriteFile(file, storage, ref.size)) {
            std::cerr << "Error when writing " << nameBuffer << "\n";
        }
    }

    delete[] buffer;



    uint32_t bufferCount[2];
    if (!proc.ReadMemory(bufferCount, proc[offset::gObjFileInfoCount], sizeof * bufferCount * 2)) {
        std::cerr << "Can't read pool count data\n";
        return -1;
    }

    const int size = 650;
    T8ObjFileInfo* bufferLinked = new T8ObjFileInfo[2 * size];

    if (!proc.ReadMemory(bufferLinked, proc[offset::gObjFileInfo], sizeof * bufferLinked * size * 2)) {
        std::cerr << "Can't read linked data\n";
        delete[] bufferLinked;
        return -1;
    }

    tool::gsc::T8GSCOBJ gsc;
    const byte magic[8] = { 0x80, 0x47, 0x53, 0x43, 0x0D, 0x0A, 0x00, 0x36 };
    const uint64_t magicLong = *(uint64_t*)(&magic[0]);

    for (size_t inst = 0; inst < scriptinstance::SI_COUNT; inst++) {
        std::cout << std::dec << "Reading " << bufferCount[inst] << " " << scriptinstance::Name(inst) << " linked script(s)\n";
        for (size_t i = 0; i < bufferCount[inst]; i++) {
            const auto& ref = bufferLinked[i + size * inst];

            if (!proc.ReadMemory(&gsc, ref.activeVersion, sizeof gsc)) {
                std::cerr << "Can't read memory from location " << std::hex << ref.activeVersion << ", index: " << i << "\n";
                continue;
            }
            if (*(uint64_t*)(&gsc.magic[0]) != magicLong) {
                std::cerr << "Bad magic for location " << std::hex << ref.activeVersion << " " << *(uint64_t*)(&gsc.magic[0]) << " != " << magicLong << "\n";
                continue;
            }

            if (allocated < gsc.script_size) {
                void* ns = std::realloc(storage, gsc.script_size + 100);
                if (ns) {
                    allocated = gsc.script_size + 100;
                    storage = ns;
                }
                else {
                    std::cerr << "Can't allocate buffer for address " << std::hex << ref.activeVersion << " of size " << std::dec << gsc.script_size << "\n";
                    continue; // bad size?
                }
            }

            sprintf_s(nameBuffer, "%s/script_%llx.gscc", outFile, gsc.name);

            if (!proc.ReadMemory(storage, ref.activeVersion, gsc.script_size)) {
                std::cerr << "Can't read pooled buffer at address " << std::hex << ref.activeVersion << " of size " << std::dec << gsc.script_size << " for file " << nameBuffer << "\n";
                continue;
            }

            std::filesystem::path file(nameBuffer);

            std::filesystem::create_directories(file.parent_path(), ec);
            if (!std::filesystem::exists(file, ec)) {
                readFile++;
                readFileOrigin[1 + inst]++;
                //std::cout << "The script " << nameBuffer << " is linked, but wasn't found in the pool\n";
            }
            if (!utils::WriteFile(file, storage, gsc.script_size)) {
                std::cerr << "Error when writing " << nameBuffer << "\n";
            }
        }
    }

    std::cout << std::dec << readFile << " new file(s) created.\n"
        << "Pool ... " << readFileOrigin[0] << "\n"
        << "Linked . " << readFileOrigin[1] << " (" << scriptinstance::Name(0) << ")\n"
        << "Linked . " << readFileOrigin[2] << " (" << scriptinstance::Name(1) << ")\n";
    

    std::free(storage);
    delete[] bufferLinked;

    return 0;
}

int linkedscripts(Process& proc, int argc, const char* argv[]) {
    uint32_t bufferCount[2];
    if (!proc.ReadMemory(bufferCount, proc[offset::gObjFileInfoCount], sizeof *bufferCount * 2)) {
        std::cerr << "Can't read count data\n";
        return -1;
    }

    const int size = 650;
    T8ObjFileInfo* buffer = new T8ObjFileInfo[2 * size];

    if (!proc.ReadMemory(buffer, proc[offset::gObjFileInfo], sizeof *buffer * size * 2)) {
        std::cerr << "Can't read linked data\n";
        delete[] buffer;
        return -1;
    }


    const char* outFile;
    if (argc == 2) {
        outFile = "linked.csv";
    }
    else {
        outFile = argv[2];
    }

    // cache reading to avoid writing empty file
    hashutils::ReadDefaultFile();

    std::ofstream out{ outFile, std::ios::out };

    if (!out) {
        std::cerr << "Can't open output file '" << outFile << "'\n";
        delete[] buffer;
        return -1;
    }



    // csv header
    out << "pool,name,slot,refcount,groupid,address\n";

    T8GSCSimpleHeader gsc;
    const byte magic[8] = { 0x80, 0x47, 0x53, 0x43, 0x0D, 0x0A, 0x00, 0x36};
    const uint64_t magicLong = *(uint64_t*)(&magic[0]);

    for (size_t inst = 0; inst < scriptinstance::SI_COUNT; inst++) {
        std::cout << std::dec << "Reading " << bufferCount[inst] << " " << scriptinstance::Name(inst) << " script(s)\n";
        for (size_t i = 0; i < bufferCount[inst]; i++) {
            const auto& ref = buffer[i + size * inst];

            if (!proc.ReadMemory(&gsc, ref.activeVersion, sizeof gsc)) {
                std::cerr << "Can't read memory from location " << std::hex << ref.activeVersion << ", index: " << i << "\n";
                continue;
            }
            if (gsc.magic != magicLong) {
                std::cerr << "Bad magic for location " << std::hex << ref.activeVersion << " " << gsc.magic << " != " << magicLong << "\n";
                continue;
            }

            out << std::dec << scriptinstance::Name(inst) 
                << "," << hashutils::ExtractTmp("script", gsc.name)
                << "," << ref.slot
                << "," << ref.refCount
                << "," << ref.groupId
                << "," << std::hex << ref.activeVersion
                << "\n";
        }
    }


    out.close();
    delete[] buffer;

    return 0;
}

int events(Process& proc, int argc, const char* argv[]) {
    const int size = 512;
    T8EventMapObj* buffer = new T8EventMapObj[2 * size];

    if (!proc.ReadMemory(buffer, proc[offset::VM_EVENT], sizeof * buffer * size * 2)) {
        std::cerr << "Can't read event data\n";
        delete[] buffer;
        return -1;
    }

    const char* outFile;
    if (argc == 2) {
        outFile = "events.csv";
    }
    else {
        outFile = argv[2];
    }

    // cache reading to avoid writing empty file
    hashutils::ReadDefaultFile();

    std::ofstream out{ outFile, std::ios::out };

    if (!out) {
        std::cerr << "Can't open output file '" << outFile << "'\n";
        delete[] buffer;
        return -1;
    }

    // csv header
    out << "pool,name,gamesideregisterd\n";

    int read = 0;

    for (size_t inst = 0; inst < scriptinstance::SI_COUNT; inst++) {
        for (size_t i = 0; i < size; i++) {
            const auto& ref = buffer[i + size * inst];
            if (!ref.name) {
                continue; // ignore empty
            }
            out << scriptinstance::Name(inst) << "," << hashutils::ExtractTmp("event", ref.name) << "," << (ref.gameSideRegistered ? "true" : "false") << "\n";
            read++;
        }
    }

    std::cout << std::dec << "Read " << read << " event(s)\n";


    out.close();
    delete[] buffer;

    return 0;
}


int dumpfunctions(Process& proc, int argc, const char* argv[]) {
    // cache reading to avoid writing empty file
    hashutils::ReadDefaultFile();

    const char* outFile;
    if (argc == 2) {
        outFile = "funcs.csv";
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
    out << "pool,func,minargs,maxargs,type,address\n";

    const size_t funcPoolSize = ARRAYSIZE(tool::dump::functionPool);
    const size_t maxSize = 1000;
    T8BuiltInFunc* buffer = new T8BuiltInFunc[maxSize];
    std::cout << std::dec << "reading: " << funcPoolSize << " pools\n";

    for (size_t i = 0; i < funcPoolSize; i++) {
        const auto& pool = functionPool[i];
        std::cout << std::hex << "reading pool " << pool.offset << "\n";
        if (pool.size > maxSize) {
            std::cerr << "bad pool size\n";
            continue;
        }
        if (!proc.ReadMemory(buffer, proc[pool.offset], sizeof(*buffer) * pool.size)) {
            std::cerr << "Can't read pool data for pool" << std::hex << pool.offset << "\n";
            continue;
        }
        for (size_t j = 0; j < pool.size; j++) {
            const auto& func = buffer[j];
            out << std::hex << (pool.instance ? "CSC" : "GSC") << "-" << (pool.methodPool ? "Method" : "Function") << "-" << pool.offset
                << std::dec
                << "," << hashutils::ExtractTmp("function", func.name)
                << "," << func.minArgs
                << "," << func.maxArgs
                << "," << func.type
                << ",";
            proc.WriteLocation(out, func.function) << "\n";
        }
    }

    out.close();
    delete[] buffer;

    return 0;
}

struct cmd_function_t {
    uintptr_t next; // cmd_function_t*
    uint64_t name;
    uint64_t pad0;
    uint64_t pad1;
    uint64_t pad2;
    uintptr_t function; // xcommand_t
};

int dumpcmdfunctions(Process& proc, int argc, const char* argv[]) {
    // cache reading to avoid writing empty file
    hashutils::ReadDefaultFile();

    const char* outFile;
    if (argc == 2) {
        outFile = "cfuncs.csv";
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
    out << "location,name,func\n";

    cmd_function_t func{};

    uintptr_t next = proc[offset::cmd_functions];

    // the first one is an header, I guess?
    if (!proc.ReadMemory(&func, next, sizeof(func))) {
        proc.WriteLocation(std::cerr << "Can't read next function at location ", next) << "\n";
    }

    next = func.next;

    int count = 0;
    do {
        proc.WriteLocation(out, next) << ",";

        if (!proc.ReadMemory(&func, next, sizeof(func))) {
            proc.WriteLocation(std::cerr << "Can't read next function at location ", next) << "\n";
            break;
        }
        next = func.next;

        out << hashutils::ExtractTmp("hash", func.name) << std::flush << ",";

        proc.WriteLocation(out, func.function) << "\n";

        count++;
    } while (next);

    out.close();

    std::cout << "Write " << std::dec << count << " functions into " << outFile << "\n";

    return 0;
}

int dumpsvcmdfunctions(Process& proc, int argc, const char* argv[]) {
    // cache reading to avoid writing empty file
    hashutils::ReadDefaultFile();

    const char* outFile;
    if (argc == 2) {
        outFile = "csfuncs.csv";
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
    out << "location,name,func\n";

    cmd_function_t func{};

    uintptr_t next = proc[0xF9F8DA0];

    // the first one is an header, I guess?
    if (!proc.ReadMemory(&func, next, sizeof(func))) {
        proc.WriteLocation(std::cerr << "Can't read next function at location ", next) << "\n";
    }

    next = func.next;

    int count = 0;
    do {
        proc.WriteLocation(out, next) << ",";

        if (!proc.ReadMemory(&func, next, sizeof(func))) {
            proc.WriteLocation(std::cerr << "Can't read next function at location ", next) << "\n";
            break;
        }
        next = func.next;

        out << hashutils::ExtractTmp("hash", func.name) << std::flush << ",";

        proc.WriteLocation(out, func.function) << "\n";

        count++;
    } while (next);

    out.close();

    std::cout << "Write " << std::dec << count << " functions into " << outFile << "\n";

    return 0;
}

ADD_TOOL("dps", " [output=pool.csv]", "dump pooled scripts", L"BlackOps4.exe", poolscripts);
ADD_TOOL("wps", " [output=scriptparsetree]", "write pooled scripts", L"BlackOps4.exe", writepoolscripts);
ADD_TOOL("dls", " [output=linked.csv]", "dump linked scripts", L"BlackOps4.exe", linkedscripts);
ADD_TOOL("dfunc", " [output=funcs.csv]", "dump functions", L"BlackOps4.exe", dumpfunctions);
ADD_TOOL("devents", " [output=events.csv]", "dump registered instance events", L"BlackOps4.exe", events);
ADD_TOOL("dcfunc", " [output=cfuncs.csv]", "dump cmd functions", L"BlackOps4.exe", dumpcmdfunctions);
ADD_TOOL("dscfunc", " [output=csfuncs.csv]", "dump sv cmd functions", L"BlackOps4.exe", dumpsvcmdfunctions);
