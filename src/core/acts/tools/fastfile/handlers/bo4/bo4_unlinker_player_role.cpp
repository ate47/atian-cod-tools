
#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/fastfile/handlers/bo4/bo4_unlinker_scriptbundle.hpp>
namespace {
    using namespace fastfile::handlers::bo4;

    // playerrolecategorytable

    struct PlayerRoleCategory {
        XHash name;
        XHash displayName;
        XHash description;
        GfxImage* icon;
        int32_t sortOrder;
        scriptbundle::SB_ObjectsArray bundle;
    };
    static_assert(sizeof(PlayerRoleCategory) == 0x60);

    // playerrolecategory

    struct PlayerRoleCategoryTable {
        XHash name;
        uint32_t numPlayerRoleCategories;
        PlayerRoleCategory** playerRoleCategories;
    };
    static_assert(sizeof(PlayerRoleCategoryTable) == 0x20);

    // playerroletemplate

    struct PlayerRoleTemplate {
        XHash name;
        XHash unk10;
        XHash unk4fa4ad655ced495;
        CharacterBodyType* bodyType;
        PlayerRoleCategory* category;
        PlayerMovementTunables* playerMovementTunables;
        PlayerTalentTemplate* playerTalentTemplate;
        XHash specialistEquipment;
        XHash specialistWeapon;
        XHash ultimateWeapon;
        scriptbundle::SB_ObjectsArray bundle;
        uint64_t unka0;
        uint64_t unka8;
        uint64_t unkb0;
        uint64_t unkb8;
        uint64_t unkc0;
        uint64_t unkc8;
        uint64_t unkd0;
        uint64_t unkd8;
        uint64_t unke0;
        uint64_t unke8;
        uint64_t unkf0;
        uint64_t unkf8;
        uint64_t unk100;
        uint64_t unk108;
        uint64_t unk110;
        uint64_t unk118;
        uint64_t unk120;
        uint64_t unk128;
        uint64_t unk130;
        uint64_t unk138;
        uint64_t unk140;
        uint64_t unk148;
        uint64_t unk150;
        uint64_t unk158;
        uint64_t unk160;
        uint64_t unk168;
        uint64_t unk170;
        uint64_t unk178;
        uint64_t unk180;
        uint64_t unk188;
        uint64_t unk190;
        uint64_t unk198;
        uint64_t unk1a0;
        uint64_t unk1a8;
        ScriptBundle* (*bundleLists)[4];
    };
    static_assert(sizeof(PlayerRoleTemplate) == 0x1b8);


    class ImplWorker : public Worker {
        using Worker::Worker;

