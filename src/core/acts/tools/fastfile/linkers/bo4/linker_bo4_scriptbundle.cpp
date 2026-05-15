#include <includes.hpp>
#include <tools/fastfile/linkers/linker_bo4.hpp>
#include <tools/fastfile/linkers/bo4/linker_bo4_scriptbundle.hpp>

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

	bool WriteArray(BO4LinkContext& ctx, rapidjson::Value& val, SB_ObjectsArray& array, BO4FFContext& ff) {
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
			SB_Object* sbObjects{ ff.data.AllocStreamPtr<SB_Object>(sbObjectsCount) };

			for (auto& [k, v] : obj) {
				if (v.IsArray()) {
					continue;
				}
				const char* keyName{ k.GetString() };

				SB_Object* sobj{ sbObjects + idxobj++ };

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
							sobj->stringRef = (ScrString_t)ff.data.AddScrString(val);
							break;
						case ET_XHASH:
							sobj->hashValue.name = ctx.HashXHash(val);
							break;
						}
						break;
					}

					if (sobj->type == KVP_STRING) {
						sobj->stringRef = (ScrString_t)ff.data.AddScrString(stringValue);
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
			SB_Sub* sbSub{ ff.data.AllocStreamPtr<SB_Sub>(sbSubCount) };
			for (auto& [k, v] : obj) {
				if (!v.IsArray()) {
					continue;
				}
				const char* keyName{ k.GetString() };
				SB_Sub* ssub{ sbSub + idxsub++ };
				ssub->keyname = (ScrString_t)ff.data.AddScrString(keyName);
				auto arr{ v.GetArray() };
				ssub->size = (uint64_t)arr.Size();
				ssub->item = (SB_ObjectsArray*)fastfile::linker::memory::POINTER_NEXT;

				// alloc sub elements
				SB_ObjectsArray* subArray{ ff.data.AllocStreamPtr<SB_ObjectsArray>(ssub->size) };

				for (auto& el : arr) {
					if (!WriteArray(ctx, el, *subArray, ff)) {
						return false;
					}
					subArray++;
				}
			}
		}

		// we have enough data, we can link the header
		if (sbObjectsCount) {
			array.sbObjects = (SB_Object*)fastfile::linker::memory::POINTER_NEXT;
			array.sbObjectCount = sbObjectsCount;
		}
		if (sbSubCount) {
			array.sbSubs = (SB_Sub*)fastfile::linker::memory::POINTER_NEXT;
			array.sbSubCount = sbSubCount;
		}
		return true;
	}

	namespace {
		class XAssetLinkerImpl : public XAssetLinker {
		public:
			using XAssetLinker::XAssetLinker;

			void Compute(BO4LinkContext& ctx, const char* id, fastfile::linker::memory::LinkerDataChunk** ref, BO4FFContext& ff) override {
				std::filesystem::path rfpath{ std::format("{}.json", id) };
				std::filesystem::path path{ ctx.linkCtx.input / "scriptbundle" / rfpath };
				std::string buffer{};
				if (!utils::ReadFile(path, buffer)) {
					LOG_ERROR("Can't read scriptbundle {}", path.string());
					ctx.error = true;
					return;
				}

				std::filesystem::path fname{ rfpath.filename() };
				fname.replace_extension();
				std::string name{ fname.string() };

				std::string type{};
				if (rfpath.has_parent_path()) {
					if (rfpath.parent_path().has_parent_path()) {
						LOG_ERROR("Can't load scriptbundle {}: too deep", name);
						ctx.error = true;
						return;
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
					return;
				}

				ff.data.PushStream(XFILE_BLOCK_TEMP);

				ScriptBundle& bundle{ ff.data.AllocStreamRef<ScriptBundle>(ref) };

				// add name and type to bundle
				bundle.name.name = ctx.HashXHash(name);
				rapidjson::Value nameValue{};
				nameValue.SetString(name.data(), main.GetAllocator());
				main.AddMember(rapidjson::StringRef("name"), nameValue, main.GetAllocator());

				if (type.size()) {
					bundle.bundleType.name = ctx.HashXHash(type);

					rapidjson::Value typeValue{};
					typeValue.SetString(type.data(), main.GetAllocator());
					main.AddMember(rapidjson::StringRef("type"), typeValue, main.GetAllocator());
				}

				ff.data.PushStream(XFILE_BLOCK_VIRTUAL);
				if (!WriteArray(ctx, main, bundle.sbObjectsArray, ff)) {
					LOG_ERROR("Can't compile json object");
					ctx.error = true;
					return;
				}
				ff.data.PopStream();

				ff.data.PopStream();


				LOG_INFO("Added asset scriptbundle {} (hash_{:x})", rfpath.string(), bundle.name.name);
			}
		};
	}

	utils::MapAdder<XAssetLinkerImpl, XAssetType, XAssetLinker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_SCRIPTBUNDLE };
}