#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_mwiii.hpp>
#include <tools/fastfile/handlers/mwiii/mwiii_unlinker_scriptbundle.hpp>

namespace fastfile::handlers::mwiii::scriptbundle {
	using namespace fastfile::handlers::mwiii;

	bool WriteDef(HandlerJsonWriter& json, ScriptBundleObjectDef& def) {
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
			json.WriteValueString(std::format("{}", utils::FormattedStringJson{ *def.value.str }));
			break;
		case SBT_LOCALIZED:
			json.WriteValueLocalized(*def.value.str);
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
		case SBT_XHASH_SCR:
			json.WriteValueHash(def.value.hash, "s#");
			break;
		default:
			json.WriteValueString(utils::va("bad type 0x%x", def.type));
			return false;
		}
		return true;
	}
	bool WriteData(HandlerJsonWriter& json, ScriptBundleObjectData& data) {
		json.BeginObject();

		for (size_t i = 0; i < data.count; i++) {
			json.WriterFieldNameHash(data.names[i]);
			if (!WriteDef(json, data.defs[i])) return false;
		}

		json.EndObject();
		return true;
	}

	bool WriteBundle(HandlerJsonWriter& json, ScriptBundle* bundle) {
		return WriteData(json, bundle->data);
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
				const char* cname{ hashutils::ExtractPtr(asset->category) };
				if (cname) {
					constexpr uint64_t nameHash = hash::Hash64("name");
					for (size_t i = 0; i < asset->data.count; i++) {
						if ((asset->data.names[i] & hash::MASK60) == (nameHash & hash::MASK60)) {
							ScriptBundleObjectDef* def{ asset->data.defs + i };
							if (def->type == SBT_STRING) {
								// try to craft the
								const char* like{ utils::va("%s:%s", cname, def->value.str) };
								if (hash::HashIWAsset(like) == asset->name) {
									hashutils::Add(like, true, true);
								}
								break;
							}
						}
					}


				}
				if (!ename) {
					bundles[asset->category].emplace_back(*asset);
					return; // we don't know the name, combine it
				}
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

			HandlerJsonWriter json{};

			LOG_OPT_INFO("Dump scriptbundle {}", outFile.string());

			if (!WriteData(json, asset->data) || !json.WriteToFile(outFile)) {
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

				HandlerJsonWriter json{};

				std::sort(vec.begin(), vec.end(), [](ScriptBundle& a, ScriptBundle& b) -> bool { return a.name < b.name; });

				LOG_OPT_INFO("Dump {} hashed scriptbundle(s) {}", vec.size(), outFile.string());

				json.BeginObject();

				for (ScriptBundle& bundle : vec) {
					json.WriterFieldNameHash(bundle.name);
					if (!WriteBundle(json, &bundle)) {
						LOG_ERROR("Error when dumping {} into {}", hashutils::ExtractTmp("hash", bundle.name), outFile.string());
						return;
					}
				}

				json.EndObject();

				if (!json.WriteToFile(outFile)) {
					LOG_ERROR("Error when dumping {}", outFile.string());
				}
			}
		}
	};

	utils::MapAdder<ImplWorker, HandlerHashedAssetType, Worker> impl{ GetWorkers(), HandlerHashedAssetType::JUPH_ASSET_SCRIPTBUNDLE, sizeof(ScriptBundle) };
}