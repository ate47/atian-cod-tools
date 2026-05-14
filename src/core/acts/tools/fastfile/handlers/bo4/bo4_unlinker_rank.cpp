#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/fastfile/handlers/bo4/bo4_unlinker_scriptbundle.hpp>
#include <core/hashes/raw_file_extractor.hpp>
#include <tools/utils/data_utils.hpp>

namespace fastfile::handlers::bo4::map {
    using namespace games::bo4::pool;
    struct RankInfoReward {
        XHash prestigeTarget;
        XHash rewardName;
    };
    struct RankInfo {
        XHash name;
        uint32_t level;
        uint32_t minXp;
        uint32_t maxXp;
        XHash shortNameRef;
        XHash fullNameRef;
        XHash ingameNameRef;
        GfxImage* icon;
        GfxImage* iconLarge;
        uint64_t rewardsCount;
        RankInfoReward* rewards;
    };

    enum PrestigeTypeOfOrigin : uint32_t {
        ORIGIN_TITLE_T4 = 0,
        ORIGIN_TITLE_T5 = 1,
        ORIGIN_TITLE_T6 = 2,
        ORIGIN_TITLE_T7 = 3,
        ORIGIN_TITLE_T8 = 4,
    };

    const char* PrestigeTypeOfOriginNames[]{
        "t4", "t5", "t6", "t7", "t8"
    };

    struct PrestigeInfo {
        XHash name;
        XHash displayName;
        uint32_t unlockLevel;
        uint32_t winsRequired;
        PrestigeTypeOfOrigin titleOfOrigin;
        uint32_t unk2c;
        GfxImage* iconName;
        GfxImage* iconNameLarge;
    };

    struct RankTable {
        XHash name;
        uint64_t ranksCount;
        RankInfo** ranks;
    };

    struct PrestigeTable {
        XHash name;
        int64_t prestigesCount;
        PrestigeInfo** prestiges;
    };

    class RankInfoWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            RankInfo* asset{ (RankInfo*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "rank" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump rankinfo{}", outFile.string());
            BO4JsonWriter json{};

            json.BeginObject();
            json.WriteFieldValueXHash("name", asset->name);
            json.WriteFieldValueXHash("shortNameRef", asset->shortNameRef);
            json.WriteFieldValueXHash("fullNameRef", asset->fullNameRef);
            json.WriteFieldValueXHash("ingameNameRef", asset->ingameNameRef);
            json.WriteFieldValueNumber("level", asset->level);
            json.WriteFieldValueNumber("minXp", asset->minXp);
            json.WriteFieldValueNumber("maxXp", asset->maxXp);
            json.WriteFieldValueXAsset("icon", games::bo4::pool::XAssetType::ASSET_TYPE_IMAGE, asset->icon);
            json.WriteFieldValueXAsset("iconLarge", games::bo4::pool::XAssetType::ASSET_TYPE_IMAGE, asset->iconLarge);
            if (asset->rewards) {
                json.WriteFieldNameString("rewards");
                json.BeginArray();
                for (size_t i = 0; i < asset->rewardsCount; i++) {
                    json.BeginObject();
                    json.WriteFieldValueXHash("prestigeTarget", asset->rewards[i].prestigeTarget);
                    json.WriteFieldValueXHash("rewardName", asset->rewards[i].rewardName);
                    json.EndObject();
                }
                json.EndArray();
            }
            json.EndObject();


            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };

    class RankTableWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            RankTable* asset{ (RankTable*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "rank" / "table" / std::format("{}.csv", hashutils::ExtractTmp("file", asset->name)) };

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump ranktable {}", outFile.string());

            utils::OutFileCE os{ outFile, true };

            os << "id,name";

            for (size_t i = 0; i < asset->ranksCount; i++) {
                os << "\n" << std::dec << i << ",";
                if (asset->ranks[i]) {
                    os << "#" << hashutils::ExtractTmp("hash", asset->ranks[i]->name);
                }
                else {
                    os << "null";
                }
            }
        }
    };


    class PrestigeInfoWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            PrestigeInfo* asset{ (PrestigeInfo*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "prestige" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump rankinfo{}", outFile.string());
            BO4JsonWriter json{};

            json.BeginObject();
            json.WriteFieldValueXHash("name", asset->name);
            json.WriteFieldValueXHash("displayName", asset->displayName);
            json.WriteFieldValueXAsset("iconName", games::bo4::pool::XAssetType::ASSET_TYPE_IMAGE, asset->iconName);
            json.WriteFieldValueXAsset("iconNameLarge", games::bo4::pool::XAssetType::ASSET_TYPE_IMAGE, asset->iconNameLarge);
            json.WriteFieldValueNumber("unlockLevel", asset->unlockLevel);
            json.WriteFieldValueNumber("winsRequired", asset->winsRequired);
            json.WriteFieldValueString("titleOfOrigin", asset->titleOfOrigin >= ACTS_ARRAYSIZE(PrestigeTypeOfOriginNames) ? utils::va("unknown:%d", asset->titleOfOrigin) : PrestigeTypeOfOriginNames[asset->titleOfOrigin]);
            json.WriteFieldValueUnknown("unk2c", asset->unk2c);
            json.EndObject();


            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };

    class PrestigeTableWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            PrestigeTable* asset{ (PrestigeTable*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "prestige" / "table" / std::format("{}.csv", hashutils::ExtractTmp("file", asset->name)) };

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump prestigetable {}", outFile.string());

            utils::OutFileCE os{ outFile, true };

            os << "id,name";

            for (size_t i = 0; i < asset->prestigesCount; i++) {
                os << "\n" << std::dec << i << ",";
                if (asset->prestiges[i]) {
                    os << "#" << hashutils::ExtractTmp("hash", asset->prestiges[i]->name);
                }
                else {
                    os << "null";
                }
            }
        }
    };

    utils::MapAdder<RankInfoWorker, games::bo4::pool::XAssetType, Worker> implr{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_RANK };
    utils::MapAdder<RankTableWorker, games::bo4::pool::XAssetType, Worker> implrt{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_RANKTABLE };
    utils::MapAdder<PrestigeInfoWorker, games::bo4::pool::XAssetType, Worker> implp{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_PRESTIGE };
    utils::MapAdder<PrestigeTableWorker, games::bo4::pool::XAssetType, Worker> implpt{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_PRESTIGETABLE };
}