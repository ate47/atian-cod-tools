#pragma once
#include <tools/utils/raw_file_extractor.hpp>

namespace fastfile::handlers::mwiii::scriptbundle {

	struct ScriptBundleObjectDef;
	struct ScriptBundleObjectData;

	struct __declspec(align(8)) ScriptBundleObjectDefValueNameId {
		const char** name;
		int32_t id;
	};

	struct ScriptBundleObjectDefValueArrayIndexed {
		int32_t count;
		int32_t id;
		XHash64* names2;
		XHash64* names1;
		ScriptBundleObjectDef* vals;
	};

	struct ScriptBundleObjectDefValueArray {
		int32_t count;
		uint32_t unk4;
		ScriptBundleObjectDef* defs;
		void* unk10;
	};
	static_assert(sizeof(ScriptBundleObjectDefValueArray) == 0x18);

	struct ScriptBundleObjectDefValueAnim {
		const char** name_offset;
		XHash64 id;
		uint32_t anim;
	};

	union ScriptBundleObjectDefValue {
		int32_t int_val;
		float float_val;
		uint32_t id;
		XHash64 hash;
		XHash32 hash32;
		const char** str;
		ScriptBundleObjectDefValueAnim* anim;
		ScriptBundleObjectDefValueArray* array;
		ScriptBundleObjectDefValueArrayIndexed* arrayIndexed;
		ScriptBundleObjectData* obj;
		ScriptBundleObjectDefValueNameId* nameId;
	};

	enum ScriptBundleObjectDefType : byte {
		SBT_UNDEFINED = 0x0,
		SBT_BOOL = 0x1,
		SBT_INTEGER = 0x2,
		SBT_FLOAT = 0x3,
		SBT_LOCALIZED = 0x4,
		SBT_STRING = 0x5,
		SBT_STRUCT = 0x6,
		SBT_ARRAY_INDEXED = 0x7,
		SBT_ARRAY = 0x8,
		SBT_STRUCT_ANIMATION = 0x9,
		SBT_STRUCT_NAMEID = 0xA,
		SBT_XHASH = 0xB,
		SBT_XHASH_32 = 0xC,
		SBT_XHASH_DVAR = 0xD,
		SBT_XHASH_SCR = 0xE,
		SBT_XHASH_ASSET = 0xF,
	};

	struct ScriptBundleObjectDef {
		ScriptBundleObjectDefType type;
		byte pad[7];
		ScriptBundleObjectDefValue value;
	};
	static_assert(sizeof(ScriptBundleObjectDef) == 0x10);


	struct ScriptBundleObjectData {
		uint32_t count;
		XHash64* rawdata;
		XHash64* names;
		ScriptBundleObjectDef* defs;
	};
	static_assert(sizeof(ScriptBundleObjectData) == 0x20);

	struct ScriptBundle {
		XHash64 name;
		XHash64 category;
		ScriptBundleObjectData data;
	};
	static_assert(sizeof(ScriptBundle) == 0x30);

	bool WriteBundle(HandlerJsonWriter& json, ScriptBundle* bundle);
	bool WriteDef(HandlerJsonWriter& json, ScriptBundleObjectDef& def);
	bool WriteData(HandlerJsonWriter& json, ScriptBundleObjectData& data);
}
