#include <includes.hpp>
#include <dbflib.hpp>
namespace {
	struct DBFFileAssetEntry {
		uint8_t type;
		uint64_t name;
		void* header;
	};

	struct DBFFileAssetTable {
		uint64_t count;
		DBFFileAssetEntry* entries;
	};

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

		// compile entries




		// write entries
		if (entries.size()) {
			dbflib::BlockId entriesId = builder.CreateBlock(entries.data(), sizeof(entries[0]) * entries.size());

			for (size_t i = 0; i < entries.size(); i++) {
				builder.CreateLink(entriesId, (dbflib::BlockOffset)(sizeof(entries[0]) * i + offsetof(DBFFileAssetEntry, header)), entriesLoc[i]);
			}
			builder.CreateLink(tableId, offsetof(DBFFileAssetTable, entries), entriesId);
		}

		DBFFileAssetTable* table = builder.GetBlock<DBFFileAssetTable>(tableId);
		table->count = entries.size();


		builder.WriteToFile(output);
		LOG_INFO("Build into {}", output);

		return tool::OK;
	}

	ADD_TOOL("dbfgen", " (out)", "Gen dynamic file", nullptr, dbfgen);
}