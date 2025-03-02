#pragma once
#include <tools/utils/raw_file_extractor.hpp>

namespace tool::cordycep::dump::t10::scriptbundle {
	namespace sbv1 {
		struct ScriptBundleObject {
			uint64_t name;
			uintptr_t def; // ScriptBundleObjectDef*
		};

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
			SBT_UNKE = 0xE,
			SBT_HASH_RESOURCE = 0xF,
		};

		union ScriptBundleObjectDefValue {
			int32_t int_val;
			float float_val;
			uint32_t id;
			uint64_t hash;
			uint32_t hash32;
			uintptr_t str; // const char**
			uintptr_t anim; // ScriptBundleObjectDefValueAnim*
			uintptr_t array; // ScriptBundleObjectDefValueArray*
			uintptr_t arrayIndexed; // ScriptBundleObjectDefValueArrayIndexed*
			uintptr_t obj; // ScriptBundleObjectData*
			uintptr_t nameId; // ScriptBundleObjectDefValueNameId*
		};

		struct ScriptBundleObjectDefValueAnim {
			uintptr_t name_offset;
			uint64_t id;
			uint64_t anim;
		};

		struct ScriptBundleObjectDefValueArray {
			int32_t count;
			int32_t id;
			uintptr_t defs; // ScriptBundleObjectDef*
		};

		struct ScriptBundleObjectDefValueArrayIndexed {
			int32_t count;
			int32_t id;
			uintptr_t names2; // uint64_t*
			uintptr_t names1; // uint64_t*
			uintptr_t vals; // ScriptBundleObjectDef*
		};

		struct ScriptBundleObjectData {
			uint32_t count;
			uintptr_t rawdata; // byte*
			uintptr_t names; // uint64_t*
			uintptr_t defs; // ScriptBundleObjectDef*
		};

		struct __declspec(align(8)) ScriptBundleObjectDefValueNameId {
			uintptr_t name; // const char**
			int32_t id;
		};

		struct ScriptBundleObjectDef {
			ScriptBundleObjectDefType type;
			byte pad[7];
			ScriptBundleObjectDefValue value;
		};

		struct ScriptBundle {
			uint64_t name;
			uint64_t unk8;
			ScriptBundleObjectData data;
		};



		bool WriteBundleDef(PoolOption& opt, std::ostream& os, Process& proc, ScriptBundle& entry, ScriptBundleObjectDef& def, int depth);
	}
	namespace sbv2 {
		struct __declspec(align(8)) ScriptBundleObjectDefValueNameId {
			uintptr_t name; // const char**
			int32_t id;
		};

		struct ScriptBundleObjectDefValueArrayIndexed {
			uintptr_t names2; // uint64_t*
			uintptr_t names1; // uint64_t*
			uintptr_t vals; // ScriptBundleObjectDef*
			int32_t count;
			int32_t id;
		};

		struct ScriptBundleObjectDefValueArray {
			uintptr_t defs; // ScriptBundleObjectDef*
			uint64_t id;
			int32_t count;
		};

		struct ScriptBundleObjectDefValueAnim {
			uintptr_t name_offset; // const char**
			uint64_t id;
			uint32_t anim;
		};

		union ScriptBundleObjectDefValue {
			int32_t int_val;
			float float_val;
			uint32_t id;
			uint64_t hash;
			uint32_t hash32;
			uintptr_t str; // const char**
			uintptr_t anim; // ScriptBundleObjectDefValueAnim*
			uintptr_t array; // ScriptBundleObjectDefValueArray*
			uintptr_t arrayIndexed; // ScriptBundleObjectDefValueArrayIndexed*
			uintptr_t obj; // ScriptBundleObjectData*
			uintptr_t nameId; // ScriptBundleObjectDefValueNameId*
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
		};

		struct ScriptBundleObjectDef {
			ScriptBundleObjectDefType type;
			byte pad[7];
			ScriptBundleObjectDefValue value;
		};


		struct ScriptBundleObjectData {
			uintptr_t rawdata; // byte*
			uintptr_t names; // uint64_t*
			uintptr_t defs; // ScriptBundleObjectDef*
			uint32_t count;
		};

		struct ScriptBundle {
			uint64_t name;
			uint64_t unk8;
			ScriptBundleObjectData data;
		};


		bool WriteBundleDef(PoolOption& opt, utils::raw_file_extractor::JsonWriter& json, Process& proc, ScriptBundleObjectDef& def, std::function<const char* (uint64_t hash)>& GetLocalized);
		bool WriteBundleData(PoolOption& opt, utils::raw_file_extractor::JsonWriter& json, Process& proc, ScriptBundleObjectData& data, std::function<const char* (uint64_t hash)>& GetLocalized);
	}

}