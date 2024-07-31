#include <includes.hpp>
#include "sp23.hpp"
#include "tools/hashes/compiled_files.hpp"

namespace sp23::dump {

	struct CordycepProc {
		uint64_t gameId;
		uintptr_t poolsAddress;
		uintptr_t stringsAddress;
		std::string gameDir;

		bool ReadCsi(const std::filesystem::path& path) {
			std::string dbBuff{};

			if (!utils::ReadFile(path, dbBuff)) {
				LOG_ERROR("Can't read Cordycep database: {}", path.string());
				return false;
			}

			struct CordycepBase {
				uint64_t gameId;
				uintptr_t poolsAddress;
				uintptr_t stringsAddress;
				uint32_t stringSize;
				byte gameDir[4];
			};

			LOG_DEBUG("Loading Cordycep csi...");
			CordycepBase* header{ (CordycepBase*)dbBuff.data() };

			const char* buffer{ dbBuff.data() };
			size_t idx{ offsetof(CordycepBase, gameDir) };

			gameDir.resize(header->stringSize);
			memcpy(gameDir.data(), buffer + idx, header->stringSize);
			idx += header->stringSize;


			LOG_DEBUG("Game Id .. 0x{:x}", header->gameId);
			LOG_DEBUG("Pools .... 0x{:x}", header->poolsAddress);
			LOG_DEBUG("Strings .. 0x{:x}", header->stringsAddress);
			LOG_DEBUG("Game dir . '{}'", gameDir);

			gameId = header->gameId;
			poolsAddress = header->poolsAddress;
			stringsAddress = header->stringsAddress;

			return true;
		}
	};

	namespace {
		int csi_test(Process& proc, int argc, const char* argv[]) {
			if (!argv[2]) return tool::BAD_USAGE;

			CordycepProc cordycep{};

			if (!cordycep.ReadCsi(argv[2])) {
				LOG_ERROR("Can't read Cordycep database: {}", argv[2]);
				return tool::BASIC_ERROR;
			}

			LOG_INFO("Loaded");

			return tool::OK;
		}
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
			uint64_t unk0;
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

		bool WriteBundleDef(std::ostringstream& os, Process& proc, ScriptBundle& entry, ScriptBundleObjectDef& def, int depth) {
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
				os << "\"&" << proc.ReadStringTmp(entry.rawdata + def.value.id) << "\"";
				break;
			case SBT_STRING:
				os << "\"" << proc.ReadStringTmp(entry.rawdata + def.value.id) << "\"";
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
						if (!WriteBundleDef(os, proc, entry, val.def, depth + 1)) return false;
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
						if (!WriteBundleDef(os, proc, entry, *def, depth + 1)) return false;
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
						if (!WriteBundleDef(os, proc, entry, defs[i], depth + 1)) return false;
					}
				}

