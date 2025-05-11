#include <includes.hpp>
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


	void WriteObject(utils::raw_file_extractor::JsonWriter& json, SB_ObjectsArray& arr, bool& error) {
		json.BeginObject();

		std::unordered_set<uint32_t> handles{};

		if (arr.sbSubCount && !IsValidHandle(arr.sbObjects)) {
			error = true;
			json.WriteFieldNameString("subs");
			json.WriteValueString(GetValidString((const char*)arr.sbObjects));
		}
		else {
			// subs
			for (size_t i = 0; i < arr.sbSubCount; i++) {
				SB_Sub& sub{ arr.sbSubs[i] };
				const char* keyName{ GetScrString(sub.keyname) };
				handles.insert(hash::HashT89Scr(keyName));
				json.WriteFieldNameString(keyName);
				if (!IsValidHandle(sub.item)) {
					error = true;
					json.WriteValueString(GetValidString((const char*)sub.item));
					continue;
				}
				json.BeginArray();
				{
					for (size_t i = 0; i < sub.size; i++) {
						WriteObject(json, sub.item[i], error);
					}
				}
				json.EndArray();
			}
		}
		if (arr.sbObjectCount && !IsValidHandle(arr.sbObjects)) {
			error = true;
			json.WriteFieldNameString("fields");
			json.WriteValueString(GetValidString((const char*)arr.sbObjects));
		}
		else {
			// objects
			for (size_t i = 0; i < arr.sbObjectCount; i++) {
				SB_Object& obj = arr.sbObjects[i];

				if (handles.contains(obj.keyScrName)) continue; // ignore sub

				json.WriterFieldNameHash(obj.keyScrName, "");

				switch (obj.type) {
				case KVP_STRING: // string
					json.WriteValueString(GetScrString(obj.stringRef));
					break;
				case KVP_XHASH: // xhash
					json.WriteValueHash(obj.hashValue.name, "#");
					break;
				case KVP_INT: //int2
					json.WriteValueNumber(obj.value.intVal);
					break;
				case KVP_FLOAT: //float
					json.WriteValueNumber(obj.value.floatVal);
					break;
				case KVP_ANIMATION: //anim4
					json.WriteValueHash(obj.hashValue.name, "anim#");
					break;
				case KVP_PLAYER_ANIMATION: //anim5
					json.WriteValueHash(obj.hashValue.name, "playeranim#");
					break;
				case KVP_SIEGE_ANIMATION: //anim6
					json.WriteValueHash(obj.hashValue.name, "siegeanim#");
					break;
				case KVP_MODEL: //model
					json.WriteValueHash(obj.hashValue.name, "model#");
					break;
				case KVP_AITYPE: //aitype
					json.WriteValueHash(obj.hashValue.name, "aitype#");
					break;
				case KVP_CHARACTER: //character
					json.WriteValueHash(obj.hashValue.name, "character#");
					break;
				case KVP_FX: //fx
					json.WriteValueHash(obj.hashValue.name, "fx#");
					break;
				case KVP_SURFACE_FX_TABLE: //surface_fx_table
					json.WriteValueHash(obj.hashValue.name, "surface_fx_table#");
					break;
				case KVP_RUMBLE_STR: //rumble
					json.WriteValueString(std::format("rumble&{}", GetScrString(obj.stringRef)));
					break;
				case KVP_SCRIPTBUNDLE_STR: //scriptbundle
					json.WriteValueString(std::format("scriptbundle&{}", GetScrString(obj.stringRef)));
					break;
				case KVP_XCAM_STR: //xcam
					json.WriteValueString(std::format("xcam&{}", GetScrString(obj.stringRef)));
					break;
				case KVP_IMAGE: //image
					json.WriteValueHash(obj.hashValue.name, "image#");
					break;
				case KVP_LOCALIZED17: //localized17
					json.WriteValueHash(obj.hashValue.name, "localized17#");
					break;
				case KVP_LOCALIZED18: //localized18
					json.WriteValueHash(obj.hashValue.name, "localized18#");
					break;
				case KVP_WEAPON: //weapon
					json.WriteValueHash(obj.hashValue.name, "weapon#");
					break;
				case KVP_VEHICLE: //vehicle
					json.WriteValueHash(obj.hashValue.name, "vehicle#");
					break;
				case KVP_ENUM_INT: //enum
					json.WriteValueString(std::format("enum:{}", obj.value.intVal));
					break;
				case KVP_STREAMERHINT_STR: //streamerhint
					json.WriteValueString(std::format("streamerhint&{}", GetScrString(obj.stringRef)));
					break;
				case KVP_STATUS_EFFECT_STR: //status_effect
					json.WriteValueString(std::format("status_effect&{}", GetScrString(obj.stringRef)));
					break;
				case KVP_DURATION_INT: //duration
					json.WriteValueString(std::format("duration:{}", obj.value.intVal));
					break;
				case KVP_OBJECTIVE: //objective
					json.WriteValueHash(obj.hashValue.name, "objective#");
					break;
				case KVP_GESTURE: //gesture
					json.WriteValueHash(obj.hashValue.name, "gesture#");
					break;
				case KVP_RENDER_OVERRIDE_BUNDLE: //render_override_bundle
					json.WriteValueHash(obj.hashValue.name, "render_override_bundle#");
					break;
				case KVP_GESTURE_TABLE_STR: //gesture_table
					json.WriteValueString(std::format("gesture_table&{}", GetScrString(obj.stringRef)));
					break;
				case KVP_IMPACT_FX_TABLE: //impact_fx_table
					json.WriteValueHash(obj.hashValue.name, "impact_fx_table#");
					break;
				case KVP_MATERIAL: //unk15
				case KVP_UNK19: //unk19
				default:
					if (obj.stringRef) {
						// str?
						json.WriteValueString(std::format("<unk{}>:{}", (int)obj.type, GetScrString(obj.stringRef)));
					}
					else {
						// hash?
						json.WriteValueString(std::format("<unk{}>:#{}", (int)obj.type, hashutils::ExtractTmp("hash", obj.hashValue.name)));
					}
					break;
				}
			}

		}

		json.EndObject();
	}

	class ScriptBundleWorker : public Worker {

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {

			ScriptBundle* asset{ (ScriptBundle*)ptr };

			// most likely added because it is inside the scr strings
			const char* n{ hashutils::ExtractTmp("file", asset->name.name) };
			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "scriptbundle" 
				/ (asset->bundleType.name ? hashutils::ExtractTmp("hash", asset->bundleType.name) : "default")
				/ std::format("{}.json", n)};

			std::filesystem::create_directories(outFile.parent_path());

			utils::raw_file_extractor::JsonWriter json{};

			LOG_INFO("Dump scriptbundle {}", outFile.string());
			bool error{};
			WriteObject(json, asset->sbObjectsArray, error);

			if (!json.WriteToFile(outFile) || error) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ScriptBundleWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_SCRIPTBUNDLE };
}