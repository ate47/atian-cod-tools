#pragma once

namespace fastfile::handlers::cw::scriptbundle {
    enum SB_ValueType : uint32_t {
        KVP_STRING = 0,
        KVP_XHASH = 1,
        KVP_INT = 2,
        KVP_FLOAT = 3,
        KVP_ANIMATION = 4,
        KVP_PLAYER_ANIMATION = 5,
        KVP_SIEGE_ANIMATION = 6,
        KVP_MODEL = 7,
        KVP_AITYPE = 8,
        KVP_CHARACTER = 9,
        KVP_FX = 10,
        KVP_SURFACE_FX_TABLE = 11,
        KVP_RUMBLE_STR = 12,
        KVP_SCRIPTBUNDLE_STR = 13,
        KVP_SCRIPTBUNDLE = 14,
        KVP_XCAM_STR = 15,
        KVP_MATERIAL = 16,
        KVP_IMAGE = 17,
        KVP_LOCALIZED18 = 18,
        KVP_LOCALIZED19 = 19,
        KVP_UNK19 = 20,
        KVP_WEAPON = 21,
        KVP_VEHICLE = 22,
        KVP_ENUM_INT = 23,
        KVP_STREAMERHINT_STR = 24,
        KVP_STATUS_EFFECT_STR = 25,
        KVP_DURATION_INT = 26,
        KVP_OBJECTIVE = 27,
        KVP_GESTURE = 28,
        KVP_RENDER_OVERRIDE_BUNDLE = 29,
        KVP_GESTURE_TABLE_STR = 30,
        KVP_IMPACT_FX_TABLE = 31,
        KVP_UNK32 = 32,
        KVP_EXECUTION = 33,
        KVP_VEHICLE_SKIN = 34,
    };

    struct SB_Object {
        CWXHash keyName;
        CWXHash hashValue;
        uint32_t keyScrName;
        ScrString_t stringRef;
        SB_ValueType type;
        union {
            int32_t intVal;
            float floatVal;
        } value;
    };
    static_assert(sizeof(SB_Object) == 0x20);

    struct SB_ObjectsArray;

    struct SB_Sub {
        CWXHash hashname;
        uint32_t namecanon;
        uint32_t unkc;
        uint64_t count;
        SB_ObjectsArray* item;
    };
    static_assert(sizeof(SB_Sub) == 0x20);

    struct SB_ObjectsArray {
        uint64_t sbObjectCount;
        SB_Object* sbObjects;
        uint64_t sbSubCount;
        SB_Sub* sbSubs;
    };
    static_assert(sizeof(SB_ObjectsArray) == 0x20);

    struct ScriptBundle {
        CWXHash name;
        CWXHash bundleType;
        SB_ObjectsArray sbObjectsArray;
    };
    static_assert(sizeof(ScriptBundle) == 0x30);

    struct ScriptBundleList {
        CWXHash name;
        ScrString_t assetType;
        uint32_t assetCount;
        ScriptBundle** assets;
    };
    static_assert(sizeof(ScriptBundleList) == 0x18);

    void WriteObject(core::hashes::raw_file_extractor::JsonWriter& json, SB_ObjectsArray& arr, bool& error);
    void WriteObject(core::hashes::raw_file_extractor::JsonWriter& json, const char* name, SB_ObjectsArray& arr);
} // namespace fastfile::handlers::cw::scriptbundle