#include <includes.hpp>
#include <core/memory_allocator.hpp>
#include "compatibility/scobalula_csi.hpp"
#include "compatibility/xensik_gscbin.hpp"
#include "tools/gsc_opcodes.hpp"
#include "tools/sp23/sp23.hpp"
#include "tools/bo6/bo6.hpp"
#include "tools/mw19/mw19.hpp"
#include "tools/hashes/compiled_files.hpp"
#include "tools/pool.hpp"
#include "gsc_iw.hpp"

namespace tool::cordycep::dump {
	struct XAsset64 {
		uintptr_t Header;
		uint64_t Temp;
		uintptr_t Next;
		uintptr_t Previous;
		uint64_t ID;
		uint64_t Type;
		int64_t HeaderSize;
	};

	struct XAssetPool64 {
		uintptr_t Root;
		uintptr_t End;
		uint64_t LookupTable;
		uint64_t HeaderMemory;
		uint64_t AssetMemory;
	};

	class PoolOption {
	public:
		virtual const char* AddString(const char* str) = 0;
	};
	int ForEachEntry(Process& proc, XAssetPool64& pool, std::function<bool(const XAsset64& asset, size_t count)> func) {
		uintptr_t curr = pool.Root;

		XAsset64 asset{};
		int res{};
		bool ok{ true };
		size_t count{};
		while (curr) {
			if (!proc.ReadMemory(&asset, curr, sizeof(asset))) {
				LOG_ERROR("Can't read pool entry {:x}", curr);
				return false;
			}

			if (asset.Header) {
				if (!func(asset, count++)) {
					ok = false;
				}
				res++;
			}

			curr = asset.Next;
		}

		return ok ? res : -res;
	}

	using namespace sp23;
	using namespace bo6;

	namespace jup {
		class PoolOptionJup : public PoolOption {
		public:
			bool m_help = false;
			bool m_any_type = false;
			bool m_dump_info = false;
			bool m_dump_all_available = false;
			bool m_pools{};
			bool m_cf_files{};
			bool m_usev1{};
			const char* m_output = "output_jup";
			const char* m_dump_strings{};
			std::vector<bool> m_dump_types{};
			uint64_t flags{};
			std::unordered_set<std::string> dstrings{};

