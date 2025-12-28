#pragma once
#include <tools/utils/raw_file_extractor.hpp>

namespace fastfile::handlers::mwii::scriptbundle {
	struct ScriptBundleObjectData;
	struct ScriptBundleObjectDef;
	struct ScriptBundleObject;

	enum ScriptBundleObjectDefType : byte {
		SBT_UNDEFINED = 0x0,
		SBT_BOOL = 0x1,
		SBT_INTEGER = 0x2,
		SBT_FLOAT = 0x3,
		SBT_LOCALIZED = 0x4,
		SBT_STRING = 0x5,
		SBT_STRUCT = 0x6,
		SBT_ARRAY = 0x7,
		SBT_STRUCT_ANIMATION = 0x8,
		SBT_STRUCT_ANIMATION_TREE = 0x9,
		SBT_XHASH = 0xA,
		SBT_XHASH_32 = 0xB,
		SBT_XHASH_DVAR = 0xC,
		SBT_XHASH_ASSET = 0xD,
	};

	struct ScriptBundleObjectStruct {
		uint32_t defsCount;
		ScriptBundleObject* defs;
	};
	static_assert(sizeof(ScriptBundleObjectStruct) == 0x10);

	struct ScriptBundleObjectData {
		int32_t dataSize;
		byte* data;
		ScriptBundleObjectStruct bundle;
	};
	static_assert(sizeof(ScriptBundleObjectData) == 0x20);

	struct ScriptBundleObjectDefValueArray {
		uint32_t count;
		int32_t id;
		ScriptBundleObjectDef* defs;
	};
	static_assert(sizeof(ScriptBundleObjectDefValueArray) == 0x10);

	struct ScriptBundleObjectDefValueAnim {
		int32_t nameIndex;
		int32_t id;
		uint64_t unk8;
		XHash64 anim;
	};
	static_assert(sizeof(ScriptBundleObjectDefValueAnim) == 0x18);


	union ScriptBundleObjectDefValue {
		int32_t intval;
		float floatval;
		uint32_t id;
		XHash64 hash;
		XHash32 hash32;
		ScriptBundleObjectDefValueAnim anim;
		ScriptBundleObjectDefValueArray array;
		ScriptBundleObjectStruct strct;
		byte __pad[0x18];
	};
	static_assert(sizeof(ScriptBundleObjectDefValue) == 0x18);

	struct ScriptBundleObjectDef {
		ScriptBundleObjectDefType type;
		ScriptBundleObjectDefValue value;
	};
	static_assert(sizeof(ScriptBundleObjectDef) == 0x20);

	struct ScriptBundleObject {
		XHash64 name0;
		XHash64 name;
		ScriptBundleObjectDef def;
	};
	static_assert(sizeof(ScriptBundleObject) == 0x30);

	struct ScriptBundle {
		XHash64 name;
		XHash64 category;
		ScriptBundleObjectData data;
	};
	static_assert(sizeof(ScriptBundle) == 0x30);



	void WriteBundle(utils::raw_file_extractor::JsonWriter& json, ScriptBundle* bundle);
	void WriteStruct(utils::raw_file_extractor::JsonWriter& json, ScriptBundleObjectStruct& data, byte* rawData, size_t rawDataLen);
	void WriteData(utils::raw_file_extractor::JsonWriter& json, ScriptBundleObjectData& data);
	void WriteDef(utils::raw_file_extractor::JsonWriter& json, ScriptBundleObjectDef& def, byte* rawData, size_t rawDataLen);
}
