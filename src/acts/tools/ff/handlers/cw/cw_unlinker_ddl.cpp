#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/ff/handlers/handler_game_cw.hpp>

namespace {
	using namespace fastfile::handlers::cw;


	struct DDLDef;
	struct DDLStruct;
	struct DDLEnum;
	struct DDLDef;

	struct DDLRoot {
		uint64_t name;
		uint64_t depends_name;
		DDLDef* ddlDef;
	}; static_assert(sizeof(DDLRoot) == 0x18);

	struct DDLDef {
		uint64_t unk0;
		CWXHash name;
		uint64_t unk10;
		uint64_t version;
		DDLStruct* structList;
		DDLEnum* enumList;
		DDLDef* next;
		uint32_t headerBitSize;
		uint32_t headerByteSize;
		int32_t numStructs;
		int32_t numEnums;
		uint32_t unk48;
		uint32_t unk4c;
		uint16_t unk50;
		uint16_t unk52;
		uint32_t unk54;
	}; static_assert(sizeof(DDLDef) == 0x58);
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
		CWXHash name;
		int32_t bitSize;
		uint32_t intSize;
		uint32_t offset;
		uint32_t maxIntValue;
		int16_t arraySize;
		int16_t externalIndex;
		int16_t externalEnumType;
		DDLType type;
		bool isArray;
	}; static_assert(sizeof(DDLMember) == 0x20);

	struct DDLHash {
		CWXHash hash;
		int index;
	}; static_assert(sizeof(DDLHash) == 0x10);

	struct DDLHashTable {
		DDLHash* list;
		int32_t count;
		int32_t max;
	}; static_assert(sizeof(DDLHashTable) == 0x10);

	struct DDLStruct {
		CWXHash name;
		int bitSize;
		int memberCount;
		DDLMember* members;
		DDLHashTable hashTableLower;
		DDLHashTable hashTableUpper;
	}; static_assert(sizeof(DDLStruct) == 0x38);
	struct DDLEnum {
		CWXHash name;
		DDLHash* members;
		int memberCount;
		DDLHashTable hashTable;
	}; static_assert(sizeof(DDLEnum) == 0x28);


	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			DDLRoot* asset{ (DDLRoot*)ptr };

			const char* name{ hashutils::ExtractPtr(asset->name) };


			DDLRoot& entry{ *asset };
			DDLDef* def{ entry.ddlDef };

			const char* dfilename = hashutils::ExtractPtr(entry.name);

			if (!dfilename) dfilename = utils::va("file_%llx.ddl", entry.name);

			std::filesystem::path loc{ opt.m_output / "cw" / "source" / "ddl" / dfilename };
			std::filesystem::create_directories(loc.parent_path());

			utils::OutFileCE os{ loc };

			if (!os) {
				LOG_ERROR("Can't open {}", loc.string());
				return;
			}


			const char* filename = hashutils::ExtractTmpPath("ddl", entry.name);

			LOG_INFO("Dump {} -> {}", filename, loc.string());
			os << "// file .... " << filename << "\n\n";
			if (entry.depends_name) {
				os << "#redirect " << hashutils::ExtractTmp("hash", entry.depends_name) << ";\n\n";
			}

			while (def) {
				os << "version " << hashutils::ExtractTmp("hash", def->version) << " {\n";
				utils::Padding(os, 1) << "// enums ..... " << std::dec << def->numEnums << " (0x" << std::hex << def->numEnums << ")\n";
				utils::Padding(os, 1) << "// structs ... " << std::dec << def->numStructs << " (0x" << std::hex << def->numStructs << ")\n";
				utils::Padding(os, 1) << "// header bit size .. " << std::dec << def->headerBitSize << " (0x" << std::hex << def->headerBitSize << ")\n";
				utils::Padding(os, 1) << "// header byte size . " << std::dec << def->headerByteSize << " (0x" << std::hex << def->headerByteSize << ")\n";
				os << "\n";

				if (!def->structList) {
					LOG_ERROR("Missing struct pointer");
					os << "// missing struct pointer\n";
					return;
				}


				DDLStruct* structList{ def->structList };
				DDLEnum* enumList{ def->enumList };

				auto DumpDDLStructMembers = [&os, &def, &entry, &opt, &structList, &enumList](DDLStruct& stct, int padding)  -> bool {
					if (stct.memberCount && stct.members) {
						std::sort(&stct.members[0], &stct.members[stct.memberCount], [](const DDLMember& d1, const DDLMember& d2) -> bool { return d1.offset < d2.offset; });

						for (size_t i = 0; i < stct.memberCount; i++) {
							DDLMember& member = stct.members[i];
							if (member.type == DDL_PAD_TYPE) {
								continue; // ignore padding
							}
							utils::Padding(os, padding) << "// offset: 0x" << std::hex << member.offset << ", bitSize: 0x" << member.bitSize;
							if (!(member.bitSize & 0x7)) {
								os << "(0x" << std::hex << (member.bitSize >> 3) << " Byte(s))";
							}
							if (member.isArray) {
								os << ", array:0x" << member.arraySize << "(hti:0x" << member.externalEnumType << ")";
							}
							os << "\n";

							utils::Padding(os, padding);

							switch (member.type) {
							case DDL_BYTE_TYPE: os << "byte"; break;
							case DDL_SHORT_TYPE: os << "short"; break;
							case DDL_UINT_TYPE: {
								if (member.intSize == 1) {
									os << "bool";
									break;
								}
								os << "uint";
								if (member.intSize != 32) {
									os << ":" << std::dec << member.intSize;
								}
								break;
							}
							case DDL_INT_TYPE: {
								os << "int";
								if (member.intSize != 32) {
									os << ":" << std::dec << member.intSize;
								}
								break;
							}
							case DDL_UINT64_TYPE: os << "uint64"; break;
							case DDL_HASH_TYPE: os << "xhash"; break;
							case DDL_FLOAT_TYPE: {
								if (member.intSize == 32) {
									os << "float";
									break;
								}
								if (member.intSize == 64) {
									os << "double";
									break;
								}

								os << "float:" << std::dec << member.intSize;
								break;
							}
							case DDL_FIXEDPOINT_TYPE: {
								os << "fixed<" << std::dec << member.intSize << "," << member.maxIntValue << ">";
								break;
							}
							case DDL_STRING_TYPE: os << "string(" << std::dec << ((member.bitSize / (member.isArray ? member.arraySize : 1)) / 8) << ")";
								break;
							case DDL_STRUCT_TYPE: {
								os << hashutils::ExtractTmp("hash", structList[member.externalIndex].name);
								break;
							}
							case DDL_ENUM_TYPE: {
								os << hashutils::ExtractTmp("hash", enumList[member.externalIndex].name);
								break;
							}
							case DDL_PAD_TYPE: os << "pad"; break;
							default: os << "<unk" << std::hex << (int)member.type << ">"; break;
							}


							os << " " << hashutils::ExtractTmp("hash", member.name);

							if (member.isArray) {
								if (member.externalEnumType >= 0 && member.externalEnumType < def->numEnums) {
									os << "[" << hashutils::ExtractTmp("hash", enumList[member.externalEnumType].name) << "]";
								}
								else {
									os << "[" << std::dec << (int)member.arraySize << "]";
								}
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
					return true;
				};

				DDLStruct& root{ structList[0] };

				for (size_t i = 1; i < def->numStructs; i++) {
					DDLStruct& stct{ structList[i] };

					utils::Padding(os, 1) << "// bitSize: 0x" << std::hex << stct.bitSize << ", members: " << std::dec << stct.memberCount << "\n";
					utils::Padding(os, 1) << "struct " << hashutils::ExtractTmp("hash", stct.name) << " {\n";
					if (!DumpDDLStructMembers(stct, 2)) return;
					utils::Padding(os, 1) << "};\n\n";
				}

				for (size_t i = 0; i < def->numEnums; i++) {
					DDLEnum& enm{ enumList[i] };

					utils::Padding(os, 1) << "// idx 0x" << std::hex << i << " members: 0x" << enm.memberCount << "\n";
					utils::Padding(os, 1) << "enum " << hashutils::ExtractTmp("hash", enm.name) << " {\n";
					if (enm.memberCount && enm.members) {

						for (size_t i = 0; i < enm.memberCount; i++) {
							utils::Padding(os, 2) << hashutils::ExtractTmp("hash", enm.members[i].hash);

							if (i + 1 != enm.memberCount) {
								os << ",";
							}

							os << " // 0x" << std::hex << i << "\n";
						}


					}
					else if (!enm.members) {
						LOG_ERROR("Missing enum members pointer");
						utils::Padding(os, 2) << "// missing enum members pointer\n";
					}

					utils::Padding(os, 1) << "};\n\n";
				}


				utils::Padding(os, 1) << "// root: bitSize: 0x" << std::hex << root.bitSize << ", members: " << std::dec << root.memberCount << "\n\n";
				if (!DumpDDLStructMembers(structList[0], 1)) return;
				os << "};\n\n";

				*os << std::flush;
				def = def->next;
			}

		}
	};

	utils::MapAdder<ImplWorker, XAssetType, Worker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_DDL };
}