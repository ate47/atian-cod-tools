#include <includes.hpp>

using namespace tool::pool;

class PoolOption {
public:
    bool m_help = false;
    LPCCH m_output = "pool";
    LPCCH m_dump_hashmap = NULL;

    bool Compute(LPCCH* args, INT startIndex, INT endIndex) {

        // default values
        for (size_t i = startIndex; i < endIndex; i++) {
            LPCCH arg = args[i];

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

struct XAssetTypeInfo {
    uintptr_t name; // const char*
    uint64_t size;
    uint64_t unk20;
    uintptr_t GetName; // const char* (__cdecl*)(const XAssetHeader*);
    uintptr_t SetName; // void(__cdecl* )(XAssetHeader*, uint64_t*);
};

struct XAssetPoolEntry {
    uintptr_t pool;
    UINT32 itemSize;
    INT32 itemCount;
    BYTE isSingleton;
    INT32 itemAllocCount;
    uintptr_t freeHead;
};
struct TranslationEntry {
    uintptr_t data;
    uintptr_t name;
    UINT64 pad0;
};
struct RawFileEntry {
    uintptr_t name; // 0x8
    uintptr_t pad0; // 0x10
    uintptr_t size; // 0x18
    uintptr_t buffer; // 0x20
};
struct RawString {
    UINT64 name; // 0x8
    uintptr_t padding; // 0x10 0
    uintptr_t stringvalue; // 0x18 const char*
};
struct LuaFile {
    UINT64 name;
    UINT64 pad08;
    UINT32 size;
    uintptr_t buffer;
};
struct DDLEntry {
    UINT64 name; // 0x8
    uintptr_t unk8;  // 0x10
    uintptr_t buffer; // 0x18
    uintptr_t unk18; // 0x20
    uintptr_t unk20; // 0x28
    uintptr_t unk28; // 0x30
    uintptr_t unk30; // 0x38
    uintptr_t unk38; // 0x40
    uintptr_t unk40; // 0x48
    uintptr_t unk48; // 0x50
    uintptr_t unk50; // 0x58
};
struct BGCache {
    UINT64 name;
    UINT64 pad08;
    uintptr_t def;
    UINT32 count;
};

struct BGCacheInfoDef {
    pool::BGCacheTypes type;
    uint64_t name;
    uint64_t pad10;
    uint64_t unk18;
};

struct BGCacheInfo {
    uintptr_t name;
    pool::XAssetType assetType;
    uint32_t unkc;
    uintptr_t registerFunc;
    uintptr_t unregisterFunc;
    uint64_t hash;
    uint64_t hashnull;
    uint64_t unk30;
    uint32_t unk38;
    uint32_t unk3c;
    uint32_t checksum;
    byte unk44;
    byte unk45;
    byte unk46;
    byte unk47;
};
struct SB_ObjectsArray {
    uint64_t sbObjectCount;
    uintptr_t sbObjects; // SB_Object
    uint64_t sbSubCount;
    uintptr_t sbSubs;
};

struct ScriptBundle {
    uint64_t hash;
    uint64_t hashnull;
    uint32_t count_sub;
    uint32_t unk14;
    uint64_t unk18;
    SB_ObjectsArray sbObjectsArray;
};

struct GametypeEntry {
    uintptr_t v1; // 0x8
    uintptr_t pad0; // 0x10
    uintptr_t v3; // 0x18
    uintptr_t v4; // 0x20
    uintptr_t v5; // 0x28
};

enum StringTableCellType : INT {
    STC_TYPE_UNDEFINED = 0,
    STC_TYPE_STRING = 1,
    STC_TYPE_HASHED2 = 2,
    STC_TYPE_INT = 4,
    STC_TYPE_FLOAT = 5,
    STC_TYPE_BOOL = 6,
    STC_TYPE_HASHED7 = 7,
    STC_TYPE_HASHED8 = 8,
};


struct StringTableCell {
    BYTE value[20];
    StringTableCellType type;
};
// item size ... 40
struct StringTableEntry {
    UINT64 name; // 8
    int pad8; // 12
    int pad12; // 16
    int columnCount; // 20
    int rowCount; // 24
    int cellscount; // 28 empty?
    int unk24; // 32
    uintptr_t cells; // 40
    uintptr_t values; // 48 StringTableCell
    uintptr_t unk48; // 56
    uintptr_t unk56; // 64
};

inline bool HexValidString(LPCCH str) {
    if (!*str) {
        return false;
    }
    for (LPCCH s = str; *s; s++) {
        if (*s < ' ' || *s > '~') {
            return false;
        }
    }
    return true;
}

void tool::pool::WriteHex(std::ostream& out, uintptr_t base, BYTE* buff, SIZE_T size, const Process& proc) {
    CHAR strBuffer[101];
    for (size_t j = 0; j < size; j++) {
        if (j % 8 == 0) {
            if (base) {
                out << std::hex << std::setw(16) << std::setfill('0') << (base + j) << "~";
            }
            out << std::hex << std::setw(3) << std::setfill('0') << j << "|";
            if (j + 7 < size) {
                out << std::hex << std::setw(16) << std::setfill('0') << *reinterpret_cast<UINT64*>(&buff[j]);
            }
        }
        if (j - j % 8 + 7 >= size) {
            out << std::hex << std::setw(2) << std::setfill('0') << (int)buff[j];
        }
        if ((j + 1) % 8 == 0) {
            out << "|";

            for (size_t i = j - 7; i <= j; i++) {
                if (buff[i] >= ' ' && buff[i] <= '~') {
                    out << (char)buff[i];
                }
                else {
                    out << ".";
                }
            }

            // check x64 values
            if (j >= 7) {
                auto val = *reinterpret_cast<UINT64*>(&buff[j - 7]);
                if (val) {
                    // not null, hash?
                    auto* h = hashutils::ExtractPtr(val);
                    if (h) {
                        out << " #" << h;
                    }
                    else if (proc.ReadString(strBuffer, val, sizeof(strBuffer) - 1) >= 0 && HexValidString(strBuffer)) {
                        out << " ->" << strBuffer;
                    }
                    else if (proc.ReadMemory(strBuffer, val, sizeof(UINT64))) {
                        auto r = *reinterpret_cast<UINT64*>(strBuffer);

                        auto* h = hashutils::ExtractPtr(r);
                        if (h) {
                            out << " ->#" << h;
                        }
                        else {
                            out << " ->0x" << std::hex << r;
                        }
                    }
                }
            }
            out << "\n";
        }
    }
    out << "\n";
}

const char* ReadMTString(const Process& proc, uint32_t val) {
    static CHAR str_read[0x2001];
    auto strptr = proc.ReadMemory<uintptr_t>(proc[offset::mt_buffer]) + (UINT32)(0x14 * val);
    if (!proc.ReadMemory<INT16>(strptr) || proc.ReadMemory<BYTE>(strptr + 3) != 7) {
        return nullptr;
    }
    
    if (!strptr || proc.ReadString(str_read, strptr + 0x10, 0x2001) < 0) {
        return nullptr;
    }

    auto flag = proc.ReadMemory<CHAR>(strptr + 2);
    LPCCH strDec;
    if ((flag & 0x40) || flag >= 0) {
        // not encrypted
        strDec = str_read;
    }
    else {
        strDec = decrypt::DecryptString(str_read);
    }

    return strDec;
}

struct SB_Object {
    uint32_t unk0;
    uint32_t unk8;
    uint32_t kvpCount;
    uint64_t hash;
    uint64_t sbObjectCount;
    uint32_t name; // ScrString_t 
    uint32_t stringRef; // ScrString_t 
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

    static UINT32 nameHash = hash::Hash32("name");

    for (size_t i = 0; i < arr.sbObjectCount; i++) {
        auto& obj = objects[i];

        if (obj.name != nameHash) {
            continue;
        }

        if (obj.stringRef) {
            // str?
            auto* strval = ReadMTString(proc, obj.stringRef);

            if (strval) {
                hashutils::Add(strval); // add name to pool
                continue;
            }
        }
        return;
    }

}

bool ReadSBObject(const Process& proc, std::ostream& defout, int depth, const SB_ObjectsArray& arr, std::unordered_set<std::string>& strings) {

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

    std::unordered_set<UINT32> keys{};

    if (arr.sbSubCount) {
        struct SB_Sub {
            uint32_t keyname;
            uint32_t unk4;
            uint64_t size;
            uintptr_t item;
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

            auto* strval = ReadMTString(proc, sub.keyname);

            if (!strval) {
                std::cerr << "Can't read array key\n";
                return false;
            }
            keys.insert(hash::Hash32(strval));
            defout << "\"" << strval << "\": [";

            for (size_t j = 0; j < sub.size; j++) {
                if (j) {
                    defout << ",";
                }
                utils::Padding(defout << "\n", depth + 2);
                SB_ObjectsArray item{};

                if (!proc.ReadMemory(&item, sub.item + sizeof(item) * j, sizeof(item))
                    || !ReadSBObject(proc, defout, depth + 2, item, strings)) {
                    std::cerr << "Can't read array item\n";
                    return false;
                }
            }
            if (sub.size) {
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
            case 25:
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
            case 25:
                defout << std::dec << obj.value.intVal;
                break;
            case 3: // float?
                defout << obj.value.floatVal;
                break;
            case 20:
                // TODO
                defout << "<error reading:" << hashutils::ExtractTmp("hash", obj.hash) << ">";
                break;
            default:
                if (obj.stringRef) {
                    // str?
                    auto* strval = ReadMTString(proc, obj.stringRef);

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
                defout << "<unk:" << obj.type << ">";
                break;
            }
        }
    }

    utils::Padding(defout << "\n", depth) << "}";

    return true;
}

int pooltool(const Process& proc, int argc, const char* argv[]) {
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

    std::error_code ec;
    std::filesystem::create_directories(opt.m_output, ec);

    CHAR outputName[256];
    if (!_strcmpi(argv[2], "all")) {
        sprintf_s(outputName, "%s/xassetpools.csv", opt.m_output);
        std::ofstream out{ outputName, std::ios::out };

        if (!out) {
            std::cerr << "Can't open output file '" << outputName << "'\n";
            return -1;
        }

        out << "id,name,location,itemsize,items,max,singleton,setname,getname\n";

        const auto count = 0xa8;
        XAssetPoolEntry entry[count];
        XAssetTypeInfo entryinfo[count];

        if (!proc.ReadMemory(&entry, proc[offset::assetPool], sizeof(*entry) * count)) {
            std::cerr << "Can't read pool entry\n";
            return tool::BASIC_ERROR;
        }
        if (!proc.ReadMemory(&entryinfo, proc[offset::s_XAssetTypeInfo], sizeof(*entryinfo) * count)) {
            std::cerr << "Can't read xasset info\n";
            return tool::BASIC_ERROR;
        }

        CHAR str[100];
        for (size_t i = 0; i < count; i++) {
            if (proc.ReadString(str, entryinfo[i].name, sizeof(str) - 1) < 0) {
                std::cerr << "Can't read xasset name from " << std::hex << entryinfo[i].name << "\n";
                continue;
            }
            auto e = entry[i];
            out << std::dec
                << i << std::hex << ","
                << str << ","
                << e.pool << ","
                << e.itemSize << ","
                << e.itemAllocCount << ","
                << e.itemCount << ","
                << (e.isSingleton ? "true" : "false") << ",";

            proc.WriteLocation(out, entryinfo[i].SetName) << ",";
            proc.WriteLocation(out, entryinfo[i].GetName) << "\n";
        }
        out.close();

        return tool::OK;
    }

    auto id = std::atoi(argv[2]);

    std::cout << std::hex << "pool id: " << id << "\n";

    XAssetPoolEntry entry{};

    if (!proc.ReadMemory(&entry, proc[offset::assetPool] + sizeof(entry) * id, sizeof(entry))) {
        std::cerr << "Can't read pool entry\n";
        return tool::BASIC_ERROR;
    }

    sprintf_s(outputName, "%s/pool_%x", opt.m_output, id);

    std::cout << std::hex
        << "pool ........ " << entry.pool << "\n"
        << "free head ... " << entry.freeHead << "\n"
        << "item size ... " << entry.itemSize << "\n"
        << "count ....... " << entry.itemCount << "\n"
        << "alloc count . " << entry.itemAllocCount << "\n"
        << "singleton ... " << (entry.isSingleton ? "true" : "false") << "\n"
        ;

    switch (id) {
    case ASSET_TYPE_LOCALIZE_ENTRY:
    {
        hashutils::ReadDefaultFile();

        std::ofstream out{ outputName, std::ios::out };

        if (!out) {
            std::cerr << "Can't open output file '" << outputName << "'\n";
            return -1;
        }

        TranslationEntry* raw = new TranslationEntry[entry.itemAllocCount];

        if (!proc.ReadMemory(raw, entry.pool, sizeof(*raw) * entry.itemAllocCount)) {
            delete[] raw;
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }


        CHAR str[4096];
        out << "key,translation\n";
        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            if (proc.ReadString(str, raw[i].data, 4096) < 0) {
                std::cerr << "Can't read translation " << i << "\n";
            }
            
            out << hashutils::ExtractTmp("hash", raw[i].name) << "," << decrypt::DecryptString(str) << "\n";
        }
        out.close();
        delete[] raw;
    }
    break;
    case ASSET_TYPE_STRINGTABLE:
    {
        hashutils::ReadDefaultFile();

        auto pool = std::make_unique<StringTableEntry[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }

        CHAR dumpbuff[MAX_PATH + 10];
        CHAR namebuf[2000];
        StringTableCell cell[200];
        const size_t cellsize = sizeof(cell) / sizeof(cell[0]);

        size_t readFile = 0;

        for (size_t i = 0; i < min(entry.itemAllocCount, entry.itemCount); i++) {
            const auto& e = pool[i];

            const auto size = e.columnCount * e.rowCount;

            if (!e.values || (size && !proc.ReadMemory(&cell, e.values, sizeof(cell[0])))) {
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

            //e.cells
            if (!(size)) {
                out.close();
                continue;
            }

            for (size_t i = 0; i < e.rowCount; i++) {
                if (!proc.ReadMemory(&cell[0], e.values + sizeof(cell[0]) * e.columnCount * i, sizeof(cell[0]) * min(cellsize, e.columnCount))) {
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
                        if (proc.ReadString(namebuf, *reinterpret_cast<uintptr_t*>(&cell[j].value[0]), sizeof(namebuf)) < 0) {
                            out << "<bad_str>";
                        }
                        else {
                            out << namebuf;
                        }
                        break;
                    case STC_TYPE_INT:
                        out << *reinterpret_cast<INT64*>(&cell[j].value[0]);
                        break;
                    case STC_TYPE_FLOAT:
                        out << *reinterpret_cast<FLOAT*>(&cell[j].value[0]);
                        break;
                    case STC_TYPE_BOOL: 
                        out << (cell[j].value[0] ? "true" : "false");
                        break;
                    case STC_TYPE_HASHED7:
                    case STC_TYPE_HASHED8:
                        //out << cell[j].type;
                    case STC_TYPE_HASHED2:
                        out << "#" << hashutils::ExtractTmp("hash", *reinterpret_cast<UINT64*>(&cell[j].value[0]));
                        break;
                    default:
                        //out << "unk type: " << cell[j].type;
                        out << "?" << std::hex
                            << *reinterpret_cast<UINT64*>(&cell[j].value[0])
                        //    << ':' << *reinterpret_cast<UINT64*>(&cell[j].value[8])
                        //    << ':' << *reinterpret_cast<UINT32*>(&cell[j].value[16])
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
    }
    break;
    case ASSET_TYPE_RAWSTRING:
    {
        hashutils::ReadDefaultFile();

        sprintf_s(outputName, "%s/strings.csv", opt.m_output);

        std::filesystem::path file(outputName);
        std::filesystem::create_directories(file.parent_path(), ec);

        std::ofstream out{ file };

        if (!out) {
            std::cerr << "Can't open file " << file << "\n";
            return tool::BASIC_ERROR;
        }


        auto pool = std::make_unique<RawString[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        CHAR str[4096];
        out << "name,string\n";
        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            const auto& p = pool[i];
            const auto* name = hashutils::ExtractTmp("hash", p.name);
            if (proc.ReadString(str, p.stringvalue, sizeof(str) - 1) < 0) {
                std::cerr << "error when reading " << std::dec << i << " (" << name << ")" << " at " << p.stringvalue << "\n";
                continue;
            }

            out << "#" << name << "," << str << "\n";
        }

        std::cout << "Dump into " << file << "\n";
    }
    break;
    case ASSET_TYPE_LUAFILE:
    {
        hashutils::ReadDefaultFile();

        auto pool = std::make_unique<LuaFile[]>(entry.itemAllocCount);

        sprintf_s(outputName, "%s/luapool", opt.m_output);

        std::filesystem::path outDir{ outputName };

        std::filesystem::create_directories(outDir);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            const auto& p = pool[i];

            auto name = hashutils::ExtractPtr(p.name);

            std::filesystem::path outFile;
            if (name) {
                outFile = outDir / name;
                std::filesystem::create_directories(outFile.parent_path());
            }
            else {
                outFile = outDir / std::format("hashed/{:x}.lua", p.name);
            }
            {
                auto buffer = std::make_unique<BYTE[]>(p.size);

                if (!proc.ReadMemory(&buffer[0], p.buffer, p.size)) {
                    std::cerr << "Can't read buffer for " << outFile.string() << "\n";
                    continue;
                }

                utils::WriteFile(outFile, &buffer[0], p.size);
                std::cout << "Dumped " << outFile.string() << "\n";
            }
        }
    }
        break;
    case ASSET_TYPE_RAWFILE:
    {

        hashutils::ReadDefaultFile();


        auto pool = std::make_unique<RawFileEntry[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        CHAR dumpbuff[MAX_PATH + 10];
        std::vector<BYTE> read{};
        size_t readFile = 0;

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            const auto& p = pool[i];

            auto n = hashutils::ExtractPtr(p.name);

            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/%s", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name << std::dec;
                sprintf_s(dumpbuff, "%s/hashed/rawfile/file_%llx.raw", opt.m_output, p.name);
            }

            if (!p.buffer || !proc.ReadMemory<UINT64>(p.buffer)) {
                std::cerr << "error when reading buffer at " << p.buffer << "\n";
                continue;
            }

            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::cout << "->" << file;

            if (!std::filesystem::exists(file, ec)) {
                readFile++;
                std::cout << " (new)";
            }
            std::cout << "\n";

            LPCVOID output;
            size_t outputsize;

            // empty file
            if (!p.size) {
                output = "";
                outputsize = 0;
            }
            else {
                read.resize(p.size);
                std::fill(read.begin(), read.end(), 0);

                if (!proc.ReadMemory(&read[0], p.buffer, p.size)) {
                    std::cerr << "error when reading buffer at " << p.buffer << "\n";
                    continue;
                }
                output = &read[0];
                outputsize = p.size;
            }


            if (!utils::WriteFile(file, output, outputsize)) {
                std::cerr << "error when writting file " << file << "\n";
            }

        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
        break;
    case ASSET_TYPE_DDL:
    {

        hashutils::ReadDefaultFile();
        

        auto pool = std::make_unique<DDLEntry[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        CHAR dumpbuff[MAX_PATH + 10];
        const size_t dumpbuffsize = sizeof(dumpbuff);
        std::vector<BYTE> read{};
        size_t readFile = 0;
        BYTE test[0x100];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            const auto& p = pool[i];


            auto n = hashutils::ExtractPtr(p.name);
            std::cout << "- " << std::dec << i << " : " << hashutils::ExtractTmp("file", p.name) << ": " << std::hex << p.buffer << "\n";

            // WriteHex(std::cout, entry.pool + i * sizeof(pool[0]), (BYTE*)& p, sizeof(p), proc); // padding

            if (!proc.ReadMemory(test, p.buffer, sizeof(test))) {
                std::cerr << "Bad read\n";
                continue;
            }

            WriteHex(std::cout, p.buffer, test, sizeof(test), proc);

        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    break;
    case ASSET_TYPE_BG_CACHE: {
        hashutils::ReadDefaultFile();
        

        auto pool = std::make_unique<BGCache[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }



        BGCacheInfo entryinfo[pool::BG_CACHE_TYPE_COUNT]{};

        if (!proc.ReadMemory(&entryinfo[0], proc[0x4EC9A90], sizeof(entryinfo))) {
            std::cerr << "Can't read cache\n";
            return tool::BASIC_ERROR;
        }

        CHAR nameInfo[pool::BG_CACHE_TYPE_COUNT][200] = {};
        // buffer pool names
        for (size_t i = 0; i < pool::BG_CACHE_TYPE_COUNT; i++) {
            if (proc.ReadString(nameInfo[i], entryinfo[i].name, sizeof(nameInfo[i])) < 0) {
                std::cerr << "Can't read bgcache info names\n";
                return tool::BASIC_ERROR;
            }
        }

        CHAR dumpbuff[MAX_PATH + 10];
        size_t readFile = 0;

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            const auto& p = pool[i];

            if (!p.name || !p.def) {
                continue;
            }


            auto n = hashutils::ExtractPtr(p.name);

            std::cout << std::dec << i << ": " << std::dec << p.count << " elem (0x" << std::hex << p.def << ") : ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/bgcache/%s.csv", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name << std::dec;
                sprintf_s(dumpbuff, "%s/bgcache/file_%llx.csv", opt.m_output, p.name);
            }


            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::cout << "->" << file;

            if (!std::filesystem::exists(file, ec)) {
                readFile++;
                std::cout << " (new)";
            }
            std::cout << "\n";



            auto defs = std::make_unique<BGCacheInfoDef[]>(p.count);

            if (!proc.ReadMemory(&defs[0], p.def, sizeof(defs[0]) * p.count)) {
                std::cerr << "Can't def data\n";
                continue;
            }
            std::ofstream defout{ file };

            if (!defout) {
                std::cerr << "Can't open output file\n";
                continue;
            }

            defout << "type,name,ptr";

            for (size_t i = 0; i < p.count; i++) {
                auto& p2 = defs[i];

                defout << "\n" 
                    << (p2.type >= 0 && p2.type < pool::BG_CACHE_TYPE_COUNT ? nameInfo[p2.type] : "<error>") << ","
                    << hashutils::ExtractTmp("hash", p2.name) << "," 
                    << std::hex << p2.unk18
                    << std::flush;
            }

            defout.close();
        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
        break;
    case pool::ASSET_TYPE_SCRIPTBUNDLE: {
        hashutils::ReadDefaultFile();


        auto pool = std::make_unique<ScriptBundle[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }

        std::unordered_set<std::string> strings{};

        CHAR dumpbuff[MAX_PATH + 10];
        size_t readFile = 0;

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            const auto& p = pool[i];

            if (!p.hash) {
                continue;
            }

            ReadSBName(proc, p.sbObjectsArray);


            auto n = hashutils::ExtractPtr(p.hash);

            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/scriptbundle/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.hash << std::dec;
                sprintf_s(dumpbuff, "%s/scriptbundle/file_%llx.json", opt.m_output, p.hash);
            }


            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::cout << "->" << file;

            if (!std::filesystem::exists(file, ec)) {
                readFile++;
                std::cout << " (new)";
            }
            std::cout << "\n";


            std::ofstream defout{ file };

            if (!defout) {
                std::cerr << "Can't open output file\n";
                continue;
            }

            if (!ReadSBObject(proc, defout, 0, p.sbObjectsArray, strings)) {
                std::cerr << "Error when reading array\n";
                defout.close();
                std::filesystem::remove(file);
                continue;
            }

            defout.close();
        }

        std::ofstream outStr{ std::format("{}/scriptbundle_str.txt", opt.m_output) };

        if (outStr) {
            for (const auto& st : strings) {
                outStr << st << "\n";
            }
            outStr.close();
        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
        break;
    default:
    {
        std::cout << "Item data\n";

        BYTE* raw = new BYTE[entry.itemSize * entry.itemAllocCount];

        if (!proc.ReadMemory(raw, entry.pool, entry.itemSize * entry.itemAllocCount)) {
            delete[] raw;
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }

        for (size_t i = 0; i < min(10, entry.itemAllocCount); i++) {
            std::cout << "Element #" << std::dec << i << " " << std::hex << (entry.pool + i * entry.itemSize) << "\n";

            WriteHex(std::cout, entry.pool, &raw[i * entry.itemSize], entry.itemSize, proc);
        }

        delete[] raw;
    }
        break;
    }

    hashutils::WriteExtracted(opt.m_dump_hashmap);
	return tool::OK;
}

int dumpbgcache(const Process& proc, int argc, const char* argv[]) {
    BGCacheInfo info[40]{};

    if (!proc.ReadMemory(&info[0], proc[0x4EC9A90], sizeof(info))) {
        std::cerr << "Can't read cache\n";
        return tool::BASIC_ERROR;
    }
    XAssetTypeInfo entryinfo[pool::ASSET_TYPE_COUNT];

    if (!proc.ReadMemory(&entryinfo, proc[offset::s_XAssetTypeInfo], sizeof(entryinfo))) {
        std::cerr << "Can't read xasset info\n";
        return tool::BASIC_ERROR;
    }

    std::ofstream out{ "bgcache.csv" };

    if (!out) {
        std::cerr << "Can't open output\n";
        return tool::BASIC_ERROR;
    }

    hashutils::ReadDefaultFile();

    CHAR strBuff[0x100] = { 0 };

    out << "id,name,xasset,unkc,registerfunc,unregisterFunc,hash,unk30,unk38,unk3C,checksum,b1,b2,b3,b4";

    for (size_t i = 0; i < (sizeof(info) / sizeof(info[0])); i++)
    {
        auto& nfo = info[i];
        out << "\n" << std::dec << i << ",";

        if (nfo.name) {
            if (proc.ReadString(strBuff, nfo.name, sizeof(strBuff)) < 0) {
                out << "<error>";
            }
            else {
                out << strBuff;
            }
        }
        else {
            out << "<undef>";
        }

        out << ",";

        if (nfo.assetType >= 0 && nfo.assetType < pool::ASSET_TYPE_COUNT && entryinfo[nfo.assetType].name) {
            if (proc.ReadString(strBuff, entryinfo[nfo.assetType].name, sizeof(strBuff)) < 0) {
                out << "<error>";
            }
            else {
                out << strBuff;
            }
        }
        else {
            out << "<none>";
        }
        
        out << "," << nfo.unkc << ",";

        proc.WriteLocation(out, nfo.registerFunc) << ",";
        proc.WriteLocation(out, nfo.unregisterFunc) << ",";

        out << hashutils::ExtractTmp("hash", nfo.hash) << "," << std::flush
            << nfo.unk30 << ","
            << nfo.unk38 << ","
            << nfo.unk3c << ","
            << nfo.checksum << ","
            << (int)nfo.unk44 << ","
            << (int)nfo.unk45 << ","
            << (int)nfo.unk46 << ","
            << (int)nfo.unk47;
    }

    out.close();

    return tool::OK;
}

int dbmtstrs(const Process& proc, int argc, const char* argv[]) {
    std::unordered_set<std::string> buffer{};

    // no clue how big it is
    uint32_t max = 0x100000;

    for (uint32_t i = 0; i < max; i++) {
        auto* str = ReadMTString(proc, i);
        if (str) {
            buffer.insert(str);
        }
        if ((i + 1) % (max / 20) == 0) {
            std::cout << std::dec << (i * 100 / max) << "% : " << buffer.size() << "read\n";
        }
    }

    std::ofstream outStr{ "mtstrings.txt" };

    if (!outStr) {
        std::cerr << "Can't open output\n";
        return tool::BASIC_ERROR;
    }

    for (const auto& st : buffer) {
        outStr << st << "\n";
    }

    outStr.close();

    std::cout << "done.\n";

    return tool::OK;
}

ADD_TOOL("dp", " [input=pool_name] (output=pool_id)", "dump pool", true, pooltool);
ADD_TOOL("dbgcache", "", "dump bg cache", true, dumpbgcache);
ADD_TOOL("dbmtstrs", "", "dump mt strings", true, dbmtstrs);
