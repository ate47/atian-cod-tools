#include <includes.hpp>
#include <deps/oodle.hpp>
#include <tools/fastfile.hpp>
#include <tools/ff/fastfile_handlers.hpp>
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


	class T9FFDecompressor : public fastfile::FFDecompressor {
	public:
        T9FFDecompressor() : fastfile::FFDecompressor("Black Ops Cold War", 0x46464154, fastfile::MASK32) {}

        void LoadFastFile(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx, std::vector<byte>& ffdata) {
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
        }
	};

	utils::ArrayAdder<T9FFDecompressor, fastfile::FFDecompressor> arr{ fastfile::GetDecompressors() };
}