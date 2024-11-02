#include <includes.hpp>
#include "tools/dump.hpp"
#include "tools/pool.hpp"
#include "tools/gsc.hpp"
#include "tools/cw/cw.hpp"
#include "tools/cw/poolt9.hpp"
#include <scriptinstance.hpp>
#include <pool.hpp>


namespace {

    struct XAssetPool {
        uintptr_t pool; // void*
        unsigned int itemSize;
        int itemCount;
        bool isSingleton;
        int itemAllocCount;
        void* freeHead;
    };
    static_assert(sizeof(XAssetPool) == 0x20);

    union __declspec(align(8)) stringtable_cell_value
    {
        byte bytes[8];
        const char* string_value;
        int64_t int_value;
        float float_value;
        byte bool_value;
        uint64_t hash_value;
        uintptr_t pointer_value;
    };
    enum stringtable_cell_type : byte {
        STC_TYPE_UNDEFINED = 0x0,
        STC_TYPE_STRING = 0x1,
        STC_TYPE_HASHED2 = 0x2,
        STC_TYPE_INT = 0x4,
        STC_TYPE_FLOAT = 0x5,
        STC_TYPE_BOOL = 0x6,
        STC_TYPE_HASHED7 = 0x7,
        STC_TYPE_HASHED8 = 0x8,
    };

    struct stringtable_cell {
        stringtable_cell_value value;
        uint32_t pad10;
        stringtable_cell_type type;
    };

    struct RawFileEntry {
        uintptr_t name; // 0x8
        uintptr_t size; // 0x18
        uintptr_t buffer; // 0x20
    };



    struct StringTable {
        uint64_t name;
        int columnCount;
        int rowCount;
        int cellcount;
        int unk14;
        uintptr_t values; // stringtable_cell* 
        uintptr_t unk20;
        uintptr_t unk28;
        uintptr_t unk30;
    };

    struct FuncInfo {
        const char* pool;
        uintptr_t offset;
        int size;
    };

    FuncInfo func_info_cw[] = {
        { "GSC-Function-d7b8d60", 0xD7B8D60, 0x8 },
        { "GSC-Function-deb19b0", 0xDEB19B0, 0x1C1 },
        { "GSC-Function-d7a3660", 0xD7A3660, 0x1AE },
        { "GSC-Function-d8136b0", 0xD8136B0, 0x4D },
        { "GSC-Function-d7c1270", 0xD7C1270, 0x99 },
        { "GSC-Function-d7d4d00", 0xD7D4D00, 0x4E },
        { "GSC-Function-d7fffc0", 0xD7FFFC0, 0x1A },
        { "GSC-Function-d7f1950", 0xD7F1950, 0xC },
        { "GSC-Function-d7d3590", 0xD7D3590, 0xD },
        { "GSC-Function-d7d85e0", 0xD7D85E0, 0x6 },
        { "GSC-Function-d7f1ad0", 0xD7F1AD0, 0x1 },
        { "GSC-Function-d7ed260", 0xD7ED260, 0x1 },
        { "GSC-Function-d76d7d0", 0xD76D7D0, 0x1 },
        { "CSC-Function-deae040", 0xDEAE040, 0x1AA },
        { "CSC-Function-d8136b0", 0xD8136B0, 0x4D },
        { "CSC-Function-d7c1270", 0xD7C1270, 0x99 },
        { "CSC-Function-d7d4d00", 0xD7D4D00, 0x4E },
        { "CSC-Function-d7fffc0", 0xD7FFFC0, 0x1A  },
        { "CSC-Function-dec5d10", 0xDEC5D10, 0x7A },
        { "CSC-Function-d7f1200", 0xD7F1200, 0x2C },
        { "CSC-Function-d7f37f0", 0xD7F37F0, 0xF },
        { "CSC-Function-d7d82e0", 0xD7D82E0, 0x10 },
        { "CSC-Function-d7d8800", 0xD7D8800, 0x24 },
        { "CSC-Function-d7d9410", 0xD7D9410, 0x28 },
        { "GSC-Method-d7a8ef0", 0xD7A8EF0, 0x23B },
        { "GSC-Method-d7b8ea0", 0xD7B8EA0, 0x2C },
        { "GSC-Method-d7b8620", 0xD7B8620, 0x3A },
        { "GSC-Method-d7ecd70", 0xD7ECD70, 0x15 },
        { "GSC-Method-d7b3a60", 0xD7B3A60, 0x94 },
        { "GSC-Method-d7ed580", 0xD7ED580, 0x4 },
        { "GSC-Method-d7bba50", 0xD7BBA50, 0x98 },
        { "GSC-Method-d7c26f0", 0xD7C26F0, 0xA },
        { "GSC-Method-d7afd00", 0xD7AFD00, 0x10E },
        { "GSC-Method-d7d3730", 0xD7D3730, 0x1 },
        { "GSC-Method-d7f1b60", 0xD7F1B60, 0x6 },
        { "GSC-Method-d7efd70", 0xD7EFD70, 0x15 },
        { "GSC-Method-deb52d0", 0xDEB52D0, 0x1B4 },
        { "CSC-Method-d79e840", 0xD79E840, 0x186 },
        { "CSC-Method-d7b9730", 0xD7B9730, 0x17 },
        { "CSC-Method-d7f1060", 0xD7F1060, 0xD },
        { "CSC-Method-d7d84e0", 0xD7D84E0, 0x4 },
        { "CSC-Method-d7d8c80", 0xD7D8C80, 0x3 },
        { "CSC-Method-d7d9940", 0xD7D9940, 0x13 },
    };
    struct SB_ObjectsArray {
        uint64_t sbObjectCount;
        uintptr_t sbObjects; // SB_Object
        uint64_t sbSubCount;
        uintptr_t sbSubs;
    };

    struct ScriptBundle {
        uint64_t hash;
        uint64_t unk8;
        SB_ObjectsArray sbObjectsArray;
    };

