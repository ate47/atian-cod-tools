#include <includes.hpp>
#include "tools/gsc.hpp"
#include <decryptiw.hpp>
#include <actslib/actslib.hpp>

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

	void DumpStringTable(void* ptr, uint64_t name) {
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
					os << std::dec << *reinterpret_cast<int64_t*>(value);
					break;
				case STT_UNK_5_64:
				case STT_UNK_6_64:
					os << "hash_" << std::hex << *reinterpret_cast<uint64_t*>(value);
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
		uint64_t name;
		const char* str;
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

		LOG_INFO("decrypting {}", file.string());

		void* bufferPtr{};
		size_t size{};

		if (!utils::ReadFileNotAlign(file, bufferPtr, size)) {
			LOG_ERROR("can't read {}", file.string());
			return tool::BASIC_ERROR;
		}

		byte* buffer = reinterpret_cast<byte*>(bufferPtr);

		size_t location = 0;

		auto f = file.parent_path() / std::format("decrypt_{}.csv", file.filename().string());

		LOG_INFO("info {}", f.string());

		std::ofstream os{ f };

		if (!os) {
			std::free(bufferPtr);
			LOG_ERROR("can't write decrypt file.");
			return tool::BASIC_ERROR;
		}
		hashutils::ReadDefaultFile();

		os << "name,string\n";

		while (location < size) {
			auto name = *reinterpret_cast<uint64_t*>(&buffer[location]);
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

			LOG_INFO("--- {}", argv[i]);

			// cg_fovscale 0x682A9BC40F96CA4A

			struct FNVVal {
				uint64_t start{};
				uint64_t iv{};
				const char* name{};
			};

			FNVVal methods[20] = {
				{ 0xcbf29ce484222325LL, 0x100000001b3, "fnv1a"},
				{ 0x47F5817A5EF961BALL, 0x100000001b3, "assets"},
				{ 0x79D6530B0BB9B5D1LL, 0x10000000233, "fields"},
				//{ 0x1429C8E20321BBCD, 0x10000000233 },
				//{ 0x7D31DB2080103883, 0x10000000233 },
				
			};

			for (const auto& method : methods) {
				if (!method.start) {
					continue;
				}

				LOG_INFO("{:x}/{:x} -> {:x} ({})", 
					method.start, method.iv, hashutils::Hash64A(argv[i], method.start, method.iv), method.name ? method.name : "???"
				);
			}

		}

		return tool::OK;
	}

	int hash23search(Process& _, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		std::filesystem::path file{ argv[2] };

		LOG_INFO("Loading {}", file.string());

		std::ifstream is{ file };

		if (!is) {
			LOG_ERROR("Can't open {}", file.string());
			return tool::BASIC_ERROR;
		}

		actslib::ToClose tc{ is };
		std::string line{};

		size_t c{};

		while (is && std::getline(is, line)) {
			if (line.empty()) {
				continue;
			}

			uint64_t hash = std::strtoull(line.c_str(), nullptr, 16);

			if (hashutils::Hash64("cg_chattime", hash, 0x10000000233) == (0x67F7F2FA59D85B1E & 0x7FFFFFFFFFFFFFFF)) {
				LOG_INFO("Candidate1: {:x}", hash);
			}
			if (hashutils::Hash64("g_chattime", hash, 0x10000000233) == (0x67F7F2FA59D85B1E & 0x7FFFFFFFFFFFFFFF)) {
				LOG_INFO("Candidate2: {:x}", hash);
			}
			if (hashutils::Hash64("_chattime", hash, 0x10000000233) == (0x67F7F2FA59D85B1E & 0x7FFFFFFFFFFFFFFF)) {
				LOG_INFO("Candidate3: {:x}", hash);
			}
			if (hashutils::Hash64("chattime", hash, 0x10000000233) == (0x67F7F2FA59D85B1E & 0x7FFFFFFFFFFFFFFF)) {
				LOG_INFO("Candidate4: {:x}", hash);
			}
			c++;
		}
		
		LOG_INFO("Can't find candidate in {} hashes", c);

		return tool::OK;
	}

	int vmtest(Process& _, int argc, const char* argv[]) {
		tool::gsc::opcode::VmInfo* nfo{};

		if (!tool::gsc::opcode::IsValidVm(tool::gsc::opcode::VM_MW23, nfo)) {
			return tool::BAD_USAGE;
		}

		int opcodes[]{
			0,1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf,0x10,0x11,0x12,0x13,0x14
			,0x15,0x16,0x17,0x59,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21
			,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0xb,0xa4,0x2c,0x2d,0x2e,0x2f
			,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e
			,0xa2,0x3f,0x40,0x41,0x42,0x43,0x44,0x46,0x47,0x48,0x49,0xa5,0x4a,0x4b,0x4c
			,0x4e,0x4f,0x50,0x51,0x52,0xa3,0x54,0x55,0x56,0x57,0x58,0x5a,0x5b,0x5c,0x5d
			,0x5e,0x5f,0x60,0x61,0x62,0x63,0x65,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x97
			,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c
			,0x7d,0x7e,0x7f,0x80,0x81,0x82,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c
			,0x8d,0x98,0x99,0x9a,0xa6
		};

		for (int opcode : opcodes) {
			if (nfo->opcodemap.find(opcode) == nfo->opcodemap.end()) {
				LOG_ERROR("Missing opcode {:x}", opcode);
				continue;
			}
		}

		return tool::OK;
	}
	
}
#ifndef CI_BUILD
ADD_TOOL("local23", " [file]", "decrypt local dump 23", nullptr, decryptlocalize);
ADD_TOOL("hash23search", " [file]", "", nullptr, hash23search);
ADD_TOOL("vmtest", "", "test vm opcodes", nullptr, vmtest);

#endif
ADD_TOOL("hash23", " [str]", "hash using iw values", nullptr, hash23);
