#include <includes.hpp>
#include "compatibility/scobalula_csi.hpp"
#include "tools/sp23/sp23.hpp"
#include "tools/hashes/compiled_files.hpp"

namespace tool::cordycep::dump {
	using namespace sp23;

	std::ostream& PrintFormattedString(std::ostream& out, const char* str) {
		if (!str) {
			return out << "nullptr";
		}
		for (; *str; str++) {
			switch (*str) {
			case '\n':
				out << "\\n";
				break;
			case '\r':
				out << "\\r";
				break;
			case '\t':
				out << "\\t";
				break;
			case '\a':
				out << "\\a";
				break;
			case '\b':
				out << "\\b";
				break;
			case '\v':
				out << "\\v";
				break;
			case '"':
				out << "\\\"";
				break;
			default:
				if (*str < 0x20 || *str >= 0x7F) {
					out << "\\" << std::oct << (unsigned int)(*reinterpret_cast<const byte*>(str)) << std::dec;
				}
				else {
					out << *str;
				}
				break;
			}
		}
		return out;
	}

	namespace jup {
		class PoolOptionJup {
		public:
			bool m_help = false;
			bool m_any_type = false;
			bool m_dump_info = false;
			bool m_dump_all_available = false;
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
							try {
								assetType = (sp23::AssetType)std::strtol(arg, nullptr, 10);
							}
							catch (const std::invalid_argument& e) {
								std::cerr << e.what() << "\n";
								assetType = sp23::ASSET_COUNT;
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
			}

			const char* AddString(const char* str) {
				if (m_dump_strings) {
					dstrings.insert(str);
				}
				return str;
			}
		};
		struct XAsset64 {
			uintptr_t Header;
			uint64_t Temp;
			uintptr_t Next;
			uintptr_t Previous;
		};

		struct XAssetPool64 {
			uintptr_t Root;
			uintptr_t End;
			uint64_t LookupTable;
			uint64_t HeaderMemory;
			uint64_t AssetMemory;
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

		struct LocalizeEntry {
			uint64_t hash;
			uintptr_t value;
		};
		struct LuaFileEntry {
			uintptr_t name;
			uint32_t len;
			uintptr_t buffer;
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
						utils::Padding(os << "\n", depth + 1) << "\"#" << hashutils::ExtractTmp("hash", val.name1) << "////" << hashutils::ExtractTmp("hash", val.name2) << "\": ";
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

		int ForEachEntry(Process& proc, XAssetPool64& pool, std::function<bool(uintptr_t asset, size_t count)> func) {
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
					if (!func(asset.Header, count++)) {
						ok = false;
					}
					res++;
				}

				curr = asset.Next;
			}

			return ok ? res : -res;
		}

