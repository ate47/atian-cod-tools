#include <dll_includes.hpp>
#include <hash.hpp>


enum StringTableCellType : INT {
	STC_TYPE_UNDEFINED = 0,
	STC_TYPE_STRING = 1,
	STC_TYPE_HASHED = 2,
	STC_TYPE_INT = 4,
	STC_TYPE_FLOAT = 5,
	STC_TYPE_BOOL = 6
};

struct StringTableCell {
	byte value[20] = { 0 };
	StringTableCellType type = STC_TYPE_UNDEFINED;
};

struct StringTableEntry {
	uint64_t name;
	int pad8 = 0;
	int pad12 = 0;
	int columnCount = 0;
	int rowCount = 0;
	int cellscount = 0;
	int unk24 = 0;
	uintptr_t cells = 0;
	StringTableCell* values = NULL;
	uintptr_t unk48 = 0;
	uintptr_t unk56 = 0;
	std::vector<StringTableCell> allocatedValues{};
	std::vector<std::shared_ptr<std::string>> strings{};
	std::vector<StringTableCellType> types{};
};

std::vector<StringTableEntry> customStringTableEntries{};
size_t customStringTableEntriesCount = 0;
std::mutex customStringTableEntriesMutex{};


void stringtables::SyncTables() {
	auto parentDir = std::filesystem::path{ inject::g_cliData.workDir } / "data";
	auto cstFile = parentDir / "stringtables.csv";

	LOG_INFO("Syncing custom stringtable '{}'", cstFile.string());

	{
		// clear old refs
		std::lock_guard<std::mutex> lg(customStringTableEntriesMutex);

		customStringTableEntriesCount = 0;
		customStringTableEntries.clear();
	}

	std::ifstream s{ cstFile };

	if (!s) {
		LOG_WARNING("Can't load custom string tables file {}", cstFile.string());
		return;
	}

	std::string line{};

	if (!(s.good() && std::getline(s, line))) {
		LOG_WARNING("Can't read stringtables header");

		s.close();
		return;
	}

	while (s.good() && std::getline(s, line)) {
		if (line.empty()) {
			continue;
		}

		auto shift = line.find(',');

		if (shift == std::string::npos) {
			LOG_WARNING("Bad custom string tables line: '{}'", line);
			continue;
		}

		auto lv = std::string_view(line);

		auto local = lv.substr(0, shift);
		auto replaced = hash::Hash64Pattern(lv.substr(shift + 1).data());

		// reading file

		auto csvFile = parentDir / local;

		LOG_INFO("Loading csv {}->{:x}", csvFile.string(), replaced);

		std::ifstream csv{ csvFile };
		if (!csv) {
			LOG_ERROR("Can't open custom csv file {}", csvFile.string());
			continue;
		}

		std::string line2{};

		if (!(csv.good() && std::getline(csv, line2))) {
			LOG_ERROR("Can't read {} header", csvFile.string());

			csv.close();
			continue;
		}

		StringTableEntry* ePtr;
		{
			std::lock_guard<std::mutex> lg(customStringTableEntriesMutex);
			ePtr = &customStringTableEntries.emplace_back(replaced);
		}
		StringTableEntry& e = *ePtr;

		bool tableError = false;

		auto hlv2 = std::string_view(line2);

		size_t loc = 0;

		// fetch the types in the header
		while (loc < line2.size()) {
			auto idx = line2.find(',', loc);

			if (idx == std::string::npos) {
				idx = line2.size();
			}

			auto type = hlv2.substr(loc, idx - loc);

			StringTableCellType ht;

			
			if (type == "int") {
				ht = STC_TYPE_INT;
			}
			else if (type == "float") {
				ht = STC_TYPE_FLOAT;
			}
			else if (type == "hash") {
				ht = STC_TYPE_HASHED;
			}
			else if (type == "bool") {
				ht = STC_TYPE_BOOL;
			}
			else if (type == "undefined") {
				ht = STC_TYPE_UNDEFINED;
			}
			else {
				if (type != "string") {
					LOG_WARNING("Undefined CSV header type: {}, using string", type);
				}
				ht = STC_TYPE_STRING;
			}

			e.types.push_back(ht);

			loc = idx + 1;
		}

		// we know the number of column
		e.columnCount = (int)e.types.size();
		e.rowCount = 0;

		// compute the rows
		while (csv.good() && std::getline(csv, line2)) {
			if (line2.empty()) {
				continue;
			}

			e.rowCount++;

			size_t loc = 0;
			int col = 0;
			while (loc <= line2.size() && col != e.types.size()) {
				size_t idx;
				if (loc == line2.size()) {
					idx = line2.size();
				}
				else {
					idx = line2.find(',', loc);
				}

				if (idx == std::string::npos) {
					idx = line2.size();
				}

				auto val = line2.substr(loc, idx - loc);
				loc = idx + 1;

				auto& v = e.allocatedValues.emplace_back();
				
				switch (v.type = e.types[col]) {
				case STC_TYPE_BOOL:
					*reinterpret_cast<uint64_t*>(&v.value[0]) = val == "true";
					break;
				case STC_TYPE_STRING:
					e.strings.push_back(std::make_shared<std::string>(val));
					*reinterpret_cast<const char**>(&v.value[0]) = e.strings[e.strings.size() - 1]->data();
					break;
				case STC_TYPE_HASHED:
					*reinterpret_cast<uint64_t*>(&v.value[0]) = hash::Hash64Pattern(val.data());
					break;
				case STC_TYPE_FLOAT:
					*reinterpret_cast<FLOAT*>(&v.value[0]) = (FLOAT)std::atof(val.data());
					break;
				case STC_TYPE_INT:
					*reinterpret_cast<int64_t*>(&v.value[0]) = (int64_t)std::atoll(val.data());
					break;
				default:
					*reinterpret_cast<uint64_t*>(&v.value[0]) = 0;
					break;
				}

				col++;
			}

			if (col != e.types.size()) {
				LOG_ERROR("Error at row {}, not enough columns! {} != {}", e.rowCount, col, e.types.size());
				tableError = true;
				break;
			}
			if (loc < line2.size()) {
				LOG_ERROR("Error at row {}, too many columns!", e.rowCount);
				tableError = true;
				break;
			}
		}
		if (tableError) {
			// clear it
			{
				std::lock_guard<std::mutex> lg(customStringTableEntriesMutex);
				customStringTableEntries.pop_back();
			}
		}
		else {
			e.name = replaced;
			// set the allocated values
			e.values = e.allocatedValues.empty() ? NULL : &e.allocatedValues[0];
			{
				std::lock_guard<std::mutex> lg(customStringTableEntriesMutex);
				customStringTableEntriesCount++;
			}
		}

		csv.close();

	}

	s.close();
}

