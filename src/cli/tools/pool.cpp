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
    uint64_t globalvar_count;
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
    uintptr_t string_count; // 0x20
    uintptr_t globalvar_count; // 0x28
    uintptr_t unk28; // 0x30
    uintptr_t unk30; // 0x38
    uintptr_t unk38; // 0x40
    uintptr_t unk40; // 0x48
    uintptr_t unk48; // 0x50
    uintptr_t unk50; // 0x58
};
struct BGPoolEntry {
    UINT64 name;
    UINT64 namepad;
    uintptr_t assetHeader;
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
    uint64_t string_count;
};

struct BGCacheInfo {
    uintptr_t name;
    pool::XAssetType assetType;
    uint32_t allocItems;
    uintptr_t registerFunc;
    uintptr_t unregisterFunc;
    uint64_t hash;
    uint64_t hashnull;
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
    uint64_t string_count;
    SB_ObjectsArray sbObjectsArray;
};

enum eModes : INT32 {
    MODE_ZOMBIES = 0x0,
    MODE_MULTIPLAYER = 0x1,
    MODE_CAMPAIGN = 0x2,
    MODE_WARZONE = 0x3,
    MODE_COUNT = 0x4,
    MODE_INVALID = 0x4,
    MODE_FIRST = 0x0,
};

const char* EModeName(eModes mode) {
    switch (mode) {
    case MODE_ZOMBIES: return "zombies";
    case MODE_MULTIPLAYER: return "multiplayer";
    case MODE_CAMPAIGN: return "campaign";
    case MODE_WARZONE: return "warzone";
    default: return "<invalid>";
    }

}

struct Hash {
    uint64_t name;
    uint64_t nullpad;
};

struct GameTypeTableEntry {
    Hash name;
    Hash baseGameType;
    uint64_t unk20;
    Hash nameRef;
    Hash nameRefCaps;
    Hash descriptionRef;
    Hash unk58;
    bool isHardcoreAvailable;
    Hash hardcoreNameRef;
    bool isTeamBased;
    bool hideHudScore;
    uint64_t groupName;
    uintptr_t image; // GfxImage*
    Hash presenceString;
    uint64_t unkA8;
    uint64_t scoreInfoFile;
    uint64_t unkB8;
    uint64_t unkC0;
    uint64_t unkC8;
    uint64_t unkD0;
    uint64_t unkD8;
    uint64_t unkE0;
    uint64_t unkE8;
    uint64_t unkF0;
    int uniqueID;
    uint64_t unk100;
    uint64_t unk108;
};

struct GameTypeTable {
    UINT64 name;
    UINT64 namepad;
    UINT32 gameTypeCount;
    uintptr_t gameTypes; // GameTypeTableEntry*
    eModes sessionMode;
};
struct MapTable {//30
    UINT64 name;
    UINT64 namepad;
    UINT32 mapCount;
    uintptr_t maps; // MapTableEntry*
    eModes sessionMode;
    UINT32 campaignMode;
    UINT32 dlcIndex;
};
struct MapTableListElem
{
    uint64_t count;
    uintptr_t names; // Hash*
    uint64_t unk10;
    uint64_t unk18;
};