		int dpcordjup(Process& proc, compatibility::scobalula::csi::CordycepProc& cordycep, int argc, const char* argv[]) {
			PoolOptionJup opt{};

			if (!opt.Compute(argv, 2, argc) || opt.m_help || !opt.m_any_type) {
				opt.PrintHelp();
				return tool::OK;
			}


			auto [pools, ok] = proc.ReadMemoryArray<XAssetPool64>(cordycep.poolsAddress, sp23::ASSET_COUNT);

			if (!ok) {
				LOG_ERROR("Can't read pools");
				return tool::BASIC_ERROR;
			}

			hashutils::ReadDefaultFile();

			std::filesystem::path outDir = opt.m_output;

			size_t total{};

			auto HandlePool = [&opt, &pools, &proc, &total](sp23::AssetType type, const std::filesystem::path& outDir, std::function<bool(uintptr_t asset, size_t count)> func) {
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
			HandlePool(ASSET_GSCOBJ, gscDir, [&proc, gscDir](uintptr_t asset, size_t count) -> bool {
				GscObjEntry entry{};
				if (!proc.ReadMemory(&entry, asset, sizeof(entry))) {
					LOG_ERROR("Can't read gscobj {:x}", asset);
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
			HandlePool(ASSET_GSCGDB, gdbDir, [&proc, gdbDir](uintptr_t asset, size_t count) -> bool {
				GscObjEntry entry{};
				if (!proc.ReadMemory(&entry, asset, sizeof(entry))) {
					LOG_ERROR("Can't read gscgdb {:x}", asset);
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
			HandlePool(ASSET_LUAFILE, luaDir, [&proc, luaDir](uintptr_t asset, size_t count) -> bool {
				LuaFileEntry entry{};
				if (!proc.ReadMemory(&entry, asset, sizeof(entry))) {
					LOG_ERROR("Can't read luafile {:x}", asset);
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
			HandlePool(ASSET_STRINGTABLE, dumpDir, [&opt, &proc, dumpDir](uintptr_t asset, size_t count) -> bool {
				StringTable entry{};
				if (!proc.ReadMemory(&entry, asset, sizeof(entry))) {
					LOG_ERROR("Can't read StringTable {:x}", asset);
					return false;
				}

				std::filesystem::path out = dumpDir / std::format("stringtable_{:x}.cf", entry.name);

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

				return true;
				});

			HandlePool(ASSET_SCRIPTBUNDLE, dumpDir, [&opt, &proc, dumpDir](uintptr_t asset, size_t count) -> bool {
				ScriptBundle entry{};
				if (!proc.ReadMemory(&entry, asset, sizeof(entry))) {
					LOG_ERROR("Can't read ScriptBundle {:x}", asset);
					return false;
				}

				std::filesystem::path out = dumpDir / std::format("scriptbundle_{:x}.cf", entry.name);

				LOG_INFO("Dump {} -> {}", hashutils::ExtractTmp("hash", entry.name), out.string());
				std::ostringstream os{};

				os << "{";

				if (entry.count) {
					auto [objects, ok] = proc.ReadMemoryArray<ScriptBundleObject>(entry.objects, entry.count);

					if (!ok) {
						LOG_ERROR("Can't read objects");
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

				return true;
			});

			// LocalizeEntry
			if (opt.m_dump_types[ASSET_LOCALIZE] || opt.m_dump_all_available) {
				opt.m_dump_types[ASSET_LOCALIZE] = false;

				std::ostringstream os{};

				os << "{";

				int du = ForEachEntry(proc, pools[ASSET_LOCALIZE], [&proc, &os](uintptr_t asset, size_t count) {
					LocalizeEntry entry{};
					if (!proc.ReadMemory(&entry, asset, sizeof(entry))) {
						LOG_ERROR("Can't read LocalizeEntry {:x}", asset);
						return false;
					}

					if (count) os << ",";

					utils::Padding(os << "\n", 1) << "\"#" << hashutils::ExtractTmp("hash", entry.hash) << "\": \"";
					PrintFormattedString(os, proc.ReadStringTmp(entry.value)) << "\"";

					return true;
				});

				os << "\n}";

				if (du < 0) {
					LOG_ERROR("Error reading pool: {}", sp23::AssetTypeName(ASSET_LOCALIZE));
				}
				else {
					total += du;

					std::filesystem::create_directories(outDir);
					std::filesystem::path loc{ outDir / "localize.cf" };
					std::ofstream osf{ loc, std::ios::binary};
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
					else {
						LOG_ERROR("Can't open {}", loc.string());
					}

				}

			}

			for (size_t i = 0; i < opt.m_dump_types.size(); i++) {
				if (!opt.m_dump_types[i]) {
					continue;
				}
				const char* pn = sp23::AssetTypeName((sp23::AssetType)i);

				std::filesystem::path rawDir = outDir / "raw" / pn;

				// TODO: hex dump
				LOG_ERROR("Can't parse type {}", pn);
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

	namespace {
		int csi_test(Process& proc, int argc, const char* argv[]) {
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
			default:
				LOG_ERROR("Can't find dumper for game {} (0x{:x})", CordycepGameName(cordycep.gameId), (uint64_t)cordycep.gameId);
				return tool::BASIC_ERROR;
			}
			return ret;
		}
	}
	ADD_TOOL("csid", "compatibility", "", "Dump csi info", nullptr, csi_test);
	ADD_TOOL("dpcord", "common", "", "Cordycep dump tool", L"Cordycep.CLI.exe", dpcord);
}