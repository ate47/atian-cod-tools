#include <includes.hpp>
#include <games/bo4/pool.hpp>
#include <dbflib.hpp>
#include <rapidcsv.h>
#include "tools/pool.hpp"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <compiler/gsc_compiler.hpp>


namespace {
	using namespace games::bo4;

	struct RawFileEntry {
		tool::pool::XHash name;
		uint64_t size; // 0x10
		char* buffer; // 0x18
	}; static_assert(sizeof(RawFileEntry) == 0x20);


	struct ScriptParseTreeEntry {
		tool::pool::XHash name;
		void* buffer;
		uint32_t size;
		uint32_t pad02;
	}; static_assert(sizeof(ScriptParseTreeEntry) == 0x20);

	union StringTableCellValue {
		byte bytes[0x10];
		const char* string_value;
		int64_t int_value;
		float float_value;
		byte bool_value;
		uint64_t hash_value;
	};

	enum StringTableCellType : byte {
		STC_TYPE_UNDEFINED = 0,
		STC_TYPE_STRING = 1,
		STC_TYPE_HASHED2 = 2,
		STC_TYPE_INT = 4,
		STC_TYPE_FLOAT = 5,
		STC_TYPE_BOOL = 6,
		STC_TYPE_HASHED7 = 7,
		STC_TYPE_HASHED8 = 8,
	};

	struct StringTableCell {
		StringTableCellValue value{};
		uint32_t pad10{};
		StringTableCellType type{};
	};

	struct StringTable {
		tool::pool::XHash name;
		int32_t columns_count{};
		int32_t rows_count{};
		int32_t cells_count{};
		int32_t unk24{};
		uintptr_t cells{};
		StringTableCell* values{};
		uintptr_t unk48{};
		uintptr_t unk56{};
	}; static_assert(sizeof(StringTable) == 0x40);

	struct LuaFileEntry {
		tool::pool::XHash name;
		uint64_t size{};
		byte* buffer{};
	}; static_assert(sizeof(LuaFileEntry) == 0x20);

	struct LocalizeEntry {
		const char* string{ "" };
		tool::pool::XHash name;
	}; static_assert(sizeof(LocalizeEntry) == 0x18);

	union AssetHeader {
		ScriptParseTreeEntry* spt;
		RawFileEntry* rawfile;
		StringTable* stringtable;
		LuaFileEntry* luafile;
		LocalizeEntry* localize;
		void* ptr;
	};

	// xasset
	struct DBFFileAssetEntry {
		pool::XAssetType type;
		uint64_t name;
		AssetHeader header;
	};

	// ref to an xasset inside the struct
	struct DBFFileAssetAssetRef {
		pool::XAssetType type;
		uint64_t name;
		void** location;
	};

	// ref to a string to link
	struct DBFileAssetStringRef {
		const char* str;
		void** location;
	};

	// root entry
	struct DBFFileAssetTable {
		uint64_t entriesCount;
		DBFFileAssetEntry* entries;
		uint64_t refEntriesCount;
		DBFFileAssetAssetRef* refEntries;
		uint64_t stringsEntriesCount;
		DBFileAssetStringRef* stringsEntries;
	};

	uint64_t NamePattern(const std::filesystem::path& path) {
		std::string name = path.string();

		if (name.starts_with("script_") || name.starts_with("hash_") || name.starts_with("file_")) {
			auto ext = path.extension();
			auto ss = name.substr(0, name.length() - ext.string().length());
			return hash::Hash64Pattern(ss.c_str());
		}

		return hash::Hash64(name.c_str());
	}