struct MapTableList
{
    Hash name;
    MapTableListElem list_campaign;
    MapTableListElem list_multiplayer;
    MapTableListElem list_zombies;
    MapTableListElem list_warzone;
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
            case 25: // int?
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

const char* ReadTmpStr(const Process& proc, uintptr_t location) {
    static CHAR tmp_buff[0x1000];

    if (proc.ReadString(tmp_buff, location, sizeof(tmp_buff)) < 0) {
        sprintf_s(tmp_buff, "<invalid:%llx>", location);
    }
    return tmp_buff;
}

int pooltool(Process& proc, int argc, const char* argv[]) {
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
    case ASSET_TYPE_GAMETYPETABLE:
    {
        hashutils::ReadDefaultFile();
        size_t readFile = 0;


        auto pool = std::make_unique<GameTypeTable[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        CHAR dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            auto n = hashutils::ExtractPtr(p.name);

            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/gametype/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/gametype/file_%llx.json", opt.m_output, p.name);
            }

            if (!p.gameTypes || !proc.ReadMemory<UINT64>(p.gameTypes)) {
                std::cerr << "error when reading buffer at " << p.gameTypes << "\n";
                continue;
            }

            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::cout << "->" << file;

            if (!std::filesystem::exists(file, ec)) {
                readFile++;
                std::cout << " (new)";
            }

            auto entries = std::make_unique<GameTypeTableEntry[]>(p.gameTypeCount);


            if (!proc.ReadMemory(&entries[0], p.gameTypes, sizeof(entries[0]) * p.gameTypeCount)) {
                std::cerr << "Can't read entries data\n";
                break;
            }

            std::ofstream out{ file };

            if (!out) {
                std::cerr << "Can't open output file\n";
                break;
            }

            out
                << "{\n"
                << "    \"name\": \"" << hashutils::ExtractTmp("hash", p.name) << std::flush << "\",\n"
                << "    \"sessionMode\": \"" << EModeName(p.sessionMode) << "\",\n"
                << "    \"gametypes\": ["
                ;

            for (size_t j = 0; j < p.gameTypeCount; j++) {
                auto& e = entries[j];

                if (j) {
                    out << ",";
                }
                out
                    << "\n"
                    << "        {\n"
                    << "            \"uniqueID\": " << std::dec << e.uniqueID << ",\n"
                    << "            \"name\": \"" << ReadTmpStr(proc, e.name.name) << std::flush << "\",\n"
                    << "            \"baseGameType\": \"" << ReadTmpStr(proc, e.baseGameType.name) << std::flush << "\",\n"
                    << "            \"nameRef\": \"#" << hashutils::ExtractTmp("hash", e.nameRef.name) << std::flush << "\",\n"
                    << "            \"nameRefCaps\": \"#" << hashutils::ExtractTmp("hash", e.nameRefCaps.name) << std::flush << "\",\n"
                    << "            \"descriptionRef\": \"#" << hashutils::ExtractTmp("hash", e.descriptionRef.name) << std::flush << "\",\n"
                    << "            \"isHardcoreAvailable\": " << (e.isHardcoreAvailable ? "true" : "false") << ",\n"
                    << "            \"hardcoreNameRef\": \"#" << hashutils::ExtractTmp("hash", e.hardcoreNameRef.name) << std::flush << "\",\n"
                    << "            \"isTeamBased\": " << (e.isTeamBased ? "true" : "false") << ",\n"
                    << "            \"hideHudScore\": " << (e.hideHudScore ? "true" : "false") << ",\n"
                    << "            \"groupname\": \"" << ReadTmpStr(proc, e.groupName) << "\",\n"
                    << "            \"presenceString\": \"#" << hashutils::ExtractTmp("hash", e.presenceString.name) << std::flush << "\"\n"
                    << "        }"
                    ;
            }

            out
                << "\n"
                << "    ]\n"
                << "}"
                ;

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";


        break;
    }
    case ASSET_TYPE_CUSTOMIZATION_TABLE:
    {
        hashutils::ReadDefaultFile();
        struct CustomizationTable
        {
            Hash name;
            UINT32 numPlayerRoles;
            uintptr_t playerRoles; // PlayerRoleLevelsPtr*
            UINT32 numHeads;
            uintptr_t heads; // CharacterHead*
        };


        auto pool = std::make_unique<CustomizationTable[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        CHAR dumpbuff[MAX_PATH + 10];

        std::unordered_set<std::string> strings{};

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            auto n = hashutils::ExtractPtr(p.name.name);

            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/customization/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/customization/file_%llx.json", opt.m_output, p.name.name);
            }

            std::cout << " -> " << dumpbuff << "\n";

            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::ofstream out{ file };

            if (!out) {
                std::cerr << "Can't open " << file << "\n";
                break;
            }

            out << "{\n"
                << "    \"heads\": ["
                ;

            if (p.numHeads) {
                struct CharacterHead {
                    Hash assetName;
                    Hash displayName;
                    uint64_t icon;
                    uint64_t xmodel;
                    uint32_t gender;
                    uint32_t kvpCount;
                    uintptr_t kvpItems;
                    uint64_t unk40;
                    uint64_t unk48;
                    uint64_t unk50;
                    uint64_t unk58;
                };
                auto heads = std::make_unique<CharacterHead[]>(p.numHeads);

                if (!proc.ReadMemory(&heads[0], p.heads, sizeof(heads[0]) * p.numHeads)) {
                    std::cerr << "Can't read heads\n";
                    break;
                }

                for (size_t j = 0; j < p.numHeads; j++) {
                    if (j) {
                        out << ",";
                    }
                    auto& h = heads[j];

                    out 
                        << "\n"
                        << "        {\n"
                        << "            \"name\": \"#" << hashutils::ExtractTmp("hash", h.assetName.name) << "\",\n"
                        << "            \"displayName\": \"#" << hashutils::ExtractTmp("hash", h.displayName.name) << "\",\n"
                        << "            \"xmodel\": \"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<UINT64>(h.xmodel)) << "\",\n"
                        << "            \"gender\": " << std::dec << h.gender << "\n"
                        << "        }"
                        ;
                }

            }


            out
                << "\n"
                << "    ],\n"
                << "    \"playerRoles\": ["
                ;

            if (p.numPlayerRoles) {
                auto roles = std::make_unique<uintptr_t[]>(p.numPlayerRoles);
                if (!proc.ReadMemory(&roles[0], p.playerRoles, sizeof(roles[0]) * p.numPlayerRoles)) {
                    std::cerr << "Can't read roles\n";
                    break;
                }

                for (size_t j = 0; j < p.numPlayerRoles; j++) {
                    if (j) {
                        out << ",";
                    }
                    out << "\n";
                    if (!roles[j]) {
                        out << "        null";
                        continue;
                    }

                    struct PlayerRoleLevels {
                        bool enabled;
                        uintptr_t data; // PlayerRoleTemplate
                    };
                    struct PlayerRoleTemplate {
                        Hash name;
                        uint64_t unk10;
                        uint64_t unk18;
                        Hash unk20;
                        uintptr_t bodyType; //CharacterBodyType*
                        uintptr_t category; //PlayerRoleCategory*
                        uint64_t unk40;
                        uint64_t unk48;
                        Hash specialistEquipment;
                        Hash specialistWeapon;
                        Hash ultimateWeapon;
                        uint64_t pad[10];
                    };

                    struct CharacterBodyType {
                        Hash name;
                        Hash displayName;
                        Hash description;
                        Hash heroWeapon;
                        uintptr_t mpDialog; // ScriptBundle*
                        uintptr_t chrName; // const char*

                        UINT64 pad[13];
                        uint32_t gender;
                        uint32_t unkbc;
                        uint64_t unkc0;
                        SB_ObjectsArray kvp;
                    };

                    struct PlayerRoleCategory {
                        Hash name;
                        Hash displayName;
                        Hash description;
                        uintptr_t icon;
                        int sortOrder;
                        SB_ObjectsArray kvp;
                    };

                    PlayerRoleLevels prl;
                    PlayerRoleTemplate tmpl;

                    if (!proc.ReadMemory(&prl, roles[j], sizeof(prl))) {
                        std::cerr << "Can't read player roles\n";
                        break;
                    }

                    if (!proc.ReadMemory(&tmpl, prl.data, sizeof(tmpl))) {
                        std::cerr << "Can't read player roles\n";
                        break;
                    }

                    CharacterBodyType body;

                    if (!proc.ReadMemory(&body, tmpl.bodyType, sizeof(body))) {
                        std::cerr << "Can't read character body\n";
                        break;
                    }

                    out
                        << "        {\n"
                        << "            \"id\": " << std::dec << j << ",\n"
                        << "            \"enabled\": " << (prl.enabled ? "true" : "false") << ",\n"
                        << "            \"name\": \"#" << hashutils::ExtractTmp("hash", tmpl.name.name) << "\",\n"
                        << "            \"specialistEquipment\": \"#" << hashutils::ExtractTmp("hash", tmpl.specialistEquipment.name) << "\",\n"
                        << "            \"specialistWeapon\": \"#" << hashutils::ExtractTmp("hash", tmpl.specialistWeapon.name) << "\",\n"
                        << "            \"ultimateWeapon\": \"#" << hashutils::ExtractTmp("hash", tmpl.ultimateWeapon.name) << "\",\n"
                        ;

                    if (tmpl.category) {
                        PlayerRoleCategory cat;
                        if (!proc.ReadMemory(&cat, tmpl.category, sizeof(cat))) {
                            std::cerr << "Can't read category\n";
                            break;
                        }
                        out
                            << "            \"category\": {\n"
                            << "                \"name\": \"#" << hashutils::ExtractTmp("hash", cat.name.name) << "\",\n"
                            << "                \"displayName\": \"#" << hashutils::ExtractTmp("hash", cat.displayName.name) << "\",\n"
                            << "                \"description\": \"#" << hashutils::ExtractTmp("hash", cat.description.name) << "\",\n"
                            << "                \"sortOrder\": " << std::dec << cat.sortOrder << ",\n"
                            << "                \"fields\": "
                            ;

                        if (!ReadSBObject(proc, out, 4, cat.kvp, strings)) {
                            std::cerr << "Can't read cat kvp array\n";
                            break;
                        }

                        out
                            << "\n"
                            << "            },\n"
                            ;
                    }

                    out
                        << "            \"body\": {\n"
                        << "                \"name\": \"#" << hashutils::ExtractTmp("hash", body.name.name) << "\",\n"
                        << "                \"displayName\": \"#" << hashutils::ExtractTmp("hash", body.displayName.name) << "\",\n"
                        << "                \"description\": \"#" << hashutils::ExtractTmp("hash", body.description.name) << "\",\n"
                        << "                \"heroWeapon\": \"#" << hashutils::ExtractTmp("hash", body.heroWeapon.name) << "\",\n"
                        << "                \"gender\": \"" << (body.gender ? "female" : "male") << "\",\n"
                        ;
                    if (body.mpDialog) {
                        out << "                \"mpDialogBundle\": \"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<UINT64>(body.mpDialog)) << "\",\n";
                    }
                    if (body.chrName) {
                        out << "                \"chrName\": \"" << ReadTmpStr(proc, body.chrName) << "\",\n";
                    }


                    out
                        << "                \"fields\": "
                        ;

                    if (!ReadSBObject(proc, out, 4, body.kvp, strings)) {
                        std::cerr << "Can't read kvp array\n";
                        break;
                    }

                    out
                        << "\n"
                        << "            }\n"
                        << "        }"
                        ;

                }
            }

            out
                << "\n"
                << "    ]\n"
                << "}";


            out.close();
        }

    }
        break;
    case ASSET_TYPE_MAPTABLE:
    {
        hashutils::ReadDefaultFile();
        size_t readFile = 0;


        auto pool = std::make_unique<MapTable[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        CHAR dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            auto n = hashutils::ExtractPtr(p.name);

            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/map/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/map/file_%llx.json", opt.m_output, p.name);
            }

            if (!p.maps || !proc.ReadMemory<UINT64>(p.maps)) {
                std::cerr << "error when reading buffer at " << p.maps << "\n";
                continue;
            }

            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::cout << "->" << file;

            if (!std::filesystem::exists(file, ec)) {
                readFile++;
                std::cout << " (new)";
            }

            struct __declspec(align(8)) MapTableEntry
            {
                uintptr_t name;
                Hash name_hashed;
                int size;
                Hash mapName;
                Hash rootMapName;
                uintptr_t missionName;
                Hash mapDescription;
                uint64_t unk58;
                uint64_t pad[18];
                uintptr_t loadingmovie;
                uint64_t pad1[20];
                uint64_t uniqueID;
            };



            auto entries = std::make_unique<MapTableEntry[]>(p.mapCount);


            if (!proc.ReadMemory(&entries[0], p.maps, sizeof(entries[0]) * p.mapCount)) {
                std::cerr << "Can't read entries data\n";
                break;
            }

            std::ofstream out{ file };

            if (!out) {
                std::cerr << "Can't open output file\n";
                break;
            }

            out
                << "{\n"
                << "    \"name\": \"" << hashutils::ExtractTmp("hash", p.name) << std::flush << "\",\n"
                << "    \"sessionMode\": \"" << EModeName(p.sessionMode) << "\",\n"
                << "    \"maps\": ["
                ;

            for (size_t j = 0; j < p.mapCount; j++) {
                auto& e = entries[j];

                if (j) {
                    out << ",";
                }
                out
                    << "\n"
                    << "        {\n"
                    << "            \"uniqueID\": " << std::dec << e.uniqueID << ",\n"
                    << "            \"name\": \"" << ReadTmpStr(proc, e.name) << std::flush << "\",\n"
                    << "            \"hashname\": \"#" << hashutils::ExtractTmp("hash", e.name_hashed.name) << std::flush << "\",\n"
                    << "            \"missionName\": \"" << ReadTmpStr(proc, e.missionName) << std::flush << "\",\n"
                    << "            \"mapName\": \"#" << hashutils::ExtractTmp("hash", e.mapName.name) << std::flush << "\",\n"
                    << "            \"rootMapName\": \"#" << hashutils::ExtractTmp("hash", e.rootMapName.name) << std::flush << "\",\n"
                    << "            \"mapDescription\": \"#" << hashutils::ExtractTmp("hash", e.mapDescription.name) << std::flush << "\",\n"
                    ;

                if (e.loadingmovie) {
                    out
                        << "            \"loadingmovie\": \"" << ReadTmpStr(proc, e.loadingmovie) << std::flush << "\",\n"
                        ;
                }

                out
                    << "            \"size\": " << std::dec << e.size << "\n"
                    << "        }"
                    ;
            }

            out
                << "\n"
                << "    ]\n"
                << "}"
                ;

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";


        break;
    }
    case ASSET_TYPE_WEAPON:
    {
        struct Weapon
        {
            Hash name;
            Hash baseWeapon;
            Hash description;
            byte pad[0x1520];
        };

        auto pool = std::make_unique<Weapon[]>(entry.itemAllocCount);

        if (sizeof(Weapon) != entry.itemSize) {
            std::cerr << "bad itemsize: " << std::hex << sizeof(Weapon) << "/" << entry.itemSize << "\n";
            return tool::BASIC_ERROR;
        }

        size_t readFile = 0;
        hashutils::ReadDefaultFile();

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        CHAR dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            auto n = hashutils::ExtractPtr(p.name.name);

            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/weapon/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/weapon/file_%llx.json", opt.m_output, p.name.name);
            }


            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::cout << "->" << file;