			bool Compute(const char** args, INT startIndex, INT endIndex) {
				m_dump_types.clear();
				m_dump_types.reserve(sp23::ASSET_COUNT);
				for (size_t i = 0; i < sp23::ASSET_COUNT; i++) {
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
					else if (!_strcmpi("--v1", arg)) {
						m_usev1 = true;
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
						auto assetType = sp23::AssetTypeFromName(arg);

						if (assetType == sp23::ASSET_COUNT) {
							if (std::isdigit(arg[0])) {
								try {
									assetType = (sp23::AssetType)std::strtol(arg, nullptr, 10);
								}
								catch (const std::invalid_argument& e) {
									std::cerr << e.what() << "\n";
									assetType = sp23::ASSET_COUNT;
								}
							}
							if (assetType < 0 || assetType >= sp23::ASSET_COUNT) {
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
				LOG_INFO("-a --all             : Dump all available pools");
				LOG_INFO("--v1                 : Use old pool dumper (compatibility)");
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


		enum StringTableCellType : uint8_t {
			STT_UNK_1_64 = 1, // string
			STT_UNK_9_64 = 9, // string2??
			STT_UNK_A_64 = 0xA, // string3??

			STT_UNK_2_64 = 2, // int??
			STT_HASH = 5, // hash (0xCBF29CE484222325/0x100000001B3)
			STT_HASH_RESOURCE = 6, // hash (0x47F5817A5EF961BA/0x100000001B3)
			STT_UNK_7_64 = 7, // ?

			STT_FLOAT = 3, // float??
			STT_UNK_8_32 = 8, // ?

			STT_BYTE = 4,
		};

		struct StringTableResult {
			uint8_t* result; // 0
			int unk8; // 8
			StringTableCellType type; // 12
		};

		struct StringTableColumn {
			StringTableCellType type;
			uint16_t* unk8;
			uintptr_t rowindexes; // uint16_t*
			uint64_t unk18;
			uintptr_t rowdata; // uint8_t*
		};

		struct StringTable {
			uint64_t name;
			int columnCount;
			int rowCount;
			uint64_t cellIndices;
			uintptr_t columns; // StringTableColumn*
			uint64_t strings;
		};

		namespace jupv1 {
			struct ScriptBundleObject;
			struct ScriptBundleObjectDef;
			struct ScriptBundleObjectDefValueArrayIndexedVal;

			struct ScriptBundle {
				uint64_t name;
				uint64_t unk8;
				uint64_t unk10;
				uintptr_t rawdata; // uint8_t*
				uint32_t count;
				uintptr_t objects; // ScriptBundleObject*
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
				SBT_HASH_RESOURCE = 0xE,
			};

			struct ScriptBundleObjectDefValueAnim
			{
				int32_t name_offset;
				int32_t id;
				uint64_t unk8;
				uint64_t anim;
			};

			struct ScriptBundleObjectDefValueArray
			{
				int32_t count;
				int32_t id;
				uintptr_t defs; // ScriptBundleObjectDef* 
			};

			struct ScriptBundleObjectDefValueArrayIndexed
			{
				int32_t count;
				int32_t id;
				uintptr_t vals; // ScriptBundleObjectDefValueArrayIndexedVal*
			};

			union ScriptBundleObjectDefValue
			{
				int32_t int_val;
				float float_val;
				uint32_t id;
				uint64_t hash;
				uint32_t hash_tag;
				ScriptBundleObjectDefValueAnim anim;
				ScriptBundleObjectDefValueArray array;
				ScriptBundleObjectDefValueArrayIndexed arrayIndexed;
				uint8_t data[24];
			};

			struct ScriptBundleObjectDef {
				ScriptBundleObjectDefType type;
				uint8_t pad[7];
				ScriptBundleObjectDefValue value;
			};

			struct ScriptBundleObject {
				uint64_t name0;
				uint64_t name;
				ScriptBundleObjectDef def;
			};

			struct ScriptBundleObjectDefValueArrayIndexedVal {
				uint64_t name1;
				uint64_t name2;
				uintptr_t def; // ScriptBundleObjectDef*
				uint64_t unk18;
				uint64_t unk20;
				uint64_t unk28;
			};


			bool WriteBundleDef(PoolOptionJup& opt, std::ostringstream& os, Process& proc, ScriptBundle& entry, ScriptBundleObjectDef& def, int depth) {
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
					os << "\"&" << opt.AddString(proc.ReadStringTmp(entry.rawdata + def.value.id)) << "\"";
					break;
				case SBT_STRING:
					os << "\"" << opt.AddString(proc.ReadStringTmp(entry.rawdata + def.value.id)) << "\"";
					break;
				case SBT_STRUCT: {
					os << "{";
					ScriptBundleObjectDefValueArrayIndexed& arrayIndexed = def.value.arrayIndexed;

					if (arrayIndexed.count) {
						auto [vals, ok] = proc.ReadMemoryArray<ScriptBundleObject>(arrayIndexed.vals, arrayIndexed.count);

						if (!ok) {
							LOG_ERROR("Can't read vals");
							return false;
						}

						for (size_t i = 0; i < arrayIndexed.count; i++) {
							ScriptBundleObject& val = vals[i];

							if (i) os << ",";
							utils::Padding(os << "\n", depth + 1) << "\"#" << hashutils::ExtractTmp("hash", val.name) << "\": ";
							if (!WriteBundleDef(opt, os, proc, entry, val.def, depth + 1)) return false;
						}

					}

					utils::Padding(os << "\n", depth) << "}";
					break;
				}
				case SBT_ARRAY_INDEXED: {
					os << "[";
					ScriptBundleObjectDefValueArrayIndexed& arrayIndexed = def.value.arrayIndexed;

					if (arrayIndexed.count) {
						auto [vals, ok] = proc.ReadMemoryArray<ScriptBundleObjectDefValueArrayIndexedVal>(arrayIndexed.vals, arrayIndexed.count);

						if (!ok) {
							LOG_ERROR("Can't read vals");
							return false;
						}

						for (size_t i = 0; i < arrayIndexed.count; i++) {
							ScriptBundleObjectDefValueArrayIndexedVal& val = vals[i];

							auto [def, ok] = proc.ReadMemoryObject<ScriptBundleObjectDef>(val.def);

							if (!ok) {
								LOG_ERROR("Can't read def");
								return false;
							}

							if (i) os << ",";
							utils::Padding(os << "\n", depth + 1) << "\"#" << hashutils::ExtractTmp("hash", val.name1) << ":#" << hashutils::ExtractTmp("hash", val.name2) << "\": ";
							if (!WriteBundleDef(opt, os, proc, entry, *def, depth + 1)) return false;
						}

					}

					utils::Padding(os << "\n", depth) << "]";
					break;
				}
				case SBT_ARRAY: {
					os << "[";
					ScriptBundleObjectDefValueArray& array = def.value.array;

					if (array.count) {
						auto [defs, ok] = proc.ReadMemoryArray<ScriptBundleObjectDef>(array.defs, array.count);
						if (!ok) {
							LOG_ERROR("Can't read vals");
							return false;
						}
						for (size_t i = 0; i < array.count; i++) {
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
					ScriptBundleObjectDefValueAnim& anim = def.value.anim;
					const char* name = opt.AddString(proc.ReadStringTmp(entry.rawdata + anim.name_offset));
					hashutils::Add(name, true, true);
					utils::Padding(os << "\n", depth + 1) << "\"#name\": \"" << name << "\",";
					utils::Padding(os << "\n", depth + 1) << "\"#id\": \"anim#" << hashutils::ExtractTmp("hash", anim.anim) << "\"";
					utils::Padding(os << "\n", depth) << "}";
					break;
				}
				case SBT_STRUCT_NAMEID: {
					os << "{";
					ScriptBundleObjectDefValueAnim& anim = def.value.anim;
					const char* name = opt.AddString(proc.ReadStringTmp(entry.rawdata + anim.name_offset));
					utils::Padding(os << "\n", depth + 1) << "\"#name\": \"" << name << "\",";
					utils::Padding(os << "\n", depth + 1) << "\"#id\": " << std::dec << anim.id;
					utils::Padding(os << "\n", depth) << "}";
					break;
				}
				case SBT_HASH:
					os << "\"#" << hashutils::ExtractTmp("hash", def.value.hash) << "\"";
					break;
				case SBT_HASH_TAG:
					os << "\"t#" << hashutils::ExtractTmp("hash", def.value.hash_tag) << "\"";
					break;
				case SBT_HASH_DVAR:
					os << "\"@" << hashutils::ExtractTmp("hash", def.value.hash) << "\"";
					break;
				case SBT_HASH_RESOURCE:
					os << "\"%" << hashutils::ExtractTmp("hash", def.value.hash) << "\"";
					break;
				default:
					os << "error";
					return false;
				}
				return true;
			}
		}
		namespace jupv2 {
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
					os << "\"@" << hashutils::ExtractTmp("hash", def.value.hash) << "\"";
					break;
				case SBT_HASH_RESOURCE:
					os << "\"%" << hashutils::ExtractTmp("hash", def.value.hash) << "\"";
					break;
				default:
					os << "\"error:0x" << std::hex << (int)def.type << "\"";
					return false;
				}
				return true;
			}
		}

		struct LocalizeEntry {
			uint64_t hash;
			uintptr_t value;
		};
		struct LuaFileEntry {
			uintptr_t name;
			uint32_t len;
			uintptr_t buffer;
		};

		int dpcordjup(Process& proc, compatibility::scobalula::csi::CordycepProc& cordycep, int argc, const char* argv[]) {
			PoolOptionJup opt{};

			if (!opt.Compute(argv, 2, argc) || opt.m_help) {
				opt.PrintHelp();
				return tool::OK;
			}


			auto [pools, ok] = proc.ReadMemoryArray<XAssetPool64>(cordycep.poolsAddress, sp23::ASSET_COUNT);

			if (!ok) {
				LOG_ERROR("Can't read pools");
				return tool::BASIC_ERROR;
			}

			std::filesystem::path outDir = opt.m_output;

			if (opt.m_pools) {
				std::filesystem::path loc{ outDir / "pools_jup.csv" };
				std::ofstream os{ loc };
				if (!os) {
					LOG_ERROR("Can't open {}", loc.string());
					return tool::BASIC_ERROR;
				}
				utils::CloseEnd ce{ [&os] {os.close(); } };

				os << "id,size,elements";

				XAsset64 asset{};
				for (size_t i = 0; i < ASSET_COUNT; i++) {
					const char* name = AssetTypeName((AssetType)i);

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

			auto HandlePool = [&opt, &pools, &proc, &total](sp23::AssetType type, const std::filesystem::path& outDir, std::function<bool(const XAsset64& asset, size_t count)> func) {
				if (!opt.m_dump_types[type] && !opt.m_dump_all_available) {
					return;
				}

				opt.m_dump_types[type] = false;

				std::filesystem::create_directories(outDir);

				int du = ForEachEntry(proc, pools[type], func);

				if (du < 0) {
					LOG_ERROR("Error reading pool: {}", sp23::AssetTypeName(type));
				}
				else {
					total += du;
				}
			};

			const std::filesystem::path gscDir = outDir / "gsc";
			HandlePool(ASSET_GSCOBJ, gscDir, [&proc, gscDir](const XAsset64& asset, size_t count) -> bool {
				GscObjEntry entry{};
				if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
					LOG_ERROR("Can't read gscobj {:x}", asset.Header);
					return false;
				}

				if (entry.len <= 0) {
					LOG_INFO("Invalid size for {} ({})", hashutils::ExtractTmpScript(entry.name), entry.len);
					return false;
				}

				std::filesystem::path out = gscDir / utils::va("script_%llx.gscc", entry.name);
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
			HandlePool(ASSET_GSCGDB, gdbDir, [&proc, gdbDir](const XAsset64& asset, size_t count) -> bool {
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

			const std::filesystem::path luaDir = outDir / "lua";
			HandlePool(ASSET_LUAFILE, luaDir, [&proc, luaDir](const XAsset64& asset, size_t count) -> bool {
				LuaFileEntry entry{};
				if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
					LOG_ERROR("Can't read luafile {:x}", asset.Header);
					return false;
				}

				if (entry.len <= 0) {
					LOG_INFO("Invalid size for {} ({})", hashutils::ExtractTmpScript(entry.name), entry.len);
					return false;
				}

				std::filesystem::path out = luaDir / utils::va("lua_%llx.lua", entry.name);
				LOG_INFO("Dump {} -> {} (0x{:x})", hashutils::ExtractTmpScript(entry.name), out.string(), entry.len);
				auto buff = std::make_unique<byte[]>(entry.len);
				if (!proc.ReadMemory(&buff[0], entry.buffer, entry.len)) {
					LOG_ERROR("Can't read luafile buff {}", entry.name);
					return false;
				}
				if (!utils::WriteFile(out, &buff[0], entry.len)) {
					LOG_ERROR("Can't write file {}", out.string());
					return false;
				}

				return true;
			});

			std::filesystem::path dumpDir = outDir / "dump";
			HandlePool(ASSET_STRINGTABLE, dumpDir, [&opt, &proc, dumpDir](const XAsset64& asset, size_t count) -> bool {
				StringTable entry{};
				if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
					LOG_ERROR("Can't read StringTable {:x}", asset.Header);
					return false;
				}

				const char* filename = hashutils::ExtractPtr(entry.name);

				if (!filename) filename = utils::va("hashed/stringtable/file_%llx.%s", entry.name, opt.m_cf_files ? ".cf" : "csv");

				std::filesystem::path out = dumpDir / filename;
				std::filesystem::create_directories(out.parent_path());

				LOG_INFO("Dump {} -> {}", hashutils::ExtractTmp("hash", entry.name), out.string());
				std::ostringstream os{};
				if (entry.columnCount * entry.rowCount) {
					auto columns = std::make_unique<StringTableColumn[]>(entry.columnCount);
					if (!proc.ReadMemory(&columns[0], entry.columns, sizeof(StringTableColumn) * entry.columnCount)) {
						LOG_ERROR("Can't read columns");
						return false;
					}

					for (size_t i = 0; i < entry.rowCount; i++) {
						for (size_t j = 0; j < entry.columnCount; j++) {
							if (j) {
								os << ",";
							}

							auto& column = columns[j];

							auto rowIndex = proc.ReadMemory<int16_t>(column.rowindexes + i * sizeof(int16_t));

							int elemSize;
							switch (column.type) {
							case STT_UNK_1_64:
							case STT_UNK_9_64:
							case STT_UNK_A_64:
							case STT_UNK_2_64:
							case STT_HASH:
							case STT_HASH_RESOURCE:
							case STT_UNK_7_64:
								elemSize = 8;
								break;
							case STT_FLOAT:
							case STT_UNK_8_32:
								elemSize = 4;
								break;
							case STT_BYTE:
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

							if (!proc.ReadMemory(value, column.rowdata + elemSize * rowIndex, elemSize)) {
								LOG_ERROR("Can't read column {}x{}", i, j);
								return false;
							}

							switch (column.type) {
							case STT_UNK_1_64:
							case STT_UNK_9_64:
							case STT_UNK_A_64:
								// strings??
								os << opt.AddString(proc.ReadStringTmp(*reinterpret_cast<uintptr_t*>(value)));
								break;
							case STT_UNK_2_64:
								// int?
								os << std::dec << *reinterpret_cast<int64_t*>(value);
								break;
							case STT_HASH:
								os << "#hash_" << std::hex << *reinterpret_cast<uint64_t*>(value);
								break;
							case STT_HASH_RESOURCE:
								os << "%hash_" << std::hex << *reinterpret_cast<uint64_t*>(value);
								break;
							case STT_UNK_7_64:
								os << "@hash_" << std::hex << *reinterpret_cast<uint64_t*>(value);
								break;
							case STT_FLOAT:
								os << *reinterpret_cast<float*>(value);
								break;
							case STT_UNK_8_32:
								os << "t#hash_" << std::hex << *reinterpret_cast<uint32_t*>(value);
								break;
							case STT_BYTE:
								os << (*value ? "TRUE" : "FALSE");
								break;
							default:
								os << "<error>";
								break;
							}
						}
						os << "\n";
					}
				}

				if (opt.m_cf_files) {
					std::ofstream osf{ out, std::ios::binary };
					if (osf) {
						tool::hashes::compiled_files::CompiledFileHeader header{};
						header.name = entry.name;
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

			HandlePool(ASSET_SCRIPTBUNDLE, dumpDir, [&opt, &proc, dumpDir](const XAsset64& asset, size_t count) -> bool {
				if (opt.m_usev1) {
					using namespace jupv1;
					ScriptBundle entry{};
					if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
						LOG_ERROR("Can't read ScriptBundle {:x}", asset.Header);
						return false;
					}

					const char* filename = hashutils::ExtractPtr(entry.name);

					if (!filename) filename = utils::va("file_%llx.%s", entry.name, opt.m_cf_files ? ".cf" : "json");

					if (!opt.m_cf_files) {
						filename = utils::va("scriptbundle/%s", filename);
					}

					std::filesystem::path out = dumpDir / filename;
					std::filesystem::create_directories(out.parent_path());

					LOG_INFO("Dump {} -> {}", hashutils::ExtractTmp("hash", entry.name), out.string());
					std::ostringstream os{};

					os << "{";

					if (entry.count) {
						auto [objects, ok] = proc.ReadMemoryArray<ScriptBundleObject>(entry.objects, entry.count);

						if (!ok) {
							LOG_ERROR("Can't read objects at 0x{:x}/{}", entry.objects, entry.count);
							return false;
						}

						for (size_t i = 0; i < entry.count; i++) {
							ScriptBundleObject& obj = objects[i];

							if (i) os << ",";
							os << "\n";
							utils::Padding(os, 1) << "\"#" << hashutils::ExtractTmp("hash", obj.name) << "\": ";
							WriteBundleDef(opt, os, proc, entry, obj.def, 1);
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
					using namespace jupv2;
					ScriptBundle entry{};
					if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
						LOG_ERROR("Can't read ScriptBundle {:x}", asset.Header);
						return false;
					}

					const char* filename = hashutils::ExtractPtr(entry.name);

					if (!filename) filename = utils::va("file_%llx.%s", entry.name, opt.m_cf_files ? ".cf" : "json");

					if (!opt.m_cf_files) {
						filename = utils::va("scriptbundle/%s", filename);
					}

					std::filesystem::path out = dumpDir / filename;
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

				return true;
			});
			HandlePool(ASSET_DDL, dumpDir, [&opt, &proc, dumpDir](const XAsset64& asset, size_t count) -> bool {
				struct DDL {
					uint64_t name;
					uintptr_t def; // DDLDef*
					uint64_t unk10;
					uint64_t unk18;
					uint64_t unk20;
					uint64_t unk28;
					uint64_t unk30;
					uint64_t unk38;
					uint64_t unk40;
					uint64_t unk48;
					uint64_t unk50;
					uint64_t unk58;
					uint64_t unk60;
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

			// LocalizeEntry
			if (opt.m_dump_types[ASSET_LOCALIZE] || opt.m_dump_all_available) {
				opt.m_dump_types[ASSET_LOCALIZE] = false;

				std::ostringstream os{};

				os << "{";

				int du = ForEachEntry(proc, pools[ASSET_LOCALIZE], [&proc, &os](const XAsset64& asset, size_t count) {
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
					LOG_ERROR("Error reading pool: {}", sp23::AssetTypeName(ASSET_LOCALIZE));
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

			for (size_t i = 0; i < opt.m_dump_types.size(); i++) {
				if (!opt.m_dump_types[i]) {
					continue;
				}
				AssetType at = (sp23::AssetType)i;
				const char* pn = sp23::AssetTypeName(at);

				std::filesystem::path rawDir = outDir / "raw" / pn;


				HandlePool(at, rawDir, [&proc, rawDir](const XAsset64& asset, size_t count) -> bool {
					GscObjEntry entry{};
					if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
						LOG_ERROR("Can't read gscobj {:x}", asset.Header);
						return false;
					}

					if (asset.HeaderSize <= 0) {
						return false;
					}

					auto buffer = std::make_unique<byte[]>(asset.HeaderSize);

					if (!proc.ReadMemory(&buffer[0], asset.Header, asset.HeaderSize)) {
						LOG_ERROR("Can't read header {:x}", asset.Header);
						return false;
					}

					std::filesystem::path loc;
					if (asset.ID) {
						loc = rawDir / utils::va("file_%llx.txt", asset.ID);
					}
					else {
						loc = rawDir / utils::va("raw_%lld.txt", count);
					}

					std::ofstream os{ loc };

					if (!os) {
						LOG_ERROR("Can't open {}", loc.string());
						return false;
					}
					utils::CloseEnd ce{ [&os] {os.close(); } };

					const char* name{ hashutils::ExtractTmp("hash", asset.ID) };

					LOG_INFO("Dump {} -> {}", name, loc.string());

					os << "// file " << name << "\n";
					os << "// len 0x" << std::hex << asset.HeaderSize << "\n\n";

					tool::pool::WriteHex(os, asset.Header, (void*)&buffer[0], asset.HeaderSize, proc);
					return true;
				});
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

	}

	namespace bo6 {
		class PoolOptionJup : public PoolOption {
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

		int dpcordbo6(Process& proc, compatibility::scobalula::csi::CordycepProc& cordycep, int argc, const char* argv[]) {
			PoolOptionJup opt{};

			if (!opt.Compute(argv, 2, argc) || opt.m_help) {
				opt.PrintHelp();
				return tool::OK;
			}


			auto [pools, ok] = proc.ReadMemoryArray<XAssetPool64>(cordycep.poolsAddress, sp23::ASSET_COUNT);

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
				for (size_t i = 0; i < ASSET_COUNT; i++) {
					const char* name = AssetTypeName((AssetType)i);

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

			auto GetLocalized = [&opt, &localized, &proc, &pools](uint64_t hash) -> const char* {
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
					return hashutils::ExtractTmp("unk", hash);
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
								os << *value ? "TRUE" : "FALSE";
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
			HandlePool(T10R_ASSET_GSCOBJ, gscDir, [&proc, gscDir](const XAsset64& asset, size_t count) -> bool {
				GscObjEntry entry{};
				if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
					LOG_ERROR("Can't read gscobj {:x}", asset.Header);
					return false;
				}

				if (entry.len <= 0) {
					LOG_INFO("Invalid size for {} ({})", hashutils::ExtractTmpScript(entry.name), entry.len);
					return false;
				}

				std::filesystem::path out = gscDir / utils::va("script_%llx.gscc", entry.name);
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
			HandlePool(T10R_ASSET_GSCGDB, gdbDir, [&proc, gdbDir](const XAsset64& asset, size_t count) -> bool {
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
						utils::Padding(os << "\n", 2) << "\"#"  << hashutils::ExtractTmp("hash", proc.ReadMemory<uint64_t>(models[i])) << "\"";
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
			HandlePool(T10R_ASSET_SCRIPTBUNDLE, dumpDir, [&opt, &proc, dumpDir](const XAsset64& asset, size_t count) -> bool {
					
					using namespace jup::jupv2;
					ScriptBundle entry{};
					if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
						LOG_ERROR("Can't read ScriptBundle {:x}", asset.Header);
						return false;
					}

					const char* filename = hashutils::ExtractPtr(entry.name);

					if (!filename) filename = utils::va("file_%llx.%s", entry.name, opt.m_cf_files ? ".cf" : "json");

					if (!opt.m_cf_files) {
						filename = utils::va("scriptbundle/%s", filename);
					}

					std::filesystem::path out = dumpDir / filename;
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

	}

	namespace mw19 {
		class PoolOptionJup : public PoolOption {
		public:
			bool m_help = false;
			bool m_any_type = false;
			bool m_dump_info = false;
			bool m_dump_all_available = false;
			bool m_pools{};
			bool m_cf_files{};
			bool m_showErr{};
			bool m_mapLocalized{};
			const char* m_output = "output_mw19";
			const char* m_dump_strings{};
			std::vector<bool> m_dump_types{};
			uint64_t flags{};
			std::unordered_set<std::string> dstrings{};

			bool Compute(const char** args, INT startIndex, INT endIndex) {
				m_dump_types.clear();
				m_dump_types.reserve(tool::mw19::IW19_ASSETTYPE_COUNT);
				for (size_t i = 0; i < tool::mw19::IW19_ASSETTYPE_COUNT; i++) {
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
						tool::mw19::IW19PoolType assetType = tool::mw19::PoolId(arg);

						if (assetType == tool::mw19::IW19_ASSETTYPE_COUNT) {
							if (std::isdigit(arg[0])) {
								try {
									assetType = (tool::mw19::IW19PoolType)std::strtol(arg, nullptr, 10);
								}
								catch (const std::invalid_argument& e) {
									std::cerr << e.what() << "\n";
									assetType = tool::mw19::IW19_ASSETTYPE_COUNT;
								}
							}
							if (assetType < 0 || assetType >= tool::mw19::IW19_ASSETTYPE_COUNT) {
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
			}

			const char* AddString(const char* str) override {
				if (m_dump_strings) {
					dstrings.insert(str);
				}
				return str;
			}
		};
		struct ScriptFile {
			uintptr_t name; // const char*
			int compressedLen;
			int len;
			int bytecodeLen;
			uintptr_t buffer; // const char* 
			uintptr_t bytecode; // unsigned __int8* 
		};


		int dpcordmw19(Process& proc, compatibility::scobalula::csi::CordycepProc& cordycep, int argc, const char* argv[]) {
			PoolOptionJup opt{};

			if (!opt.Compute(argv, 2, argc) || opt.m_help) {
				opt.PrintHelp();
				return tool::OK;
			}


			auto [pools, ok] = proc.ReadMemoryArray<XAssetPool64>(cordycep.poolsAddress, tool::mw19::IW19_ASSETTYPE_COUNT);

			if (!ok) {
				LOG_ERROR("Can't read pools");
				return tool::BASIC_ERROR;
			}

			std::filesystem::path outDir = opt.m_output;


			if (!opt.m_any_type) {
				opt.PrintHelp();
				return tool::OK;
			}

			hashutils::ReadDefaultFile();

			size_t total{};

			auto HandlePool = [&opt, &pools, &proc, &total](tool::mw19::IW19PoolType type, const std::filesystem::path& outDir, std::function<bool(const XAsset64& asset, size_t count)> func) {
				if (!opt.m_dump_types[type] && !opt.m_dump_all_available) {
					return;
				}

				opt.m_dump_types[type] = false;

				std::filesystem::create_directories(outDir);

				int du = ForEachEntry(proc, pools[type], func);

				if (du < 0) {
					LOG_ERROR("Error reading pool: {}", tool::mw19::PoolName(type));
				}
				else {
					total += du;
				}
			};

			const std::filesystem::path gscDir = outDir / "gsc";
			HandlePool(tool::mw19::IW19_ASSETTYPE_SCRIPTFILE, gscDir, [&opt, &proc, gscDir](const XAsset64& asset, size_t count) -> bool {
				ScriptFile entry{};
				if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
					LOG_ERROR("Can't read scriptfile entry {:x}", asset.Header);
					return false;
				}

				std::vector<byte> dumpFile{};

				const char* fileNameRead = proc.ReadStringTmp(entry.name);

				utils::Allocate(dumpFile, sizeof(compatibility::xensik::gscbin::GscBinHeader));

				compatibility::xensik::gscbin::GscBinHeader& header{ *reinterpret_cast<compatibility::xensik::gscbin::GscBinHeader*>(dumpFile.data()) };

				header.Magic() = compatibility::xensik::gscbin::GSCBIN_MAGIC;
				header.compressedLen = entry.compressedLen;
				header.len = entry.len;
				header.bytecodeLen = entry.bytecodeLen;

				size_t bufferLoc = utils::Allocate(dumpFile, entry.compressedLen);
				size_t byteCodeLoc = utils::Allocate(dumpFile, entry.bytecodeLen);

				if (entry.compressedLen) {
					if (!proc.ReadMemory(&dumpFile[bufferLoc], entry.buffer, entry.compressedLen)) {
						LOG_ERROR("Can't read scriptfile buffer {}", fileNameRead);
						return false;
					}
				}
				if (entry.bytecodeLen) {
					if (!proc.ReadMemory(&dumpFile[byteCodeLoc], entry.bytecode, entry.bytecodeLen)) {
						LOG_ERROR("Can't read scriptfile bytecode buffer {}", fileNameRead);
						return false;
					}
				}

				char* fileName = utils::CloneString(fileNameRead);

				std::string_view nameView{ fileName };

				if (nameView.ends_with(".gsc")) {
					fileName[nameView.length() - 4] = 0;
				}
				fileName = utils::va("%s.gscbin", fileName);

				std::filesystem::path out = gscDir / fileName;
				std::filesystem::create_directories(out.parent_path());

				if (!utils::WriteFile(out, dumpFile.data(), dumpFile.size())) {
					LOG_ERROR("Can't write file {}", out.string());
					return false;
				}
				LOG_INFO("Dumped {} ({})", out.string(), fileNameRead);

				return true;
			});

			LOG_INFO("Dumped {} files", total);

			return tool::OK;
		}
	}

	namespace {
		int csi_test(int argc, const char* argv[]) {
			if (!argv[2]) return tool::BAD_USAGE;

			compatibility::scobalula::csi::CordycepProc cordycep{};

			if (!cordycep.ReadCsi(argv[2])) {
				LOG_ERROR("Can't read Cordycep database: {}", argv[2]);
				return tool::BASIC_ERROR;
			}

			LOG_INFO("Loaded");

			LOG_INFO("Game Id .. {} (0x{:x})", compatibility::scobalula::csi::CordycepGameName(cordycep.gameId), (uint64_t)cordycep.gameId);
			LOG_INFO("Pools .... 0x{:x}", cordycep.poolsAddress);
			LOG_INFO("Strings .. 0x{:x}", cordycep.stringsAddress);
			LOG_INFO("Game dir . '{}'", cordycep.gameDir);

			return tool::OK;
		}

		int dpcord(Process& proc, int argc, const char* argv[]) {
			std::filesystem::path exe{ proc[nullptr].path };
			std::filesystem::path db{ exe.parent_path() / "Data/CurrentHandler.csi" };
			std::string dbBuff{};

			compatibility::scobalula::csi::CordycepProc cordycep{};

			if (!cordycep.ReadCsi(db)) {
				LOG_ERROR("Can't read Cordycep database: {}", db.string());
				return tool::BASIC_ERROR;
			}

			int ret;
			switch (cordycep.gameId) {
			case compatibility::scobalula::csi::CG_MW6:
				ret = jup::dpcordjup(proc, cordycep, argc, argv);
				break;
			case compatibility::scobalula::csi::CG_BO6:
				ret = bo6::dpcordbo6(proc, cordycep, argc, argv);
				break;
			case compatibility::scobalula::csi::CG_MW4:
				ret = mw19::dpcordmw19(proc, cordycep, argc, argv);
				break;
			default:
				LOG_ERROR("Can't find dumper for game {} (0x{:x})", CordycepGameName(cordycep.gameId), (uint64_t)cordycep.gameId);
				return tool::BASIC_ERROR;
			}
			return ret;
		}
	}
	int csidheadtest(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;

		std::vector<byte> data{};

		utils::WriteValue<uint64_t>(data, compatibility::scobalula::csi::CG_GHOSTS);
		utils::WriteValue<uint64_t>(data, compatibility::scobalula::csi::CG_AW);
		utils::WriteValue<uint64_t>(data, compatibility::scobalula::csi::CG_IW);
		utils::WriteValue<uint64_t>(data, compatibility::scobalula::csi::CG_MWR);
		utils::WriteValue<uint64_t>(data, compatibility::scobalula::csi::CG_MW2R);
		utils::WriteValue<uint64_t>(data, compatibility::scobalula::csi::CG_MW4);
		utils::WriteValue<uint64_t>(data, compatibility::scobalula::csi::CG_MW5);
		utils::WriteValue<uint64_t>(data, compatibility::scobalula::csi::CG_MW6);
		utils::WriteValue<uint64_t>(data, compatibility::scobalula::csi::CG_VANGUARD);
		utils::WriteValue<uint64_t>(data, compatibility::scobalula::csi::CG_BO6);

		utils::WriteFile(argv[2], data.data(), data.size());

		return tool::OK;
	}

	ADD_TOOL(csid, "compatibility", "", "Dump csi info", csi_test);
	//ADD_TOOL(csidheadtest, "dev", " [file]", "CSI test", csidheadtest);
	ADD_TOOL(dpcord, "common", "", "Cordycep dump tool", L"Cordycep.CLI.exe", dpcord);
}