        void Unlink(fastfile::FastFileOption& opt, void* ptr) {

            PlayerRoleCategory* asset{ (PlayerRoleCategory*)ptr };

            BO4JsonWriter json{};

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "player" / "role" / "category" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name))};
            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump player role category {}", outFile.string());

            json.BeginObject();
            json.WriteFieldValueXHash("name", asset->name);
            json.WriteFieldValueXHash("displayName", asset->displayName);
            json.WriteFieldValueXHash("description", asset->description);
            json.WriteFieldValueXAsset("icon", games::bo4::pool::XAssetType::ASSET_TYPE_IMAGE, asset->icon);
            json.WriteFieldValueNumber("sortOrder", asset->sortOrder);
            scriptbundle::WriteObject(json, "bundle", asset->bundle);

            json.EndObject();


            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };
    class ImplTableWorker : public Worker {
        using Worker::Worker;

        void Unlink(fastfile::FastFileOption& opt, void* ptr) {

            PlayerRoleCategoryTable* asset{ (PlayerRoleCategoryTable*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "player" / "role" / "category" / "table" / std::format("{}.csv", hashutils::ExtractTmp("file", asset->name))};
            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump player role category table {}", outFile.string());

            utils::OutFileCE os{ outFile, true };

            os << "id,name";

            for (size_t i = 0; i < asset->numPlayerRoleCategories; i++) {
                os << "\n" << std::dec << i << ",";
                if (asset->playerRoleCategories[i]) {
                    os << "#" << hashutils::ExtractTmp("hash", asset->playerRoleCategories[i]->name);
                }
                else {
                    os << "null";
                }
            }
        }
    };

    class ImplTemplateWorker : public Worker {
        using Worker::Worker;

        void Unlink(fastfile::FastFileOption& opt, void* ptr) {

            PlayerRoleTemplate* asset{ (PlayerRoleTemplate*)ptr };

            BO4JsonWriter json{};

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "player" / "role" / "template" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name))};
            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump player role template {}", outFile.string());

            json.BeginObject();
            json.WriteFieldValueXHash("name", asset->name);
            json.WriteFieldValueXHash("unk10", asset->unk10);
            json.WriteFieldValueXHash(0x4fa4ad655ced495, asset->unk4fa4ad655ced495);
            json.WriteFieldValueXAsset("bodyType", games::bo4::pool::XAssetType::ASSET_TYPE_CHARACTER_BODY_TYPE, asset->bodyType);
            json.WriteFieldValueXAsset("category", games::bo4::pool::XAssetType::ASSET_TYPE_PLAYER_ROLE_CATEGORY, asset->category);
            json.WriteFieldValueXAsset("playerMovementTunables", games::bo4::pool::XAssetType::ASSET_TYPE_PLAYER_MOVEMENT_TUNABLES, asset->playerMovementTunables);
            json.WriteFieldValueXAsset("playerTalentTemplate", games::bo4::pool::XAssetType::ASSET_TYPE_PLAYERTALENTTEMPLATE, asset->playerTalentTemplate);
            json.WriteFieldValueXAssetArray("bundleLists", games::bo4::pool::XAssetType::ASSET_TYPE_SCRIPTBUNDLE, ACTS_ARRAYSIZE(*asset->bundleLists), asset->bundleLists);
            json.WriteFieldValueXHash("specialistEquipment", asset->specialistEquipment);
            json.WriteFieldValueXHash("specialistWeapon", asset->specialistWeapon);
            json.WriteFieldValueXHash("ultimateWeapon", asset->ultimateWeapon);

            if (opt.testDump) {
                json.WriteFieldValueUnknown("unka0", asset->unka0);
                json.WriteFieldValueUnknown("unka8", asset->unka8);
                json.WriteFieldValueUnknown("unkb0", asset->unkb0);
                json.WriteFieldValueUnknown("unkb8", asset->unkb8);
                json.WriteFieldValueUnknown("unkc0", asset->unkc0);
                json.WriteFieldValueUnknown("unkc8", asset->unkc8);
                json.WriteFieldValueUnknown("unkd0", asset->unkd0);
                json.WriteFieldValueUnknown("unkd8", asset->unkd8);
                json.WriteFieldValueUnknown("unke0", asset->unke0);
                json.WriteFieldValueUnknown("unke8", asset->unke8);
                json.WriteFieldValueUnknown("unkf0", asset->unkf0);
                json.WriteFieldValueUnknown("unkf8", asset->unkf8);
                json.WriteFieldValueUnknown("unk100", asset->unk100);
                json.WriteFieldValueUnknown("unk108", asset->unk108);
                json.WriteFieldValueUnknown("unk110", asset->unk110);
                json.WriteFieldValueUnknown("unk118", asset->unk118);
                json.WriteFieldValueUnknown("unk120", asset->unk120);
                json.WriteFieldValueUnknown("unk128", asset->unk128);
                json.WriteFieldValueUnknown("unk130", asset->unk130);
                json.WriteFieldValueUnknown("unk138", asset->unk138);
                json.WriteFieldValueUnknown("unk140", asset->unk140);
                json.WriteFieldValueUnknown("unk148", asset->unk148);
                json.WriteFieldValueUnknown("unk150", asset->unk150);
                json.WriteFieldValueUnknown("unk158", asset->unk158);
                json.WriteFieldValueUnknown("unk160", asset->unk160);
                json.WriteFieldValueUnknown("unk168", asset->unk168);
                json.WriteFieldValueUnknown("unk170", asset->unk170);
                json.WriteFieldValueUnknown("unk178", asset->unk178);
                json.WriteFieldValueUnknown("unk180", asset->unk180);
                json.WriteFieldValueUnknown("unk188", asset->unk188);
                json.WriteFieldValueUnknown("unk190", asset->unk190);
                json.WriteFieldValueUnknown("unk198", asset->unk198);
                json.WriteFieldValueUnknown("unk1a0", asset->unk1a0);
                json.WriteFieldValueUnknown("unk1a8", asset->unk1a8);
            }


            scriptbundle::WriteObject(json, "bundle", asset->bundle);

            json.EndObject();


            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };

    utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_PLAYER_ROLE_CATEGORY };
    utils::MapAdder<ImplTableWorker, games::bo4::pool::XAssetType, Worker> impltb{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_PLAYER_ROLE_CATEGORY_TABLE };
    utils::MapAdder<ImplTemplateWorker, games::bo4::pool::XAssetType, Worker> impltp{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_PLAYER_ROLE_TEMPLATE };
}