    int dumppoolcw(Process& proc, int argc, const char* argv[]) {

        const char* outFile;
        if (argc == 2) {
            outFile = "scriptparsetree_cw";
        }
        else {
            outFile = argv[2];
        }

        std::filesystem::create_directories(outFile);

        int gObjFileInfoCount[2];

        if (!proc.ReadMemory(gObjFileInfoCount, proc[0xF6F5FD0], sizeof(gObjFileInfoCount))) {
            std::cerr << "Can't read gObjFileInfoCount\n";
            return tool::BASIC_ERROR;
        }

        hashutils::ReadDefaultFile();

        auto elements = std::make_unique<tool::dump::T8ObjFileInfo[]>(800ull * 2 * sizeof(tool::dump::T8ObjFileInfo));

        if (!proc.ReadMemory(&elements[0], proc[0xF6EC9D0], sizeof(elements[0]) * 800 * 2)) {
            std::cerr << "Can't read gObjFileInfo\n";
            return tool::BASIC_ERROR;
        }

        tool::gsc::T9GSCOBJ headerTmp{};

        char namebuff[MAX_PATH + 10];
        std::cout << "dump using linked scripts\n";

        for (size_t i = 0; i < scriptinstance::SI_COUNT; i++) {
            for (size_t j = 0; j < gObjFileInfoCount[i]; j++) {
                auto& elem = elements[i * 800 + j];

                if (!proc.ReadMemory(&headerTmp, elem.activeVersion, sizeof(headerTmp))) {
                    std::cerr << "Can't read elem at " << j << "\n";
                    continue;
                }

                auto file = std::make_unique<byte[]>(headerTmp.file_size);

                if (!proc.ReadMemory(&file[0], elem.activeVersion, headerTmp.file_size)) {
                    std::cerr << "Can't read file elem at " << j << "\n";
                    continue;
                }

                sprintf_s(namebuff, "%s/script_%llx.gscc", outFile, headerTmp.name);

                if (utils::WriteFile(namebuff, &file[0], headerTmp.file_size)) {
                    std::cout << "- " << hashutils::ExtractTmpScript(headerTmp.name) << " -> " << namebuff << "\n";
                }
                else {
                    std::cerr << "Error when writting " << namebuff << "\n";
                }
            }
        }

        XAssetPool sptPool{};

        uintptr_t poolLoc = cw::ScanPool(proc);

        proc.WriteLocation(std::cout << "proc loc: ", poolLoc) << "\n";

        if (!proc.ReadMemory(&sptPool, poolLoc + sizeof(sptPool) * cw::ASSET_TYPE_SCRIPTPARSETREE, sizeof(sptPool))) {
            std::cerr << "Can't read SPT pool\n";
            return tool::BASIC_ERROR;
        }

        auto entries = std::make_unique<cw::ScriptParseTree[]>(sptPool.itemAllocCount);

        if (!proc.ReadMemory(&entries[0], sptPool.pool, sptPool.itemAllocCount * sizeof(entries[0]))) {
            std::cerr << "Can't read SPT pool entries\n";
            return tool::BASIC_ERROR;
        }

        std::cout << "dump using pool\n";

        for (size_t i = 0; i < sptPool.itemAllocCount; i++) {
            auto& e = entries[i];

            if (!proc.ReadMemory(&headerTmp, e.buffer, sizeof(headerTmp))) {
                continue;
            }

            auto file = std::make_unique<byte[]>(headerTmp.file_size);

            if (!proc.ReadMemory(&file[0], e.buffer, headerTmp.file_size)) {
                std::cerr << "Can't read file elem at " << i << "\n";
                continue;
            }

            sprintf_s(namebuff, "%s/script_%llx.gscc", outFile, headerTmp.name);

            if (utils::WriteFile(namebuff, &file[0], headerTmp.file_size)) {
                std::cout << "- " << hashutils::ExtractTmpScript(headerTmp.name) << " -> " << namebuff << "\n";
            }
            else {
                std::cerr << "Error when writting " << namebuff << "\n";
            }
        }


        return tool::OK;
    }


    const char* ReadTmpStr(const Process& proc, uintptr_t location) {
        static char tmp_buff[0x1000];

        if (proc.ReadString(tmp_buff, location, sizeof(tmp_buff)) < 0) {
            sprintf_s(tmp_buff, "<invalid:%llx>", location);
        }
        return tmp_buff;
    }

    int dpnamescw(Process& proc, int argc, const char* argv[]) {

        auto loc = proc[0xD7C8D90];

        XAssetPool pool{};
        char tmp_buff[0x50];

        int id = 0;

        uintptr_t poolLoc = cw::ScanPool(proc);

        std::cout << "id,name,itemSize,itemCount,itemAllocCount\n";

        while (true) {
            auto addr = proc.ReadMemory<uintptr_t>(loc + id * 8);
            if (!addr || proc.ReadString(tmp_buff, addr, sizeof(tmp_buff)) < 0) {
                break;
            }
            if (!proc.ReadMemory(&pool, poolLoc + sizeof(pool) * id, sizeof(pool))) {
                break;
            }

            std::cout << std::dec << id << "," << tmp_buff << "," << std::hex << pool.itemSize << "," << pool.itemCount << "," << pool.itemAllocCount << "\n";

            id++;
        }


        return tool::OK;
    }


#pragma region ddl_dump

    struct DDLDef {
        uint64_t name;
        uint64_t metatable; // ID64Metatable
        uintptr_t structList; // DDLStruct*
        uintptr_t enumList; // DDLEnum*
        uintptr_t next; // DDLDef*
        uint32_t unk38;
        uint32_t unk3c;
        uint32_t unk40;
        uint32_t unk44;
        uint32_t unk48;
        uint32_t unk4c;
        uint16_t version;
        uint16_t unk52;
        uint32_t unk54;
    };
    enum DDLType : byte
    {
        DDL_INVALID_TYPE = 0xFF,
        DDL_BYTE_TYPE = 0,
        DDL_SHORT_TYPE,
        DDL_UINT_TYPE,
        DDL_INT_TYPE,
        DDL_UINT64_TYPE,
        DDL_HASH_TYPE,
        DDL_FLOAT_TYPE,
        DDL_FIXEDPOINT_TYPE,
        DDL_STRING_TYPE,
        DDL_STRUCT_TYPE,
        DDL_ENUM_TYPE,
        DDL_PAD_TYPE,
    };

