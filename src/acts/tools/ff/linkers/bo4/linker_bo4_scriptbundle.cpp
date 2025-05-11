#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {

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
		{ SBT_XHASH, ET_XHASH, "#" },
		{ SBT_ANIM4, ET_XHASH, "anim4#" },
		{ SBT_ANIM5, ET_XHASH, "anim5#" },
		{ SBT_ANIM6, ET_XHASH, "anim6#" },
		{ SBT_MODEL, ET_XHASH, "model#" },
		{ SBT_AITYPE, ET_XHASH, "aitype#" },
		{ SBT_CHARACTER, ET_XHASH, "character#" },
		{ SBT_FX, ET_XHASH, "fx#" },
		{ SBT_SURFACE_FX_TABLE, ET_XHASH, "surface_fx_table#" },
		{ SBT_RUMBLE_STR, ET_STRING, "rumble&" },
		{ SBT_SCRIPTBUNDLE_STR, ET_STRING, "scriptbundle&" },
		{ SBT_XCAM_STR, ET_STRING, "xcam&" },
		{ SBT_IMAGE, ET_XHASH, "image#" },
		{ SBT_LOCALIZED17, ET_XHASH, "localized17#" },
		{ SBT_LOCALIZED18, ET_XHASH, "localized18#" },
		{ SBT_WEAPON, ET_XHASH, "weapon#" },
		{ SBT_VEHICLE, ET_XHASH, "vehicle#" },
		{ SBT_BUTTON_INT, ET_INT, "button:" },
		{ SBT_STREAMERHINT_STR, ET_STRING, "streamerhint&" },
		{ SBT_STATUS_EFFECT_STR, ET_STRING, "status_effect&" },
		{ SBT_DURATION_INT, ET_INT, "duration:" },
		{ SBT_OBJECTIVE, ET_XHASH, "objective#" },
		{ SBT_GESTURE, ET_XHASH, "gesture#" },
		{ SBT_RENDER_OVERRIDE_BUNDLE, ET_XHASH, "render_override_bundle#" },
		{ SBT_GESTURE_TABLE_STR, ET_STRING, "gesture_table&" },
		{ SBT_IMPACT_FX_TABLE, ET_XHASH, "impact_fx_table#" },
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
					sobj->type = SBT_INT;
					sobj->value.intVal = v.GetInt();
				}
				else if (v.IsNumber()) {
					sobj->type = SBT_FLOAT;
					sobj->value.floatVal = v.GetFloat();
				}
				else if (v.IsBool()) {
					sobj->type = SBT_INT;
					sobj->value.intVal = v.GetBool() ? 1 : 0;
				}
				else if (v.IsString()) {
					const char* stringValue{ v.GetString() };
					std::string_view sw{ stringValue };

					sobj->type = SBT_STRING;

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

					if (sobj->type == SBT_STRING) {
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

				bundle->name.name = hash::Hash64Pattern(name.data());

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