				utils::Padding(os << "\n", depth) << "]";
				break;
			}
			case SBT_STRUCT_ANIMATION: {
				os << "{";
				ScriptBundleObjectDefValueAnim& anim = def.value.anim;
				const char* name = proc.ReadStringTmp(entry.rawdata + anim.name_offset);
				hashutils::Add(name, true, true);
				utils::Padding(os << "\n", depth + 1) << "\"#name\": \"" << name << "\",";
				utils::Padding(os << "\n", depth + 1) << "\"#id\": \"anim#" << hashutils::ExtractTmp("hash", anim.anim) << "\"";
				utils::Padding(os << "\n", depth) << "}";
				break;
			}
			case SBT_STRUCT_NAMEID: {
				os << "{";
				ScriptBundleObjectDefValueAnim& anim = def.value.anim;
				const char* name = proc.ReadStringTmp(entry.rawdata + anim.name_offset);
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

		int ForEachEntry(Process& proc, XAssetPool64& pool, std::function<bool(uintptr_t asset)> func) {
			uintptr_t curr = pool.Root;

			XAsset64 asset{};
			int res{};
			bool ok{ true };
			while (curr) {
				if (!proc.ReadMemory(&asset, curr, sizeof(asset))) {
					LOG_ERROR("Can't read pool entry {:x}", curr);
					return false;
				}

				if (asset.Header) {
					if (!func(asset.Header)) {
						ok = false;
					}
					res++;
				}

				curr = asset.Next;
			}

			return ok ? res : -res;
		}

		int dpjup(Process& proc, int argc, const char* argv[]) {
			std::filesystem::path exe{ proc[nullptr].path };
			std::filesystem::path db{ exe.parent_path() / "Data/CurrentHandler.csi" };
			std::string dbBuff{};

			CordycepProc cordycep{};

			if (!cordycep.ReadCsi(db)) {
				LOG_ERROR("Can't read Cordycep database: {}", db.string());
				return tool::BASIC_ERROR;
			}

			auto[pools, ok] = proc.ReadMemoryArray<XAssetPool64>(cordycep.poolsAddress, sp23::ASSET_COUNT);

			if (!ok) {
				LOG_ERROR("Can't read pools");
				return tool::BASIC_ERROR;
			}

			hashutils::ReadDefaultFile();

			std::filesystem::path outDir = "output_jup";

			size_t total{};
			for (size_t i = 2; i < argc; i++) {
				const char* pn = argv[i];

				AssetType type = sp23::AssetTypeFromName(pn);

				if (type == ASSET_COUNT) {
					LOG_ERROR("Invalid asset type name: {}", pn);
					return tool::BAD_USAGE;
				}

				std::function<bool(uintptr_t asset)> func{};

				switch (type) {
				case ASSET_GSCOBJ: {
					struct GscObjEntry {
						uint64_t name;
						int len;
						int padc;
						uintptr_t buffer;
					};

					std::filesystem::path gscDir = outDir / "gsc";
					std::filesystem::create_directories(gscDir);

					func = [&proc, gscDir](uintptr_t asset) -> bool {
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
						};
					break;
				}
				case ASSET_GSCGDB: {
					struct GscObjEntry {
						uint64_t name;
						int len;
						int padc;
						uintptr_t buffer;
					};

					std::filesystem::path gscDir = outDir / "gscgdb";
					std::filesystem::create_directories(gscDir);

					func = [&proc, gscDir](uintptr_t asset) -> bool {
						GscObjEntry entry{};
						if (!proc.ReadMemory(&entry, asset, sizeof(entry))) {
							LOG_ERROR("Can't read gscgdb {:x}", asset);
							return false;
						}

						if (entry.len <= 0) {
							LOG_INFO("Invalid size for {} ({})", hashutils::ExtractTmpScript(entry.name), entry.len);
							return false;
						}

						std::filesystem::path out = gscDir / utils::va("script_%llx.gdbc", entry.name);
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
						};
					break;
				}
				case ASSET_STRINGTABLE: {
					enum StringTableCellType : uint8_t {
						STT_UNK_1_64 = 1, // string
						STT_UNK_9_64 = 9, // string2??
						STT_UNK_A_64 = 0xA, // string3??

						STT_UNK_2_64 = 2, // int??
						STT_UNK_5_64 = 5, // hash (0xCBF29CE484222325/0x100000001B3)
						STT_UNK_6_64 = 6, // hash (0x47F5817A5EF961BA/0x100000001B3)
						STT_UNK_7_64 = 7, // ?

						STT_UNK_3_32 = 3, // float??
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


					std::filesystem::path dumpDir = outDir / "dump";
					std::filesystem::create_directories(dumpDir);

					func = [&proc, dumpDir](uintptr_t asset) -> bool {
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
									case STT_UNK_5_64:
									case STT_UNK_6_64:
									case STT_UNK_7_64:
										elemSize = 8;
										break;
									case STT_UNK_3_32:
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
										os << proc.ReadStringTmp(*reinterpret_cast<uintptr_t*>(value));
										break;
									case STT_UNK_2_64:
										// int?
										os << std::dec << *reinterpret_cast<int64_t*>(value);
										break;
									case STT_UNK_5_64:
									case STT_UNK_6_64:
										os << "#hash_" << std::hex << *reinterpret_cast<uint64_t*>(value);
										break;
									case STT_UNK_7_64:
										os << "7?" << std::hex << *reinterpret_cast<uint64_t*>(value);
										break;
									case STT_UNK_3_32:
										os << *reinterpret_cast<float*>(value);
										break;
									case STT_UNK_8_32:
										os << "8?" << std::hex << *reinterpret_cast<uint32_t*>(value);
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
					};

				}
					break;
				case ASSET_SCRIPTBUNDLE: {
					std::filesystem::path dumpDir = outDir / "dump";
					std::filesystem::create_directories(dumpDir);

					func = [&proc, dumpDir](uintptr_t asset) -> bool {
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
								WriteBundleDef(os, proc, entry, obj.def, 1);
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
					};
					break;
				}
				default:
					LOG_ERROR("Handler not implemented for type {}", pn);
					continue;
				}
				int du = ForEachEntry(proc, pools[type], func);

				if (du < 0) {
					LOG_ERROR("Error reading pool: {}", pn);
				}
				else {
					total += du;
				}
			}

			LOG_INFO("Dumped {} file(s)", total);

			return tool::OK;
		}
	}
#ifndef CI_BUILD
	ADD_TOOL("csi_test", "dev", " [file]", "dump pool", nullptr, csi_test);
#endif
	ADD_TOOL("dpjup", "mwiii", " [pool]", "dump pool", L"Cordycep.CLI.exe", dpjup);
}