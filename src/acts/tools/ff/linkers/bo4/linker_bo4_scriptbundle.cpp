#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>
#include <tools/ff/linkers/bo4/linker_bo4_scriptbundle.hpp>

namespace fastfile::linker::bo4::scriptbundle {
	enum EntryType {
		ET_XHASH,
		ET_STRING,
		ET_INT,
	};

	static struct {
		SB_ValueType vtype;
		EntryType type;
		const char* start;
	} typeEntries[] {
		{ KVP_XHASH, ET_XHASH, "#" },
		{ KVP_ANIMATION, ET_XHASH, "anim#" },
		{ KVP_PLAYER_ANIMATION, ET_XHASH, "playeranim#" },
		{ KVP_SIEGE_ANIMATION, ET_XHASH, "siegeanim#" },
		{ KVP_MODEL, ET_XHASH, "model#" },
		{ KVP_AITYPE, ET_XHASH, "aitype#" },
		{ KVP_CHARACTER, ET_XHASH, "character#" },
		{ KVP_FX, ET_XHASH, "fx#" },
		{ KVP_SURFACE_FX_TABLE, ET_XHASH, "surface_fx_table#" },
		{ KVP_RUMBLE_STR, ET_STRING, "rumble&" },
		{ KVP_SCRIPTBUNDLE_STR, ET_STRING, "scriptbundle&" },
		{ KVP_XCAM_STR, ET_STRING, "xcam&" },
		{ KVP_IMAGE, ET_XHASH, "image#" },
		{ KVP_LOCALIZED17, ET_XHASH, "localized17#" },
		{ KVP_LOCALIZED18, ET_XHASH, "localized18#" },
		{ KVP_WEAPON, ET_XHASH, "weapon#" },
		{ KVP_VEHICLE, ET_XHASH, "vehicle#" },
		{ KVP_ENUM_INT, ET_INT, "enum:" },
		{ KVP_STREAMERHINT_STR, ET_STRING, "streamerhint&" },
		{ KVP_STATUS_EFFECT_STR, ET_STRING, "status_effect&" },
		{ KVP_DURATION_INT, ET_INT, "duration:" },
		{ KVP_OBJECTIVE, ET_XHASH, "objective#" },
		{ KVP_GESTURE, ET_XHASH, "gesture#" },
		{ KVP_RENDER_OVERRIDE_BUNDLE, ET_XHASH, "render_override_bundle#" },
		{ KVP_GESTURE_TABLE_STR, ET_STRING, "gesture_table&" },
		{ KVP_IMPACT_FX_TABLE, ET_XHASH, "impact_fx_table#" },
	};

	bool WriteArray(BO4LinkContext& ctx, rapidjson::Value& val, size_t arrayOffset) {
		size_t sbObjectsCount{};
		size_t sbSubCount{};
		if (!val.IsObject()) {
			LOG_ERROR("Value is not an object");
			return false;
		}

		auto obj{ val.GetObj() };

		for (auto& [k, v] : obj) {
			if (v.IsArray()) {
				sbSubCount++;
			}
			else {
				sbObjectsCount++;
			}
		}

		if (sbObjectsCount) {
			size_t idxobj{};
			size_t sbObjects{ ctx.mainFF.data.AllocData(sizeof(SB_Object) * sbObjectsCount) };

			for (auto& [k, v] : obj) {
				if (v.IsArray()) {
					continue;
				}
				const char* keyName{ k.GetString() };

				SB_Object* sobj{ ctx.mainFF.data.GetData<SB_Object>(sbObjects) + idxobj++ };

				// not good, what if the user wants a hashed key
				sobj->keyName.name = ctx.HashXHash(keyName);
				sobj->keyScrName = ctx.HashScr(keyName);

				if (v.IsInt() || v.IsInt64()) {
					sobj->type = KVP_INT;
					sobj->value.intVal = v.GetInt();
				}
				else if (v.IsNumber()) {
					sobj->type = KVP_FLOAT;
					sobj->value.floatVal = v.GetFloat();
				}
				else if (v.IsBool()) {
					sobj->type = KVP_INT;
					sobj->value.intVal = v.GetBool() ? 1 : 0;
				}
				else if (v.IsString()) {
					const char* stringValue{ v.GetString() };
					std::string_view sw{ stringValue };

					sobj->type = KVP_STRING;

					for (auto& typeEntry : typeEntries) {
						if (!sw.starts_with(typeEntry.start)) continue; // not our type
						const char* val = stringValue + std::strlen(typeEntry.start); // skip type name

						sobj->type = typeEntry.vtype;

						switch (typeEntry.type) {
						case ET_INT:
							sobj->value.intVal = (int32_t)utils::ParseFormatInt(val);
							break;
						case ET_STRING:
							sobj->stringRef = (ScrString_t)ctx.mainFF.data.AddString(val);
							break;
						case ET_XHASH:
							sobj->hashValue.name = ctx.HashXHash(val);
							break;
						}
						break;
					}

					if (sobj->type == KVP_STRING) {
						sobj->stringRef = (ScrString_t)ctx.mainFF.data.AddString(stringValue);
						sobj->hashValue.name = ctx.HashXHash(stringValue);
					}
				
				}
				else {
					LOG_ERROR("Invalid ScriptBundle value type: {}", (int)v.GetType());
					ctx.error = true;
					return false;
				}
			}
		}

		if (sbSubCount) {
			size_t idxsub{};
			size_t sbSub{ ctx.mainFF.data.AllocData(sizeof(SB_Sub) * sbSubCount) };
			for (auto& [k, v] : obj) {
				if (!v.IsArray()) {
					continue;
				}
				const char* keyName{ k.GetString() };
				SB_Sub* ssub{ ctx.mainFF.data.GetData<SB_Sub>(sbSub) + idxsub++ };
				ssub->keyname = (ScrString_t)ctx.mainFF.data.AddString(keyName);
				auto arr{ v.GetArray() };
				ssub->size = (uint64_t)arr.Size();
				ssub->item = (SB_ObjectsArray*)fastfile::linker::data::POINTER_NEXT;

				// alloc sub elements
				size_t subArray{ ctx.mainFF.data.AllocData(sizeof(SB_ObjectsArray) * ssub->size) };

				for (auto& el : arr) {
					if (!WriteArray(ctx, el, subArray)) {
						return false;
					}
					subArray += sizeof(SB_ObjectsArray);
				}
			}
		}

		// we have enough data, we can link the header
		SB_ObjectsArray* arr{ ctx.mainFF.data.GetData<SB_ObjectsArray>(arrayOffset) };
		if (sbObjectsCount) {
			arr->sbObjects = (SB_Object*)fastfile::linker::data::POINTER_NEXT;
			arr->sbObjectCount = sbObjectsCount;
		}
		if (sbSubCount) {
			arr->sbSubs = (SB_Sub*)fastfile::linker::data::POINTER_NEXT;
			arr->sbSubCount = sbSubCount;
		}
		return true;
	}

