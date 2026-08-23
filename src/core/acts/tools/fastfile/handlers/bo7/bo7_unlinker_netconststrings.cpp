#include <includes.hpp>
#include <core/config.hpp>
#include <tools/fastfile/handlers/handler_game_bo7.hpp>

namespace fastfile::handlers::bo7::netconststrings {

    union NetConstString {
        const char* str;
        XHash64 hash;
    };

    const char* nameSuffixes[]{
        "level",
        "level_entities",
    };

    static std::filesystem::path GetCfgDir() {
        std::string scanpath{ core::config::GetString("data.dir", "") };
        if (scanpath.empty()) {
            return utils::GetProgDir() / "data" / "nsc_types.json";
        }
        return std::filesystem::path{ scanpath } / "nsc_types.json";
    }

    void InitHashes(const char* ffname) {
        static core::config::Config nscTypes{ GetCfgDir() };

        core::config::RapidJsonGeneric& keys{ nscTypes.GetVal("keys") };

        if (keys.IsNull()) {
            return; // not set
        }
        if (!keys.IsObject()) {
            LOG_WARNING("Invalid keys field in {}, can't init hashes", GetCfgDir().string());
            return;
        }

        if (!ffname) {
            core::config::RapidJsonGeneric& suffixes{ nscTypes.GetVal("suffixes") };
            if (suffixes.IsNull()) {
                return; // not set
            }
            if (!suffixes.IsArray()) {
                LOG_WARNING("Invalid suffixes field in {}, can't init hashes", GetCfgDir().string());
                return;
            }
            // common ones
            for (core::config::RapidJsonGeneric& valSuffix : suffixes.GetArray()) {
                const char* nameSuffix{ valSuffix.GetString() };
                for (auto& [valName, str] : keys.GetObj()) {
                    const char* name{ valName.GetString() };
                    const char* str{ utils::va("ncs_%s_%s", name, nameSuffix) };
                    hashutils::AddPrecomputed(hash::HashIWAsset(str), str, true);
                }
            }
        } else {
            // ff specific
            for (auto& [valName, str] : keys.GetObj()) {
                const char* name{ valName.GetString() };
                const char* str{ utils::va("ncs_%s_%s", name, ffname) };
                hashutils::AddPrecomputed(hash::HashIWAsset(str), str, true);
            }
        }
    }

    typedef uint32_t NetConstStringsType;

    enum NetConstStringsSource : byte {
        NCSS_CODE = 0,
        NCSS_GLOBAL = 1,
        NCSS_INGAME = 2,
        NCSS_LEVEL = 3,
        NCSS_UNK4 = 4,
        NCSS_VOID = 5,
    };

    const char* sourceNames[]{
        "code", "global", "ingame", "level", "unk4", "void",
    };

    struct NetConstStrings {
        uint64_t name;
        NetConstStringsType type;
        NetConstStringsSource source;
        bool isHashed;
        uint16_t unk0e;
        bool localized;
        uint32_t stringsCount;
        uint32_t checksum;
        uint32_t unk1c;
        NetConstString* strings;
    };
    static_assert(sizeof(NetConstStrings) == 0x28);

    class ImplWorker : public Worker {
        using Worker::Worker;

        void PreLoadWorker(fastfile::FastFileContext* ctx) override { InitHashes(ctx ? ctx->ffname : nullptr); }

        void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
            NetConstStrings* asset{ (NetConstStrings*)ptr };
            HandlerJsonWriter json{};

            json.BeginObject();

            json.WriteFieldValueXHash("name", asset->name);
            json.WriteFieldValueNumber("type", asset->type);
            if (asset->source >= 0 && asset->source < ACTS_ARRAYSIZE(sourceNames)) {
                json.WriteFieldValueString("source", sourceNames[asset->source]);
            } else {
                json.WriteFieldValueNumber<uint32_t>("source", asset->source);
            }
            json.WriteFieldValueBool("isHashed", asset->isHashed);
            json.WriteFieldValueBool("localized", asset->localized);
            json.WriteFieldValueNumber<uint32_t>("checksum", asset->checksum);
            if (opt.testDump) {
                json.WriteFieldValueUnknown("unk0e", asset->unk0e); // empty?
                json.WriteFieldValueUnknown("unk1c", asset->unk1c); // empty?
            }

            json.WriteFieldNameString("strings");
            json.BeginArray();
            if (asset->isHashed) {
                for (size_t i = 0; i < asset->stringsCount; i++) {
                    json.WriteValueHash(asset->strings[i].hash);
                }
            } else {
                for (size_t i = 0; i < asset->stringsCount; i++) {
                    json.WriteValueString(asset->strings[i].str);
                }
            }
            json.EndArray();
            json.EndObject();

            std::filesystem::path outFile{ opt.m_output / gamePath / "source" / "tables" / "netconststrings" /
                                           fastfile::GetCurrentContext().ffname /
                                           std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };
            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump netconststrings {}", outFile.string());

            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };

    utils::MapAdder<ImplWorker, SatHashAssetType, Worker> impl{ GetWorkers(),
                                                                SatHashAssetType::SATH_ASSET_NETCONSTSTRINGS,
                                                                sizeof(NetConstStrings) };
} // namespace fastfile::handlers::bo7::netconststrings