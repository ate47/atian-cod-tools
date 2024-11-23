#include <includes.hpp>
#include <utils/decrypt.hpp>

namespace {
    struct T8GSCOBJ {
        byte magic[8];
        int32_t crc;
        int32_t pad;
        uint64_t name;
        int32_t include_offset;
        uint16_t string_count;
        uint16_t exports_count;
        int32_t cseg_offset;
        int32_t string_offset;
        int16_t imports_count;
        uint16_t fixup_count;
        int32_t devblock_string_offset;
        int32_t export_table_offset;
        int32_t ukn34;
        int32_t imports_offset;
        uint16_t globalvar_count;
        int32_t fixup_offset;
        int32_t globalvar_offset;
        int32_t script_size;
        int32_t requires_implements_offset;
        int16_t ukn50;
        int16_t devblock_string_count;
        int32_t cseg_size;
        uint16_t include_count;
        byte ukn5a;
        byte requires_implements_count;
    };

    struct T8GSCFixup {
        uintptr_t offset;
        uintptr_t address;
    };

    struct T8GSCImport {
        uint32_t name;
        uint32_t import_namespace;
        uint16_t num_address;
        uint8_t param_count;
        uint8_t flags;
    };

    struct T8GSCGlobalVar {
        uint32_t name;
        uint32_t num_address;
    };

    struct T8GSCExport {
        uint32_t checksum;
        uint32_t address;
        uint32_t name;
        uint32_t name_space;
        uint32_t callback_event;
        uint8_t param_count;
        uint8_t flags;
        uint16_t padding;
    };

    struct T8GSCString {
        uint32_t string;
        uint8_t num_address;
        uint8_t type;
        uint16_t pad;
    };


	int gscdemo(Process& _, int argc, const char* argv[]) {
        std::string buffer{};

        if (argc < 3 || !utils::ReadFile(argv[2], buffer)) {
            return tool::BAD_USAGE;
        }

        byte* ptr = (byte*)buffer.data();
        T8GSCOBJ* obj = (T8GSCOBJ*)ptr;


        hashutils::ReadDefaultFile();
        LOG_INFO("{}", hashutils::ExtractTmpScript(obj->name));

        uint64_t* includes = (uint64_t*)(ptr + obj->include_offset);

        for (size_t i = 0; i < obj->include_count; i++) {
            LOG_INFO("#using {};", hashutils::ExtractTmpScript(includes[i]));
        }

        LOG_INFO("");

        T8GSCExport* exports = (T8GSCExport*)(ptr + obj->export_table_offset);

        for (size_t i = 0; i < obj->exports_count; i++) {
            T8GSCExport& exp = exports[i];

            LOG_INFO(
                "function {}::{}({})",
                hashutils::ExtractTmp("namespace", exp.name_space),
                hashutils::ExtractTmp("function", exp.name),
                (int)exp.param_count
                );

            std::ostringstream ss;

            byte* bytecode = ptr + exp.address;

            for (size_t i = 0; i < 0x10; i++) {
                ss << std::hex << std::setw(2) << std::setfill('0') << (int)bytecode[i] << " ";
            }

            LOG_INFO("bytecode: {}", ss.str());

        }

        LOG_INFO("");

        T8GSCImport* imports = (T8GSCImport*)(ptr + obj->imports_offset);

        for (size_t i = 0; i < obj->imports_count; i++) {
            T8GSCImport& imp = *imports;

            LOG_INFO(
                "import {}::{}",
                hashutils::ExtractTmp("namespace", imp.import_namespace),
                hashutils::ExtractTmp("function", imp.name)
            );

            uint32_t* addr = (uint32_t*)(imports + 1);

            std::ostringstream ss;

            for (size_t i = 0; i < imp.num_address; i++) {
                ss << std::hex << "0x" << addr[i] << " ";
            }

            LOG_INFO("addresses: {}", ss.str());


            imports = (T8GSCImport*)(addr + imp.num_address);
        }
        LOG_INFO("");

        T8GSCString* strings = (T8GSCString*)(ptr + obj->string_offset);

        for (size_t i = 0; i < obj->string_count; i++) {
            T8GSCString& str = *strings;

            const char* s = decrypt::DecryptString((char*)(ptr + str.string));

            LOG_INFO("{}", s);

            uint32_t* addr = (uint32_t*)(strings + 1);

            std::ostringstream ss;

            for (size_t i = 0; i < str.num_address; i++) {
                ss << std::hex << "0x" << addr[i] << " ";
            }

            LOG_INFO("addresses: {}", ss.str());


            strings = (T8GSCString*)(addr + str.num_address);
        }
        LOG_INFO("");


        // import 1 (num adresses)
        // adresse 1 adr 2 adr 3
        // import 2
        // 0x36000A0D43534780
        // 0xA0D43534780

        // 0x36 bo4
        // 0x37 bocw 
        // 0x38 
        // 0x1b bo3
        // 0x1c
        // 8a mwiii
        // 8b mwiii
        // 06 bo6
        // 07 




		return tool::OK;
	}
}

#ifndef CI_BUILD

ADD_TOOL(gscdemo, "dev", "", "", nullptr, gscdemo);

#endif