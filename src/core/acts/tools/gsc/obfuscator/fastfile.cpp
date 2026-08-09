#include <includes.hpp>
#include <tools/gsc/obfuscator/fastfile.hpp>
#include <core/bytebuffer.hpp>
#include <utils/utils.hpp>
#include <utils/data_utils.hpp>
#include <utils/compress_utils.hpp>

namespace tool::gsc::obfuscator::fastfile {
    constexpr uint32_t FF_T7_VERSION = 0x251;
    constexpr uint64_t FF_T7_MAGIC = 0x3030303066664154;

    using FastFileCompression = ::fastfile::FastFileCompression;

    utils::compress::CompressionAlgorithm GetFastFileCompressionAlgorithm(fastfile::FastFileCompression comp) {
        switch (comp) {
        case ::fastfile::XFILE_UNCOMPRESSED:
            return utils::compress::COMP_NONE;
        case ::fastfile::XFILE_ZLIB:
            return utils::compress::COMP_ZLIB;
        case ::fastfile::XFILE_ZLIB_HC:
            return utils::compress::COMP_ZLIB | utils::compress::COMP_HIGH_COMPRESSION;
        case ::fastfile::XFILE_LZ4:
            return utils::compress::COMP_LZ4;
        case ::fastfile::XFILE_LZ4_HC:
            return utils::compress::COMP_LZ4 | utils::compress::COMP_HIGH_COMPRESSION;
        case ::fastfile::XFILE_OODLE_KRAKEN:
            return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_KRAKEN;
        case ::fastfile::XFILE_OODLE_MERMAID:
            return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_MERMAID;
        case ::fastfile::XFILE_OODLE_SELKIE:
            return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_SELKIE;
        case ::fastfile::XFILE_OODLE_LZNA:
            return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_LZNA;
        default:
            throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)comp));
        }
    }

    FastfileInfo DecompressFastfile(void* file, size_t fileSize) {
        core::bytebuffer::ByteBuffer reader{ (byte*)file, fileSize };

        if (!reader.CanRead(sizeof(::fastfile::XFileBO3))) {
            throw std::runtime_error("Invalid fastfile size");
        }

        uint64_t magic{ *reader.Ptr<uint64_t>() };
        if (magic != fastfile::FF_T7_MAGIC) {
            throw std::runtime_error(std::format("Invalid magic 0x{:x}", magic));
        }

        FastfileInfo info{};
        reader.Read(&info.header, sizeof(info.header));

        if (info.header.platform != ::fastfile::XFILE_PC) {
            throw std::runtime_error("Platform not yet supported");
        }
        if (info.header.encrypted) {
            throw std::runtime_error("Encrypted fastfile not yet supported");
        }
        if (info.header.compression >= ::fastfile::XFILE_BDELTA_UNCOMP &&
            info.header.compression <= ::fastfile::XFILE_BDELTA_LZMA) {
            throw std::runtime_error("Delta fastfile"); // wtf
        }

        if (!utils::data::IsNulled(info.header.signature, sizeof(info.header.signature))) {
            LOG_WARNING("RSA signature found, this signature will be erased from the result fastfile!");
            std::memset(info.header.signature, 0, sizeof(info.header.signature));
        }

        info.out.resize(info.header.size);
        size_t idx{};
        size_t offset{};
        size_t decompressedSize{ info.header.size };
        utils::compress::CompressionAlgorithm alg{ fastfile::GetFastFileCompressionAlgorithm(info.header.compression) };

        LOG_TRACE("decompressing {} ff {} with size 0x{:x}", alg, info.header.fastfileName, decompressedSize);

        while (offset < decompressedSize) {
            size_t loc{ reader.Loc() };

            ::fastfile::DBStreamHeader* block{ reader.ReadPtr<::fastfile::DBStreamHeader>() };

            if (block->offset != loc) {
                throw std::runtime_error(std::format("bad block position: 0x{:x} != 0x{:x}", loc, block->offset));
            }
            size_t blockId{ idx++ };

            if (!block->uncompressedSize) {
                reader.Align(0x800000);
                continue;
            }

            if (info.maxBlockSize < block->uncompressedSize) {
                info.maxBlockSize = block->uncompressedSize; // used for recompression
            }

            byte* blockBuff{ reader.ReadPtr<byte>(block->alignedSize) };

            LOG_TRACE(
                "Decompressing {}{} block 0x{:x} (0x{:x}/0x{:x} -> 0x{:x})",
                info.header.encrypted ? "encrypted " : "",
                alg,
                loc,
                block->compressedSize,
                block->alignedSize,
                block->uncompressedSize
            );

            byte* decompressed{ &info.out[offset] };
            offset += block->uncompressedSize;

            if (!utils::compress::Decompress(
                    alg,
                    decompressed,
                    block->uncompressedSize,
                    blockBuff,
                    block->compressedSize
                )) {
                throw std::runtime_error(std::format("Can't decompress block 0x{:x}", loc));
            }
        }
        LOG_TRACE(
            "Decompressed 0x{:x} byte(s) from 0x{:x} block(s) with max block size 0x{:x}",
            info.out.size(),
            idx,
            info.maxBlockSize
        );

        return info;
    }
    void CompressFastfile(std::vector<byte>& out, FastfileInfo& info) {
        utils::compress::CompressionAlgorithm alg{ fastfile::GetFastFileCompressionAlgorithm(info.header.compression) };
        out.clear();
        // write header
        utils::WriteValue(out, &info.header, sizeof(info.header));

        std::vector<byte> compressBuffer{};
        size_t idx{};

        byte* toCompress{ info.out.data() };
        size_t remainingSize{ info.out.size() };

        while (remainingSize) {
            size_t blockDataLen{ std::min<size_t>(info.maxBlockSize, remainingSize) };
            size_t blockId{ idx++ };

            compressBuffer.clear();
            if (!utils::compress::CompressBuffer(alg, toCompress, blockDataLen, compressBuffer)) {
                throw std::runtime_error(
                    std::format("Can't compress chunk 0x{:x} of size 0x{:x}", blockId, blockDataLen)
                );
            }

            // TODO: add alignment for empty blocks

            ::fastfile::DBStreamHeader block;
            block.offset = (uint32_t)out.size();
            block.uncompressedSize = (uint32_t)blockDataLen;
            block.compressedSize = (uint32_t)compressBuffer.size();
            block.alignedSize = utils::Aligned<uint32_t>((uint32_t)compressBuffer.size());

            utils::WriteValue(out, &block, sizeof(block));
            utils::WriteValue(out, compressBuffer.data(), compressBuffer.size());
            utils::Aligned<uint32_t>(out);

            LOG_TRACE(
                "Compressed 0x{:x}->0x{:x} at chunk 0x{:x}, writing to: 0x{:x}, remaining 0x{:x}",
                block.uncompressedSize,
                block.compressedSize,
                idx,
                block.offset,
                remainingSize
            );

            toCompress += blockDataLen;
            remainingSize -= blockDataLen;
        }
    }
} // namespace tool::gsc::obfuscator::fastfile