	pool::XAssetType GetAssetTypeFromPath(const std::filesystem::path& root, const std::filesystem::path& path, uint64_t& nameout, bool& precompiled) {
		std::filesystem::path rel{ std::filesystem::relative(path, root) };

		auto it = rel.begin();

		if (it == rel.end()) {
			return pool::ASSET_TYPE_COUNT; // wtf?
		}

		std::wstring first = *it;
		it++;

		if (first == L"tables") {
			// custom xasset

			if (it == rel.end()) {
				LOG_WARNING("tables/ can't be a file");
				return pool::ASSET_TYPE_COUNT; // no type name?
			}

			std::string typeName = utils::WStrToStr(*it);

			pool::XAssetType type = pool::XAssetIdFromName(typeName.c_str());

			if (type == pool::ASSET_TYPE_COUNT) {
				LOG_WARNING("invalid type {} for file: {}", typeName, path.string());
				return pool::ASSET_TYPE_COUNT; // bad type?
			}

			it++;

			if (it == rel.end()) {
				return pool::ASSET_TYPE_COUNT; // no name?
			}

			std::wstring name = *it;
			it++;

			if (it != rel.end()) {
				LOG_WARNING("invalid {} file: {}", typeName, path.string());
				return pool::ASSET_TYPE_COUNT; // bad name?
			}

			if (!name.ends_with(L".json")) {
				return pool::ASSET_TYPE_COUNT; // bad extension?
			}

			std::wstring nameUn = name.substr(0, name.length() - 5);
			nameout = hash::Hash64Pattern(nameUn.c_str());

			return type;
		}

		if (first == L"scriptbundle") {
			// scriptbundle
			if (it == rel.end()) {
				LOG_WARNING("scriptbundle/ can't be a file");
				return pool::ASSET_TYPE_COUNT; // no name?
			}
			std::wstring name = *it;

			it++;

			if (it != rel.end()) {
				LOG_WARNING("invalid scriptbundle file: {}", path.string());
				return pool::ASSET_TYPE_COUNT; // bad name?
			}

			if (!name.ends_with(L".json")) {
				return pool::ASSET_TYPE_COUNT; // bad extension?
			}

			std::wstring nameUn = name.substr(0, name.length() - 5);
			nameout = hash::Hash64Pattern(nameUn.c_str());

			return pool::ASSET_TYPE_SCRIPTBUNDLE;
		}

		if (first == L"localize.json") {
			if (it != rel.end() || !std::filesystem::is_regular_file(path)) {
				LOG_WARNING("localize.json should be a file");
				return pool::ASSET_TYPE_COUNT; // no name?
			}

			return pool::ASSET_TYPE_LOCALIZE_ENTRY;
		}

		auto ext = path.extension();

		if (ext == ".csv") {
			nameout = NamePattern(path);
			return pool::ASSET_TYPE_STRINGTABLE;
		}

		if (ext == ".ddl") {
			nameout = NamePattern(path);
			return pool::ASSET_TYPE_DDL;
		}

		if (ext == ".lua") {
			nameout = NamePattern(path);
			return pool::ASSET_TYPE_LUAFILE;
		}

		if (ext == ".luac") {
			nameout = NamePattern(path);
			precompiled = true;
			return pool::ASSET_TYPE_LUAFILE;
		}

		if (ext == ".ttf") {
			nameout = NamePattern(path);
			return pool::ASSET_TYPE_TTF;
		}

		if (ext == ".gsc" || ext == ".csc") {
			nameout = NamePattern(path);
			return pool::ASSET_TYPE_SCRIPTPARSETREE;
		}

		if (ext == ".gscc" || ext == ".cscc") {
			nameout = NamePattern(path);
			precompiled = true;
			return pool::ASSET_TYPE_SCRIPTPARSETREE;
		}

		nameout = NamePattern(path);
		return pool::ASSET_TYPE_RAWFILE; // maybe it might not be the best lol
	}

