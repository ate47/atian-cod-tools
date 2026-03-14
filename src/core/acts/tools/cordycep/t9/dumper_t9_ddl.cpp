#include <includes.hpp>
#include <tools/cordycep/t9/dumper_t9.hpp>

namespace {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t9;
	using namespace cw;

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
		uint64_t name;
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
		uint64_t name;
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
		uint64_t hash;
		int index;
	}; static_assert(sizeof(DDLHash) == 0x10);

	struct DDLHashTable {
		DDLHash* list;
		int32_t count;
		int32_t max;
	}; static_assert(sizeof(DDLHashTable) == 0x10);

	struct DDLStruct {
		uint64_t name;
		int bitSize;
		int memberCount;
		DDLMember* members;
		DDLHashTable hashTableLower;
		DDLHashTable hashTableUpper;
	}; static_assert(sizeof(DDLStruct) == 0x38);
	struct DDLEnum {
		uint64_t name;
		DDLHash* members;
		int memberCount;
		DDLHashTable hashTable;
	}; static_assert(sizeof(DDLEnum) == 0x28);





	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };


			DDLRoot entry{};
			DDLDef def{};
			if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
				LOG_ERROR("Can't read DDL {:x}", asset.Header);
				return false;
			}

			if (!proc.ReadMemory(&def, reinterpret_cast<uintptr_t>(entry.ddlDef), sizeof(def))) {
				LOG_ERROR("Can't read DDL definition {}", (void*)entry.ddlDef);
				return false;
			}

			const char* dfilename = hashutils::ExtractPtr(entry.name);

			if (!dfilename) dfilename = utils::va("file_%llx.ddl", entry.name);

			std::filesystem::path loc{ ctx.outDir / "dump" / "ddl" / dfilename };
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());
			utils::OutFileCE os{ loc };

			if (!os) {
				LOG_ERROR("Can't open {}", loc.string());
				return false;
			}


			const char* filename = hashutils::ExtractTmpPath("ddl", asset.ID);

			LOG_INFO("Dump {} -> {}", filename, loc.string());
			os << "// file .... " << filename << "\n\n";
			if (entry.depends_name) {
				os << "#redirect " << hashutils::ExtractTmp("hash", entry.depends_name) << ";\n\n";
			}

			auto WriteHeader = [&os, &def]() {
				utils::Padding(os, 1) << "// enums ..... " << std::dec << def.numEnums << " (0x" << std::hex << def.numEnums << ")\n";
				utils::Padding(os, 1) << "// structs ... " << std::dec << def.numStructs << " (0x" << std::hex << def.numStructs << ")\n";
				utils::Padding(os, 1) << "// header bit size .. " << std::dec << def.headerBitSize << " (0x" << std::hex << def.headerBitSize << ")\n";
				utils::Padding(os, 1) << "// header byte size . " << std::dec << def.headerByteSize << " (0x" << std::hex << def.headerByteSize << ")\n";
			};

			while (true) {
				os << "version " << hashutils::ExtractTmp("hash", def.version) << " {\n";
				WriteHeader();
				os << "\n";
				//tool::pool::WriteHex(os, 0, &def, sizeof(def), proc);

				if (!def.structList) {
					LOG_ERROR("Missing struct pointer");
					os << "// missing struct pointer\n";
					return false;
				}


				auto structList{ proc.ReadMemoryArrayEx<DDLStruct>(reinterpret_cast<uintptr_t>(def.structList), def.numStructs) };
				auto enumList{ def.numEnums ? proc.ReadMemoryArrayEx<DDLEnum>(reinterpret_cast<uintptr_t>(def.enumList), def.numEnums) : std::make_unique<DDLEnum[]>(1) };

				auto DumpDDLStructMembers = [&os, &proc, &def, &entry, &opt, &structList, &enumList](DDLStruct& stct, int padding)  -> bool {
					if (stct.memberCount && stct.members) {
						auto [members, ok] = proc.ReadMemoryArray<DDLMember>(reinterpret_cast<uintptr_t>(stct.members), stct.memberCount);

						if (!ok) {
							LOG_ERROR("Can't read DDL members {}", hashutils::ExtractTmpPath("ddl", entry.name));
							return false;
						}

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
								if (member.externalEnumType >= 0 && member.externalEnumType < def.numEnums) {
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

				for (size_t i = 1; i < def.numStructs; i++) {
					DDLStruct& stct{ structList[i] };

					utils::Padding(os, 1) << "// bitSize: 0x" << std::hex << stct.bitSize << ", members: " << std::dec << stct.memberCount << "\n";
					utils::Padding(os, 1) << "struct " << hashutils::ExtractTmp("hash", stct.name) << " {\n";
					if (!DumpDDLStructMembers(stct, 2)) return false;
					utils::Padding(os, 1) << "};\n\n";
				}

				for (size_t i = 0; i < def.numEnums; i++) {
					DDLEnum& enm{ enumList[i] };

					utils::Padding(os, 1) << "// idx 0x" << std::hex << i << " members: 0x" << enm.memberCount << "\n";
					utils::Padding(os, 1) << "enum " << hashutils::ExtractTmp("hash", enm.name) << " {\n";
					if (enm.memberCount && enm.members) {

						auto [members, ok] = proc.ReadMemoryArray<DDLHash>(reinterpret_cast<uintptr_t>(enm.members), enm.memberCount);

						if (!ok) {
							LOG_ERROR("Can't read DDL enum members {} / {}", hashutils::ExtractTmpPath("ddl", entry.name), (void*)enm.members);
							return false;
						}

						for (size_t i = 0; i < enm.memberCount; i++) {
							utils::Padding(os, 2) << hashutils::ExtractTmp("hash", members[i].hash);

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
				if (!DumpDDLStructMembers(structList[0], 1)) return false;
				os << "};\n\n";

				if (opt.ignoreArchiveDDL) {
					while (def.next) {
						if (!proc.ReadMemory(&def, reinterpret_cast<uintptr_t>(def.next), sizeof(def))) {
							LOG_ERROR("Can't read DDL definition {}", (void*)def.next);
							return false;
						}

						os << "version " << hashutils::ExtractTmp("hash", def.version) << " {\n";
						WriteHeader();
						os << "}; \n\n";
					}

					break;
				}

				if (!def.next) {
					break;
				}

				if (!proc.ReadMemory(&def, reinterpret_cast<uintptr_t>(def.next), sizeof(def))) {
					LOG_ERROR("Can't read DDL definition {}", (void*)def.next);
					return false;
				}
			}

			return true;
		}

	};

	utils::MapAdder<UnlinkerImpl, cw::XAssetType, Unlinker> impl{ GetUnlinkers(), cw::ASSET_TYPE_DDL };
}
