#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo6.hpp>

namespace {
	using namespace fastfile::handlers::bo6;

	struct DDLDef;
	struct DDLStruct;
	struct DDLEnum;
	struct DDLDef;

	struct DDL {
		XHash64 name;
		DDLDef* def;
	};
	static_assert(sizeof(DDL) == 0x10);

	struct DDLDef {
		uint64_t version;
		const char* nameStr;
		uint64_t unk10;
		uint64_t archiveId;
		DDLStruct* structList;
		DDLEnum* enumList;
		DDLDef* next;
		uint32_t headerBitSize;
		uint32_t headerByteSize;
		int32_t structsCount;
		int16_t enumsCount;
		uint64_t unk48;
		uint64_t unk50;
	};
	static_assert(sizeof(DDLDef) == 0x58);

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
		DDL_HASH_RESOURCE_TYPE = 0xC,
	};

	struct DDLMember {
		const char* name;
		XHash64 nameHashed;
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
	static_assert(sizeof(DDLMember) == 0x28);

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
		const char* name;
		int bitSize;
		int memberCount;
		DDLMember* members;
		DDLHashTable hashTableLower;
		DDLHashTable hashTableUpper;
	};
	struct DDLEnum {
		const char* name;
		DDLHashTable list;
		const char** members;
		uint16_t count;
		DDLType type;
	};
	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {

			DDL* asset{ (DDL*)ptr };

			if (!asset->def) {
				return; // empty ddl
			}

			DDLDef* ndef{ asset->def };

			if (ndef->nameStr) {
				if (!opt.archiveDDL && std::string_view{ hashutils::CleanPath(utils::CloneString(ndef->nameStr)) }.starts_with("ddl\\cer\\archive")) {
					LOG_INFO("Ignore archive {}", ndef->nameStr);
					return;
				}
				hashutils::AddPrecomputed(hash::HashIWAsset(ndef->nameStr), ndef->nameStr, true);
			}

			const char* dfilename = hashutils::ExtractPtr(asset->name);

			if (!dfilename) dfilename = utils::va("hashed/ddl/file_%llx.ddl", asset->name);

			std::filesystem::path outFile{ opt.m_output / gamePath / "source" / dfilename };
			std::filesystem::create_directories(outFile.parent_path());
			utils::OutFileCE os{ outFile };

			if (!os) {
				LOG_ERROR("Can't open {}", outFile.string());
				return;
			}

			const char* filename = hashutils::ExtractTmpPath("ddl", asset->name);

			LOG_INFO("Dump ddl {} -> {}", filename, outFile.string());
			os << "// file .... " << filename << "\n\n";

			while (true) {
				DDLDef& def{ *ndef };
				os << "version " << std::dec << def.version << " {\n";

				utils::Padding(os, 1) << "// enums ..... " << std::dec << def.enumsCount << " (0x" << std::hex << def.enumsCount << ")\n";
				utils::Padding(os, 1) << "// structs ... " << std::dec << def.structsCount << " (0x" << std::hex << def.structsCount << ")\n";
				utils::Padding(os, 1) << "// bit size .. " << std::dec << def.headerBitSize << " (0x" << std::hex << def.headerBitSize << ")\n";
				utils::Padding(os, 1) << "// byte size . " << std::dec << def.headerByteSize << " (0x" << std::hex << def.headerByteSize << ")\n";
				utils::Padding(os, 1) << "// archive ... " << hashutils::ExtractTmp("hash", def.archiveId) << "\n\n";

				if (!def.structList) {
					LOG_ERROR("Missing struct pointer");
					os << "// missing struct pointer\n";
					return;
				}
				*os << std::flush;

				DDLStruct* structList{ def.structList };
				DDLEnum* enumList{ def.enumList };

				auto DumpDDLStructMembers = [&os, &def, &opt, &structList, &enumList](DDLStruct& stct, int padding)  -> bool {
					if (stct.memberCount && stct.members) {
						DDLMember* members{ stct.members };

						std::sort(&members[0], &members[stct.memberCount], [](const DDLMember& d1, const DDLMember& d2) -> bool { return d1.offset < d2.offset; });

						for (size_t i = 0; i < stct.memberCount; i++) {
							DDLMember& member = members[i];
							if (member.type == DDL_PAD_TYPE) {
								continue; // ignore padding
							}
							utils::Padding(os, padding) << "// offset: 0x" << std::hex << member.offset << ", bitSize: 0x" << member.bitSize;
							if (!(member.bitSize & 0x7)) {
								os << "(0x" << std::hex << (member.bitSize >> 3) << " Byte(s))";
							}
							if (member.isArray) {
								os << ", array:0x" << member.arraySize << "(hti:0x" << member.hashTableIndex << ")";
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
							case DDL_HASH_TYPE: os << "hash"; break;
							case DDL_HASH_RESOURCE_TYPE: os << "hashres"; break;
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
								os << structList[member.externalIndex].name;
								break;
							}
							case DDL_ENUM_TYPE: {
								os << enumList[member.externalIndex].name;
								break;
							}
							case DDL_PAD_TYPE: os << "pad"; break;
							default: os << "<unk" << std::hex << (int)member.type << ">"; break;
							}


							os << " " << (member.name ? member.name : hashutils::ExtractTmp("hash", member.nameHashed));

							if (member.isArray) {
								if (member.hashTableIndex >= 0 && member.hashTableIndex < def.enumsCount) {
									os << "[" << enumList[member.hashTableIndex].name << "]";
								}
								else {
									os << "[" << std::dec << (int)member.arraySize << "]";
								}
							}
							else if (member.type == DDL_PAD_TYPE) {
								os << "[" << std::dec << member.bitSize << "]";
							}

							os << ";\n";
							*os << std::flush;
						}

					}
					else if (stct.memberCount) {
						LOG_ERROR("Missing struct members pointer with {} member(s)", stct.memberCount);
						utils::Padding(os, 1) << "// missing struct members pointer\n";
					}
					return true;
				};

				DDLStruct& root{ structList[0] };

				for (size_t i = 1; i < def.structsCount; i++) {
					DDLStruct& stct{ structList[i] };

					utils::Padding(os, 1) << "// bitSize: 0x" << std::hex << stct.bitSize << ", members: " << std::dec << stct.memberCount << "\n";
					utils::Padding(os, 1) << "struct " << stct.name << " {\n";
					if (!DumpDDLStructMembers(stct, 2)) return;
					utils::Padding(os, 1) << "};\n\n";
				}

				for (size_t i = 0; i < def.enumsCount; i++) {
					DDLEnum& enm{ enumList[i] };

					utils::Padding(os, 1) << "// idx 0x" << std::hex << i << " members: 0x" << enm.count << " type: " << (int)enm.type << "\n";
					utils::Padding(os, 1) << "enum " << enm.name << " {\n";
					if (enm.count && enm.members) {

						for (size_t i = 0; i < enm.count; i++) {
							utils::Padding(os, 2) << enm.members[i];

							if (i + 1 != enm.count) {
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

				if (!def.next) {
					break;
				}

				if (def.next) {
					LOG_ERROR("Can't read DDL definition {}", (void*)def.next);
					ndef = def.next;
				}
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> impl{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_DDL, sizeof(DDL) };
}