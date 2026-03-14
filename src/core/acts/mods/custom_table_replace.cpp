#include <includes.hpp>
#include <games/bo4/offsets.hpp>
#include <games/bo4/pool.hpp>

using namespace games::bo4;

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
    byte value[20];
    StringTableCellType type;
};
// item size ... 40
struct StringTableEntry {
    uint64_t name; // 8
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
struct XAssetPoolEntry {
    uintptr_t pool;
    uint32_t itemSize;
    int32_t itemCount;
    byte isSingleton;
    int32_t itemAllocCount;
    uintptr_t freeHead;
};

static int custom_table_replace(int argc, const char* argv[]) {
    hashutils::ReadDefaultFile();
    Process proc{ "blackops4.exe" };

    if (!proc || !proc.Open()) {
        std::cerr << "Can't find black ops 4\n";
        return tool::BASIC_ERROR;
    }

    XAssetPoolEntry entry{};

    if (!proc.ReadMemory(&entry, proc[offset::assetPool] + sizeof(entry) * pool::ASSET_TYPE_STRINGTABLE, sizeof(entry))) {
        std::cerr << "Can't read pool entry\n";
        return tool::BASIC_ERROR;
    }

    auto pool = std::make_unique<StringTableEntry[]>(entry.itemAllocCount);

    if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
        std::cerr << "Can't read pool data\n";
        return tool::BASIC_ERROR;
    }

    StringTableCell cell;
    StringTableCell cell3[20];

    size_t readFile = 0;

    auto target1 = 0x5c6dc30ffb97ee8bull;
    auto target2 = hash::Hash64Pattern("gamedata/events/schedule_pc.csv");
    auto target3 = hash::Hash64Pattern("gamedata/loot/loot_bribes.csv");
    auto global_challenges = hash::Hash64Pattern("gamedata/stats/globalchallenges.csv");

    uint64_t targetstats[] = {
        hash::Hash64Pattern("gamedata/stats/wz/statsmilestones1.csv"),
        hash::Hash64Pattern("gamedata/stats/wz/statsmilestones2.csv"),
        hash::Hash64Pattern("gamedata/stats/wz/statsmilestones3.csv"),

        hash::Hash64Pattern("gamedata/stats/mp/statsmilestones1.csv"),
        hash::Hash64Pattern("gamedata/stats/mp/statsmilestones2.csv"),
        hash::Hash64Pattern("gamedata/stats/mp/statsmilestones3.csv"),
        hash::Hash64Pattern("gamedata/stats/mp/statsmilestones4.csv"),
        hash::Hash64Pattern("gamedata/stats/mp/statsmilestones5.csv"),

        hash::Hash64Pattern("gamedata/stats/zm/statsmilestones1.csv"),
        hash::Hash64Pattern("gamedata/stats/zm/statsmilestones2.csv"),
        hash::Hash64Pattern("gamedata/stats/zm/statsmilestones3.csv"),
        hash::Hash64Pattern("gamedata/stats/zm/statsmilestones4.csv"),
        hash::Hash64Pattern("gamedata/stats/zm/statsmilestones5.csv"),
    };
    uint64_t targetstats2[] = {
        hash::Hash64Pattern("gamedata/tables/mp/mp_combat_training_challenges.csv"),
        hash::Hash64Pattern("gamedata/weapons/zm/zm_gunlevels.csv"),
        hash::Hash64Pattern("gamedata/weapons/mp/mp_gunlevels.csv"),
    };

