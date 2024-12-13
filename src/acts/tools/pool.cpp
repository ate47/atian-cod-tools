#include <includes.hpp>
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include "tools/pool.hpp"
#include "tools/fastfile.hpp"
#include "pool_weapon.hpp"
#include <utils/decrypt.hpp>
#include <games/bo4/pool.hpp>
#include <games/bo4/offsets.hpp>

using namespace tool::pool;
using namespace games::bo4;

// min hash value, otherwise might be a ptr
constexpr auto MIN_HASH_VAL = 0x1000000000000;

#pragma region data



static const char* itemGroupNames[] = {
    "weapon_smg",
    "weapon_assault",
    "weapon_tactical",
    "weapon_cqb",
    "weapon_lmg",
    "weapon_sniper",
    "weapon_pistol",
    "weapon_launcher",
    "weapon_knife",
    "weapon_special",
    "weapon_shotgun",
    "weapon_explosive",
    "weapon_grenade",
    "weapon_masterkey",
    "weapon_grenadelauncher",
    "weapon_flamethrower",
    "specialty",
    "talisman",
    "specialgrenade",
    "miscweapon",
    "feature",
    "bonuscard",
    "killstreak",
    "hero",
    "talent",
    "bubblegum",
    "bubblegum_consumable",
    "character",
};

static const char* attachmentsNames[] = {
    "none",
    "acog",
    "adsreload",
    "barrelchoke",
    "clantag",
    "custom1",
    "custom2",
    "custom3",
    "custom4",
    "custom5",
    "damage",
    "damage2",
    "dualoptic",
    "dw",
    "dynzoom",
    "elo",
    "extbarrel",
    "extbarrel2",
    "extclip",
    "extclip2",
    "extrapellets",
    "fastlockon",
    "fastreload",
    "fastreload2",
    "fmj",
    "fmj2",
    "gl",
    "grip",
    "grip2",
    "hipgrip",
    "holdbreath",
    "holo",
    "ir",
    "is",
    "killcounter",
    "mixclip",
    "mk",
    "mms",
    "null",
    "pistolscope",
    "quickdraw",
    "quickdraw2",
    "rangefinder",
    "reddot",
    "reflex",
    "rf",
    "sf",
    "speedreloader",
    "stackfire",
    "stalker",
    "stalker2",
    "steadyaim",
    "steadyaim2",
    "supply",
    "suppressed",
    "swayreduc",
    "tacknife",
    "uber",
    "vzoom"
};


const char* tool::pool::WeapInventoryTypeName(weapInventoryType_t t) {
    static const char* names[] = {
        "PRIMARY",
        "OFFHAND",
        "ITEM",
        "ALTMODE",
        "MELEE",
        "DWLEFTHAND",
        "ABILITY",
        "HEAVY",
    };
    return t >= ARRAYSIZE(names) ? "invalid" : names[t];
}
const char* tool::pool::WeapTypeName(weapType_t t) {
    static const char* names[] = {
        "BULLET",
        "GRENADE",
        "PROJECTILE",
        "BINOCULARS",
        "GAS",
        "BOMB",
        "MINE",
        "MELEE",
        "RIOTSHIELD",
        "SCRIPT",
    };
    return t >= ARRAYSIZE(names) ? "invalid" : names[t];
}
const char* tool::pool::GadgetTypeName(gadgetType_e t) {
    static const char* names[] = {
        "NONE",
        "OTHER",
        "OPTIC_CAMO",
        "ARMOR_REGEN",
        "ARMOR",
        "DRONE",
        "VISION_PULSE",
        "MULTI_ROCKET",
        "TURRET_DEPLOY",
        "GRENADE",
        "JUKE",
        "HACKER",
        "INFRARED",
        "SPEED_BURST",
        "HERO_WEAPON",
        "COMBAT_EFFICIENCY",
        "FLASHBACK",
        "CLEANSE",
        "SYSTEM_OVERLOAD",
        "SERVO_SHORTOUT",
        "EXO_BREAKDOWN",
        "SURGE",
        "RAVAGE_CORE",
        "REMOTE_HIJACK",
        "IFF_OVERRIDE",
        "CACOPHONY",
        "FORCED_MALFUNCTION",
        "CONCUSSIVE_WAVE",
        "OVERDRIVE",
        "UNSTOPPABLE_FORCE",
        "RAPID_STRIKE",
        "ACTIVE_CAMO",
        "SENSORY_OVERLOAD",
        "ES_STRIKE",
        "IMMOLATION",
        "FIREFLY_SWARM",
        "SMOKESCREEN",
        "MISDIRECTION",
        "MRPUKEY",
        "SHOCK_FIELD",
        "RESURRECT",
        "HEAT_WAVE",
        "CLONE",
        "ROULETTE",
        "THIEF",
        "DISRUPTOR",
        "HORNET_SWARM",
        "GROUP_REVIVE",
        "XRAY_EYES",
        "EMERGENCY_MELEE",
        "SHOULDER_GUN",
        "GRAPPLE",
        "INVULNERABLE",
        "SPRINT_BOOST",
        "HEALTH_REGEN",
        "SELF_DESTRUCT",
        "BARRIER_BUILDER",
        "SPAWN_BEACON",
    };
    return t >= ARRAYSIZE(names) ? "invalid" : names[t];
}
const char* tool::pool::GuidedMissileTypeName(guidedMissileType_t t) {
    static const char* names[] = {
        "NONE",
        "SIDEWINDER",
        "HELLFIRE",
        "JAVELIN",
        "BALLISTIC",
        "WIREGUIDED",
        "TVGUIDED",
        "DRONE",
        "HEATSEEKING",
        "ROBOTECH",
        "DYNAMICIMPACTPOINT",
    };
    return t >= ARRAYSIZE(names) ? "invalid" : names[t];
}
const char* tool::pool::OffhandClassName(OffhandClass t) {
    static const char* names[] = {
        "NONE",
        "FRAG_GRENADE",
        "SMOKE_GRENADE",
        "FLASH_GRENADE",
        "GEAR",
        "SUPPLYDROP_MARKER",
        "GADGET",
    };
    return t >= ARRAYSIZE(names) ? "invalid" : names[t];
}
const char* tool::pool::OffhandSlotName(OffhandSlot t) {
    static const char* names[] = {
        "NONE",
        "LETHAL_GRENADE",
        "TACTICAL_GRENADE",
        "EQUIPMENT",
        "SPECIFIC_USE",
        "GADGET",
        "SPECIAL",
        "HERO_WEAPON",
        "TAUNT",
        "SCRIPTED",
    };
    return t >= ARRAYSIZE(names) ? "invalid" : names[t];
}
const char* tool::pool::LockOnTypeName(lockOnType_t t) {
    static const char* names[] = {
        "NONE",
        "LEGACY_SINGLE",
        "AP_SINGLE",
        "AP_MULTI",
    };
    return t >= ARRAYSIZE(names) ? "invalid" : names[t];
}
const char* tool::pool::WeapClassName(weapClass_t t) {
    static const char* names[] = {
        "RIFLE",
        "MG",
        "SMG",
        "SPREAD",
        "PISTOL",
        "GRENADE",
        "ROCKETLAUNCHER",
        "TURRET",
        "NON_PLAYER",
        "GAS",
        "ITEM",
        "MELEE",
        "KILLSTREAK_ALT_STORED_WEAPON",
        "PISTOL_SPREAD",
        "BALL",
    };
    return t >= ARRAYSIZE(names) ? "invalid" : names[t];
}
const char* tool::pool::ProjExplosionTypeName(projExplosionType_t t) {
    static const char* names[] = {
        "NONE",
        "GRENADE",
        "ROCKET",
        "FLASHBANG",
        "DUD",
        "SMOKE",
        "HEAVY_EXPLOSIVE",
        "FIRE",
        "NAPALM_BLOB",
        "BOLD",
        "SHRAPNEL_SPAN",
    };
    return t >= ARRAYSIZE(names) ? "invalid" : names[t];
}

#pragma endregion
enum PoolDumpOptionFlags {
    DDL_OFFSET = 1,
    DUMP_ASSETS = 2, // dump assets (not for the source repository because it's not an acceptable content)
};

class PoolOption {
public:
    bool m_help = false;
    bool m_any_type = false;
    bool m_dump_info = false;
    bool m_dump_all_available = false;
    const char* m_output = "pool";
    const char* m_dump_hashmap = NULL;
    std::vector<bool> m_dump_types{};
    uint64_t flags{};

    bool Compute(const char** args, INT startIndex, INT endIndex) {
        m_dump_types.clear();
        m_dump_types.reserve(pool::ASSET_TYPE_COUNT);
        for (size_t i = 0; i < pool::ASSET_TYPE_COUNT; i++) {
            m_dump_types.push_back(false);
        }
        // default values
        for (size_t i = startIndex; i < endIndex; i++) {
            const char* arg = args[i];

            if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
                m_help = true;
            }
            else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                m_output = args[++i];
            }
            else if (!_strcmpi("--info", arg) || !strcmp("-i", arg)) {
                m_dump_info = true;
            }
            else if (!_strcmpi("--all", arg) || !strcmp("-a", arg)) {
                m_dump_all_available = true;
            }
            else if (!strcmp("-f", arg) || !_strcmpi("--flag", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                const char* flagName = args[++i];

                if (!_strcmpi("ddloffset", arg)) {
                    flags |= DDL_OFFSET;
                }
                else if (!_strcmpi("assets", arg)) {
                    flags |= DUMP_ASSETS;
                }
                else {
                    LOG_ERROR("Invalid flag for {}: {}", arg, flagName);
                    return false;
                }
            }
            else if (!strcmp("-m", arg) || !_strcmpi("--hashmap", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                m_dump_hashmap = args[++i];
            }
            else if (*arg == '-') {
                LOG_ERROR("Invalid argument {}!", arg);
                return false;
            }
            else {
                pool::XAssetType assetType = pool::XAssetIdFromName(arg);

                if (assetType == pool::ASSET_TYPE_COUNT) {
                    if (*arg >= '0' && *arg <= '9') {
                        try {
                            assetType = (pool::XAssetType)std::strtol(arg, nullptr, 10);
                        }
                        catch (const std::invalid_argument& e) {
                            LOG_ERROR("Can't parse pool id '{}'", e.what());
                            assetType = pool::ASSET_TYPE_COUNT;
                        }
                    }
                    if (assetType < 0 || assetType >= pool::ASSET_TYPE_COUNT) {
                        LOG_ERROR("Invalid pool name: {}!", arg);
                        return false;
                    }
                }

                m_dump_types[assetType] = true;
                m_any_type = true;
            }
        }
        return true;
    }
    void PrintHelp() {
        LOG_INFO("-h --help       : Print help");
        LOG_INFO("-i --info       : Dump pool info");
        LOG_INFO("-a --all        : Dump all available pools");
        LOG_INFO("-o --output [d] : Output dir");
        LOG_INFO("-f --flag [f]   : Add flag");
    }
};


#pragma region structs



struct XAssetTypeInfo {
    uintptr_t name; // const char*
    uint64_t size;
    uint64_t globalvar_count;
    uintptr_t GetName; // const char* (__cdecl*)(const XAssetHeader*);
    uintptr_t SetName; // void(__cdecl* )(XAssetHeader*, uint64_t*);
};

struct XAssetPoolEntry {
    uintptr_t pool;
    uint32_t itemSize;
    int32_t itemCount;
    byte isSingleton;
    int32_t itemAllocCount;
    uintptr_t freeHead;
};
struct TranslationEntry {
    uintptr_t data;
    uintptr_t name;
    uint64_t pad0;
};
struct RawFileEntry {
    uintptr_t name; // 0x8
    uintptr_t pad0; // 0x10
    uintptr_t size; // 0x18
    uintptr_t buffer; // 0x20
};
struct RawString {
    uint64_t name; // 0x8
    uintptr_t padding; // 0x10 0
    uintptr_t stringvalue; // 0x18 const char*
};
struct LuaFile {
    uint64_t name;
    uint64_t pad08;
    uint32_t size;
    uintptr_t buffer;
};
struct BGPoolEntry {
    uint64_t name;
    uint64_t namepad;
    uintptr_t assetHeader;
};

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

