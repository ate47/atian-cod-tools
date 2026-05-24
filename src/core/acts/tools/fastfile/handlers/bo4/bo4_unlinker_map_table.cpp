#include <includes.hpp>
#include <tools/fastfile/handlers/bo4/bo4_unlinker_map.hpp>
#include <tools/fastfile/handlers/bo4/bo4_unlinker_scriptbundle.hpp>
#include <core/hashes/raw_file_extractor.hpp>
#include <tools/utils/data_utils.hpp>

namespace fastfile::handlers::bo4::map {
	using namespace games::bo4::pool;

    struct ScriptBundleList;
    struct ObjectiveList;

    struct MapTableEntry {
        const char* nameStr;
        XHash name;
        int size;
        XHash mapName;
        XHash rootMapName;
        const char* missionName;
        XHash mapDescription;
        XHash unk58;
        uint64_t unk68;
        XHash location;
        XHash unk80;
        XHash presence;
        XHash unka0;
        XHash previewImage;
        XHash loadScreen;
        XHash unkd0;
        GfxImage* unke0;
        GfxImage* unke8;
        const char* loadingmovie;
        uint64_t unkf8;
        ObjectiveList* objectiveList;
        const char* unk108;
        ScriptBundleList* bundleList;
        XHash unk118;
        uint64_t unk128;
        uint64_t unk130;
        const char* unk138;
        GfxImage* unk140;
        uint32_t playerRoleTemplatesCount;
        PlayerRoleTemplate** playerRoleTemplates;
        uint64_t unk158;
        uint32_t unk160;
        uint32_t unk164;
        XHash unk168;
        scriptbundle::SB_ObjectsArray bundle;
        uint64_t uniqueID;
    };
    static_assert(sizeof(MapTableEntry) == 0x1A0);

    struct MapTable {
        XHash name;
        uint32_t mapCount;
        MapTableEntry* maps;
        eModes sessionMode;
        uint32_t campaignMode;
        int32_t dlcIndex;
    };
    static_assert(sizeof(MapTable) == 0x30);

    struct MapTableListElem {
        uint64_t mapsCount;
        XHash* maps;
        uint64_t devMapsCount;
        XHash* devMaps;
    };

    struct MapTableList {
        XHash name;
        MapTableListElem cp;
        MapTableListElem mp;
        MapTableListElem zm;
        MapTableListElem wz;
    };

    struct MapTableLoadingImages {
        XHash name;
        uint64_t imageCount;
        GfxImage** images;
        uint64_t mapCount;
    };
    static_assert(sizeof(MapTableLoadingImages) == 0x28);

    struct MapTablePreviewImages {
        XHash name;
        uint64_t imageCount;
        GfxImage** images;
        uint64_t mapCount;
    };
    static_assert(sizeof(MapTablePreviewImages) == 0x28);

    class MapTableWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
            MapTable& asset{ *(MapTable*)ptr };


            BO4JsonWriter json{};

            json.BeginObject();
            json.WriteFieldValueXHash("name", asset.name);
            json.WriteFieldValueString("sessionMode", GetEModeName(asset.sessionMode));
            if (asset.dlcIndex) json.WriteFieldValueNumber("dlcIndex", asset.dlcIndex);
            if (asset.campaignMode) json.WriteFieldValueNumber("campaignMode", asset.campaignMode);

