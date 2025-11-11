#include <includes.hpp>
#include <tools/ff/handlers/handler_game_mwii.hpp>
#include <tools/ff/handlers/mwii/mwii_unlinker_scriptbundle.hpp>


namespace fastfile::handlers::mwii::scriptbundle {
	using namespace fastfile::handlers::mwii;

	static void WriteString(utils::raw_file_extractor::JsonWriter& json, const char* prefix, size_t offset, byte* rawData, size_t rawDataLen) {
		if (offset >= rawDataLen) {
			json.WriteValueString(std::format("invalid:0x%llx", offset));
		}
		else {
			json.WriteValueString(std::format("{}{}", prefix, utils::FormattedStringJson{ (char*)(&rawData[offset]) }));
		}
	}

	void WriteDef(utils::raw_file_extractor::JsonWriter& json, ScriptBundleObjectDef& def, byte* rawData, size_t rawDataLen) {
		switch (def.type) {
		case SBT_UNDEFINED:
			json.WriteValueLiteral("undefined");
			break;
		case SBT_BOOL:
			json.WriteValueBool(def.value.intval);
			break;
		case SBT_INTEGER:
			json.WriteValueNumber(def.value.intval);
			break;
		case SBT_FLOAT:
			json.WriteValueNumber(def.value.floatval);
			break;
		case SBT_STRING:
			WriteString(json, "", def.value.id, rawData, rawDataLen);
			break;
		case SBT_LOCALIZED:
			WriteString(json, "&", def.value.id, rawData, rawDataLen);
			break;
		case SBT_STRUCT: {
			WriteStruct(json, def.value.strct, rawData, rawDataLen);
			break;
		}
		case SBT_ARRAY: {
			json.BeginArray();
			ScriptBundleObjectDefValueArray* array{ &def.value.array };

			for (size_t i = 0; i < array->count; i++) {
				WriteDef(json, array->defs[i], rawData, rawDataLen);
			}

			json.EndArray();
			break;
		}
		case SBT_STRUCT_ANIMATION: {
			json.BeginObject();
			ScriptBundleObjectDefValueAnim* anim{ &def.value.anim };
			json.WriteFieldValueString("type", "anim");
			json.WriteFieldNameString("name");
			WriteString(json, "", anim->nameIndex, rawData, rawDataLen);
			if (anim->anim) {
				json.WriteFieldNameString("id");
				json.WriteValueHash(anim->anim, "anim#");
			}
			json.EndObject();
			break;
		}
		case SBT_STRUCT_ANIMATION_TREE:
			WriteString(json, "animtree#", def.value.id, rawData, rawDataLen);
			break;
		case SBT_XHASH:
			json.WriteValueHash(def.value.hash, "#");
			break;
		case SBT_XHASH_32:
			json.WriteValueHash(def.value.hash32, "x32#");
			break;
		case SBT_XHASH_DVAR:
			json.WriteValueHash(def.value.hash, "@#");
			break;
		case SBT_XHASH_ASSET:
			json.WriteValueHash(def.value.hash, "%#");
			break;
		default:
			json.WriteValueString(utils::va("bad type 0x%x", def.type));
			break;
		}
	}
	void WriteStruct(utils::raw_file_extractor::JsonWriter& json, ScriptBundleObjectStruct& data, byte* rawData, size_t rawDataLen) {
		json.BeginObject();

		for (size_t i = 0; i < data.defsCount; i++) {
			json.WriterFieldNameHash(data.defs[i].name);
			WriteDef(json, data.defs[i].def, rawData, rawDataLen);
		}

		json.EndObject();
	}

	void WriteData(utils::raw_file_extractor::JsonWriter& json, ScriptBundleObjectData& data) {
		WriteStruct(json, data.bundle, data.data, data.dataSize);
	}

	void WriteBundle(utils::raw_file_extractor::JsonWriter& json, ScriptBundle* bundle) {
		WriteData(json, bundle->data);
	}

	class ImplWorker : public Worker {
		using Worker::Worker;

		std::unordered_map<uint64_t, std::vector<ScriptBundle>> bundles{};

		void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			bundles.clear();
		}

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			ScriptBundle* asset{ (ScriptBundle*)ptr };

			// most likely added because it is inside the scr strings
			const char* ename{ hashutils::ExtractPtr(asset->name) };

			if (!ename) {
				bundles[asset->category].emplace_back(*asset);
				return;
			}

			char* name{ utils::MapString(utils::va("%s.json", ename), [](char c) -> char { return c == ':' ? '/' : c; }) };

			std::string_view sw{ name };
			size_t catcut{ sw.find('/') };
			if (catcut != std::string::npos) {
				name[catcut] = 0;
				hashutils::AddPrecomputed(hash::Hash64(name), name); // add category to know categories
				name[catcut] = '/';
			}

			std::filesystem::path outFile{ opt.m_output / gamePath / "source" / "scriptbundle" / name };


			std::filesystem::create_directories(outFile.parent_path());

			utils::raw_file_extractor::JsonWriter json{};

			LOG_INFO("Dump scriptbundle {}", outFile.string());

			WriteData(json, asset->data);

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}

		void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) override {
			if (bundles.empty()) return;

			for (auto& [cat, vec] : bundles) {
				std::filesystem::path outFile{
					opt.m_output / gamePath / "source" / "scriptbundle"
					/ (cat ? hashutils::ExtractTmp("hash", cat) : "default") / "hashed"
					/ std::format("{}.json", ctx.ffname)
				};

				std::filesystem::create_directories(outFile.parent_path());

				utils::raw_file_extractor::JsonWriter json{};

				std::sort(vec.begin(), vec.end(), [](ScriptBundle& a, ScriptBundle& b) -> bool { return a.name < b.name; });

				LOG_INFO("Dump {} hashed scriptbundle(s) {}", vec.size(), outFile.string());

				json.BeginObject();

				for (ScriptBundle& bundle : vec) {
					json.WriterFieldNameHash(bundle.name);
					WriteBundle(json, &bundle);
				}

				json.EndObject();

				if (!json.WriteToFile(outFile)) {
					LOG_ERROR("Error when dumping {}", outFile.string());
				}
			}
		}
	};

	utils::MapAdder<ImplWorker, HandlerHashedAssetType, Worker> impl{ GetWorkers(), HandlerHashedAssetType::IW9H_ASSET_SCRIPTBUNDLE, sizeof(ScriptBundle) };
}
