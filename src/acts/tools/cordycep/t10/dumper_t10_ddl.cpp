#include <includes.hpp>
#include <tools/cordycep/t10/dumper_t10.hpp>

namespace {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t10;
	using namespace bo6;

	struct DDLDef;
	struct DDLStruct;
	struct DDLEnum;
	struct DDLDef;

	struct DDL {
		uint64_t name;
		DDLDef* def;
	};

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
		uint64_t* members;
		uint16_t count;
		DDLType type;
	};


	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };


			DDL entry{};
			DDLDef def{};
			if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
				LOG_ERROR("Can't read DDL {:x}", asset.Header);
				return false;
			}

			if (!proc.ReadMemory(&def, reinterpret_cast<uintptr_t>(entry.def), sizeof(def))) {
				LOG_ERROR("Can't read DDL definition {}", (void*)entry.def);
				return false;
			}

			if (def.nameStr) {
				const char* nameStr{ proc.ReadStringTmp(reinterpret_cast<uintptr_t>(def.nameStr)) };
				if (opt.ignoreArchiveDDL && std::string_view{ hashutils::CleanPath(utils::CloneString(nameStr)) }.starts_with("ddl\\cer\\archive")) {
					LOG_INFO("Ignore archive {}", nameStr);
					return true;
				}
				hashutils::AddPrecomputed(hash::HashIWRes(nameStr), nameStr, true);
			}

			const char* dfilename = hashutils::ExtractPtr(entry.name);

			if (!dfilename) dfilename = utils::va("hashed/ddl/file_%llx.ddl", entry.name);

			std::filesystem::path loc{ ctx.outDir / "dump" / dfilename};
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());
			utils::OutFileCE os{ loc };

			if (!os) {
				LOG_ERROR("Can't open {}", loc.string());
				return false;
			}


			const char* filename = hashutils::ExtractTmpPath("ddl", entry.name);

			LOG_INFO("Dump {} -> {}", filename, loc.string());
			os << "// file .... " << filename << "\n\n";

			while (true) {
				os << "version " << std::dec << def.version << " {\n";

				utils::Padding(os, 1) << "// enums ..... " << std::dec << def.enumsCount << " (0x" << std::hex << def.enumsCount << ")\n";
				utils::Padding(os, 1) << "// structs ... " << std::dec << def.structsCount << " (0x" << std::hex << def.structsCount << ")\n";
				utils::Padding(os, 1) << "// bit size .. " << std::dec << def.headerBitSize << " (0x" << std::hex << def.headerBitSize << ")\n";
				utils::Padding(os, 1) << "// byte size . " << std::dec << def.headerByteSize << " (0x" << std::hex << def.headerByteSize << ")\n";
				utils::Padding(os, 1) << "// archive ... " << hashutils::ExtractTmp("hash", def.archiveId) << "\n\n";
				//tool::pool::WriteHex(os, 0, &def, sizeof(def), proc);

				if (!def.structList) {
					LOG_ERROR("Missing struct pointer");
					os << "// missing struct pointer\n";
					return false;
				}


				auto structList{ proc.ReadMemoryArrayEx<DDLStruct>(reinterpret_cast<uintptr_t>(def.structList), def.structsCount) };
				auto enumList{ def.enumsCount ? proc.ReadMemoryArrayEx<DDLEnum>(reinterpret_cast<uintptr_t>(def.enumList), def.enumsCount) : std::make_unique<DDLEnum[]>(1) };

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
								os << opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(structList[member.externalIndex].name)));
								break;
							}
							case DDL_ENUM_TYPE: {
								os << opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(enumList[member.externalIndex].name)));
								break;
							}
							case DDL_PAD_TYPE: os << "pad"; break;
							default: os << "<unk" << std::hex << (int)member.type << ">"; break;
							}


							os << " " << opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(member.name)));

							if (member.isArray) {
								if (member.hashTableIndex >= 0 && member.hashTableIndex < def.enumsCount) {
									os << "[" << opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(enumList[member.hashTableIndex].name))) << "]";
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

				for (size_t i = 1; i < def.structsCount; i++) {
					DDLStruct& stct{ structList[i] };

					utils::Padding(os, 1) << "// bitSize: 0x" << std::hex << stct.bitSize << ", members: " << std::dec << stct.memberCount << "\n";
					utils::Padding(os, 1) << "struct " << opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(stct.name))) << " {\n";
					if (!DumpDDLStructMembers(stct, 2)) return false;
					utils::Padding(os, 1) << "};\n\n";
				}

				for (size_t i = 0; i < def.enumsCount; i++) {
					DDLEnum& enm{ enumList[i] };

					utils::Padding(os, 1) << "// idx 0x" << std::hex << i << " members: 0x" << enm.count << " type: " << enm.type << "\n";
					utils::Padding(os, 1) << "enum " << opt.AddString(proc.ReadStringTmp(reinterpret_cast<uintptr_t>(enm.name))) << " {\n";
					if (enm.count && enm.members) {

						auto [members, ok] = proc.ReadMemoryArray<uint64_t>(reinterpret_cast<uintptr_t>(enm.members), enm.count);

						if (!ok) {
							LOG_ERROR("Can't read DDL enum members {} / {}", hashutils::ExtractTmpPath("ddl", entry.name), (void*)enm.members);
							return false;
						}

						for (size_t i = 0; i < enm.count; i++) {
							utils::Padding(os, 2) << opt.AddString(proc.ReadStringTmp(members[i]));

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
				if (!DumpDDLStructMembers(structList[0], 1)) return false;
				os << "};\n\n";

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

	utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_DDL };
}
