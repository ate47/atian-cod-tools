#include <includes.hpp>
#include "acti_crypto_keys.hpp"
#include <core/memory_allocator.hpp>
#include <deps/base64.hpp>
#define LTC_NO_PROTOTYPES
#include <tomcrypt.h>

namespace compatibility::acti::crypto_keys {
    namespace {
        core::memory_allocator::MemoryAllocator alloc{};
    }

    std::vector<AesKeyLocal*>& GetKeys() {
        static std::vector<AesKeyLocal*> keys{};
        return keys;
    }
    std::vector<RsaKeyLocal*>& GetRsaKeys() {
        static std::vector<RsaKeyLocal*> keys{};
        return keys;
    }
    RsaKeyLocal* GetRSAKeyByName(const char* id) {
        for (RsaKeyLocal* k : GetRsaKeys()) {
            if (!_strcmpi(id, k->id)) {
                return k;
            }
        }
        return nullptr;
    }

    KeyVersion GetKeyVersionByName(const char* version) {
        if (!_strcmpi(version, "bo3"))
            return KeyVersion::VER_BO3;
        if (!_strcmpi(version, "bo4"))
            return KeyVersion::VER_BO4;
        return KeyVersion::VER_INVALID;
    }

    KeyType GetKeyTypeByName(const char* version) {
        if (!_strcmpi(version, "private") || !_strcmpi(version, "prv"))
            return KeyType::KEYT_PRIVATE;
        if (!_strcmpi(version, "public") || !_strcmpi(version, "pub"))
            return KeyType::KEYT_PUBLIC;
        return KeyType::KEYT_INVALID;
    }

    const char* GetKeyVersionName(KeyVersion version) {
        switch (version) {
        case KeyVersion::VER_BO3:
            return "BO3";
        case KeyVersion::VER_BO4:
            return "BO4";
        default:
            return "invalid";
        }
    }

    const char* GetKeyTypeName(KeyType type) {
        switch (type) {
        case KeyType::KEYT_PUBLIC:
            return "PUBLIC";
        case KeyType::KEYT_PRIVATE:
            return "PRIVATE";
        default:
            return "invalid";
        }
    }

    AesKeyLocal* GetKeyByName(const char* filename, KeyVersion version) {
        for (AesKeyLocal* k : GetKeys()) {
            if (!_strcmpi(filename, k->fileName) && k->version == version) {
                return k;
            }
        }
        return nullptr;
    }

    void LoadAesKeys(const std::filesystem::path& path) {
        utils::InFileCE is{ path };
        if (!is) {
            return; // no file
        }

        std::string line;
        size_t idx{};
        size_t added{};
        while (*is && std::getline(*is, line)) {
            idx++;
            if (line.empty() || line[0] == '#') {
                continue;
            }

            size_t cut1{ line.find(',') };
            size_t cut2{ cut1 != std::string::npos ? line.find(',', cut1 + 1) : cut1 };

            if (cut2 == std::string::npos) {
                LOG_WARNING("invalid line {}::{}: a line should contain 3 parts (NAME,TYPE,KEY)", path.string(), idx);
                continue;
            }

            char* file{ line.data() };
            file[cut1] = 0;
            file[cut2] = 0;

            char* typeStr{ &file[cut1 + 1] };
            char* key{ &file[cut2 + 1] };

            // LOG_DEBUG("add aes: {}/{}/{} ({}/{})", file, typeStr, key, cut1, cut2);

            KeyVersion type{ GetKeyVersionByName(typeStr) };

            if (!type) {
                LOG_WARNING("invalid line {}::{}: invalid aes key type: {}", path.string(), idx, typeStr);
                continue;
            }

            std::string dec{ base64_decode(key) };
            if (dec.size() != sizeof(AesKeyLocal::key)) {
                LOG_WARNING("invalid line {}::{}: invalid aes key len", path.string(), idx, typeStr);
                continue;
            }

            AesKeyLocal* newKey{ alloc.Alloc<AesKeyLocal>() };
            newKey->fileName = alloc.CloneStr(file);
            newKey->version = type;
            std::memcpy(newKey->key, dec.data(), sizeof(newKey->key));
            GetKeys().push_back(newKey);
            added++;
        }

        if (added) {
            LOG_TRACE("loaded {} AES keys", added);
        }
    }
    void LoadRsaKeys(const std::filesystem::path& path) {
        utils::InFileCE is{ path };
        if (!is) {
            return; // no file
        }

        std::string line;
        size_t idx{};
        size_t added{};
        while (*is && std::getline(*is, line)) {
            idx++;
            if (line.empty() || line[0] == '#') {
                continue;
            }

            size_t cut1{ line.find(',') };
            size_t cut2{ cut1 != std::string::npos ? line.find(',', cut1 + 1) : cut1 };

            if (cut2 == std::string::npos) {
                LOG_WARNING("invalid line {}::{}: a line should contain 3 parts (NAME,TYPE,KEY)", path.string(), idx);
                continue;
            }

            char* file{ line.data() };
            file[cut1] = 0;
            file[cut2] = 0;

            char* typeStr{ &file[cut1 + 1] };
            char* key{ &file[cut2 + 1] };

            KeyType type{ GetKeyTypeByName(typeStr) };

            if (!type) {
                LOG_WARNING("invalid line {}::{}: invalid rsa key type: {}", path.string(), idx, typeStr);
                continue;
            }

            std::string dec{ base64_decode(key) };
            if (dec.size() > sizeof(RsaKeyLocal::key)) {
                LOG_WARNING("invalid line {}::{}: rsa key too big", path.string(), idx, typeStr);
                continue;
            }

            RsaKeyLocal* newKey{ alloc.Alloc<RsaKeyLocal>() };
            newKey->id = alloc.CloneStr(file);
            newKey->type = type;
            newKey->len = dec.size();
            std::memcpy(newKey->key, dec.data(), dec.size());
            GetRsaKeys().push_back(newKey);
            added++;
        }

        if (added) {
            LOG_TRACE("loaded {} RSA keys", added);
        }
    }

