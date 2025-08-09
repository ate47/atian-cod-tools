#pragma once
#include <tools/utils/raw_file_extractor.hpp>

namespace fastfile::handlers::bo6::scriptbundle {
	struct ScriptBundleObjectDef;
	struct ScriptBundleObjectData;

	struct __declspec(align(8)) ScriptBundleObjectDefValueNameId {
		const char** name;
		int32_t id;
	};

	struct ScriptBundleObjectDefValueArrayIndexed {
		uint64_t* names2;
		uint64_t* names1;
		ScriptBundleObjectDef* vals;
		int32_t count;
		int32_t id;
	};

	struct ScriptBundleObjectDefValueArray {
		ScriptBundleObjectDef* defs;
		uint64_t id;
		int32_t count;
	};

	struct ScriptBundleObjectDefValueAnim {
		const char** name_offset;
		uint64_t id;
		uint32_t anim;
	};

	union ScriptBundleObjectDefValue {
		int32_t int_val;
		float float_val;
		uint32_t id;
		uint64_t hash;
		uint32_t hash32;
		const char** str;
		ScriptBundleObjectDefValueAnim* anim;
		ScriptBundleObjectDefValueArray* array;
		ScriptBundleObjectDefValueArrayIndexed* arrayIndexed;
		ScriptBundleObjectData* obj;
		ScriptBundleObjectDefValueNameId* nameId;
	};

	enum ScriptBundleObjectDefType : unsigned __int8 {
		SBT_UNDEFINED = 0x0,
		SBT_BOOL = 0x1,
		SBT_INTEGER = 0x2,
		SBT_FLOAT = 0x3,
		SBT_IHASH = 0x4,
		SBT_STRING = 0x5,
		SBT_STRUCT = 0x6,
		SBT_ARRAY_INDEXED = 0x7,
		SBT_ARRAY = 0x8,
		SBT_STRUCT_ANIMATION = 0x9,
		SBT_STRUCT_NAMEID = 0xA,
		SBT_HASH = 0xB,
		SBT_HASH_TAG = 0xC,
		SBT_HASH_DVAR = 0xD,
		SBT_HASH_SCR = 0xE,
		SBT_HASH_RESOURCE = 0xF,
		SBT_HASH_UNK10 = 0x10,
		SBT_HASH_UNK11 = 0x11,
		SBT_STRUCT_ANIMATION_2 = 0x12,
	};

	struct ScriptBundleObjectDef {
		ScriptBundleObjectDefType type;
		byte pad[7];
		ScriptBundleObjectDefValue value;
	};
	static_assert(sizeof(ScriptBundleObjectDef) == 0x10);


	struct ScriptBundleObjectData {
		uint64_t* rawdata;
		uint64_t* names;
		ScriptBundleObjectDef* defs;
		uint32_t count;
	};
	static_assert(sizeof(ScriptBundleObjectData) == 0x20);

	struct ScriptBundle {
		uint64_t name;
		uint64_t unk8;
		ScriptBundleObjectData data;
	};
	static_assert(sizeof(ScriptBundle) == 0x30);

	bool WriteBundle(utils::raw_file_extractor::JsonWriter& json, ScriptBundle* bundle);
	bool WriteDef(utils::raw_file_extractor::JsonWriter& json, ScriptBundleObjectDef& def);
	bool WriteData(utils::raw_file_extractor::JsonWriter& json, ScriptBundleObjectData& data);
}