#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>

namespace {
	using namespace fastfile::handlers::bo4;

    struct DDLStruct;
    struct DDLEnum;
    struct DDLDef;
    struct DDLMember;
    struct DDLHash;

    struct DDLDef {
        XHash name;
        uint64_t unk10;
        ID64Metatable metatable;
        DDLStruct* structList;
        DDLEnum* enumList;
        DDLDef* next;
        uint32_t unk38;
        uint32_t byteSize;
        int32_t structCount;
        int32_t enumCount;
        uint32_t unk48;
        uint32_t unk4c;
        uint16_t version;
        uint16_t unk52;
        uint32_t unk54;
    };

    enum DDLType : byte {
        DDL_INVALID_TYPE = 0xFF,
        DDL_BYTE_TYPE = 0,
        DDL_SHORT_TYPE,
        DDL_UINT_TYPE,
        DDL_INT_TYPE,
        DDL_UINT64_TYPE,
        DDL_HASH_TYPE,
        DDL_FLOAT_TYPE,
        DDL_FIXEDPOINT_TYPE,
        DDL_STRING_TYPE,
        DDL_STRUCT_TYPE,
        DDL_ENUM_TYPE,
        DDL_PAD_TYPE,
    };

    struct DDLHashTable {
        DDLHash* list;
        int count;
        int max;
    };

    struct DDLStruct {
        XHash name;
        uint32_t bitSize;
        uint32_t memberCount;
        DDLMember* members; 
        DDLHashTable hashTableLower;
        DDLHashTable hashTableUpper;
    };

    struct __declspec(align(8)) DDLMember {
        XHash name;
        int32_t bitSize;
        uint32_t intSize;
        uint32_t offset;
        uint32_t maxIntValue;
        int16_t arraySize;
        int16_t externalIndex;
        int16_t externalEnumType;
        DDLType type;
        bool isArray;
    };

    struct DDLHash {
        uint64_t hash;
        int index;
    };

    struct DDLEnum {
        XHash name;
        XHash* members;
        int memberCount;
        DDLHashTable hashTable;
    };

    struct DDL {
        XHash name;
        DDLDef* ddlDef;
    };


    const char* DDLTypeName(const DDLMember& member) {
        switch (member.type) {
        case DDL_BYTE_TYPE: return "byte";
        case DDL_SHORT_TYPE: return "short";
        case DDL_UINT_TYPE: {
            if (member.intSize == 1) return "bool";
            if (member.intSize == 32) return "uint";
            return utils::va("uint:%lld", member.intSize);
        }
        case DDL_INT_TYPE: {
            if (member.intSize == 32) return "int";
            return utils::va("int:%lld", member.intSize);
        }
        case DDL_UINT64_TYPE: return "uint64";
        case DDL_FLOAT_TYPE: return "float";
        case DDL_FIXEDPOINT_TYPE: {
            return utils::va("fixed<%lld,%lld>", member.intSize, member.maxIntValue);
        }
        case DDL_HASH_TYPE: return "xhash";
        case DDL_STRING_TYPE: return utils::va("string(%lld)", (member.bitSize / (member.isArray ? member.arraySize : 1)) / 8);
        case DDL_STRUCT_TYPE: return "struct";
        case DDL_ENUM_TYPE: return "enum";
        case DDL_PAD_TYPE: return "uint:1";
        case DDL_INVALID_TYPE:
        default: return "<invalid>";
        }
    }

