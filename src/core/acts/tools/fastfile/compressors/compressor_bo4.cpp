#include <includes.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/fastfile_data_tre.hpp>
#include <tools/fastfile/linkers/linker_bo4.hpp>
#include <tools/compatibility/scobalula_wnigen.hpp>
#include <tools/compatibility/acti_crypto_keys.hpp>
#include <utils/data_utils.hpp>

namespace {
    using namespace fastfile;
    using namespace compatibility::acti::crypto_keys;
    struct XFile {
        uint8_t magic[8];
        uint32_t version;
        uint8_t server;
        FastFileCompression compression;
        uint8_t platform;
        uint8_t encrypted;
        uint64_t timestamp;
        uint32_t changelist;
        uint32_t archiveChecksum[4];
        char builder[32];
        uint32_t metaVersion;
        char mergeFastfile[64];
        char missionFastFiles[16][64];
        uint64_t size;
        uint64_t externalSize;
        uint64_t memMappedOffset;
        uint64_t blockSize[fastfile::linker::bo4::XFILE_BLOCK_COUNT];
        uint64_t unk4f0;
        uint64_t unk4f8;
        uint64_t unk500;
        uint64_t unk508;
        uint64_t unk510s;
        uint64_t unk518s;
        uint64_t unk520pa;
        byte pad0[392];
        char unkSign[64];
        char fastfileName[64];
        uint8_t signature[256];
        uint8_t aesIV[16];
    };
    static_assert(sizeof(XFile) == 0x840);

    class FFCompressorBO4 : public FFCompressor {
      public:
        static constexpr uint32_t READ_SEGMENT = 0x800000;
        static constexpr uint32_t ZLIB_STORED_OVERHEAD = 11;
        static constexpr uint32_t MIN_FILLER_GAP =
            (uint32_t)sizeof(fastfile::DBStreamHeader) + ZLIB_STORED_OVERHEAD + 4;
        static constexpr uint32_t SAFE_MARGIN = MIN_FILLER_GAP;

        FFCompressorBO4() : FFCompressor("BO4", "Black ops 4 fast file compressor") {}

        void Init(FastFileLinkerOption& opt) override {
            constexpr size_t maxSize{ utils::GetMaxSize<int32_t>() };
            if (opt.chunkSize > maxSize) {
                LOG_WARNING("Chunk size can't be above {}", maxSize);
                opt.chunkSize = maxSize;
            }
        }

        void Compress(FastFileLinkerContext& ctx) override {
            // bo4 seems constantly use oodle, but DB_DecompressIOStream can handle everything
            FastFileCompression compression{ FastFileCompression::XFILE_UNCOMPRESSED };
            const char* compressionType{ ctx.zone.GetConfig("compression") };
            if (compressionType) {
                compression = GetFastFileCompression(compressionType);

                if (compression == FastFileCompression::XFILE_COMPRESSION_COUNT) {
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
                AesKeyLocal* aesKey{};
                uint8_t aesIV[16]{};
                uint8_t aesVal[16]{};

                if (ctx.opt.encrypt) {
                    aesKey = GetKeyByName(ff.ffname, KeyVersion::VER_BO4);

                    if (!aesKey) {
                        LOG_WARNING("Missing bo4 AES key for file {}, it won't be encrypted", ff.ffname);
                    } else {
                        utils::data::FillRandomBuffer(aesIV, sizeof(aesIV));
                    }
                }
                // clone iv for later
                std::memcpy(aesVal, aesIV, sizeof(aesVal));

                std::vector<byte> out{};
                utils::Allocate<XFile>(out);

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
                        if (aesKey) {
                            symmetric_CTR ctr{};
                            int r;
                            if ((r = ctr_start(aesCipher, aesVal, aesKey->key, sizeof(aesKey->key), 0, 0, &ctr)) !=
                                CRYPT_OK) {
                                throw std::runtime_error(
                                    std::format("Failed to start ctr {} for ff {}", error_to_string(r), ff.ffname)
                                );
                            }
                            if ((r = ctr_encrypt(fillerData, fillerData, fh.compressedSize, &ctr)) != CRYPT_OK) {
                                throw std::runtime_error(
                                    std::format("Can't encrypt filler block 0x{:x}: {}", idx, error_to_string(r))
                                );
                            }
                            *((uint64_t*)&aesVal[0]) += fh.compressedSize;
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
                    if (aesKey) {
                        symmetric_CTR ctr{};
                        int r;
                        if ((r = ctr_start(aesCipher, aesVal, aesKey->key, sizeof(aesKey->key), 0, 0, &ctr)) !=
                            CRYPT_OK) {
                            throw std::runtime_error(
                                std::format("Failed to start ctr {} for ff {}", error_to_string(r), ff.ffname)
                            );
                        }
                        if ((r = ctr_encrypt(compressedData, compressedData, h.compressedSize, &ctr)) != CRYPT_OK) {
                            throw std::runtime_error(
                                std::format("Can't encrypt block 0x{:x}: {}", idx, error_to_string(r))
                            );
                        }
                        *((uint64_t*)&aesVal[0]) += h.compressedSize;
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
                XFile& header{ *(XFile*)out.data() };
                *(uint64_t*)&header.magic[0] = 0x3030303066664154;
                header.version = 0x27F;
                header.platform = ctx.opt.platform;
                header.server = ctx.opt.server;
                header.timestamp = utils::GetTimestamp() / 1000;
                header.size = ff.linkedData.size();
                header.compression = compression;
                header.encrypted = aesKey != nullptr;
                std::memcpy(header.aesIV, aesIV, sizeof(aesIV));

                // header.unk4f0 = 0xFFFFFFFFF;
                // header.unk4f8 = 0xFFFFFFFFF;
                // header.unk500 = 0xFFFFFFFFF;
                // header.unk508 = 0xFFFFFFFFF;
                // header.unk510s = 0xFFFFFFFFF;
                // header.unk518s = 0xFFFFFFFFF;
                // header.unk520pa = 0xFFFFFFFFF;
                // std::memset(header.pad0, 0xff, sizeof(header.pad0));
                // static uint32_t archiveChecksum[4]{ 0x34FF23CB, 0xE4505D2, 0xB3C783A, 0x3208003D };
                static uint32_t archiveChecksum[4]{ 0xCF92ECF4, 0xA75D3F79, 0x2A550D25, 0xF927447B };
                static_assert(sizeof(archiveChecksum) == sizeof(header.archiveChecksum));
                std::memcpy(header.archiveChecksum, archiveChecksum, sizeof(archiveChecksum));

                platform::GetComputerInfoName(header.builder, sizeof(header.builder));
                snprintf(header.fastfileName, sizeof(header.fastfileName), "%s", ff.ffname);

                // blocks load data
                for (size_t i = 0; i < fastfile::linker::bo4::XFILE_BLOCK_COUNT; i++) {
                    header.blockSize[i] = (uint64_t)ff.blockSizes[i];
                }

                // todo: write other data

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

    utils::ArrayAdder<FFCompressorBO4, FFCompressor> impl{ GetCompressors() };
} // namespace