const char* EModeName(eModes mode, bool allocInvalid = false, const char* countVal = nullptr) {
    switch (mode) {
    case MODE_ZOMBIES: return "zombies";
    case MODE_MULTIPLAYER: return "multiplayer";
    case MODE_CAMPAIGN: return "campaign";
    case MODE_WARZONE: return "warzone";
    case MODE_COUNT:
        if (countVal) return countVal;
    default: {
        if (!allocInvalid) {
            return "<invalid>";
        }
        static char invalidBuffer[0x50];

        sprintf_s(invalidBuffer, "<invalid:%d>", (int)mode);
        return invalidBuffer;
    }
    }
}
const char* LobbyModeName(int mode, bool allocInvalid = false) {
    switch (mode) {
    case -1: return "campaign";
    case 0: return "multiplayer";
    case 1: return "zombies";
    case 2: return "warzone";
    default: {
        if (!allocInvalid) {
            return "<invalid>";
        }
        static char invalidBuffer[0x50];

        sprintf_s(invalidBuffer, "<invalid:%d>", (int)mode);
        return invalidBuffer;
    }
    }
}

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
    uint64_t name;
    uint64_t namepad;
    uint32_t gameTypeCount;
    uintptr_t gameTypes; // GameTypeTableEntry*
    eModes sessionMode;
};
struct MapTable {//30
    uint64_t name;
    uint64_t namepad;
    uint32_t mapCount;
    uintptr_t maps; // MapTableEntry*
    eModes sessionMode;
    uint32_t campaignMode;
    uint32_t dlcIndex;
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
#pragma endregion

inline bool HexValidString(const char* str) {
    if (!*str) {
        return false;
    }
    for (const char* s = str; *s; s++) {
        if (*s < ' ' || *s > '~') {
            return false;
        }
    }
    return true;
}

void tool::pool::WriteHex(std::ostream& out, uintptr_t base, byte* buff, size_t size, const Process& proc) {
    char strBuffer[101];
    for (size_t j = 0; j < size; j++) {
        if (j % 8 == 0) {
            if (base) {
                out << std::hex << std::setw(16) << std::setfill('0') << (base + j) << "~";
            }
            out << std::hex << std::setw(3) << std::setfill('0') << j << "|";
            if (j + 7 < size) {
                out << std::hex << std::setw(16) << std::setfill('0') << *reinterpret_cast<uint64_t*>(&buff[j]);
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
                uint64_t val = *reinterpret_cast<uint64_t*>(&buff[j - 7]);
                if (val) {
                    // not null, hash?
                    const char* h = hashutils::ExtractPtr(val);
                    if (h) {
                        out << " #" << h;
                    }
                    else if (proc.ReadString(strBuffer, val, sizeof(strBuffer) - 1) >= 0 && HexValidString(strBuffer)) {
                        out << " ->" << strBuffer;
                    }
                    else if (proc.ReadMemory(strBuffer, val, sizeof(uint64_t))) {
                        uint64_t r = *reinterpret_cast<uint64_t*>(strBuffer);

                        const char* h = hashutils::ExtractPtr(r);
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
void tool::pool::WriteHex(std::ostream& out, uintptr_t base, byte* buff, size_t size) {
    for (size_t j = 0; j < size; j++) {
        if (j % 8 == 0) {
            if (base) {
                out << std::hex << std::setw(16) << std::setfill('0') << (base + j) << "~";
            }
            out << std::hex << std::setw(3) << std::setfill('0') << j << "|";
            if (j + 7 < size) {
                out << std::hex << std::setw(16) << std::setfill('0') << *reinterpret_cast<uint64_t*>(&buff[j]);
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
                uint64_t val = *reinterpret_cast<uint64_t*>(&buff[j - 7]);
                if (val) {
                    // not null, hash?
                    const char* h = hashutils::ExtractPtr(val);
                    if (h) {
                        out << " #" << h;
                    }
                }
            }
            out << "\n";
        }
    }
    out << "\n";
}

const char* tool::pool::ReadMTString(const Process& proc, uint32_t val) {
    static char str_read[0x2001];
    uintptr_t strptr = proc.ReadMemory<uintptr_t>(proc[offset::mt_buffer]) + (uint32_t)(0x14 * val);
    if (!proc.ReadMemory<int16_t>(strptr) || proc.ReadMemory<byte>(strptr + 3) != 7) {
        return nullptr;
    }
    
    if (!strptr || proc.ReadString(str_read, strptr + 0x10, 0x2001) < 0) {
        return nullptr;
    }

    byte flag = proc.ReadMemory<byte>(strptr + 2);
    const char* strDec;
    if ((flag & 0x40) || flag >= 0) {
        // not encrypted
        strDec = str_read;
    }
    else {
        strDec = decrypt::DecryptString(str_read);
    }

    return strDec;
}

const char* ReadTmpStr(const Process& proc, uintptr_t location) {
    static char tmp_buff[0x1000];

    if (proc.ReadString(tmp_buff, location, sizeof(tmp_buff)) < 0) {
        sprintf_s(tmp_buff, "<invalid:%llx>", location);
    }
    return tmp_buff;
}

#pragma region scriptbundle_dump

struct SB_ObjectsArray {
    uint64_t sbObjectCount;
    uintptr_t sbObjects; // SB_Object
    uint64_t sbSubCount;
    uintptr_t sbSubs;
};

struct ScriptBundle {
    Hash name;
    Hash bundleType;
    SB_ObjectsArray sbObjectsArray;
};

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

    constexpr uint32_t nameHash = hash::HashT89Scr("name");
    constexpr uint32_t typeHash = hash::HashT89Scr("type");

    for (size_t i = 0; i < arr.sbObjectCount; i++) {
        auto& obj = objects[i];

        if (obj.name != nameHash && obj.name != typeHash) {
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
        LOG_ERROR("Error when reading object at depth {}", depth);
        return false;
    }

    defout << "{";

    bool nofirst = false;

    std::unordered_set<uint32_t> keys{};

    if (arr.sbSubCount) {
        struct SB_Sub {
            uint32_t keyname;
            uint32_t unk4;
            uint64_t size;
            uintptr_t item;
        };
        auto subs = std::make_unique<SB_Sub[]>(arr.sbSubCount);

        if (!proc.ReadMemory(&subs[0], arr.sbSubs, sizeof(subs[0]) * arr.sbSubCount)) {
            LOG_ERROR("Error when reading object at depth {}", depth);
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
                LOG_ERROR("Can't read array key");
                return false;
            }
            keys.insert(hash::HashT89Scr(strval));
            defout << "\"" << strval << "\": [";

            for (size_t j = 0; j < sub.size; j++) {
                if (j) {
                    defout << ",";
                }
                utils::Padding(defout << "\n", depth + 2);
                SB_ObjectsArray item{};

                if (!proc.ReadMemory(&item, sub.item + sizeof(item) * j, sizeof(item))
                    || !ReadSBObject(proc, defout, depth + 2, item, strings)) {
                    LOG_ERROR("Can't read array item");
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
    if (nofirst) {
        utils::Padding(defout << "\n", depth) << "}";
    }
    else {
        defout << "}";
    }

    return true;
}

#pragma endregion

#pragma region ddl_dump

struct DDLDef {
    XHash name;
    uint64_t unk10;
    uint64_t metatable; // ID64Metatable
    uintptr_t structList; // DDLStruct*
    uintptr_t enumList; // DDLEnum*
    uintptr_t next; // DDLDef*
    uint32_t unk38;
    uint32_t byteSize;
    int32_t structCount;
    int32_t enumCount;
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

struct DDLHashTable {
    uintptr_t list; //DDLHash*
    int count;
    int max;
};
struct DDLStruct {
    Hash name;
    uint32_t bitSize;
    uint32_t memberCount;
    uintptr_t members; // DDLMember* 
    DDLHashTable hashTableLower;
    DDLHashTable hashTableUpper;
};

struct __declspec(align(8)) DDLMember {
    Hash name;
    int32_t bitSize;
    uint32_t intSize;
    uint32_t offset;
    uint32_t maxIntValue;
    int16_t arraySize;
    int16_t externalIndex;
    int16_t externalEnumType;
    DDLType type;
    bool isArray;
};
struct DDLHash {
    uint64_t hash;
    int index;
};

struct DDLEnum {
    Hash name;
    uintptr_t members; // Hash*
    int memberCount;
    DDLHashTable hashTable;
};

const char* DDLTypeName(const DDLMember& member) {
    switch (member.type) {
    case DDL_BYTE_TYPE: return "byte";
    case DDL_SHORT_TYPE: return "short";
    case DDL_UINT_TYPE: {
        if (member.intSize == 1) return "bool";
        if (member.intSize == 32) return "uint";
        return utils::va("uint:%lld", member.intSize);
    }
    case DDL_INT_TYPE: {
        if (member.intSize == 32) return "int";
        return utils::va("int:%lld", member.intSize);
    }
    case DDL_UINT64_TYPE: return "uint64";
    case DDL_FLOAT_TYPE: return "float";
    case DDL_FIXEDPOINT_TYPE: {
        return utils::va("fixed<%lld,%lld>", member.intSize, member.maxIntValue);
    }
    case DDL_HASH_TYPE: return "xhash";
    case DDL_STRING_TYPE: return utils::va("string(%lld)", (member.bitSize / (member.isArray ? member.arraySize : 1)) / 8);
    case DDL_STRUCT_TYPE: return "struct";
    case DDL_ENUM_TYPE: return "enum";
    case DDL_PAD_TYPE: return "uint:1";
    case DDL_INVALID_TYPE:
    default: return "<invalid>";
    }
}




void ReadDDLStruct(PoolOption& opt, Process& proc, std::ostream& defout, DDLDef& def, DDLStruct& stct, size_t idx) {
    bool isRoot = stct.name.name == hash::Hash64("root");
    int depth = 1;
    int depthFields = isRoot ? 1 : 2;

    auto members = std::make_unique<DDLMember[]>(stct.memberCount);

    if (!proc.ReadMemory(&members[0], stct.members, sizeof(members[0]) * stct.memberCount)) {
        defout << "<error reading members entry for " << hashutils::ExtractTmp("hash", stct.name.name) << ">\n";
        return;
    }
    // sort members because they don't match the internal structure (they match the hashmap)
    std::sort(&members[0], &members[stct.memberCount], [](const DDLMember& e1, const DDLMember& e2) {
        return e1.offset < e2.offset;
    });

    utils::Padding(defout, 1) << "// idx " << std::dec << idx << " members " << stct.memberCount << " size 0x" << std::hex << stct.bitSize;
    if (!isRoot) utils::Padding(defout << "\n", depth) << "struct " << hashutils::ExtractTmp("hash", stct.name.name) << " {";


    int64_t currentShift = 0;
    for (size_t i = 0; i < stct.memberCount; i++) {

        auto& mbm = members[i];

        utils::Padding(defout << "\n", depthFields);

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
            utils::Padding(defout, depthFields);
        }

        utils::Padding(defout << "// offset 0x" << std::hex << mbm.offset << ", size 0x" << mbm.bitSize << "\n", depthFields);
        currentShift = mbm.offset + mbm.bitSize;

        bool addSize = false;
        if (mbm.type == DDL_STRUCT_TYPE) {
            DDLStruct substct{};
            if (!proc.ReadMemory(&substct, def.structList + mbm.externalIndex * sizeof(substct), sizeof(substct))) {
                defout << "<error reading sub struct entry>\n";
                return;
            }
            defout << hashutils::ExtractTmp("hash", substct.name.name);
        }
        else if (mbm.type == DDL_ENUM_TYPE) {
            DDLEnum subenum{};
            if (!proc.ReadMemory(&subenum, def.enumList + mbm.externalIndex * sizeof(subenum), sizeof(subenum))) {
                defout << "<error reading sub enum entry>\n";
                return;
            }
            defout << hashutils::ExtractTmp("hash", subenum.name.name);
        }
        else {
            defout << DDLTypeName(mbm);
        }

        defout << " " << hashutils::ExtractTmp("hash", mbm.name.name);

        if (mbm.isArray) {
            if (mbm.externalEnumType >= 0 && mbm.externalEnumType < def.enumCount) {
                DDLEnum subenum{};
                if (!proc.ReadMemory(&subenum, def.enumList + mbm.externalEnumType * sizeof(subenum), sizeof(subenum))) {
                    defout << "<error reading sub enum entry>\n";
                    return;
                }
                defout << "[" << hashutils::ExtractTmp("hash", subenum.name.name) << "]";
            }
            else {
                defout << "[" << std::dec << mbm.arraySize << "]";
            }
        }

        if (mbm.type == DDL_PAD_TYPE) {
            defout << "[" << std::dec << mbm.bitSize << "]";
        }

        defout << ";";
    }
    defout << "\n";

    if (!isRoot)utils::Padding(defout, depth) << "};\n\n";
}


void ReadDDLEnum(Process& proc, std::ostream& defout, DDLEnum& enumst, size_t idx) {
    utils::Padding(defout, 1) << "// idx " << std::dec << idx << " members " << enumst.memberCount << "\n";
    utils::Padding(defout, 1) << "enum " << hashutils::ExtractTmp("hash", enumst.name.name) << " {";

    auto members = std::make_unique<Hash[]>(enumst.memberCount);

    if (!proc.ReadMemory(&members[0], enumst.members, sizeof(members[0]) * enumst.memberCount)) {
        defout << "<error reading members entry>\n";
    }
    else {

        for (size_t i = 0; i < enumst.memberCount; i++) {
            auto& mbm = members[i];
            if (i) defout << ",";
            defout << "\n";

            const char* ext{ hashutils::ExtractTmp("hash", mbm.name) };
            bool containBad{};

            const char* ext2{ ext };
            while (*ext2) {
                if (*ext2 >= 0x7f || *ext2 <= 0x21) {
                    containBad = true;
                    break;
                }
                ext2++;
            }


            utils::Padding(defout, 2);
            
            if (containBad) {
                utils::PrintFormattedString(defout << "\"", ext) << "\"";
            }
            else {
                defout << ext;
            }
            
            defout << ", // 0x" << std::hex << i;
        }
        defout << "\n";
    }

    utils::Padding(defout, 1) << "};\n\n";
}

void ReadDDLDefEntry(PoolOption& opt, Process& proc, std::ostream& defout, uintptr_t entry) {
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
        << "// " << hashutils::ExtractTmp("hash", def.name.name) << "\n"
        << "// metatable \"" << hashutils::ExtractTmp("hash", def.metatable) << "\"\n"
        << "\n"
        ;

    if (def.structCount) {
        auto structList = proc.ReadMemoryArrayEx<DDLStruct>(def.structList, def.structCount);

        defout
            << "version " << std::dec << def.version << " {\n"
            ;
        for (size_t i = 1; i < def.structCount; i++) {
            ReadDDLStruct(opt, proc, defout, def, structList[i], i);
        }
        if (def.enumCount) {
            auto enumList = proc.ReadMemoryArrayEx<DDLEnum>(def.enumList, def.enumCount);
            for (size_t i = 0; i < def.enumCount; i++) {
                ReadDDLEnum(proc, defout, enumList[i], i);
            }
        }

        ReadDDLStruct(opt, proc, defout, def, structList[0], 0);
        defout << "}\n\n";
    }
    else {
        defout << "version " << std::dec << def.version << "{}\n";
    }

    if (def.next) {
        defout << "/////////////////////////////////////////////////\n";
        ReadDDLDefEntry(opt, proc, defout, def.next);
    }
}

#pragma endregion

int pooltoolnames(Process& proc, int argc, const char* argv[]) {
    using namespace pool;

    hashutils::ReadDefaultFile();

    XAssetPoolEntry entry{};

    std::unordered_map<XAssetType, size_t> xoffsets{
        { ASSET_TYPE_RAGDOLL, 0xBA0 },
        { ASSET_TYPE_FLAMETABLE, 0x1B0 },
        { ASSET_TYPE_XANIM, 0x70 },
        { ASSET_TYPE_IMAGE, 0x20 },
        { ASSET_TYPE_KLF, 0x18 },
        { ASSET_TYPE_PLAYLISTS, 0x10 },
        { ASSET_TYPE_DLOGEVENT, 8 },
        { ASSET_TYPE_UNLOCKABLE_ITEM, 8 },
        { ASSET_TYPE_STORECATEGORY, 8 },
        { ASSET_TYPE_STORAGEFILE, 8 },
        { ASSET_TYPE_SOUND, 8 },
        { ASSET_TYPE_SANIM, 8 },
        { ASSET_TYPE_PLAYLIST_GLOBAL_SETTINGS, 8 },
        { ASSET_TYPE_PLAYLIST_SCHEDULE, 8 },
        { ASSET_TYPE_LOCALIZE_ENTRY, 8 },
    };

    for (size_t i = 0; i < ASSET_TYPE_COUNT; i++) {
        auto id = (XAssetType)i;

        if (!proc.ReadMemory(&entry, proc[offset::assetPool] + sizeof(entry) * id, sizeof(entry))) {
            LOG_ERROR("Can't read pool entry");
            return false;
        }

        std::error_code ec;

        auto name = XAssetNameFromId(id);

        auto output = utils::va("output_xassets_names/%s.csv", name);
        std::filesystem::path file{ output };
        std::filesystem::create_directories(file.parent_path(), ec);
        std::ofstream defout{ file };
        LOG_INFO("{} -> {}", name, output);

        if (!defout) {
            LOG_ERROR("Can't open output file");
            continue;
        }
            

        defout << "id,name,type,offset";
        auto off = xoffsets[id];

        for (size_t asset = 0; asset < entry.itemAllocCount; asset++) {
            uintptr_t offset = entry.pool + entry.itemSize * asset;
            uint64_t xhash = proc.ReadMemory<uint64_t>(offset + off);

            defout << "\n" << std::dec << asset << "," << hashutils::ExtractTmp("hash", xhash) << "," << name << "," << std::hex << offset;
        }

        defout.close();
    }

    return tool::OK;
}

int tool::pool::pooltool(Process& proc, int argc, const char* argv[]) {
    using namespace ::pool;
    PoolOption opt;

    if (!opt.Compute(argv, 2, argc) || opt.m_help) {
        opt.PrintHelp();
        return tool::OK;
    }

    if (opt.m_dump_hashmap) {
        hashutils::SaveExtracted(true, false);
    }

    std::error_code ec;
    std::filesystem::create_directories(opt.m_output, ec);

    char outputName[256];
    if (opt.m_dump_info) {
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

        char str[100];
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

        std::filesystem::path pretty{ outputName };
        std::cout << "Dump info " << pretty.string() << "\n";

        return tool::OK;
    }

    if (!opt.m_any_type && !opt.m_dump_all_available) {
        opt.PrintHelp();
        return tool::OK;
    }
    hashutils::ReadDefaultFile();

    std::unordered_set<std::string> strings{};
    XAssetPoolEntry entry{};
    auto ShouldHandle = [&proc, &opt, &outputName, &entry](XAssetType id, bool isKnown = true) {
        if (!opt.m_dump_types[id] && !(isKnown && opt.m_dump_all_available)) {
            return false;
        }
        // set to false for the default loop
        opt.m_dump_types[id] = false;

        std::cout << "pool: " << std::dec << XAssetNameFromId(id) << " (" << (int)id << ")\n";

        if (!proc.ReadMemory(&entry, proc[offset::assetPool] + sizeof(entry) * id, sizeof(entry))) {
            std::cerr << "Can't read pool entry\n";
            return false;
        }

        sprintf_s(outputName, "%s/pool_%s", opt.m_output, XAssetNameFromId(id));

        std::cout << std::hex
            << "pool ........ " << entry.pool << "\n"
            << "free head ... " << entry.freeHead << "\n"
            << "item size ... " << entry.itemSize << "\n"
            << "count ....... " << entry.itemCount << "\n"
            << "alloc count . " << entry.itemAllocCount << "\n"
            << "singleton ... " << (entry.isSingleton ? "true" : "false") << "\n"
            ;

        return true;
    };

    if (ShouldHandle(ASSET_TYPE_LOCALIZE_ENTRY)) { 
        std::ofstream out{ outputName, std::ios::out };

        if (!out) {
            std::cerr << "Can't open output file '" << outputName << "'\n";
            return -1;
        }

        auto raw = std::make_unique<TranslationEntry[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(raw.get(), entry.pool, sizeof(raw[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }


        char str[4096];
        out << "key,translation\n";
        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            if (proc.ReadString(str, raw[i].data, 4096) < 0) {
                std::cerr << "Can't read translation " << i << "\n";
            }
            
            out << hashutils::ExtractTmp("hash", raw[i].name) << "," << decrypt::DecryptString(str) << "\n";
        }
        out.close();
    }
    if (ShouldHandle(ASSET_TYPE_STRINGTABLE)) {
        auto pool = std::make_unique<StringTableEntry[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }

        char dumpbuff[MAX_PATH + 10];
        char namebuf[2000];
        StringTableCell cell[200];
        const size_t cellsize = sizeof(cell) / sizeof(cell[0]);

        size_t readFile = 0;

        for (size_t i = 0; i < std::min(entry.itemAllocCount, entry.itemCount); i++) {
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
                if (!proc.ReadMemory(&cell[0], e.values + sizeof(cell[0]) * e.columnCount * i, sizeof(cell[0]) * std::min(cellsize, (size_t)e.columnCount))) {
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
                            out << "<bad_str:" << std::hex << *reinterpret_cast<uintptr_t*>(&cell[j].value[0]) << ">";
                        }
                        else {
                            out << namebuf;
                        }
                        break;
                    case STC_TYPE_INT:
                        out << *reinterpret_cast<int64_t*>(&cell[j].value[0]);
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
                        out << "#" << hashutils::ExtractTmp("hash", *reinterpret_cast<uint64_t*>(&cell[j].value[0]));
                        break;
                    default:
                        //out << "unk type: " << cell[j].type;
                        out << "?" << std::hex
                            << *reinterpret_cast<uint64_t*>(&cell[j].value[0])
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
    }
    if (ShouldHandle(ASSET_TYPE_RAWSTRING)) {
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
        char str[4096];
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
    if (ShouldHandle(ASSET_TYPE_LUAFILE, false)) {
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
                auto buffer = std::make_unique<byte[]>(p.size);

                if (!proc.ReadMemory(&buffer[0], p.buffer, p.size)) {
                    std::cerr << "Can't read buffer for " << outFile.string() << "\n";
                    continue;
                }

                utils::WriteFile(outFile, &buffer[0], p.size);
                std::cout << "Dumped " << outFile.string() << "\n";
            }
        }
    }
    if (ShouldHandle(ASSET_TYPE_RAWFILE)) {
        auto pool = std::make_unique<RawFileEntry[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];
        std::vector<byte> read{};
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

            if (!p.buffer || !proc.ReadMemory<uint64_t>(p.buffer)) {
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

            const void* output;
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
    if (ShouldHandle(ASSET_TYPE_GAMETYPETABLE)) {
        size_t readFile = 0;


        auto pool = std::make_unique<GameTypeTable[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

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

            if (!p.gameTypes || !proc.ReadMemory<uint64_t>(p.gameTypes)) {
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
    }
    if (ShouldHandle(ASSET_TYPE_CUSTOMIZATION_TABLE)) {
        struct CustomizationTable
        {
            Hash name;
            uint32_t numPlayerRoles;
            uintptr_t playerRoles; // PlayerRoleLevelsPtr*
            uint32_t numHeads;
            uintptr_t heads; // characterHead*
        };


        auto pool = std::make_unique<CustomizationTable[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

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
                struct characterHead {
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
                auto heads = std::make_unique<characterHead[]>(p.numHeads);

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
                        << "            \"xmodel\": \"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(h.xmodel)) << "\",\n"
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
                        uintptr_t bodyType; //characterBodyType*
                        uintptr_t category; //PlayerRoleCategory*
                        uint64_t unk40;
                        uint64_t unk48;
                        Hash specialistEquipment;
                        Hash specialistWeapon;
                        Hash ultimateWeapon;
                        uint64_t pad[10];
                    };

                    struct characterBodyType {
                        Hash name;
                        Hash displayName;
                        Hash description;
                        Hash heroWeapon;
                        uintptr_t mpDialog; // ScriptBundle*
                        uintptr_t chrName; // const char*

                        uint64_t pad[13];
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

                    characterBodyType body;

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
                        out << "                \"mpDialogBundle\": \"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(body.mpDialog)) << "\",\n";
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
    if (ShouldHandle(ASSET_TYPE_HIERARCHICAL_TASK_NETWORK)) {
        size_t readFile = 0;
        struct __declspec(align(8)) HierarchicalTaskNetwork {
            XHash name;
            uintptr_t htnNodes; // HierarchicalTaskNetworkNode*
            uint32_t numNodes;
        };
        enum HierarchicalTaskNetworkValueType_t : uint32_t {
            HTN_BOOL_TYPE = 0x0,
            HTN_FLOAT_TYPE = 0x1,
            HTN_INT_TYPE = 0x2,
            HTN_HASH_TYPE = 0x3,
            HTN_DATA_TYPE_COUNT = 0x4,
            HTN_INVALID_DATA_TYPE = 0x4,
        };
        enum HierarchicalTaskNetworkNodeType_t : uint32_t {
            HTN_ACTION_TYPE = 0x0,
            HTN_GOTO_TYPE = 0x1,
            HTN_PLANNER_TYPE = 0x2,
            HTN_POSTCONDITION_TYPE = 0x3,
            HTN_PRECONDITION_TYPE = 0x4,
            HTN_SELECTOR_TYPE = 0x5,
            HTN_SEQUENCE_TYPE = 0x6,
            HTN_TYPE_COUNT = 0x7,
            HTN_INVALID_TYPE = 0x7,
        };

        static const char* hierarchicalTaskNetworkValueTypeNames[]{
            "bool",
            "float",
            "int",
            "hash"
        };

        static const char* hierarchicalTaskNetworkNodeTypeNames[]{
            "action",
            "goto",
            "planner",
            "postcondition",
            "precondition",
            "selector",
            "sequence"
        };

        struct __declspec(align(8)) HierarchicalTaskNetworkNode {
            XHash name;
            HierarchicalTaskNetworkNodeType_t type;
            uint32_t index;
            uintptr_t childNodeIndexes; // uint32_t*
            uint32_t numChildrenNodes;
            uintptr_t constants; // HierarchicalTaskNetworkNodeConstant*
            uint32_t numConstants;
        };
        union HierarchicalTaskNetworkNodeConstantUnion {
            bool boolValue;
            float floatValue;
            int32_t intValue;
            ScrString_t stringValue;
            XHash hash;
        };

        struct HierarchicalTaskNetworkNodeConstant {
            XHash key;
            HierarchicalTaskNetworkValueType_t type;
            HierarchicalTaskNetworkNodeConstantUnion value;
        };

        auto [pool, ok] = proc.ReadMemoryArray<HierarchicalTaskNetwork>(entry.pool, entry.itemAllocCount);

        if (!ok) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }

        char dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            const char* n = hashutils::ExtractPtr(p.name.name);

            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/hierarchicaltasknetwork/%s", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/hierarchicaltasknetwork/file_%llx.ai_htn", opt.m_output, p.name.name);
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
                << "    \"name\": \"" << hashutils::ExtractTmp("hash", p.name.name) << std::flush << "\",\n"
                << "    \"nodes\": ["
                ;

            if (p.numNodes) {
                auto [htnNodes, ok2] = proc.ReadMemoryArray<HierarchicalTaskNetworkNode>(p.htnNodes, p.numNodes);

                if (!ok2) {
                    std::cerr << "Can't read htn nodes\n";
                    continue;
                }

                for (size_t j = 0; j < p.numNodes; j++) {
                    HierarchicalTaskNetworkNode& htnNode{ htnNodes[j] };

                    if (j) {
                        out << ",";
                    }
                    out
                        << "\n"
                        << "        {\n"
                        << "            \"id\": " << std::dec << j << ",\n"
                        << "            \"name\": \"#" << hashutils::ExtractTmp("hash", htnNode.name.name) << std::flush << "\",\n"
                        << "            \"index\": " << std::dec << htnNode.index << ",\n"
                        << "            \"type\": \"" << (htnNode.type >= HTN_TYPE_COUNT ? "<invalid>" : hierarchicalTaskNetworkNodeTypeNames[htnNode.type]) << "\",\n"
                        << "            \"childindexes\": ["
                        ;

                    if (htnNode.numChildrenNodes) {
                        auto [childNodeIndexes, ok3] = proc.ReadMemoryArray<uint32_t>(htnNode.childNodeIndexes, htnNode.numChildrenNodes);

                        if (!ok3) {
                            std::cerr << "Can't read childNodeIndexes\n";
                            continue;
                        }
                        for (size_t k = 0; k < htnNode.numChildrenNodes; k++) {
                            if (k) out << ", ";
                            out << std::dec << childNodeIndexes[k];
                        }
                    }
                    out << "],\n"
                        << "            \"constants\": {"
                        ;

                    if (htnNode.numConstants) {
                        auto [constants, ok3] = proc.ReadMemoryArray<HierarchicalTaskNetworkNodeConstant>(htnNode.constants, htnNode.numConstants);

                        if (!ok3) {
                            std::cerr << "Can't read childNodeIndexes\n";
                            continue;
                        }
                        for (size_t k = 0; k < htnNode.numConstants; k++) {
                            HierarchicalTaskNetworkNodeConstant& constant{ constants[k] };

                            if (k) out << ", ";

                            out
                                << "\n"
                                << "                \"#" << hashutils::ExtractTmp("hash", constant.key.name) << "\": {\n"
                                << "                    \"type\": \"" << (constant.type < HTN_DATA_TYPE_COUNT ? hierarchicalTaskNetworkValueTypeNames[constant.type] : "undefined") << "\",\n"
                                << "                    \"value\": "
                                ;

                            switch (constant.type) {
                            case HTN_BOOL_TYPE:
                                out << std::dec << (constant.value.boolValue ? "true" : "false");
                                break;
                            case HTN_FLOAT_TYPE:
                                out << constant.value.floatValue;
                                break;
                            case HTN_INT_TYPE:
                                out << std::dec << constant.value.intValue;
                                break;
                            case HTN_HASH_TYPE:
                                out << "\"#" << hashutils::ExtractTmp("hash", constant.value.hash.name) << "\"";
                                break;
                            default:
                                out << "null";
                            }

                            out
                                << "\n"
                                << "                }"
                                ;
                        }

                        out
                            << "\n"
                            << "            }\n"
                            ;
                    }
                    else {
                        out << "}\n";
                    }
                    out
                        << "        }"
                        ;
                }

                out
                    << "\n"
                    << "    ]\n"
                    ;
            }
            else {
                out << "]\n";
            }
            out << "}";

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_MAPTABLE)) {
        size_t readFile = 0;


        auto pool = std::make_unique<MapTable[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

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

            if (!p.maps || !proc.ReadMemory<uint64_t>(p.maps)) {
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
    }
    if (ShouldHandle(ASSET_TYPE_WEAPON, false)) {
        auto pool = std::make_unique<WeaponDef[]>(entry.itemAllocCount);

        if (sizeof(WeaponDef) != entry.itemSize) {
            std::cerr << "bad itemsize: " << std::hex << sizeof(WeaponDef) << "/" << entry.itemSize << "\n";
            return tool::BASIC_ERROR;
        }

        size_t readFile = 0;

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

        WeaponDefProperties props{};

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

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

            if (p.properties && !proc.ReadMemory(&props, p.properties, sizeof(props))) {
                std::cerr << "Can't read properties\n";
                continue;
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

            out << "{\n";
            utils::Padding(out, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(out, 1) << "\"baseWeapon\": \"#" << hashutils::ExtractTmp("hash", p.baseWeapon.name) << "\",\n";
            utils::Padding(out, 1) << "\"displayname\": \"#" << hashutils::ExtractTmp("hash", p.displayname.name) << "\",\n";
            utils::Padding(out, 1) << "\"sessionMode\": \"" << EModeName(p.sessionMode, false, "all") << "\",\n";
            utils::Padding(out, 1) << "\"itemIndex\": " << std::dec << p.itemIndex << ",\n";
            utils::Padding(out, 1) << "\"dualWieldWeaponIndex\": " << std::dec << p.dualWieldWeaponIndex << ",\n";
            utils::Padding(out, 1) << "\"altWeaponIndex\": " << std::dec << p.altWeaponIndex << ",\n";
            utils::Padding(out, 1) << "\"reticle\": \"" << ReadTmpStr(proc, p.unk8b0) << "\",\n";
#ifndef CI_BUILD
            // 8a0\|0*[^0]+.*\|
            utils::Padding(out, 1) << "\"unk8a0\": " << std::dec << p.unk8a0 << ",\n";
            utils::Padding(out, 1) << "\"unk8a4\": " << std::dec << p.unk8a4 << ",\n";
            utils::Padding(out, 1) << "\"unk8a8\": " << std::dec << p.unk8a8 << ",\n";
            utils::Padding(out, 1) << "\"unk8ac\": " << std::dec << p.unk8ac << ",\n";
            utils::Padding(out, 1) << "\"unk8e0\": " << std::dec << p.unk8e0 << ",\n";
            utils::Padding(out, 1) << "\"unk8e4\": " << std::dec << p.unk8e4 << ",\n";
            utils::Padding(out, 1) << "\"unk900\": " << std::dec << p.unk900 << ",\n";
            utils::Padding(out, 1) << "\"unk904\": " << std::dec << p.unk904 << ",\n";
            utils::Padding(out, 1) << "\"unka38\": " << std::dec << p.unka38 << ",\n";
            
#endif

            auto addPtrName = [&proc, &out](const char* title, uintptr_t ptr, size_t offset) {
                if (!ptr) {
                    return;
                }
                auto name = proc.ReadMemory<uint64_t>(ptr + offset);
                if (!name) {
                    return;
                }
                utils::Padding(out, 1) << "\"" << title << "\": \"#" << hashutils::ExtractTmp("hash", name) << "\",\n";
                };
            auto addPtrName2 = [&proc, &addPtrName](const char* title, uintptr_t ptr, size_t offset, size_t offset2) {
                return addPtrName(title, proc.ReadMemory<uintptr_t>(ptr + offset), offset2);
                };
            auto addPtrName3 = [&proc, &addPtrName2](const char* title, uintptr_t ptr, size_t offset, size_t offset2, size_t offset3) {
                return addPtrName2(title, proc.ReadMemory<uintptr_t>(ptr + offset), offset2, offset3);
                };

            auto addXHashName = [&proc, &out](const char* title, XHash& val) {
                if (!val.name) {
                    return;
                }
                utils::Padding(out, 1) << "\"" << title << "\": \"#" << hashutils::ExtractTmp("hash", val.name) << "\",\n";
                };
            addXHashName("unk30", p.unk30);
            addXHashName("unk40", p.unk40);
            addXHashName("unk50", p.unk50);
            addXHashName("unk60", p.unk60);
            addXHashName("unk70", p.unk70);
            addXHashName("unk80", p.unk80);

            // GfxImageHandle
            addPtrName("gadgetIconAvailable", p.gadgetIconAvailable, 0x20);
            addPtrName("gadgetIconUnavailable", p.gadgetIconUnavailable, 0x20);
            addPtrName("unkc70", p.unkc70, 0x20);
            addPtrName("unkc78", p.unkc78, 0x20);
            // Fx
            addPtrName("unkc80", p.unkc80, 0);
            addPtrName("unkc88", p.unkc88, 0);
            addPtrName("unkc90", p.unkc90, 0);
            addPtrName("unkc98", p.unkc98, 0);

            //addPtrName3("unkcb0", p.unkcb0, 0, 0, 0); // ???


            // XModel
            addPtrName2("frontendmodel", p.frontendmodel, 0, 0);
            addPtrName2("viewmodel", p.viewmodel, 0, 0);
            addPtrName2("worldModel", p.worldModel, 0, 0);
            addPtrName2("unkcd0", p.unkcd0, 0, 0);
            addPtrName2("stowedmodel", p.stowedmodel, 0, 0);
            addPtrName2("clipmodel", p.clipmodel, 0, 0);

            //addPtrName("unkd18", p.unkd18, 0);
            //addPtrName("unkd20", p.unkd20, 0);
            addPtrName("unkd28", p.unkd28, 0);

            addPtrName("var_22082a57", p.var_22082a57, 0);
            addPtrName("projectilemodel", p.projectilemodel, 0);
            addPtrName("var_4bcd08b0", p.var_4bcd08b0, 0);
            addPtrName("var_96850284", p.var_96850284, 0);
            addPtrName("var_26f68e75", p.var_26f68e75, 0);

            addXHashName("sound90", p.sound90);
            addXHashName("sounda0", p.sounda0);
            addXHashName("soundb0", p.soundb0);
            addXHashName("soundc0", p.soundc0);
            addXHashName("firesounddistant", p.firesounddistant);
            addXHashName("firesound", p.firesound);
            addXHashName("firesoundplayer", p.firesoundplayer);
            addXHashName("sound100", p.sound100);
            addXHashName("sound110", p.sound110);
            addXHashName("sound120", p.sound120);
            addXHashName("sound130", p.sound130);
            addXHashName("sound140", p.sound140);
            addXHashName("sound150", p.sound150);
            addXHashName("sound160", p.sound160);
            addXHashName("sound170", p.sound170);
            addXHashName("sound180", p.sound180);
            addXHashName("sound190", p.sound190);
            addXHashName("sound1a0", p.sound1a0);
            addXHashName("sound1b0", p.sound1b0);
            addXHashName("sound1c0", p.sound1c0);
            addXHashName("sound1d0", p.sound1d0);
            addXHashName("sound1e0", p.sound1e0);
            addXHashName("sound1f0", p.sound1f0);
            addXHashName("sound200", p.sound200);
            addXHashName("sound210", p.sound210);
            addXHashName("sound220", p.sound220);
            addXHashName("sound230", p.sound230);
            addXHashName("sound240", p.sound240);
            addXHashName("sound250", p.sound250);
            addXHashName("sound260", p.sound260);
            addXHashName("sound270", p.sound270);
            addXHashName("sound280", p.sound280);
            addXHashName("sound290", p.sound290);
            addXHashName("sound2a0", p.sound2a0);
            addXHashName("sound2b0", p.sound2b0);
            addXHashName("sound2c0", p.sound2c0);
            addXHashName("sound2d0", p.sound2d0);
            addXHashName("sound2e0", p.sound2e0);
            addXHashName("sound2f0", p.sound2f0);
            addXHashName("sound300", p.sound300);
            addXHashName("sound310", p.sound310);
            addXHashName("sound320", p.sound320);
            addXHashName("sound330", p.sound330);
            addXHashName("sound340", p.sound340);
            addXHashName("sound350", p.sound350);
            addXHashName("sound360", p.sound360);
            addXHashName("sound370", p.sound370);
            addXHashName("sound380", p.sound380);
            addXHashName("gadgetreadysound", p.gadgetreadysound);
            addXHashName("var_1f7ccc3b", p.var_1f7ccc3b);
            addXHashName("sound3b0", p.sound3b0);
            addXHashName("sound3c0", p.sound3c0);
            addXHashName("sound3d0", p.sound3d0);
            addXHashName("sound3e0", p.sound3e0);
            addXHashName("sound3f0", p.sound3f0);
            addXHashName("sound400", p.sound400);
            addXHashName("sound410", p.sound410);
            addXHashName("gadgetreadysoundplayer", p.gadgetreadysoundplayer);
            addXHashName("var_fb22040b", p.var_fb22040b);
            addXHashName("sound440", p.sound440);
            addXHashName("sound450", p.sound450);
            addXHashName("sound460", p.sound460);
            addXHashName("sound470", p.sound470);
            addXHashName("sound480", p.sound480);
            addXHashName("sound490", p.sound490);
            addXHashName("sound4a0", p.sound4a0);
            addXHashName("sound4b0", p.sound4b0);
            addXHashName("sound4c0", p.sound4c0);
            addXHashName("sound4d0", p.sound4d0);
            addXHashName("sound4e0", p.sound4e0);
            addXHashName("sound4f0", p.sound4f0);
            addXHashName("sound500", p.sound500);
            addXHashName("sound510", p.sound510);
            addXHashName("sound520", p.sound520);
            addXHashName("sound530", p.sound530);
            addXHashName("sound540", p.sound540);
            addXHashName("sound550", p.sound550);
            addXHashName("sound560", p.sound560);
            addXHashName("sound570", p.sound570);
            addXHashName("sound580", p.sound580);
            addXHashName("sound590", p.sound590);
            addXHashName("sound5a0", p.sound5a0);
            addXHashName("sound5b0", p.sound5b0);
            addXHashName("sound5c0", p.sound5c0);
            addXHashName("sound5d0", p.sound5d0);
            addXHashName("sound5e0", p.sound5e0);
            addXHashName("sound5f0", p.sound5f0);
            addXHashName("sound600", p.sound600);
            addXHashName("sound610", p.sound610);
            addXHashName("sound620", p.sound620);
            addXHashName("sound630", p.sound630);
            addXHashName("sound640", p.sound640);
            addXHashName("sound650", p.sound650);
            addXHashName("sound660", p.sound660);
            addXHashName("sound670", p.sound670);
            addXHashName("sound680", p.sound680);
            addXHashName("sound690", p.sound690);
            addXHashName("sound6a0", p.sound6a0);
            addXHashName("sound6b0", p.sound6b0);
            addXHashName("sound6c0", p.sound6c0);
            addXHashName("lockonseekersearchsound", p.lockonseekersearchsound);
            addXHashName("lockonseekerlockedsound", p.lockonseekerlockedsound);
            addXHashName("lockontargetlockedsound", p.lockontargetlockedsound);
            addXHashName("lockontargetfiredonsound", p.lockontargetfiredonsound);
            addXHashName("sound710", p.sound710);
            addXHashName("sound720", p.sound720);
            addXHashName("sound730", p.sound730);
            addXHashName("sound740", p.sound740);
            addXHashName("var_8a03df2b", p.var_8a03df2b);
            addXHashName("var_2f3ca476", p.var_2f3ca476);
            addXHashName("var_5c29f743", p.var_5c29f743);
            addXHashName("projexplosionsound", p.projexplosionsound);
            addXHashName("projexplosionsoundplayer", p.projexplosionsoundplayer);
            addXHashName("projsmokestartsound", p.projsmokestartsound);
            addXHashName("projsmokeloopsound", p.projsmokeloopsound);
            addXHashName("projsmokeendsound", p.projsmokeendsound);
            addXHashName("sound7d0", p.sound7d0);
            addXHashName("sound7e0", p.sound7e0);
            addXHashName("sound7f0", p.sound7f0);
            addXHashName("sound800", p.sound800);
            addXHashName("sound810", p.sound810);
            addXHashName("sound820", p.sound820);
            addXHashName("sound830", p.sound830);
            addXHashName("sound840", p.sound840);
            addXHashName("sound850", p.sound850);
            addXHashName("sound860", p.sound860);
            addXHashName("hitsound", p.hitsound);

            addXHashName("unk880", p.unk880);


            addPtrName("customsettings", p.customsettings, 0);
            addPtrName("shrapnelsettings", p.shrapnelsettings, 0);
            addPtrName("var_2e4a8800", p.var_2e4a8800, 0);
            addPtrName("var_8456d4d", p.var_8456d4d, 0);
            
            if (p.properties) {
                utils::Padding(out, 1) << "\"properties\": {\n";
                utils::Padding(out, 2) << "\"var_f56ac2bd\": \"" << ReadTmpStr(proc, props.var_f56ac2bd) << "\",\n";
                utils::Padding(out, 2) << "\"spawninfluencer\": \"" << ReadTmpStr(proc, props.spawninfluencer) << "\",\n";
                utils::Padding(out, 2) << "\"var_77b46a8c\": \"#" << hashutils::ExtractTmp("hash", props.var_77b46a8c.name) << "\",\n";
                if (props.hackertriggerorigintag) {
                    utils::Padding(out, 2) << "\"hackertriggerorigintag\": \"" << ReadMTString(proc, props.hackertriggerorigintag) << "\",\n";
                }
                utils::Padding(out, 2) << "\"weapClass\": \"" << WeapClassName(props.weapClass) << "\",\n";
                utils::Padding(out, 2) << "\"gadgetType\": \"" << GadgetTypeName(props.gadget_type) << "\",\n";
                utils::Padding(out, 2) << "\"guidedMissileType\": \"" << GuidedMissileTypeName(props.guidedMissileType) << "\",\n";
                utils::Padding(out, 2) << "\"inventoryType\": \"" << WeapInventoryTypeName(props.inventoryType) << "\",\n";
                utils::Padding(out, 2) << "\"weapType\": \"" << WeapTypeName(props.weapType) << "\",\n";
                utils::Padding(out, 2) << "\"offhandClass\": \"" << OffhandClassName(props.offhandClass) << "\",\n";
                utils::Padding(out, 2) << "\"offhandSlot\": \"" << OffhandSlotName(props.offhandSlot) << "\",\n";
                utils::Padding(out, 2) << "\"lockontype\": \"" << LockOnTypeName(props.lockontype) << "\",\n";
                utils::Padding(out, 2) << "\"projExplosionType\": \"" << ProjExplosionTypeName(props.projExplosionType) << "\",\n";
                utils::Padding(out, 2) << "\"bAltWeaponDisableSwitching\": " << (props.bAltWeaponDisableSwitching ? "true" : "false") << ",\n";
                utils::Padding(out, 2) << "\"bDieOnRespawn\": " << (props.bDieOnRespawn ? "true" : "false") << ",\n";
                utils::Padding(out, 2) << "\"isdualwield\": " << (props.isdualwield ? "true" : "false") << ",\n";
                utils::Padding(out, 2) << "\"istimeddetonation\": " << (props.istimeddetonation ? "true" : "false") << ",\n";
                utils::Padding(out, 2) << "\"unlimitedAmmo\": " << (props.unlimitedAmmo ? "true" : "false") << ",\n";
                utils::Padding(out, 2) << "\"altoffhand\": " << std::dec << (int)props.altoffhand << ",\n";
                utils::Padding(out, 2) << "\"ammoCountClipRelative\": " << std::dec << (int)props.ammoCountClipRelative << ",\n";
                utils::Padding(out, 2) << "\"ammoregen\": " << std::dec << (int)props.ammoregen << ",\n";
                utils::Padding(out, 2) << "\"anyplayercanretrieve\": " << std::dec << (int)props.anyplayercanretrieve << ",\n";
                utils::Padding(out, 2) << "\"bIsHybridWeapon\": " << std::dec << (int)props.bIsHybridWeapon << ",\n";
                utils::Padding(out, 2) << "\"blocksprone\": " << std::dec << (int)props.blocksprone << ",\n";
                utils::Padding(out, 2) << "\"bulletimpactexplode\": " << std::dec << (int)props.bulletimpactexplode << ",\n";
                utils::Padding(out, 2) << "\"canuseunderwater\": " << std::dec << (int)props.canuseunderwater << ",\n";
                utils::Padding(out, 2) << "\"craftitem\": " << std::dec << (int)props.craftitem << ",\n";
                utils::Padding(out, 2) << "\"damagealwayskillsplayer\": " << std::dec << (int)props.damagealwayskillsplayer << ",\n";
                utils::Padding(out, 2) << "\"decoy\": " << std::dec << (int)props.decoy << ",\n";
                utils::Padding(out, 2) << "\"deployable\": " << std::dec << (int)props.deployable << ",\n";
                utils::Padding(out, 2) << "\"destroyablebytrophysystem\": " << std::dec << (int)props.destroyablebytrophysystem << ",\n";
                utils::Padding(out, 2) << "\"destroysequipment\": " << std::dec << (int)props.destroysequipment << ",\n";
                utils::Padding(out, 2) << "\"disabledeploy\": " << std::dec << (int)props.disabledeploy << ",\n";
                utils::Padding(out, 2) << "\"disallowatmatchstart\": " << std::dec << (int)props.disallowatmatchstart << ",\n";
                utils::Padding(out, 2) << "\"doannihilate\": " << std::dec << (int)props.doannihilate << ",\n";
                utils::Padding(out, 2) << "\"doblowback\": " << std::dec << (int)props.doblowback << ",\n";
                utils::Padding(out, 2) << "\"dodamagefeedback\": " << std::dec << (int)props.dodamagefeedback << ",\n";
                utils::Padding(out, 2) << "\"doempdestroyfx\": " << std::dec << (int)props.doempdestroyfx << ",\n";
                utils::Padding(out, 2) << "\"doesfiredamage\": " << std::dec << (int)props.doesfiredamage << ",\n";
                utils::Padding(out, 2) << "\"dogibbing\": " << std::dec << (int)props.dogibbing << ",\n";
                utils::Padding(out, 2) << "\"dogibbingonmelee\": " << std::dec << (int)props.dogibbingonmelee << ",\n";
                utils::Padding(out, 2) << "\"dohackedstats\": " << std::dec << (int)props.dohackedstats << ",\n";
                utils::Padding(out, 2) << "\"donotdamageowner\": " << std::dec << (int)props.donotdamageowner << ",\n";
                utils::Padding(out, 2) << "\"dostun\": " << std::dec << (int)props.dostun << ",\n";
                utils::Padding(out, 2) << "\"forcedamagehitlocation\": " << std::dec << (int)props.forcedamagehitlocation << ",\n";
                utils::Padding(out, 2) << "\"gadget_heroversion_2_0\": " << std::dec << (int)props.gadget_heroversion_2_0 << ",\n";
                utils::Padding(out, 2) << "\"gadget_power_consume_on_ammo_use\": " << std::dec << (int)props.gadget_power_consume_on_ammo_use << ",\n";
                utils::Padding(out, 2) << "\"gadget_power_reset_on_class_change\": " << std::dec << (int)props.gadget_power_reset_on_class_change << ",\n";
                utils::Padding(out, 2) << "\"gadget_power_reset_on_round_switch\": " << std::dec << (int)props.gadget_power_reset_on_round_switch << ",\n";
                utils::Padding(out, 2) << "\"gadget_power_reset_on_spawn\": " << std::dec << (int)props.gadget_power_reset_on_spawn << ",\n";
                utils::Padding(out, 2) << "\"gadget_power_reset_on_team_change\": " << std::dec << (int)props.gadget_power_reset_on_team_change << ",\n";
                utils::Padding(out, 2) << "\"gadget_powergainscoreignoreself\": " << std::dec << (int)props.gadget_powergainscoreignoreself << ",\n";
                utils::Padding(out, 2) << "\"gadget_powergainscoreignorewhenactive\": " << std::dec << (int)props.gadget_powergainscoreignorewhenactive << ",\n";
                utils::Padding(out, 2) << "\"gadget_turnoff_onempjammed\": " << std::dec << (int)props.gadget_turnoff_onempjammed << ",\n";
                utils::Padding(out, 2) << "\"grappleweapon\": " << std::dec << (int)props.grappleweapon << ",\n";
                utils::Padding(out, 2) << "\"ignoresflakjacket\": " << std::dec << (int)props.ignoresflakjacket << ",\n";
                utils::Padding(out, 2) << "\"ignoreslightarmor\": " << std::dec << (int)props.ignoreslightarmor << ",\n";
                utils::Padding(out, 2) << "\"ignorespowerarmor\": " << std::dec << (int)props.ignorespowerarmor << ",\n";
                utils::Padding(out, 2) << "\"ignoreteamkills\": " << std::dec << (int)props.ignoreteamkills << ",\n";
                utils::Padding(out, 2) << "\"isaikillstreakdamage\": " << std::dec << (int)props.isaikillstreakdamage << ",\n";
                utils::Padding(out, 2) << "\"isballisticknife\": " << std::dec << (int)props.isballisticknife << ",\n";
                utils::Padding(out, 2) << "\"isboltaction\": " << std::dec << (int)props.isboltaction << ",\n";
                utils::Padding(out, 2) << "\"iscarriedkillstreak\": " << std::dec << (int)props.iscarriedkillstreak << ",\n";
                utils::Padding(out, 2) << "\"iscliponly\": " << std::dec << (int)props.iscliponly << ",\n";
                utils::Padding(out, 2) << "\"isemp\": " << std::dec << (int)props.isemp << ",\n";
                utils::Padding(out, 2) << "\"isflash\": " << std::dec << (int)props.isflash << ",\n";
                utils::Padding(out, 2) << "\"isflourishweapon\": " << std::dec << (int)props.isflourishweapon << ",\n";
                utils::Padding(out, 2) << "\"isgameplayweapon\": " << std::dec << (int)props.isgameplayweapon << ",\n";
                utils::Padding(out, 2) << "\"ishacktoolweapon\": " << std::dec << (int)props.ishacktoolweapon << ",\n";
                utils::Padding(out, 2) << "\"isnotdroppable\": " << std::dec << (int)props.isnotdroppable << ",\n";
                utils::Padding(out, 2) << "\"isperkbottle\": " << std::dec << (int)props.isperkbottle << ",\n";
                utils::Padding(out, 2) << "\"isscavengable\": " << std::dec << (int)props.isscavengable << ",\n";
                utils::Padding(out, 2) << "\"issignatureweapon\": " << std::dec << (int)props.issignatureweapon << ",\n";
                utils::Padding(out, 2) << "\"issniperweapon\": " << std::dec << (int)props.issniperweapon << ",\n";
                utils::Padding(out, 2) << "\"isstun\": " << std::dec << (int)props.isstun << ",\n";
                utils::Padding(out, 2) << "\"issupplydropweapon\": " << std::dec << (int)props.issupplydropweapon << ",\n";
                utils::Padding(out, 2) << "\"istacticalinsertion\": " << std::dec << (int)props.istacticalinsertion << ",\n";
                utils::Padding(out, 2) << "\"isthrowback\": " << std::dec << (int)props.isthrowback << ",\n";
                utils::Padding(out, 2) << "\"isvaluable\": " << std::dec << (int)props.isvaluable << ",\n";
                utils::Padding(out, 2) << "\"meleeignoreslightarmor\": " << std::dec << (int)props.meleeignoreslightarmor << ",\n";
                utils::Padding(out, 2) << "\"mountable\": " << std::dec << (int)props.mountable << ",\n";
                utils::Padding(out, 2) << "\"noadslockoncheck\": " << std::dec << (int)props.noadslockoncheck << ",\n";
                utils::Padding(out, 2) << "\"nohitmarker\": " << std::dec << (int)props.nohitmarker << ",\n";
                utils::Padding(out, 2) << "\"nonstowedweapon\": " << std::dec << (int)props.nonstowedweapon << ",\n";
                utils::Padding(out, 2) << "\"notkillstreak\": " << std::dec << (int)props.notkillstreak << ",\n";
                utils::Padding(out, 2) << "\"requirelockontofire\": " << std::dec << (int)props.requirelockontofire << ",\n";
                utils::Padding(out, 2) << "\"setusedstat\": " << std::dec << (int)props.setusedstat << ",\n";
                utils::Padding(out, 2) << "\"shownenemyequip\": " << std::dec << (int)props.shownenemyequip << ",\n";
                utils::Padding(out, 2) << "\"shownenemyexplo\": " << std::dec << (int)props.shownenemyexplo << ",\n";
                utils::Padding(out, 2) << "\"shownretrievable\": " << std::dec << (int)props.shownretrievable << ",\n";
                utils::Padding(out, 2) << "\"skipbattlechatterkill\": " << std::dec << (int)props.skipbattlechatterkill << ",\n";
                utils::Padding(out, 2) << "\"skipbattlechatterreload\": " << std::dec << (int)props.skipbattlechatterreload << ",\n";
                utils::Padding(out, 2) << "\"skiplowammovox\": " << std::dec << (int)props.skiplowammovox << ",\n";
                utils::Padding(out, 2) << "\"specialpain\": " << std::dec << (int)props.specialpain << ",\n";
                utils::Padding(out, 2) << "\"var_130391b9\": " << std::dec << (int)props.var_130391b9 << ",\n";
                utils::Padding(out, 2) << "\"var_18608bfe\": " << std::dec << (int)props.var_18608bfe << ",\n";
                utils::Padding(out, 2) << "\"var_251796e3\": " << std::dec << (int)props.var_251796e3 << ",\n";
                utils::Padding(out, 2) << "\"var_256488f1\": " << std::dec << (int)props.var_256488f1 << ",\n";
                utils::Padding(out, 2) << "\"var_28bb357c\": " << std::dec << (int)props.var_28bb357c << ",\n";
                utils::Padding(out, 2) << "\"var_29d24e37\": " << std::dec << (int)props.var_29d24e37 << ",\n";
                utils::Padding(out, 2) << "\"var_2cb95b88\": " << std::dec << (int)props.var_2cb95b88 << ",\n";
                utils::Padding(out, 2) << "\"var_3344c07e\": " << std::dec << (int)props.var_3344c07e << ",\n";
                utils::Padding(out, 2) << "\"var_337fc1cf\": " << std::dec << (int)props.var_337fc1cf << ",\n";
                utils::Padding(out, 2) << "\"var_33d50507\": " << std::dec << (int)props.var_33d50507 << ",\n";
                utils::Padding(out, 2) << "\"var_5801b768\": " << std::dec << (int)props.var_5801b768 << ",\n";
                utils::Padding(out, 2) << "\"var_58543a1c\": " << std::dec << (int)props.var_58543a1c << ",\n";
                utils::Padding(out, 2) << "\"var_6262fd11\": " << std::dec << (int)props.var_6262fd11 << ",\n";
                utils::Padding(out, 2) << "\"var_6f12adba\": " << std::dec << (int)props.var_6f12adba << ",\n";
                utils::Padding(out, 2) << "\"var_70b09d5b\": " << std::dec << (int)props.var_70b09d5b << ",\n";
                utils::Padding(out, 2) << "\"var_76ce72e8\": " << std::dec << (int)props.var_76ce72e8 << ",\n";
                utils::Padding(out, 2) << "\"var_775d2aad\": " << std::dec << (int)props.var_775d2aad << ",\n";
                utils::Padding(out, 2) << "\"var_7b5016a7\": " << std::dec << (int)props.var_7b5016a7 << ",\n";
                utils::Padding(out, 2) << "\"var_8025ffca\": " << std::dec << (int)props.var_8025ffca << ",\n";
                utils::Padding(out, 2) << "\"var_8032088a\": " << std::dec << (int)props.var_8032088a << ",\n";
                utils::Padding(out, 2) << "\"var_8072cf0b\": " << std::dec << (int)props.var_8072cf0b << ",\n";
                utils::Padding(out, 2) << "\"var_9111ccc0\": " << std::dec << (int)props.var_9111ccc0 << ",\n";
                utils::Padding(out, 2) << "\"var_9fffdcee\": " << std::dec << (int)props.var_9fffdcee << ",\n";
                utils::Padding(out, 2) << "\"var_b8a85edd\": " << std::dec << (int)props.var_b8a85edd << ",\n";
                utils::Padding(out, 2) << "\"var_ba335ef\": " << std::dec << (int)props.var_ba335ef << ",\n";
                utils::Padding(out, 2) << "\"var_bec5136b\": " << std::dec << (int)props.var_bec5136b << ",\n";
                utils::Padding(out, 2) << "\"var_ca947940\": " << std::dec << (int)props.var_ca947940 << ",\n";
                utils::Padding(out, 2) << "\"var_ce34bb7e\": " << std::dec << (int)props.var_ce34bb7e << ",\n";
                utils::Padding(out, 2) << "\"var_d69ee9ed\": " << std::dec << (int)props.var_d69ee9ed << ",\n";
                utils::Padding(out, 2) << "\"var_dbbd4cec\": " << std::dec << (int)props.var_dbbd4cec << ",\n";
                utils::Padding(out, 2) << "\"var_ddaa57f2\": " << std::dec << (int)props.var_ddaa57f2 << ",\n";
                utils::Padding(out, 2) << "\"var_e0d42861\": " << std::dec << (int)props.var_e0d42861 << ",\n";
                utils::Padding(out, 2) << "\"var_f076a292\": " << std::dec << (int)props.var_f076a292 << ",\n";
                utils::Padding(out, 2) << "\"var_f10d73e1\": " << std::dec << (int)props.var_f10d73e1 << ",\n";
                utils::Padding(out, 2) << "\"var_f23e9d19\": " << std::dec << (int)props.var_f23e9d19 << ",\n";
                utils::Padding(out, 2) << "\"var_f6dea63a\": " << std::dec << (int)props.var_f6dea63a << ",\n";
                utils::Padding(out, 2) << "\"var_fab9617b\": " << std::dec << (int)props.var_fab9617b << ",\n";
                utils::Padding(out, 2) << "\"var_ff0b00ba\": " << std::dec << (int)props.var_ff0b00ba << ",\n";
                utils::Padding(out, 2) << "\"adsspread\": " << std::dec << props.adsspread << ",\n";
                utils::Padding(out, 2) << "\"aimeleerange\": " << std::dec << props.aimeleerange << ",\n";
                utils::Padding(out, 2) << "\"damagetoownerscalar\": " << std::dec << props.damagetoownerscalar << ",\n";
                utils::Padding(out, 2) << "\"fightdist\": " << std::dec << props.fightdist << ",\n";
                utils::Padding(out, 2) << "\"gadget_power_usage_rate\": " << std::dec << props.gadget_power_usage_rate << ",\n";
                utils::Padding(out, 2) << "\"gadget_powergainscorefactor\": " << std::dec << props.gadget_powergainscorefactor << ",\n";
                utils::Padding(out, 2) << "\"gadget_powermovespeed\": " << std::dec << props.gadget_powermovespeed << ",\n";
                utils::Padding(out, 2) << "\"gadget_powerofflossondamage\": " << std::dec << props.gadget_powerofflossondamage << ",\n";
                utils::Padding(out, 2) << "\"gadget_poweronlossondamage\": " << std::dec << props.gadget_poweronlossondamage << ",\n";
                utils::Padding(out, 2) << "\"gadget_powerreplenishfactor\": " << std::dec << props.gadget_powerreplenishfactor << ",\n";
                utils::Padding(out, 2) << "\"gadget_shockfield_damage\": " << std::dec << props.gadget_shockfield_damage << ",\n";
                utils::Padding(out, 2) << "\"gadget_shockfield_radius\": " << std::dec << props.gadget_shockfield_radius << ",\n";
                utils::Padding(out, 2) << "\"lifetime\": " << std::dec << props.lifetime << ",\n";
                utils::Padding(out, 2) << "\"lockonanglehorizontal\": " << std::dec << props.lockonanglehorizontal << ",\n";
                utils::Padding(out, 2) << "\"lockonanglevertical\": " << std::dec << props.lockonanglevertical << ",\n";
                utils::Padding(out, 2) << "\"lockonlossanglehorizontal\": " << std::dec << props.lockonlossanglehorizontal << ",\n";
                utils::Padding(out, 2) << "\"lockonlossanglevertical\": " << std::dec << props.lockonlossanglevertical << ",\n";
                utils::Padding(out, 2) << "\"maxdist\": " << std::dec << props.maxdist << ",\n";
                utils::Padding(out, 2) << "\"maxgibdistance\": " << std::dec << props.maxgibdistance << ",\n";
                utils::Padding(out, 2) << "\"meleelungerange\": " << std::dec << props.meleelungerange << ",\n";
                utils::Padding(out, 2) << "\"sprintboostduration\": " << std::dec << props.sprintboostduration << ",\n";
                utils::Padding(out, 2) << "\"sprintboostradius\": " << std::dec << props.sprintboostradius << ",\n";
                utils::Padding(out, 2) << "\"var_16e90b80\": " << std::dec << props.var_16e90b80 << ",\n";
                utils::Padding(out, 2) << "\"var_19f920eb\": " << std::dec << props.var_19f920eb << ",\n";
                utils::Padding(out, 2) << "\"var_1f13c7f1\": " << std::dec << props.var_1f13c7f1 << ",\n";
                utils::Padding(out, 2) << "\"var_27c94b15\": " << std::dec << props.var_27c94b15 << ",\n";
                utils::Padding(out, 2) << "\"var_2cf65b96\": " << std::dec << props.var_2cf65b96 << ",\n";
                utils::Padding(out, 2) << "\"var_367c47fc\": " << std::dec << props.var_367c47fc << ",\n";
                utils::Padding(out, 2) << "\"var_36c76157\": " << std::dec << props.var_36c76157 << ",\n";
                utils::Padding(out, 2) << "\"var_3a00e7eb\": " << std::dec << props.var_3a00e7eb << ",\n";
                utils::Padding(out, 2) << "\"var_40ffe7d2\": " << std::dec << props.var_40ffe7d2 << ",\n";
                utils::Padding(out, 2) << "\"var_416021d8\": " << std::dec << props.var_416021d8 << ",\n";
                utils::Padding(out, 2) << "\"var_5be370e9\": " << std::dec << props.var_5be370e9 << ",\n";
                utils::Padding(out, 2) << "\"var_66103577\": " << std::dec << props.var_66103577 << ",\n";
                utils::Padding(out, 2) << "\"var_6844746b\": " << std::dec << props.var_6844746b << ",\n";
                utils::Padding(out, 2) << "\"var_6cb9946f\": " << std::dec << props.var_6cb9946f << ",\n";
                utils::Padding(out, 2) << "\"var_7872b3a\": " << std::dec << props.var_7872b3a << ",\n";
                utils::Padding(out, 2) << "\"var_9aa1ef19\": " << std::dec << props.var_9aa1ef19 << ",\n";
                utils::Padding(out, 2) << "\"var_a2d7b97c\": " << std::dec << props.var_a2d7b97c << ",\n";
                utils::Padding(out, 2) << "\"var_abb570e0\": " << std::dec << props.var_abb570e0 << ",\n";
                utils::Padding(out, 2) << "\"var_c15c6b39\": " << std::dec << props.var_c15c6b39 << ",\n";
                utils::Padding(out, 2) << "\"var_c94f007c\": " << std::dec << props.var_c94f007c << ",\n";
                utils::Padding(out, 2) << "\"var_cb3d0f65\": " << std::dec << props.var_cb3d0f65 << ",\n";
                utils::Padding(out, 2) << "\"var_ccebc40\": " << std::dec << props.var_ccebc40 << ",\n";
                utils::Padding(out, 2) << "\"var_cd539cb2\": " << std::dec << props.var_cd539cb2 << ",\n";
                utils::Padding(out, 2) << "\"var_deb0b2fe\": " << std::dec << props.var_deb0b2fe << ",\n";
                utils::Padding(out, 2) << "\"var_e1811962\": " << std::dec << props.var_e1811962 << ",\n";
                utils::Padding(out, 2) << "\"var_e2b40cd5\": " << std::dec << props.var_e2b40cd5 << ",\n";
                utils::Padding(out, 2) << "\"var_e4d4fa7e\": " << std::dec << props.var_e4d4fa7e << ",\n";
                utils::Padding(out, 2) << "\"var_f9eec1ec\": " << std::dec << props.var_f9eec1ec << ",\n";
                utils::Padding(out, 2) << "\"vehicleprojectiledamagescalar\": " << std::dec << props.vehicleprojectiledamagescalar << ",\n";
                utils::Padding(out, 2) << "\"vehicleprojectilesplashdamagescalar\": " << std::dec << props.vehicleprojectilesplashdamagescalar << ",\n";
                utils::Padding(out, 2) << "\"aiFuseTime\": " << std::dec << props.aiFuseTime << ",\n";
                utils::Padding(out, 2) << "\"ammoindex\": " << std::dec << props.ammoindex << ",\n";
                utils::Padding(out, 2) << "\"burndamage\": " << std::dec << props.burndamage << ",\n";
                utils::Padding(out, 2) << "\"burndamageinterval\": " << std::dec << props.burndamageinterval << ",\n";
                utils::Padding(out, 2) << "\"burnduration\": " << std::dec << props.burnduration << ",\n";
                utils::Padding(out, 2) << "\"burstcount\": " << std::dec << props.burstcount << ",\n";
                utils::Padding(out, 2) << "\"explosioninnerdamage\": " << std::dec << props.explosioninnerdamage << ",\n";
                utils::Padding(out, 2) << "\"explosionouterdamage\": " << std::dec << props.explosionouterdamage << ",\n";
                utils::Padding(out, 2) << "\"fuellife\": " << std::dec << props.fuellife << ",\n";
                utils::Padding(out, 2) << "\"gadget_flickerondamage\": " << std::dec << props.gadget_flickerondamage << ",\n";
                utils::Padding(out, 2) << "\"gadget_flickeronpowerloss\": " << std::dec << props.gadget_flickeronpowerloss << ",\n";
                utils::Padding(out, 2) << "\"gadget_max_hitpoints\": " << std::dec << props.gadget_max_hitpoints << ",\n";
                utils::Padding(out, 2) << "\"gadget_power_round_end_active_penalty\": " << std::dec << props.gadget_power_round_end_active_penalty << ",\n";
                utils::Padding(out, 2) << "\"gadget_powergainonretrieve\": " << std::dec << props.gadget_powergainonretrieve << ",\n";
                utils::Padding(out, 2) << "\"gadget_powermoveloss\": " << std::dec << props.gadget_powermoveloss << ",\n";
                utils::Padding(out, 2) << "\"gadget_powersprintloss\": " << std::dec << props.gadget_powersprintloss << ",\n";
                utils::Padding(out, 2) << "\"gadget_pulse_duration\": " << std::dec << props.gadget_pulse_duration << ",\n";
                utils::Padding(out, 2) << "\"gadget_pulse_max_range\": " << std::dec << props.gadget_pulse_max_range << ",\n";
                utils::Padding(out, 2) << "\"heal\": " << std::dec << props.heal << ",\n";
                utils::Padding(out, 2) << "\"lockonlossradius\": " << std::dec << props.lockonlossradius << ",\n";
                utils::Padding(out, 2) << "\"lockonminrange\": " << std::dec << props.lockonminrange << ",\n";
                utils::Padding(out, 2) << "\"lockonradius\": " << std::dec << props.lockonradius << ",\n";
                utils::Padding(out, 2) << "\"lockonscreenradius\": " << std::dec << props.lockonscreenradius << ",\n";
                utils::Padding(out, 2) << "\"maxheal\": " << std::dec << props.maxheal << ",\n";
                utils::Padding(out, 2) << "\"maxinstancesallowed\": " << std::dec << props.maxinstancesallowed << ",\n";
                utils::Padding(out, 2) << "\"multidetonation\": " << std::dec << props.multidetonation << ",\n";
                utils::Padding(out, 2) << "\"playerdamage\": " << std::dec << props.playerdamage << ",\n";
                utils::Padding(out, 2) << "\"var_1123a989\": " << std::dec << props.var_1123a989 << ",\n";
                utils::Padding(out, 2) << "\"var_1c0e3cb7\": " << std::dec << props.var_1c0e3cb7 << ",\n";
                utils::Padding(out, 2) << "\"var_1e89f40\": " << std::dec << props.var_1e89f40 << ",\n";
                utils::Padding(out, 2) << "\"var_227c90e1\": " << std::dec << props.var_227c90e1 << ",\n";
                utils::Padding(out, 2) << "\"var_42a3cafc\": " << std::dec << props.var_42a3cafc << ",\n";
                utils::Padding(out, 2) << "\"var_4465ef1e\": " << std::dec << props.var_4465ef1e << ",\n";
                utils::Padding(out, 2) << "\"var_44c79c09\": " << std::dec << props.var_44c79c09 << ",\n";
                utils::Padding(out, 2) << "\"var_4941de5\": " << std::dec << props.var_4941de5 << ",\n";
                utils::Padding(out, 2) << "\"var_4d88a1ff\": " << std::dec << props.var_4d88a1ff << ",\n";
                utils::Padding(out, 2) << "\"var_5ac2e7a4\": " << std::dec << props.var_5ac2e7a4 << ",\n";
                utils::Padding(out, 2) << "\"var_5af80bb6\": " << std::dec << props.var_5af80bb6 << ",\n";
                utils::Padding(out, 2) << "\"var_60563796\": " << std::dec << props.var_60563796 << ",\n";
                utils::Padding(out, 2) << "\"var_61d29b2f\": " << std::dec << props.var_61d29b2f << ",\n";
                utils::Padding(out, 2) << "\"var_6821283d\": " << std::dec << props.var_6821283d << ",\n";
                utils::Padding(out, 2) << "\"var_68a0f062\": " << std::dec << props.var_68a0f062 << ",\n";
                utils::Padding(out, 2) << "\"var_6a864cad\": " << std::dec << props.var_6a864cad << ",\n";
                utils::Padding(out, 2) << "\"var_76127e14\": " << std::dec << props.var_76127e14 << ",\n";
                utils::Padding(out, 2) << "\"var_7a93ed37\": " << std::dec << props.var_7a93ed37 << ",\n";
                utils::Padding(out, 2) << "\"var_8134b209\": " << std::dec << props.var_8134b209 << ",\n";
                utils::Padding(out, 2) << "\"var_81683931\": " << std::dec << props.var_81683931 << ",\n";
                utils::Padding(out, 2) << "\"var_829de2ac\": " << std::dec << props.var_829de2ac << ",\n";
                utils::Padding(out, 2) << "\"var_849af6b4\": " << std::dec << props.var_849af6b4 << ",\n";
                utils::Padding(out, 2) << "\"var_8e0b0827\": " << std::dec << props.var_8e0b0827 << ",\n";
                utils::Padding(out, 2) << "\"var_95d8fabf\": " << std::dec << props.var_95d8fabf << ",\n";
                utils::Padding(out, 2) << "\"var_98333ae\": " << std::dec << props.var_98333ae << ",\n";
                utils::Padding(out, 2) << "\"var_9d776ba6\": " << std::dec << props.var_9d776ba6 << ",\n";
                utils::Padding(out, 2) << "\"var_a8bd8bb2\": " << std::dec << props.var_a8bd8bb2 << ",\n";
                utils::Padding(out, 2) << "\"var_ab300840\": " << std::dec << props.var_ab300840 << ",\n";
                utils::Padding(out, 2) << "\"var_ac36c1db\": " << std::dec << props.var_ac36c1db << ",\n";
                utils::Padding(out, 2) << "\"var_b9951041\": " << std::dec << props.var_b9951041 << ",\n";
                utils::Padding(out, 2) << "\"var_c264efc6\": " << std::dec << props.var_c264efc6 << ",\n";
                utils::Padding(out, 2) << "\"var_c4aae0fa\": " << std::dec << props.var_c4aae0fa << ",\n";
                utils::Padding(out, 2) << "\"var_c4d4d2a9\": " << std::dec << props.var_c4d4d2a9 << ",\n";
                utils::Padding(out, 2) << "\"var_cddb5cd0\": " << std::dec << props.var_cddb5cd0 << ",\n";
                utils::Padding(out, 2) << "\"var_d911d477\": " << std::dec << props.var_d911d477 << ",\n";
                utils::Padding(out, 2) << "\"var_df0f9ce9\": " << std::dec << props.var_df0f9ce9 << ",\n";
                utils::Padding(out, 2) << "\"var_df381b5d\": " << std::dec << props.var_df381b5d << ",\n";
                utils::Padding(out, 2) << "\"var_e4109b63\": " << std::dec << props.var_e4109b63 << ",\n";
                utils::Padding(out, 2) << "\"var_e5db3b95\": " << std::dec << props.var_e5db3b95 << ",\n";
                utils::Padding(out, 2) << "\"var_ec2cbce2\": " << std::dec << props.var_ec2cbce2 << ",\n";
                utils::Padding(out, 2) << "\"var_f7e67f28\": " << std::dec << props.var_f7e67f28 << ",\n";
                utils::Padding(out, 2) << "\"iClipSize\": " << std::dec << props.iClipSize << ",\n";
                utils::Padding(out, 2) << "\"iMaxAmmo\": " << std::dec << props.iMaxAmmo << ",\n";
                utils::Padding(out, 2) << "\"iStartAmmo\": " << std::dec << props.iStartAmmo << ",\n";
                utils::Padding(out, 2) << "\"meleechargetime\": " << std::dec << props.meleechargetime << ",\n";
                utils::Padding(out, 2) << "\"meleedamage\": " << std::dec << props.meleedamage << ",\n";
                utils::Padding(out, 2) << "\"meleepowertime\": " << std::dec << props.meleepowertime << ",\n";
                utils::Padding(out, 2) << "\"meleepowertimeleft\": " << std::dec << props.meleepowertimeleft << ",\n";
                utils::Padding(out, 2) << "\"meleetime\": " << std::dec << props.meleetime << "\n";

                
                utils::Padding(out, 1) << "}\n";
            }
            out
                << "}\n"
                ;



            tool::pool::WriteHex(out, entry.pool + sizeof(pool[0]) * i, (byte*)&p, sizeof(p), proc);
            

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_MAPTABLE_LIST)) {
        auto pool = std::make_unique<MapTableList[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

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
                std::max(
                    std::max(p.list_campaign.count, p.list_multiplayer.count),
                    std::max(p.list_warzone.count, p.list_zombies.count)
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
    }
    if (ShouldHandle(ASSET_TYPE_PLAYLISTS)) {
        struct PlayLists { // 0x50
            uint32_t unk0; // this thing is passed in a checksum32
            uint32_t unk4;
            uint64_t unk8;
            Hash name;
            uintptr_t maps; // PlaylistMap**
            uint64_t maps_count;
            uintptr_t gametypes; // PlaylistGametype*
            uint64_t gametypes_count;
            uintptr_t categories; //PlaylistData*
            uint64_t categories_count;
        };

        struct PlaylistData { // unk40 // 0x50
            Hash name;
            int32_t lobbyMainMode;
            int32_t unk14;
            Hash description;
            uint64_t icon;
            uint64_t unk30;
            uint32_t unk38;
            bool hidden;
            byte unk3D;
            byte unk3E;
            byte unk3F;
            uintptr_t unk40;
            int32_t unk40_count;
            int32_t unk4c;
        };
        struct PlaylistEntry {
            Hash name;
            eModes mainMode;
            int unk14;
            uint64_t unk18;
            Hash unique_name;
            uint64_t unk30;
            uintptr_t image; // GfxImageHandle
            uintptr_t imageBackground; // GfxImageHandle
            uintptr_t imageBackgroundFocus; // GfxImageHandle
            uintptr_t imageTileLarge; // GfxImageHandle
            uintptr_t imageTileSmall; // GfxImageHandle
            uintptr_t imageTileSideInfo; // GfxImageHandle
            int minPartySize;
            int maxPartySize;
            int maxLocalPlayers;
            int maxPlayers;
            int minPlayers;
            int minPlayersToCreate;
            uint32_t searchType;
            uint32_t minUserTier;
            uint32_t maxUserTier;
            uint32_t parkingPlaylist;
            bool isCustomMatch;
            uint32_t unlockXp;
            uint32_t unlockPLevel;
            uint32_t unk98;
            uint64_t unka0;
            uintptr_t unka8; // char*
            bool hideifmissingdlc;
            bool disableGuests;
            bool excludePublicLobby;
            bool customMutation;
            bool unkb4;
            bool isSpectreRising;
            bool isQuickplayCard;
            int arenaSlot;
            int unkbc;
            uint64_t unkc0;
            uint64_t unkc8;
            uint64_t unkd0;
            uint64_t unkd8;
            uint64_t unke0;
            uint64_t unke8;
            uintptr_t rules; // PlaylistRule*
            int32_t rules_count;
            uint32_t unkfc;
            uintptr_t rotations; // PlaylistRotation*
            int32_t rotations_count;
            int id;
            bool isNewGameOrResumeGame;
            uint64_t unk118;
            uint64_t unk120;
            uint64_t unk128;
            uint64_t unk130;
        };

        struct PlaylistRule {
            uint32_t type;
            uint32_t unk4;
            Hash name;
            uintptr_t value; // const char* 
            int32_t environmentMask;
            byte platformMask;
            uint64_t unk28;
            uint64_t unk30;
            uint64_t unk38;
            uint64_t unk40;
            uint32_t utcStartTime;
            uint32_t utcEndTime;
        };

        struct PlaylistRotation {
            uintptr_t map; // ?
            uintptr_t gametype; // ?
            uint32_t weight;
            bool isFree;
        };
        struct PlaylistMap {
            Hash name;
            Hash baseMapName;
            byte mapPlatformMask;
        };
        struct PlaylistGametype {
            Hash name;
            Hash description;
            Hash mode;
            // I think it is followed by some DDL?
        };

        auto pool = std::make_unique<PlayLists[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            auto n = hashutils::ExtractPtr(p.name.name);

            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/playlists/%lld_%s", opt.m_output, i, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/playlists/%lld_%llx", opt.m_output, i, p.name.name);
            }

            std::cout << " -> " << dumpbuff << "\n";

            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file, ec);

            {
                std::ofstream out{ file / "playlists.json" };

                if (!out) {
                    std::cerr << "Can't open playlists.json\n";
                    continue;
                }

                auto checksum = fastfiles::ComputeChecksum32(reinterpret_cast<char*>(&p.unk0), 4, 0);


                out << "{\n"
                    << "    \"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n"
                    << "    \"unk0\": " << std::dec << p.unk0 << ",\n"
                    << "    \"checksum\": " << std::dec << *reinterpret_cast<uint32_t*>(&checksum) << ",\n"
                    << "    \"maps\": ["
                    ;

                if (p.maps_count) {
                    auto maps = std::make_unique<uintptr_t[]>(p.maps_count);


                    if (proc.ReadMemory(&maps[0], p.maps, sizeof(maps[0]) * p.maps_count)) {

                        PlaylistMap mapInfo;
                        for (size_t j = 0; j < p.maps_count; j++) {
                            if (j) out << ",";
                            if (!proc.ReadMemory(&mapInfo, maps[j], sizeof(PlaylistMap))) {
                                out << "Can't read data\n";
                                continue;
                            }
                            out
                                << "\n"
                                << "        {\n"
                                << "            \"index\": " << std::dec << j << ",\n"
                                << "            \"name\": \"#" << hashutils::ExtractTmp("hash", mapInfo.name.name) << "\",\n"
                                << "            \"baseMapName\": \"#" << hashutils::ExtractTmp("hash", mapInfo.baseMapName.name) << "\",\n"
                                << "            \"mapPlatformMask\": " << std::dec << (int)mapInfo.mapPlatformMask << "\n"
                                << "        }"
                                ;
                            //tool::pool::WriteHex(out, 0, reinterpret_cast<byte*>(&mapInfo), sizeof(mapInfo), proc);
                        }
                    }


                    out << "\n    ],\n";
                }
                else {
                    out << "],\n";
                }

                out
                    << "    \"gametypes\": ["
                    ;


                if (p.gametypes_count) {
                    auto gametypes = std::make_unique<uintptr_t[]>(p.gametypes_count);


                    if (proc.ReadMemory(&gametypes[0], p.gametypes, sizeof(gametypes[0]) * p.gametypes_count)) {

                        PlaylistGametype gtInfo;
                        for (size_t j = 0; j < p.gametypes_count; j++) {
                            if (j) out << ",";
                            if (!proc.ReadMemory(&gtInfo, gametypes[j], sizeof(gtInfo))) {
                                out << "Can't read data\n";
                                continue;
                            }
                            out
                                << "\n"
                                << "        {\n"
                                << "            \"index\": " << std::dec << j << ",\n"
                                << "            \"name\": \"#" << hashutils::ExtractTmp("hash", gtInfo.name.name) << "\",\n"
                                << "            \"description\": \"#" << hashutils::ExtractTmp("hash", gtInfo.description.name) << "\",\n"
                                << "            \"mode\": \"#" << hashutils::ExtractTmp("hash", gtInfo.mode.name) << "\"\n"
                                << "        }"
                                ;
                        }
                    }

                    out << "\n    ]\n";
                }
                else {
                    out << "]\n";
                }

                out
                    << "}\n"
                    ;

                out.close();
            }

            if (p.categories_count) {
                std::filesystem::path categoryDir = file / "categories";
                std::filesystem::create_directories(categoryDir, ec);

                auto data = std::make_unique<PlaylistData[]>(p.categories_count);

                if (!proc.ReadMemory(&data[0], p.categories, sizeof(data[0]) * p.categories_count)) {
                    std::cerr << "Can't read categories data\n";
                    continue;
                }


                for (size_t j = 0; j < p.categories_count; j++) {
                    auto& pl = data[j];

                    std::filesystem::path categoryFile = categoryDir / std::format("{}.json", hashutils::ExtractTmp("file", pl.name.name));

                    std::ofstream out{ categoryFile };

                    if (!out) {
                        std::cerr << "Can't open " << categoryFile << "\n";
                        continue;
                    }
                    out 
                        << "\n"
                        << "{\n"
                        << "    \"index\": " << std::dec << j << ",\n"
                        << "    \"name\": \"#" << hashutils::ExtractTmp("hash", pl.name.name) << "\",\n"
                        << "    \"description\": \"#" << hashutils::ExtractTmp("hash", pl.description.name) << "\",\n"
                        << "    \"lobbyMainMode\": " << (int)pl.lobbyMainMode << ",\n"
                        << "    \"hidden\": " << (pl.hidden ? "true" : "false") << ",\n"
                        ;
                    if (pl.icon) {
                        out << "    \"icon\": \"#" << hashutils::ExtractTmp("hash", pl.icon) << "\",\n";
                    }
                    out
                        << "    \"entries\": ["
                        ;

                    if (pl.unk40_count) {
                        auto entries = std::make_unique<PlaylistEntry[]>(pl.unk40_count);
                        if (!proc.ReadMemory(&entries[0], pl.unk40, sizeof(entries[0]) * pl.unk40_count)) {
                            out << "Can't read entries\n";
                            out.close();
                            continue;
                        }
                        
                        for (size_t k = 0; k < pl.unk40_count; k++) {
                            auto& ple = entries[k];
                            if (k) out << ",";
                            out
                                << "\n"
                                << "        {\n"
                                << "            \"index\": " << std::dec << k << ",\n"
                                << "            \"id\": " << std::dec << ple.id << ",\n"
                                << "            \"name\": \"#" << hashutils::ExtractTmp("hash", ple.name.name) << "\",\n"
                                << "            \"uniqueName\": \"#" << hashutils::ExtractTmp("hash", ple.unique_name.name) << "\",\n"
                                << "            \"mainMode\": \"" << EModeName(ple.mainMode, true) << "\",\n"
                                //<< "            \"unka8\": \"" << ReadTmpStr(proc, ple.unka8) << "\",\n" // constantly NULL, check in zombies?
                                ;

                            auto addGFXName = [&proc, &ple, &out](const char* title, uintptr_t ptr) {
                                if (!ptr) {
                                    return;
                                }
                                auto name = proc.ReadMemory<uint64_t>(ptr + 0x20);
                                if (!name) {
                                    return;
                                }
                                out << "            \"" << title << "\": \"#" << hashutils::ExtractTmp("hash", name) << "\",\n";
                            };
                            addGFXName("image", ple.image);
                            addGFXName("imageBackground", ple.imageBackground);
                            addGFXName("imageBackgroundFocus", ple.imageBackgroundFocus);
                            addGFXName("imageTileLarge", ple.imageTileLarge);
                            addGFXName("imageTileSmall", ple.imageTileSmall);
                            addGFXName("imageTileSideInfo", ple.imageTileSideInfo);
                            out
                                << std::dec
                                << "            \"unlockXp\": " << std::dec << ple.unlockXp << ",\n"
                                << "            \"unlockPLevel\": " << std::dec << ple.unlockPLevel << ",\n"
                                << "            \"maxPartySize\": " << std::dec << ple.maxPartySize << ",\n"
                                << "            \"minPartySize\": " << std::dec << ple.minPartySize << ",\n"
                                << "            \"maxPlayers\": " << std::dec << ple.maxPlayers << ",\n"
                                << "            \"minPlayers\": " << std::dec << ple.minPlayers << ",\n"
                                << "            \"minPlayersToCreate\": " << std::dec << ple.minPlayersToCreate << ",\n"
                                << "            \"maxLocalPlayers\": " << std::dec << ple.maxLocalPlayers << ",\n"
                                << "            \"searchType\": " << std::dec << ple.searchType << ",\n"
                                << "            \"minUserTier\": " << std::dec << ple.minUserTier << ",\n"
                                << "            \"maxUserTier\": " << std::dec << ple.maxUserTier << ",\n"
                                << "            \"arenaSlot\": " << std::dec << ple.arenaSlot << ",\n"
                                << "            \"parkingPlaylist\": " << std::dec << ple.parkingPlaylist << ",\n"
                                << "            \"disableGuests\": " << std::dec << (ple.disableGuests ? "true" : "false") << ",\n"
                                << "            \"excludePublicLobby\": " << std::dec << (ple.excludePublicLobby ? "true" : "false") << ",\n"
                                << "            \"customMutation\": " << std::dec << (ple.customMutation ? "true" : "false") << ",\n"
                                << "            \"isSpectreRising\": " << std::dec << (ple.isSpectreRising ? "true" : "false") << ",\n"
                                << "            \"isQuickplayCard\": " << std::dec << (ple.isQuickplayCard ? "true" : "false") << ",\n"
                                << "            \"hideifmissingdlc\": " << std::dec << (ple.hideifmissingdlc ? "true" : "false") << ",\n"
                                << "            \"isCustomMatch\": " << std::dec << (ple.isCustomMatch ? "true" : "false") << ",\n"
                                << "            \"isNewGameOrResumeGame\": " << std::dec << (ple.isNewGameOrResumeGame ? "true" : "false") << ",\n"
                                << "            \"rules\": ["
                                ;

                            if (ple.rules_count) {
                                auto rules = std::make_unique<PlaylistRule[]>(ple.rules_count);
                                if (!proc.ReadMemory(&rules[0], ple.rules, sizeof(rules[0]) * ple.rules_count)) {
                                    out << "Can't read rules\n";
                                    out.close();
                                    continue;
                                }

                                for (size_t l = 0; l < ple.rules_count; l++) {
                                    auto& rule = rules[l];

                                    if (l) out << ",";
                                    out
                                        << "\n"
                                        << "                {\n"
                                        << "                    \"type\": " << std::dec << rule.type << ",\n"
                                        << "                    \"name\": \"#" << hashutils::ExtractTmp("hash", rule.name.name) << "\",\n"
                                        << "                    \"value\": \"" << ReadTmpStr(proc, rule.value) << "\",\n"
                                        << "                    \"utcStartTime\": " << std::dec << rule.utcStartTime << ",\n"
                                        << "                    \"utcEndTime\": " << std::dec << rule.utcEndTime << ",\n"
                                        << "                    \"platformMask\": " << std::dec << (int)rule.platformMask << ",\n"
                                        << "                    \"environmentMask\": " << std::dec << (int)rule.environmentMask << "\n"
                                        << "                }"
                                        ;
                                }

                                // 
                                out
                                    << "\n"
                                    << "            ],\n";
                            }
                            else {
                                out << "],\n";
                            }

                            out
                                << "            \"rotationList\": [";
                            if (ple.rotations_count) {
                                auto rotations = std::make_unique<PlaylistRotation[]>(ple.rotations_count);
                                if (!proc.ReadMemory(&rotations[0], ple.rotations, sizeof(rotations[0]) * ple.rotations_count)) {
                                    out << "Can't read rotations\n";
                                    out.close();
                                    continue;
                                }

                                byte tmpBuffer[0x28];
                                
                                for (size_t l = 0; l < ple.rotations_count; l++) {
                                    auto& rotation = rotations[l];



                                    if (l) out << ",";
                                    out
                                        << "\n"
                                        << "                {\n"
                                        ;

                                    if (proc.ReadMemory(tmpBuffer, rotation.map, sizeof(tmpBuffer))) {
                                        out
                                            << "                    \"map\": \"#" << hashutils::ExtractTmp("hash", reinterpret_cast<Hash*>(tmpBuffer)->name) << "\",\n"
                                            << "                    \"mapPlatformMask\": " << std::dec << (int)tmpBuffer[0x20] << ",\n"
                                            ;
                                    }

                                    if (proc.ReadMemory(tmpBuffer, rotation.gametype, sizeof(tmpBuffer))) {
                                        out
                                            << "                    \"gametype\": \"#" << hashutils::ExtractTmp("hash", reinterpret_cast<Hash*>(tmpBuffer)->name) << "\",\n"
                                            ;
                                    }

                                    out
                                        << "                    \"weight\": " << std::dec << rotation.weight << ",\n"
                                        << "                    \"isFree\": " << (rotation.isFree ? "true" : "false") << "\n"
                                        << "                }"
                                        ;
                                }

                                // 
                                out
                                    << "\n"
                                    << "            ]\n";
                            }
                            else {
                                out << "]\n";
                            }

                            out
                                << "        }"
                                ;

                        }

                        out
                            << "\n"
                            << "    ]\n";
                    }
                    else {
                        out << "]\n";
                    }


                    out
                        << "}";
                }
            }
        }
    }
    if (ShouldHandle(ASSET_TYPE_SCRIPTPARSETREEFORCED)) {
        struct ScriptParseTreeForced {
            Hash name;
            uint32_t gscCount;
            uint32_t cscCount;
            uintptr_t gscScripts; // Hash*
            uintptr_t cscScripts; // Hash*
        };


        auto pool = std::make_unique<ScriptParseTreeForced[]>(entry.itemAllocCount);

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

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto* n = hashutils::ExtractPtr(p.name.name);
            if (n) {
                sprintf_s(dumpbuff, "%s/tables/scriptparsetreeforced/%s.json", opt.m_output, n);
            }
            else {
                sprintf_s(dumpbuff, "%s/tables/scriptparsetreeforced/file_%llx.json", opt.m_output, p.name.name);
            }

            std::cout << "Element #" << std::dec << i << " -> " << dumpbuff << "\n";



            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::ofstream defout{ file };

            if (!defout) {
                std::cerr << "Can't open output file\n";
                continue;
            }

            defout << "{\n";
            utils::Padding(defout, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(defout, 1) << "\"gscScripts\": [";

            auto scripts = std::make_unique<Hash[]>(std::max(p.gscCount, p.cscCount));

            if (p.gscCount) {
                if (!proc.ReadMemory(&scripts[0], p.gscScripts, sizeof(Hash) * p.gscCount)) {
                    std::cerr << "Can't read GSC scripts\n";
                    defout.close();
                    continue;
                }

                for (size_t j = 0; j < p.gscCount; j++) {
                    if (j) defout << ",";
                    utils::Padding(defout << "\n", 2) << "\"#" << hashutils::ExtractTmp("script", scripts[j].name) << "\"";
                }
                utils::Padding(defout << "\n", 1) << "],\n";
            }
            else {
                defout << "],\n";
            }


            utils::Padding(defout, 1) << "\"cscScripts\": [";

            if (p.cscCount) {
                if (p.cscCount && !proc.ReadMemory(&scripts[0], p.cscScripts, sizeof(Hash) * p.cscCount)) {
                    std::cerr << "Can't read CSC scripts\n";
                    defout.close();
                    continue;
                }

                for (size_t j = 0; j < p.cscCount; j++) {
                    if (j) defout << ",";
                    utils::Padding(defout << "\n", 2) << "\"#" << hashutils::ExtractTmp("script", scripts[j].name) << "\"";
                }
                utils::Padding(defout << "\n", 1) << "]\n";
            }
            else {
                defout << "]\n";
            }
            defout << "}\n";

            defout.close();
        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_PRESTIGE)) {
        struct PrestigeInfo {
            XHash name;
            XHash displayName;
            uint32_t unlockLevel;
            uint32_t winsRequired;
            uint32_t titleOfOrigin;
            uint32_t unk2c;
            uintptr_t iconName;
            uintptr_t iconNameLarge;
        };




        auto pool = std::make_unique<PrestigeInfo[]>(entry.itemAllocCount);

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

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto* n = hashutils::ExtractPtr(p.name.name);
            if (n) {
                sprintf_s(dumpbuff, "%s/tables/prestige/%s.json", opt.m_output, n);
            }
            else {
                sprintf_s(dumpbuff, "%s/tables/prestige/file_%llx.json", opt.m_output, p.name.name);
            }

            std::cout << "Table #" << std::dec << i << " -> " << dumpbuff << "\n";



            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::ofstream defout{ file };

            if (!defout) {
                std::cerr << "Can't open output file\n";
                continue;
            }

            defout << "{\n";
            utils::Padding(defout, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(defout, 1) << "\"displayName\": \"#" << hashutils::ExtractTmp("hash", p.displayName.name) << "\",\n";
            if (p.iconName) {
                utils::Padding(defout, 1) << "\"iconName\": \"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(p.iconName + 0x20)) << "\",\n";
            }
            if (p.iconNameLarge) {
                utils::Padding(defout, 1) << "\"iconNameLarge\": \"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(p.iconNameLarge + 0x20)) << "\",\n";
            }
            utils::Padding(defout, 1) << "\"unlockLevel\": " << std::dec << p.unlockLevel << ",\n";
            utils::Padding(defout, 1) << "\"winsRequired\": " << std::dec << p.winsRequired << ",\n";
            utils::Padding(defout, 1) << "\"titleOfOrigin\": " << std::dec << p.titleOfOrigin << "\n";

            defout << "}\n";


            defout.close();
        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_PRESTIGETABLE)) {
        struct PrestigeTable {
            XHash name;
            uint64_t prestigeCount;
            uintptr_t data;
        };

        struct PrestigeInfo {
            XHash name;
            XHash displayName;
            uint32_t unlockLevel;
            uint32_t winsRequired;
            uint32_t titleOfOrigin;
            uint32_t unk2c;
            uintptr_t iconName;
            uintptr_t iconNameLarge;
        };


        auto pool = std::make_unique<PrestigeTable[]>(entry.itemAllocCount);

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

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto* n = hashutils::ExtractPtr(p.name.name);
            if (n) {
                sprintf_s(dumpbuff, "%s/tables/prestige/table/%s.csv", opt.m_output, n);
            }
            else {
                sprintf_s(dumpbuff, "%s/tables/prestige/table/file_%llx.csv", opt.m_output, p.name.name);
            }

            std::cout << "Table #" << std::dec << i << " -> " << dumpbuff << "\n";



            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::ofstream defout{ file };

            if (!defout) {
                std::cerr << "Can't open output file\n";
                continue;
            }

            defout << "iconId,name,displayName,unlockLevel,winsRequired,titleOfOrigin,iconName,iconNameLarge";

            if (p.prestigeCount) {
                auto tableVals = std::make_unique<uintptr_t[]>(p.prestigeCount);
                PrestigeInfo nfo{};

                if (!proc.ReadMemory(&tableVals[0], p.data, sizeof(tableVals[0]) * p.prestigeCount)) {
                    defout.close();
                    std::cerr << "Can't read memory\n";
                    break;
                }

                for (size_t i = 0; i < p.prestigeCount; i++) {
                    if (!proc.ReadMemory(&nfo, tableVals[i], sizeof(nfo))) {
                        defout.close();
                        std::cerr << "Can't read memory\n";
                        continue;
                    }
                    defout 
                        << "\n" 
                        << std::dec << i << ","
                        << "#" << hashutils::ExtractTmp("hash", nfo.name.name) << ","
                        << "#" << hashutils::ExtractTmp("hash", nfo.displayName.name) << ","
                        << std::dec << nfo.unlockLevel << ","
                        << std::dec << nfo.winsRequired << ","
                        << std::dec << nfo.titleOfOrigin << ","
                        << "#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(nfo.iconName + 0x20)) << ","
                        << "#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(nfo.iconNameLarge + 0x20))
                        ;
                }
            }


            defout.close();
        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_RANK)) {
        struct RankInfo {
            XHash name;
            uint32_t level;
            uint32_t minXp;
            uint32_t maxXp;
            XHash shortNameRef;
            XHash fullNameRef;
            XHash ingameNameRef;
            uintptr_t icon; // GfxImagePtr
            uintptr_t iconLarge; // GfxImagePtr
            uint64_t rewardsCount;
            uintptr_t rewards; // RankInfoReward*
        };
        struct RankInfoReward {
            XHash prestigeTarget;
            XHash rewardName;
        };

        auto pool = std::make_unique<RankInfo[]>(entry.itemAllocCount);

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

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto* n = hashutils::ExtractPtr(p.name.name);
            if (n) {
                sprintf_s(dumpbuff, "%s/tables/rank/%s.json", opt.m_output, n);
            }
            else {
                sprintf_s(dumpbuff, "%s/tables/rank/file_%llx.json", opt.m_output, p.name.name);
            }

            std::cout << "Table #" << std::dec << i << " -> " << dumpbuff << "\n";



            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::ofstream defout{ file };

            if (!defout) {
                std::cerr << "Can't open output file\n";
                continue;
            }

            defout << "{\n";
            utils::Padding(defout, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(defout, 1) << "\"shortNameRef\": \"#" << hashutils::ExtractTmp("hash", p.shortNameRef.name) << "\",\n";
            utils::Padding(defout, 1) << "\"fullNameRef\": \"#" << hashutils::ExtractTmp("hash", p.fullNameRef.name) << "\",\n";
            utils::Padding(defout, 1) << "\"ingameNameRef\": \"#" << hashutils::ExtractTmp("hash", p.ingameNameRef.name) << "\",\n";
            if (p.icon) {
                utils::Padding(defout, 1) << "\"iconName\": \"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(p.icon + 0x20)) << "\",\n";
            }
            if (p.iconLarge) {
                utils::Padding(defout, 1) << "\"iconNameLarge\": \"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(p.iconLarge + 0x20)) << "\",\n";
            }
            utils::Padding(defout, 1) << "\"level\": " << std::dec << p.level << ",\n";
            utils::Padding(defout, 1) << "\"minXp\": " << std::dec << p.minXp << ",\n";
            utils::Padding(defout, 1) << "\"maxXp\": " << std::dec << p.maxXp << ",\n";
            utils::Padding(defout, 1) << "\"rewards\": [";

            if (p.rewardsCount) {
                auto rewards = std::make_unique<RankInfoReward[]>(p.rewardsCount);

                if (!proc.ReadMemory(&rewards[0], p.rewards, sizeof(rewards[0]) * p.rewardsCount)) {
                    std::cerr << "Can't read memory\n";
                    break;
                }

                for (size_t j = 0; j < p.rewardsCount; j++) {
                    auto& rew = rewards[j];

                    if (j) defout << ",";
                    defout << "\n";
                    utils::Padding(defout, 2) << "{\n";
                    utils::Padding(defout, 3) << "\"prestigeTarget\": \"#" << hashutils::ExtractTmp("hash", rew.prestigeTarget.name) << "\",\n";
                    utils::Padding(defout, 3) << "\"rewardName\": \"#" << hashutils::ExtractTmp("hash", rew.rewardName.name) << "\"\n";
                    utils::Padding(defout, 2) << "}";
                }

                utils::Padding(defout << "\n", 1) << "]\n";
            }
            else {
                defout << "]\n";
            }

            defout << "}\n";


            defout.close();
        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_RANKTABLE)) {
        struct RankTable {
            XHash name;
            uint64_t rankCount;
            uintptr_t data;
        };
        struct RankInfo {
            XHash name;
            uint32_t level;
            uint32_t minXp;
            uint32_t maxXp;
            XHash shortNameRef;
            XHash fullNameRef;
            XHash ingameNameRef;
            uintptr_t icon; // GfxImagePtr
            uintptr_t iconLarge; // GfxImagePtr
            uint64_t rewardsCount;
            uintptr_t rewards; // RankInfoReward*
        };
        struct RankInfoReward {
            XHash prestigeTarget;
            XHash rewardName;
        };

        auto pool = std::make_unique<RankTable[]>(entry.itemAllocCount);

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

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto* n = hashutils::ExtractPtr(p.name.name);
            if (n) {
                sprintf_s(dumpbuff, "%s/tables/rank/table/%s.csv", opt.m_output, n);
            }
            else {
                sprintf_s(dumpbuff, "%s/tables/rank/table/file_%llx.csv", opt.m_output, p.name.name);
            }

            std::cout << "Table #" << std::dec << i << " -> " << dumpbuff << "\n";



            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::ofstream defout{ file };

            if (!defout) {
                std::cerr << "Can't open output file\n";
                continue;
            }

            RankInfo nfo{};

            defout << "iconId,name,level,minXp,maxXp,shortNameRef,fullNameRef,ingameNameRef,iconName,iconNameLarge,rewards";

            if (p.rankCount) {
                auto tableVals = std::make_unique<uintptr_t[]>(p.rankCount);

                if (!proc.ReadMemory(&tableVals[0], p.data, sizeof(tableVals[0]) * p.rankCount)) {
                    defout.close();
                    std::cerr << "Can't read memory\n";
                    break;
                }

                for (size_t i = 0; i < p.rankCount; i++) {
                    if (!proc.ReadMemory(&nfo, tableVals[i], sizeof(nfo))) {
                        defout.close();
                        std::cerr << "Can't read memory\n";
                        continue;
                    }


                    defout
                        << "\n"
                        << std::dec << i << ","
                        << "#" << hashutils::ExtractTmp("hash", nfo.name.name) << ","
                        << std::dec << nfo.level << ","
                        << std::dec << nfo.minXp << ","
                        << std::dec << nfo.maxXp << ","
                        << "#" << hashutils::ExtractTmp("hash", nfo.shortNameRef.name) << ","
                        << "#" << hashutils::ExtractTmp("hash", nfo.fullNameRef.name) << ","
                        << "#" << hashutils::ExtractTmp("hash", nfo.ingameNameRef.name) << ","
                        << "#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(nfo.icon + 0x20)) << ","
                        << "#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(nfo.icon + 0x20)) << ","
                        ;

                    if (nfo.rewardsCount) {
                        auto rewards = std::make_unique<RankInfoReward[]>(nfo.rewardsCount);

                        if (!proc.ReadMemory(&rewards[0], nfo.rewards, sizeof(rewards[0]) * nfo.rewardsCount)) {
                            std::cerr << "Can't read memory\n";
                            break;
                        }

                        for (size_t j = 0; j < nfo.rewardsCount; j++) {
                            auto& rew = rewards[j];

                            if (j) defout << ";";

                            defout << "#" << hashutils::ExtractTmp("hash", rew.rewardName.name);
                        }
                    }
                }
            }


            defout.close();
        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_STORECATEGORYLIST)) {
        struct StoreCategoryList {
            XHash name;
            uint64_t count;
            uintptr_t data;
        };

        auto pool = std::make_unique<StoreCategoryList[]>(entry.itemAllocCount);

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

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto* n = hashutils::ExtractPtr(p.name.name);
            if (n) {
                sprintf_s(dumpbuff, "%s/tables/store/category/list/%s.csv", opt.m_output, n);
            }
            else {
                sprintf_s(dumpbuff, "%s/tables/store/category/list/file_%llx.csv", opt.m_output, p.name.name);
            }

            std::cout << "Table #" << std::dec << i << " -> " << dumpbuff << "\n";



            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::ofstream defout{ file };

            if (!defout) {
                std::cerr << "Can't open output file\n";
                continue;
            }

            defout << "id,name,nameHashed,displayName,image,productsCount,visibility";

            if (p.count) {
                auto tableVals = std::make_unique<uintptr_t[]>(p.count);

                if (!proc.ReadMemory(&tableVals[0], p.data, sizeof(tableVals[0]) * p.count)) {
                    defout.close();
                    std::cerr << "Can't read memory\n";
                    break;
                }
                struct StoreCategory
                {
                    uintptr_t name2; // const char*
                    XHash name;
                    uintptr_t image; // GfxImage*
                    XHash displayName;
                    byte productsCount;
                    uintptr_t products; // StoreProducts**
                    bool visibility;
                };

                StoreCategory nfo{};
                for (size_t i = 0; i < p.count; i++) {
                    if (!proc.ReadMemory(&nfo, tableVals[i], sizeof(nfo))) {
                        defout.close();
                        std::cerr << "Can't read memory\n";
                        continue;
                    }

                    auto* nameUn = ReadTmpStr(proc, nfo.name2);

                    hashutils::Add(nameUn);

                    defout << std::dec << "\n" << i
                        << "," << nameUn
                        << ",#" << hashutils::ExtractTmp("hash", nfo.name.name)
                        << ",#" << hashutils::ExtractTmp("hash", nfo.displayName.name)
                        << ",#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(nfo.image + 0x20))
                        << "," << std::dec << (int)nfo.productsCount
                        << "," << (nfo.visibility ? "true" : "false")
                        ;

                    //tool::pool::WriteHex(defout << "\n", 0, (byte*)&nfo, sizeof(nfo), proc);
                }
            }


            defout.close();
        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_STORECATEGORY)) {
        struct StoreCategory
        {
            uintptr_t name2; // const char*
            XHash name;
            uintptr_t image; // GfxImage*
            XHash displayName;
            byte productsCount;
            uintptr_t products; // StoreProducts**
            bool visibility;
        };

        auto pool = std::make_unique<StoreCategory[]>(entry.itemAllocCount);

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

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto* nameUn = ReadTmpStr(proc, p.name2);

            hashutils::Add(nameUn);

            auto* n = hashutils::ExtractPtr(p.name.name);
            if (n) {
                sprintf_s(dumpbuff, "%s/tables/store/category/%s.json", opt.m_output, n);
            }
            else {
                sprintf_s(dumpbuff, "%s/tables/store/category/file_%llx.json", opt.m_output, p.name.name);
            }

            std::cout << "Table #" << std::dec << i << " -> " << dumpbuff << "\n";



            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::ofstream defout{ file };

            if (!defout) {
                std::cerr << "Can't open output file\n";
                continue;
            }

            defout << "{\n";
            utils::Padding(defout, 1) << "\"name\": \"" << nameUn << "\",\n";
            utils::Padding(defout, 1) << "\"nameHash\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(defout, 1) << "\"displayName\": \"#" << hashutils::ExtractTmp("hash", p.displayName.name) << "\",\n";
            utils::Padding(defout, 1) << "\"imageName\": \"#" << proc.ReadMemory<uint64_t>(p.image + 0x20) << "\",\n";
            utils::Padding(defout, 1) << "\"visibility\": " << (p.visibility ? "true" : "false") << ",\n";
            utils::Padding(defout, 1) << "\"products\": [";
            if (p.productsCount) {
                auto products = std::make_unique<uintptr_t[]>(p.productsCount);

                if (!proc.ReadMemory(&products[0], p.products, sizeof(products[0]) * p.productsCount)) {
                    std::cerr << "Can't read memory";
                    break;
                }

                for (size_t j = 0; j < p.productsCount; j++) {
                    if (j) defout << ",";
                    utils::Padding(defout << "\n", 2) << "\"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(products[j])) << "\"";
                }
                utils::Padding(defout << "\n", 1) << "]\n";
            }
            else {
                defout << "]\n";
            }
            defout << "}";


            defout.close();
        }
    }
    if (ShouldHandle(ASSET_TYPE_STOREPRODUCT)) {
        struct StoreProduct
        {
            XHash name;
            uintptr_t previewImage;// GfxImage*
            uintptr_t productImage; // GfxImage*
            uintptr_t blackMarketImage;// GfxImage*
            uintptr_t metadata;// const char*
            uintptr_t videoHighResRef;// const char*
            uintptr_t videoLowResRef;// const char*
            XHash blackMarketName;
            XHash blackMarketDesc;
            XHash entitlementNameXHash; // const char*
            uintptr_t productID; // const char*
            uintptr_t unk78;
            uint64_t unk80;
            uint64_t unk88;
            uint64_t unk90;
            uint64_t unk98;
            bool visibility;
            bool unka1;
            bool unka2;
            bool unka3;
            uint32_t unka4;
            uint64_t unka8;
            uint64_t unkb0;
            uint64_t unkb8;
            uint64_t unkc0;
        };


        auto pool = std::make_unique<StoreProduct[]>(entry.itemAllocCount);

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

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto* n = hashutils::ExtractPtr(p.name.name);
            if (n) {
                sprintf_s(dumpbuff, "%s/tables/store/product/%s.json", opt.m_output, n);
            }
            else {
                sprintf_s(dumpbuff, "%s/tables/store/product/file_%llx.json", opt.m_output, p.name.name);
            }

            std::cout << "Table #" << std::dec << i << " -> " << dumpbuff << "\n";



            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::ofstream defout{ file };

            if (!defout) {
                std::cerr << "Can't open output file\n";
                continue;
            }

            defout << "{\n";
            utils::Padding(defout, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(defout, 1) << "\"blackMarketName\": \"#" << hashutils::ExtractTmp("hash", p.blackMarketName.name) << "\",\n";
            utils::Padding(defout, 1) << "\"blackMarketDesc\": \"#" << hashutils::ExtractTmp("hash", p.blackMarketDesc.name) << "\",\n";
            utils::Padding(defout, 1) << "\"entitlementNameXHash\": \"#" << hashutils::ExtractTmp("hash", p.entitlementNameXHash.name) << "\",\n";
            if (p.productID) {
                utils::Padding(defout, 1) << "\"productID\": \"" << ReadTmpStr(proc, p.productID) << "\",\n";
            }
            if (p.metadata) {
                utils::Padding(defout, 1) << "\"metadata\": \"" << ReadTmpStr(proc, p.metadata) << "\",\n";
            }
            if (p.videoLowResRef) {
                utils::Padding(defout, 1) << "\"videoLowResRef\": \"" << ReadTmpStr(proc, p.videoLowResRef) << "\",\n";
            }
            if (p.videoHighResRef) {
                utils::Padding(defout, 1) << "\"videoHighResRef\": \"" << ReadTmpStr(proc, p.videoHighResRef) << "\",\n";
            }
            if (p.previewImage) {
                utils::Padding(defout, 1) << "\"previewImageName\": \"" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(p.previewImage + 0x20)) << "\",\n";
            }
            if (p.productImage) {
                utils::Padding(defout, 1) << "\"productImageName\": \"" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(p.productImage + 0x20)) << "\",\n";
            }
            if (p.blackMarketImage) {
                utils::Padding(defout, 1) << "\"blackMarketImageName\": \"" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(p.blackMarketImage + 0x20)) << "\",\n";
            }
            utils::Padding(defout, 1) << "\"visibility\": " << (p.visibility ? "true" : "false") << "\n";

            
            //tool::pool::WriteHex(defout << "\n", 0, (byte*)&p, sizeof(p), proc);
            defout << "}";


            defout.close();
        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_STRUCTURED_TABLE)) {
        struct StructuredTable
        {
            XHash name;
            int cellCount;
            int columnCount;
            int rowCount;
            uintptr_t cells; // StructuredTableCell*
            uintptr_t cellIndex; // int*
            uintptr_t headers; // StructuredTableHeader*
            uintptr_t headerIndex; // int*
        };
        enum StructuredTableCellType {
            STRUCTURED_TABLE_CELL_TYPE_NONE = 0x0,
            STRUCTURED_TABLE_CELL_TYPE_STRING = 0x1,
            STRUCTURED_TABLE_CELL_TYPE_NUMBER = 0x2,
            STRUCTURED_TABLE_CELL_TYPE_COUNT = 0x3,
        };
        struct StructuredTableCell
        {
            union {
                int number;
                int hash;
                uintptr_t ptr;
            };
            StructuredTableCellType type;
            int unkc;
        };
        struct StructuredTableHeader
        {
            uintptr_t string; // const char*
            int hash;
            int index;
        };




        auto pool = std::make_unique<StructuredTable[]>(entry.itemAllocCount);

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

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto* n = hashutils::ExtractPtr(p.name.name);
            if (n) {
                sprintf_s(dumpbuff, "%s/%s", opt.m_output, n);
            }
            else {
                sprintf_s(dumpbuff, "%s/hashed/structuredtable/file_%llx.json", opt.m_output, p.name.name);
            }

            std::cout << "Table #" << std::dec << i << " -> " << dumpbuff << "\n";



            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::ofstream defout{ file };

            if (!defout) {
                std::cerr << "Can't open output file\n";
                continue;
            }

            if (!p.columnCount) {
                defout << "[]";
                defout.close();
                continue; // empty
            }

            auto headerInfo = std::make_unique<StructuredTableHeader[]>(p.columnCount);
            
            if (!proc.ReadMemory(&headerInfo[0], p.headers, sizeof(headerInfo[0]) * p.columnCount)) {
                defout.close();
                std::cerr << "Can't read memory\n";
                break;
            }

            //for (size_t col = 0; col < p.columnCount; col++) {
            //    auto& column = headerInfo[col];
            //    if (col) defout << ",";
            //    defout << ReadTmpStr(proc, column.string);
            //}

            defout << "[";
            if (p.rowCount) {
                auto cells = std::make_unique<StructuredTableCell[]>(p.columnCount * p.rowCount);
                if (!proc.ReadMemory(&cells[0], p.cells, sizeof(cells[0]) * p.columnCount * p.rowCount)) {
                    defout.close();
                    std::cerr << "Can't read memory\n";
                    break;
                }

                for (size_t row = 0; row < p.rowCount; row++) {
                    auto* rowcells = &cells[row * p.columnCount];

                    if (row) defout << ",";
                    utils::Padding(defout << "\n", 1) << "{";
                    int count = 0;
                    for (size_t col = 0; col < p.columnCount; col++) {
                        auto& cell = rowcells[col];

                        if (cell.type == STRUCTURED_TABLE_CELL_TYPE_NONE) {
                            continue;
                        }

                        if (count++) defout << ",";
                        utils::Padding(defout << "\n", 2) << "\"" << ReadTmpStr(proc, headerInfo[col].string) << "\": ";
                        switch (cell.type) {
                        case STRUCTURED_TABLE_CELL_TYPE_STRING:
                            defout << "\"" << ReadTmpStr(proc, cell.ptr) << "\"";
                            break;
                        case STRUCTURED_TABLE_CELL_TYPE_NUMBER:
                            defout << std::dec << cell.number;
                            break;
                        default:
                            defout << std::dec << "\"unk:" << cell.type << "\"";
                            break; // ignore
                        }
                    }
                    utils::Padding(defout << "\n", 1) << "}";

                }
                defout << "\n";
            }
            defout << "]";

            defout.close();
        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_TTF)) {
        struct TTFDef {
            Hash name;
            Hash file2;
            uint32_t size;
            uint32_t pad24;
            uintptr_t buffer28;
            uint64_t unk30;
            uintptr_t buffer38;
            uintptr_t buffer40;
            uint32_t unk48;
            uint32_t unk4c;
        };


        auto pool = std::make_unique<TTFDef[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];
        const size_t dumpbuffsize = sizeof(dumpbuff);
        std::vector<byte> read{};
        size_t readFile = 0;

        if (opt.flags & DUMP_ASSETS) {

            for (size_t i = 0; i < entry.itemAllocCount; i++) {
                const auto& p = pool[i];

                if (p.name.name < MIN_HASH_VAL) {
                    continue;
                }

                auto* n = hashutils::ExtractPtr(p.name.name);
                if (n) {
                    sprintf_s(dumpbuff, "%s/tables/ttf/%s.ttf", opt.m_output, n);
                }
                else {
                    sprintf_s(dumpbuff, "%s/tables/ttf/file_%llx.ttf", opt.m_output, p.name.name);
                }

                std::cout << "Font #" << std::dec << i << " -> " << dumpbuff << "\n";


                std::filesystem::path file(dumpbuff);
                std::filesystem::create_directories(file.parent_path(), ec);

                auto data = std::make_unique<byte[]>(p.size);

                if (!(proc.ReadMemory(&data[0], p.buffer28, p.size) && utils::WriteFile(file, &data[0], p.size))) {
                    std::cerr << "Can't read/write font\n";
                }
            }
        }
        else {
            sprintf_s(dumpbuff, "%s/tables/ttf.csv", opt.m_output);
            std::cout << dumpbuff << "\n";
            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::ofstream ttfdump{ file };

            if (!ttfdump) {
                std::cerr << "Can't create ttf output\n";
                return tool::BASIC_ERROR;
            }

            ttfdump << "name,file,size";

            for (size_t i = 0; i < entry.itemAllocCount; i++) {
                const auto& p = pool[i];

                if (p.name.name < MIN_HASH_VAL) {
                    continue;
                }

                ttfdump << "\n" << hashutils::ExtractTmp("hash", p.name.name) << "," << hashutils::ExtractTmp("hash", p.file2.name) << "," << std::dec << p.size;
            }

            ttfdump.close();
        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_DDL)) {
        struct DDLEntry {
            Hash name;
            uintptr_t ddlDef; // DDLDef*
            uint64_t pad[8];
        };


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

            auto* n = hashutils::ExtractPtr(p.name.name);
            if (n) {
                sprintf_s(dumpbuff, "%s/%s", opt.m_output, n);
            }
            else {
                sprintf_s(dumpbuff, "%s/hashed/ddl/file_%llx.ddl", opt.m_output, p.name.name);
            }

            std::cout << "Writing DDL #" << std::dec << i << " -> " << dumpbuff << "\n";



            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::ofstream defout{ file };

            if (!defout) {
                std::cerr << "Can't open output file\n";
                continue;
            }

            ReadDDLDefEntry(opt, proc, defout, p.ddlDef);

            defout.close();
        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_BG_CACHE)) {
        auto pool = std::make_unique<BGCache[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }



        BGCacheInfo entryinfo[BG_CACHE_TYPE_COUNT]{};

        if (!proc.ReadMemory(&entryinfo[0], proc[0x4EC9A90], sizeof(entryinfo))) {
            std::cerr << "Can't read cache\n";
            return tool::BASIC_ERROR;
        }

        char nameInfo[BG_CACHE_TYPE_COUNT][200] = {};
        // buffer pool names
        for (size_t i = 0; i < BG_CACHE_TYPE_COUNT; i++) {
            if (proc.ReadString(nameInfo[i], entryinfo[i].name, sizeof(nameInfo[i])) < 0) {
                std::cerr << "Can't read bgcache info names\n";
                return tool::BASIC_ERROR;
            }
        }

        char dumpbuff[MAX_PATH + 10];
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
                    << (p2.type >= 0 && p2.type < BG_CACHE_TYPE_COUNT ? nameInfo[p2.type] : "<error>") << ","
                    << hashutils::ExtractTmp("hash", p2.name) << "," 
                    << std::hex << p2.string_count
                    << std::flush;
            }

            defout.close();
        }

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_SCRIPTBUNDLELIST)) {
        size_t readFile = 0;

        struct ScriptBundleList
        {
            XHash name;
            ScrString_t assetType;
            uint32_t assetCount;
            uintptr_t assets; // ScriptBundlePtr*
        };

        auto pool = std::make_unique<ScriptBundleList[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto n = hashutils::ExtractPtr(p.name.name);


            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/scriptbundle/list/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/scriptbundle/list/file_%llx.json", opt.m_output, p.name.name);
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

            out << "{\n";
            utils::Padding(out, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(out, 1) << "\"assetType\": \"" << ReadMTString(proc, p.assetType) << "\",\n";
            utils::Padding(out, 1) << "\"assets\": [";

            if (p.assetCount) {
                auto assets = std::make_unique<uintptr_t[]>(p.assetCount);

                if (!proc.ReadMemory(&assets[0], p.assets, sizeof(assets[0]) * p.assetCount)) {
                    std::cerr << "Can't read memory\n";
                    out.close();
                    break;
                }

                for (size_t j = 0; j < p.assetCount; j++) {
                    if (j) out << ",";
                    utils::Padding(out << "\n", 2) << "\"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(assets[j])) << "\"";
                }

                utils::Padding(out << "\n", 1) << "]\n";
            }
            else {
                out << "]\n";
            }

            out << "}";

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_SCRIPTBUNDLE)) {
        auto pool = std::make_unique<ScriptBundle[]>(entry.itemCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }

        char dumpbuff[MAX_PATH + 10];
        size_t readFile = 0;

        for (size_t i = 0; i < entry.itemCount; i++) {
            const auto& p = pool[i];

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            ReadSBName(proc, p.sbObjectsArray);

            std::cout << std::dec << i << ": ";

            sprintf_s(dumpbuff, "%s/scriptbundle/%s/%s.json", opt.m_output, p.bundleType.name ? hashutils::ExtractTmp("hash", p.bundleType.name) : "default", hashutils::ExtractTmp("hash", p.name.name));


            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::cout << file;

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

        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_SCRIPTPARSETREE)) {
        auto pool = std::make_unique<tool::dump::T8ScriptParseTreeEntry[]>(entry.itemCount);

        size_t readFile = 0;

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

        std::cout << "Dumping compiled GSC scripts into scriptparsetree...\n";
        std::filesystem::create_directories("scriptparsetree", ec);

        for (size_t i = 0; i < entry.itemCount; i++) {
            auto& p = pool[i];

            if (p.name < MIN_HASH_VAL) {
                continue;
            }

            sprintf_s(dumpbuff, "scriptparsetree/script_%llx.gsc", p.name);

            std::filesystem::path file(dumpbuff);

            std::cout << "Dumping into " << dumpbuff << "...\n";

            auto buff = std::make_unique<byte[]>(p.size);

            if (!proc.ReadMemory(&buff[0], p.buffer, p.size) || !utils::WriteFile(file, &buff[0], p.size)) {
                std::cerr << "Error when dumping\n";
                continue;
            }
        }

        std::cout << "Decompiling dumped GSC scripts...\n";

        const char* argvinfo[] = {
            argv[0],
            "gscinfo",
            "-g", // run decompiler
            "scriptparsetree", // decompile directory
            "-o", opt.m_output, // output dir
        };

        tool::gsc::gscinfo(proc, ARRAYSIZE(argvinfo), argvinfo);
    }
    if (ShouldHandle(ASSET_TYPE_UNLOCKABLE_ITEM)) {
        struct UnlockableItemTableElem {
            uintptr_t name; // const char*
            Hash nameHash;
            Hash displayName;
            Hash displayNameShort;
            Hash description;
            Hash wzDescription;
            Hash zmDescription;
            uintptr_t previewImage; // GfxImage*
            uintptr_t previewImageLarge; // GfxImage*
            uintptr_t wzPreviewImage; // GfxImage*
            uintptr_t wzPreviewImageLarge; // GfxImage*
            uintptr_t zmPreviewImage; // GfxImage*
            uintptr_t zmPreviewImageLarge; // GfxImage*
            uint32_t globalItemIndex;
            uint32_t itemIndex;
            int32_t scoreToUnlock;
            int32_t lowestScoreToUnlockAllowed;
            uint64_t unka8;
            uint32_t unkb0;
            int32_t allocation;
            int32_t modes;
            int32_t unkbc;
            int32_t unkc0;
            int32_t unkc4;
            int32_t unkc8;
            int32_t unkcc;
            bool attributeItems;
            uint32_t unkd4;
            uint32_t loadoutSlotIndex;
            int32_t itemGroupIndex; // itemGroup_t
            uint64_t unke0;
            uint64_t unke8;
            uint32_t attachmentsCount;
            uint32_t unkf4;
            uintptr_t attachments; // eAttachment*
            uint64_t unk100;
            uint64_t unk108;
            uint64_t unk110;
            uint64_t unk118;
            uint64_t unk120;
            uint64_t unk128;
            uint64_t unk130;
            uint64_t unk138;
        };
        size_t readFile = 0;

        // 12D15670
        auto pool = std::make_unique<UnlockableItemTableElem[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            if (p.nameHash.name < MIN_HASH_VAL) {
                continue;
            }
            auto* nameclear = ReadTmpStr(proc, p.name);

            hashutils::Add(nameclear);

            auto n = hashutils::ExtractPtr(p.nameHash.name);


            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/unlockableitem/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.nameHash.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/unlockableitem/file_%llx.json", opt.m_output, p.nameHash.name);
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

            out << "{\n";
            auto addGFXName = [&proc, &out](const char* title, uintptr_t ptr) {
                if (!ptr) {
                    return;
                }
                auto name = proc.ReadMemory<uint64_t>(ptr + 0x20);
                if (!name) {
                    return;
                }
                utils::Padding(out, 1) << "\"" << title << "\": \"#" << hashutils::ExtractTmp("hash", name) << "\",\n";
            };

            utils::Padding(out, 1) << "\"name\": \"" << nameclear << "\",\n";
            utils::Padding(out, 1) << "\"nameHash\": \"#" << hashutils::ExtractTmp("hash", p.nameHash.name) << "\",\n";
            utils::Padding(out, 1) << "\"displayName\": \"#" << hashutils::ExtractTmp("hash", p.displayName.name) << "\",\n";
            utils::Padding(out, 1) << "\"displayNameShort\": \"#" << hashutils::ExtractTmp("hash", p.displayNameShort.name) << "\",\n";
            utils::Padding(out, 1) << "\"description\": \"#" << hashutils::ExtractTmp("hash", p.description.name) << "\",\n";
            utils::Padding(out, 1) << "\"wzDescription\": \"#" << hashutils::ExtractTmp("hash", p.wzDescription.name) << "\",\n";
            utils::Padding(out, 1) << "\"zmDescription\": \"#" << hashutils::ExtractTmp("hash", p.zmDescription.name) << "\",\n";

            addGFXName("previewImage", p.previewImage);
            addGFXName("previewImageLarge", p.previewImageLarge);
            addGFXName("wzPreviewImage", p.wzPreviewImage);
            addGFXName("wzPreviewImageLarge", p.wzPreviewImageLarge);
            addGFXName("zmPreviewImage", p.zmPreviewImage);
            addGFXName("zmPreviewImageLarge", p.zmPreviewImageLarge);

            utils::Padding(out, 1) << "\"globalItemIndex\": " << std::dec << p.globalItemIndex << ",\n";
            utils::Padding(out, 1) << "\"itemIndex\": " << std::dec << p.itemIndex << ",\n";
            utils::Padding(out, 1) << "\"scoreToUnlock\": " << std::dec << p.scoreToUnlock << ",\n";
            utils::Padding(out, 1) << "\"lowestScoreToUnlockAllowed\": " << std::dec << p.lowestScoreToUnlockAllowed << ",\n";
            utils::Padding(out, 1) << "\"allocation\": " << std::dec << p.allocation << ",\n";
            utils::Padding(out, 1) << "\"attributeItems\": " << (p.attributeItems ? "true" : "false") << ",\n";
            utils::Padding(out, 1) << "\"itemGroupIndex\": \"" << (p.itemGroupIndex == -1 ? "none" : itemGroupNames[p.itemGroupIndex]) << "\",\n";
            utils::Padding(out, 1) << "\"attachments\": [";

            if (p.attachmentsCount) {
                auto attachments = std::make_unique<uint32_t[]>(p.attachmentsCount);

                if (!proc.ReadMemory(&attachments[0], p.attachments, sizeof(attachments[0]) * p.attachmentsCount)) {
                    std::cerr << "Can't read attachments\n";
                    break;
                }

                for (size_t j = 0; j < p.attachmentsCount; j++) {
                    if (j) out << ",";
                    utils::Padding(out << "\n", 2);;

                    if (attachments[j] < ARRAYSIZE(attachmentsNames)) {
                        out << "\"" << attachmentsNames[attachments[j]] << "\"";
                    }
                    else {
                        out << std::dec << attachments[j];
                    }

                }
                utils::Padding(out << "\n", 1) << "]\n";
            }
            else {
                out << "]\n";
            }


            out << "}";

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";

    }
    if (ShouldHandle(ASSET_TYPE_UNLOCKABLE_ITEM_TABLE)) {
        size_t readFile = 0;

        struct UnlockableItemTable { // 50
            Hash name;
            uint32_t unk10;
            uint32_t elements_count;
            uintptr_t elements; // UnlockableItemTableElem**
            uint32_t unk20;
            uint32_t gadgets;
            uint32_t bubblegumpack;
            uint32_t speciality;
            uint32_t talisman;
            uint32_t talent;
            uint32_t bonuscard;
            uint32_t unk3c;
            uint32_t unk40;
            uint32_t unk44;
            uintptr_t unk48;
        };


        // 12D15670
        auto pool = std::make_unique<UnlockableItemTable[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto n = hashutils::ExtractPtr(p.name.name);


            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/unlockableitem/table/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/unlockableitem/table/file_%llx.json", opt.m_output, p.name.name);
            }

            //if (!p.gameTypes || !proc.ReadMemory<uint64_t>(p.gameTypes)) {
            //    std::cerr << "error when reading buffer at " << p.gameTypes << "\n";
            //    continue;
            //}

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

            out << "{\n";
            utils::Padding(out, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(out, 1) << "\"gadgets\": " << std::dec << p.gadgets << ",\n";
            utils::Padding(out, 1) << "\"bubblegumpack\": " << std::dec << p.bubblegumpack << ",\n";
            utils::Padding(out, 1) << "\"speciality\": " << std::dec << p.speciality << ",\n";
            utils::Padding(out, 1) << "\"talisman\": " << std::dec << p.talisman << ",\n";
            utils::Padding(out, 1) << "\"talent\": " << std::dec << p.talent << ",\n";
            utils::Padding(out, 1) << "\"bonuscard\": " << std::dec << p.bonuscard << ",\n";
            utils::Padding(out, 1) << "\"entries\": [";

            if (p.elements_count) {
                auto entries = std::make_unique<uintptr_t[]>(p.elements_count);


                if (!proc.ReadMemory(&entries[0], p.elements, sizeof(uintptr_t) * p.elements_count)) {
                    std::cerr << "Can't read elements\n";
                    break;
                }
                for (size_t i = 0; i < p.elements_count; i++) {
                    auto name = proc.ReadMemory<uint64_t>(entries[i] + 0x8); // skip name
                    if (i) out << ",";
                    auto* nameclear = ReadTmpStr(proc, proc.ReadMemory<uintptr_t>(entries[i]));

                    if (*nameclear != '<') {
                        hashutils::Add(nameclear);
                    }

                    utils::Padding(out << "\n", 2) << "\"#" << hashutils::ExtractTmp("hash", name) << "\"";
                }

                utils::Padding(out << "\n", 1) << "]\n";
            }
            else {
                out << "]\n";
            }

            out << "}";

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_PLAYER_ROLE_CATEGORY_TABLE)) {
        size_t readFile = 0;

        struct PlayerRoleCategoryTable
        {
            Hash name;
            uint32_t numPlayerRoleCategories;
            uintptr_t playerRoleCategories; // PlayerRoleCategoryPtr*
        };


        // 12D15670
        auto pool = std::make_unique<PlayerRoleCategoryTable[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto n = hashutils::ExtractPtr(p.name.name);


            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/playerrolecategory/table/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/playerrolecategory/table/file_%llx.json", opt.m_output, p.name.name);
            }

            //if (!p.gameTypes || !proc.ReadMemory<uint64_t>(p.gameTypes)) {
            //    std::cerr << "error when reading buffer at " << p.gameTypes << "\n";
            //    continue;
            //}

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

            out << "{\n";
            utils::Padding(out, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(out, 1) << "\"entries\": [";

            if (p.numPlayerRoleCategories) {
                auto entries = std::make_unique<uintptr_t[]>(p.numPlayerRoleCategories);


                if (!proc.ReadMemory(&entries[0], p.playerRoleCategories, sizeof(uintptr_t) * p.numPlayerRoleCategories)) {
                    std::cerr << "Can't read elements\n";
                    break;
                }
                for (size_t i = 0; i < p.numPlayerRoleCategories; i++) {
                    if (i) out << ",";
                    auto name = proc.ReadMemory<uint64_t>(entries[i]); // skip name
                    utils::Padding(out << "\n", 2) << "\"#" << hashutils::ExtractTmp("hash", name) << "\"";
                }

                utils::Padding(out << "\n", 1) << "]\n";
            }
            else {
                out << "]\n";
            }

            out << "}";

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_PLAYER_ROLE_CATEGORY)) {
        size_t readFile = 0;


        struct PlayerRoleCategory {
            Hash name;
            Hash displayName;
            Hash description;
            uintptr_t icon;
            int sortOrder;
            SB_ObjectsArray kvp;
        };

        // 12D15670
        auto pool = std::make_unique<PlayerRoleCategory[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto n = hashutils::ExtractPtr(p.name.name);


            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/playerrolecategory/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/playerrolecategory/file_%llx.json", opt.m_output, p.name.name);
            }

            //if (!p.gameTypes || !proc.ReadMemory<uint64_t>(p.gameTypes)) {
            //    std::cerr << "error when reading buffer at " << p.gameTypes << "\n";
            //    continue;
            //}

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

            out << "{\n";
            
            utils::Padding(out, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(out, 1) << "\"displayName\": \"#" << hashutils::ExtractTmp("hash", p.displayName.name) << "\",\n";
            utils::Padding(out, 1) << "\"description\": \"#" << hashutils::ExtractTmp("hash", p.description.name) << "\",\n";
            utils::Padding(out, 1) << "\"sortOrder\": " << std::dec << p.sortOrder << ",\n";
            utils::Padding(out, 1) << "\"fields\": ";

            if (!ReadSBObject(proc, out, 1, p.kvp, strings)) {
                std::cerr << "Can't read cat kvp array\n";
                break;
            }

            out << "\n}\n";

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_STORAGEFILE)) {
        size_t readFile = 0;

        struct StorageFile {
            uintptr_t nameStr; // const char*
            Hash name;
            Hash ddlName;
            uintptr_t ddl; // ? DDL?
            uint64_t unk30;
            uint64_t unk38;
            uint64_t unk40;
            uint64_t unk48;
            uint64_t unk50;
            uint64_t unk58;
        };


        // 12D15670
        auto pool = std::make_unique<StorageFile[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }


            auto* str = ReadTmpStr(proc, p.nameStr);

            auto ddlGuess = std::format("gamedata/ddl/generated/{}.ddl", str);

            hashutils::Add(str);
            hashutils::Add(ddlGuess.c_str());

            auto n = hashutils::ExtractPtr(p.name.name);


            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/storagefile/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/storagefile/file_%llx.json", opt.m_output, p.name.name);
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

            out << "{\n";
            utils::Padding(out, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(out, 1) << "\"nameStr\": \"" << str << "\",\n";
            utils::Padding(out, 1) << "\"ddl\": \"#" << hashutils::ExtractTmp("hash", p.ddlName.name) << "\"\n";
            out << "}";

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_STORAGEFILELIST)) {
        size_t readFile = 0;

        struct StorageFileList {
            Hash name;
            uint32_t count;
            uintptr_t list;
        };


        // 12D15670
        auto pool = std::make_unique<StorageFileList[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto n = hashutils::ExtractPtr(p.name.name);


            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/storagefile/list/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/storagefile/list/file_%llx.json", opt.m_output, p.name.name);
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

            out << "{\n";
            utils::Padding(out, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(out, 1) << "\"list\": [";

            if (p.count) {
                auto entries = std::make_unique<uintptr_t[]>(p.count);

                if (!proc.ReadMemory(&entries[0], p.list, p.count * sizeof(entries[0]))) {
                    std::cerr << "Can't read entries\n";
                    break;
                }

                for (size_t j = 0; j < p.count; j++) {
                    if (j) out << ",";

                    auto* str = ReadTmpStr(proc, proc.ReadMemory<uint64_t>(entries[j]));

                    hashutils::Add(str);
                    utils::Padding(out << "\n", 2) << "\"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(entries[j] + 0x8)) << "\"";
                }

                utils::Padding(out << "\n", 1) << "]\n";
            }
            else {
                out << "]\n";
            }

            out << "}";

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_MAPTABLE_LOADING_IMAGES)) {
        size_t readFile = 0;

        struct MapTableLoadingImages
        {
            XHash name;
            uint64_t imageCount;
            uintptr_t images; // GfxImage**
            uint64_t mapCount;
        };

        auto pool = std::make_unique<MapTableLoadingImages[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto n = hashutils::ExtractPtr(p.name.name);


            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/map/loadingimages/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/map/loadingimages/file_%llx.json", opt.m_output, p.name.name);
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

            out << "{\n";
            utils::Padding(out, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(out, 1) << "\"mapCount\": " << std::dec << p.mapCount << ",\n";

            utils::Padding(out, 1) << "\"images\": [";

            size_t count = p.imageCount;

            if (count) {
                auto entries = std::make_unique<uintptr_t[]>(count);

                if (!proc.ReadMemory(&entries[0], p.images, count * sizeof(entries[0]))) {
                    std::cerr << "Can't read entries\n";
                    break;
                }

                for (size_t j = 0; j < count; j++) {
                    if (j) out << ",";

                    auto* str = ReadTmpStr(proc, proc.ReadMemory<uint64_t>(entries[j]));

                    hashutils::Add(str);
                    utils::Padding(out << "\n", 2) << "\"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(entries[j] + 0x20)) << "\"";
                }

                utils::Padding(out << "\n", 1) << "]\n";
            }
            else {
                out << "]\n";
            }

            out << "}";

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_MAPTABLE_PREVIEW_IMAGES)) {
        size_t readFile = 0;

        struct MapTablePreviewImages
        {
            XHash name;
            uint64_t imageCount;
            uintptr_t images; // GfxImage**
            uint64_t mapCount;
        };

        auto pool = std::make_unique<MapTablePreviewImages[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto n = hashutils::ExtractPtr(p.name.name);


            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/map/previewimages/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/map/previewimages/file_%llx.json", opt.m_output, p.name.name);
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

            out << "{\n";
            utils::Padding(out, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(out, 1) << "\"mapCount\": " << std::dec << p.mapCount << ",\n";

            utils::Padding(out, 1) << "\"images\": [";

            size_t count = p.imageCount;

            if (count) {
                auto entries = std::make_unique<uintptr_t[]>(count);

                if (!proc.ReadMemory(&entries[0], p.images, count * sizeof(entries[0]))) {
                    std::cerr << "Can't read entries\n";
                    break;
                }

                for (size_t j = 0; j < count; j++) {
                    if (j) out << ",";

                    auto* str = ReadTmpStr(proc, proc.ReadMemory<uint64_t>(entries[j]));

                    hashutils::Add(str);
                    utils::Padding(out << "\n", 2) << "\"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(entries[j] + 0x20)) << "\"";
                }

                utils::Padding(out << "\n", 1) << "]\n";
            }
            else {
                out << "]\n";
            }

            out << "}";

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_STREAMKEY, false)) {
        size_t readFile = 0;

        // wtf?
        struct StreamKey {
            XHash name;
            uint64_t unk10;
            uint64_t unk18;
            uint64_t unk20;
            uint64_t unk28;
            uint64_t unk30;
            uint64_t xanim;
            uint32_t unk40;
            uint32_t unk44;
        };



        // 12D15670
        auto pool = std::make_unique<StreamKey[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto n = hashutils::ExtractPtr(p.name.name);


            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/tables/streamkey/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/tables/streamkey/file_%llx.json", opt.m_output, p.name.name);
            }

            //if (!p.gameTypes || !proc.ReadMemory<uint64_t>(p.gameTypes)) {
            //    std::cerr << "error when reading buffer at " << p.gameTypes << "\n";
            //    continue;
            //}

            std::filesystem::path file(dumpbuff);
            std::filesystem::create_directories(file.parent_path(), ec);

            std::cout << "->" << file;

            if (!std::filesystem::exists(file, ec)) {
                readFile++;
                std::cout << " (new)";
            }

            std::ofstream defout{ file };

            if (!defout) {
                std::cerr << "Can't open output file\n";
                break;
            }

            defout << "{\n";
            utils::Padding(defout, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(defout, 1) << "\"xanim\": \"#" << std::hex << hashutils::ExtractTmp("hash", p.xanim) << "\",\n";

            utils::Padding(defout, 1) << "\"unk10\": \"" << std::hex << p.unk10 << "\",\n";
            utils::Padding(defout, 1) << "\"unk18\": \"" << std::hex << p.unk18 << "\",\n";
            utils::Padding(defout, 1) << "\"unk20\": \"" << std::hex << p.unk20 << "\",\n";
            utils::Padding(defout, 1) << "\"unk28\": \"" << std::hex << p.unk28 << "\",\n";
            utils::Padding(defout, 1) << "\"unk30\": \"" << std::hex << p.unk30 << "\",\n";
            utils::Padding(defout, 1) << "\"unk40\": \"" << std::hex << p.unk40 << "\",\n";
            utils::Padding(defout, 1) << "\"unk44\": \"" << std::hex << p.unk44 << "\"\n";
            tool::pool::WriteHex(defout, 0, (byte*)&p, entry.itemSize, proc);
            defout << "}";

            defout.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_IMAGE, false)) {
        size_t readFile = 0;

        enum GfxImageFlags : uint32_t {
            GFXF_UNK_2 = 0x2,
            GFXF_CUBE = 0x4,
            GFXF_3D = 0x8,

            GFXF_ARRAY = 0x200,
        };
        struct GfxImage
        {
            uintptr_t unk0;
            uintptr_t unk8;
            uintptr_t unk10;
            uintptr_t pixels; // byte*
            Hash name;
            uintptr_t unk30;
            uint64_t unk38;
            uint64_t unk40;
            uint64_t unk48;
            uint64_t unk50;
            uint32_t imageFlags;
            uint32_t unk5c;
            uint32_t totalSize;
            uint32_t imageFormat; // GfxPixelFormat
            uint16_t width;
            uint16_t height;
            uint16_t depth;
            uint16_t unk6e;
            byte unk70;
            byte unk71;
            uint16_t unk72;
            uint16_t unk74;
            byte unk76;
            byte alignment;
            byte unk78;
            byte mapType; // MapType
            byte unk7a;
            byte unk7b;
            uint32_t unk7c;
            uint64_t unk80;
        };




        // 12D15670
        auto pool = std::make_unique<GfxImage[]>(entry.itemAllocCount);

        char dumpbuff[MAX_PATH + 10];
        sprintf_s(dumpbuff, "%s/tables/ximages.csv", opt.m_output);
        std::cout << dumpbuff << "\n";
        std::filesystem::path file(dumpbuff);
        std::filesystem::create_directories(file.parent_path(), ec);

        std::ofstream out{ file };

        if (!out) {
            std::cerr << "Can't open output\n";
            return tool::BASIC_ERROR;
        }

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }

        out << "name,width,height,flags,format,align,type";

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            const char* type;

            if (p.imageFlags & (GfxImageFlags::GFXF_ARRAY | GfxImageFlags::GFXF_ARRAY)) {
                type = "cubearray";
            }
            else if (p.imageFlags & GfxImageFlags::GFXF_ARRAY) {
                type = "array";
            }
            else if (p.imageFlags & GfxImageFlags::GFXF_3D) {
                type = "3d";
            }
            else if (p.imageFlags & GfxImageFlags::GFXF_ARRAY) {
                type = "2darray";
            }
            else {
                type = "2d";
            }

            out << "\n"
                << std::dec
                << hashutils::ExtractTmp("hash", p.name.name) << ","
                << p.width << ","
                << p.height << ","
                << std::hex
                << (int)p.imageFlags << ","
                << (int)p.imageFormat << ","
                << (int)p.alignment << ","
                << type
                ;



        }
        out.close();
        std::cout << "Dump " << file << "\n";
    }
    if (ShouldHandle(ASSET_TYPE_LABELSTORELIST)) {
        size_t readFile = 0;

        struct LabelStoreList {
            XHash name;
            uint64_t count;
            uintptr_t stores;
        };
        struct LabelStore {
            XHash name;
            uint64_t count;
            uintptr_t stores;
        };

        auto pool = std::make_unique<LabelStoreList[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto n = hashutils::ExtractPtr(p.name.name);


            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/labelstore/list/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/labelstore/list/file_%llx.json", opt.m_output, p.name.name);
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

            out << "{\n";
            utils::Padding(out, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(out, 1) << "\"stores\": [";

            if (p.count) {
                auto buff = std::make_unique<uintptr_t[]>(p.count);

                if (!proc.ReadMemory(&buff[0], p.stores, sizeof(buff[0]) * p.count)) {
                    LOG_ERROR("Can't read memory");
                    break;
                }
                LabelStore store{};
                for (size_t j = 0; j < p.count; j++) {
                    if (!proc.ReadMemory(&store, buff[j], sizeof(store))) {
                        LOG_ERROR("Can't read memory");
                        break;
                    }
                    if (j) out << ",";
                    out << "\n";

                    utils::Padding(out, 2) << "\"#" << hashutils::ExtractTmp("hash", store.name.name) << "\"";
                }

                utils::Padding(out << "\n", 1) << "]\n";
            }
            else {
                out << "]\n";
            }
            out << "}";

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    if (ShouldHandle(ASSET_TYPE_LABELSTORE)) {
        size_t readFile = 0;

        struct LabelStore {
            XHash name;
            uint64_t count;
            uintptr_t stores;
        };

        auto pool = std::make_unique<LabelStore[]>(entry.itemAllocCount);

        if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemAllocCount)) {
            std::cerr << "Can't read pool data\n";
            return tool::BASIC_ERROR;
        }
        char dumpbuff[MAX_PATH + 10];

        for (size_t i = 0; i < entry.itemAllocCount; i++) {
            auto& p = pool[i];

            if (p.name.name < MIN_HASH_VAL) {
                continue;
            }

            auto n = hashutils::ExtractPtr(p.name.name);


            std::cout << std::dec << i << ": ";

            if (n) {
                std::cout << n;
                sprintf_s(dumpbuff, "%s/labelstore/%s.json", opt.m_output, n);
            }
            else {
                std::cout << "file_" << std::hex << p.name.name << std::dec;
                sprintf_s(dumpbuff, "%s/labelstore/file_%llx.json", opt.m_output, p.name.name);
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

            out << "{\n";
            utils::Padding(out, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", p.name.name) << "\",\n";
            utils::Padding(out, 1) << "\"sku\": [";

            if (p.count) {
                auto buff = std::make_unique<uintptr_t[]>(p.count);

                if (!proc.ReadMemory(&buff[0], p.stores, sizeof(buff[0]) * p.count)) {
                    LOG_ERROR("Can't read memory");
                    break;
                }
                XHash val{};
                for (size_t j = 0; j < p.count; j++) {
                    if (!proc.ReadMemory(&val, buff[j], sizeof(val))) {
                        LOG_ERROR("Can't read memory");
                        break;
                    }
                    if (j) out << ",";
                    out << "\n";

                    utils::Padding(out, 2) << "\"#" << hashutils::ExtractTmp("hash", val.name) << "\"";
                }

                utils::Padding(out << "\n", 1) << "]\n";
            }
            else {
                out << "]\n";
            }
            out << "}";

            out.close();

            std::cout << "\n";

        }
        std::cout << "Dump " << readFile << " new file(s)\n";
    }
    for (size_t i = 0; i < ASSET_TYPE_COUNT; i++) {
        auto id = (XAssetType)i;
        if (ShouldHandle(id, false)) {
            std::cout << "Item data\n";

            auto readAlloc = std::min(entry.itemAllocCount, 500); // max to 500

            auto raw = std::make_unique<byte[]>(entry.itemSize * readAlloc);

            if (!proc.ReadMemory(&raw[0], entry.pool, entry.itemSize * readAlloc)) {
                std::cerr << "Can't read pool data\n";
                return tool::BASIC_ERROR;
            }

            char dumpbuff[MAX_PATH + 10];
            for (size_t i = 0; i < readAlloc; i++) {
                sprintf_s(dumpbuff, "%s/rawpool/%s/%lld.json", opt.m_output, XAssetNameFromId(id), i);

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
    }

    if (strings.size()) {
        std::ofstream outStr{ std::format("{}/scriptbundle_str.txt", opt.m_output) };

        if (outStr) {
            for (const auto& st : strings) {
                outStr << st << "\n";
            }
            outStr.close();
        }
    }


    //tool::pool::WriteHex(defout, 0, (byte*)&p, entry.itemSize, proc);
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

    char strBuff[0x100] = { 0 };

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

    
    std::filesystem::path out{ "bgpool/bo4" };

    std::filesystem::create_directories(out);
    hashutils::ReadDefaultFile();

    std::ofstream outInfo{ out / "caches.csv" };

    if (!outInfo) {
        std::cerr << "Can't open caches.csv file\n";
        return tool::BASIC_ERROR;
    }


    outInfo << "id,name,start,count";

    auto pool = proc[0x5D9D6D0];

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
ADD_TOOL(dpbo4, "bo4", " [pool]+", "Black Ops 4 dump pool", L"BlackOps4.exe", pooltool);
ADD_TOOL(dpnbo4, "bo4", "", "dump pool names", L"BlackOps4.exe", pooltoolnames);
ADD_TOOL(dbgcachebo4, "bo4", "", "dump bg cache", L"BlackOps4.exe", dumpbgcache);
ADD_TOOL(dbmtstrsbo4, "bo4", "", "dump mt strings", L"BlackOps4.exe", dbmtstrs);
ADD_TOOL(dbgpbo4, "bo4", "", "dump bg pool", L"BlackOps4.exe", dbgp);
