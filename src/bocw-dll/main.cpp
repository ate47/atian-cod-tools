#include <dll_includes.hpp>
#include <utils.hpp>

#define EXPORT extern "C" __declspec(dllexport)

namespace {

    struct T9GSCOBJ {
        byte magic[8];
        int32_t crc;
        int32_t pad;
        uint64_t name;
        uint16_t string_count;
        uint16_t exports_count;
        uint16_t imports_count;
        uint16_t unk1E;
        uint32_t globalvar_count;
        uint16_t includes_count;
        uint16_t unk26;
        uint32_t loc_28;
        uint32_t loc_2C;
        uint32_t string_offset;
        uint32_t includes_table;
        uint32_t exports_tables;
        uint32_t import_tables;
        uint32_t globalvar_offset;
        uint32_t file_size;
        uint32_t unk_48;
        uint32_t unk_4C;
        uint16_t unk_50;
        uint16_t unk_52;
        uint16_t unk_54;
        uint16_t unk_56;
    };

    struct T8GSCString {
        uint32_t string;
        uint8_t num_address;
        uint8_t type;
        uint16_t pad;
    };

    uintptr_t startModule;

    byte* DecryptSTR(byte* str) {
        return reinterpret_cast<byte*(*)(byte * str)>(startModule + 0xC990AE0)(str);
    }


    int DecryptGSCFile(T9GSCOBJ* script) {
        uintptr_t str_location = reinterpret_cast<uintptr_t>(&script->magic[script->string_offset]);

        auto decrypted = 0;
        for (size_t i = 0; i < script->string_count; i++) {

            const auto* str = reinterpret_cast<T8GSCString*>(str_location);

            byte* encryptedString = &script->magic[str->string];

            auto* sd = reinterpret_cast<const char*>(DecryptSTR(encryptedString));

            LOG_INFO("decrypted: {}", sd);

            decrypted++;

            const auto* strings = reinterpret_cast<const uint32_t*>(&str[1]);

            str_location += sizeof(*str) + sizeof(*strings) * str->num_address;
        }

        return decrypted;
    }

    void DecryptMTBuffer(size_t count) {
        int done = 0;
        std::unordered_set<std::string> strs{};
        auto* mt_buffer = *reinterpret_cast<byte**>(startModule + 0xF5EC9C8);
        for (size_t i = 1; i < count; i++) {
            auto* loc = mt_buffer + i * 0x10;
            if (i % 0x100 == 0) {
                LOG_INFO("decrypting MT: {:x}/{:x}, done={}", i, count, done);
            }
            auto loc2 = reinterpret_cast<char*>(loc)[2];
            if (*reinterpret_cast<int16_t*>(loc) == 0 || loc[3] != 7 || (loc2 & 0x40) != 0 || loc2 >= 0) {
                continue;
            }

            const byte* v = DecryptSTR(reinterpret_cast<byte*>(loc + 0x18));
            strs.insert(reinterpret_cast<const char*>(v));
            done++;
        }
        std::ofstream outStr{ "scriptbundle_str.txt" };

        if (outStr) {
            for (const auto& st : strs) {
                outStr << st << "\n";
            }
            outStr.close();
            LOG_INFO("dumped strings to scriptbundle_str.txt");
        }
    }

    void InitDll() {
        alogs::setfile("acts-bocw.log");

        LOG_INFO("init bocw dll");


        startModule = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
    }
    void DecryptGSCScripts() {
        LOG_INFO("decrypting gsc scripts...");
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

            void* buffer{};
            void* bufferAlign{};
            size_t size{};
            size_t sizeAlign{};

            if (!utils::ReadFileAlign(file, buffer, bufferAlign, size, sizeAlign)) {

                LOG_ERROR("Can't read file");
                return;
            }

            if (sizeAlign >= sizeof(T9GSCOBJ) && *reinterpret_cast<uint64_t*>(bufferAlign) == 0x38000a0d43534780) {

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
        InitDll();
        return TRUE;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

EXPORT void DLL_DecryptMTBuffer(size_t count) {
    LOG_INFO("Decrypting MT buffer...");
    DecryptMTBuffer(count ? count : 0x100000);
    LOG_INFO("Done");
}

EXPORT void DLL_DecryptGSCScripts() {
    DecryptGSCScripts();
}