    const char* DdlTypeName(DDLType type, size_t intSize, size_t bitsize) {
        static char typeNameBuff[0x10];
        switch (type) {
        case DDL_BYTE_TYPE: return "byte";
        case DDL_SHORT_TYPE: return "short";
        case DDL_UINT_TYPE: {
            if (intSize == 1) return "bit";
            sprintf_s(typeNameBuff, "uint%lld", intSize);
            return typeNameBuff;
        }
        case DDL_INT_TYPE: {
            if (intSize == 1) return "bit";
            sprintf_s(typeNameBuff, "int%lld", intSize);
            return typeNameBuff;
        }
        case DDL_UINT64_TYPE: return "uint64";
        case DDL_FLOAT_TYPE: {
            if (intSize == 32) return "float";
            if (intSize == 64) return "double";

            sprintf_s(typeNameBuff, "float%lld", intSize);
            return typeNameBuff;
        }
        case DDL_FIXEDPOINT_TYPE: {
            sprintf_s(typeNameBuff, "fixedpoint%lld", bitsize);
            return typeNameBuff;
        }
        case DDL_HASH_TYPE: return "hash";
        case DDL_STRING_TYPE: return "char";
        case DDL_STRUCT_TYPE: return "struct";
        case DDL_ENUM_TYPE: return "enum";
        case DDL_PAD_TYPE: return "padbit";
        case DDL_INVALID_TYPE:
        default: return "<invalid>";
        }
    }


    struct DDLHashTable {
        uintptr_t list; //DDLHash*
        int count;
        int max;
    };
    struct DDLStruct {
        uint64_t name;
        uint32_t bitSize;
        uint32_t memberCount;
        uintptr_t members; // DDLMember* 
        DDLHashTable hashTableLower;
        DDLHashTable hashTableUpper;
    };

    struct __declspec(align(8)) DDLMember {
        uint64_t name;
        int32_t bitSize;
        uint32_t intSize;
        uint32_t offset;
        uint32_t maxIntValue;
        int16_t arraySize;
        int16_t externalIndex;
        int16_t unk24;
        DDLType type;
        bool isArray;
    };
    struct DDLHash {
        uint64_t hash;
        int index;
    };

    struct DDLEnum {
        uint64_t name;
        uintptr_t members; // Hash*
        int memberCount;
        DDLHashTable hashTable;
    };



    void ReadDDLStruct(Process& proc, std::ostream& defout, DDLDef& def, uintptr_t entry, std::unordered_set<uint64_t>& nextindexes) {
        DDLStruct stct{};
        if (!proc.ReadMemory(&stct, entry, sizeof(stct))) {
            defout << "<error reading struct entry>\n";
            return;
        }
        defout << "struct " << hashutils::ExtractTmp("hash", stct.name) << " {";

        auto members = std::make_unique<DDLMember[]>(stct.memberCount);

        if (!proc.ReadMemory(&members[0], stct.members, sizeof(members[0]) * stct.memberCount)) {
            defout << "<error reading members entry>\n";
        }
        else {
            // sort members because they don't match the internal structure (they match the hashmap)
            std::sort(&members[0], &members[stct.memberCount], [](const DDLMember& e1, const DDLMember& e2) {
                return e1.offset < e2.offset;
                });

            int64_t currentShift = 0;
            for (size_t i = 0; i < stct.memberCount; i++) {

                auto& mbm = members[i];
                utils::Padding(defout << "\n", 1);

                if (currentShift != mbm.offset) {
                    defout << "// invalid struct offset, missing ";
                    int64_t delta = (currentShift - (int64_t)mbm.offset);
                    if (delta >= 0) {
                        defout << "0x" << std::hex << delta;
                    }
                    else {
                        defout << "-0x" << std::hex << (-delta);
                    }
                    defout << " bits\n";
                    utils::Padding(defout, 1);
                }
                currentShift = mbm.offset + mbm.bitSize;

                //if (opt.flags & DDL_OFFSET) {
                utils::Padding(defout << "#offset 0x" << std::hex << currentShift << "\n", 1);
                //}

                bool addSize = false;
                if (mbm.type == DDL_STRUCT_TYPE) {
                    DDLStruct substct{};
                    if (!proc.ReadMemory(&substct, def.structList + mbm.externalIndex * sizeof(substct), sizeof(substct))) {
                        defout << "<error reading sub struct entry>\n";
                        return;
                    }
                    defout << hashutils::ExtractTmp("hash", substct.name);
                    nextindexes.insert(utils::CatLocated(0, mbm.externalIndex));
                }
                else if (mbm.type == DDL_ENUM_TYPE) {
                    DDLEnum subenum{};
                    if (!proc.ReadMemory(&subenum, def.enumList + mbm.externalIndex * sizeof(subenum), sizeof(subenum))) {
                        defout << "<error reading sub enum entry>\n";
                        return;
                    }
                    defout << hashutils::ExtractTmp("hash", subenum.name);
                    nextindexes.insert(utils::CatLocated(1, mbm.externalIndex));
                }
                else {
                    defout << DdlTypeName(mbm.type, mbm.intSize, mbm.bitSize);
                }

                defout << " " << hashutils::ExtractTmp("hash", mbm.name);

                if (mbm.isArray) {
                    defout << "[" << std::dec << mbm.arraySize << "]";
                }

                if (mbm.type == DDL_PAD_TYPE) {
                    defout << "[" << std::dec << mbm.bitSize << "]";
                }
                else  if (mbm.type == DDL_STRING_TYPE) {
                    auto bitSize = (mbm.bitSize / (mbm.isArray ? mbm.arraySize : 1));
                    defout << "[";
                    if (bitSize & 7) {
                        defout << std::dec << bitSize << "b"; // using non bytes for strings, wtf??
                    }
                    else {
                        defout << std::dec << (bitSize >> 3);
                    }
                    defout << "]";
                }

                defout << ";";
                defout << " // offset: 0x" << std::hex << mbm.offset << " + 0x" << mbm.bitSize << " = 0x" << currentShift;
            }
            defout << "\n";
            //if (opt.flags & DDL_OFFSET) {
            utils::Padding(defout, 1) << "#offset 0x" << std::hex << currentShift << "\n";
            //}
        }


        defout << "};\n\n";
    }


