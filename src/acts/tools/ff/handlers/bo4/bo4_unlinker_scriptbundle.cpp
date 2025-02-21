#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>
#include <tools/utils/raw_file_extractor.hpp>

namespace {
	using namespace fastfile::handlers::bo4;

	enum SB_ValueType : uint32_t {
		SBT_INT2 = 2,
		SBT_INT22 = 22,
		SBT_INT25 = 25,
		SBT_FLOAT = 3,
		SBT_WEAPON = 20,
	};

	struct SB_Object {
		uint32_t unk0;
		uint32_t unk8;
		uint32_t kvpCount;
		uint64_t hash;
		uint64_t sbObjectCount;
		ScrString_t name;
		ScrString_t stringRef;
		SB_ValueType type;
		union {
			int32_t intVal;
			float floatVal;
		} value;
	};
	struct SB_ObjectsArray;

	struct SB_Sub {
		ScrString_t keyname;
		uint32_t unk4;
		uint64_t size;
		SB_ObjectsArray* item;
	};

	struct SB_ObjectsArray {
		uint64_t sbObjectCount;
		SB_Object* sbObjects;
		uint64_t sbSubCount;
		SB_Sub* sbSubs;
	};

	struct ScriptBundle {
		XHash name;
		XHash bundleType;
		SB_ObjectsArray sbObjectsArray;
	};

	class ScriptBundleWorker : public Worker {
		static void WriteObject(tool::utils::raw_file_extractor::JsonWriter& json, SB_ObjectsArray& arr) {
			json.BeginObject();

			// subs
			for (size_t i = 0; i < arr.sbSubCount; i++) {
				SB_Sub& sub{ arr.sbSubs[i] };
				json.WriteFieldNameString(GetScrString(sub.keyname));
				json.BeginArray();
				{
					for (size_t i = 0; i < sub.size; i++) {
						WriteObject(json, sub.item[i]);
					}
				}
				json.EndArray();
			}

			// objects
			for (size_t i = 0; i < arr.sbObjectCount; i++) {
				SB_Object& obj = arr.sbObjects[i];

				// no idea why
				switch (obj.type) {
				case SBT_INT2:
				case SBT_INT25:
					if (!obj.value.intVal) {
						continue;
					}
					break;
				case SBT_FLOAT:
					if (!obj.value.floatVal) {
						continue;
					}
					break;
				}

				json.WriterFieldNameHash(obj.name, "");

				switch (obj.type) {
				case SBT_INT2:
				case SBT_INT22:
				case SBT_INT25: // int?
					json.WriteValueNumber(obj.value.intVal);
					break;
				case SBT_FLOAT: // float?
					json.WriteValueNumber(obj.value.floatVal);
					break;
				case SBT_WEAPON:
					// weapon
					json.WriteValueHash(obj.hash, "weapon#");
					break;
				default:
					if (obj.stringRef) {
						// str?
						json.WriteValueString(GetScrString(obj.stringRef));
					}
					else if (obj.hash & 0x7FFFFFFFFFFFFFFF) {
						// hash?
						json.WriteValueHash(obj.hash, "#");
					}
					else {
						json.WriteValueString(utils::va("<unk:%d>", obj.type));
					}
					break;
				}
			}

			json.EndObject();
		}

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {

			ScriptBundle* asset{ (ScriptBundle*)ptr };

			// most likely added because it is inside the scr strings
			const char* n{ hashutils::ExtractTmp("file", asset->name.hash) };
			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "scriptbundle" 
				/ (asset->bundleType.hash ? hashutils::ExtractTmp("hash", asset->bundleType.hash) : "default") 
				/ std::format("{}.json", n)};

			std::filesystem::create_directories(outFile.parent_path());

			tool::utils::raw_file_extractor::JsonWriter json{};

			LOG_INFO("Dump scriptbundle {}", outFile.string());
			WriteObject(json, asset->sbObjectsArray);

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ScriptBundleWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_SCRIPTBUNDLE };
}