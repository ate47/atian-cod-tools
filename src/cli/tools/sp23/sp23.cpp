#include <includes.hpp>
#include "tools/gsc.hpp"

namespace {
	enum StringTableCellType : byte {
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
		byte* result; // 0
		int unk8; // 8
		StringTableCellType type; // 12
	};

	struct StringTableColumn {
		StringTableCellType type;
		uint16_t* unk8;
		uint16_t* rowindexes;
		uint64_t unk18;
		byte* rowdata;
	};

	struct StringTable {
		uint64_t name;
		int columnCount;
		int rowCount;
		uint64_t cellIndices;
		StringTableColumn* columns;
		uint64_t strings;
	};

	void DumpStringTable(void* ptr, UINT64 name) {
		auto nameFormat = std::format("stringtable_{:x}.csv", name);

		// change here the dump location dir
		std::filesystem::path outPath = std::filesystem::path{ "csv" } / nameFormat;

		auto* st = reinterpret_cast<StringTable*>(ptr);

		if (!st->rowCount || !st->columnCount || !st->columns) {
			return; // empty, ignore
		}

		std::ofstream os{ outPath, std::ios::out };

		if (!os) {
			return; // wtf?
		}

		for (size_t i = 0; i < st->rowCount; i++) {
			for (size_t j = 0; j < st->columnCount; j++) {
				if (j) {
					os << ",";
				}
				auto& columns = st->columns[j];

				auto rowIndex = columns.rowindexes[i];

				int elemSize;
				switch (columns.type) {
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
					os << "<badtype:" << std::hex << columns.type << ">";
					continue; // wtf?
				}

				auto* value = &columns.rowdata[elemSize * rowIndex];

				switch (columns.type) {
					/*

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

					*/
				case STT_UNK_1_64:
				case STT_UNK_9_64:
				case STT_UNK_A_64:
					// strings??
					os << *reinterpret_cast<const char**>(value);
					break;
				case STT_UNK_2_64:
					// int?
					os << std::dec << *reinterpret_cast<INT64*>(value);
					break;
				case STT_UNK_5_64:
				case STT_UNK_6_64:
					os << "hash_" << std::hex << *reinterpret_cast<UINT64*>(value);
					break;
				case STT_UNK_7_64:
					os << "7?" << std::hex << *reinterpret_cast<UINT64*>(value);
					break;
				case STT_UNK_3_32:
					os << *reinterpret_cast<float*>(value);
					break;
				case STT_UNK_8_32:
					os << "8?" << std::hex << *reinterpret_cast<UINT32*>(value);
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

		os.close();
	}
	struct DBAssetPool {
		uintptr_t* m_entries;
		uintptr_t* m_freeHead;
		int m_poolSize;
		int m_elementSize;
		int m_usedCount;
		int m_maxUsedCount;
	};

#define DB_GetAssetEntryPool(type) ((DBAssetPool*)0x12345678)

	void DumpPoolValues() {

		std::ofstream os{ "pools.csv", std::ios::out };

		if (!os) return;

		os << "id,count,max,poolSize,size\n";

		for (size_t i = 0; i < 0xEE; i++) {
			auto* pool = DB_GetAssetEntryPool(i);
			os
				<< std::hex << i << ","
				<< std::dec
				<< pool->m_usedCount << ","
				<< pool->m_maxUsedCount << ","
				<< pool->m_poolSize << ","
				<< std::hex << pool->m_elementSize << "\n";
		}

		os.close();
	}

	struct LocalizeEntry {
		UINT64 name;
		LPCCH str;
	};

	void DumpLocalizeValues() {
		std::ofstream os{ "localize.csv", std::ios::out };
		std::ofstream osbin{ "localize.csvbin", std::ios::out | std::ios::binary };

		if (!os || !osbin) {
			if (os) os.close();
			if (osbin) osbin.close();
			return;
		}

		os << "name,string\n";

		auto* pool = DB_GetAssetEntryPool(AssetType::ASSET_LOCALIZE);

		for (size_t i = 0; i < pool->m_usedCount; i++) {
			auto& entry = reinterpret_cast<LocalizeEntry*>(pool->m_entries)[i];
			if (*entry.str && (*entry.str & 0xC0) == 0x80) {
				// decrypted
				os
					<< "hash_" << std::hex << entry.name << ","
					<< entry.str << "\n";
			}
			else {
				osbin.write(reinterpret_cast<const char*>(&entry.name), sizeof(entry.name));
				osbin.write(entry.str, 400);
			}
		}

		os.close();
		osbin.close();
	}



	int decryptlocalize(Process& _, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		std::filesystem::path file{ argv[2] };

		std::cout << "decrypting " << file.string() << "\n";

		LPVOID bufferPtr = nullptr;
		size_t size = 0;

		if (!utils::ReadFileNotAlign(file, bufferPtr, size)) {
			std::cerr << "can't read file.\n";
			return tool::BASIC_ERROR;
		}

		BYTE* buffer = reinterpret_cast<BYTE*>(bufferPtr);

		size_t location = 0;

		auto f = file.parent_path() / std::format("decrypt_{}.csv", file.filename().string());

		std::cout << "into " << f.string() << "\n";

		std::ofstream os{ f };

		if (!os) {
			std::free(bufferPtr);
			std::cerr << "can't write decrypt file.\n";
			return tool::BASIC_ERROR;
		}
		hashutils::ReadDefaultFile(true, true);

		os << "name,string\n";

		while (location < size) {
			auto name = *reinterpret_cast<UINT64*>(&buffer[location]);
			auto* dec = decrypt::DecryptStringIW(reinterpret_cast<char*>(&buffer[location + 8]));
			if (!dec) {
				continue;
			}
			os << hashutils::ExtractTmp("hash", name)
				<< ","
				<< dec
				<< "\n";
			location += 408;
		}

		std::free(bufferPtr);

		return tool::OK;
	}

	int hash23(Process& _, int argc, const char* argv[]) {
		for (size_t i = 2; i < argc; i++) {

			std::cout << "--- " << argv[i] << "\n";


			UINT64 methods[20][2] = {
				{ 0xcbf29ce484222325LL, 0x100000001b3 },
				{ 0x47F5817A5EF961BALL, 0x100000001b3 },
				{ 0x79D6530B0BB9B5D1LL, 0x10000000233 }
			};

			for (size_t j = 0; j < sizeof(methods) / sizeof(methods[0]); j++) {
				if (!methods[j][0]) {
					continue;
				}
				std::cout
					<< std::hex
					<< methods[j][0] << "/" << methods[j][1] << " -> "
					<< hashutils::Hash64(argv[i], methods[j][0], methods[j][1])
					<< "\n";

			}

		}

		return tool::OK;
	}
}
ADD_TOOL("local23", " [file]", "decrypt local dump 23", nullptr, decryptlocalize);
ADD_TOOL("hash23", " [str]", "hash using iw values", nullptr, hash23);