    void ReadDDLEnum(Process& proc, std::ostream& defout, uintptr_t entry) {
        DDLEnum enumst{};
        if (!proc.ReadMemory(&enumst, entry, sizeof(enumst))) {
            defout << "<error reading struct entry>\n";
            return;
        }
        defout << "enum " << hashutils::ExtractTmp("hash", enumst.name) << " {";

        auto members = std::make_unique<uint64_t[]>(enumst.memberCount);

        if (!proc.ReadMemory(&members[0], enumst.members, sizeof(members[0]) * enumst.memberCount)) {
            defout << "<error reading members entry>\n";
        }
        else {
            for (size_t i = 0; i < enumst.memberCount; i++) {
                auto& mbm = members[i];
                if (i) defout << ",";
                defout << "\n";
                utils::Padding(defout, 1) << "\"" << hashutils::ExtractTmp("hash", mbm) << "\" = 0x" << std::hex << i;
            }
            defout << "\n";
        }

        defout << "};\n\n";
    }
    void ReadDDLDefEntry(Process& proc, std::ostream& defout, uintptr_t entry) {
        if (!entry) {
            return;
        }

        DDLDef def{};
        if (!proc.ReadMemory(&def, entry, sizeof(def))) {
            defout << "<error reading entry>\n";
            return;
        }

        // GTS:
        // 142DD0405EFBF851
        // CA8192BAB9B812D
        // 3A8B1F6E71786EFF
        // 37A455F7364D8C91

        defout
            << "begin \"" << hashutils::ExtractTmp("hash", def.name) << "\";\n"
            << "version " << std::dec << def.version << ";\n"
            << "metatable \"" << hashutils::ExtractTmp("hash", def.metatable) << "\";\n"
            << "\n"
            ;

        if (def.structList) {
            std::unordered_set<uint64_t> nextIndexes{ { 0 } };
            std::unordered_set<uint64_t> doneIndexes{};
            do {
                uint64_t val{};

                for (auto id : nextIndexes) {
                    if (doneIndexes.find(id) == doneIndexes.end()) {
                        val = id;
                        break;
                    }
                }

                auto [type, idx] = utils::UnCatLocated(val);

                if (type == 0) {
                    ReadDDLStruct(proc, defout, def, def.structList + idx * sizeof(DDLStruct), nextIndexes);
                }
                else {
                    // READ ENUM
                    ReadDDLEnum(proc, defout, def.enumList + idx * sizeof(DDLEnum));
                }

                // add this id as parsed
                doneIndexes.insert(val);
            } while (doneIndexes.size() != nextIndexes.size());
        }
        else {
            defout << "\n";
        }
        defout << "\n";

        if (def.next) {
            defout << "/////////////////////////////////////////////////\n";
            ReadDDLDefEntry(proc, defout, def.next);
        }
    }

#pragma endregion


    int dfuncscw(Process& proc, int argc, const char* argv[]) {
        hashutils::ReadDefaultFile();
        std::ofstream out{ "funcs_cw.csv" };

        if (!out) {
            std::cerr << "Can't open output file\n";
            return tool::BASIC_ERROR;
        }

        out << "pool,func,minargs,maxargs,type,address,definition";

        struct BuiltinFunctionDef {
            uint32_t canonId;
            int min_args;
            int max_args;
            uintptr_t actionFunc;
            int type;
        };

        for (const auto& func : func_info_cw) {
            auto pool = std::make_unique<BuiltinFunctionDef[]>(func.size);

            if (!proc.ReadMemory(&pool[0], proc[func.offset], sizeof(pool[0]) * func.size)) {
                std::cerr << "Can't read pool " << std::hex << func.offset << "\n";
                continue;
            }

            for (size_t i = 0; i < func.size; i++) {
                out
                    << "\n"
                    << func.pool << ","
                    << hashutils::ExtractTmp("function", pool[i].canonId) << ","
                    << pool[i].min_args << ","
                    << pool[i].max_args << ","
                    << pool[i].type << ","
                    ;
                proc.WriteLocation(out, pool[i].actionFunc) << ",";
            }
        }

        out.close();

        std::cout << "done\n";

        return tool::OK;
    }

    int dcfuncscw(Process& proc, int argc, const char* argv[]) {
        hashutils::ReadDefaultFile();
        std::ofstream out{ "cfuncs_cw.csv" };

        if (!out) {
            std::cerr << "Can't open output file\n";
            return tool::BASIC_ERROR;
        }

        out << "location,name,func";

        struct cmd_function_t {
            uintptr_t next; // cmd_function_t*
            uint64_t name;
            uint64_t unk10;
            uint64_t unk18;
            uintptr_t function; // xcommand_t
            uint64_t unk28;
        };

        cmd_function_t buff{};

        buff.next = proc.ReadMemory<uintptr_t>(proc[0x120DC220]);

        while (buff.next) {
            proc.WriteLocation(out << "\n", buff.next) << "," << std::flush;
            if (!proc.ReadMemory(&buff, buff.next, sizeof(buff))) {
                std::cerr << "Error when reading next\n";
                break;
            }

            out << hashutils::ExtractTmp("hash", buff.name) << ",";
            proc.WriteLocation(out << "\n", buff.function);
        }

        out.close();

        std::cout << "done\n";

        return tool::OK;
    }

    class PoolOption {
    public:
        bool m_help = false;
        const char* m_output = "poolcw";
        const char* m_dump_hashmap = NULL;

