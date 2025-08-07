#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>
#include <tools/ff/handlers/bo6/bo6_unlinker_scriptbundle.hpp>

namespace fastfile::handlers::bo6::scriptbundle {
	using namespace fastfile::handlers::bo6;

	bool WriteDef(utils::raw_file_extractor::JsonWriter& json, ScriptBundleObjectDef& def) {
		switch (def.type) {
		case SBT_UNDEFINED:
			json.WriteValueLiteral("undefined");
			break;
		case SBT_BOOL:
			json.WriteValueBool(def.value.int_val);
			break;
		case SBT_INTEGER:
			json.WriteValueNumber(def.value.int_val);
			break;
		case SBT_FLOAT:
			json.WriteValueNumber(def.value.float_val);
			break;
		case SBT_STRING:
			json.WriteValueString(*def.value.str);
			break;
		case SBT_STRUCT: {
			WriteData(json, *def.value.obj);
			break;
		}
		case SBT_ARRAY_INDEXED: {
			json.BeginArray();
			ScriptBundleObjectDefValueArrayIndexed* arrayIndexed{ def.value.arrayIndexed };
			for (size_t i = 0; i < arrayIndexed->count; i++) {
				json.WriterFieldNameHash(arrayIndexed->names1[i]);
				if (!WriteDef(json, arrayIndexed->vals[i])) return false;
			}
			json.EndArray();
			break;
		}
		case SBT_ARRAY: {
			json.BeginArray();
			ScriptBundleObjectDefValueArray* array{ def.value.array };

			for (size_t i = 0; i < array->count; i++) {
				if (!WriteDef(json, array->defs[i])) return false;
			}

			json.EndArray();
			break;
		}
		case SBT_STRUCT_ANIMATION: {
			json.BeginObject();
			ScriptBundleObjectDefValueAnim* anim{ def.value.anim };
			json.WriteFieldValueString("type", "anim");
			json.WriteFieldNameString("name");
			json.WriteValueString(*anim->name_offset);
			if (anim->anim) {
				json.WriteFieldNameString("id");
				json.WriteValueHash(anim->anim, "anim#");
			}
			json.EndObject();
			break;
		}
		case SBT_STRUCT_ANIMATION_2: {
			json.BeginObject();
			json.WriteFieldValueString("type", "anim2");
			ScriptBundleObjectDefValueAnim* anim{ def.value.anim };
			json.WriteFieldNameString("name");
			json.WriteValueString(*anim->name_offset);
			if (anim->anim) {
				json.WriteFieldNameString("id");
				json.WriteValueHash(anim->anim, "anim#");
			}
			json.EndObject();
			break;
		}
		case SBT_STRUCT_NAMEID: {
			json.BeginObject();
			ScriptBundleObjectDefValueNameId* nameId{ def.value.nameId };
			json.WriteFieldNameString("name");
			json.WriteValueString(*nameId->name);
			json.WriteFieldNameString("id");
			json.WriteValueNumber(nameId->id);
			json.EndObject();
			break;
		}
		case SBT_HASH:
			json.WriteValueHash(def.value.hash, "#");
			break;
		case SBT_IHASH:
			json.WriteValueHash(def.value.hash, "r#");
			break;
		case SBT_HASH_TAG:
			json.WriteValueHash(def.value.hash32, "t#");
			break;
		case SBT_HASH_DVAR:
			json.WriteValueHash(def.value.hash, "@#");
			break;
		case SBT_HASH_RESOURCE:
			json.WriteValueHash(def.value.hash, "%#");
			break;
		case SBT_HASH_SCR:
			json.WriteValueHash(def.value.hash, "&#");
			break;
		case SBT_HASH_UNK10:
			json.WriteValueHash(def.value.hash, "?10#");
			break;
		case SBT_HASH_UNK11:
			json.WriteValueHash(def.value.hash, "?11#");
			break;
		default:
			json.WriteValueString(utils::va("bad type 0x%x", def.type));
			return false;
		}
		return true;
	}
	bool WriteData(utils::raw_file_extractor::JsonWriter& json, ScriptBundleObjectData& data) {
		json.BeginObject();

		for (size_t i = 0; i < data.count; i++) {
			json.WriterFieldNameHash(data.names[i]);
			if (!WriteDef(json, data.defs[i])) return false;
		}

		json.EndObject();
		return true;
	}

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			ScriptBundle* asset{ (ScriptBundle*)ptr };

			// most likely added because it is inside the scr strings
			const char* name{ hashutils::ExtractPtr(asset->name) };

			if (name) {
				name = utils::MapString(utils::va("%s.json", name), [](char c) -> char { return c == ':' ? '/' : c; });
			}
			else {
				name = utils::va("hashed/file_%llx.json", asset->name);
			}

			std::filesystem::path outFile{ opt.m_output / "bo6" / "source" / "scriptbundle" / name };

			std::filesystem::create_directories(outFile.parent_path());

			utils::raw_file_extractor::JsonWriter json{};

			LOG_INFO("Dump scriptbundle {}", outFile.string());

			if (!WriteData(json, asset->data) || !json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10RAssetType, Worker> impl{ GetWorkers(), bo6::T10RAssetType::T10R_ASSET_SCRIPTBUNDLE };
}