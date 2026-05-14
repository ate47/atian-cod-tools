#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
namespace {
    using namespace fastfile::handlers::bo4;

    enum HierarchicalTaskNetworkValueType_t : uint32_t {
        HTN_BOOL_TYPE = 0x0,
        HTN_FLOAT_TYPE = 0x1,
        HTN_INT_TYPE = 0x2,
        HTN_STRING_TYPE = 0x3,
        HTN_HASH_TYPE = 0x4,
        HTN_DATA_TYPE_COUNT = 0x5,
        HTN_INVALID_DATA_TYPE = 0x5,
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

    const char* hierarchicalTaskNetworkValueTypeNames[]{
        "bool",
        "float",
        "int",
        "string",
        "xhash"
    };

    const char* hierarchicalTaskNetworkNodeTypeNames[]{
        "action",
        "goto",
        "planner",
        "postcondition",
        "precondition",
        "selector",
        "sequence"
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
    struct __declspec(align(8)) HierarchicalTaskNetworkNode {
        XHash name;
        HierarchicalTaskNetworkNodeType_t type;
        uint32_t index;
        uint32_t* childNodeIndexes;
        uint32_t numChildrenNodes;
        HierarchicalTaskNetworkNodeConstant* constants;
        uint32_t numConstants;
    };

    struct HierarchicalTaskNetwork {
        XHash name;
        HierarchicalTaskNetworkNode* htnNodes;
        uint32_t numNodes;
    };
    static_assert(sizeof(HierarchicalTaskNetwork) == 0x20);

    class ImplWorker : public Worker {
        using Worker::Worker;

        void Unlink(fastfile::FastFileOption& opt, void* ptr) {

            HierarchicalTaskNetwork* asset{ (HierarchicalTaskNetwork*)ptr };

            const char* n = hashutils::ExtractPtr(asset->name);
            if (!n) n = utils::va("file_%llx.ai_htn", asset->name.name);
            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "hierarchicaltasknetwork" / n };
            std::filesystem::create_directories(outFile.parent_path());
            BO4JsonWriter json{};

            LOG_OPT_INFO("Dump hierarchicaltasknetwork {}", outFile.string());

            json.BeginObject();
            json.WriteFieldValueXHash("name", asset->name);
            if (asset->htnNodes) {
                json.WriteFieldNameString("nodes");

                for (size_t i = 0; i < asset->numNodes; i++) {
                    HierarchicalTaskNetworkNode& htnNode{ asset->htnNodes[i] };
                    json.BeginObject();

                    json.WriteFieldValueNumber("id", i);
                    json.WriteFieldValueXHash("name", htnNode.name);
                    json.WriteFieldValueNumber("index", htnNode.index);
                    json.WriteFieldValueString("type", (htnNode.type >= HTN_TYPE_COUNT ? utils::va("<invalid:%d>", htnNode.type) : hierarchicalTaskNetworkNodeTypeNames[htnNode.type]));

                    if (htnNode.childNodeIndexes) {
                        json.WriteFieldNameString("childindexes");
                        json.BeginArray();
                        for (size_t j = 0; j < htnNode.numChildrenNodes; j++) {
                            json.WriteValueNumber(htnNode.childNodeIndexes[j]);
                        }
                        json.EndArray();
                    }
                    if (htnNode.constants) {
                        json.WriteFieldNameString("constants");
                        json.BeginObject();
                        for (size_t j = 0; j < htnNode.numConstants; j++) {
                            HierarchicalTaskNetworkNodeConstant& constant{ htnNode.constants[j] };
                            json.WriterFieldNameHash(constant.key);
                            json.BeginObject();
                            json.WriteFieldValueString("type", (constant.type >= HTN_DATA_TYPE_COUNT ? utils::va("<invalid:%d>", constant.type) : hierarchicalTaskNetworkValueTypeNames[constant.type]));

                            json.WriteFieldNameString("value");
                            switch (constant.type) {
                            case HTN_BOOL_TYPE:
                                json.WriteValueBool(constant.value.boolValue);
                                break;
                            case HTN_FLOAT_TYPE:
                                json.WriteValueNumber(constant.value.floatValue);
                                break;
                            case HTN_INT_TYPE:
                                json.WriteValueNumber(constant.value.intValue);
                                break;
                            case HTN_STRING_TYPE:
                                json.WriteValueScrString(constant.value.stringValue);
                                break;
                            case HTN_HASH_TYPE:
                                json.WriteValueHash(constant.value.hash);
                                break;
                            default:
                                json.WriteUnknownData(constant.value);
                                break;
                            }
                            json.EndObject();
                        }
                        json.EndObject();
                    }

                    json.EndObject();
                }
            }

            json.EndObject();


            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };

    utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_HIERARCHICAL_TASK_NETWORK };
}