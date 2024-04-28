#include <includes.hpp>
#include <pool.hpp>
#include <dbflib.hpp>
#include "tools/pool.hpp"
namespace {

	struct RawFileEntry {
		tool::pool::XHash name;
		uint64_t size; // 0x10
		char* buffer; // 0x18
	}; static_assert(sizeof(RawFileEntry) == 0x20);


	// xasset
	struct DBFFileAssetEntry {
		pool::XAssetType type;
		uint64_t name;
		void* header;

		template<typename Type>
		Type* Header() {
			return reinterpret_cast<Type*>(header);
		}
	};

	// ref to an xasset inside the struct
	struct DBFFileAssetAssetRef {
		pool::XAssetType type;
		uint64_t name;
		void** location;
	};

	// root entry
	struct DBFFileAssetTable {
		uint64_t entriesCount;
		DBFFileAssetEntry* entries;
		uint64_t refEntriesCount;
		DBFFileAssetAssetRef* refEntries;
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

	pool::XAssetType GetAssetTypeFromPath(const std::filesystem::path& root, const std::filesystem::path& path, uint64_t& nameout) {
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

		if (ext == ".ttf") {
			nameout = NamePattern(path);
			return pool::ASSET_TYPE_TTF;
		}

		if (ext == ".gsc" || ext == ".csc") {
			nameout = NamePattern(path);
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

		// compile entries


		const std::filesystem::path root{ input };
		std::vector<std::filesystem::path> paths{};
		utils::GetFileRecurse(root, paths);

		for (const std::filesystem::path& path : paths) {

			uint64_t name;
			pool::XAssetType type = GetAssetTypeFromPath(root, path, name);

			if (type == pool::ASSET_TYPE_COUNT) {
				// bad type, ignore
				continue;
			}

			LOG_INFO("Reading {} with type {} -> hash_{:x}", path.string(), pool::XAssetNameFromId(type), name);

			switch (type) {
			case pool::ASSET_TYPE_RAWFILE:
			{
				auto& e = entries.emplace_back();

				e.name = name;
				e.type = type;

				std::string buffer{};

				if (!utils::ReadFile(path, buffer)) {
					LOG_WARNING("Can't read {}", path.string());
					continue;
				}
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

			default:
				LOG_WARNING("Type {} isn't implemented!", pool::XAssetNameFromId(type));
				continue;
			}

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

		DBFFileAssetTable* table = builder.GetBlock<DBFFileAssetTable>(tableId);
		table->entriesCount = entries.size();
		table->refEntriesCount = refEntries.size();


		builder.WriteToFile(output);
		LOG_INFO("Build into {}", output);

		return tool::OK;
	}
	int dbfread(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		dbflib::DBFileReader reader{ argv[2] };

		DBFFileAssetTable* table = reader.GetStart<DBFFileAssetTable>();

		LOG_INFO("Entries: {}", table->entriesCount);
		for (size_t i = 0; i < table->entriesCount; i++) {
			auto& e = table->entries[i];
			LOG_INFO("- {:x} ({}) -> {}", e.name, pool::XAssetNameFromId(e.type), (void*)e.header);
		}

		LOG_INFO("RefEntries: {}", table->refEntriesCount);
		for (size_t i = 0; i < table->refEntriesCount; i++) {
			auto& e = table->refEntries[i];
			LOG_INFO("- {:x} ({}) -> {}", e.name, pool::XAssetNameFromId(e.type), (void*)e.location);
		}

		return tool::OK;
	}

	ADD_TOOL("dbfgen", " (in) (out)", "Gen dynamic file", nullptr, dbfgen);
	ADD_TOOL("dbfread", " (out)", "Read dynamic file", nullptr, dbfread);
}