#pragma once
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/fastfile_data_tre.hpp>
#include <tools/fastfile/fastfile_flexible.hpp>
#include <tools/fastfile/linkers/linker_cw.hpp>
#include <tools/compatibility/scobalula_wnigen.hpp>
#include <tools/compatibility/acti_crypto_keys.hpp>
#include <tools/compatibility/acti_archive_checksums.hpp>
#include <utils/data_utils.hpp>

namespace fastfile::compressor::t789 {
    using namespace compatibility::acti::crypto_keys;

    enum T789Version {
        T789V_FLAG_CW = 0x1000,

        T789V_BO3 = 0x251,
        T789V_BO4 = 0x27F,
        T789V_CW = 0x64 | T789V_FLAG_CW,
    };

    struct FFCompressorSecureInfo {
        AesKeyLocal* aesKey{};
        uint8_t aesIV[16]{};
        uint8_t aesVal[16]{};
    };

    template<T789Version version, size_t compressionMax, KeyVersion keyVersion, typename HeaderWriterData = void*>
    class AbstractCompressorT789 : public FFCompressor {
        static constexpr uint32_t READ_SEGMENT = 0x800000;
        static constexpr uint32_t ZLIB_STORED_OVERHEAD = 11;
        static constexpr uint32_t MIN_FILLER_GAP =
            (uint32_t)sizeof(fastfile::DBStreamHeader) + ZLIB_STORED_OVERHEAD + 4;
        static constexpr uint32_t SAFE_MARGIN = MIN_FILLER_GAP;

      public:
        using FFCompressor::FFCompressor;
        virtual ~AbstractCompressorT789() {}

        void Init(FastFileLinkerOption& opt) override {
            constexpr size_t maxSize{ utils::GetMaxSize<int32_t>() };
            if (opt.chunkSize > maxSize) {
                LOG_WARNING("Chunk size can't be above {}", maxSize);
                opt.chunkSize = maxSize;
            }
        }

        virtual void
        AllocateHeader(FastFileLinkerContext& ctx, std::vector<byte>& output, HeaderWriterData& header) = 0;
        virtual void WriteHeaderData(
            FastFileLinkerContext& ctx, std::vector<byte>& output, fastfile::FastFile& ff,
            FastFileCompression compression, FFCompressorSecureInfo& secure, HeaderWriterData& header
        ) = 0;