    namespace {
        int crypto_create_rsa(int argc, const char* argv[]) {
            if (tool::NotEnoughParam(argc, 2)) {
                return tool::BAD_USAGE;
            }
            int prng{ register_prng(&sprng_desc) };
            if (prng < 0) {
                LOG_ERROR("Can't register prng");
                return tool::BASIC_ERROR;
            }

            const char* keyName{ argv[2] };
            KeyType type{ GetKeyTypeByName(argv[3]) };
            if (!type) {
                LOG_ERROR("Invalid key type: '{}'", argv[3]);
                return tool::BAD_USAGE;
            }

            rsa_key key{};

            ltc_mp = ltm_desc;
            int err{ rsa_make_key(nullptr, prng, 0x100, 65537, &key) };

            if (err != CRYPT_OK) {
                LOG_ERROR("Can't make rsa key: {}", error_to_string(err));
                return tool::BASIC_ERROR;
            }

            byte buff[0x1000];
            unsigned long buffLen;

            buffLen = sizeof(buff);
            if ((err = rsa_export(buff, &buffLen, type == KEYT_PUBLIC ? PK_PUBLIC : PK_PRIVATE, &key)) != CRYPT_OK) {
                throw std::runtime_error(std::format("Can't export rsa key: {}", error_to_string(err)));
            }

            LOG_INFO("{},{},{}", keyName, type == KEYT_PUBLIC ? "PUBLIC" : "PRIVATE", base64_encode(buff, buffLen));

            return tool::OK;
        }

        int crypto_write_aes(int argc, const char* argv[]) {
            if (tool::NotEnoughParam(argc, 1)) {
                return tool::BAD_USAGE;
            }
            std::filesystem::path path{ argv[2] };

            utils::OutFileCE os{ path, true };

            for (AesKeyLocal* key : GetKeys()) {
                os << key->fileName << "," << GetKeyVersionName(key->version) << ","
                   << base64_encode(key->key, sizeof(key->key)) << "\n";
            }

            LOG_INFO("dump into {}", argv[2]);

            return tool::OK;
        }

        int crypto_write_rsa(int argc, const char* argv[]) {
            if (tool::NotEnoughParam(argc, 1)) {
                return tool::BAD_USAGE;
            }
            std::filesystem::path path{ argv[2] };

            utils::OutFileCE os{ path, true };

            for (RsaKeyLocal* key : GetRsaKeys()) {
                os << key->id << "," << GetKeyTypeName(key->type) << "," << base64_encode(key->key, key->len) << "\n";
            }

            LOG_INFO("dump into {}", argv[2]);

            return tool::OK;
        }
    } // namespace

    ADD_TOOL(crypto_create_rsa, "compatibility", " [name] [type]", "create rsa key", crypto_create_rsa);
    ADD_TOOL(crypto_write_rsa, "compatibility", " [out]", "write rsa keys file", crypto_write_rsa);
    ADD_TOOL(crypto_write_aes, "compatibility", " [out]", "write aes keys file", crypto_write_aes);
} // namespace compatibility::acti::crypto_keys