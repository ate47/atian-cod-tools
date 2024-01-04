#include <dll_includes.hpp>

namespace {

    struct T9GSCOBJ {
        BYTE magic[8];
        INT32 crc;
        INT32 pad;
        UINT64 name;
        UINT16 string_count;
        UINT16 exports_count;
        UINT16 imports_count;
        UINT16 unk1E;
        UINT32 globalvar_count;
        UINT16 includes_count;
        UINT16 unk26;
        UINT32 loc_28;
        UINT32 loc_2C;
        UINT32 string_offset;
        UINT32 includes_table;
        UINT32 exports_tables;
        UINT32 import_tables;
        UINT32 globalvar_offset;
        UINT32 file_size;
        UINT32 unk_48;
        UINT32 unk_4C;
        UINT16 unk_50;
        UINT16 unk_52;
        UINT16 unk_54;
        UINT16 unk_56;
    };

    struct T8GSCString {
        UINT32 string;
        UINT8 num_address;
        UINT8 type;
        UINT16 pad;
    };

    uintptr_t startModule;

    BYTE* DecryptSTR(BYTE* str) {
        return reinterpret_cast<BYTE*(*)(BYTE * str)>(startModule + 0xC990AE0)(str);
    }


    int DecryptGSCFile(T9GSCOBJ* script) {
        uintptr_t str_location = reinterpret_cast<uintptr_t>(&script->magic[script->string_offset]);

        auto decrypted = 0;
        for (size_t i = 0; i < script->string_count; i++) {

            const auto* str = reinterpret_cast<T8GSCString*>(str_location);

            BYTE* encryptedString = &script->magic[str->string];

            auto* sd = reinterpret_cast<LPCCH>(DecryptSTR(encryptedString));

            LOG_INFO("decrypted: {}", sd);

            decrypted++;

            const auto* strings = reinterpret_cast<const UINT32*>(&str[1]);

            str_location += sizeof(*str) + sizeof(*strings) * str->num_address;
        }

        return decrypted;
    }

    void RunDll() {
        alogs::setfile("acts-bocw.log");

        LOG_INFO("init bocw dll");


        startModule = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));

        LOG_INFO("decrypting scripts...");
        std::filesystem::path spt{ "scriptparsetree_cw" };
        std::filesystem::path sptd{ "scriptparsetree_cw_decrypt" };

        std::filesystem::create_directories(sptd);

        std::vector<std::filesystem::path> files{};
        utils::GetFileRecurse(spt, files, [](const std::filesystem::path& path) { 
            auto name = path.string();
            return name.ends_with(".gscc");
        });

        for (const auto& file : files) {
            LOG_INFO("decrypting {}...", file.string());

            LPVOID buffer{};
            LPVOID bufferAlign{};
            SIZE_T size{};
            SIZE_T sizeAlign{};

            if (!utils::ReadFileAlign(file, buffer, bufferAlign, size, sizeAlign)) {

                LOG_ERROR("Can't read file");
                return;
            }

            if (sizeAlign >= sizeof(T9GSCOBJ) && *reinterpret_cast<UINT64*>(bufferAlign) == 0x38000a0d43534780) {

                auto* s = reinterpret_cast<T9GSCOBJ*>(bufferAlign);

                auto dec = DecryptGSCFile(s);

                LOG_INFO("{} string(s) decrypted", dec);

                std::filesystem::path out = sptd / std::format("script_{:x}.gscc", s->name);

                if (!utils::WriteFile(out, s, sizeAlign)) {
                    LOG_ERROR("Error when writing back the file");
                }
            }
            else {
                LOG_ERROR("BAD MAGIC/SIZE");
            }

            std::free(buffer);
        }
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        RunDll();
        return TRUE;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