    for (size_t i = 0; i < std::min(entry.itemAllocCount, entry.itemCount); i++) {
        const auto& e = pool[i];

        bool isstats = std::find(&targetstats[0], std::end(targetstats), e.name) != std::end(targetstats);
        bool isstats2 = std::find(&targetstats2[0], std::end(targetstats2), e.name) != std::end(targetstats2);

        if (!(e.name == target1
            || e.name == target2
            || e.name == target3
            || isstats
            || isstats2
            || e.name == global_challenges
            )) {
            continue; // not our taget
        }

        const auto size = e.columnCount * e.rowCount;

        if (!e.values || !size) {
            continue; // check that we can read at least the cell
        }

        std::cout << std::dec << i << ": " << hashutils::ExtractTmpScript(e.name) 
            << " (columns: " << e.columnCount << ", rows:" << e.rowCount << "/" << std::hex << (entry.pool + i * sizeof(entry)) << "/" << e.values << std::dec << ")\n";

        /*
         * season 2 = 0
         * season 2 bonus = 1
         * season 3 = 2
         * season 4 = 5
         * season 5 = 7
         * season 6 4 july = 9
         * season 6 = 10
         * season 7 = 11
         * season 7 bonus = 12
         * season 8 = 13
         */
        auto targetCell1 = 13 * e.columnCount + 2; // old1
        auto targetCell2 = 0 * e.columnCount + 2; // new1
        auto targetCell3 = 1 * e.columnCount + 2; // new2

        // set season 2
        if (target2 == e.name) {
            if (!proc.ReadMemory(&cell, e.values + sizeof(cell) * targetCell2, sizeof(cell))) {
                std::cerr << "Can't read cell " << targetCell2 << "\n";
                continue;
            }

            std::cout << "old:" << cell.type << "  " << *reinterpret_cast<int64_t*>(&(cell.value[0])) << "\n";
            cell.type = STC_TYPE_INT;
            *reinterpret_cast<int64_t*>(&(cell.value[0])) = 2147364000;

            if (!proc.WriteMemory(e.values + sizeof(cell) * targetCell2, &cell, sizeof(cell))) {
                std::cerr << "Can't write cell " << i << "\n";
                continue;
            }
            if (!proc.ReadMemory(&cell, e.values + sizeof(cell) * targetCell3, sizeof(cell))) {
                std::cerr << "Can't read cell " << targetCell3 << "\n";
                continue;
            }

            std::cout << "old:" << cell.type << "  " << *reinterpret_cast<int64_t*>(&(cell.value[0])) << "\n";
            cell.type = STC_TYPE_INT;
            *reinterpret_cast<int64_t*>(&(cell.value[0])) = 2147364000;

            if (!proc.WriteMemory(e.values + sizeof(cell) * targetCell3, &cell, sizeof(cell))) {
                std::cerr << "Can't write cell " << i << "\n";
                continue;
            }

            if (!proc.ReadMemory(&cell, e.values + sizeof(cell) * targetCell1, sizeof(cell))) {
                std::cerr << "Can't read cell " << targetCell1 << "\n";
                continue;
            }

            std::cout << "old:" << cell.type << "  " << *reinterpret_cast<int64_t*>(&(cell.value[0])) << "\n";
            cell.type = STC_TYPE_INT;
            *reinterpret_cast<int64_t*>(&(cell.value[0])) = 1569862900;
            //std::cout << cell.type << "  " << *reinterpret_cast<int64_t*>(&(cell.value[0])) << "\n";

            if (!proc.WriteMemory(e.values + sizeof(cell) * targetCell1, &cell, sizeof(cell))) {
                std::cerr << "Can't write cell " << i << "\n";
                continue;
            }
        }
        else if (target1 == e.name) {
            continue;
            // set all item cost to 0 (not working)
            for (size_t i = 1; i < e.rowCount; i++) {
                if (!proc.ReadMemory(&cell3[0], e.values + sizeof(cell) * (i * e.columnCount + 2), sizeof(cell) * 4)) {
                    std::cerr << "Can't read cell " << i << "\n";
                    continue;
                }
        
                //std::cout << cell.type << "  " << *reinterpret_cast<int64_t*>(&(cell.value[0])) << "\n";
                cell3[2].type = STC_TYPE_INT;
                cell3[3].type = STC_TYPE_INT;
                *reinterpret_cast<int64_t*>(&(cell3[1].value[0])) = 1; // cp
                *reinterpret_cast<int64_t*>(&(cell3[3].value[0])) = 1; // crates
        
        
                if (!proc.WriteMemory(e.values + sizeof(cell) * (i * e.columnCount + 2), &cell3[0], sizeof(cell) * 4)) {
                    std::cerr << "Can't write cell " << i << "\n";
                    continue;
                }
            }
        }
        else if (target3 == e.name) {
            // set all item cost to 1 (not working)
            for (size_t i = 0; i < e.rowCount; i++) {
                if (!proc.ReadMemory(&cell, e.values + sizeof(cell) * (i * e.columnCount + 8), sizeof(cell))) {
                    std::cerr << "Can't read cell " << i << "\n";
                    continue;
                }

                //std::cout << "old:" << cell.type << "  " << *reinterpret_cast<int64_t*>(&(cell.value[0])) << "\n";
                cell.type = STC_TYPE_INT;
                *reinterpret_cast<int64_t*>(&(cell.value[0])) = 1;


                if (!proc.WriteMemory(e.values + sizeof(cell) * (i * e.columnCount + 8), &cell, sizeof(cell))) {
                    std::cerr << "Can't write cell " << i << "\n";
                    continue;
                }
            }
        }
        else if (isstats) {
            // set all stats min to 0 (index = 2)
            for (size_t i = 0; i < e.rowCount; i++) {
                if (!proc.ReadMemory(&cell, e.values + sizeof(cell) * (i * e.columnCount + 2), sizeof(cell))) {
                    std::cerr << "Can't read cell " << i << "\n";
                    continue;
                }

                //std::cout << "old:" << cell.type << "  " << *reinterpret_cast<int64_t*>(&(cell.value[0])) << "\n";
                cell.type = STC_TYPE_INT;
                *reinterpret_cast<int64_t*>(&(cell.value[0])) = 0;


                if (!proc.WriteMemory(e.values + sizeof(cell) * (i * e.columnCount + 2), &cell, sizeof(cell))) {
                    std::cerr << "Can't write cell " << i << "\n";
                    continue;
                }
            }
        }
        else if (isstats2) {
            // set all stats min to 0 (index = 1)
            for (size_t i = 0; i < e.rowCount; i++) {
                if (!proc.ReadMemory(&cell, e.values + sizeof(cell) * (i * e.columnCount + 1), sizeof(cell))) {
                    std::cerr << "Can't read cell " << i << "\n";
                    continue;
                }

                //std::cout << "old:" << cell.type << "  " << *reinterpret_cast<int64_t*>(&(cell.value[0])) << "\n";
                cell.type = STC_TYPE_INT;
                *reinterpret_cast<int64_t*>(&(cell.value[0])) = 0;


                if (!proc.WriteMemory(e.values + sizeof(cell) * (i * e.columnCount + 1), &cell, sizeof(cell))) {
                    std::cerr << "Can't write cell " << i << "\n";
                    continue;
                }
            }
        }
        else if (global_challenges == e.name) {
            // set all stats min to 0 (index = 1,2,3)
            for (size_t i = 0; i < e.rowCount; i++) {
                if (!proc.ReadMemory(&cell3[0], e.values + sizeof(cell) * (i * e.columnCount + 1), sizeof(cell) * 3)) {
                    std::cerr << "Can't read cell " << i << "\n";
                    continue;
                }

                // mp
                cell3[0].type = STC_TYPE_INT;
                *reinterpret_cast<int64_t*>(&(cell3[0].value[0])) = 0;
                // zm
                cell3[1].type = STC_TYPE_INT;
                *reinterpret_cast<int64_t*>(&(cell3[1].value[0])) = 0;
                // wz
                cell3[2].type = STC_TYPE_INT;
                *reinterpret_cast<int64_t*>(&(cell3[2].value[0])) = 0;


                if (!proc.WriteMemory(e.values + sizeof(cell) * (i * e.columnCount + 1), &cell3[0], sizeof(cell) * 2)) {
                    std::cerr << "Can't write cell " << i << "\n";
                    continue;
                }
            }
        }

    }


	return tool::OK;
}

#ifndef CI_BUILD

ADD_MOD("ctr", "replace table data", custom_table_replace);

#endif