        bool Compute(const char** args, INT startIndex, INT endIndex) {

            // default values
            for (size_t i = startIndex; i < endIndex; i++) {
                const char* arg = args[i];

                if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
                    m_help = true;
                }
                else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
                    if (i + 1 == endIndex) {
                        std::cerr << "Missing value for param: " << arg << "!\n";
                        return false;
                    }
                    m_output = args[++i];
                }
                else if (!strcmp("-m", arg) || !_strcmpi("--hashmap", arg)) {
                    if (i + 1 == endIndex) {
                        std::cerr << "Missing value for param: " << arg << "!\n";
                        return false;
                    }
                    m_dump_hashmap = args[++i];
                }
            }
            return true;
        }
        void PrintHelp(std::ostream& out) {
            out << "-h --help            : Print help\n"
                << "-o --output [d]      : Output dir\n"
                ;
        }
    };


    const char* ReadMTString(const Process& proc, uint32_t val) {
        static char str_read[0x2001];
        auto strptr = proc.ReadMemory<uintptr_t>(proc[0xF5EC9C8]) + (uint32_t)(0x10 * val);
        if (!strptr || !proc.ReadMemory<int16_t>(strptr) || proc.ReadMemory<byte>(strptr + 3) != 7) {
            return nullptr;
        }

        byte start = proc.ReadMemory<byte>(strptr + 0x18);

        if ((start & 0xC0) != 0x80) {
            // clear
            if (proc.ReadString(str_read, strptr + 0x18, sizeof(str_read)) < 0) {
                return nullptr;
            }
            return str_read;
        }

        // encrypted, assume it was decrypted by the dll
        if (proc.ReadString(str_read, strptr + 0x18 + 3, sizeof(str_read)) < 0) {
            return nullptr;
        }

        return str_read;
    }

    struct SB_Object {
        uint32_t unk0;
        uint64_t hash;
        uint32_t name;
        uint32_t fullKey;
        uint32_t type;
        union {
            int32_t intVal;
            float floatVal;
        } value;
    };


    void ReadSBName(const Process& proc, const SB_ObjectsArray& arr) {
        // ugly, but good enought to get the name
        if (!arr.sbObjectCount || arr.sbObjectCount > 10000 || arr.sbSubCount > 10000) {
            return; // bad read, wtf?
        }

        auto objects = std::make_unique<SB_Object[]>(arr.sbObjectCount + 1);

        if (!proc.ReadMemory(&objects[0], arr.sbObjects, sizeof(objects[0]) * arr.sbObjectCount)) {
            return;
        }

        static uint32_t nameHash = hash::HashT89Scr("name");

        for (size_t i = 0; i < arr.sbObjectCount; i++) {
            auto& obj = objects[i];

            if (obj.name != nameHash) {
                continue;
            }

            if (obj.fullKey) {
                // str?
                auto* strval = ReadMTString(proc, obj.fullKey);

                if (strval) {
                    hashutils::Add(strval); // add name to pool
                    continue;
                }
            }
            return;
        }

    }

    bool ReadSBObject(Process& proc, std::ostream& defout, int depth, const SB_ObjectsArray& arr, std::unordered_set<std::string>& strings) {

        if (!arr.sbObjectCount && !arr.sbSubCount) {
            defout << "{}";
            return true;
        }
        if (arr.sbObjectCount > 10000 || arr.sbSubCount > 10000) {
            return false; // bad read, wtf?
        }
        auto objects = std::make_unique<SB_Object[]>(arr.sbObjectCount + 1);

        if (!proc.ReadMemory(&objects[0], arr.sbObjects, sizeof(objects[0]) * arr.sbObjectCount)) {
            std::cerr << "Error when reading object at depth " << std::dec << depth << "\n";
            return false;
        }

        defout << "{";

        bool nofirst = false;

        std::unordered_set<uint32_t> keys{};

        if (arr.sbSubCount) {
            struct SB_Sub {
                uint32_t keyname;
                uint32_t unk4;
                uint32_t name;
                uint32_t size;
                uintptr_t item;
                uintptr_t struct_val; // SB_ObjectsArray*
            };
            auto subs = std::make_unique<SB_Sub[]>(arr.sbSubCount);

            if (!proc.ReadMemory(&subs[0], arr.sbSubs, sizeof(subs[0]) * arr.sbSubCount)) {
                std::cerr << "Error when reading object at depth " << std::dec << depth << "\n";
                return false;
            }

            for (size_t i = 0; i < arr.sbSubCount; i++) {
                auto& sub = subs[i];

                if (nofirst) {
                    defout << ",";
                }
                nofirst = true;
                utils::Padding(defout << "\n", depth + 1);

                keys.insert(sub.name);
                defout << "\"" << hashutils::ExtractTmp("var", sub.name) << "\": [";

                for (size_t j = 0; j < sub.item; j++) {
                    if (j) {
                        defout << ",";
                    }
                    utils::Padding(defout << "\n", depth + 2);
                    SB_ObjectsArray item{};

                    if (!proc.ReadMemory(&item, sub.struct_val + sizeof(item) * j, sizeof(item))
                        || !ReadSBObject(proc, defout, depth + 2, item, strings)) {
                        std::cerr << "Can't read array item\n";
                        return false;
                    }
                }
                if (sub.item) {
                    utils::Padding(defout << "\n", depth + 1) << "]";
                }
                else {
                    defout << "]";
                }

            }
        }

        if (arr.sbObjectCount) {
            for (size_t i = 0; i < arr.sbObjectCount; i++) {
                auto& obj = objects[i];

                // no idea why
                switch (obj.type) {
                case 2:
                case 26:
                    if (!obj.value.intVal) {
                        continue;
                    }
                case 3:
                    if (!obj.value.floatVal) {
                        continue;
                    }
                }

                if (keys.find(obj.name) != keys.end()) {
                    continue; // already computed
                }

                if (nofirst) {
                    defout << ",";
                }

                nofirst = true;
                utils::Padding(defout << "\n", depth + 1);

                defout << "\"" << hashutils::ExtractTmp("var", obj.name) << "\": ";

                switch (obj.type) {
                case 2:
                case 22:
                case 23:
                case 26: // int?
                    defout << std::dec << obj.value.intVal;
                    break;
                case 3: // float?
                    defout << obj.value.floatVal;
                    break;
                case 20:
                    // weapon
                    defout << "\"weapon#" << hashutils::ExtractTmp("hash", obj.hash) << "\"";
                    break;
                default:
                    if (obj.fullKey) {
                        // str?
                        auto* strval = ReadMTString(proc, obj.fullKey);

                        if (strval) {
                            strings.insert(strval);
                            defout << "\"" << strval << "\"";
                            continue;
                        }
                    }
                    else if (obj.hash & 0x7FFFFFFFFFFFFFFF) {
                        // hash?
                        defout << "\"#" << hashutils::ExtractTmp("hash", obj.hash) << "\"";
                        continue;
                    }
                    defout << "<unk:" << obj.type << "/" << std::hex << obj.value.intVal << ">";
                    break;
                }
            }
        }

        utils::Padding(defout << "\n", depth) << "}";

        return true;
    }
}