            if (asset.maps) {
                json.WriteFieldNameString("maps");
                json.BeginArray();

                for (size_t i = 0; i < asset.mapCount; i++) {
                    MapTableEntry& e{ asset.maps[i] };

                    json.BeginObject();
                    json.WriteFieldValueNumber("uniqueID", e.uniqueID);
                    json.WriteFieldValueXString("name", e.nameStr);
                    json.WriteFieldValueXHash("hashname", e.name);
                    json.WriteFieldValueXString("missionName", e.missionName);
                    json.WriteFieldValueXString("loadingmovie", e.loadingmovie);
                    json.WriteFieldValueXHash("mapName", e.mapName);
                    json.WriteFieldValueXHash("rootMapName", e.rootMapName);
                    json.WriteFieldValueXHash("mapDescription", e.mapDescription);
                    json.WriteFieldValueXHash("unk58", e.unk58);
                    json.WriteFieldValueXHash("location", e.location);
                    json.WriteFieldValueXHash("unk80", e.unk80);
                    json.WriteFieldValueXHash("presence", e.presence);
                    json.WriteFieldValueXHash("unka0", e.unka0);
                    json.WriteFieldValueXHash("previewImage", e.previewImage);
                    json.WriteFieldValueXHash("loadScreen", e.loadScreen);
                    json.WriteFieldValueXHash("unkd0", e.unkd0);
                    json.WriteFieldValueXHash("unk118", e.unk118);
                    json.WriteFieldValueXHash("unk168", e.unk168);
                    json.WriteFieldValueXAsset("e:unke0", XAssetType::ASSET_TYPE_IMAGE, e.unke0);
                    json.WriteFieldValueXAsset("e:unke8", XAssetType::ASSET_TYPE_IMAGE, e.unke8);
                    json.WriteFieldValueXAsset("e:unk140", XAssetType::ASSET_TYPE_IMAGE, e.unk140);
                    if (e.unk164) json.WriteFieldValueNumber("e:unk164", e.unk164);
                    json.WriteFieldValueXAsset("bundleList", XAssetType::ASSET_TYPE_SCRIPTBUNDLELIST, e.bundleList);
                    json.WriteFieldValueXAsset("objectiveList", XAssetType::ASSET_TYPE_OBJECTIVE_LIST, e.objectiveList);
                    json.WriteFieldValueXAssetArray("playerRoleTemplates", XAssetType::ASSET_TYPE_PLAYER_ROLE_TEMPLATE, e.playerRoleTemplatesCount, e.playerRoleTemplates);
                    json.WriteFieldValueNumber("size", e.size);

                    scriptbundle::WriteObject(json, "bundle", e.bundle);

                    json.WriteFieldValueUnknown("e:unk68", e.unk68);
                    json.WriteFieldValueUnknown("e:unkf8", e.unkf8);
                    json.WriteFieldValueUnknown("e:unk108", e.unk108);
                    json.WriteFieldValueUnknown("e:unk128", e.unk128);
                    json.WriteFieldValueUnknown("e:unk130", e.unk130);
                    json.WriteFieldValueUnknown("e:unk138", e.unk138);
                    json.WriteFieldValueUnknown("e:unk158", e.unk158);
                    json.WriteFieldValueUnknown("e:unk160", e.unk160);
                    json.EndObject();
                }

                json.EndArray();
            }

            json.EndObject();


            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "map" / std::format("{}.json", hashutils::ExtractTmp("file", asset.name)) };

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump map table {}", outFile.string());

            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };
    class MapTablListWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
            MapTableList& asset{ *(MapTableList*)ptr };


            BO4JsonWriter json{};

            json.BeginObject();
            json.WriteFieldValueXHash("name", asset.name);

            auto WriteList = [&json](MapTableListElem& elem, const char* id) {
                if (!(elem.maps || elem.devMaps)) return;
                json.WriteFieldNameString(id);
                json.BeginObject();
                if (elem.maps) {
                    json.WriteFieldNameString("maps");
                    json.BeginArray();
                    for (size_t i = 0; i < elem.mapsCount; i++) {
                        json.WriteValueHash(elem.maps[i]);
                    }
                    json.EndArray();
                }
                if (elem.devMaps) {
                    json.WriteFieldNameString("devMaps");
                    json.BeginArray();
                    for (size_t i = 0; i < elem.devMapsCount; i++) {
                        json.WriteValueHash(elem.devMaps[i]);
                    }
                    json.EndArray();
                }
                json.EndObject();
            };

            WriteList(asset.cp, "cp");
            WriteList(asset.mp, "mp");
            WriteList(asset.zm, "zm");
            WriteList(asset.wz, "wz");

            json.EndObject();


            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "map" / "list" / std::format("{}.json", hashutils::ExtractTmp("file", asset.name)) };

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump map table list {}", outFile.string());

            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };

    class MapTableImagesWorker : public Worker {
        const char* type;
    public:
        MapTableImagesWorker(const char* type) : type(type) {}

        void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
            MapTablePreviewImages& asset{ *(MapTablePreviewImages*)ptr };


            BO4JsonWriter json{};

            json.BeginObject();
            json.WriteFieldValueXHash("name", asset.name);
            json.WriteFieldValueNumber("mapCount", asset.mapCount);
            json.WriteFieldValueXAssetArray("images", XAssetType::ASSET_TYPE_IMAGE, asset.imageCount, asset.images);
            json.EndObject();


            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "map" / type / std::format("{}.json", hashutils::ExtractTmp("file", asset.name)) };

            std::filesystem::create_directories(outFile.parent_path());
            LOG_OPT_INFO("Dump map table {} {}", type, outFile.string());

            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };

	utils::MapAdder<MapTableWorker, games::bo4::pool::XAssetType, Worker> impltable{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_MAPTABLE };
	utils::MapAdder<MapTablListWorker, games::bo4::pool::XAssetType, Worker> impltablelist{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_MAPTABLE_LIST };
	utils::MapAdder<MapTableImagesWorker, games::bo4::pool::XAssetType, Worker> implimgl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_MAPTABLE_LOADING_IMAGES, "loadingimages" };
	utils::MapAdder<MapTableImagesWorker, games::bo4::pool::XAssetType, Worker> implimgp{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_MAPTABLE_PREVIEW_IMAGES, "previewimages" };
}