#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>
#include <tools/utils/raw_file_extractor.hpp>

namespace {
	using namespace fastfile::handlers::bo4;

	enum SB_ValueType : uint32_t {
		SBT_STRING = 0,
		SBT_XHASH = 1,
		SBT_INT = 2,
		SBT_FLOAT = 3,
		SBT_ANIM4 = 4,
		SBT_ANIM5 = 5,
		SBT_ANIM6 = 6,
		SBT_MODEL = 7,
		SBT_AITYPE = 8,
		SBT_CHARACTER = 9,
		SBT_FX = 10,
		SBT_SURFACE_FX_TABLE = 11,
		SBT_RUMBLE_STR = 12,
		SBT_SCRIPTBUNDLE_STR = 13,
		SBT_XCAM_STR = 14,
		SBT_UNK15 = 15,
		SBT_IMAGE = 16,
		SBT_LOCALIZED17 = 17,
		SBT_LOCALIZED18 = 18,
		SBT_UNK19 = 19,
		SBT_WEAPON = 20,
		SBT_VEHICLE = 21,
		SBT_BUTTON_INT = 22,
		SBT_STREAMERHINT_STR = 23,
		SBT_STATUS_EFFECT_STR = 24,
		SBT_DURATION_INT = 25,
		SBT_OBJECTIVE = 26,
		SBT_GESTURE = 27,
		SBT_RENDER_OVERRIDE_BUNDLE = 28,
		SBT_GESTURE_TABLE_STR = 29,
		SBT_IMPACT_FX_TABLE = 30,
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

	class ScriptBundleWorker : public Worker {
		static void WriteObject(utils::raw_file_extractor::JsonWriter& json, SB_ObjectsArray& arr, bool& error) {
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
					case SBT_STRING: // string
						json.WriteValueString(GetScrString(obj.stringRef));
						break;
					case SBT_XHASH: // xhash
						json.WriteValueHash(obj.hashValue.name, "#");
						break;
					case SBT_INT: //int2
						json.WriteValueNumber(obj.value.intVal);
						break;
					case SBT_FLOAT: //float
						json.WriteValueNumber(obj.value.floatVal);
						break;
					case SBT_ANIM4: //anim4
						json.WriteValueHash(obj.hashValue.name, "anim4#");
						break;
					case SBT_ANIM5: //anim5
						json.WriteValueHash(obj.hashValue.name, "anim5#");
						break;
					case SBT_ANIM6: //anim6
						json.WriteValueHash(obj.hashValue.name, "anim6#");
						break;
					case SBT_MODEL: //model
						json.WriteValueHash(obj.hashValue.name, "model#");
						break;
					case SBT_AITYPE: //aitype
						json.WriteValueHash(obj.hashValue.name, "aitype#");
						break;
					case SBT_CHARACTER: //character
						json.WriteValueHash(obj.hashValue.name, "character#");
						break;
					case SBT_FX: //fx
						json.WriteValueHash(obj.hashValue.name, "fx#");
						break;
					case SBT_SURFACE_FX_TABLE: //surface_fx_table
						json.WriteValueHash(obj.hashValue.name, "surface_fx_table#");
						break;
					case SBT_RUMBLE_STR: //rumble
						json.WriteValueString(std::format("rumble&{}", GetScrString(obj.stringRef)));
						break;
					case SBT_SCRIPTBUNDLE_STR: //scriptbundle
						json.WriteValueString(std::format("scriptbundle&{}", GetScrString(obj.stringRef)));
						break;
					case SBT_XCAM_STR: //xcam
						json.WriteValueString(std::format("xcam&{}", GetScrString(obj.stringRef)));
						break;
					case SBT_IMAGE: //image
						json.WriteValueHash(obj.hashValue.name, "image#");
						break;
					case SBT_LOCALIZED17: //localized17
						json.WriteValueHash(obj.hashValue.name, "localized17#");
						break;
					case SBT_LOCALIZED18: //localized18
						json.WriteValueHash(obj.hashValue.name, "localized18#");
						break;
					case SBT_WEAPON: //weapon
						json.WriteValueHash(obj.hashValue.name, "weapon#");
						break;
					case SBT_VEHICLE: //vehicle
						json.WriteValueHash(obj.hashValue.name, "vehicle#");
						break;
					case SBT_BUTTON_INT: //button
						json.WriteValueString(std::format("button:{}", obj.value.intVal));
						break;
					case SBT_STREAMERHINT_STR: //streamerhint
						json.WriteValueString(std::format("streamerhint&{}", GetScrString(obj.stringRef)));
						break;
					case SBT_STATUS_EFFECT_STR: //status_effect
						json.WriteValueString(std::format("status_effect&{}", GetScrString(obj.stringRef)));
						break;
					case SBT_DURATION_INT: //duration
						json.WriteValueString(std::format("duration:{}", obj.value.intVal));
						break;
					case SBT_OBJECTIVE: //objective
						json.WriteValueHash(obj.hashValue.name, "objective#");
						break;
					case SBT_GESTURE: //gesture
						json.WriteValueHash(obj.hashValue.name, "gesture#");
						break;
					case SBT_RENDER_OVERRIDE_BUNDLE: //render_override_bundle
						json.WriteValueHash(obj.hashValue.name, "render_override_bundle#");
						break;
					case SBT_GESTURE_TABLE_STR: //gesture_table
						json.WriteValueString(std::format("gesture_table&{}", GetScrString(obj.stringRef)));
						break;
					case SBT_IMPACT_FX_TABLE: //impact_fx_table
						json.WriteValueHash(obj.hashValue.name, "impact_fx_table#");
						break;
					case SBT_UNK15: //unk15
					case SBT_UNK19: //unk19
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