int cw::pool::pooltool(Process& proc, int argc, const char* argv[]) {
    using namespace pool;
    if (argc < 3) {
        return tool::BAD_USAGE;
    }
    PoolOption opt;

    if (!opt.Compute(argv, 2, argc) || opt.m_help) {
        opt.PrintHelp(std::cout);
        return tool::OK;
    }

    hashutils::SaveExtracted(opt.m_dump_hashmap != NULL);
    hashutils::ReadDefaultFile();

    std::error_code ec;
    std::filesystem::create_directories(opt.m_output, ec);

    auto id = std::atoi(argv[2]);

    std::cout << std::hex << "pool id: " << id << "\n";

    XAssetPool entry{};

    uintptr_t poolLoc = cw::ScanPool(proc);
    if (!proc.ReadMemory(&entry, poolLoc + sizeof(entry) * id, sizeof(entry))) {
        std::cerr << "Can't read pool entry\n";
        return tool::BASIC_ERROR;
    }

    char outputName[256];
    sprintf_s(outputName, "%s/pool_%x", opt.m_output, id);
    char dumpbuff[MAX_PATH + 10];

    std::cout << std::hex
        << "pool ........ " << entry.pool << "\n"
        << "free head ... " << entry.freeHead << "\n"
        << "item size ... " << entry.itemSize << "\n"
        << "count ....... " << entry.itemCount << "\n"
        << "alloc count . " << entry.itemAllocCount << "\n"
        << "singleton ... " << (entry.isSingleton ? "true" : "false") << "\n"
        ;

    switch (id) {
    case cw::ASSET_TYPE_STRINGTABLE: {

        auto stPool = std::make_unique<StringTable[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&stPool[0], entry.pool, sizeof(stPool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }

        size_t readFile = 0;
        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            const auto& e = stPool[i];

            const auto size = e.columnCount * e.rowCount;

            int test;
            if (!e.values || (size && !proc.ReadMemory(&test, e.values, sizeof(test)))) {
                continue; // check that we can read at least the cell
            }

            auto n = hashutils::ExtractPtr(e.name);

            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/%s", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << e.name << std::dec;
                sprintf_s(dumpbuff, "%s/hashed/stringtables/file_%llx.csv", opt.m_output, e.name);

            }

            std::cout << " (columns: " << e.columnCount << ", rows:" << e.rowCount << "/" << std::hex << (entry.pool + i * sizeof(entry)) << std::dec << ") into " << dumpbuff;


            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            if (!std::filesystem::exists(file, ec)) {
                readFile++;
                std::cout << " (new)";
            }
            std::cout << "\n";

            std::ofstream out{ file };

            if (!out) {
                std::cerr << "Can't open file " << file << "\n";
                continue;
            }

            auto cell = std::make_unique<stringtable_cell[]>(e.columnCount);

            //e.cells
            if (!(size)) {
                out.close();
                continue;
            }

            for (size_t i = 0; i < e.rowCount; i++) {
                if (!proc.ReadMemory(&cell[0], e.values + sizeof(cell[0]) * e.columnCount * i, sizeof(cell[0]) * e.columnCount)) {
                    std::cerr << "can't read cells for " << dumpbuff << "\n";
                    out.close();
                    continue;
                }
                for (size_t j = 0; j < e.columnCount; j++) {
                    switch (cell[j].type)
                    {
                    case STC_TYPE_UNDEFINED:
                        out << "undefined";
                        break;
                    case STC_TYPE_STRING:
                        out << ReadTmpStr(proc, cell[j].value.pointer_value);
                        break;
                    case STC_TYPE_INT:
                        out << cell[j].value.int_value;
                        break;
                    case STC_TYPE_FLOAT:
                        out << cell[j].value.float_value;
                        break;
                    case STC_TYPE_BOOL:
                        out << (cell[j].value.bool_value ? "true" : "false");
                        break;
                    case STC_TYPE_HASHED7:
                    case STC_TYPE_HASHED8:
                        //out << cell[j].type;
                    case STC_TYPE_HASHED2:
                        out << "#" << hashutils::ExtractTmp("hash", cell[j].value.hash_value);
                        break;
                    default:
                        //out << "unk type: " << cell[j].type;
                        out << "?" << std::hex
                            << cell[j].value.hash_value
                            //    << ':' << *reinterpret_cast<uint64_t*>(&cell[j].value[8])
                            //    << ':' << *reinterpret_cast<uint32_t*>(&cell[j].value[16])
                            << std::dec;
                        break;
                    }
                    if (j + 1 != e.columnCount) {
                        out << ",";
                    }
                }
                out << "\n";
            }
            out.close();
        }
        std::cout << "Dump " << readFile << " new file(s)\n";
        break;
    }
    case cw::ASSET_TYPE_SCRIPTBUNDLE: {
        auto pool = std::make_unique<ScriptBundle[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }


        std::filesystem::path progpath = utils::GetProgDir();
        std::filesystem::path dllfile = progpath / std::filesystem::path("acts-bocw-dll.dll");
        auto str = dllfile.string();

        std::cout << "dll location -> " << str << "\n";

        if (!proc.LoadDll(str.c_str())) {
            std::cerr << "Can't inject dll\n";
            return tool::BASIC_ERROR;
        }
        std::cout << "dll injected, decrypting MT Buffer...\n";

        auto& DLL_DecryptMTBufferFunc = proc["acts-bocw-dll.dll"]["DLL_DecryptMTBuffer"];

        if (!DLL_DecryptMTBufferFunc) {
            std::cerr << "Can't find DLL_DecryptMTBuffer export\n";
            return tool::BASIC_ERROR;
        }

        auto thr = proc.Exec(DLL_DecryptMTBufferFunc.m_location, 0x100000);

        if (!(thr == INVALID_HANDLE_VALUE || !thr)) {
            WaitForSingleObject(thr, INFINITE);
            CloseHandle(thr);
        }
        else {
            std::cerr << "Can't create decryption thread\n";
        }
        std::cout << "decrypted, dumping SB...\n";

        std::unordered_set<std::string> strings{};

        size_t readFile = 0;
        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            const auto& e = pool[i];


            if (e.hash < 0x1000000000000) {
                continue; // probably a ptr
            }
            ReadSBName(proc, e.sbObjectsArray);

            auto n = hashutils::ExtractPtr(e.hash);

            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/scriptbundle/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << e.hash << std::dec;
                sprintf_s(dumpbuff, "%s/scriptbundle/file_%llx.json", opt.m_output, e.hash);

            }

            std::cout << " into " << dumpbuff;


            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            if (!std::filesystem::exists(file, ec)) {
                readFile++;
                std::cout << " (new)";
            }
            std::cout << "\n";

            std::ofstream out{ file };

            if (!out) {
                std::cerr << "Can't open file " << file << "\n";
                continue;
            }

            ReadSBObject(proc, out, 0, e.sbObjectsArray, strings);

            out.close();
        }
        std::ofstream outStr{ std::format("{}/scriptbundle_str.txt", opt.m_output) };

        if (outStr) {
            for (const auto& st : strings) {
                outStr << st << "\n";
            }
            outStr.close();
        }

        std::cout << "Dump " << readFile << " new file(s)\n";
        break;
    }
    case cw::ASSET_TYPE_DDL: {
        struct DDLEntry {
            uint64_t name;
            uint64_t name2;
            uintptr_t ddlDef; // DDLDef*
            uint64_t pad[8];
        }; static_assert(sizeof(DDLEntry) == 0x58 && "bad DDLEntry size");



        auto pool = std::make_unique<DDLEntry[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];
        const size_t dumpbuffsize = sizeof(dumpbuff);
        std::vector<byte> read{};
        size_t readFile = 0;

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            const auto& p = pool[i];

            auto* n = hashutils::ExtractPtr(p.name);
            if (n) {
                sprintf_s(dumpbuff, "%s/%s", opt.m_output, n);
            }
            else {
                sprintf_s(dumpbuff, "%s/hashed/ddl/file_%llx.ddl", opt.m_output, p.name);
            }

            std::cout << "Writing DDL #" << std::dec << i << " -> " << dumpbuff << "\n";



            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::ofstream defout{ file };

            if (!defout) {
                std::cerr << "Can't open output file\n";
                continue;
            }

            ReadDDLDefEntry(proc, defout, p.ddlDef);

            defout.close();
        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    case cw::ASSET_TYPE_RAWFILE:
    case cw::ASSET_TYPE_RAWTEXTFILE:
    case cw::ASSET_TYPE_RAWFILEPREPROC: {
        auto pool = std::make_unique<RawFileEntry[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }

        size_t readFile = 0;
        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            const auto& e = pool[i];

            int test;
            if (!e.buffer || (e.size && !proc.ReadMemory(&test, e.buffer, sizeof(test)))) {
                continue; // check that we can read at least the data
            }

            auto n = hashutils::ExtractPtr(e.name);

            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/%s", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << e.name << std::dec;
                sprintf_s(dumpbuff, "%s/hashed/rawfile/file_%llx.raw", opt.m_output, e.name);

            }

            std::cout << " into " << dumpbuff;


            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            if (!std::filesystem::exists(file, ec)) {
                readFile++;
                std::cout << " (new)";
            }
            std::cout << "... ";

            if (!e.size) {
                // empty file
                if (!utils::WriteFile(file, "", 0)) {
                    std::cerr << "Can't write file\n";
                }
                std::cout << " empty / dumped\n";
                continue;
            }

            auto buff = std::make_unique<byte[]>(e.size + 0x10);

            if (!proc.ReadMemory(&buff[0], e.buffer, e.size + 0x10)) {
                std::cerr << "Can't read buffer\n";
                continue;
            }


            // decrypt
            byte* buffDecrypt{ &buff[0]};
            size_t size{ e.size };
            if (id != cw::ASSET_TYPE_RAWFILE) {
                buffDecrypt = cw::DecryptRawBuffer(buffDecrypt);
                size--;
            }



            if (!utils::WriteFile(file, buffDecrypt, size)) {
                std::cerr << "Can't write file\n";
                continue;
            }
            std::cout << " dumped\n";
        }
        std::cout << "Dump " << readFile << " new file(s)\n";
        break;
    }
    default: {
        std::cout << "Item data\n";

        auto raw = std::make_unique<byte[]>(entry.itemSize * entry.itemAllocCount);

        if (!proc.ReadMemory(&raw[0], entry.pool, entry.itemSize * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }

        char dumpbuff[MAX_PATH + 10];
        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            sprintf_s(dumpbuff, "%s/rawpool/%d/%lld.json", opt.m_output, (int)id, i);

            std::cout << "Element #" << std::dec << i << " -> " << dumpbuff << "\n";



            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::ofstream defout{ file };

            if (!defout) {
                std::cerr << "Can't open output file\n";
                continue;
            }

            tool::pool::WriteHex(defout, entry.pool + entry.itemSize * i, &raw[0] + (entry.itemSize * i), entry.itemSize, proc);

            defout.close();
        }

    }
    break;
    }

    return tool::OK;
}

