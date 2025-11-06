#pragma once
#include <tools/utils/raw_file_extractor.hpp>

namespace fastfile::handlers::mwiiisp::scriptbundle {


	struct ScriptBundleObject;
	struct ScriptBundleObjectDef;
	struct ScriptBundleObjectDefValueArrayIndexedVal;

	enum ScriptBundleObjectDefType : byte {
		SBT_UNDEFINED = 0x0,
		SBT_INT1 = 0x1,
		SBT_INT2 = 0x2,
		SBT_FLOAT = 0x3,
		SBT_ISTRING = 0x4,
		SBT_STRING = 0x5,
		SBT_STRUCT = 0x6,
		SBT_ARRAY_INDEXED = 0x7,
		SBT_ARRAY = 0x8,
		SBT_STRUCT_ANIMATION = 0x9,
		SBT_STRUCT_NAMEID = 0xA,
		SBT_HASH = 0xB,
		SBT_HASH_TAG = 0xC,
		SBT_HASH_DVAR = 0xD,
		SBT_HASH_RESOURCE = 0xE,
	};

	struct ScriptBundleObjectDefValueAnim
	{
		int32_t name_offset;
		int32_t id;
		uint64_t unk8;
		uint64_t anim;
	};

	struct ScriptBundleObjectDefValueArray
	{
		int32_t count;
		int32_t id;
		ScriptBundleObjectDef* defs;
	};

	struct ScriptBundleObjectDefValueArrayIndexed
	{
		int32_t count;
		int32_t id;
		ScriptBundleObjectDefValueArrayIndexedVal* vals;
	};

	union ScriptBundleObjectDefValue
	{
		int32_t int_val;
		float float_val;
		uint32_t id;
		uint64_t hash;
		uint32_t hash_tag;
		ScriptBundleObjectDefValueAnim anim;
		ScriptBundleObjectDefValueArray array;
		ScriptBundleObjectDefValueArrayIndexed arrayIndexed;
		uint8_t data[24];
	};

	struct ScriptBundleObjectDef {
		ScriptBundleObjectDefType type;
		uint8_t pad[7];
		ScriptBundleObjectDefValue value;
	};

	struct ScriptBundleObject {
		uint64_t name0;
		uint64_t name;
		ScriptBundleObjectDef def;
	};

	struct ScriptBundleObjectDefValueArrayIndexedVal {
		uint64_t name1;
		uint64_t name2;
		ScriptBundleObjectDef* def;
		uint64_t unk18;
		uint64_t unk20;
		uint64_t unk28;
	};

	struct ScriptBundle {
		XHash64 name;
		XHash64 category;
		ScriptBundleObjectData data;
	};
	static_assert(sizeof(ScriptBundle) == 0x30);
	
	
	struct ScriptBundleObjectData {
		uint32_t count;
		XHash64* rawdata;
		XHash64* names;
		ScriptBundleObjectDef* defs;
	};
	static_assert(sizeof(ScriptBundleObjectData) == 0x20);


	//struct ScriptBundle {
	//	uint64_t name;
	//	uint64_t unk8;
	//	uint64_t unk10;
	//	uint8_t* rawdata;
	//	uint32_t count;
	//	ScriptBundleObject* objects;
	//};

	bool WriteBundle(utils::raw_file_extractor::JsonWriter& json, ScriptBundle* bundle);
	bool WriteDef(utils::raw_file_extractor::JsonWriter& json, ScriptBundleObjectDef& def);
	bool WriteData(utils::raw_file_extractor::JsonWriter& json, ScriptBundleObjectData& data);
}
