#include <includes.hpp>
#include <tools/cordycep_dump.hpp>
#include "tools/bo6/bo6.hpp"
#include "tools/hashes/compiled_files.hpp"
#include <core/memory_allocator.hpp>
#include "tools/pool.hpp"
#include <tools/utils/data_utils.hpp>

namespace {
	using namespace tool::cordycep::dump;
	using namespace bo6;

	class PoolOptionImpl : public PoolOption {
	public:
		bool m_help = false;
		bool m_any_type = false;
		bool m_dump_info = false;
		bool m_dump_all_available = false;
		bool m_pools{};
		bool m_cf_files{};
		bool m_usev1{};
		bool m_sp{};
		bool m_showErr{};
		bool m_mapLocalized{};
		bool m_ignoreOld{};
		bool analVal{};
		const char* m_output = "output_bo6";
		const char* m_dump_strings{};
		std::vector<bool> m_dump_types{};
		uint64_t flags{};
		std::unordered_set<std::string> dstrings{};

		bool Compute(const char** args, INT startIndex, INT endIndex) {
			m_dump_types.clear();
			m_dump_types.reserve(T10R_ASSET_COUNT);
			for (size_t i = 0; i < T10R_ASSET_COUNT; i++) {
				m_dump_types.push_back(false);
			}
			// default values
			for (size_t i = startIndex; i < endIndex; i++) {
				const char* arg = args[i];

				if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
					m_help = true;
				}
				else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					m_output = args[++i];
				}
				else if (!_strcmpi("--all", arg) || !strcmp("-a", arg)) {
					m_dump_all_available = true;
					m_any_type = true;
				}
				else if (!_strcmpi("--pools", arg) || !strcmp("-p", arg)) {
					m_pools = true;
				}
				else if (!_strcmpi("--cf", arg) || !strcmp("-C", arg)) {
					m_cf_files = true;
				}
				else if (!_strcmpi("--localized", arg) || !strcmp("-L", arg)) {
					m_mapLocalized = true;
				}
				else if (!_strcmpi("--err", arg) || !strcmp("-e", arg)) {
					m_showErr = true;
				}
				else if (!_strcmpi("--sp", arg)) {
					m_sp = true;
				}
				else if (!_strcmpi("--v1", arg)) {
					m_usev1 = true;
				}
				else if (!_strcmpi("--analVal", arg)) {
					analVal = true;
				}
				else if (!_strcmpi("--ignoreOld", arg)) {
					m_ignoreOld = true;
				}
				else if (!strcmp("-S", arg) || !_strcmpi("--strings", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					m_dump_strings = args[++i];
				}
				else if (*arg == '-') {
					std::cerr << "Invalid argument: " << arg << "!\n";
					return false;
				}
				else {
					T10RAssetType assetType = PoolIdRelease(arg);

					if (assetType == T10R_ASSET_COUNT) {
						if (std::isdigit(arg[0])) {
							try {
								assetType = (T10RAssetType)std::strtol(arg, nullptr, 10);
							}
							catch (const std::invalid_argument& e) {
								std::cerr << e.what() << "\n";
								assetType = T10R_ASSET_COUNT;
							}
						}
						if (assetType < 0 || assetType >= T10R_ASSET_COUNT) {
							std::cerr << "Invalid pool name: " << arg << "!\n";
							return false;
						}
					}

					m_dump_types[assetType] = true;
					m_any_type = true;
				}
			}
			return true;
		}
		void PrintHelp() {
			LOG_INFO("-h --help            : Print help");
			LOG_INFO("-o --output [d]      : Output dir");
			LOG_INFO("-p --pools           : Dump pools");
			LOG_INFO("-C --cf              : Produce .cf files");
			LOG_INFO("-e --err             : Show errors");
			LOG_INFO("-L --localized       : Map localized");
			LOG_INFO("-a --all             : Dump all available pools");
			LOG_INFO("--v1                 : Use old pool dumper (compatibility)");
			LOG_INFO("--analVal            : Dump binaries");
			LOG_INFO("--ignoreOld          : Ignore old");
		}

