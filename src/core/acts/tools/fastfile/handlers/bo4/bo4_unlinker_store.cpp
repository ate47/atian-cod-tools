#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <core/hashes/raw_file_extractor.hpp>
#include <tools/utils/data_utils.hpp>

namespace fastfile::handlers::bo4::map {
    using namespace games::bo4::pool;
    struct StoreProduct {
        XHash name;
        GfxImage* previewImage;
        GfxImage* productImage; 
        GfxImage* blackMarketImage;
        const char* metadata;
        const char* videoHighResRef;
        const char* videoLowResRef;
        XHash blackMarketName;
        XHash blackMarketDesc;
        XHash entitlementNameXHash; 
        const char* productID;
        const char* unk78;
        XHash productName;
        XHash productDesc;
        bool visibility;
        bool unka1;
        bool unka2;
        bool unka3;
        const char* unka8;
        const char* unkb0;
        const char* unkb8;
        const char* unkc0;
    };
    static_assert(sizeof(StoreProduct) == 0xc8);

    struct StoreCategory {
        const char* name2;
        XHash name;
        GfxImage* image;
        XHash displayName;
        byte productsCount;
        StoreProduct** products;
        bool visibility;
    };
    struct StoreCategoryList {
        XHash name;
        uint64_t count;
        StoreCategory** categories;
    };

    struct Sku {
        XHash name;
        uint64_t unk10;
    };

    struct LabelStore {
        XHash name;
        uint64_t count;
        Sku** skus;
    };
    struct LabelStoreList {
        XHash name;
        uint64_t count;
        LabelStore** stores;
    };

    class SkuWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            Sku* asset{ (Sku*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "store" / "sku" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name))};

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump sku {}", outFile.string());
            BO4JsonWriter json{};

            json.BeginObject();
            json.WriteFieldValueXHash("name", asset->name);
            json.WriteFieldValueUnknown("unk10", asset->unk10);
            json.EndObject();


            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };


    class LabelStoreWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            LabelStore* asset{ (LabelStore*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "store" / "label" / std::format("{}.csv", hashutils::ExtractTmp("file", asset->name))};

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump labelstore{}", outFile.string());

            utils::OutFileCE os{ outFile, true };

            os << "id,sku";

            for (size_t i = 0; i < asset->count; i++) {
                os << "\n" << std::dec << i << ",";
                if (asset->skus[i]) {
                    os << "#" << hashutils::ExtractTmp("hash", asset->skus[i]->name);
                }
                else {
                    os << "null";
                }
            }
        }
    };

    class LabelStoreListWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            LabelStoreList* asset{ (LabelStoreList*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "store" / "label" / "list" / std::format("{}.csv", hashutils::ExtractTmp("file", asset->name))};

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump labelstorelist {}", outFile.string());

            utils::OutFileCE os{ outFile, true };

            os << "id,name";

            for (size_t i = 0; i < asset->count; i++) {
                os << "\n" << std::dec << i << ",";
                if (asset->stores[i]) {
                    os << "#" << hashutils::ExtractTmp("hash", asset->stores[i]->name);
                }
                else {
                    os << "null";
                }
            }
        }
    };

    class StoreCategoryWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            StoreCategory* asset{ (StoreCategory*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "store" / "category" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump storecategory {}", outFile.string());
            BO4JsonWriter json{};

            json.BeginObject();
            json.WriteFieldValueXString("nameStr", asset->name2);
            json.WriteFieldValueXHash("name", asset->name);
            json.WriteFieldValueXHash("displayName", asset->displayName);
            json.WriteFieldValueBool("visibility", asset->visibility);
            json.WriteFieldValueXAsset("image", games::bo4::pool::XAssetType::ASSET_TYPE_IMAGE, asset->image);
            json.WriteFieldValueXAssetArray("products", games::bo4::pool::XAssetType::ASSET_TYPE_STOREPRODUCT, asset->productsCount, asset->products);

            json.EndObject();


            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };

    class StoreCategoryListWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            StoreCategoryList* asset{ (StoreCategoryList*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "store" / "category" / "list" / std::format("{}.csv", hashutils::ExtractTmp("file", asset->name))};

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump storecategorylist {}", outFile.string());

            utils::OutFileCE os{ outFile, true };

            os << "id,name";

            for (size_t i = 0; i < asset->count; i++) {
                os << "\n" << std::dec << i << ",";
                if (asset->categories[i]) {
                    os << "#" << hashutils::ExtractTmp("hash", asset->categories[i]->name);
                }
                else {
                    os << "null";
                }
            }
        }
    };

    class StoreProductWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            StoreProduct* asset{ (StoreProduct*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "store" / "product" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump storeproduct {}", outFile.string());
            BO4JsonWriter json{};

            json.BeginObject();

            json.WriteFieldValueXHash("name", asset->name);
            json.WriteFieldValueXHash("blackMarketName", asset->blackMarketName);
            json.WriteFieldValueXHash("blackMarketDesc", asset->blackMarketDesc);
            json.WriteFieldValueXHash("entitlementNameXHash", asset->entitlementNameXHash);
            json.WriteFieldValueXHash("productName", asset->productName);
            json.WriteFieldValueXHash("productDesc", asset->productDesc);
            json.WriteFieldValueXAsset("previewImage", games::bo4::pool::XAssetType::ASSET_TYPE_IMAGE, asset->previewImage);
            json.WriteFieldValueXAsset("productImage", games::bo4::pool::XAssetType::ASSET_TYPE_IMAGE, asset->productImage);
            json.WriteFieldValueXAsset("blackMarketImage", games::bo4::pool::XAssetType::ASSET_TYPE_IMAGE, asset->blackMarketImage);
            json.WriteFieldValueXString("metadata", asset->metadata);
            json.WriteFieldValueXString("videoHighResRef", asset->videoHighResRef);
            json.WriteFieldValueXString("videoLowResRef", asset->videoLowResRef);
            json.WriteFieldValueXString("productID", asset->productID);
            json.WriteFieldValueXString("unk78", asset->unk78);
            json.WriteFieldValueXString("unka8", asset->unka8);
            json.WriteFieldValueXString("unkb0", asset->unkb0);
            json.WriteFieldValueXString("unkb8", asset->unkb8);
            json.WriteFieldValueXString("unkc0", asset->unkc0);
            json.WriteFieldValueBool("visibility", asset->visibility);
            if (asset->unka1) json.WriteFieldValueBool("unka1", asset->unka1);
            if (asset->unka2) json.WriteFieldValueBool("unka2", asset->unka2);
            if (asset->unka3) json.WriteFieldValueBool("unka3", asset->unka3);
            json.EndObject();


            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };
    utils::MapAdder<StoreProductWorker, games::bo4::pool::XAssetType, Worker> implsp{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_STOREPRODUCT};
    utils::MapAdder<StoreCategoryWorker, games::bo4::pool::XAssetType, Worker> implsc{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_STORECATEGORY};
    utils::MapAdder<StoreCategoryListWorker, games::bo4::pool::XAssetType, Worker> implscl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_STORECATEGORYLIST };
    utils::MapAdder<LabelStoreWorker, games::bo4::pool::XAssetType, Worker> implr{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_LABELSTORE };
    utils::MapAdder<LabelStoreListWorker, games::bo4::pool::XAssetType, Worker> implrt{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_LABELSTORELIST};
    utils::MapAdder<SkuWorker, games::bo4::pool::XAssetType, Worker> impls{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_SKU };
}