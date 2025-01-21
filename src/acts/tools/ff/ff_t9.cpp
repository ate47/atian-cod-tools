#include <includes.hpp>
#include <deps/oodle.hpp>
#include <tools/fastfile.hpp>
#include <tools/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>
#include <tools/utils/data_utils.hpp>
#include <tools/compatibility/acti_crypto_keys.hpp>
#include <wincrypt.h>

namespace {
	static deps::oodle::Oodle oodle{};
    struct XFileData {
        uint32_t version;
        bool server;
        byte compression;
        byte platform;
        bool encrypted;
        uint64_t unk8;
        uint64_t unk10;
        uint64_t unk18;
        uint32_t archiveChecksum[4];
        uint64_t unk30;
        uint64_t unk38;
        uint64_t unk40;
        uint64_t unk48;
        uint32_t unk50;
        byte linkingResult;
        char builder[36];
        byte data[879];
        char fastfileName[64];
        byte data2[0x110];
    };


	struct XFileMagic {
		byte magic[4];
		uint32_t blocks;
	};


    static byte ff_magic[4]{ 'T', 'A', 'F', 'F' };
	void DecryptHeader(core::bytebuffer::ByteBuffer& headerBuffer, void* to) {
        byte* data{ (byte*)to };
        __int64 v7; // rcx
        __int64 v8; // r8
        byte v9; // dl
        __int64 v10; // rax
        byte v11; // r9
        size_t v12; // r14d
        __int64 v14; // rbx
        byte* v15; // rcx
        uint32_t v17[2]; // [rsp+20h] [rbp-48h] BYREF
        XFileMagic magicData; // [rsp+28h] [rbp-40h] BYREF

        std::memset(data, 0, sizeof(XFileData));
        //read_func->stream_readskip(&magicData, 8i64, 0i64, &read_func->streamff);
        headerBuffer.Read(&magicData, sizeof(magicData));
        v7 = 0i64;
        v8 = 4i64;
        while (1) {
            v9 = magicData.magic[v7];
            v10 = v8;
            v11 = ff_magic[v7++];
            --v8;
            if (!v10) {
            LABEL_5:
                v12 = 0;
                if (!magicData.blocks) {
                    return;
                }
                while (1) {
                    //read_func->stream_readskip(v17, 8i64, 0i64, &read_func->streamff);
                    headerBuffer.Read(&v17[0], sizeof(v17));

                    v14 = 0i64;
                    if (v17[0] > 0x39CB44F1) {
                        switch (v17[0]) {
                        case 0x66C65056u:
                            v14 = 96i64;
                            if (v17[1] <= 0x60)
                                v14 = v17[1];
                            v15 = data + 368;
                            goto LABEL_44;
                        case 0x6AEBB196u:
                            v14 = 96i64;
                            if (v17[1] <= 0x60)
                                v14 = v17[1];
                            v15 = data + 464;
                            goto LABEL_44;
                        case 0x8578C004:
                            v14 = 264i64;
                            if (v17[1] <= 0x108)
                                v14 = v17[1];
                            v15 = data + 80;
                            goto LABEL_44;
                        case 0xC7708CDA:
                            v14 = 336i64;
                            if (v17[1] <= 0x150)
                                v14 = v17[1];
                            v15 = data + 1000;
                            goto LABEL_44;
                        }
                    }
                    else {
                        switch (v17[0]) {
                        case 0x39CB44F1u:
                            v14 = 4i64;
                            if (v17[1] <= 4)
                                v14 = v17[1];
                            v15 = data;
                            goto LABEL_44;
                        case 0xF992DFEu:
                            v14 = 24i64;
                            if (v17[1] <= 0x18)
                                v14 = v17[1];
                            v15 = data + 344;
                            goto LABEL_44;
                        case 0x1CE68F50u:
                            v14 = 440i64;
                            if (v17[1] <= 0x1B8)
                                v14 = v17[1];
                            v15 = data + 560;
                            goto LABEL_44;
                        case 0x2C2381CFu:
                            v14 = 5i64;
                            if (v17[1] <= 5)
                                v14 = v17[1];
                            v15 = data + 4;
                            goto LABEL_44;
                        case 0x37F9D612u:
                            v14 = 64i64;
                            if (v17[1] <= 0x40)
                                v14 = v17[1];
                            v15 = data + 16;
                        LABEL_44:
                            //read_func->stream_readskip(v15, v14, 0i64, &read_func->streamff);
                            headerBuffer.Read(v15, v14);
                            break;
                        }
                    }
                    if (v14 != v17[1]) {
                        //read_func->stream_readskip(0i64, v17[1] - v14, 1i64, &read_func->streamff);
                        headerBuffer.Skip(v17[1] - v14);
                    }
                    if (++v12 >= magicData.blocks) {
                        return;
                    }
                }
            }
            if (v9 != v11)
                break;
            if (!v9)
                goto LABEL_5;

        }
		throw std::runtime_error("Invalid fastfile magic");
	}


	class T9FFHandler : public fastfile::FFHandler {
	public:
		T9FFHandler() : fastfile::FFHandler("Black Ops Cold War", 0x46464154, fastfile::MASK32) {}

