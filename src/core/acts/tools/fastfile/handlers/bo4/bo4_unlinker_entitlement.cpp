#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
    struct FirstPartyEntitlement {
        XHash name;
        const char* entitlementRef;
        uint64_t entitlementDescCount;
        const char** entitlementsDesc;
    };
    static_assert(sizeof(FirstPartyEntitlement) == 0x28);

    struct FirstPartyEntitlementList {
        XHash name;
        uint64_t count;
        FirstPartyEntitlement** entitlements;
    };
    static_assert(sizeof(FirstPartyEntitlementList) == 0x20);

    struct Entitlement {
        XHash name;
        bool hasFirstPartyEntitlement;
        bool unk11;
        bool unk12;
        bool hasItemIds;
        uint32_t unk14;
        FirstPartyEntitlement* firstPartyEntitlement;
        uint32_t dwItemID;
        uint32_t entitlementDescCount;
        XHash unk28;
        XHash desc;
        const char* unk48;
        uint64_t itemIdsCount;
        uint32_t* itemIds;
    };
    static_assert(sizeof(Entitlement) == 0x60);

    struct EntitlementList {
        XHash name;
        uint64_t count;
        Entitlement** entitlements;
    };
    static_assert(sizeof(EntitlementList) == 0x20);

    class EntitlementWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            Entitlement* asset{ (Entitlement*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "entitlement" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump entitlement {}", outFile.string());
            BO4JsonWriter json{};

            json.BeginObject();

            json.WriteFieldValueXHash("name", asset->name);
            json.WriteFieldValueXHash("unk28", asset->unk28);
            json.WriteFieldValueXHash("desc", asset->desc);
            if (asset->hasFirstPartyEntitlement) json.WriteFieldValueBool("hasFirstPartyEntitlement", asset->hasFirstPartyEntitlement);
            if (asset->unk11) json.WriteFieldValueBool("unk11", asset->unk11);
            if (asset->unk12) json.WriteFieldValueBool("unk12", asset->unk12);
            if (asset->hasItemIds) json.WriteFieldValueBool("hasItemIds", asset->hasItemIds);
            json.WriteFieldValueUnknown("unk14", asset->unk14);
            json.WriteFieldValueNumber("dwItemID", asset->dwItemID);
            json.WriteFieldValueNumber("entitlementDescCount", asset->entitlementDescCount);
            json.WriteFieldValueXString("unk48", asset->unk48);
            json.WriteFieldValueXAsset("firstPartyEntitlement", games::bo4::pool::XAssetType::ASSET_TYPE_FIRSTPARTYENTITLEMENT, asset->firstPartyEntitlement);

            if (asset->itemIds) {
                json.WriteFieldNameString("itemIds");
                json.BeginArray();
                for (size_t i = 0; i < asset->itemIdsCount; i++) {
                    json.WriteValueNumber(asset->itemIds[i]);
                }
                json.EndArray();
            }
            json.EndObject();


            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };

    class EntitlementListWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            EntitlementList* asset{ (EntitlementList*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "entitlement" / "list" / std::format("{}.csv", hashutils::ExtractTmp("file", asset->name)) };

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump entitlementlist {}", outFile.string());

            utils::OutFileCE os{ outFile, true };

            os << "id,name";

            for (size_t i = 0; i < asset->count; i++) {
                os << "\n" << std::dec << i << ",";
                if (asset->entitlements[i]) {
                    os << "#" << hashutils::ExtractTmp("hash", asset->entitlements[i]->name);
                }
                else {
                    os << "null";
                }
            }
        }
    };

    class FirstPartyEntitlementWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            FirstPartyEntitlement* asset{ (FirstPartyEntitlement*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "entitlement" / "firstparty" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name))};

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump firstpartyentitlement {}", outFile.string());
            BO4JsonWriter json{};

            json.BeginObject();

            json.WriteFieldValueXHash("name", asset->name);
            json.WriteFieldValueXString("entitlementRef", asset->entitlementRef);

            if (asset->entitlementsDesc) {
                json.WriteFieldNameString("entitlementsDesc");
                json.BeginArray();
                for (size_t i = 0; i < asset->entitlementDescCount; i++) {
                    if (asset->entitlementsDesc[i]) {
                        json.WriteValueString(asset->entitlementsDesc[i]);
                    }
                    else {
                        json.WriteValueNull();
                    }
                }
                json.EndArray();
            }
            json.EndObject();


            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };

    class FirstPartyEntitlementListWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            FirstPartyEntitlementList* asset{ (FirstPartyEntitlementList*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "entitlement" / "firstparty" / "list" / std::format("{}.csv", hashutils::ExtractTmp("file", asset->name))};

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump firstpartyentitlementlist {}", outFile.string());

            utils::OutFileCE os{ outFile, true };

            os << "id,name";

            for (size_t i = 0; i < asset->count; i++) {
                os << "\n" << std::dec << i << ",";
                if (asset->entitlements[i]) {
                    os << "#" << hashutils::ExtractTmp("hash", asset->entitlements[i]->name);
                }
                else {
                    os << "null";
                }
            }
        }
    };
    utils::MapAdder<EntitlementWorker, games::bo4::pool::XAssetType, Worker> imple{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_ENTITLEMENT};
    utils::MapAdder<EntitlementListWorker, games::bo4::pool::XAssetType, Worker> implel{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_ENTITLEMENTLIST};
    utils::MapAdder<FirstPartyEntitlementWorker, games::bo4::pool::XAssetType, Worker> implfpe{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_FIRSTPARTYENTITLEMENT};
    utils::MapAdder<FirstPartyEntitlementListWorker, games::bo4::pool::XAssetType, Worker> implfpel{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_FIRSTPARTYENTITLEMENTLIST};
}