	int dbfgen(Process& proc, int argc, const char* argv[]) {
		if (argc < 4) {
			return tool::BAD_USAGE;
		}

		const char* input{ argv[2] };
		const char* output{ argv[3] };

		// builder
		dbflib::DBFileBuilder builder{ dbflib::DBFBO_ALIGN };

		auto [tableId, tableptr] = builder.CreateBlock<DBFFileAssetTable>();

		std::vector<DBFFileAssetEntry> entries{};
		std::vector<dbflib::BlockId> entriesLoc{};
		std::vector<DBFFileAssetAssetRef> refEntries{};
		std::vector<DBFileAssetStringRef> refStrings{};

		// compile entries


		const std::filesystem::path root{ input };
		std::vector<std::filesystem::path> paths{};
		utils::GetFileRecurse(root, paths);
		bool err{};

		for (const std::filesystem::path& path : paths) {
			uint64_t name;
			bool precompiled{};
			pool::XAssetType type = GetAssetTypeFromPath(root, path, name, precompiled);

			if (type == pool::ASSET_TYPE_COUNT) {
				// bad type, ignore
				continue;
			}

			LOG_INFO("Reading {} with type {} -> hash_{:x}", path.string(), pool::XAssetNameFromId(type), name);

			switch (type) {
			case pool::ASSET_TYPE_RAWFILE: {
				std::string buffer{};

				if (!utils::ReadFile(path, buffer)) {
					LOG_WARNING("Can't read {}", path.string());
					continue;
				}
				auto& e = entries.emplace_back();

				e.name = name;
				e.type = type;
				// allocate data
				dbflib::BlockId rawDataId = builder.CreateBlock(buffer.data(), buffer.length() + 1);

				// allocate header
				auto [rawId, rawptr] = builder.CreateBlock<RawFileEntry>();
				entriesLoc.emplace_back(rawId);
				rawptr->name.name = name;
				rawptr->size = buffer.length();

				// link header -> data
				builder.CreateLink(rawId, offsetof(RawFileEntry, buffer), rawDataId);
			}
				break;
			case pool::ASSET_TYPE_SCRIPTPARSETREE: {
				std::vector<byte> buffer{};

				if (precompiled) {
					if (!utils::ReadFile(path, buffer)) {
						LOG_WARNING("Can't read {}", path.string());
						continue;
					}
				}
				else {
					// compile file
					acts::compiler::CompilerConfig cfg{};
					char namePatt[0x100];
					sprintf_s(namePatt, "hash_%llx", name);

					cfg.name = namePatt;
					cfg.platform = tool::gsc::opcode::PLATFORM_PC;
					cfg.vm = tool::gsc::opcode::VMI_T8;
					cfg.detourType = acts::compiler::DETOUR_ACTS;
					cfg.processorOpt.defines.insert("_DFG_GEN");

					try {
						acts::compiler::CompileGsc(path, buffer, cfg);
					}
					catch (std::runtime_error& re) {
						LOG_ERROR("Can't compile {}: {}", path.string(), re.what());
						continue;
					}
				}

				auto& e = entries.emplace_back();

				e.name = name;
				e.type = type;

				// allocate data
				dbflib::BlockId rawDataId = builder.CreateBlock(buffer.data(), buffer.size());

				// allocate header
				auto [rawId, rawptr] = builder.CreateBlock<ScriptParseTreeEntry>();
				entriesLoc.emplace_back(rawId);
				rawptr->name.name = name;
				rawptr->size = (uint32_t)buffer.size();

				// link header -> data
				builder.CreateLink(rawId, offsetof(ScriptParseTreeEntry, buffer), rawDataId);
			}
				break;
			case pool::ASSET_TYPE_LUAFILE: {
				if (!precompiled) {
					LOG_WARNING("No LUA compiler available for file {}", path.string());
					continue;
				}

				std::string buffer{};

				if (!utils::ReadFile(path, buffer)) {
					LOG_WARNING("Can't read {}", path.string());
					continue;
				}

				auto& e = entries.emplace_back();

				e.name = name;
				e.type = type;

				// allocate data
				dbflib::BlockId rawDataId = builder.CreateBlock(buffer.data(), buffer.length());

				// allocate header
				auto [rawId, rawptr] = builder.CreateBlock<LuaFileEntry>();
				entriesLoc.emplace_back(rawId);
				rawptr->name.name = name;
				rawptr->size = (uint32_t)buffer.length();

				// link header -> data
				builder.CreateLink(rawId, offsetof(LuaFileEntry, buffer), rawDataId);
			}
				break;
			case pool::ASSET_TYPE_STRINGTABLE: {
				std::string buffer{};

				if (!utils::ReadFile(path, buffer)) {
					LOG_WARNING("Can't read {}", path.string());
					continue;
				}

				rapidcsv::Document doc{};

				std::stringstream stream{ buffer };

				doc.Load(stream, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(','));

				std::vector<StringTableCellType> cellTypes{};
				for (size_t i = 0; i < doc.GetColumnCount(); i++) {
					// read cell types
					const std::string cell = doc.GetCell<std::string>(i, 0);

					StringTableCellType ctype = STC_TYPE_STRING;
					if (cell == "undefined") {
						ctype = STC_TYPE_UNDEFINED;
					}
					else if (cell == "string") {
						ctype = STC_TYPE_STRING;
					}
					else if (cell == "int") {
						ctype = STC_TYPE_INT;
					}
					else if (cell == "float") {
						ctype = STC_TYPE_FLOAT;
					}
					else if (cell == "hash" || cell == "hash2") {
						ctype = STC_TYPE_HASHED2;
					}
					else if (cell == "hash7") {
						ctype = STC_TYPE_HASHED7;
					}
					else if (cell == "hash8") {
						ctype = STC_TYPE_HASHED8;
					}
					else if (cell == "bool") {
						ctype = STC_TYPE_BOOL;
					}
					else {
						LOG_ERROR("Can't read {} StringTable type of column {} : '{}'", path.string(), i, cell);
						err = true;
						break;
					}

					cellTypes.emplace_back(ctype);
				}
				if (err) {
					continue;
				}

				auto& e = entries.emplace_back();

				e.name = name;
				e.type = type;
				auto [stId, stptr] = builder.CreateBlock<StringTable>();

				entriesLoc.emplace_back(stId);
				stptr->name.name = name;
				size_t rc = doc.GetRowCount();
				int32_t rows = (stptr->rows_count = (int32_t)(rc ? (rc - 1) : 0));
				int32_t columns = (stptr->columns_count = (int32_t)doc.GetColumnCount());

				if (rows && columns) {
					// at least one cell
					auto [cellsId, cellsptr] = builder.CreateBlock<StringTableCell>(sizeof(StringTableCell) * rows * columns);
					// link cells
					builder.CreateLink(stId, offsetof(StringTable, values), cellsId);

					size_t idx{};
					// read cells
					for (size_t row = 1; row <= rows; row++) {
						for (size_t column = 0; column < columns; column++) {
							StringTableCellType cellType = cellTypes[column];

							const std::string cellVal = doc.GetCell<std::string>(column, row);

							StringTableCell& cell = builder.GetBlock<StringTableCell>(cellsId)[idx];
							cell.type = cellType;

							try {
								switch (cellType) {
								case STC_TYPE_UNDEFINED:
									cell.value.int_value = 0;
									break;
								case STC_TYPE_BOOL:
									cell.value.bool_value = cellVal == "true" || cellVal == "TRUE";
									break;
								case STC_TYPE_HASHED2:
								case STC_TYPE_HASHED7:
								case STC_TYPE_HASHED8:
									if (cellVal.empty()) {
										cell.value.hash_value = 0;
									}
									else {
										cell.value.hash_value = hash::Hash64Pattern(cellVal.c_str());
									}
									break;
								case STC_TYPE_INT:
									cell.value.int_value = utils::ParseFormatInt(cellVal.c_str());
									break;
								case STC_TYPE_FLOAT:
									cell.value.float_value = std::stof(cellVal);
									break;
								case STC_TYPE_STRING: {
									dbflib::BlockId allocatedString = builder.CreateBlock((void*)cellVal.data(), cellVal.size() + 1);
									builder.CreateLink(cellsId, (dbflib::BlockOffset)(sizeof(StringTableCell) * idx + offsetof(StringTableCell, value)), allocatedString);
									break;
								}
								}
							}
							catch (const std::invalid_argument& e) {
								LOG_WARNING("Can't read StringTable {} : {} [line {} col {}] '{}'", path.string(), e.what(), row, column, cellVal);
								err = true;
								break;
							}
							idx++;
						}
					}

				}

				break;
			}
			case pool::ASSET_TYPE_LOCALIZE_ENTRY: {
				// unlike the other files, the localize file is a json file with multiple assets
				std::string buffer{};

				if (!utils::ReadFile(path, buffer)) {
					LOG_WARNING("Can't read {}", path.string());
					continue;
				}

				rapidjson::Document doc{};
				doc.Parse(buffer.c_str());
				if (doc.HasParseError()) {
					LOG_ERROR("Invalid json for file {}", path.string());
					err = true;
					continue;
				}

				for (const auto& [k, v] : doc.GetObj()) {
					const char* ks = k.GetString();

					if (!v.IsString()) {
						LOG_ERROR("Invalid json for file {}, the key {} isn't a string", path.string(), ks);
						err = true;
					}

					const char* vs = v.GetString();

					auto& e = entries.emplace_back();

					e.name = hash::Hash64Pattern(ks);
					e.type = type;

					// allocate data
					dbflib::BlockId strVal = builder.CreateBlock((void*)vs, strlen(vs) + 1);

					// allocate header
					auto [rawId, rawptr] = builder.CreateBlock<LocalizeEntry>();
					entriesLoc.emplace_back(rawId);
					rawptr->name.name = name;

					// link header -> data
					builder.CreateLink(rawId, offsetof(LocalizeEntry, string), strVal);
				}

				break;
			}
			default:
				LOG_WARNING("Type {} isn't implemented!", pool::XAssetNameFromId(type));
				continue;
			}

		}
		if (err) {
			return tool::BASIC_ERROR;
		}

		// write entries
		if (entries.size()) {
			dbflib::BlockId entriesId = builder.CreateBlock(entries.data(), sizeof(entries[0]) * entries.size());

			for (size_t i = 0; i < entries.size(); i++) {
				builder.CreateLink(entriesId, (dbflib::BlockOffset)(sizeof(entries[0]) * i + offsetof(DBFFileAssetEntry, header)), entriesLoc[i]);
			}
			builder.CreateLink(tableId, offsetof(DBFFileAssetTable, entries), entriesId);
		}

		// write ref entries
		if (refEntries.size()) {
			dbflib::BlockId refEntriesId = builder.CreateBlock(refEntries.data(), sizeof(refEntries[0]) * refEntries.size());

			for (size_t i = 0; i < refEntries.size(); i++) {
				builder.CreateLink(refEntriesId, (dbflib::BlockOffset)(sizeof(refEntries[0]) * i + offsetof(DBFFileAssetAssetRef, location)), entriesLoc[i]);
			}
			builder.CreateLink(tableId, offsetof(DBFFileAssetTable, refEntries), refEntriesId);
		}

		if (refStrings.size()) {
			dbflib::BlockId refStringsId = builder.CreateBlock(refStrings.data(), sizeof(refStrings[0]) * refStrings.size());

			for (size_t i = 0; i < refStrings.size(); i++) {
				builder.CreateLink(refStringsId, (dbflib::BlockOffset)(sizeof(refStrings[0]) * i + offsetof(DBFileAssetStringRef, str)), (dbflib::BlockId)refStrings[i].str);
				builder.CreateLink(refStringsId, (dbflib::BlockOffset)(sizeof(refStrings[0]) * i + offsetof(DBFileAssetStringRef, location)), (dbflib::BlockId)refStrings[i].location);
			}
			builder.CreateLink(tableId, offsetof(DBFFileAssetTable, stringsEntries), refStringsId);
		}

		DBFFileAssetTable* table = builder.GetBlock<DBFFileAssetTable>(tableId);
		table->entriesCount = entries.size();
		table->refEntriesCount = refEntries.size();
		table->stringsEntriesCount = refStrings.size();


		builder.WriteToFile(output);
		LOG_INFO("Build into {}", output);

		return tool::OK;
	}