        void LoadFastFile(fastfile::FFAssetPool& pool, fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, const char* file) {
            if (!reader.CanRead(sizeof(XFileData))) {
                throw std::runtime_error("Can't read XFile header");
            }

            XFileMagic* header{ reader.Ptr<XFileMagic>() };

            size_t numBlocks{ header->blocks };

            // patch header B96310

            XFileData data{};
            DecryptHeader(reader, &data);

            //if (HAS_LOG_LEVEL(core::logs::LVL_DEBUG)) utils::data::WriteHex(std::cout, 0, &data, sizeof(data));
            LOG_DEBUG("{}", data.fastfileName);

            if (data.version != 0x64) {
                throw std::runtime_error(std::format("Fast file version not supported: 0x{:x}", data.version));
            }

            HCRYPTPROV hProv{};
            HCRYPTHASH hHash{};
            HCRYPTKEY hKey{};
            utils::CloseEnd hProvCE{ [hProv, hKey, hHash] {
                if (hProv) CryptReleaseContext(hProv, 0);
                if (hHash) CryptDestroyHash(hHash);
                if (hKey) CryptDestroyKey(hKey);
            } };

            if (data.encrypted) {
                compatibility::acti::crypto_keys::AesKeyLocal* key{ compatibility::acti::crypto_keys::GetKeyByName(data.fastfileName) };

                if (!key) {
                    throw std::runtime_error(std::format("Missing aes key for ff {}", data.fastfileName));
                }

                if (!CryptAcquireContextA(&hProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
                    throw std::runtime_error(std::format("Can't acquire AES context 0x{:x}", GetLastError()));
                }

                if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
                    throw std::runtime_error(std::format("Can't get SHA256 hash 0x{:x}", GetLastError()));
                }

                if (!CryptHashData(hHash, (BYTE*)key->key, sizeof(key->key), 0)) {
                    throw std::runtime_error(std::format("Can't hash key 0x{:x}", GetLastError()));
                }

                if (!CryptDeriveKey(hProv, CALG_AES_128, hHash, 0, &hKey)) {
                    throw std::runtime_error(std::format("Can't get key 0x{:x}", GetLastError()));
                }

                throw std::runtime_error(std::format("Can't read encrypted header: error {}", data.fastfileName));
            }


            //std::vector<byte>& ffdata{ pool.CreateMemoryBuffer() };

            std::vector<byte> ffdata{};
            size_t idx{};
            while (true) {
                size_t loc{ reader.Loc() };

                fastfile::DBStreamHeader* block{ reader.ReadPtr<fastfile::DBStreamHeader>() };
                if (!block->alignedSize) break; // end

                if (block->offset != loc) {
                    LOG_ERROR("bad block position: 0x{:x} != 0x{:x}", loc, block->offset);
                    break;
                }
                idx++;

                if (!block->uncompressedSize) {
                    reader.Align(0x800000);
                    continue;
                }

                byte* blockBuff{ reader.ReadPtr<byte>(block->alignedSize) };
                LOG_TRACE("Decompressing block 0x{:x} {}(0x{:x}/0x{:x} -> 0x{:x})", loc, data.encrypted ? "encrypted " : "", block->compressedSize, block->alignedSize, block->uncompressedSize);

                if (data.encrypted) {
                    DWORD comp{ block->compressedSize };
                    if (!CryptDecrypt(hKey, hHash, true, 0, blockBuff, &comp)) {
                        throw std::runtime_error(std::format("Can't read decrypt chunk 0x{:x}", GetLastError()));
                    }
                    continue;
                }


                size_t unloc{ utils::Allocate(ffdata, block->uncompressedSize) };
                byte* decompressed{ &ffdata[unloc] };

                switch (data.compression) {
                case fastfile::XFILE_UNCOMPRESSED:
                    if (block->uncompressedSize > block->compressedSize) {
                        throw std::runtime_error(std::format("Can't decompress block, decompressed size isn't big enough: 0x{:x} != 0x{:x}", block->compressedSize, block->uncompressedSize));
                    }
                    memcpy(decompressed, blockBuff, block->uncompressedSize);
                    break;
                case fastfile::XFILE_ZLIB:
                case fastfile::XFILE_ZLIB_HC: {

                    uLongf sizef = (uLongf)block->uncompressedSize;
                    uLongf sizef2{ (uLongf)block->compressedSize };
                    int ret;
                    if (ret = uncompress2(decompressed, &sizef, reader.Ptr<const Bytef>(), &sizef2) < 0) {
                        throw std::runtime_error(std::format("error when decompressing {}", zError(ret)));
                    }
                    break;
                }
                case fastfile::XFILE_OODLE_KRAKEN:
                case fastfile::XFILE_OODLE_MERMAID:
                case fastfile::XFILE_OODLE_SELKIE:
                case fastfile::XFILE_OODLE_LZNA: {
                    if (!oodle && !oodle.LoadOodle(deps::oodle::OO2CORE_6)) {
                        throw std::runtime_error("Oodle is required to read this fastfile, did you put oo2core_6_win64.dll inside the deps directory?");
                    }
                    int ret{ oodle.Decompress(blockBuff, block->compressedSize, decompressed, block->uncompressedSize, deps::oodle::OODLE_FS_YES) };

                    if (ret != block->uncompressedSize) {
                        throw std::runtime_error(std::format("Can't decompress block, returned size isn't the expected one: 0x{:x} != 0x{:x}", ret, block->uncompressedSize));
                    }
                    break;
                }
                default:
                    throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)data.compression));
                }
            }

            LOG_TRACE("Decompressed 0x{:x} byte(s) from 0x{:x} block(s)", ffdata.size(), idx);

            if (opt.dump_decompressed) {
                std::filesystem::path decfile{ file };

                decfile.replace_extension(".ff.dec");

                if (!utils::WriteFile(decfile, ffdata)) {
                    LOG_ERROR("Can't dump {}", decfile.string());
                }
                else {
                    LOG_INFO("Dump into {}", decfile.string());
                }
            }


            core::bytebuffer::ByteBuffer buff{ ffdata };

            uint64_t dbgMagic{ 0xA0D42444780 };
            uint64_t dbgMagicMask{ 0xFFFFFFFFFFFFF };
            if (buff.FindMasked(&dbgMagic, &dbgMagicMask, sizeof(dbgMagic)) != std::string::npos) {
                LOG_WARNING("FIND A DBG SPT");
            }



            // search gscobj
            {
                /*
                 The pattern is :
                 align<8>
                 ScriptParseTree {
                    u64 name;
                    u64 namepad?; // maybe not there
                    void* buffer = -1;
                    u32 size;
                    u32 unk;
                 }
                 align<32>
                 GSC_OBJ {
                    u64 magic = 0x??00a0d43534780
                 }

                */
                uint64_t magic{ 0x00a0d43534780 };
                uint64_t magicMask{ 0xFFFFFFFFFFFFF };

                size_t loc{};
                std::filesystem::path out{ "scriptparsetree_cod2020ff" };
                buff.Goto(0);
                byte* start{ buff.Ptr<byte>() };
                while (true) {
                    loc = buff.FindMasked((byte*)&magic, (byte*)&magicMask, sizeof(magic));
                    if (loc == std::string::npos) break;
                    struct T8GSCOBJ {
                        byte magic[8];
                        int32_t crc;
                        int32_t pad;
                        uint64_t name;
                    };

                    buff.Goto(loc);

                    if (!buff.CanRead(sizeof(T8GSCOBJ))) {
                        break; // can't read buffer
                    }

                    byte* sptCan{ buff.Ptr<byte>() - 0x18 }; // 0x18 is the minimum size to lookup

                    // we are 32 bytes aligned
                    T8GSCOBJ* obj{ buff.ReadPtr<T8GSCOBJ>() };

                    uint64_t name{ obj->name };

                    while (*(uint64_t*)sptCan != name) {
                        sptCan--;
                        if (start == sptCan) {
                            break;
                        }
                    }
                    if (start == sptCan) {
                        loc++;
                        continue;
                    }


                    uint64_t smagic{ *reinterpret_cast<uint64_t*>(obj) };

                    size_t size;
                    struct T8SPT {
                        uint64_t name;
                        uint64_t pad0;
                        uintptr_t buffer;
                        uint32_t size;
                        uint32_t pad02;
                    };
                    struct T8SPTOld {
                        uint64_t name;
                        uintptr_t buffer;
                        uint32_t size;
                        uint32_t pad02;
                    };

                    // Depending on how old the ff is, we might use a XHash of 0x10 or 8 bytes. The pointer
                    // to the buffer will constantly be -1 because it is not linked yet
                    if (((T8SPT*)sptCan)->buffer == 0xFFFFFFFFFFFFFFFF) {
                        size = ((T8SPT*)sptCan)->size;
                    }
                    else if (((T8SPTOld*)sptCan)->buffer == 0xFFFFFFFFFFFFFFFF) {
                        size = ((T8SPTOld*)sptCan)->size;
                    }
                    else {
                        LOG_ERROR("Can't get size 0x{:x} for loc 0x{:x}", smagic, loc);
                        loc++;
                        continue;
                    }

                    LOG_TRACE("gsc: 0x{:x} 0x{:x} 0x{:x}: {}", smagic, loc, size, hashutils::ExtractTmpScript(obj->name));


                    if (!buff.CanRead(size)) {
                        loc++;
                        LOG_ERROR("Bad size 0x{:x} 0x{:x} for loc 0x{:x}", smagic, size, loc);
                        continue;
                    }
                    buff.Skip(size);

                    std::filesystem::path outFile{ out / std::format("vm_{:x}/script_{:x}.gscc", smagic, obj->name) };
                    std::filesystem::create_directories(outFile.parent_path());

                    if (!utils::WriteFile(outFile, obj->magic, size)) {
                        LOG_ERROR("Can't write {}", outFile.string());
                    }
                    else {
                        LOG_INFO("Dump {}", outFile.string());
                    }

                    loc++;
                }

            }
        }
	};

	utils::ArrayAdder<T9FFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}