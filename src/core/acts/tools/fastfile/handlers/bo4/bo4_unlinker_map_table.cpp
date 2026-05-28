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
        const char* rootMapNameStr;
        XHash mapDescription;
        XHash mapDescriptionShort;
        const char* missionName;
        XHash mapLocation;
        XHash mapDateTime;
        XHash presenceString;
        XHash unka0;
        XHash previewImage;
        XHash loadingImage;
        XHash loadingImage_axis;
        GfxImage* compassMap;
        GfxImage* compassMapCorrupt;
        const char* loadingmovie;
        uint64_t unkf8;
        ObjectiveList* objectives;
        const char* collectibles;
        ScriptBundleList* accoladelist;
        XHash mapObjective;
        vec3_t engagementRanges;
        const char* resistanceText;
        GfxImage* operationsBackground;
        uint32_t playerRoleTemplatesCount;
        uint32_t unk14c;
        PlayerRoleTemplate** playerRoleTemplates;
        uint64_t unk158;
        uint64_t unk160;
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
                    json.WriteFieldValueXString("rootMapNameStr", e.rootMapNameStr);
                    json.WriteFieldValueXString("resistanceText", e.resistanceText);
                    json.WriteFieldValueXHash("mapDescription", e.mapDescription);
                    json.WriteFieldValueXHash("mapDescriptionShort", e.mapDescriptionShort);
                    json.WriteFieldValueXHash("mapLocation", e.mapLocation);
                    json.WriteFieldValueXHash("mapDateTime", e.mapDateTime);
                    json.WriteFieldValueXHash("presence", e.presenceString);
                    json.WriteFieldValueXHash("unka0", e.unka0);
                    json.WriteFieldValueXHash("previewImage", e.previewImage);
                    json.WriteFieldValueXHash("loadingImage", e.loadingImage);
                    json.WriteFieldValueXHash("loadingImage_axis", e.loadingImage_axis);
                    json.WriteFieldValueXHash("mapObjective", e.mapObjective);
                    json.WriteFieldValueXHash("unk168", e.unk168);
                    json.WriteFieldValueXAsset("compassMap", XAssetType::ASSET_TYPE_IMAGE, e.compassMap);
                    json.WriteFieldValueXAsset("compassMapCorrupt", XAssetType::ASSET_TYPE_IMAGE, e.compassMapCorrupt);
                    json.WriteFieldValueXAsset("operationsBackground", XAssetType::ASSET_TYPE_IMAGE, e.operationsBackground);
                    json.WriteFieldValueXAsset("accoladelist", XAssetType::ASSET_TYPE_SCRIPTBUNDLELIST, e.accoladelist);
                    json.WriteFieldValueXAsset("objectives", XAssetType::ASSET_TYPE_OBJECTIVE_LIST, e.objectives);
                    json.WriteFieldValueXAssetArray("playerRoleTemplates", XAssetType::ASSET_TYPE_PLAYER_ROLE_TEMPLATE, e.playerRoleTemplatesCount, e.playerRoleTemplates);
                    json.WriteFieldValueNumber("size", e.size);
                    json.WriteFieldValueVector("engagementRanges", e.engagementRanges);

                    json.WriteFieldValueUnknown("e:unkf8", e.unkf8);
                    json.WriteFieldValueUnknown("e:unk158", e.unk158);
                    json.WriteFieldValueUnknown("e:unk160", e.unk160);

                    scriptbundle::WriteObject(json, "bundle", e.bundle);
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