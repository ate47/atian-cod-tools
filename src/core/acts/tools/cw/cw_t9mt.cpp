#include <includes.hpp>
#include <core/bytebuffer.hpp>
#include <utils/compress_utils.hpp>
#define LTC_NO_PROTOTYPES
#include <tomcrypt.h>
#include <sha256.h>
#include <utils/data_utils.hpp>

namespace {
    constexpr uint64_t CWPKG_MAGIC = 0x1474B504D5743;
    constexpr uint64_t CWPKG_END_MAGIC = 0x14749534D5743;

#pragma pack(push, 1)
    struct CWPKGHeader {
        uint64_t magic;
        uint64_t version;
        byte aesKey[0x10];
        byte iv[0xc];
        byte tag[0x10];
        uint64_t fileSize;
        uint64_t numFiles;
    };
    static_assert(sizeof(CWPKGHeader) == 0x4C);
#pragma pack(pop)

    void RegisterAES() {
        static bool done{};
        if (!done) {
            done = true;
            int r;
            if ((r = register_cipher(&aes_desc)) != CRYPT_OK) {
                throw std::runtime_error(std::format("Can't register aes_desc {}", error_to_string(r)));
            }
        }
    }

    static inline byte HexNibble(char c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        }
        if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
        }
        if (c >= 'A' && c <= 'F') {
            return c - 'A' + 10;
        }
        throw std::runtime_error("invalid hex digit");
    }

    std::vector<byte> FromHex(std::string_view hex) {
        if (hex.size() % 2 != 0) {
            throw std::runtime_error("hex length must be even");
        }

        std::vector<byte> out;
        out.resize(hex.size() / 2);

        byte* d{ out.data() };
        for (size_t i = 0; i < hex.size(); i += 2) {
            uint8_t hi = HexNibble(hex[i]);
            uint8_t lo = HexNibble(hex[i + 1]);
            *(d++) = (hi << 4) | lo;
        }

        return out;
    }

    int DecryptGCM(
        byte* key, size_t keyLen, byte* iv, size_t ivLen, byte* aad, size_t aadLen, byte* ct, size_t ctLen, byte* tag,
        size_t tagLen, byte* pt
    ) {
        int err;
        std::unique_ptr<gcm_state> gcm{ std::make_unique<gcm_state>() };
        gcm_state* pgcm{ gcm.get() };

        int aesCipher{ find_cipher("aes") };
        if (aesCipher == -1) {
            throw std::runtime_error("Missing aes");
        }

        if ((err = gcm_init(pgcm, aesCipher, key, keyLen)) != CRYPT_OK ||
            (err = gcm_add_iv(pgcm, iv, ivLen)) != CRYPT_OK) {
            LOG_ERROR("Can't init");
            return err;
        };

        // AAD
        if (aadLen > 0) {
            if ((err = gcm_add_aad(pgcm, aad, aadLen)) != CRYPT_OK) {
                LOG_ERROR("Can't add aad");
                return err;
            }
        }

        // decrypt
        if ((err = gcm_process(pgcm, pt, ctLen, ct, GCM_DECRYPT)) != CRYPT_OK) {
            LOG_ERROR("Can't progress");
            return err;
        }

        unsigned char ctag[16];
        unsigned long ctagLen{ (unsigned long)tagLen };
        if ((err = gcm_done(pgcm, ctag, &ctagLen)) != CRYPT_OK) {
            LOG_ERROR("Can't done");
            return err;
        }

        return CRYPT_OK;
    }

    void DeriveKey(std::vector<uint8_t>& launcher, const uint8_t (&blob)[16], uint8_t (&outkey)[32]) {
        SHA256 sha256{};

        sha256.add(launcher.data(), launcher.size());
        sha256.add(blob, sizeof(blob));
        sha256.getHash(outkey);
    }

    int cwmr(int argc, const char* argv[]) {
        if (tool::NotEnoughParam(argc, 2)) {
            return tool::BAD_USAGE;
        }
        RegisterAES();

        std::vector<std::filesystem::path> paths{};
        utils::GetFileRecurseExt(argv[2], paths, ".cwm\0");

        std::vector<uint8_t> launcherKey{ FromHex(argv[3]) };
        if (launcherKey.size() != 32) {
            LOG_ERROR("Invalid launcher key, should be 32 bytes");
            return tool::BASIC_ERROR;
        }

        for (const std::filesystem::path& path : paths) {
            LOG_INFO("reading {}", path.string());

            std::vector<byte> buff{ utils::ReadFile<std::vector<byte>>(path) };
            core::bytebuffer::ByteBuffer reader{ buff };

            if (!reader.CanRead(sizeof(CWPKGHeader))) {
                LOG_ERROR("Can't read CWPKGHeader: too small");
                continue;
            }

            CWPKGHeader& header{ *reader.ReadPtr<CWPKGHeader>() };

            if (header.magic != CWPKG_MAGIC || header.version != 1) {
                LOG_ERROR("Can't read CWPKGHeader: bad magic or version");
                continue;
            }

            byte* data{ reader.ReadPtr<byte>(header.fileSize) };

            if (reader.CanRead(1)) {
                // is signed
                byte* signature{ reader.ReadPtr<byte>(0x40) }; // ECDSA_P256
                uint64_t endMagic{ reader.Read<uint64_t>() };

                if (endMagic != CWPKG_END_MAGIC) {
                    LOG_ERROR(
                        "Can't read CWPKGHeader: bad end magic at {:x}/{:x}/{:x}",
                        reader.Loc(),
                        reader.Remaining(),
                        endMagic
                    );
                    continue;
                }
            }

            int r;
            uint8_t aesKey[32];
            DeriveKey(launcherKey, header.aesKey, aesKey);

            LOG_TRACE("derived key: {}", utils::data::AsHex(aesKey, sizeof(aesKey)));

            std::vector<byte> dec{};
            dec.resize(header.fileSize);

            if ((r = DecryptGCM(
                     aesKey,
                     sizeof(aesKey),
                     header.iv,
                     sizeof(header.iv), // iv
                     nullptr,
                     0, // aad
                     data,
                     header.fileSize, // ct
                     header.tag,
                     sizeof(header.tag), // tag
                     dec.data()
                 )) != CRYPT_OK) {
                LOG_ERROR("Failed to decrypt {}", error_to_string(r));
                continue;
            }

            std::filesystem::path out{ path };
            out.replace_extension();

            core::bytebuffer::ByteBuffer decryptedReader{ dec };

            for (size_t i = 0; i < header.numFiles; i++) {
                std::string fileName{ decryptedReader.ReadSizedString<uint16_t>() };
                uint32_t len{ decryptedReader.Read<uint32_t>() };
                byte* fileData{ decryptedReader.ReadPtr<byte>(len) };

                std::filesystem::path outSub{ out / fileName };
                std::filesystem::create_directories(outSub.parent_path());
                if (!utils::WriteFile(outSub, fileData, len)) {
                    LOG_ERROR("Failed to write {}", outSub.string());
                    continue;
                }
                LOG_INFO("{} / {} - dump {}", i + 1, header.numFiles, outSub.string());
            }

            LOG_INFO("Write {}", out.string());
        }

        return tool::OK;
    }

    ADD_TOOL(cwmr, "cw", " [file] [key]", "read cwm file", cwmr);
} // namespace