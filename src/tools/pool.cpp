#include <includes.hpp>

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
struct RawEntry {
    uintptr_t unk1; // 0x8
    uintptr_t pad0; // 0x10
    uintptr_t unk3; // 0x18
};
struct RawFileEntry {
    uintptr_t unk1; // 0x8
    uintptr_t unk2; // 0x10
    uintptr_t unk3; // 0x18
    uintptr_t unk4; // 0x20
};
struct GametypeEntry {
    uintptr_t v1; // 0x8
    uintptr_t pad0; // 0x10
    uintptr_t v3; // 0x18
    uintptr_t v4; // 0x20
    uintptr_t v5; // 0x28
};

void WriteHex(BYTE* buff, SIZE_T size) {
    for (size_t j = 0; j < size; j++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)buff[j] << " ";
        if ((j + 1) % 4 == 0) {
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}

int tool::pool::pooltool(const Process& proc, int argc, const char* argv[]) {
    if (argc < 3) {
        return tool::BAD_USAGE;
    }

    std::error_code ec;
    std::filesystem::create_directories("pool", ec);

    CHAR outputName[256];
    if (!_strcmpi(argv[2], "all")) {
        std::ofstream out{ "pool/xassetpools.csv", std::ios::out };

        if (!out) {
            std::cerr << "Can't open output file '" << outputName << "'\n";
            return -1;
        }

        out << "id,name,location,itemsize,items,max,singleton\n";

        const auto count = 0xa8;
        XAssetPoolEntry entry[count];
        uintptr_t entryNames[count];

        if (!proc.ReadMemory(&entry, proc[OFFSET_assetPool], sizeof(*entry) * count)) {
            std::cerr << "Can't read pool entry\n";
            return tool::BASIC_ERROR;
        }
        if (!proc.ReadMemory(&entryNames, proc[OFFSET_g_assetNames], sizeof(*entryNames) * count)) {
            std::cerr << "Can't read xasset names\n";
            return tool::BASIC_ERROR;
        }

        CHAR str[100];
        for (size_t i = 0; i < count; i++) {
            out << std::dec
                << i << ",";
            if (proc.ReadString(str, entryNames[i], 100) < 0) {
                std::cerr << "Can't read xasset name from " << std::hex << entryNames[i] << "\n";
                continue;
            }
            auto e = entry[i];
            out << std::hex
                << str << ","
                << e.pool << ","
                << e.itemSize << ","
                << e.itemAllocCount << ","
                << e.itemCount << ","
                << (e.isSingleton ? "true" : "false") << "\n";
        }
        out.close();

        return tool::OK;
    }

    auto id = std::atoi(argv[2]);

    std::cout << std::hex << "pool id: " << id << "\n";

    XAssetPoolEntry entry;

    if (!proc.ReadMemory(&entry, proc[OFFSET_assetPool] + sizeof(entry) * id, sizeof(entry))) {
        std::cerr << "Can't read pool entry\n";
        return tool::BASIC_ERROR;
    }

    snprintf(outputName, 256, "pool/pool_%x", id);

    std::cout << std::hex
        << "pool ........ " << entry.pool << "\n"
        << "free head ... " << entry.freeHead << "\n"
        << "item size ... " << entry.itemSize << "\n"
        << "count ....... " << entry.itemCount << "\n"
        << "alloc count . " << entry.itemAllocCount << "\n"
        << "singleton ... " << (entry.isSingleton ? "true" : "false") << "\n"
        ;

    switch (id) {
    case 0x10: // translation
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
            for (size_t j = 0; j < entry.itemSize; j++) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)raw[i * entry.itemSize + j] << " ";
                if ((j + 1) % 4 == 0) {
                    std::cout << "\n";
                }
            }
            std::cout << "\n";
        }

        delete[] raw;
    }
        break;
    }


	return tool::OK;
}
