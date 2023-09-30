#include <includes.hpp>

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

void WriteHex(std::ostream& out, uintptr_t base, BYTE* buff, SIZE_T size, const Process& proc) {
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
        snprintf(outputName, sizeof(outputName), "%s/xassetpools.csv", opt.m_output);
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

    snprintf(outputName, 256, "%s/pool_%x", opt.m_output, id);

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
        const size_t dumpbuffsize = sizeof(dumpbuff);
        CHAR namebuf[2000];
        const size_t namebufsize = sizeof(namebuf);
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
                snprintf(dumpbuff, dumpbuffsize, "%s/%s", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << e.name << std::dec;
                snprintf(dumpbuff, dumpbuffsize, "%s/hashed/stringtables/file_%llx.csv", opt.m_output, e.name);

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
                        if (proc.ReadString(namebuf, *reinterpret_cast<uintptr_t*>(&cell[j].value[0]), namebufsize) < 0) {
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

        snprintf(outputName, sizeof(outputName), "%s/strings.csv", opt.m_output);

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
    case ASSET_TYPE_RAWFILE:
    {

        hashutils::ReadDefaultFile();


        auto pool = std::make_unique<RawFileEntry[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        CHAR dumpbuff[MAX_PATH + 10];
        const size_t dumpbuffsize = sizeof(dumpbuff);
        std::vector<BYTE> read{};
        size_t readFile = 0;

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            const auto& p = pool[i];

            auto n = hashutils::ExtractPtr(p.name);

            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                snprintf(dumpbuff, dumpbuffsize, "%s/%s", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name << std::dec;
                snprintf(dumpbuff, dumpbuffsize, "%s/hashed/rawfile/file_%llx.raw", opt.m_output, p.name);
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

        for (size_t i = 0; i < min(10, entry.itemAllocCount); i++) {
            const auto& p = pool[i];

            auto n = hashutils::ExtractPtr(p.name);
            std::cout << "- " << std::dec << i << " : " << hashutils::ExtractTmp("file", p.name) << ": " << std::hex << p.buffer << "\n";

            if (!proc.ReadMemory(test, p.buffer, sizeof(test))) {
                std::cerr << "Bad read\n";
                continue;
            }

            WriteHex(std::cout, p.buffer, test, sizeof(test), proc);

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

ADD_TOOL("dp", " [input=pool_name] (output=pool_id)", "dump pool", true, pooltool);
