#include <includes.hpp>
#include <tools/cordycep/t10/dumper_t10.hpp>
#include <tools/cordycep/t10/dumper_t10_scriptbundle.hpp>

namespace tool::cordycep::dump::t10::scriptbundle {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t10;
	using namespace bo6;


	bool sbv1::WriteBundleDef(PoolOption& opt, std::ostream& os, Process& proc, ScriptBundle& entry, ScriptBundleObjectDef& def, int depth) {
		switch (def.type) {
		case SBT_UNDEFINED:
			os << "undefined";
			break;
		case SBT_INT1:
		case SBT_INT2:
			os << std::dec << def.value.int_val;
			break;
		case SBT_FLOAT:
			os << def.value.float_val;
			break;
		case SBT_ISTRING:
			os << "\"&" << opt.AddString(proc.ReadStringTmp(proc.ReadMemory<uintptr_t>(def.value.str))) << "\"";
			break;
		case SBT_STRING:
			os << "\"" << opt.AddString(proc.ReadStringTmp(proc.ReadMemory<uintptr_t>(def.value.str))) << "\"";
			break;
		case SBT_STRUCT: {
			os << "{";
			std::unique_ptr<ScriptBundleObjectData> obj = proc.ReadMemoryObjectEx<ScriptBundleObjectData>(def.value.obj);

			if (obj->count) {
				std::unique_ptr<uint64_t[]> names = proc.ReadMemoryArrayEx<uint64_t>(obj->names, obj->count);
				std::unique_ptr<ScriptBundleObjectDef[]> vals = proc.ReadMemoryArrayEx<ScriptBundleObjectDef>(obj->defs, obj->count);

				for (size_t i = 0; i < obj->count; i++) {

					if (i) os << ",";
					utils::Padding(os << "\n", depth + 1) << "\"#" << hashutils::ExtractTmp("hash", names[i]) << "\": ";
					if (!WriteBundleDef(opt, os, proc, entry, vals[i], depth + 1)) return false;
				}

			}

			utils::Padding(os << "\n", depth) << "}";
			break;
		}
		case SBT_ARRAY_INDEXED: {
			os << "[";
			auto arrayIndexed = proc.ReadMemoryObjectEx<ScriptBundleObjectDefValueArrayIndexed>(def.value.arrayIndexed);

			if (arrayIndexed->count) {
				auto vals = proc.ReadMemoryArrayEx<ScriptBundleObjectDef>(arrayIndexed->vals, arrayIndexed->count);
				auto names1 = proc.ReadMemoryArrayEx<uint64_t>(arrayIndexed->names1, arrayIndexed->count);
				auto names2 = proc.ReadMemoryArrayEx<uint64_t>(arrayIndexed->names2, arrayIndexed->count);

				for (size_t i = 0; i < arrayIndexed->count; i++) {
					if (i) os << ",";
					utils::Padding(os << "\n", depth + 1) << "\"#" << hashutils::ExtractTmp("hash", names1[i]) << ":#" << hashutils::ExtractTmp("hash", names2[i]) << "\": ";
					if (!WriteBundleDef(opt, os, proc, entry, vals[i], depth + 1)) return false;
				}

			}

			utils::Padding(os << "\n", depth) << "]";
			break;
		}
		case SBT_ARRAY: {
			os << "[";
			auto array = proc.ReadMemoryObjectEx<ScriptBundleObjectDefValueArray>(def.value.array);

			if (array->count) {
				auto defs = proc.ReadMemoryArrayEx<ScriptBundleObjectDef>(array->defs, array->count);

				for (size_t i = 0; i < array->count; i++) {
					if (i) os << ",";
					utils::Padding(os << "\n", depth + 1);
					if (!WriteBundleDef(opt, os, proc, entry, defs[i], depth + 1)) return false;
				}
			}

			utils::Padding(os << "\n", depth) << "]";
			break;
		}
		case SBT_STRUCT_ANIMATION: {
			os << "{";
			auto anim = proc.ReadMemoryObjectEx<ScriptBundleObjectDefValueAnim>(def.value.anim);
			const char* name = opt.AddString(proc.ReadStringTmp(proc.ReadMemory<uintptr_t>(anim->name_offset)));
			hashutils::Add(name, true, true);
			utils::Padding(os << "\n", depth + 1) << "\"#name\": \"" << name << "\"";
			if (anim->anim) utils::Padding(os << ",\n", depth + 1) << "\"#id\": \"anim#" << hashutils::ExtractTmp("hash", anim->anim) << "\"";
			utils::Padding(os << "\n", depth) << "}";
			break;
		}
		case SBT_STRUCT_NAMEID: {
			os << "{";
			auto nameId = proc.ReadMemoryObjectEx<ScriptBundleObjectDefValueNameId>(def.value.nameId);
			const char* name = opt.AddString(proc.ReadStringTmp(proc.ReadMemory<uintptr_t>(nameId->name)));
			utils::Padding(os << "\n", depth + 1) << "\"#name\": \"" << name << "\",";
			utils::Padding(os << "\n", depth + 1) << "\"#id\": " << std::dec << nameId->id;
			utils::Padding(os << "\n", depth) << "}";
			break;
		}
		case SBT_HASH:
			os << "\"#" << hashutils::ExtractTmp("hash", def.value.hash) << "\"";
			break;
		case SBT_HASH_TAG:
			os << "\"t#" << hashutils::ExtractTmp("hash", def.value.hash32) << "\"";
			break;
		case SBT_HASH_DVAR:
			os << "\"@#" << hashutils::ExtractTmp("hash", def.value.hash) << "\"";
			break;
		case SBT_HASH_RESOURCE:
			os << "\"%#" << hashutils::ExtractTmp("hash", def.value.hash) << "\"";
			break;
		default:
			os << "\"error:0x" << std::hex << (int)def.type << "\"";
			return false;
		}
		return true;
	}
	