		const char* AddString(const char* str) override {
			if (m_dump_strings) {
				dstrings.insert(str);
			}
			return str;
		}
	};

	struct GscObjEntry {
		uint64_t name;
		int len;
		int padc;
		uintptr_t buffer;
	};


	enum StringTableType : uint8_t {
		STT_UNK0 = 0x0,
		STT_STRING = 0x1,
		STT_INT64 = 0x2,
		STT_FLOAT = 0x3,
		STT_BOOL = 0x4,
		STT_XHASH = 0x5,
		STT_XHASH_RES = 0x6,
		STT_XHASH_DVAR = 0x7,
		STT_UNK8_32 = 0x8,
		STT_XHASH_LOCALIZED = 0x9,
	};
	union StringTableValue {
		uint64_t u64;
		uint32_t u32;
		float f;
		const char* str;
		bool b;
	};

	struct StringTableOutput {
		StringTableValue output;
		uint32_t unk8;
		StringTableType type;
	};

	struct StringTableColumn {
		StringTableType type;
		uintptr_t unk8_row; // uint16_t*
		uintptr_t offsets_row; // uint16_t*
		uint64_t unk18;
		uintptr_t data; // void*
	};

	struct StringTable {
		uint64_t hash;
		int32_t columnCount;
		int32_t rowCount;
		uint64_t unk10;
		uintptr_t columns; // StringTableColumn*
	};

	struct LocalizeEntry {
		uint64_t hash;
		uintptr_t value;
	};
	struct LuaFileEntry {
		uintptr_t name;
		uint32_t len;
		uintptr_t buffer;
	};

	namespace sbv1 {
		struct ScriptBundleObject {
			uint64_t name;
			uintptr_t def; // ScriptBundleObjectDef*
		};

		enum ScriptBundleObjectDefType : byte {
			SBT_UNDEFINED = 0x0,
			SBT_INT1 = 0x1,
			SBT_INT2 = 0x2,
			SBT_FLOAT = 0x3,
			SBT_ISTRING = 0x4,
			SBT_STRING = 0x5,
			SBT_STRUCT = 0x6,
			SBT_ARRAY_INDEXED = 0x7,
			SBT_ARRAY = 0x8,
			SBT_STRUCT_ANIMATION = 0x9,
			SBT_STRUCT_NAMEID = 0xA,
			SBT_HASH = 0xB,
			SBT_HASH_TAG = 0xC,
			SBT_HASH_DVAR = 0xD,
			SBT_UNKE = 0xE,
			SBT_HASH_RESOURCE = 0xF,
		};

		union ScriptBundleObjectDefValue {
			int32_t int_val;
			float float_val;
			uint32_t id;
			uint64_t hash;
			uint32_t hash32;
			uintptr_t str; // const char**
			uintptr_t anim; // ScriptBundleObjectDefValueAnim*
			uintptr_t array; // ScriptBundleObjectDefValueArray*
			uintptr_t arrayIndexed; // ScriptBundleObjectDefValueArrayIndexed*
			uintptr_t obj; // ScriptBundleObjectData*
			uintptr_t nameId; // ScriptBundleObjectDefValueNameId*
		};

		struct ScriptBundleObjectDefValueAnim {
			uintptr_t name_offset;
			uint64_t id;
			uint64_t anim;
		};

		struct ScriptBundleObjectDefValueArray {
			int32_t count;
			int32_t id;
			uintptr_t defs; // ScriptBundleObjectDef*
		};

		struct ScriptBundleObjectDefValueArrayIndexed {
			int32_t count;
			int32_t id;
			uintptr_t names2; // uint64_t*
			uintptr_t names1; // uint64_t*
			uintptr_t vals; // ScriptBundleObjectDef*
		};

		struct ScriptBundleObjectData {
			uint32_t count;
			uintptr_t rawdata; // byte*
			uintptr_t names; // uint64_t*
			uintptr_t defs; // ScriptBundleObjectDef*
		};

		struct __declspec(align(8)) ScriptBundleObjectDefValueNameId {
			uintptr_t name; // const char**
			int32_t id;
		};

		struct ScriptBundleObjectDef {
			ScriptBundleObjectDefType type;
			byte pad[7];
			ScriptBundleObjectDefValue value;
		};

		struct ScriptBundle {
			uint64_t name;
			uint64_t unk8;
			ScriptBundleObjectData data;
		};



		bool WriteBundleDef(PoolOption& opt, std::ostringstream& os, Process& proc, ScriptBundle& entry, ScriptBundleObjectDef& def, int depth) {
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

	}

	namespace sbv2 {
		struct __declspec(align(8)) ScriptBundleObjectDefValueNameId {
			uintptr_t name; // const char**
			int32_t id;
		};

		struct ScriptBundleObjectDefValueArrayIndexed {
			uintptr_t names2; // uint64_t*
			uintptr_t names1; // uint64_t*
			uintptr_t vals; // ScriptBundleObjectDef*
			int32_t count;
			int32_t id;
		};

		struct ScriptBundleObjectDefValueArray {
			uintptr_t defs; // ScriptBundleObjectDef*
			uint64_t id;
			int32_t count;
		};

		struct ScriptBundleObjectDefValueAnim {
			uintptr_t name_offset; // const char**
			uint64_t id;
			uint32_t anim;
		};

		union ScriptBundleObjectDefValue {
			int32_t int_val;
			float float_val;
			uint32_t id;
			uint64_t hash;
			uint32_t hash32;
			uintptr_t str; // const char**
			uintptr_t anim; // ScriptBundleObjectDefValueAnim*
			uintptr_t array; // ScriptBundleObjectDefValueArray*
			uintptr_t arrayIndexed; // ScriptBundleObjectDefValueArrayIndexed*
			uintptr_t obj; // ScriptBundleObjectData*
			uintptr_t nameId; // ScriptBundleObjectDefValueNameId*
		};

		enum ScriptBundleObjectDefType : unsigned __int8 {
			SBT_UNDEFINED = 0x0,
			SBT_BOOL = 0x1,
			SBT_INTEGER = 0x2,
			SBT_FLOAT = 0x3,
			SBT_IHASH = 0x4,
			SBT_STRING = 0x5,
			SBT_STRUCT = 0x6,
			SBT_ARRAY_INDEXED = 0x7,
			SBT_ARRAY = 0x8,
			SBT_STRUCT_ANIMATION = 0x9,
			SBT_STRUCT_NAMEID = 0xA,
			SBT_HASH = 0xB,
			SBT_HASH_TAG = 0xC,
			SBT_HASH_DVAR = 0xD,
			SBT_HASH_SCR = 0xE,
			SBT_HASH_RESOURCE = 0xF,
		};

		struct ScriptBundleObjectDef {
			ScriptBundleObjectDefType type;
			byte pad[7];
			ScriptBundleObjectDefValue value;
		};


		struct ScriptBundleObjectData {
			uintptr_t rawdata; // byte*
			uintptr_t names; // uint64_t*
			uintptr_t defs; // ScriptBundleObjectDef*
			uint32_t count;
		};

		struct ScriptBundle {
			uint64_t name;
			uint64_t unk8;
			ScriptBundleObjectData data;
		};


		bool WriteBundleDef(PoolOption& opt, std::ostringstream& os, Process& proc, ScriptBundle& entry, ScriptBundleObjectDef& def, int depth, std::function<const char* (uint64_t hash)>& GetLocalized) {
			switch (def.type) {
			case SBT_UNDEFINED:
				os << "undefined";
				break;
			case SBT_BOOL:
				if (def.value.int_val && def.value.int_val != 1) {
					os << std::dec << def.value.int_val; // wtf?
				}
				else {
					os << '"' << (def.value.int_val ? "TRUE" : "FALSE") << '"';
				}
				break;
			case SBT_INTEGER:
				os << std::dec << def.value.int_val;
				break;
			case SBT_FLOAT:
				os << def.value.float_val;
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
						if (!WriteBundleDef(opt, os, proc, entry, vals[i], depth + 1, GetLocalized)) return false;
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
						if (!WriteBundleDef(opt, os, proc, entry, vals[i], depth + 1, GetLocalized)) return false;
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
						if (!WriteBundleDef(opt, os, proc, entry, defs[i], depth + 1, GetLocalized)) return false;
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
			case SBT_IHASH:
				os << "\"r#" << GetLocalized(def.value.hash) << "\"";
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
			case SBT_HASH_SCR:
				os << "\"&#" << hashutils::ExtractTmp("hash", def.value.hash) << "\"";
				break;
			default:
				os << "\"error:0x" << std::hex << (int)def.type << "\"";
				return false;
			}
			return true;
		}
	}
	bool IsNull(void* ptr, size_t p) {
		byte* pp{ (byte*)ptr };
		size_t r{ p };
		static byte nullBuff[0x1000]{};
		while (r) {
			size_t tr{ std::min<size_t>(sizeof(nullBuff), r) };

			if (std::memcmp(pp, nullBuff, tr)) {
				return false;
			}

			r -= tr;
			pp += tr;
		}
		return true;
	}

	int dpcordimpl(Process& proc, compatibility::scobalula::csi::CordycepProc& cordycep, int argc, const char* argv[]) {
		PoolOptionImpl opt{};

		if (!opt.Compute(argv, 2, argc) || opt.m_help) {
			opt.PrintHelp();
			return tool::OK;
		}


		auto [pools, ok] = proc.ReadMemoryArray<XAssetPool64>(cordycep.poolsAddress, T10R_ASSET_COUNT);

		if (!ok) {
			LOG_ERROR("Can't read pools");
			return tool::BASIC_ERROR;
		}

		std::filesystem::path outDir = opt.m_output;

		if (opt.m_pools) {
			std::filesystem::path loc{ outDir / "pools_bo6.csv" };
			std::ofstream os{ loc };
			if (!os) {
				LOG_ERROR("Can't open {}", loc.string());
				return tool::BASIC_ERROR;
			}
			utils::CloseEnd ce{ [&os] {os.close(); } };

			os << "id,size,elements";

			XAsset64 asset{};
			for (size_t i = 0; i < T10R_ASSET_COUNT; i++) {
				const char* name = bo6::PoolNameRelease((bo6::T10RAssetType)i);

				os << "\n" << name << ",";

				XAssetPool64& pool = pools[i];

				if (!pool.Root) {
					os << "0,0";
					continue;
				}


				if (!proc.ReadMemory(&asset, pool.Root, sizeof(asset))) {
					os << "<error>,<error>";
					LOG_ERROR("Can't dump pool {}", name);
					continue;
				}

				size_t count{ 1 };

				while (asset.Next) {
					if (!proc.ReadMemory(&asset, asset.Next, sizeof(asset))) {
						count = std::string::npos;
						LOG_ERROR("Can't dump pool {}", name);
						break;
					}

					count++;
				}

				if (count == std::string::npos) {
					os << "<error>";
				}
				else {
					os << std::hex << asset.HeaderSize << "," << count;
				}
			}

			LOG_INFO("Dump into {}", loc.string());

			return tool::OK;
		}

		if (!opt.m_any_type) {
			opt.PrintHelp();
			return tool::OK;
		}

		hashutils::ReadDefaultFile();

		size_t total{};

		if (opt.analVal) {
			for (size_t i = 0; i < T10R_ASSET_COUNT; i++) {
				T10RAssetType type{ (T10RAssetType)i };

				if (!opt.m_dump_types[type] && !opt.m_dump_all_available) {
					continue; // not handled
				}

				size_t nullFiles{};
				const char* name{ PoolNameRelease(type) };
				std::filesystem::path outDirPool{ outDir / name };
				std::filesystem::create_directories(outDirPool);
				int du = ForEachEntry(proc, pools[type], [&nullFiles, &outDirPool, &proc](const XAsset64& asset, size_t count) -> bool {
					std::filesystem::path outFile{ outDirPool / std::format("file_{:x}.txt", asset.ID) };

					try {
						auto buff{ proc.ReadMemoryArrayEx<byte>(asset.Header, asset.HeaderSize) };
						if (IsNull(buff.get(), asset.HeaderSize)) {
							//LOG_ERROR("null file {}", outFile.string());
							nullFiles++;
							return false;
						}

						utils::OutFileCE os{ outFile };

						os << hashutils::ExtractTmp("hash", asset.ID) << "\n\n";

						tool::pool::WriteHex(os, asset.Header, (void*)buff.get(), asset.HeaderSize, proc);
						LOG_INFO("dump {}", outFile.string());
					}
					catch (...) {
						LOG_INFO("error reading {}", outFile.string());
					}

					return true;
				});

				if (nullFiles) {
					LOG_ERROR("Find {} null files", nullFiles);
				}

				if (du < 0) {
					LOG_ERROR("Error reading pool: {}", name);
				}
				else {
					total += du;
				}

			}

			return tool::OK;
		}

		auto HandlePool = [&opt, &pools, &proc, &total](T10RAssetType type, const std::filesystem::path& outDir, std::function<bool(const XAsset64& asset, size_t count)> func) {
			if (!opt.m_dump_types[type] && !opt.m_dump_all_available) {
				return;
			}

			opt.m_dump_types[type] = false;

			const char* name{ PoolNameRelease(type) };
			if (opt.m_sp) {
				switch (type) {
				case T10RAssetType::T10R_ASSET_GSCOBJ:
					type = (T10RAssetType)69;
					break;
				case T10RAssetType::T10R_ASSET_GSCGDB:
					type = (T10RAssetType)70;
					break;
				default:
					LOG_ERROR("Can't remap pool {}", name);
					return;
					break;
				}
			}

			std::filesystem::create_directories(outDir);

			LOG_TRACE("Reading pool {} ({})", name, (int)type);

			int du = ForEachEntry(proc, pools[type], func);

			if (du < 0) {
				LOG_ERROR("Error reading pool: {}", name);
			}
			else {
				total += du;
			}
			};

		struct {
			bool loaded{};
			core::memory_allocator::MemoryAllocator alloc{};
			std::unordered_map<uint64_t, const char*> map{};
		} localized;

		std::function<const char*(uint64_t hash)> GetLocalized = [&opt, &localized, &proc, &pools](uint64_t hash) -> const char* {
			if (!opt.m_mapLocalized) {
				return hashutils::ExtractTmp("hash", hash);
			}
			if (!localized.loaded) {

				localized.loaded = true;

				ForEachEntry(proc, pools[T10R_ASSET_LOCALIZE], [&proc, &localized](const XAsset64& asset, size_t count) {
					LocalizeEntry entry{};
					if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
						LOG_ERROR("Can't read LocalizeEntry {:x}", asset.Header);
						return false;
					}

					localized.map[entry.hash & hash::MASK62] = localized.alloc.CloneStr(proc.ReadStringTmp(entry.value));

					return true;
					});
			}

			auto it = localized.map.find(hash & hash::MASK62);
			if (it == localized.map.end()) {
				return hashutils::ExtractTmp("hash", hash);
			}
			return it->second;
			};

		std::filesystem::path dumpDir = outDir / "dump";

		HandlePool(T10R_ASSET_STRINGTABLE, dumpDir, [&opt, &proc, dumpDir, &GetLocalized](const XAsset64& asset, size_t count) -> bool {
			StringTable entry{};
			if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
				LOG_ERROR("Can't read StringTable {:x}", asset.Header);
				return false;
			}

			const char* filename = hashutils::ExtractPtr(entry.hash);

			if (!filename) filename = utils::va("hashed/stringtable/file_%llx.%s", entry.hash, opt.m_cf_files ? ".cf" : "csv");

			std::filesystem::path out = dumpDir / filename;
			if (opt.m_ignoreOld && std::filesystem::exists(out)) return true;
			std::filesystem::create_directories(out.parent_path());

			LOG_INFO("Dump {} -> {}", hashutils::ExtractTmp("hash", entry.hash), out.string());
			std::ostringstream os{};
			if (entry.columnCount * entry.rowCount) {
				if (!entry.columns) {
					LOG_ERROR("No columns");
					return false;
				}
				auto columns{ proc.ReadMemoryArrayEx<StringTableColumn>(entry.columns, entry.columnCount) };

				for (size_t i = 0; i < entry.columnCount; i++) {
					StringTableColumn& col{ columns[i] };

					if (i) os << ",";

					switch (col.type) {
					case STT_STRING: os << "string"; break;
					case STT_INT64: os << "int"; break;
					case STT_FLOAT: os << "float"; break;
					case STT_BOOL: os << "bool"; break;
					case STT_XHASH: os << "xhash"; break;
					case STT_XHASH_RES: os << "xhashres"; break;
					case STT_XHASH_LOCALIZED: os << "xhashlocalized"; break;
					case STT_XHASH_DVAR: os << "xhashdvar"; break;
					case STT_UNK8_32: os << "unk8_32"; break;
					default: os << "unk" << std::dec << col.type; break;
					}
				}
				for (size_t i = 0; i < entry.rowCount; i++) {
					os << "\n";
					for (size_t j = 0; j < entry.columnCount; j++) {
						if (j) {
							os << ",";
						}
						StringTableColumn& column = columns[j];
						if (!column.data) {
							if (opt.m_showErr) {
								os << "<no_data>";
							}
							continue;
						}
						int16_t rowIndex = proc.ReadMemory<int16_t>(column.offsets_row + i * sizeof(int16_t));

						int elemSize;
						switch (column.type) {
						case STT_STRING:
						case STT_XHASH:
						case STT_XHASH_RES:
						case STT_XHASH_LOCALIZED:
						case STT_XHASH_DVAR:
						case STT_INT64:
							elemSize = 8;
							break;
						case STT_UNK8_32:
						case STT_FLOAT:
							elemSize = 4;
							break;
						case STT_BOOL:
							elemSize = 1;
							break;
						default:
							elemSize = 0;
							break;
						}
						if (!elemSize) {
							os << "<badtype:" << std::hex << column.type << ">";
							continue; // wtf?
						}
						byte value[0x20];

						if (!proc.ReadMemory(value, column.data + elemSize * rowIndex, elemSize)) {
							LOG_ERROR("Can't read column {}x{} {:x}", i, j, column.data);
							continue;
						}
						switch (column.type) {
						case STT_STRING: {
							const char* strr{ proc.ReadStringTmp(*reinterpret_cast<uintptr_t*>(value), nullptr) };
							if (strr) {
								os << opt.AddString(strr);
							}
							else if (opt.m_showErr) {
								os << "<invalid:0x" << std::hex << *reinterpret_cast<uintptr_t*>(value) << ">";
							}
						}
										break;
						case STT_XHASH:
							os << "#" << hashutils::ExtractTmp("hash", *reinterpret_cast<uint64_t*>(value));
							break;
						case STT_XHASH_RES:
							os << "%#" << hashutils::ExtractTmp("hash", *reinterpret_cast<uint64_t*>(value));
							break;
						case STT_XHASH_LOCALIZED:
							os << "r#" << GetLocalized(*reinterpret_cast<uint64_t*>(value));
							break;
						case STT_XHASH_DVAR:
							os << "@#" << hashutils::ExtractTmp("hash", *reinterpret_cast<uint64_t*>(value));
							break;
						case STT_INT64:
							os << std::dec << *reinterpret_cast<uint64_t*>(value);
							break;
						case STT_UNK8_32:
							os << std::dec << *reinterpret_cast<uint32_t*>(value);
							break;
						case STT_FLOAT:
							os << *reinterpret_cast<float*>(value);
							break;
						case STT_BOOL:
							os << (*value ? "TRUE" : "FALSE");
							break;
						default:
							os << "unk:" << column.type;
							break;
						}
					}
				}

			}

			if (opt.m_cf_files) {
				std::ofstream osf{ out, std::ios::binary };
				if (osf) {
					tool::hashes::compiled_files::CompiledFileHeader header{};
					header.name = entry.hash;
					header.isString = true;
					strcpy_s(header.type, "stringtables");
					strcpy_s(header.preferedExtension, "csv");
					osf.write((const char*)&header, sizeof(header));
					std::string osc = os.str();
					osf.write(osc.data(), osc.size());
					osf.close();
				}
			}
			else {
				utils::WriteFile(out, os.str());
			}


			return true;
			});
		const std::filesystem::path gscDir = outDir / "gsc";
		HandlePool(T10R_ASSET_GSCOBJ, gscDir, [&opt, &proc, gscDir](const XAsset64& asset, size_t count) -> bool {
			GscObjEntry entry{};
			LOG_TRACE("{} / {:x} / {:x}", hashutils::ExtractTmpScript(asset.ID), asset.Header, asset.HeaderSize);
			if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
				LOG_ERROR("Can't read gscobj {:x}", asset.Header);
				return false;
			}

			//LOG_INFO("{}", utils::data::AsHex(&asset, sizeof(asset)));

			if (entry.len <= 0) {
				LOG_INFO("Invalid size for {} (0x{:x}) 0x{:x}", hashutils::ExtractTmpScript(entry.name), entry.len, entry.buffer);
				return false;
			}

			std::filesystem::path out = gscDir / utils::va("script_%llx.gscc", entry.name);
			if (opt.m_ignoreOld && std::filesystem::exists(out)) return true;
			LOG_INFO("Dump {} -> {} (0x{:x})", hashutils::ExtractTmpScript(entry.name), out.string(), entry.len);
			auto buff = std::make_unique<byte[]>(entry.len);
			if (!proc.ReadMemory(&buff[0], entry.buffer, entry.len)) {
				LOG_ERROR("Can't read gscobj buff {}", entry.name);
				return false;
			}
			if (!utils::WriteFile(out, &buff[0], entry.len)) {
				LOG_ERROR("Can't write file {}", out.string());
				return false;
			}

			return true;
			});

		const std::filesystem::path gdbDir = outDir / "gscgdb";
		HandlePool(T10R_ASSET_GSCGDB, gdbDir, [&opt, &proc, gdbDir](const XAsset64& asset, size_t count) -> bool {
			GscObjEntry entry{};
			if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
				LOG_ERROR("Can't read gscgdb {:x}", asset.Header);
				return false;
			}

			if (entry.len <= 0) {
				LOG_INFO("Invalid size for {} ({})", hashutils::ExtractTmpScript(entry.name), entry.len);
				return false;
			}

			std::filesystem::path out = gdbDir / utils::va("script_%llx.gdbc", entry.name);
			if (opt.m_ignoreOld && std::filesystem::exists(out)) return true;
			LOG_INFO("Dump {} -> {} (0x{:x})", hashutils::ExtractTmpScript(entry.name), out.string(), entry.len);
			auto buff = std::make_unique<byte[]>(entry.len);
			if (!proc.ReadMemory(&buff[0], entry.buffer, entry.len)) {
				LOG_ERROR("Can't read gscgdb buff {}", entry.name);
				return false;
			}
			if (!utils::WriteFile(out, &buff[0], entry.len)) {
				LOG_ERROR("Can't write file {}", out.string());
				return false;
			}

			return true;
			});

		HandlePool(T10R_ASSET_DDL, dumpDir, [&opt, &proc, dumpDir](const XAsset64& asset, size_t count) -> bool {
			struct DDL {
				uint64_t name;
				uintptr_t def; // DDLDef*
			};

			struct DDLDef {
				uint64_t name;
				uint64_t unk08;
				uint64_t unk10;
				uint64_t unk18;
				uintptr_t structList; // DDLStruct*
				uintptr_t enumList; // DDLEnum*
			};
			enum DDLType : byte {
				DDL_INVALID_TYPE = 0xFF,
				DDL_BYTE_TYPE = 0x0,
				DDL_SHORT_TYPE = 0x1,
				DDL_UINT_TYPE = 0x2,
				DDL_INT_TYPE = 0x3,
				DDL_UINT64_TYPE = 0x4,
				DDL_HASH_TYPE = 0x5,
				DDL_FLOAT_TYPE = 0x6,
				DDL_FIXEDPOINT_TYPE = 0x7,
				DDL_STRING_TYPE = 0x8,
				DDL_STRUCT_TYPE = 0x9,
				DDL_ENUM_TYPE = 0xA,
				DDL_PAD_TYPE = 0xB,
			};

			struct DDLMember {
				uintptr_t name; // const char*
				int32_t bitSize;
				uint32_t intSize;
				uint32_t offset;
				int32_t maxIntValue;
				int16_t arraySize;
				uint16_t externalIndex;
				int16_t hashTableIndex;
				DDLType type;
				byte isArray;
			};
			struct DDLHash {
				uint64_t hash;
				int index;
			};

			struct DDLHashTable {
				DDLHash* list;
				int count;
				int max;
			};

			struct DDLStruct {
				uintptr_t name; // const char*
				int bitSize;
				int memberCount;
				uintptr_t members; // DDLMember*
				DDLHashTable hashTableLower;
				DDLHashTable hashTableUpper;
			};
			struct DDLEnum {
				uint64_t name;
				DDLHashTable list;
				uintptr_t members; // uint64_t
				uint16_t count;
				uint16_t unk0;
				uint32_t unk1;
			};


			DDL entry{};
			DDLDef def{};
			if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
				LOG_ERROR("Can't read DDL {:x}", asset.Header);
				return false;
			}


			const char* dfilename = hashutils::ExtractPtr(entry.name);

			if (!dfilename) dfilename = utils::va("hashed/ddl/file_%llx.%s", entry.name, opt.m_cf_files ? ".cf" : "ddl");

			std::filesystem::path loc{ dumpDir / dfilename };
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());
			std::ostringstream os{};

			const char* filename = hashutils::ExtractTmpPath("ddl", entry.name);

			LOG_INFO("Dump {} -> {}", filename, loc.string());
			os << "// file " << filename << "\n\n";


			if (!entry.def || !proc.ReadMemory(&def, entry.def, sizeof(def))) {
				os << "// Can't read DDL definition\n";
				LOG_ERROR("Can't read DDL definition {}", entry.def);
			}
			else {
				std::set<int> toParse{ {0} };
				std::set<int> toParseEnum{};
				std::set<int> parsedStructs{};


				if (!def.structList) {
					LOG_ERROR("Missing struct pointer");
					os << "// missing struct pointer\n";
					return false;
				}

				DDLStruct stct{};

				while (toParse.size()) {
					auto first = toParse.begin();
					int id = *first;
					toParse.erase(first);
					if (parsedStructs.contains(id)) continue;
					parsedStructs.insert(id);

					if (!proc.ReadMemory(&stct, def.structList + sizeof(stct) * id, sizeof(stct))) {
						LOG_ERROR("Can't read DDL struct {} / {}", hashutils::ExtractTmpPath("ddl", entry.name), def.structList);
						return false;
					}

					os << "// bitSize: 0x" << std::hex << stct.bitSize << ", members: " << std::dec << stct.memberCount << "\n";
					os << "struct " << opt.AddString(proc.ReadStringTmp(stct.name)) << " {\n";

					if (stct.memberCount && stct.members) {
						auto [members, ok] = proc.ReadMemoryArray<DDLMember>(stct.members, stct.memberCount);

						if (!ok) {
							LOG_ERROR("Can't read DDL members {}", hashutils::ExtractTmpPath("ddl", entry.name));
							return false;
						}

						std::sort(&members[0], &members[stct.memberCount - 1], [](const DDLMember& d1, const DDLMember& d2) -> bool { return d1.offset < d2.offset; });

						for (size_t i = 0; i < stct.memberCount; i++) {
							DDLMember& member = members[i];
							utils::Padding(os, 1) << "// offset: 0x" << std::hex << member.offset << ", bitSize: 0x" << member.bitSize;
							if (!(member.bitSize & 0x7)) {
								os << "(0x" << std::hex << (member.bitSize >> 3) << "B)";
							}
							os << "\n";

							utils::Padding(os, 1);

							switch (member.type) {
							case DDL_BYTE_TYPE: os << "byte"; break;
							case DDL_SHORT_TYPE: os << "short"; break;
							case DDL_UINT_TYPE: {
								if (member.intSize == 1) {
									os << "bit";
									break;
								}
								os << "uint" << std::dec << member.intSize;

								break;
							}
							case DDL_INT_TYPE: {
								os << "int" << std::dec << member.intSize;
								break;
							}
							case DDL_UINT64_TYPE: os << "uint64"; break;
							case DDL_HASH_TYPE: os << "hash"; break;
							case DDL_FLOAT_TYPE: {
								if (member.intSize == 32) {
									os << "float";
									break;
								}
								if (member.intSize == 64) {
									os << "double";
									break;
								}

								os << "float" << std::dec << member.intSize;
								break;
							}
							case DDL_FIXEDPOINT_TYPE: {
								os << "fixedpoint" << std::dec << member.intSize;
								break;
							}
							case DDL_STRING_TYPE: os << "char"; break;
							case DDL_STRUCT_TYPE: {
								// os << "struct"; break;

								DDLStruct stct2{};

								if (!proc.ReadMemory(&stct2, def.structList + sizeof(stct2) * member.externalIndex, sizeof(stct2))) {
									LOG_ERROR("Can't read DDL {}", hashutils::ExtractTmpPath("ddl", entry.name));
									return false;
								}
								toParse.insert(member.externalIndex);
								os << opt.AddString(proc.ReadStringTmp(stct2.name));

								break;
							}
							case DDL_ENUM_TYPE: {
								// os << "enum"; break;

								DDLEnum enum2{};

								if (!proc.ReadMemory(&enum2, def.enumList + sizeof(enum2) * member.externalIndex, sizeof(enum2))) {
									LOG_ERROR("Can't read DDL {}", hashutils::ExtractTmpPath("ddl", entry.name));
									return false;
								}
								toParseEnum.insert(member.externalIndex);

								os << opt.AddString(proc.ReadStringTmp(enum2.name));


								break;
							}
							case DDL_PAD_TYPE: os << "pad"; break;
							default: os << "<invalid:" << std::dec << (int)member.type << ">"; break;
							}


							os << " " << opt.AddString(proc.ReadStringTmp(member.name));

							if (member.type == DDL_STRING_TYPE) {
								int strLen = member.bitSize / 8;
								if (member.isArray) {
									strLen /= member.arraySize;
								}
								os << "[" << std::dec << (int)strLen << "]";
							}

							if (member.isArray) {
								os << "[" << std::dec << (int)member.arraySize << "]";
							}
							else if (member.type == DDL_PAD_TYPE) {
								os << "[" << std::dec << member.bitSize << "]";
							}

							os << ";\n";
						}

					}
					else if (!stct.members) {
						LOG_ERROR("Missing struct members pointer");
						utils::Padding(os, 1) << "// missing struct members pointer\n";
					}

					os << "}\n\n";
				}

				DDLEnum enm{};
				for (int id : toParseEnum) {

					if (!proc.ReadMemory(&enm, def.enumList + sizeof(enm) * id, sizeof(enm))) {
						LOG_ERROR("Can't read DDL enum {}", hashutils::ExtractTmpPath("ddl", entry.name));
						return false;
					}


					os << "// members: " << std::hex << enm.count << "\n";
					os << "enum " << opt.AddString(proc.ReadStringTmp(enm.name)) << " {\n";
					if (enm.count && enm.members) {

						auto [members, ok] = proc.ReadMemoryArray<uint64_t>(enm.members, enm.count);

						if (!ok) {
							LOG_ERROR("Can't read DDL enum members {} / {}", hashutils::ExtractTmpPath("ddl", entry.name), enm.members);
							return false;
						}

						for (size_t i = 0; i < enm.count; i++) {
							utils::Padding(os, 1) << "\"" << opt.AddString(proc.ReadStringTmp(members[i])) << "\" = 0x" << std::hex << i << ",\n";
						}


					}
					else if (!enm.members) {
						LOG_ERROR("Missing enum members pointer");
						utils::Padding(os, 1) << "// missing enum members pointer\n";
					}

					os << "}\n\n";

				}
			}

			if (opt.m_cf_files) {
				std::ofstream osf{ loc, std::ios::binary };
				if (osf) {
					tool::hashes::compiled_files::CompiledFileHeader header{};
					header.name = entry.name;
					header.isString = true;
					strcpy_s(header.type, "ddl");
					strcpy_s(header.preferedExtension, "ddl");
					osf.write((const char*)&header, sizeof(header));
					std::string osc = os.str();
					osf.write(osc.data(), osc.size());
					osf.close();
				}
			}
			else {
				utils::WriteFile(loc, os.str());
			}


			return true;
			});

		HandlePool(T10R_ASSET_SCRIPTABLE, dumpDir, [&opt, &proc, dumpDir](const XAsset64& asset, size_t count) -> bool {
			struct ScriptableInfo
			{
				uint64_t unk0;
				uint64_t unk8;
				uint32_t flags;
				byte unk14;
				byte unk15;
				byte unk16;
				byte unk17;
				uint64_t unk18;
				uint64_t unk20;
				uint64_t unk28;
				uint32_t unk30;
				uint32_t unk34;
				uint64_t unk38;
				uint64_t unk40;
				uint32_t unk50_count;
				uint32_t unk4c;
				byte* unk50;
				uint64_t unk58;
				uint64_t unk60;
				uint16_t unk68;
				uint16_t unk6a;
				uint32_t unk6c;
			};

			struct Scriptable
			{
				uint64_t name;
				uint64_t unk8;
				uintptr_t nextScriptableDef; // Scriptable*
				uint64_t flags;
				uint32_t unk28_count;
				uint32_t unk24;
				uintptr_t unk28; // byte*
				uintptr_t unk30; // ScriptableInfo*
				uint64_t unk38;
				byte unk40;
				byte unk41;
				byte unk42;
				byte unk43;
				byte unk44;
				byte unk45;
				byte unk46;
				byte unk47;
				uint64_t unk48;
				byte unk50;
				byte unk51;
				byte unk52;
				byte unk53;
				uint32_t unk54;
				uint64_t unk58;
				byte unk60;
				byte unk61;
				byte unk62;
				byte unk63;
				byte unk64;
				byte unk65;
				byte unk66;
				byte unk67;
				uint64_t unk68;
				uint64_t unk70;
				uint64_t unk78;
				uint32_t unk80;
				byte unk84;
				byte unk85;
				byte unk86;
				byte unk87;
				uint32_t unk88;
				uint32_t numXModels;
				uintptr_t models; // XModel**
				uint64_t unk98;
				uintptr_t unka0; // SoundBankTransient*
				uint64_t unka8;
				uint64_t unkb0;
				uint64_t unkb8;
			};

			auto entry = proc.ReadMemoryObjectEx<Scriptable>(asset.Header);
			auto info = proc.ReadMemoryObjectEx< ScriptableInfo>(entry->unk30);

			const char* filename = hashutils::ExtractPtr(entry->name);
			std::filesystem::path path{ std::filesystem::path{ opt.m_output } / (filename ? utils::va("scriptable/%s.json", filename) : utils::va("scriptable/file_%llx.json", entry->name)) };
			if (opt.m_ignoreOld && std::filesystem::exists(path)) return true;
			std::filesystem::create_directories(path.parent_path());

			utils::OutFileCE os{ path };

			if (!os) {
				LOG_ERROR("Can't open {}", path.string());
				return false;
			}
			os << "{\n";
			utils::Padding(os, 1) << "\"name\": \"#" << hashutils::ExtractTmp("hash", entry->name) << "\"";

			uint64_t next{ entry->nextScriptableDef ? proc.ReadMemory<uint64_t>(entry->nextScriptableDef) : 0 };

			if (next) {
				utils::Padding(os << ",\n", 1) << "\"next\": \"#" << hashutils::ExtractTmp("hash", next) << "\"";
			}

			if (entry->numXModels) {
				auto models = proc.ReadMemoryArrayEx<uintptr_t>(entry->models, entry->numXModels);

				utils::Padding(os << ",\n", 1) << "\"xModels\": [";
				for (size_t i = 0; i < entry->numXModels; i++) {
					if (i) os << ",";
					utils::Padding(os << "\n", 2) << "\"#" << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(models[i])) << "\"";
				}
				utils::Padding(os << "\n", 1) << "]";
			}

			os << "\n}";
			tool::pool::WriteHex(os, asset.Header, (void*)entry.get(), asset.HeaderSize, proc);
			os << "----------------------------------\n";
			tool::pool::WriteHex(os, asset.Header, (void*)info.get(), sizeof(*info), proc);

			LOG_INFO("Dump into {}", path.string());

			return true;
			});
		HandlePool(T10R_ASSET_SCRIPTBUNDLE, dumpDir, [&opt, &proc, dumpDir, &GetLocalized](const XAsset64& asset, size_t count) -> bool {
			if (opt.m_usev1) {
				using namespace sbv1;
				ScriptBundle entry{};
				if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
					LOG_ERROR("Can't read ScriptBundle {:x}", asset.Header);
					return false;
				}

				const char* filename = hashutils::ExtractPtr(entry.name);

				if (!filename) filename = utils::va("file_%llx.%s", entry.name, opt.m_cf_files ? ".cf" : "json");

				if (!opt.m_cf_files) {
					filename = utils::va("scriptbundle/%s.json", filename);
				}

				std::filesystem::path out = dumpDir / filename;
				if (opt.m_ignoreOld && std::filesystem::exists(out)) return true;
				std::filesystem::create_directories(out.parent_path());

				LOG_INFO("Dump {} -> {}", hashutils::ExtractTmp("hash", entry.name), out.string());
				std::ostringstream os{};

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

				if (opt.m_cf_files) {
					std::ofstream osf{ out, std::ios::binary };
					if (osf) {
						tool::hashes::compiled_files::CompiledFileHeader header{};
						header.name = entry.name;
						header.isString = true;
						header.isSpecial = true;
						strcpy_s(header.type, "scriptbundle");
						strcpy_s(header.preferedExtension, "json");
						osf.write((const char*)&header, sizeof(header));
						std::string osc = os.str();
						osf.write(osc.data(), osc.size());
						osf.close();
					}
				}
				else {
					utils::WriteFile(out, os.str());
				}
			}
			else {
				using namespace sbv2;
				ScriptBundle entry{};
				if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
					LOG_ERROR("Can't read ScriptBundle {:x}", asset.Header);
					return false;
				}

				const char* filename = hashutils::ExtractPtr(entry.name);

				if (!filename) {
					filename = utils::va("file_%llx", entry.name);
				}

				if (!opt.m_cf_files) {
					filename = utils::va("scriptbundle/%s.%s", filename, opt.m_cf_files ? ".cf" : "json");
				}

				std::filesystem::path out = dumpDir / filename;
				if (opt.m_ignoreOld && std::filesystem::exists(out)) return true;
				std::filesystem::create_directories(out.parent_path());

				LOG_INFO("Dump {} -> {}", hashutils::ExtractTmp("hash", entry.name), out.string());
				std::ostringstream os{};

				os << "{";

				if (entry.data.count) {
					auto objects = proc.ReadMemoryArrayEx<ScriptBundleObjectDef>(entry.data.defs, entry.data.count);
					auto names = proc.ReadMemoryArrayEx<uint64_t>(entry.data.names, entry.data.count);

					for (size_t i = 0; i < entry.data.count; i++) {
						if (i) os << ",";
						os << "\n";
						utils::Padding(os, 1) << "\"#" << hashutils::ExtractTmp("hash", names[i]) << "\": ";
						WriteBundleDef(opt, os, proc, entry, objects[i], 1, GetLocalized);
					}

				}
				os
					<< "\n"
					<< "}";

				if (opt.m_cf_files) {
					std::ofstream osf{ out, std::ios::binary };
					if (osf) {
						tool::hashes::compiled_files::CompiledFileHeader header{};
						header.name = entry.name;
						header.isString = true;
						header.isSpecial = true;
						strcpy_s(header.type, "scriptbundle");
						strcpy_s(header.preferedExtension, "json");
						osf.write((const char*)&header, sizeof(header));
						std::string osc = os.str();
						osf.write(osc.data(), osc.size());
						osf.close();
					}
				}
				else {
					utils::WriteFile(out, os.str());
				}
			}

			return true;
			});
		// LocalizeEntry
		if (opt.m_dump_types[T10R_ASSET_LOCALIZE] || opt.m_dump_all_available) {
			opt.m_dump_types[T10R_ASSET_LOCALIZE] = false;

			std::ostringstream os{};

			os << "{";
			int du = ForEachEntry(proc, pools[T10R_ASSET_LOCALIZE], [&proc, &os](const XAsset64& asset, size_t count) {
				LocalizeEntry entry{};
				if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
					LOG_ERROR("Can't read LocalizeEntry {:x}", asset.Header);
					return false;
				}

				if (count) os << ",";

				utils::Padding(os << "\n", 1) << "\"#" << hashutils::ExtractTmp("hash", entry.hash) << "\": \"";
				utils::PrintFormattedString(os, proc.ReadStringTmp(entry.value)) << "\"";

				return true;
				});

			os << "\n}";

			if (du < 0) {
				LOG_ERROR("Error reading pool T10R_ASSET_LOCALIZE");
			}
			else {
				total += du;

				std::filesystem::create_directories(outDir);

				std::filesystem::path loc{ dumpDir / utils::va("localize.%s", opt.m_cf_files ? ".cf" : "json") };
				std::filesystem::create_directories(loc.parent_path());
				if (opt.m_cf_files) {
					std::ofstream osf{ loc, std::ios::binary };
					if (osf) {
						tool::hashes::compiled_files::CompiledFileHeader header{};
						header.name = hash::Hash64("localize.json");
						header.isString = true;
						strcpy_s(header.type, "localize");
						strcpy_s(header.preferedExtension, "json");
						osf.write((const char*)&header, sizeof(header));
						std::string osc = os.str();
						osf.write(osc.data(), osc.size());
						osf.close();
						LOG_INFO("Dump into {}", loc.string());
					}
				}
				else {
					utils::WriteFile(loc, os.str());
				}

			}

		}

		LOG_INFO("Dumped {} file(s)", total);

		if (opt.m_dump_strings) {
			std::ofstream outStr{ opt.m_dump_strings };
			if (outStr) {
				outStr << "string";
				for (const std::string& str : opt.dstrings) {
					outStr << "\n" << str;
				}
				outStr.close();
				LOG_INFO("Dumped {} string(s) into '{}'", opt.dstrings.size(), opt.m_dump_strings);
			}
			else {
				LOG_ERROR("Can't open file '{}'", opt.m_dump_strings);
			}
		}

		return tool::OK;
	}

	utils::ArrayAdder<tool::cordycep::dump::CordycepDumper> impl{ tool::cordycep::dump::GetDumpers(), compatibility::scobalula::csi::CG_BO6, dpcordimpl };
}