	class ScriptBundleWorker : public LinkerWorker {
	public:
		ScriptBundleWorker() : LinkerWorker("ScriptBundle") {}

		void Compute(BO4LinkContext& ctx) override {
			for (fastfile::zone::AssetData& assval : ctx.linkCtx.zone.assets["scriptbundle"]) {
				std::filesystem::path rfpath{ std::format("{}.json", assval.value) };
				assval.handled = true;
				std::filesystem::path path{ ctx.linkCtx.input / "scriptbundle" / rfpath };
				std::string buffer{};
				if (!utils::ReadFile(path, buffer)) {
					LOG_ERROR("Can't read {}", path.string());
					ctx.error = true;
					continue;
				}

				std::filesystem::path fname{ rfpath.filename() };
				fname.replace_extension();
				std::string name{ fname.string() };

				std::string type{};
				if (rfpath.has_parent_path()) {
					if (rfpath.parent_path().has_parent_path()) {
						LOG_ERROR("Can't load scriptbundle {}: too deep", name);
						ctx.error = true;
						continue;
					}
					std::filesystem::path typeFile{ rfpath.parent_path().filename() };
					type = typeFile.string();
					if (utils::EqualIgnoreCase("default", type)) {
						type = {};
					}
				}


				rapidjson::Document main{};
				main.Parse(buffer.data());

				if (main.HasParseError()) {
					LOG_ERROR("Can't parse {}", path.string());
					ctx.error = true;
					continue;
				}

				ctx.mainFF.data.AddAsset(games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, fastfile::linker::data::POINTER_NEXT);

				ctx.mainFF.data.PushStream(XFILE_BLOCK_TEMP);

				size_t header{ ctx.mainFF.data.AllocData(sizeof(ScriptBundle)) };
				size_t arrayOffset{ header + offsetof(ScriptBundle, sbObjectsArray) };

				ScriptBundle* bundle{ ctx.mainFF.data.GetData<ScriptBundle>(header) };

				// add name and type to bundle
				bundle->name.name = ctx.HashXHash(name);
				rapidjson::Value nameValue{};
				nameValue.SetString(name.data(), main.GetAllocator());
				main.AddMember(rapidjson::StringRef("name"), nameValue, main.GetAllocator());
				
				if (type.size()) {
					bundle->bundleType.name = ctx.HashXHash(type);

					rapidjson::Value typeValue{};
					typeValue.SetString(type.data(), main.GetAllocator());
					main.AddMember(rapidjson::StringRef("type"), typeValue, main.GetAllocator());
				}

				ctx.mainFF.data.PushStream(XFILE_BLOCK_VIRTUAL);
				if (!WriteArray(ctx, main, arrayOffset)) {
					LOG_ERROR("Can't compile json object");
					ctx.error = true;
					continue;
				}
				ctx.mainFF.data.PopStream();

				ctx.mainFF.data.PopStream();


				LOG_INFO("Added asset scriptbundle {} (hash_{:x})", rfpath.string(), bundle->name.name);
			}
		}
	};

	utils::ArrayAdder<ScriptBundleWorker, LinkerWorker> impl{ GetWorkers() };
}