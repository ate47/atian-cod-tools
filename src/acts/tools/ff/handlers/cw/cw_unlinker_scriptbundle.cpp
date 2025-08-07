#include <includes.hpp>
#include <tools/ff/handlers/handler_game_cw.hpp>
#include <tools/utils/raw_file_extractor.hpp>

namespace fastfile::handlers::cw::scriptbundle {
	using namespace fastfile::handlers::cw;
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
	}; static_assert(sizeof(SB_Object) == 0x20);


	struct SB_ObjectsArray;

	struct SB_Sub {
		CWXHash hashname;
		uint32_t namecanon;
		uint32_t unkc;
		uint64_t count;
		SB_ObjectsArray* item;
	}; static_assert(sizeof(SB_Sub) == 0x20);

	struct SB_ObjectsArray {
		uint64_t sbObjectCount;
		SB_Object* sbObjects;
		uint64_t sbSubCount;
		SB_Sub* sbSubs;
	}; static_assert(sizeof(SB_ObjectsArray) == 0x20);

	struct ScriptBundle {
		CWXHash name;
		CWXHash bundleType;
		SB_ObjectsArray sbObjectsArray;
	}; static_assert(sizeof(ScriptBundle) == 0x30);

	struct ScriptBundleList {
		CWXHash name;
		ScrString_t assetType;
		uint32_t assetCount;
		ScriptBundle* assets;
	}; static_assert(sizeof(ScriptBundleList) == 0x18);

	void WriteObject(utils::raw_file_extractor::JsonWriter& json, SB_ObjectsArray& arr, bool& error) {
		json.BeginObject();

		std::unordered_set<uint32_t> handles{};

		if (arr.sbSubCount && !IsValidHandle(arr.sbSubs)) {
			LOG_ERROR("Invalid arr.sbSubs handle: {}", (void*)arr.sbSubs);
			error = true;
			json.WriteFieldNameString("subs");
			json.WriteValueString(GetValidString((const char*)arr.sbSubs));
		}
		else {
			// subs
			for (size_t i = 0; i < arr.sbSubCount; i++) {
				SB_Sub& sub{ arr.sbSubs[i] };
				const char* xhashstr{ hashutils::ExtractPtr(sub.hashname) };
				if (xhashstr) json.WriteFieldNameString(xhashstr);
				else json.WriterFieldNameHash(sub.namecanon);
				json.BeginArray();
				{
					for (size_t i = 0; i < sub.count; i++) {
						WriteObject(json, sub.item[i], error);
					}
				}
				json.EndArray();
			}
		}
		if (arr.sbObjectCount && !IsValidHandle(arr.sbObjects)) {
			LOG_ERROR("Invalid arr.sbObjects handle: {}", (void*)arr.sbObjects);
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
					json.WriteValueHash(obj.hashValue, "#");
					break;
				case KVP_INT: //int2
					json.WriteValueNumber(obj.value.intVal);
					break;
				case KVP_FLOAT: //float
					json.WriteValueNumber(obj.value.floatVal);
					break;
				case KVP_ANIMATION: //anim4
					json.WriteValueHash(obj.hashValue, "anim#");
					break;
				case KVP_PLAYER_ANIMATION: //anim5
					json.WriteValueHash(obj.hashValue, "playeranim#");
					break;
				case KVP_SIEGE_ANIMATION: //anim6
					json.WriteValueHash(obj.hashValue, "siegeanim#");
					break;
				case KVP_MODEL: //model
					json.WriteValueHash(obj.hashValue, "model#");
					break;
				case KVP_AITYPE: //aitype
					json.WriteValueHash(obj.hashValue, "aitype#");
					break;
				case KVP_CHARACTER: //character
					json.WriteValueHash(obj.hashValue, "character#");
					break;
				case KVP_FX: //fx
					json.WriteValueHash(obj.hashValue, "fx#");
					break;
				case KVP_SURFACE_FX_TABLE: //surface_fx_table
					json.WriteValueHash(obj.hashValue, "surface_fx_table#");
					break;
				case KVP_RUMBLE_STR: //rumble
					json.WriteValueString(std::format("rumble&{}", GetScrString(obj.stringRef)));
					break;
				case KVP_SCRIPTBUNDLE_STR: //scriptbundle
					json.WriteValueString(std::format("scriptbundle&{}", GetScrString(obj.stringRef)));
					break;
				case KVP_SCRIPTBUNDLE: //scriptbundle hash
					json.WriteValueHash(obj.hashValue, "scriptbundle#");
					break;
				case KVP_XCAM_STR: //xcam
					json.WriteValueString(std::format("xcam&{}", GetScrString(obj.stringRef)));
					break;
				case KVP_IMAGE: //image
					json.WriteValueHash(obj.hashValue, "image#");
					break;
				case KVP_LOCALIZED18: //localized18
					json.WriteValueHash(obj.hashValue, "localized18#");
					break;
				case KVP_LOCALIZED19: //localized19
					json.WriteValueHash(obj.hashValue, "localized19#");
					break;
				case KVP_WEAPON: //weapon
					json.WriteValueHash(obj.hashValue, "weapon#");
					break;
				case KVP_VEHICLE: //vehicle
					json.WriteValueHash(obj.hashValue, "vehicle#");
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
					json.WriteValueHash(obj.hashValue, "objective#");
					break;
				case KVP_GESTURE: //gesture
					json.WriteValueHash(obj.hashValue, "gesture#");
					break;
				case KVP_RENDER_OVERRIDE_BUNDLE: //render_override_bundle
					json.WriteValueHash(obj.hashValue, "render_override_bundle#");
					break;
				case KVP_GESTURE_TABLE_STR: //gesture_table
					json.WriteValueString(std::format("gesture_table&{}", GetScrString(obj.stringRef)));
					break;
				case KVP_IMPACT_FX_TABLE: //impact_fx_table
					json.WriteValueHash(obj.hashValue, "impact_fx_table#");
					break;
				case KVP_EXECUTION: //execution
					json.WriteValueHash(obj.hashValue, "execution#");
					break;
				case KVP_VEHICLE_SKIN: //vehicle_skin
					json.WriteValueHash(obj.hashValue, "vehicle_skin#");
					break;
				case KVP_MATERIAL: //unk15
				case KVP_UNK19: //unk19
				case KVP_UNK32: //unk32
				default:
					if (obj.stringRef) {
						// str?
						json.WriteValueString(std::format("<unk{}>:{}", (int)obj.type, GetScrString(obj.stringRef)));
					}
					else {
						// hash?
						json.WriteValueString(std::format("<unk{}>:#{}", (int)obj.type, hashutils::ExtractTmp("hash", obj.hashValue)));
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
			const char* n{ hashutils::ExtractTmp("file", asset->name) };
			std::filesystem::path outFile{ opt.m_output / "cw" / "source" / "scriptbundle"
				/ (asset->bundleType ? hashutils::ExtractTmp("hash", asset->bundleType) : "default")
				/ std::format("{}.json", n) };

			std::filesystem::create_directories(outFile.parent_path());

			utils::raw_file_extractor::JsonWriter json{};

			LOG_INFO("Dump scriptbundle {}", outFile.string());
			if (!asset->sbObjectsArray.sbObjectCount) {
				LOG_INFO("ignore empty asset->sbObjectsArray");
				return;
			}
			bool error{};
			WriteObject(json, asset->sbObjectsArray, error);

			if (!json.WriteToFile(outFile) || error) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};
	class ScriptBundleListWorker : public Worker {

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			ScriptBundleList* asset{ (ScriptBundleList*)ptr };
			std::filesystem::path outFile{ opt.m_output / "cw" / "source" / "scriptbundle" / "list"
				/ std::format("{}.json", hashutils::ExtractTmp("hash", asset->name)) };

			std::filesystem::create_directories(outFile.parent_path());

			utils::raw_file_extractor::JsonWriter json{};

			LOG_INFO("Dump scriptbundlelist {}", outFile.string());

			json.BeginObject();

			json.WriteFieldNameString("name");
			json.WriteValueHash(asset->name);

			if (asset->assetType) {
				json.WriteFieldNameString("type");
				json.WriteValueString(GetScrString(asset->assetType));
			}
			json.WriteFieldNameString("bundles");
			json.BeginArray();
			for (size_t i = 0; i < asset->assetCount; i++) {
				ScriptBundle* bundle{ asset->assets + i };
				if (bundle->bundleType) {
					json.WriteValueString(
						std::format("#{}/#{}",
							hashutils::ExtractTmp("hash", bundle->bundleType),
							hashutils::ExtractTmp("hash", bundle->name)
						));
				}
				else {
					json.WriteValueHash(bundle->name);
				}
			}
			json.EndArray();

			json.EndObject();

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ScriptBundleWorker, XAssetType, Worker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_SCRIPTBUNDLE };
	//utils::MapAdder<ScriptBundleListWorker, XAssetType, Worker> impllist{ GetWorkers(), XAssetType::ASSET_TYPE_SCRIPTBUNDLELIST };
}