    void ReadDDLStruct(std::ostream& defout, DDLDef& def, DDLStruct& stct, size_t idx) {
        bool isRoot = stct.name == "root";
        int depth = 1;
        int depthFields = isRoot ? 1 : 2;

        // sort members because they don't match the internal structure (they match the hashmap)
        std::sort(&stct.members[0], &stct.members[stct.memberCount], [](const DDLMember& e1, const DDLMember& e2) {
            return e1.offset < e2.offset;
            });

        utils::Padding(defout, 1) << "// idx " << std::dec << idx << " members " << stct.memberCount << " size 0x" << std::hex << stct.bitSize;
        if (!isRoot) utils::Padding(defout << "\n", depth) << "struct " << hashutils::ExtractTmp("hash", stct.name) << " {";


        int64_t currentShift = 0;
        for (size_t i = 0; i < stct.memberCount; i++) {

            auto& mbm = stct.members[i];

            utils::Padding(defout << "\n", depthFields);

            if (currentShift != mbm.offset) {
                defout << "// invalid struct offset, missing ";
                int64_t delta = (currentShift - (int64_t)mbm.offset);
                if (delta >= 0) {
                    defout << "0x" << std::hex << delta;
                }
                else {
                    defout << "-0x" << std::hex << (-delta);
                }
                defout << " bits\n";
                utils::Padding(defout, depthFields);
            }

            utils::Padding(defout << "// offset 0x" << std::hex << mbm.offset << ", size 0x" << mbm.bitSize << "\n", depthFields);
            currentShift = mbm.offset + mbm.bitSize;

            bool addSize = false;
            if (mbm.type == DDL_STRUCT_TYPE) {
                defout << hashutils::ExtractTmp("hash", def.structList[mbm.externalIndex].name);
            }
            else if (mbm.type == DDL_ENUM_TYPE) {
                defout << hashutils::ExtractTmp("hash", def.enumList[mbm.externalIndex].name);
            }
            else {
                defout << DDLTypeName(mbm);
            }

            defout << " " << hashutils::ExtractTmp("hash", mbm.name);

            if (mbm.isArray) {
                if (mbm.externalEnumType >= 0 && mbm.externalEnumType < def.enumCount) {
                    defout << "[" << hashutils::ExtractTmp("hash", def.enumList[mbm.externalEnumType].name) << "]";
                }
                else {
                    defout << "[" << std::dec << mbm.arraySize << "]";
                }
            }

            if (mbm.type == DDL_PAD_TYPE) {
                defout << "[" << std::dec << mbm.bitSize << "]";
            }

            defout << ";";
        }
        defout << "\n";

        if (!isRoot)utils::Padding(defout, depth) << "};\n\n";
    }


    void ReadDDLEnum(std::ostream& defout, DDLEnum& enumst, size_t idx) {
        utils::Padding(defout, 1) << "// idx " << std::dec << idx << " members " << enumst.memberCount << "\n";
        utils::Padding(defout, 1) << "enum " << hashutils::ExtractTmp("hash", enumst.name) << " {";

        for (size_t i = 0; i < enumst.memberCount; i++) {
            XHash& mbm = enumst.members[i];
            if (i) defout << ",";
            defout << "\n";

            const char* ext{ hashutils::ExtractTmp("hash", mbm.name) };
            bool containBad{};

            const char* ext2{ ext };
            while (*ext2) {
                if (*ext2 >= 0x7f || *ext2 <= 0x21) {
                    containBad = true;
                    break;
                }
                ext2++;
            }


            utils::Padding(defout, 2);

            if (containBad) {
                utils::PrintFormattedString(defout << "\"", ext) << "\"";
            }
            else {
                defout << ext;
            }

            defout << ", // 0x" << std::hex << i;
        }
        defout << "\n";

        utils::Padding(defout, 1) << "};\n\n";
    }

    void ReadDDLDefEntry(std::ostream& defout, DDLDef& def) {
        defout
            << "// " << hashutils::ExtractTmp("hash", def.name.name) << "\n"
            << "// metatable \"" << hashutils::ExtractTmp("hash", def.metatable) << "\"\n"
            << "\n"
            ;

        if (def.structCount) {
            defout
                << "version " << std::dec << def.version << " {\n"
                ;
            for (size_t i = 1; i < def.structCount; i++) {
                ReadDDLStruct(defout, def, def.structList[i], i);
            }
            if (def.enumCount) {
                for (size_t i = 0; i < def.enumCount; i++) {
                    ReadDDLEnum(defout, def.enumList[i], i);
                }
            }

            ReadDDLStruct(defout, def, def.structList[0], 0);
            defout << "}\n\n";
        }
        else {
            defout << "version " << std::dec << def.version << "{}\n";
        }

        if (def.next) {
            defout << "/////////////////////////////////////////////////\n";
            ReadDDLDefEntry(defout, *def.next);
        }
    }

	class ScriptParseTreeWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			DDL* asset{ (DDL*)ptr };

			const char* n{ hashutils::ExtractPtr(asset->name.name) };

			if (!n) {
				n = utils::va("hashed/ddl/file_%llx.ddl", asset->name.name);
			}

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / n };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump {}", outFile.string());

			utils::OutFileCE os{ outFile };

			if (!os) {
				LOG_ERROR("Can't dump {}", outFile.string());
				return;
			}
            if (asset->ddlDef) {
                ReadDDLDefEntry(os, *asset->ddlDef);
            }
		}
	};

	utils::MapAdder<ScriptParseTreeWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_DDL };
}