            if (!std::filesystem::exists(file, ec)) {
                readFile++;
                std::cout << " (new)";
            }

            std::ofstream out{ file };

            if (!out) {
                std::cerr << "Can't open output file\n";
                break;
            }

            out
                << "{\n"
                << "    \"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << std::flush << "\",\n"
                << "    \"baseWeapon\": \"#" << hashutils::ExtractTmp("hash", p.baseWeapon.name) << std::flush << "\",\n"
                << "    \"description\": \"#" << hashutils::ExtractTmp("hash", p.description.name) << std::flush << "\"\n"
                << "}\n"
                ;
            tool::pool::WriteHex(out, entry.pool + sizeof(pool[0]) * i, (BYTE*)&p, sizeof(p), proc);
            

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";


        break;
    }
    case ASSET_TYPE_MAPTABLE_LIST:
    {
        hashutils::ReadDefaultFile();


        auto pool = std::make_unique<MapTableList[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        CHAR dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            auto n = hashutils::ExtractPtr(p.name.name);

            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/map/list/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/map/list/file_%llx.json", opt.m_output, p.name.name);
            }

            std::cout << " -> " << dumpbuff << "\n";

            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);


            auto names = std::make_unique<Hash[]>(
                max(
                    max(p.list_campaign.count, p.list_multiplayer.count),
                    max(p.list_warzone.count, p.list_zombies.count)
                )
            );

            std::ofstream out{ file };

            if (!out) {
                std::cerr << "Can't open file\n";
                continue;
            }


            auto func = [&proc, &names, &out](MapTableListElem& lst, const char* name, bool begin = false) {
                if (!begin) {
                    out << ",";
                }
                out
                    << "\n"
                    << "    \"" << name << "\": [";

                if (lst.count && !proc.ReadMemory(&names[0], lst.names, sizeof(names[0]) * lst.count)) {
                    std::cerr << "Can't read count\n";
                    return;
                }

                for (size_t j = 0; j < lst.count; j++)
                {
                    if (j) {
                        out << ",";
                    }
                    out << "\n";
                    out << "        \"#" << hashutils::ExtractTmp("hash", names[j].name) << "\"";
                }

                out << "\n    ]";
            };

            out << "{";

            func(p.list_campaign, "cp", true);
            func(p.list_multiplayer, "mp");
            func(p.list_zombies, "zm");
            func(p.list_warzone, "wz");

            out << "\n}";


            out.close();
        }


        break;
    }
    case ASSET_TYPE_DDL + 200:
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
                    << std::hex << p2.string_count
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

        auto raw = std::make_unique<BYTE[]>(entry.itemSize * entry.itemAllocCount);

        if (!proc.ReadMemory(&raw[0], entry.pool, entry.itemSize * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }

        CHAR dumpbuff[MAX_PATH + 10];
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

    hashutils::WriteExtracted(opt.m_dump_hashmap);
	return tool::OK;
}

