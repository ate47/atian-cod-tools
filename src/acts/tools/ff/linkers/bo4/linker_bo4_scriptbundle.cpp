#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {

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
			size_t sbObjects{ ctx.data.AllocData(sizeof(SB_Object) * sbObjectsCount) };

			for (auto& [k, v] : obj) {
				if (v.IsArray()) {
					continue;
				}
				const char* keyName{ k.GetString() };

				SB_Object* sobj{ ctx.data.GetData<SB_Object>(sbObjects) + idxobj++ };

				if (hash::TryHashPattern(keyName, sobj->keyName.name)) {
					// not good, what if the user wants a XHash key
					sobj->keyScrName = (uint32_t)sobj->keyName.name;
				}
				else {
					sobj->keyName.name = hash::Hash64Pattern(keyName);
					sobj->keyScrName = hash::HashT89Scr(keyName);
				}

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

						if (typeEntry.type == ET_INT) {
							sobj->value.intVal = (int32_t)utils::ParseFormatInt(val);
							break;
						}

						if (typeEntry.type == ET_STRING) {
							sobj->stringRef = (ScrString_t)ctx.data.AddString(val);
						}
						sobj->hashValue.name = hash::Hash64(val);
						break;
					}

					if (sobj->type == KVP_STRING) {
						sobj->hashValue.name = hash::Hash64(stringValue);
						sobj->stringRef = (ScrString_t)ctx.data.AddString(stringValue);
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
			size_t sbSub{ ctx.data.AllocData(sizeof(SB_Sub) * sbSubCount) };
			for (auto& [k, v] : obj) {
				if (!v.IsArray()) {
					continue;
				}
				const char* keyName{ k.GetString() };
				SB_Sub* ssub{ ctx.data.GetData<SB_Sub>(sbSub) + idxsub++ };
				ssub->keyname = (ScrString_t)ctx.data.AddString(keyName);
				auto arr{ v.GetArray() };
				ssub->size = (uint64_t)arr.Size();
				ssub->item = (SB_ObjectsArray*)fastfile::linker::data::POINTER_NEXT;

				// alloc sub elements
				size_t subArray{ ctx.data.AllocData(sizeof(SB_ObjectsArray) * ssub->size) };

				for (auto& el : arr) {
					if (!WriteArray(ctx, el, subArray)) {
						return false;
					}
					subArray += sizeof(SB_ObjectsArray);
				}
			}
		}

		// we have enough data, we can link the header
		SB_ObjectsArray* arr{ ctx.data.GetData<SB_ObjectsArray>(arrayOffset) };
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
			std::vector<std::filesystem::path> files{};
			std::filesystem::path dir{ ctx.linkCtx.input / "scriptbundle" };
			utils::GetFileRecurseExt(dir, files, ".json\0", true);

			for (const std::filesystem::path& rfpath : files) {
				std::filesystem::path path{ dir / rfpath };
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

				ctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_SCRIPTBUNDLE, fastfile::linker::data::POINTER_NEXT);

				ctx.data.PushStream(XFILE_BLOCK_TEMP);

				size_t header{ ctx.data.AllocData(sizeof(ScriptBundle)) };
				size_t arrayOffset{ header + offsetof(ScriptBundle, sbObjectsArray) };

				ScriptBundle* bundle{ ctx.data.GetData<ScriptBundle>(header) };

				// add name and type to bundle
				bundle->name.name = hash::Hash64(name);
				rapidjson::Value nameValue{};
				nameValue.SetString(name.data(), main.GetAllocator());
				main.AddMember(rapidjson::StringRef("name"), nameValue, main.GetAllocator());
				
				if (type.size()) {
					bundle->bundleType.name = hash::Hash64(type);

					rapidjson::Value typeValue{};
					typeValue.SetString(type.data(), main.GetAllocator());
					main.AddMember(rapidjson::StringRef("type"), typeValue, main.GetAllocator());
				}

				ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
				if (!WriteArray(ctx, main, arrayOffset)) {
					LOG_ERROR("Can't compile json object");
					ctx.error = true;
					continue;
				}
				ctx.data.PopStream();

				ctx.data.PopStream();


				LOG_INFO("Added asset scriptbundle {}", path.string());
			}
		}
	};

	utils::ArrayAdder<ScriptBundleWorker, LinkerWorker> impl{ GetWorkers() };
}