	int dbfread(Process& proc, int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) {
			return tool::BAD_USAGE;
		}

		dbflib::DBFileReader reader{ argv[2] };
		std::filesystem::path outDir{ argv[3] };

		std::filesystem::create_directories(outDir);
		DBFFileAssetTable* table = reader.GetStart<DBFFileAssetTable>();
		
		

		LOG_INFO("Entries: {}", table->entriesCount);
		for (size_t i = 0; i < table->entriesCount; i++) {
			auto& e = table->entries[i];
			switch (e.type) {
			case pool::ASSET_TYPE_LOCALIZE_ENTRY:
				LOG_INFO("- {:x} ({}) -> '{}'", e.name, pool::XAssetNameFromId(e.type), e.header.localize->string);
				break;
			case pool::ASSET_TYPE_SCRIPTPARSETREE: {
				std::filesystem::path outGscc{ outDir / std::format("script_{:x}.gscc", e.header.spt->name.name) };
				utils::WriteFile(outGscc, e.header.spt->buffer, e.header.spt->size);
				LOG_INFO("- {:x} ({}) -> {}", e.name, pool::XAssetNameFromId(e.type), outGscc.string());
				break;
			}
			default:
				LOG_INFO("- {:x} ({}) -> {}", e.name, pool::XAssetNameFromId(e.type), e.header.ptr);
				break;
			}
		}

		LOG_INFO("RefEntries: {}", table->refEntriesCount);
		for (size_t i = 0; i < table->refEntriesCount; i++) {
			auto& e = table->refEntries[i];
			LOG_INFO("- {:x} ({}) -> {}", e.name, pool::XAssetNameFromId(e.type), (void*)e.location);
		}

		return tool::OK;
	}

	ADD_TOOL(dbfgen, "compatibility", " (in) (out)", "Gen dynamic file", nullptr, dbfgen);
	ADD_TOOL(dbfread, "compatibility", " (out)", "Read dynamic file", nullptr, dbfread);
}