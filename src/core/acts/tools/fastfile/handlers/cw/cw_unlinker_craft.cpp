#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_cw.hpp>
#include <tools/fastfile/handlers/cw/cw_unlinker_scriptbundle.hpp>

namespace {
    using namespace fastfile::handlers::cw;

    struct CraftCategory {
        uint32_t number;
        CWXHash name;
        CWXHash categoryName;
        CWXHash desc;
    };
    static_assert(sizeof(CraftCategory) == 0x20);
    struct CraftCategoryList {
        CWXHash name;
        uint32_t unk8;
        uint32_t unkc;
        uint32_t unk10;
        uint64_t cat1_count;
        CraftCategory** cat1;
        uint64_t cat2_count;
        CraftCategory** cat2;
        uint64_t cat3_count;
        CraftCategory** cat3;
    };
    static_assert(sizeof(CraftCategoryList) == 0x48);

    struct CraftIcon {
        uint32_t number;
        uint32_t sortIndex;
        GfxImage* image;
        CWXHash name;
        CWXHash unk18;
        CWXHash string;
        CWXHash mtxItemBundle;
        uint32_t unk30;
        float unk34;
    };
    static_assert(sizeof(CraftIcon) == 0x38);

    struct CraftIconList {
        CWXHash name;
        uint32_t unk8;
        uint32_t unkc;
        uint64_t count;
        CraftIcon** icons;
    };
    static_assert(sizeof(CraftIconList) == 0x20);

    struct CraftWeaponSticker;           // ASSET_TYPE_CRAFTWEAPONSTICKER
    struct CraftWeaponStickerList;       // ASSET_TYPE_CRAFTWEAPONSTICKERLIST
    struct CraftBackground;              // ASSET_TYPE_CRAFTBACKGROUND
    struct CraftBackgroundList;          // ASSET_TYPE_CRAFTBACKGROUNDLIST
    struct CraftMaterial;                // ASSET_TYPE_CRAFTMATERIAL
    struct CraftMaterialList;            // ASSET_TYPE_CRAFTMATERIALLIST
    struct CraftWeaponIconTransform;     // ASSET_TYPE_CRAFTWEAPONICONTRANSFORM
    struct CraftWeaponIconTransformList; // ASSET_TYPE_CRAFTWEAPONICONTRANSFORMLIST

    class ImplCraftIconWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
            CraftIcon* asset{ (CraftIcon*)ptr };

            std::filesystem::path outFile{ opt.m_output / "cw" / "source" / "tables" / "craft" / "icon" /
                                           std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };

            BOCWJsonWriter json{};

            json.BeginObject();
            json.WriteFieldValueXHash("name", asset->name);
            json.WriteFieldValueXAsset("icon", XAssetType::ASSET_TYPE_IMAGE, asset->image);
            json.WriteFieldValueNumber("number", asset->number);
            json.WriteFieldValueNumber("sortIndex", asset->sortIndex);
            json.WriteFieldValueXHash("unk18", asset->unk18);
            json.WriteFieldValueXHash("string", asset->string);
            json.WriteFieldValueXHash("mtxItemBundle", asset->mtxItemBundle);
            json.WriteFieldValueNumber("unk30", asset->unk30);
            json.WriteFieldValueNumber("unk34", asset->unk34);
            json.EndObject();

            std::filesystem::create_directories(outFile.parent_path());
            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Can't write to {}", outFile.string());
                return;
            }
            LOG_OPT_INFO("Dump CraftIcon {}", outFile.string());
        }
    };

    class ImplCraftIconListWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
            CraftIconList* asset{ (CraftIconList*)ptr };

            std::filesystem::path outFile{ opt.m_output / "cw" / "source" / "tables" / "craft" / "icon" / "list" /
                                           std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };

            BOCWJsonWriter json{};

            json.BeginObject();
            json.WriteFieldValueXHash("name", asset->name);
            json.WriteFieldValueUnknown("unk8", asset->unk8);
            if (asset->icons) {
                json.WriteFieldNameString("icons");
                json.BeginArray();
                for (size_t i = 0; i < asset->count; i++) {
                    json.WriteValueXAsset(XAssetType::ASSET_TYPE_CRAFTICON, asset->icons[i]);
                }
                json.EndArray();
            }
            json.EndObject();

            std::filesystem::create_directories(outFile.parent_path());
            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Can't write to {}", outFile.string());
                return;
            }
            LOG_OPT_INFO("Dump CraftIconList {}", outFile.string());
        }
    };

    class ImplCraftCategoryWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
            CraftCategory* asset{ (CraftCategory*)ptr };

            std::filesystem::path outFile{ opt.m_output / "cw" / "source" / "tables" / "craft" / "category" /
                                           std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };

            BOCWJsonWriter json{};

            json.BeginObject();
            json.WriteFieldValueXHash("name", asset->name);
            json.WriteFieldValueNumber("number", asset->number);
            json.WriteFieldValueXHash("categoryName", asset->categoryName);
            json.WriteFieldValueString("desc", opt.GetTranslation(asset->desc));
            json.EndObject();

            std::filesystem::create_directories(outFile.parent_path());
            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Can't write to {}", outFile.string());
                return;
            }
            LOG_OPT_INFO("Dump CraftCategory {}", outFile.string());
        }
    };

    class ImplCraftCategoryListWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
            CraftCategoryList* asset{ (CraftCategoryList*)ptr };

            std::filesystem::path outFile{ opt.m_output / "cw" / "source" / "tables" / "craft" / "category" / "list" /
                                           std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };

            BOCWJsonWriter json{};

            json.BeginObject();
            json.WriteFieldValueXHash("name", asset->name);
            json.WriteFieldValueNumber("unk8", asset->unk8);
            json.WriteFieldValueNumber("unkc", asset->unkc);
            json.WriteFieldValueNumber("unk10", asset->unk10);
            auto WriteCat = [&json](const char* cat, CraftCategory** array, size_t count) {
                if (array) {
                    json.WriteFieldNameString(cat);
                    json.BeginArray();
                    for (size_t i = 0; i < count; i++) {
                        json.WriteValueXAsset(XAssetType::ASSET_TYPE_CRAFTCATEGORY, array[i]);
                    }
                    json.EndArray();
                }
            };
            WriteCat("cat1", asset->cat1, asset->cat1_count);
            WriteCat("cat2", asset->cat2, asset->cat2_count);
            WriteCat("cat3", asset->cat3, asset->cat3_count);
            json.EndObject();

            std::filesystem::create_directories(outFile.parent_path());
            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Can't write to {}", outFile.string());
                return;
            }
            LOG_OPT_INFO("Dump CraftCategory {}", outFile.string());
        }
    };

#ifndef CI_BUILD
    utils::MapAdder<ImplCraftIconWorker, XAssetType, Worker> implci{ GetWorkers(), XAssetType::ASSET_TYPE_CRAFTICON };
    utils::MapAdder<ImplCraftIconListWorker, XAssetType, Worker> implcil{ GetWorkers(),
                                                                          XAssetType::ASSET_TYPE_CRAFTICONLIST };

    utils::MapAdder<ImplCraftCategoryWorker, XAssetType, Worker> implcc{ GetWorkers(),
                                                                         XAssetType::ASSET_TYPE_CRAFTCATEGORY };

    utils::MapAdder<ImplCraftCategoryListWorker, XAssetType, Worker> implccl{
        GetWorkers(), XAssetType::ASSET_TYPE_CRAFTCATEGORYLIST
    };
#endif
} // namespace