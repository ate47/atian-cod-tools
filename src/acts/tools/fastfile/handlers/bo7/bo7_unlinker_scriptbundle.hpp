#pragma once
#include <tools/fastfile/handlers/handler_game_bo7.hpp>

namespace fastfile::handlers::bo7::scriptbundle {
	struct ScriptBundleObjectDef;
	struct ScriptBundleObjectData;
	union ScriptBundleObjectDefValue;

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
		SBT_XHASH_OMNVAR = 0x10,
		SBT_XHASH_UNK11 = 0x11,
		SBT_STRUCT_ANIMATION_2 = 0x12,
	};

	struct __declspec(align(8)) ScriptBundleObjectDefValueNameId {
		const char** name;
		uint32_t unk8;
		int32_t id;
	};
	struct ScriptBundleObjectDefValueArrayIndexed {
		XHash64* names;
		uint64_t unk8;
		ScriptBundleObjectDef* defs;
		int32_t count;
	};

	struct ScriptBundleObjectDefValueArray {
		ScriptBundleObjectDefType type;
		ScriptBundleObjectDefValue* values;
		uint64_t unk10;
		int32_t count;
		int32_t unk1c;
	};

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
		XHash64 name;
		XHash64 category;
		ScriptBundleObjectData data;
	};
	static_assert(sizeof(ScriptBundle) == 0x30);

	void WriteBundle(HandlerJsonWriter& json, ScriptBundle* bundle);
	void WriteDef(HandlerJsonWriter& json, ScriptBundleObjectDef& def);
	void WriteData(HandlerJsonWriter& json, ScriptBundleObjectData& data);
}