namespace {
    int dbgcw(Process& proc, int argc, const char* argv[]) {
        if (argc < 4) {
            return tool::BAD_USAGE;
        }

        auto inst = std::strtoul(argv[2], nullptr, 10);
        auto name = hash::Hash64Pattern(argv[3]);

        if (inst > 1) {
            return tool::BAD_USAGE;
        }

        std::cout << "Searching " << std::hex << name << " in " << scriptinstance::Name(inst) << "\n";

        auto gScrVarGlob = proc[0xF7363A0];
        auto gScrVarPub = proc[0xF736180];

        auto globalVars = proc.ReadMemory<uintptr_t>(gScrVarPub + inst * 0x110 + 0x28);

        if (!globalVars) {
            std::cerr << "Can't find globalVars\n";
            return tool::BASIC_ERROR;
        }

        auto sharedStructs = proc.ReadMemory<uint32_t>(globalVars + 5 * 0xC + 4);

        if (!sharedStructs) {
            std::cerr << "Can't find sharedStructs\n";
            return tool::BASIC_ERROR;
        }

        auto scriptVar = proc.ReadMemory<uintptr_t>(gScrVarGlob + inst * 0x228 + 0x10);

        if (!scriptVar) {
            std::cerr << "Can't find scriptvar\n";
            return tool::BASIC_ERROR;
        }

        auto sharedStructElem = (0x22325 * sharedStructs + 0x1B3 * (((name << 32) >> 32) ^ (name >> 32))) & 0xFFFFF;

        auto res = proc.ReadMemory<uint32_t>(scriptVar + 4 * sharedStructElem);
        
        if (!res) {
            std::cerr << "Can't find struct\n";
            return tool::BASIC_ERROR;
        }



        return tool::OK;
    }

