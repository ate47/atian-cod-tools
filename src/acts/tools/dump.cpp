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
}

ADD_TOOL("dps", " [output=pool.csv]", "dump pooled scripts", L"BlackOps4.exe", poolscripts);
ADD_TOOL("ddv", " [output=dvars.csv]", "dump dvars", L"BlackOps4.exe", dumpdvars);
ADD_TOOL("wps", " [output=scriptparsetree]", "write pooled scripts", L"BlackOps4.exe", writepoolscripts);
ADD_TOOL("dls", " [output=linked.csv]", "dump linked scripts", L"BlackOps4.exe", linkedscripts);
ADD_TOOL("dfunc", " [output=funcs.csv]", "dump functions", L"BlackOps4.exe", dumpfunctions);
ADD_TOOL("devents", " [output=events.csv]", "dump registered instance events", L"BlackOps4.exe", events);
ADD_TOOL("dcfunc", " [output=cfuncs.csv]", "dump cmd functions", L"BlackOps4.exe", dumpcmdfunctions);
ADD_TOOL("dscfunc", " [output=csfuncs.csv]", "dump sv cmd functions", L"BlackOps4.exe", dumpsvcmdfunctions);
ADD_TOOL("dfields", " [output=dfields.csv]", "dump class fields", L"BlackOps4.exe", dfields);