	bool sbv2::WriteBundleDef(PoolOption& opt, utils::raw_file_extractor::JsonWriter& json, Process& proc, ScriptBundleObjectDef& def, std::function<const char* (uint64_t hash)>& GetLocalized) {
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
			json.WriteValueString(opt.AddString(proc.ReadStringTmp(proc.ReadMemory<uintptr_t>(def.value.str))));
			break;
		case SBT_STRUCT: {
			json.BeginObject();
			std::unique_ptr<ScriptBundleObjectData> obj = proc.ReadMemoryObjectEx<ScriptBundleObjectData>(def.value.obj);

			if (obj->count) {
				std::unique_ptr<uint64_t[]> names = proc.ReadMemoryArrayEx<uint64_t>(obj->names, obj->count);
				std::unique_ptr<ScriptBundleObjectDef[]> vals = proc.ReadMemoryArrayEx<ScriptBundleObjectDef>(obj->defs, obj->count);

				for (size_t i = 0; i < obj->count; i++) {
					json.WriterFieldNameHash(names[i]);
					if (!WriteBundleDef(opt, json, proc, vals[i], GetLocalized)) return false;
				}

			}

			json.EndObject();
			break;
		}
		case SBT_ARRAY_INDEXED: {
			json.BeginArray();
			auto arrayIndexed = proc.ReadMemoryObjectEx<ScriptBundleObjectDefValueArrayIndexed>(def.value.arrayIndexed);

			if (arrayIndexed->count) {
				auto vals = proc.ReadMemoryArrayEx<ScriptBundleObjectDef>(arrayIndexed->vals, arrayIndexed->count);
				auto names1 = proc.ReadMemoryArrayEx<uint64_t>(arrayIndexed->names1, arrayIndexed->count);
				auto names2 = proc.ReadMemoryArrayEx<uint64_t>(arrayIndexed->names2, arrayIndexed->count);

				for (size_t i = 0; i < arrayIndexed->count; i++) {
					json.WriterFieldNameHash(names1[i]);
					if (!WriteBundleDef(opt, json, proc, vals[i], GetLocalized)) return false;
				}

			}
			json.EndArray();
			break;
		}
		case SBT_ARRAY: {
			json.BeginArray();
			auto array = proc.ReadMemoryObjectEx<ScriptBundleObjectDefValueArray>(def.value.array);

			if (array->count) {
				auto defs = proc.ReadMemoryArrayEx<ScriptBundleObjectDef>(array->defs, array->count);

				for (size_t i = 0; i < array->count; i++) {
					if (!WriteBundleDef(opt, json, proc, defs[i], GetLocalized)) return false;
				}
			}

			json.EndArray();
			break;
		}
		case SBT_STRUCT_ANIMATION: {
			json.BeginObject();
			auto anim = proc.ReadMemoryObjectEx<ScriptBundleObjectDefValueAnim>(def.value.anim);
			const char* name = opt.AddString(proc.ReadStringTmp(proc.ReadMemory<uintptr_t>(anim->name_offset)));
			hashutils::Add(name, true, true);
			json.WriteFieldNameString("name");
			json.WriteValueString(name);
			if (anim->anim) {
				json.WriteFieldNameString("id");
				json.WriteValueHash(anim->anim, "anim#");
			}
			json.EndObject();
			break;
		}
		case SBT_STRUCT_NAMEID: {
			json.BeginObject();
			auto nameId = proc.ReadMemoryObjectEx<ScriptBundleObjectDefValueNameId>(def.value.nameId);
			const char* name = opt.AddString(proc.ReadStringTmp(proc.ReadMemory<uintptr_t>(nameId->name)));
			json.WriteFieldNameString("name");
			json.WriteValueString(name);
			json.WriteFieldNameString("id");
			json.WriteValueNumber(nameId->id);
			json.EndObject();
			break;
		}
		case SBT_HASH:
			json.WriteValueHash(def.value.hash, "#");
			break;
		case SBT_IHASH:
			json.WriteValueString(std::format("r#{}", GetLocalized(def.value.hash)));
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
		default:
			json.WriteValueString(utils::va("bad type 0x%x", def.type));
			return false;
		}
		return true;
	}
	bool sbv2::WriteBundleData(PoolOption& opt, utils::raw_file_extractor::JsonWriter& json, Process& proc, ScriptBundleObjectData& data, std::function<const char* (uint64_t hash)>& GetLocalized) {
		json.BeginObject();

		auto objects = proc.ReadMemoryArrayEx<ScriptBundleObjectDef>(data.defs, data.count);
		auto names = proc.ReadMemoryArrayEx<uint64_t>(data.names, data.count);

		for (size_t i = 0; i < data.count; i++) {
			json.WriterFieldNameHash(names[i]);
			if (!WriteBundleDef(opt, json, proc, objects[i], GetLocalized)) return false;
		}
		json.EndObject();
		return true;
	}

	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			if (opt.m_usev1) {
				using namespace sbv1;
				ScriptBundle entry{};
				if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
					LOG_ERROR("Can't read ScriptBundle {:x}", asset.Header);
					return false;
				}

				std::filesystem::path out = ctx.outDir / "dump" / "scriptbundle" / std::format("{}.json", hashutils::ExtractTmp("file", entry.name));
				if (opt.m_ignoreOld && std::filesystem::exists(out)) return true;
				std::filesystem::create_directories(out.parent_path());

				LOG_INFO("Dump {} -> {}", hashutils::ExtractTmp("hash", entry.name), out.string());

				utils::OutFileCE os{ out };

				if (!os) {
					LOG_ERROR("Can't open {}", out.string());
					return false;
				}

				os << "{";

				if (entry.data.count) {
					auto objects = proc.ReadMemoryArrayEx<ScriptBundleObjectDef>(entry.data.defs, entry.data.count);
					auto names = proc.ReadMemoryArrayEx<uint64_t>(entry.data.names, entry.data.count);

					for (size_t i = 0; i < entry.data.count; i++) {
						if (i) os << ",";
						os << "\n";
						utils::Padding(os, 1) << "\"#" << hashutils::ExtractTmp("hash", names[i]) << "\": ";
						WriteBundleDef(opt, os, proc, entry, objects[i], 1);
					}

				}
				os
					<< "\n"
					<< "}";

			}
			else {
				using namespace sbv2;
				ScriptBundle entry{};
				if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
					LOG_ERROR("Can't read ScriptBundle {:x}", asset.Header);
					return false;
				}

				std::filesystem::path out = ctx.outDir / "dump" / "scriptbundle" / std::format("{}.json", hashutils::ExtractTmp("file", entry.name));
				if (opt.m_ignoreOld && std::filesystem::exists(out)) return true;
				std::filesystem::create_directories(out.parent_path());

				LOG_INFO("Dump {} -> {}", hashutils::ExtractTmp("hash", entry.name), out.string());

				utils::raw_file_extractor::JsonWriter json{};

				sbv2::WriteBundleData(opt, json, proc, entry.data, ctx.GetLocalized);

				if (!json.WriteToFile(out)) {
					LOG_ERROR("Can't write to {}", out.string());
					return false;
				}
				
			}

			return true;
		}

	};

	utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_SCRIPTBUNDLE };
}