int dumpbgcache(Process& proc, int argc, const char* argv[]) {
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

    out << "id,name,xasset,count,registerfunc,unregisterFunc,hash,start,checksum";

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
        
        out << "," << nfo.allocItems << ",";

        proc.WriteLocation(out, nfo.registerFunc) << ",";
        proc.WriteLocation(out, nfo.unregisterFunc) << ",";

        out << hashutils::ExtractTmp("hash", nfo.hash) << "," << std::flush
            << nfo.startIndex << ","
            << nfo.checksum;
    }

    out.close();

    return tool::OK;
}

int dbmtstrs(Process& proc, int argc, const char* argv[]) {
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

int dbgp(Process& proc, int argc, const char* argv[]) {
    BGCacheInfo info[40] = {};

    if (!proc.ReadMemory(&info[0], proc[0x4EC9A90], sizeof(info))) {
        std::cerr << "Can't read cache\n";
        return tool::BASIC_ERROR;
    }

    
    std::filesystem::path out{ "bgpool" };

    std::filesystem::create_directories(out);
    hashutils::ReadDefaultFile();

    std::ofstream outInfo{ out / "caches.csv" };

    if (!outInfo) {
        std::cerr << "Can't open caches.csv file\n";
        return tool::BASIC_ERROR;
    }


    outInfo << "id,name,start,count";

    auto pool = proc[0x5D9D6D0];

    CHAR nameInfo[200] = {};
    CHAR fileInfo[200] = {};
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

ADD_TOOL("dp", " [input=pool_name] (output=pool_id)", "dump pool", L"BlackOps4.exe", pooltool);
ADD_TOOL("dbgcache", "", "dump bg cache", L"BlackOps4.exe", dumpbgcache);
ADD_TOOL("dbmtstrs", "", "dump mt strings", L"BlackOps4.exe", dbmtstrs);
ADD_TOOL("dbgp", "", "dump bg pool", L"BlackOps4.exe", dbgp);