        void Compress(FastFileLinkerContext& ctx) override {
            // bo4/cw seems constantly use oodle,
            // bo3 zlib, but DB_DecompressIOStream can handle everything
            FastFileCompression compression{ FastFileCompression::XFILE_UNCOMPRESSED };
            const char* compressionType{ ctx.zone.GetConfig("compression") };
            if (compressionType) {
                compression = GetFastFileCompression(compressionType);

                if (compression >= compressionMax) {
                    throw std::runtime_error(std::format("Invalid compression format name: {}", compressionType));
                }
            }

            utils::compress::CompressionAlgorithm alg{ fastfile::GetFastFileCompressionAlgorithm(compression) };
            if (ctx.zone.GetConfigBool("compression.high", false)) {
                alg = alg | utils::compress::COMP_HIGH_COMPRESSION;
            }
            uint32_t chunkSize = (uint32_t)(ctx.opt.chunkSize ? ctx.opt.chunkSize : 0x3fee0);

            int aesCipher{};

            if (ctx.opt.encrypt) {
                if ((aesCipher = find_cipher("aes")) == -1) {
                    throw std::runtime_error("Missing aes cipher");
                }
            }

            for (fastfile::FastFile& ff : ctx.fastfiles) {
                FFCompressorSecureInfo secure{};

                if (ctx.opt.encrypt) {
                    secure.aesKey = GetKeyByName(ff.ffname, keyVersion);

                    if (!secure.aesKey) {
                        LOG_WARNING("Missing {} AES key for file {}, it won't be encrypted", name, ff.ffname);
                    } else {
                        utils::data::FillRandomBuffer(secure.aesIV, sizeof(secure.aesIV));
                    }
                }
                // clone iv for later
                std::memcpy(secure.aesVal, secure.aesIV, sizeof(secure.aesVal));

                std::vector<byte> out{};
                HeaderWriterData header{};
                AllocateHeader(ctx, out, header);

                // write blocks
                byte* toCompress{ ff.linkedData.data() };
                size_t remainingSize{ ff.linkedData.size() };

                size_t idx{};
                std::vector<byte> compressBuffer{};
                size_t compressedSize{};
                LOG_TRACE(
                    "start compressing chunk 0x{:x} byte(s) using {} with chunks of size 0x{:x}...",
                    remainingSize,
                    alg,
                    chunkSize
                );

                while (remainingSize > 0) {
                    uint32_t uncompressedSize{ (uint32_t)std::min<size_t>(chunkSize, remainingSize) };
                    uint32_t chunkStart{ (uint32_t)out.size() };
                    uint32_t boundary{ ((chunkStart / READ_SEGMENT) + 1) * READ_SEGMENT };

                    bool fits{ false };
                    for (;;) {
                        compressBuffer.clear();
                        if (!utils::compress::CompressBuffer(alg, toCompress, uncompressedSize, compressBuffer)) {
                            throw std::runtime_error(
                                std::format("Can't compress chunk 0x{:x} of size 0x{:x}", idx, uncompressedSize)
                            );
                        }
                        uint32_t alignedSize{ utils::Aligned<uint32_t>((uint32_t)compressBuffer.size()) };
                        uint32_t chunkEnd{ chunkStart + (uint32_t)sizeof(fastfile::DBStreamHeader) + alignedSize };
                        uint32_t gapAfter{ chunkEnd <= boundary ? boundary - chunkEnd : 0 };

                        if (chunkEnd <= boundary && (gapAfter == 0 || gapAfter >= SAFE_MARGIN)) {
                            fits = true;
                            break;
                        }
                        if (uncompressedSize <= 1) {
                            fits = false; // give up shrinking and don't write this attempt at all
                            break;
                        }
                        uncompressedSize = (uncompressedSize + 1) / 2;
                    }

                    if (!fits) {
                        // close the entire remaining gap
                        uint32_t gap{ boundary - chunkStart };
                        if (gap < MIN_FILLER_GAP) {
                            throw std::runtime_error(
                                std::format("Dead-zone gap 0x{:x} too small to close at 0x{:x}", gap, chunkStart)
                            );
                        }
                        uint32_t targetAligned{ gap - (uint32_t)sizeof(fastfile::DBStreamHeader) };
                        uint32_t fillerInput{ targetAligned - ZLIB_STORED_OVERHEAD };

                        size_t fillerCompressedSize{};
                        std::unique_ptr<byte[]> fillerCompressed{ utils::compress::Compress(
                            utils::compress::COMP_ZLIB | utils::compress::COMP_STORED,
                            toCompress,
                            fillerInput,
                            &fillerCompressedSize
                        ) };
                        if (!fillerCompressed) {
                            throw std::runtime_error("Can't build boundary filler chunk");
                        }

                        uint32_t fillerOffset{
                            (uint32_t)utils::Allocate(out, sizeof(fastfile::DBStreamHeader) + targetAligned)
                        };
                        fastfile::DBStreamHeader& fh{ *(fastfile::DBStreamHeader*)&out[fillerOffset] };
                        fh.offset = fillerOffset;
                        fh.uncompressedSize = fillerInput;
                        fh.compressedSize = (uint32_t)fillerCompressedSize;
                        fh.alignedSize = targetAligned;

                        byte* fillerData{ fillerCompressed.get() };
                        if (secure.aesKey) {
                            symmetric_CTR ctr{};
                            int r;
                            if ((r = ctr_start(
                                     aesCipher,
                                     secure.aesVal,
                                     secure.aesKey->key,
                                     sizeof(secure.aesKey->key),
                                     0,
                                     0,
                                     &ctr
                                 )) != CRYPT_OK) {
                                throw std::runtime_error(
                                    std::format("Failed to start ctr {} for ff {}", error_to_string(r), ff.ffname)
                                );
                            }
                            if ((r = ctr_encrypt(fillerData, fillerData, fh.compressedSize, &ctr)) != CRYPT_OK) {
                                throw std::runtime_error(
                                    std::format("Can't encrypt filler block 0x{:x}: {}", idx, error_to_string(r))
                                );
                            }
                            *((uint64_t*)&secure.aesVal[0]) += fh.compressedSize;
                        }

                        std::memcpy(
                            &out[fillerOffset + sizeof(fastfile::DBStreamHeader)],
                            fillerData,
                            fh.compressedSize
                        );

                        toCompress += fillerInput;
                        remainingSize -= fillerInput;

                        LOG_TRACE(
                            "filler chunk {} @ offset 0x{:x}: exact-fit stored zlib, {} src bytes, closes boundary "
                            "0x{:x} exactly",
                            idx,
                            fillerOffset,
                            fillerInput,
                            boundary
                        );
                        idx++;
                        continue;
                    }

                    uint32_t alignedSize{ utils::Aligned<uint32_t>((uint32_t)compressBuffer.size()) };
                    compressedSize += compressBuffer.size();

                    uint32_t blockOffset{
                        (uint32_t)utils::Allocate(out, sizeof(fastfile::DBStreamHeader) + alignedSize)
                    };

                    fastfile::DBStreamHeader& h{ *(fastfile::DBStreamHeader*)&out[blockOffset] };

                    h.offset = blockOffset;
                    h.uncompressedSize = uncompressedSize;
                    h.compressedSize = (uint32_t)compressBuffer.size();
                    h.alignedSize = (uint32_t)alignedSize;

                    byte* compressedData{ compressBuffer.data() };
                    if (secure.aesKey) {
                        symmetric_CTR ctr{};
                        int r;
                        if ((r = ctr_start(
                                 aesCipher,
                                 secure.aesVal,
                                 secure.aesKey->key,
                                 sizeof(secure.aesKey->key),
                                 0,
                                 0,
                                 &ctr
                             )) != CRYPT_OK) {
                            throw std::runtime_error(
                                std::format("Failed to start ctr {} for ff {}", error_to_string(r), ff.ffname)
                            );
                        }
                        if ((r = ctr_encrypt(compressedData, compressedData, h.compressedSize, &ctr)) != CRYPT_OK) {
                            throw std::runtime_error(
                                std::format("Can't encrypt block 0x{:x}: {}", idx, error_to_string(r))
                            );
                        }
                        *((uint64_t*)&secure.aesVal[0]) += h.compressedSize;
                    }

                    // write compressed chunk
                    std::memcpy(&out[blockOffset + sizeof(fastfile::DBStreamHeader)], compressedData, h.compressedSize);

                    // move to the next buffer
                    toCompress += uncompressedSize;
                    remainingSize -= uncompressedSize;

                    LOG_TRACE(
                        "Compressed 0x{:x}->0x{:x} at chunk 0x{:x}, writing to: 0x{:x}, remaining 0x{:x}",
                        uncompressedSize,
                        h.compressedSize,
                        idx,
                        blockOffset,
                        remainingSize
                    );

                    idx++;
                }

                // end stream header
                uint32_t endBlockOffset{ (uint32_t)utils::Allocate(out, sizeof(fastfile::DBStreamHeader)) };
                fastfile::DBStreamHeader& h{ *(fastfile::DBStreamHeader*)&out[endBlockOffset] };
                h.offset = endBlockOffset;
                h.uncompressedSize = 0;
                h.alignedSize = 0;
                h.compressedSize = 0;
                utils::Allocate(out, 0x40);

                // write header data
                WriteHeaderData(ctx, out, ff, compression, secure, header);

                std::filesystem::path outputFileFF{ ctx.opt.m_output / "zone" / std::format("{}.ff", ff.ffname) };

                std::filesystem::create_directories(outputFileFF.parent_path());
                if (!utils::WriteFile(outputFileFF, out)) {
                    throw std::runtime_error(std::format("Can't write into {}", outputFileFF.string()));
                }

                if (ctx.opt.m_fd) {
                    LOG_WARNING(".fd file generator not implemented");
                }

                LOG_INFO(
                    "Compressed {} into {} [{}]({} -> {} bytes / {}% saved)",
                    ff.ffname,
                    outputFileFF.string(),
                    alg,
                    ff.linkedData.size(),
                    compressedSize,
                    (100 - 100 * compressedSize / ff.linkedData.size())
                );
            }

            if (!ctx.storedHashes.empty()) {
                std::filesystem::path outputFileWNI{ ctx.opt.m_output / "package_index" /
                                                     std::format("{}.wni", ctx.mainFFName) };
                std::filesystem::create_directories(outputFileWNI.parent_path());

                if (compatibility::scobalula::wnigen::CompressWNIFile(ctx.storedHashes, outputFileWNI) != tool::OK) {
                    throw std::runtime_error(std::format("Can't write wni info into {}", outputFileWNI.string()));
                }
            }
        }
    };

} // namespace fastfile::compressor::t789