    int injectcw(Process& proc, int argc, const char* argv[]) {
        if (argc < 5) {
            return tool::BAD_USAGE;
        }

        const char* script = argv[2];
        const char* target = argv[3];
        const char* replaced = argv[4];

        std::string notify{};
        int ret = cw::InjectScriptCW(proc, script, target, replaced, notify);

        if (!notify.empty()) {
            if (ret) {
                LOG_ERROR("{}", notify);
            }
            else {
                LOG_INFO("{}", notify);
            }
        }

        return ret;
    }

    int injectcwalpha(Process& proc, int argc, const char* argv[]) {
        if (argc < 5) {
            return tool::BAD_USAGE;
        }

        const char* script = argv[2];
        const char* target = argv[3];
        const char* replaced = argv[4];

        std::string notify{};
        int ret = cw::InjectScriptCWAlpha(proc, script, target, replaced, notify);

        if (!notify.empty()) {
            if (ret) {
                LOG_ERROR("{}", notify);
            }
            else {
                LOG_INFO("{}", notify);
            }
        }

        return ret;
    }


    struct BGCache {
        uint64_t name;
        uint64_t pad08;
        uintptr_t def;
        uint32_t count;
    };

    struct BGCacheInfoDef {
        pool::BGCacheTypes type;
        uint64_t name;
        uint64_t pad10;
        uint64_t string_count;
    };

    struct BGCacheInfo {
        uintptr_t name;
        pool::XAssetType assetType;
        uint32_t allocItems;
        uintptr_t registerFunc;
        uintptr_t unregisterFunc;
        uint64_t hash;
        byte demoOnly;
        byte unk31;
        byte unk32;
        byte unk33;
        uint32_t defaultEntryIndex;
        uint32_t startIndex;
        uint32_t unk3c;
        uint32_t checksum;
        byte unk44;
        byte unk45;
        byte unk46;
        byte unk47;
    };
    struct BGPoolEntry {
        uint64_t name;
        uintptr_t assetHeader;
    };


    int dbgp(Process& proc, int argc, const char* argv[]) {
        BGCacheInfo info[40] = {};

        if (!proc.ReadMemory(&info[0], proc[0xDF1B4F0], sizeof(info))) {
            std::cerr << "Can't read cache\n";
            return tool::BASIC_ERROR;
        }


        std::filesystem::path out{ "bgpool/cw" };

        std::filesystem::create_directories(out);
        hashutils::ReadDefaultFile();

        std::ofstream outInfo{ out / "caches.csv" };

        if (!outInfo) {
            std::cerr << "Can't open caches.csv file\n";
            return tool::BASIC_ERROR;
        }


        outInfo << "id,name,start,count";

        auto pool = proc[0x11868C50];

        char nameInfo[200] = {};
        char fileInfo[200] = {};
        // buffer pool names
        for (size_t i = 0; i < pool::BG_CACHE_TYPE_COUNT; i++) {
            if (proc.ReadString(nameInfo, info[i].name, sizeof(nameInfo)) < 0) {
                std::cerr << "Can't read bgcache info names\n";
                break;
            }

            outInfo << "\n" << std::dec << i << "," << nameInfo << "," << info[i].startIndex << "," << info[i].allocItems;


            auto entries = std::make_unique<BGPoolEntry[]>(info[i].allocItems);

            if (!proc.ReadMemory(&entries[0], pool + sizeof(entries[0]) * info[i].startIndex, sizeof(entries[0]) * info[i].allocItems)) {
                std::cerr << "Can't read cache entries\n";
                break;
            }

            sprintf_s(fileInfo, "%s.csv", nameInfo);

            std::filesystem::path entriesPath = out / fileInfo;

            std::ofstream entriesFile{ entriesPath };

            if (!entriesFile) {
                std::cerr << "Can't open entries file\n";
                break;
            }

            entriesFile << "id,pool,name,ptr";

            size_t res = 0;

            for (size_t j = 0; j < info[i].allocItems; j++) {
                if (!entries[j].name) {
                    continue;
                }
                entriesFile << "\n" << std::dec << j << "," << nameInfo << "," << hashutils::ExtractTmp("hash", entries[j].name) << "," << std::hex << entries[j].assetHeader;
                res++;
            }

            entriesFile.close();
            std::cout << "write " << entriesPath.string() << " with " << std::dec << res << " entries\n";
        }
        outInfo.close();

        std::cout << "done into " << out.string() << "\n";

        return tool::OK;
    }

    using namespace cw::pool;
    ADD_TOOL(dpcw, "cw", " [input=pool_name] (output=pool_id)", "Black Ops Cold War dump pool", L"BlackOpsColdWar.exe", pooltool);
    ADD_TOOL(wpscw, "cw", "", "write pooled scripts (cw)", L"BlackOpsColdWar.exe", dumppoolcw);
    ADD_TOOL(dpncw, "cw", "", "dump pool names (cw)", L"BlackOpsColdWar.exe", dpnamescw);
    ADD_TOOL(dfuncscw, "cw", "", "dump function names (cw)", L"BlackOpsColdWar.exe", dfuncscw);
    ADD_TOOL(dcfuncscw, "cw", "", "dump cmd names (cw)", L"BlackOpsColdWar.exe", dcfuncscw);
    ADD_TOOL(dbgcw, "cw", " [inst]", "dbg (cw)", L"BlackOpsColdWar.exe", dbgcw);
    ADD_TOOL(injectcw, "cw", " (script) (target) (replace)", "inject script (cw)", L"BlackOpsColdWar.exe", injectcw);
    ADD_TOOL(injectcwalpha, "cw", " (script) (target) (replace)", "inject script (cw alpha)", L"COD2020.exe", injectcwalpha);
    ADD_TOOL(dbgpcw, "cw", "", "dump bg pool (cw)", L"BlackOpsColdWar.exe", dbgp);
}