void* stringtables::GetTable(uint64_t name) {
	static std::unordered_map<uint64_t, bool> dones{};
	void* ret;
	{
		std::lock_guard<std::mutex> lg(customStringTableEntriesMutex);
		auto end = customStringTableEntries.begin() + customStringTableEntriesCount;
		auto it = std::find_if(customStringTableEntries.begin(), end, [name](const auto& e) { return e.name == name; });

		if (it == end) {
			return NULL;
		}
		ret = &(*it);
	}
	auto& d = dones[name];
	if (!d) {
		LOG_INFO("replace StringTable {}", hash_lookup::ExtractTmp(scriptinstance::SI_COUNT, name));
		d = true;
	}
	return ret;
}

void SyncStringTable() {
	auto strLookup = (std::filesystem::path{ inject::g_cliData.workDir } / "strings.txt").string();

	LOG_INFO("Syncing hash map '{}'", strLookup.data());

	hash_lookup::LoadFile(strLookup.data());

	stringtables::SyncTables();

	// add the commands

	REGISTER_COMMAND(ACTS_CSV_READER, "acts_csv", []() { stringtables::SyncTables(); });
}

ADD_INJECTED_SYSTEM(stringtables, "stringtables", inject::PRIORITY_NORMAL, nullptr, SyncStringTable);