#include <includes.hpp>
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include <pool.hpp>
#include "tools/pool.hpp"
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
            if (*reinterpret_cast<uint64_t*>(gsc.magic) != magicLong) {
                std::cerr << "Bad magic for location " << std::hex << ref.activeVersion << " " << *reinterpret_cast<uint64_t*>(gsc.magic) << " != " << magicLong << "\n";
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

namespace {

    int dfields(Process& proc, int argc, const char* argv[]) {
        // cache reading to avoid writing empty file
        hashutils::ReadDefaultFile();

        const char* outFile;
        if (argc == 2) {
            outFile = "dfields.csv";
        }
        else {
            outFile = argv[2];
        }
        typedef uint32_t ScrVarIndex_t;

        struct ScrVar_t  {
            uint64_t nameIndex;
            struct {
                unsigned __int32 nameType : 3;
                unsigned __int32 flags : 5;
                unsigned __int32 refCount : 24;
            };
            uint32_t nextSibling;
            uint32_t prevSibling;
            uint32_t parentId;
            uint32_t nameSearchHashList;
        }; static_assert(sizeof(ScrVar_t) == 0x20);

        union ScrVarObjectInfo1_t {
            uint64_t object_o;
            unsigned int size;
            ScrVarIndex_t nextEntId;
            ScrVarIndex_t self;
            ScrVarIndex_t free;
        };

        union ScrVarObjectInfo2_t {
            uint32_t object_w;
        };

        struct scrVarGlob_t {
            uintptr_t scriptNameSearchHashList; // ScrVarIndex_t*
            uintptr_t scriptVariables; // ScrVar_t*
            uintptr_t scriptVariablesObjectInfo1; // ScrVarObjectInfo1_t*
            uintptr_t scriptVariablesObjectInfo2; // ScrVarObjectInfo2_t*
            uintptr_t scriptValues; // ScrVarValue_t*
        }; static_assert(sizeof(scrVarGlob_t) == 0x28);

        struct ScrClassStruct_t {
            uint32_t id;
            uint32_t entArrayId;
            char charId;
            uintptr_t name; // const char*
            bool instancedPerLocalClient;
        }; static_assert(sizeof(ScrClassStruct_t) == 0x20);


        auto [vals, ok] = proc.ReadMemoryArray<uintptr_t>(proc[0x4EED230], 2);

        if (!ok) {
            LOG_ERROR("Can't read class map");
            return tool::BASIC_ERROR;
        }

        ScrClassStruct_t classMap[2][7]{};
        for (size_t i = 0; i < 2; i++) {
            if (vals[i] && !proc.ReadMemory(classMap[i], vals[i], sizeof(classMap[i]))) {
                LOG_ERROR("Can't read class map {}", i);
                return tool::BASIC_ERROR;
            }
        }

        auto [gScrVarGlob, ok2] = proc.ReadMemoryArray<scrVarGlob_t>(proc[0x4EED230], 2);

        if (!ok2) {
            LOG_ERROR("Can't read global variables");
            return tool::BASIC_ERROR;
        }

        std::ofstream out{ outFile, std::ios::out };

        if (!out) {
            LOG_ERROR("Can't open output file {}", outFile);
            return -1;
        }

        out << "vm,index,id,entArrayId,name,charId,instancedPerLocalClient,nameIndex" << std::flush;


        for (size_t i = 0; i < 2; i++) {
            std::unordered_map<uint32_t, ScrClassStruct_t*> structMap{};

            // build index to lookup ids
            for (size_t j = 0; j < ARRAYSIZE(classMap[i]); j++) {
                auto& cls = classMap[i][j];
                structMap[cls.id] = &cls;
                structMap[cls.entArrayId] = &cls;
            }

            auto printStart = [&structMap, &out, &proc, i](int32_t structId) -> bool {
                auto it = structMap.find(structId);

                ScrClassStruct_t* cls;
                if (it == structMap.end()) {
                    return false;
                }
                else {
                    cls = it->second;
                }

                out
                    << "\n"
                    << scriptinstance::Name(i) << ","
                    << std::dec << structId << ","
                    << cls->id << ","
                    << cls->entArrayId << ","
                    << proc.ReadStringTmp(cls->name) << ","
                    << cls->charId << ","
                    << (cls->instancedPerLocalClient ? "true" : "false")
                    << std::flush
                    ;
                return true;
            };

            constexpr size_t scriptNameSearchHashListLen = 0x80000;
            auto [scriptNameSearchHashList, ok3] = proc.ReadMemoryArray<ScrVarIndex_t>(gScrVarGlob[i].scriptNameSearchHashList, scriptNameSearchHashListLen);

            if (!ok3) {
                LOG_ERROR("Can't scriptNameSearchHashList for vm {}", i);
                continue;
            }
            
            ScrVar_t var{};
            for (size_t j = 0; j < scriptNameSearchHashListLen; j++) {
                ScrVarIndex_t h = scriptNameSearchHashList[j];
                while (h) {
                    // can be unused, so we check first

                    if (!proc.ReadMemory(&var, gScrVarGlob[i].scriptVariables + h * sizeof(var), sizeof(var)) || !printStart(var.parentId)) {
                        //LOG_ERROR("Can't hash var for 0x{:x}", h);
                        break;
                    }

                    out
                        << ","
                        << hashutils::ExtractTmp("var", var.nameIndex)
                        ;

                    h = var.nameSearchHashList;
                }
            }
        }

        out.close();

        LOG_INFO("Dump into {}", outFile);

        return tool::OK;
    }

    int dumpdvars(Process & proc, int argc, const char* argv[]) {
        hashutils::ReadDefaultFile();

        const char* outFile;
        if (argc == 2) {
            outFile = "dfields.csv";
        }
        else {
            outFile = argv[2];
        }

        int count = proc.ReadMemory<int>(proc[0xFA0EF8C]);
        auto [pool, ok] = proc.ReadMemoryArray<uintptr_t>(proc[0xFA0EFB0], count);

        if (!ok) {
            LOG_ERROR("Can't read pool");
            return tool::BASIC_ERROR;
        }

        std::ofstream out{ outFile, std::ios::out };

        if (!out) {
            LOG_ERROR("Can't open output file {}", outFile);
            return -1;
        }

        union DvarValue {
            bool enabled;
            int integer;
            uint32_t unsignedInt;
            int64_t integer64;
            uint64_t unsignedInt64;
            float value;
            float vector[4];
            const char* string;
            byte color[4];
            uintptr_t indirect[3]; // const dvar_t*
        };

        struct DvarData {
            DvarValue current;
            DvarValue latched;
            DvarValue reset;
            DvarValue unk48;
        };
        enum dvarType_t : __int32 {
            DVAR_TYPE_INVALID = 0x0,
            DVAR_TYPE_BOOL = 0x1,
            DVAR_TYPE_FLOAT = 0x2,
            DVAR_TYPE_FLOAT_2 = 0x3,
            DVAR_TYPE_FLOAT_3 = 0x4,
            DVAR_TYPE_FLOAT_4 = 0x5,
            DVAR_TYPE_INT = 0x6,
            DVAR_TYPE_ENUM = 0x7,
            DVAR_TYPE_STRING = 0x8,
            DVAR_TYPE_COLOR = 0x9,
            DVAR_TYPE_INT64 = 0xA,
            DVAR_TYPE_UINT64 = 0xB,
            DVAR_TYPE_LINEAR_COLOR_RGB = 0xC,
            DVAR_TYPE_COLOR_XYZ = 0xD,
            DVAR_TYPE_COLOR_LAB = 0xE,
            DVAR_TYPE_SESSIONMODE_BASE_DVAR = 0xF,
            DVAR_TYPE_COUNT = 0x10,
        };
        union DvarLimits {
            struct {
                int stringCount;
                const char** strings;
            } enumeration;
            struct {
                int min;
                int max;
            } integer;
            struct  {
                int64_t min;
                int64_t max;
            } integer64;
            struct {
                uint64_t min;
                uint64_t max;
            } unsignedInt64;
            struct {
                float min;
                float max;
            } value;
            struct {
                float min;
                float max;
            } vector;
        };

        struct dvar_t {
            tool::pool::XHash name;
            uintptr_t hashnext; // dvar_t*
            uintptr_t value; // DvarData*
            dvarType_t type;
            unsigned int flags;
            DvarLimits domain;
            uint64_t unk38;
        };

        dvar_t dvar{};
        DvarData value[4];

        out << "name,flags,type,domain,value" << std::flush;

        for (size_t i = 0; i < count; i++) {
            if (!proc.ReadMemory(&dvar, pool[i], sizeof(dvar)) || !proc.ReadMemory(&value[0], dvar.value, sizeof(value[0]))) {
                LOG_INFO("Can't read dvar at index {}", i);
                continue;
            }

            out
                << "\n"
                << hashutils::ExtractTmp("hash", dvar.name.name) << ","
                << "0x" << std::hex << dvar.flags << ","
                ;

            switch (dvar.type) {
            case DVAR_TYPE_BOOL: {
                out << "BOOL,,"
                    << value[0].current.enabled ? "true" : "false";
                break;
            }
            case DVAR_TYPE_FLOAT: {
                out << "FLOAT,"
                    << dvar.domain.value.min << ":" << dvar.domain.value.min << ","
                    << value[0].current.value;
                break;
            }
            case DVAR_TYPE_FLOAT_2: {
                out << "FLOAT_2,"
                    << dvar.domain.vector.min << ":" << dvar.domain.vector.min << ","
                    << value[0].current.vector[0] << " " << value[0].current.vector[1];
                break;
            }
            case DVAR_TYPE_FLOAT_3: {
                out << "FLOAT_3"
                    << dvar.domain.vector.min << ":" << dvar.domain.vector.min << ","
                    << value[0].current.vector[0] << " " << value[0].current.vector[1] << " " << value[0].current.vector[2];
                
                break;
            }
            case DVAR_TYPE_FLOAT_4: {
                out << "FLOAT_4"
                    << dvar.domain.vector.min << ":" << dvar.domain.vector.min << ","
                    << value[0].current.vector[0] 
                    << " " << value[0].current.vector[1] 
                    << " " << value[0].current.vector[2] 
                    << " " << value[0].current.vector[3];

                break;
            }
            case DVAR_TYPE_INT: {
                out << "INT"; 
                
                break;
            }
            case DVAR_TYPE_ENUM: {
                out << "ENUM"; 
                
                break;
            }
            case DVAR_TYPE_STRING: {
                out << "STRING"; 
                
                break;
            }
            case DVAR_TYPE_COLOR: {
                out << "COLOR"; 
                
                break;
            }
            case DVAR_TYPE_INT64: {
                out << "INT64"; 
                
                break;
            }
            case DVAR_TYPE_UINT64: {
                out << "UINT64"; 
                
                break;
            }
            case DVAR_TYPE_LINEAR_COLOR_RGB: {
                out << "LINEAR_COLOR_RGB"; 
                
                break;
            }
            case DVAR_TYPE_COLOR_XYZ: {
                out << "COLOR_XYZ"; 
                
                break;
            }
            case DVAR_TYPE_COLOR_LAB: {
                out << "COLOR_LAB"; 
                
                break;
            }
            case DVAR_TYPE_SESSIONMODE_BASE_DVAR: {
                out << "SESSIONMODE_BASE_DVAR"; 
                
                break;
            }
            default: out << "INVALID"; break;
            }
            out << ",";
        }

        out.close();

        return tool::OK;
    }
    const char* fieldTypeNames[]{
        "int",
        "short",
        "byte",
        "float",
        "lstring",
        "string",
        "hash",
        "vector",
        "entity",
        "enthandle",
        "actor",
        "sentient",
        "sentienthandle",
        "client",
        "pathnode",
        "actorgroup",
        "object",
        "xmodel_index",
        "xmodel",
        "bitflag",
        "bitflag64",
        "fx",
        "weapon",
        "rumble",
        "scriptbundle",
    };
    enum fieldtype_t : uint32_t {
        F_INT = 0x0,
        F_SHORT = 0x1,
        F_BYTE = 0x2,
        F_FLOAT = 0x3,
        F_LSTRING = 0x4,
        F_STRING = 0x5,
        F_HASH = 0x6,
        F_VECTOR = 0x7,
        F_ENTITY = 0x8,
        F_ENTHANDLE = 0x9,
        F_ACTOR = 0xA,
        F_SENTIENT = 0xB,
        F_SENTIENTHANDLE = 0xC,
        F_CLIENT = 0xD,
        F_PATHNODE = 0xE,
        F_ACTORGROUP = 0xF,
        F_OBJECT = 0x10,
        F_XMODEL_INDEX = 0x11,
        F_XMODEL = 0x12,
        F_BITFLAG = 0x13,
        F_BITFLAG64 = 0x14,
        F_FX = 0x15,
        F_WEAPON = 0x16,
        F_RUMBLE = 0x17,
        F_SCRIPTBUNDLE = 0x18,
        F_COUNT = 0x19
    };
    // ent_field_t fields[36]
    struct ent_field_t {
        uint32_t canonId;
        fieldtype_t type;
        bool isReadOnly;
        int32_t ofs;
        int32_t size;
        uintptr_t setter;
        uintptr_t getter;
    }; static_assert(0x28 == sizeof(ent_field_t));

    
    int wef(Process& proc, int argc, const char* argv[]) {
        hashutils::ReadDefaultFile();

        const char* outFile;
        if (argc == 2) {
            outFile = "efields.csv";
        }
        else {
            outFile = argv[2];
        }
        constexpr size_t count{ 36 };

        auto [fields, ok] = proc.ReadMemoryArray<ent_field_t>(proc[0x4991E80], count);

        if (!ok) {
            LOG_ERROR("Can't read fields");
            return tool::BASIC_ERROR;
        }

        std::ofstream out{ outFile };
        if (!out) {
            LOG_ERROR("Can't open {}", outFile);
            return tool::BASIC_ERROR;
        }

        out << "name,type,readonly,offset,size,setter,getter";

        for (size_t i = 0; i < count; i++) {
            const ent_field_t& field{ fields[i] };
            out
                << "\n"
                << hashutils::ExtractTmp("var", field.canonId) << ","
                << (field.type < F_COUNT ? fieldTypeNames[field.type] : "<error>") << ","
                << (field.isReadOnly ? "true" : "false") << ","
                << std::hex << "0x" << field.ofs << ","
                << std::hex << "0x" << field.size << ","
                ;
            proc.WriteLocation(out, field.setter) << ",";
            proc.WriteLocation(out, field.getter);
        }

        out.close();
        LOG_INFO("Write into {}", outFile);

        return tool::OK;
    }
    typedef float vec3_t[3];

    std::ostream& operator<<(std::ostream& out, vec3_t& vec) {
        return out << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
    }
    typedef float vec4_t[4];

    std::ostream& operator<<(std::ostream& out, vec4_t& vec) {
        return out << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ", " << vec[3] << ")";
    }

    int dcm(Process& proc, int argc, const char* argv[]) {
        hashutils::ReadDefaultFile();

        const char* outFile;
        if (argc == 2) {
            outFile = "gfxworld.json";
        }
        else {
            outFile = argv[2];
        }

        uintptr_t data = proc.ReadMemory<uintptr_t>(proc[0xBD6E868]);

        if (!data) {
            LOG_ERROR("Can't read memory pointer");
            return tool::BASIC_ERROR;
        }

        struct __declspec(align(8)) GfxWorld {
            tool::pool::XHash name;
            uintptr_t baseName; // const char*
            int32_t surfaceCount;
            uint64_t unk20;
            uint64_t unk28;
            uint64_t unk30;
            uint64_t unk38;
            uint64_t unk40;
            uint64_t unk48;
            uint64_t unk50;
            uint64_t unk58;
            uint64_t unk60;
            uint64_t unk68;
            uint64_t unk70;
            uint64_t unk78;
            uint64_t unk80;
            uint64_t unk88;
            uint64_t unk90;
            uint64_t unk98;
            uint64_t unka0;
            uint64_t unka8;
            uint64_t unkb0;
            uint64_t unkb8;
            uint64_t unkc0;
            uint64_t unkc8;
            uint64_t unkd0;
            uint64_t unkd8;
            uint64_t unke0;
            uint64_t unke8;
            uint64_t unkf0;
            uint64_t unkf8;
            uint64_t unk100;
            uint64_t unk108;
            uint64_t unk110;
            uint64_t unk118;
            uint64_t unk120;
            uint64_t unk128;
            uint64_t unk130;
            uint64_t unk138;
            uint64_t unk140;
            uint64_t unk148;
            uint64_t unk150;
            uint64_t unk158;
            uint64_t unk160;
            uint64_t unk168;
            uint64_t unk170;
            uint64_t unk178;
            uint64_t unk180;
            uint64_t unk188;
            uint64_t unk190;
            uint64_t unk198;
            uint64_t unk1a0;
            uint32_t checksum;
            uint32_t unk1ac;
            uint64_t unk1b0;
            uint32_t unk1b8;
            uint32_t static_model_count;
            uint64_t unk1c0;
            uint64_t unk1c8;
            uint64_t unk1d0;
            uint64_t unk1d8;
            uint64_t unk1e0;
            uint64_t unk1e8;
            uint64_t unk1f0;
            uint64_t unk1f8;
            uint64_t unk200;
            uint64_t unk208;
            uint64_t unk210;
            uint64_t unk218;
            uint64_t unk220;
            uint64_t unk228;
            uint64_t unk230;
            uint64_t unk238;
            uint64_t unk240;
            uint64_t unk248;
            uint64_t unk250;
            uint64_t unk258;
            uint64_t unk260;
            uint64_t unk268;
            uint64_t unk270;
            uint64_t unk278;
            uint64_t unk280;
            uint64_t unk288;
            uint64_t unk290;
            uint64_t unk298;
            uint64_t unk2a0;
            uint64_t unk2a8;
            uint64_t unk2b0;
            uint64_t unk2b8;
            uint64_t unk2c0;
            uint64_t unk2c8;
            uint64_t unk2d0;
            uint64_t unk2d8;
            uint64_t unk2e0;
            uint64_t unk2e8;
            uint64_t unk2f0;
            uint64_t unk2f8;
            uint64_t unk300;
            uint64_t unk308;
            uint64_t unk310;
            uint64_t unk318;
            uint64_t unk320;
            uint64_t unk328;
            uint64_t unk330;
            uint64_t unk338;
            uint64_t unk340;
            uint64_t unk348;
            uint64_t unk350;
            uint64_t unk358;
            uint64_t unk360;
            uint64_t unk368;
            uint64_t unk370;
            uint64_t unk378;
            uint64_t unk380;
            uint64_t unk388;
            uint64_t unk390;
            uint64_t unk398;
            uint64_t unk3a0;
            uint64_t unk3a8;
            uint64_t unk3b0;
            uint64_t unk3b8;
            uint64_t unk3c0;
            uint64_t unk3c8;
            uint64_t unk3d0;
            uint64_t unk3d8;
            uint64_t unk3e0;
            uint64_t unk3e8;
            uint64_t unk3f0;
            uint64_t unk3f8;
            uintptr_t static_model; // static_model_info*
            uint64_t unk408;
            uint64_t unk410;
            uint64_t unk418;
            uint64_t unk420;
            uint64_t unk428;
            uint64_t unk430;
            uint64_t unk438;
            uint64_t unk440;
            uint64_t unk448;
            uint64_t unk450;
            uint64_t unk458;
            uint64_t unk460;
            uint64_t unk468;
            uint64_t unk470;
            uint64_t unk478;
            uint64_t unk480;
            uint64_t unk488;
            uint64_t unk490;
            uint64_t unk498;
            uint64_t unk4a0;
            uint64_t unk4a8;
            uint64_t unk4b0;
            uint64_t unk4b8;
            uint64_t unk4c0;
            uint64_t unk4c8;
            uint64_t unk4d0;
            uint64_t unk4d8;
            uint64_t unk4e0;
            uint64_t unk4e8;
            uint64_t unk4f0;
            uint64_t unk4f8;
            uint64_t unk500;
            uint64_t unk508;
            uint64_t unk510;
            uint64_t unk518;
            uint64_t unk520;
            uint64_t unk528;
            uint64_t unk530;
            uint64_t unk538;
            uint64_t unk540;
            uint64_t unk548;
            uintptr_t unk550[4]; // GfxImage*
            uint64_t unk570;
            uint64_t unk578;
            uint64_t unk580;
            uint64_t unk588;
            uint32_t volumeDecalCount;
            uintptr_t volumeDecals; // GfxVolumeDecal*
            uint64_t unk5a0;
            uintptr_t volumeDecalRevealTexture; // GfxImage*
            uint64_t unk5b0;
            uint64_t unk5b8;
            uint64_t unk5c0;
            uint64_t unk5c8;
            uint64_t unk5d0;
            uint64_t unk5d8;
            uint64_t unk5e0;
            uint64_t unk5e8;
            uint64_t unk5f0;
            uint64_t unk5f8;
            uint64_t unk600;
            uint64_t unk608;
            uint64_t unk610;
            uint64_t unk618;
            uint64_t unk620;
            uint64_t unk628;
            uint64_t unk630;
            uint64_t unk638;
            uint64_t unk640;
            uint64_t unk648;
            uint64_t unk650;
            uint64_t unk658;
            uint64_t unk660;
            uint64_t unk668;
            uint64_t unk670;
            uint64_t unk678;
            uint64_t unk680;
            uint64_t unk688;
            uint64_t unk690;
            uint64_t unk698;
            uint64_t unk6a0;
            uint64_t unk6a8;
            uint64_t unk6b0;
            uint64_t unk6b8;
            uint64_t unk6c0;
            uint64_t unk6c8;
            uint64_t unk6d0;
            uint64_t unk6d8;
            uint64_t unk6e0;
            uint64_t unk6e8;
            uint64_t unk6f0;
            uint64_t unk6f8;
            uint64_t unk700;
            uint64_t unk708;
            uint64_t unk710;
            uint64_t unk718;
            uint64_t unk720;
            uint64_t unk728;
            uint64_t unk730;
            uint64_t unk738;
            uint64_t unk740;
            uint64_t unk748;
            uint64_t unk750;
            uint64_t unk758;
            uint64_t unk760;
            uint64_t unk768;
            uint64_t unk770;
            uint64_t unk778;
            uint64_t unk780;
            uint64_t unk788;
            uint64_t unk790;
            uint64_t unk798;
            uint64_t unk7a0;
            uint64_t unk7a8;
            uint64_t unk7b0;
            uint64_t unk7b8;
            uint64_t unk7c0;
            uint64_t unk7c8;
            uint64_t unk7d0;
            uint64_t unk7d8;
            uint64_t unk7e0;
            uint64_t unk7e8;
            uint64_t unk7f0;
            uint64_t unk7f8;
            uint64_t unk800;
            uint64_t unk808;
            uint64_t unk810;
            uint64_t unk818;
            uint64_t unk820;
            uint64_t unk828;
            uint64_t unk830;
            uint64_t unk838;
            uint64_t unk840;
            uint64_t unk848;
            uint64_t unk850;
            uint64_t unk858;
            uint64_t unk860;
            uint64_t unk868;
            uint64_t unk870;
            uint64_t unk878;
            uint64_t unk880;
            uint64_t unk888;
            uint64_t unk890;
            uint64_t unk898;
            uint64_t unk8a0;
            uint64_t unk8a8;
            uint64_t unk8b0;
            uint64_t unk8b8;
            uint64_t unk8c0;
            uint64_t unk8c8;
            uint64_t unk8d0;
            uint64_t unk8d8;
            uint64_t unk8e0;
            uint64_t unk8e8;
            uint64_t unk8f0;
            uint64_t unk8f8;
            uint64_t unk900;
            uint64_t unk908;
            uint64_t unk910;
            uint64_t unk918;
            uint64_t unk920;
            uint64_t unk928;
            uint64_t unk930;
            uint64_t unk938;
            uint64_t unk940;
            uint64_t unk948;
            uint64_t unk950;
            uint64_t unk958;
            uint64_t unk960;
            uint64_t unk968;
            uint64_t unk970;
            uint64_t unk978;
            uint64_t unk980;
            uint64_t unk988;
            uint64_t unk990;
            uint64_t unk998;
            uint64_t unk9a0;
            uint64_t unk9a8;
            uint64_t unk9b0;
            uint64_t unk9b8;
            uint64_t unk9c0;
            uint64_t unk9c8;
            uint64_t unk9d0;
            uint64_t unk9d8;
            uint64_t unk9e0;
            uint64_t unk9e8;
            uint64_t unk9f0;
            uint64_t unk9f8;
            uint64_t unka00;
            uint64_t unka08;
            uint64_t unka10;
            uint64_t unka18;
            uint64_t unka20;
            uint64_t unka28;
            uint64_t unka30;
            uint64_t unka38;
            uint64_t unka40;
            uint64_t unka48;
            uint64_t unka50;
            uint64_t unka58;
            uint64_t unka60;
            uint64_t unka68;
            uint64_t unka70;
            uint64_t unka78;
            uint64_t unka80;
            uint64_t unka88;
            uint64_t unka90;
            uint64_t unka98;
            uint64_t unkaa0;
            uint64_t unkaa8;
            uint64_t unkab0;
            uint64_t unkab8;
            uint64_t unkac0;
            uint64_t unkac8;
            uint64_t unkad0;
            uint64_t unkad8;
            uint64_t unkae0;
            uint64_t unkae8;
            uint64_t unkaf0;
            uint64_t unkaf8;
            uint64_t unkb00;
            uint64_t unkb08;
            uint64_t unkb10;
            uint64_t unkb18;
            uint64_t unkb20;
            uint64_t unkb28;
            uint64_t unkb30;
            uint64_t unkb38;
            uint64_t unkb40;
            uint64_t unkb48;
            uint64_t unkb50;
            uint64_t unkb58;
            uint64_t unkb60;
            uint64_t unkb68;
            uint64_t unkb70;
            uint64_t unkb78;
            uint64_t unkb80;
            uint64_t unkb88;
            uint64_t unkb90;
            uint64_t unkb98;
            uint64_t unkba0;
            uint64_t unkba8;
            uint64_t unkbb0;
            uint64_t unkbb8;
            uint64_t unkbc0;
            uint64_t unkbc8;
            uint64_t unkbd0;
            uint64_t unkbd8;
            uint64_t unkbe0;
            uint64_t unkbe8;
            uint64_t unkbf0;
            uint64_t unkbf8;
            uint64_t unkc00;
            uint64_t unkc08;
            uint64_t unkc10;
            uint64_t unkc18;
            uint64_t unkc20;
            uint64_t unkc28;
            uint64_t unkc30;
            uint64_t unkc38;
            uint64_t unkc40;
            uint64_t unkc48;
            uint64_t unkc50;
            uint64_t unkc58;
            uint64_t unkc60;
            uint64_t unkc68;
            uint64_t unkc70;
            uint64_t unkc78;
            uint64_t unkc80;
            uint64_t unkc88;
            uint64_t unkc90;
            uint64_t unkc98;
            uint64_t unkca0;
            uint64_t unkca8;
            uint64_t unkcb0;
            uint64_t unkcb8;
            uint64_t unkcc0;
            uint64_t unkcc8;
            uint64_t unkcd0;
            uint64_t unkcd8;
            uint64_t unkce0;
            uint64_t unkce8;
            uint64_t unkcf0;
            uint64_t unkcf8;
            uint64_t unkd00;
            uint64_t unkd08;
            uint64_t unkd10;
            uint64_t unkd18;
            uint64_t unkd20;
            uint64_t unkd28;
            uint64_t unkd30;
            uint64_t unkd38;
            uint64_t unkd40;
            uint64_t unkd48;
            uint64_t unkd50;
            uint64_t unkd58;
            uint64_t unkd60;
            uint64_t unkd68;
            uint64_t unkd70;
            uint64_t unkd78;
            uint64_t unkd80;
            uint64_t unkd88;
            uint64_t unkd90;
            uint64_t unkd98;
            uint64_t unkda0;
            uint64_t unkda8;
            uint64_t unkdb0;
            uint64_t unkdb8;
            uint64_t unkdc0;
            uint64_t unkdc8;
            uint64_t unkdd0;
            uint64_t unkdd8;
            uint64_t unkde0;
            uint64_t unkde8;
            uint64_t unkdf0;
            uint64_t unkdf8;
            uint64_t unke00;
            uint64_t unke08;
            uint64_t unke10;
            uint64_t unke18;
            uint64_t unke20;
            uint64_t unke28;
            uint64_t unke30;
            uint64_t unke38;
            uint64_t unke40;
            uint64_t unke48;
            uint64_t unke50;
            uint64_t unke58;
            uint64_t unke60;
            uint64_t unke68;
            uint64_t unke70;
            uint64_t unke78;
            uint64_t unke80;
            uint64_t unke88;
            uint64_t unke90;
            uint64_t unke98;
            uint64_t unkea0;
            uint64_t unkea8;
            uint64_t unkeb0;
            uint64_t unkeb8;
            uint64_t unkec0;
            uint64_t unkec8;
            uint64_t unked0;
            uint64_t unked8;
            uint64_t unkee0;
            uint64_t unkee8;
            uint64_t unkef0;
            uint64_t unkef8;
            uint64_t unkf00;
            uint64_t unkf08;
            uint64_t unkf10;
            uint64_t unkf18;
            uint64_t unkf20;
            uint64_t unkf28;
            uint64_t unkf30;
            uint64_t unkf38;
            uint64_t unkf40;
            uint64_t unkf48;
            uint64_t unkf50;
            uint64_t unkf58;
            uint64_t unkf60;
            uint64_t unkf68;
            uint64_t unkf70;
            uint64_t unkf78;
            uint64_t unkf80;
            uint64_t unkf88;
            uint64_t unkf90;
            uint64_t unkf98;
            uint64_t unkfa0;
            uint64_t unkfa8;
            uint64_t unkfb0;
            uint64_t unkfb8;
            uint64_t unkfc0;
            uint64_t unkfc8;
            uint64_t unkfd0;
            uint64_t unkfd8;
            uint64_t unkfe0;
            uint64_t unkfe8;
            uint64_t unkff0;
            uint64_t unkff8;
            uint64_t unk1000;
            uint64_t unk1008;
            uint64_t unk1010;
            uint64_t unk1018;
            uint64_t unk1020;
            uint64_t unk1028;
            uint64_t unk1030;
            uint64_t unk1038;
            uint64_t unk1040;
            uint64_t unk1048;
            uint64_t unk1050;
            uint64_t unk1058;
            uint64_t unk1060;
            uint64_t unk1068;
            uint64_t unk1070;
            uint64_t unk1078;
            uint64_t unk1080;
            uint64_t unk1088;
            uint64_t unk1090;
            uint64_t unk1098;
            uint64_t unk10a0;
            uint64_t unk10a8;
            uint64_t unk10b0;
            uint64_t unk10b8;
            uint64_t unk10c0;
            uint64_t unk10c8;
            uint64_t unk10d0;
            uint64_t unk10d8;
            uint64_t unk10e0;
            uint64_t unk10e8;
            uint64_t unk10f0;
            uint64_t unk10f8;
            uint64_t unk1100;
            uint64_t unk1108;
            uint64_t unk1110;
            uint64_t unk1118;
            uint64_t unk1120;
            uint64_t unk1128;
            uint64_t unk1130;
            uint64_t unk1138;
            uint64_t unk1140;
            uint64_t unk1148;
            uint64_t unk1150;
            uint64_t unk1158;
            uint64_t unk1160;
            uint64_t unk1168;
            uint64_t unk1170;
            uint64_t unk1178;
            uint64_t unk1180;
            uint64_t unk1188;
            uint64_t unk1190;
            uint64_t unk1198;
            uint64_t unk11a0;
            uint64_t unk11a8;
            uint64_t unk11b0;
            uint64_t unk11b8;
            uint64_t unk11c0;
            uint64_t unk11c8;
            uint64_t unk11d0;
            uint64_t unk11d8;
            uint64_t unk11e0;
            uint64_t unk11e8;
            uint64_t unk11f0;
            uint64_t unk11f8;
            uint64_t unk1200;
            uint64_t unk1208;
            uint64_t unk1210;
            uint64_t unk1218;
            uint64_t unk1220;
            uint64_t unk1228;
            uint64_t unk1230;
            uint64_t unk1238;
            uint64_t unk1240;
            uint64_t unk1248;
            uint64_t unk1250;
            uint64_t unk1258;
            uint64_t unk1260;
            uint64_t unk1268;
            uint64_t unk1270;
            uint64_t unk1278;
            uint64_t unk1280;
            uint64_t unk1288;
            uint64_t unk1290;
            uint64_t unk1298;
            uint64_t unk12a0;
            uint64_t unk12a8;
            uint64_t unk12b0;
            uint64_t unk12b8;
            uint64_t unk12c0;
            uint64_t unk12c8;
            uint64_t unk12d0;
            uint64_t unk12d8;
            uint64_t unk12e0;
            uint64_t unk12e8;
            uint64_t unk12f0;
            uint64_t unk12f8;
            uint64_t unk1300;
            uint64_t unk1308;
            uint64_t unk1310;
            uint64_t unk1318;
            uint64_t unk1320;
            uint64_t unk1328;
            uint64_t unk1330;
            uint64_t unk1338;
            uint64_t unk1340;
            uint64_t unk1348;
            uint64_t unk1350;
            uint64_t unk1358;
            uint64_t unk1360;
            uint64_t unk1368;
            uint64_t unk1370;
            uint64_t unk1378;
            uint64_t unk1380;
            uint64_t unk1388;
            uint64_t unk1390;
            uint64_t unk1398;
            uint64_t unk13a0;
            uint64_t unk13a8;
            uint64_t unk13b0;
            uint64_t unk13b8;
            uint64_t unk13c0;
            uint64_t unk13c8;
            uint64_t unk13d0;
            uint64_t unk13d8;
            uint64_t unk13e0;
            uint64_t unk13e8;
            uint64_t unk13f0;
            uint64_t unk13f8;
            uint64_t unk1400;
            uint64_t unk1408;
            uint64_t unk1410;
            uint64_t unk1418;
            uint64_t unk1420;
            uint64_t unk1428;
            uint64_t unk1430;
            uint64_t unk1438;
            uint64_t unk1440;
            uint64_t unk1448;
            uint64_t unk1450;
            uint64_t unk1458;
            uint64_t unk1460;
            uint64_t unk1468;
            uint64_t unk1470;
            uint64_t unk1478;
            uint64_t unk1480;
            uint64_t unk1488;
            uint64_t unk1490;
            uint64_t unk1498;
            uint64_t unk14a0;
            uint64_t unk14a8;
            uint64_t unk14b0;
            uint64_t unk14b8;
            uint64_t unk14c0;
            uint64_t unk14c8;
            uint64_t unk14d0;
            uint64_t unk14d8;
            uint64_t unk14e0;
            uint64_t unk14e8;
            uint64_t unk14f0;
            uint64_t unk14f8;
            uint64_t unk1500;
            uint64_t unk1508;
            uint64_t unk1510;
            uint64_t unk1518;
            uint64_t unk1520;
            uint64_t unk1528;
            uint64_t unk1530;
            uint64_t unk1538;
            uint64_t unk1540;
            uint64_t unk1548;
            uint64_t unk1550;
            uint64_t unk1558;
            uint64_t unk1560;
            uint64_t unk1568;
            uint64_t unk1570;
            uint64_t unk1578;
            uint64_t unk1580;
            uint64_t unk1588;
            uint64_t unk1590;
            uint64_t unk1598;
            uint64_t unk15a0;
            uint64_t unk15a8;
            uint64_t unk15b0;
            uint64_t unk15b8;
            uint64_t unk15c0;
            uint64_t unk15c8;
            uint64_t unk15d0;
            uint64_t unk15d8;
            uint64_t unk15e0;
            uint64_t unk15e8;
            uint64_t unk15f0;
            uint64_t unk15f8;
            uint64_t unk1600;
            uint64_t unk1608;
            uint64_t unk1610;
            uint64_t unk1618;
            uint64_t unk1620;
            uint64_t unk1628;
            uint64_t unk1630;
            uint64_t unk1638;
            uint64_t unk1640;
            uint64_t unk1648;
            uint64_t unk1650;
            uint64_t unk1658;
            uint64_t unk1660;
            uint64_t unk1668;
            uint64_t unk1670;
            uint64_t unk1678;
            uint64_t unk1680;
            uint64_t unk1688;
            uint64_t unk1690;
            uint64_t unk1698;
            uint64_t unk16a0;
            uint64_t unk16a8;
            uint64_t unk16b0;
            uint64_t unk16b8;
            uint64_t unk16c0;
            uint64_t unk16c8;
            uint64_t unk16d0;
            uint64_t unk16d8;
            uint64_t unk16e0;
            uint64_t unk16e8;
            uint64_t unk16f0;
            uint64_t unk16f8;
            uint64_t unk1700;
            uint64_t unk1708;
            uint64_t unk1710;
            uint64_t unk1718;
            uint64_t unk1720;
            uint64_t unk1728;
            uint64_t unk1730;
            uint64_t unk1738;
            uint64_t unk1740;
            uint64_t unk1748;
            uint64_t unk1750;
            uint64_t unk1758;
            uint64_t unk1760;
            uint64_t unk1768;
            uint64_t unk1770;
            uint64_t unk1778;
            uint64_t unk1780;
            uint64_t unk1788;
            uint64_t unk1790;
            uint64_t unk1798;
            uint64_t unk17a0;
            uint64_t unk17a8;
            uint64_t unk17b0;
            uint64_t unk17b8;
            uint64_t unk17c0;
            uint64_t unk17c8;
            uint64_t unk17d0;
            uint64_t unk17d8;
            uint64_t unk17e0;
            uint64_t unk17e8;
            uint64_t unk17f0;
            uintptr_t lighting; // GfxLighting*
            uintptr_t streamerworld; // StreamerWorld*
            uint64_t unk1808;
            uint64_t unk1810;
            uint64_t unk1818;
            uint64_t unk1820;
            uint64_t unk1828;
            uint64_t unk1830;
            uint64_t unk1838;
            uint64_t unk1840;
            uint64_t unk1848;
            uint64_t unk1850;
            uint64_t unk1858;
            uint64_t unk1860;
            uint64_t unk1868;
            uint64_t unk1870;
            uint64_t unk1878;
            uint64_t unk1880;
            uint64_t unk1888;
            uint64_t unk1890;
            uint64_t unk1898;
            uint64_t unk18a0;
            uint64_t unk18a8;
            uint64_t unk18b0;
            uint64_t unk18b8;
            uint64_t unk18c0;
            uint64_t unk18c8;
            uint64_t unk18d0;
            uint64_t unk18d8;
            uint64_t unk18e0;
            uint64_t unk18e8;
            uint64_t unk18f0;
            uint64_t unk18f8;
            uint64_t unk1900;
            uint64_t unk1908;
            uint64_t unk1910;
            uint64_t unk1918;
            uint64_t unk1920;
            uint64_t unk1928;
            uint64_t unk1930;
            uint64_t unk1938;
            uint64_t unk1940;
            uint64_t unk1948;
            uint64_t unk1950;
            uint64_t unk1958;
            uint64_t unk1960;
            uint64_t unk1968;
            uint64_t unk1970;
            uint64_t unk1978;
            uint64_t unk1980;
            uint64_t unk1988;
            uint64_t unk1990;
            uint64_t unk1998;
            uint64_t unk19a0;
            uint64_t unk19a8;
            uint64_t unk19b0;
            uint64_t unk19b8;
            uint64_t unk19c0;
            uint64_t unk19c8;
            uint64_t unk19d0;
            uint64_t unk19d8;
            uint64_t unk19e0;
            uint64_t unk19e8;
            uint64_t unk19f0;
            uint64_t unk19f8;
            uint64_t unk1a00;
            uint64_t unk1a08;
            uint64_t unk1a10;
            uint64_t unk1a18;
            uint64_t unk1a20;
            uint64_t unk1a28;
            uint64_t unk1a30;
            uint64_t unk1a38;
            uint64_t unk1a40;
            uint64_t unk1a48;
            uint64_t unk1a50;
            uint64_t unk1a58;
            uintptr_t unk1a60[10]; // GfxImage*
        };
        struct GfxVolumeDecal
        {
            uint64_t id;
            byte hidden;
            vec3_t localToWld[4];
            vec3_t wldToLocal[4];
            vec3_t halfExtents;
            vec3_t edgeFeather;
            vec4_t revealTextureUV;
            vec4_t uvBaseAndScale;
            vec4_t unka8;
            uintptr_t material; // Material*
            uintptr_t forwardMaterial; // Material*
            uint32_t unkc8;
            uint32_t targetName;
            uint64_t unkd0;
        }; static_assert(sizeof(GfxVolumeDecal) == 0xd8);

        struct static_model_info
        {
            uintptr_t model; // XModel*
            uint64_t unk8;
            uint64_t unk10;
            uintptr_t unk18;
            uint32_t unk20;
            uint32_t name;
            uint64_t unk28;
            uint32_t unk30;
            uint32_t unk34;
        };

        auto [info, ok] = proc.ReadMemoryObject<GfxWorld>(data); // s_world

        if (!ok) {
            LOG_ERROR("Can't read memory pointer data");
            return tool::BASIC_ERROR;
        }
        
        std::ofstream out{ outFile };
        if (!out) {
            LOG_ERROR("Can't open {}", outFile);
            return tool::BASIC_ERROR;
        }
        utils::CloseEnd outce{ [&out] { out.close(); } };

        tool::pool::WriteHex(out << "\n", data, (void*)&*info, sizeof(*info), proc);

        out << "{";
        utils::Padding(out << "\n", 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", info->name.name) << "\",";
        utils::Padding(out << "\n", 1) << "\"baseName\": \"" << proc.ReadStringTmp(info->baseName) << "\",";
        utils::Padding(out << "\n", 1) << "\"checksum\": " << std::dec << info->checksum << ",";
        //utils::Padding(out << "\n", 1) << "\"surfaceCount\": " << std::dec << info->surfaceCount << ",";

        if (info->lighting) {
            utils::Padding(out << "\n", 1) << "\"lighting\": \"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(info->lighting)) << "\",";
        }

        if (info->streamerworld) {
            utils::Padding(out << "\n", 1) << "\"streamerworld\": \"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(info->streamerworld)) << "\",";
        }
        
        for (size_t i = 0; i < ARRAYSIZE(info->unk550); i++) {
            if (info->unk550[i]) {
                utils::Padding(out << "\n", 1) << "\"lut" << i << "\": \"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(info->unk550[i] + 0x20)) << "\",";
            }
        }

        for (size_t i = 0; i < ARRAYSIZE(info->unk1a60); i++) {
            if (info->unk1a60[i]) {
                utils::Padding(out << "\n", 1) << "\"unk1a60_" << i << "\": \"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(info->unk1a60[i] + 0x20)) << "\",";
            }
        }

        if (info->volumeDecalRevealTexture) {
            utils::Padding(out << "\n", 1) << "\"volumeDecalRevealTexture\": \"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(info->volumeDecalRevealTexture + 0x20)) << "\",";
        }

        utils::Padding(out << "\n", 1) << "\"decals\": ";

        if (info->volumeDecalCount) {
            out << "[";


            auto [decals, ok2] = proc.ReadMemoryArray<GfxVolumeDecal>(info->volumeDecals, info->volumeDecalCount);

            if (!ok2) {
                LOG_ERROR("Can't read decal info");
                return tool::BASIC_ERROR;
            }

            for (size_t i = 0; i < info->volumeDecalCount; i++) {
                GfxVolumeDecal& decal = decals[i];

                if (i) {
                    out << ",";
                }
                utils::Padding(out << "\n", 2) << "{";
                utils::Padding(out << "\n", 3) << "\"id\": \"" << hashutils::ExtractTmp("hash", decal.id) << "\",";
                if (decal.targetName) {
                    const char* targetName = tool::pool::ReadMTString(proc, decal.targetName);
                    utils::Padding(out << "\n", 3) << "\"targetName\": \"" << (targetName ? targetName : utils::va("<invalid:%d>", decal.targetName)) << "\",";
                }
                if (decal.material) {
                    uint64_t material = proc.ReadMemory<uint64_t>(decal.material);
                    if (material) {
                        utils::Padding(out << "\n", 3) << "\"material\": \"#" << hashutils::ExtractTmp("hash", material) << "\",";
                    }
                }
                if (decal.forwardMaterial) {
                    uint64_t material = proc.ReadMemory<uint64_t>(decal.forwardMaterial);
                    if (material) {
                        utils::Padding(out << "\n", 3) << "\"forwardMaterial\": \"#" << hashutils::ExtractTmp("hash", material) << "\",";
                    }
                }

                utils::Padding(out << "\n", 3) << "\"localToWld\": [";
                for (size_t i = 0; i < 4; i++) {
                    if (i) out << ",";
                    utils::Padding(out << "\n", 4) << "\"" << decal.localToWld[i] << "\"";
                }
                utils::Padding(out << "\n", 3) << "],";
                utils::Padding(out << "\n", 3) << "\"wldToLocal\": [";
                for (size_t i = 0; i < 4; i++) {
                    if (i) out << ",";
                    utils::Padding(out << "\n", 4) << "\"" << decal.wldToLocal[i] << "\"";
                }
                utils::Padding(out << "\n", 3) << "],";

                utils::Padding(out << "\n", 3) << "\"halfExtents\": \"" << decal.halfExtents << "\",";
                utils::Padding(out << "\n", 3) << "\"edgeFeather\": \"" << decal.edgeFeather << "\",";
                utils::Padding(out << "\n", 3) << "\"revealTextureUV\": \"" << decal.revealTextureUV << "\",";
                utils::Padding(out << "\n", 3) << "\"uvBaseAndScale\": \"" << decal.uvBaseAndScale << "\",";
                utils::Padding(out << "\n", 3) << "\"unka8\": \"" << decal.unka8 << "\",";

                utils::Padding(out << "\n", 3) << "\"hidden\": " << (decal.hidden ? "true" : "false");
                //tool::pool::WriteHex(out << "\n", info->volumeDecals + sizeof(decal) * i, &decal, sizeof(decal), proc);
                utils::Padding(out << "\n", 2) << "}";
            }


            utils::Padding(out << "\n", 1) << "]";
        }
        else {
            out << "[]";
        }
        utils::Padding(out << ",\n", 1) << "\"static_models\": ";

        if (info->static_model_count) {
            out << "[";


            auto [static_models, ok2] = proc.ReadMemoryArray<static_model_info>(info->static_model, info->static_model_count);

            if (!ok2) {
                LOG_ERROR("Can't read decal info");
                return tool::BASIC_ERROR;
            }

            for (size_t i = 0; i < info->static_model_count; i++) {
                static_model_info& model = static_models[i];

                if (!model.model) continue;

                if (i) {
                    out << ",";
                }
                //utils::Padding(out << "\n", 2) << "{";
                utils::Padding(out << "\n", 2) << "\"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(model.model)) << "\"";
                //tool::pool::WriteHex(out << "\n", info->static_model + sizeof(model) * i, &model, sizeof(model), proc);
                //utils::Padding(out << "\n", 2) << "}";
            }

            utils::Padding(out << "\n", 1) << "]";
        }
        else {
            out << "[]";
        }
        out << "\n";

        out << "}\n";

        LOG_INFO("Dump into {}", outFile);


        return tool::OK;
    }

    int dstorage(Process& proc, int argc, const char* argv[]) {
        hashutils::ReadDefaultFile();

        const char* outFile;
        if (argc == 2) {
            outFile = "storage.json";
        }
        else {
            outFile = argv[2];
        }

        std::ofstream out{ outFile };
        if (!out) {
            LOG_ERROR("Can't open {}", outFile);
            return tool::BASIC_ERROR;
        }
        utils::CloseEnd outce{ [&out] { out.close(); } };

        struct unk_ddl_storage
        {
            uint64_t unk00;
            uint64_t unk08;
            uint64_t unk10;
            uint64_t unk18;
            uint64_t unk20;
        };
        auto [storages, ok] = proc.ReadMemoryArray<unk_ddl_storage>(proc[0xF8F2C30], 0x29);

        if (!ok) return tool::BASIC_ERROR;
        
        for (size_t i = 0; i < 0x29; i++) {
            auto& storage = storages[i];
            out << "#" << i << "\n";

            tool::pool::WriteHex(out << "\n", 0, (void*)&storage, sizeof(storage), proc);
        }
        LOG_INFO("Dump into {}", outFile);



        return tool::OK;
    }
}

ADD_TOOL("dps", "bo4", " [output=pool.csv]", "dump pooled scripts", L"BlackOps4.exe", poolscripts);
ADD_TOOL("ddv", "bo4", " [output=dvars.csv]", "dump dvars", L"BlackOps4.exe", dumpdvars);
ADD_TOOL("wpsbo4", "bo4", " [output=scriptparsetree]", "write pooled scripts", L"BlackOps4.exe", writepoolscripts);
ADD_TOOL("wef", "bo4", " [output=efields.csv]", "write ent fields", L"BlackOps4.exe", wef);
ADD_TOOL("dls", "bo4", " [output=linked.csv]", "dump linked scripts", L"BlackOps4.exe", linkedscripts);
ADD_TOOL("dfunc", "bo4", " [output=funcs.csv]", "dump functions", L"BlackOps4.exe", dumpfunctions);
ADD_TOOL("devents", "bo4", " [output=events.csv]", "dump registered instance events", L"BlackOps4.exe", events);
ADD_TOOL("dcfunc", "bo4", " [output=cfuncs.csv]", "dump cmd functions", L"BlackOps4.exe", dumpcmdfunctions);
ADD_TOOL("dscfunc", "bo4", " [output=csfuncs.csv]", "dump sv cmd functions", L"BlackOps4.exe", dumpsvcmdfunctions);
ADD_TOOL("dfields", "bo4", " [output=dfields.csv]", "dump class fields", L"BlackOps4.exe", dfields);
ADD_TOOL("dcm", "bo4", " [output=gfxworld.json]", "dump gfx world", L"BlackOps4.exe", dcm);
ADD_TOOL("dstorage", "bo4", " [output=storage.json]", "dump storage", L"BlackOps4.exe", dstorage);
