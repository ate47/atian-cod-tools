#pragma once
#include <tools/ff/handlers/handler_game_bo4.hpp>
#include <tools/utils/raw_file_extractor.hpp>

namespace fastfile::handlers::bo4::scriptbundle {
	using namespace fastfile::handlers::bo4;
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
		KVP_XCAM_STR = 14,
		KVP_MATERIAL = 15,
		KVP_IMAGE = 16,
		KVP_LOCALIZED17 = 17,
		KVP_LOCALIZED18 = 18,
		KVP_UNK19 = 19,
		KVP_WEAPON = 20,
		KVP_VEHICLE = 21,
		KVP_ENUM_INT = 22,
		KVP_STREAMERHINT_STR = 23,
		KVP_STATUS_EFFECT_STR = 24,
		KVP_DURATION_INT = 25,
		KVP_OBJECTIVE = 26,
		KVP_GESTURE = 27,
		KVP_RENDER_OVERRIDE_BUNDLE = 28,
		KVP_GESTURE_TABLE_STR = 29,
		KVP_IMPACT_FX_TABLE = 30,
	};

	struct SB_Object {
		XHash keyName;
		XHash hashValue;
		uint32_t keyScrName;
		ScrString_t stringRef;
		SB_ValueType type;
		union {
			int32_t intVal;
			float floatVal;
		} value;
	}; static_assert(sizeof(SB_Object) == 0x30);


	struct SB_ObjectsArray;

	struct SB_Sub {
		ScrString_t keyname;
		uint64_t size;
		SB_ObjectsArray* item;
	}; static_assert(sizeof(SB_Sub) == 0x18);

	struct SB_ObjectsArray {
		uint64_t sbObjectCount;
		SB_Object* sbObjects;
		uint64_t sbSubCount;
		SB_Sub* sbSubs;
	}; static_assert(sizeof(SB_ObjectsArray) == 0x20);

	struct ScriptBundle {
		XHash name;
		XHash bundleType;
		SB_ObjectsArray sbObjectsArray;
	}; static_assert(sizeof(ScriptBundle) == 0x40);

	void WriteObject(utils::raw_file_extractor::JsonWriter& json, SB_ObjectsArray& arr, bool& error);
}