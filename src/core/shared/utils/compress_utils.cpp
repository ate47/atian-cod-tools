#include <includes_shared.hpp>
#if __has_include(<deps/oodle.hpp>)
#include <deps/oodle.hpp>
#define __ACTS_COMPRESS_HAS_OODLE
#endif
#if __has_include(<zlib.h>)
#include <zlib.h>
#define __ACTS_COMPRESS_HAS_ZLIB
#endif
#if __has_include(<lz4.h>)
#include <lz4.h>
#define __ACTS_COMPRESS_HAS_LZ4

#if __has_include(<lz4hc.h>)
#include <lz4hc.h>
#define __ACTS_COMPRESS_HAS_LZ4HC
#endif

#endif

#if __has_include(<zstd.h>)
#include <zstd.h>
#define __ACTS_COMPRESS_HAS_ZSTD
#endif

#include <utils/compress_utils.hpp>

namespace utils::compress {
    namespace {
        thread_local int lastoutput;

#ifdef __ACTS_COMPRESS_HAS_OODLE
        deps::oodle::OodleCompressor GetOodleCompressor(CompressionAlgorithm c) {
            if (GetCompressionType(c) != COMP_OODLE)
                throw std::runtime_error(actssec("Not an oodle compressor"));

            switch (GetOodleCompressionType(c)) {
            case COMP_OODLE_TYPE_KRAKEN:
                return deps::oodle::OODLE_COMP_KRAKEN;
            case COMP_OODLE_TYPE_LZH:
                return deps::oodle::OODLE_COMP_LZH;
            case COMP_OODLE_TYPE_LZH_LW:
                return deps::oodle::OODLE_COMP_LZH_LW;
            case COMP_OODLE_TYPE_LZNIB:
                return deps::oodle::OODLE_COMP_LZNIB;
            case COMP_OODLE_TYPE_NONE:
                return deps::oodle::OODLE_COMP_NONE;
            case COMP_OODLE_TYPE_LZB16:
                return deps::oodle::OODLE_COMP_LZB16;
            case COMP_OODLE_TYPE_LZBW:
                return deps::oodle::OODLE_COMP_LZBW;
            case COMP_OODLE_TYPE_LZA:
                return deps::oodle::OODLE_COMP_LZA;
            case COMP_OODLE_TYPE_LZNA:
                return deps::oodle::OODLE_COMP_LZNA;
            case COMP_OODLE_TYPE_MERMAID:
                return deps::oodle::OODLE_COMP_MERMAID;
            case COMP_OODLE_TYPE_BITKNIT:
                return deps::oodle::OODLE_COMP_BITKNIT;
            case COMP_OODLE_TYPE_SELKIE:
                return deps::oodle::OODLE_COMP_SELKIE;
            case COMP_OODLE_TYPE_HYDRA:
                return deps::oodle::OODLE_COMP_HYDRA;
            case COMP_OODLE_TYPE_LEVIATHAN:
                return deps::oodle::OODLE_COMP_LEVIATHAN;
            default:
                throw std::runtime_error(std::format("Unknown oodle compressor for {}", c));
            }
        }
#endif
    } // namespace

    int GetLastErr() { return lastoutput; }

    int Decompress2(CompressionAlgorithm alg, void* dest, size_t destSize, const void* src, size_t srcSize) {
        CompressionAlgorithm type{ GetCompressionType(alg) };
        switch (type) {
        case COMP_NONE:
            if (destSize < srcSize) {
                return DecompressResult::DCOMP_DEST_TOO_SMALL;
            }
            std::memcpy(dest, src, srcSize);
            return (int)srcSize;
#ifdef __ACTS_COMPRESS_HAS_LZ4
        case COMP_LZ4: {
            int r{ LZ4_decompress_safe((const char*)src, (char*)dest, (int)srcSize, (int)destSize) };
            lastoutput = r;
            if (r < 0) {
                LOG_ERROR("lz4 error: {}", r);
                return DecompressResult::DCOMP_UNKNOWN_ERROR;
            }
            return r;
        }
#endif
#ifdef __ACTS_COMPRESS_HAS_OODLE
        case COMP_OODLE: {
            deps::oodle::Oodle& oodle{ deps::oodle::GetInstance() };

            int r{ oodle.Decompress(src, (uint32_t)srcSize, dest, (uint32_t)destSize, deps::oodle::OODLE_FS_NO) };
            lastoutput = r;

            if (r <= 0) {
                return DecompressResult::DCOMP_UNKNOWN_ERROR;
            }
            return r;
        }
#endif
#ifdef __ACTS_COMPRESS_HAS_ZLIB
        case COMP_ZLIB:
        case COMP_ZLIB_DEFLATE:
        case COMP_ZLIB_GZIP: {
            z_stream zs{};

            int window{ GetZlibWindow(alg) };
            if ((lastoutput = inflateInit2(&zs, window)) != Z_OK) {
                LOG_ERROR("Can't init zstream {} with window {}", zs.msg ? zs.msg : "<err>", window);
                return DecompressResult::DCOMP_UNKNOWN_ERROR;
            }

            zs.next_in = (z_const Bytef*)src;
            zs.avail_in = (uInt)srcSize;
            zs.next_out = (Bytef*)dest;
            zs.avail_out = (uInt)destSize;

            lastoutput = inflate(&zs, Z_FULL_FLUSH);

            inflateEnd(&zs);

            switch (lastoutput) {
            case Z_OK:
            case Z_STREAM_END:
                return (int)(destSize - zs.avail_out);
            case Z_BUF_ERROR:
                return DecompressResult::DCOMP_DEST_TOO_SMALL;
            default:
                LOG_ERROR("Can't inflate zstream {} with window {}: {}", lastoutput, window, zs.msg ? zs.msg : "");
                return DecompressResult::DCOMP_UNKNOWN_ERROR;
            }
        }
#endif
#ifdef __ACTS_COMPRESS_HAS_ZSTD
        case COMP_ZSTD: {
            size_t ret{ ZSTD_decompress(dest, destSize, src, srcSize) };
            if (ZSTD_isError(ret)) {
                switch (ret) {
                case ZSTD_ErrorCode::ZSTD_error_dstSize_tooSmall:
                    return DecompressResult::DCOMP_DEST_TOO_SMALL;
                default:
                    LOG_ERROR("zstd error: {}", ZSTD_getErrorName(ret));
                    return DecompressResult::DCOMP_UNKNOWN_ERROR;
                }
            }
            return (int)ret;
        }
#endif
        default:
            throw std::runtime_error(std::format("Can't decompress {}", alg));
            return DecompressResult::DCOMP_BAD_ALGORITHM;
        }
    }

    int Decompress(
        CompressionAlgorithm alg, std::vector<byte>& outBuff, const void* src, size_t srcSize, float increaseFactor
    ) {
        if (alg == CompressionAlgorithm::COMP_NONE) {
            size_t len{ srcSize };
            outBuff.resize(len);

            return Decompress2(alg, outBuff.data(), outBuff.size(), src, srcSize);
        }

        if (!srcSize)
            return 0;

        size_t len{ srcSize };
        if (len == len * increaseFactor) {
            increaseFactor = 2; // too small, we use 2 as default
        }

        while (true) {
            len = (size_t)(len * increaseFactor);
            outBuff.resize(len);

            int r{ Decompress2(alg, outBuff.data(), outBuff.size(), src, srcSize) };

            if (r >= 0) {
                return r;
            }

            if (r != DecompressResult::DCOMP_DEST_TOO_SMALL) {
                return r;
            }
        }
    }

    std::vector<byte> Decompress(CompressionAlgorithm alg, const void* src, size_t srcSize, float increaseFactor) {
        std::vector<byte> outBuff{};
        int r{ Decompress(alg, outBuff, src, srcSize, increaseFactor) };
        if (r >= 0) {
            return outBuff;
        }
        throw std::runtime_error(std::format("Error when decompressing {}", r));
    }

    bool Compress(CompressionAlgorithm alg, void* dest, size_t* destSize, const void* src, size_t srcSize) {
        switch (GetCompressionType(alg)) {
        case COMP_NONE:
            if (*destSize < srcSize) {
                return false;
            }
            *destSize = srcSize;
            std::memcpy(dest, src, srcSize);
            return true;
#ifdef __ACTS_COMPRESS_HAS_OODLE
        case COMP_OODLE: {
            deps::oodle::Oodle& oodle{ deps::oodle::GetInstance() };
            deps::oodle::OodleCompressionLevel lvl{ alg & COMP_HIGH_COMPRESSION ? deps::oodle::OODLE_COMPL_OPTIMAL5
                                                                                : deps::oodle::OODLE_COMPL_NORMAL };
            int r{ oodle.Compress(GetOodleCompressor(alg), src, (int32_t)srcSize, dest, lvl) };

            if (r < 0)
                return false;
            *destSize = r;
            return true;
        }
#endif
#ifdef __ACTS_COMPRESS_HAS_ZLIB
        case COMP_ZLIB:
        case COMP_ZLIB_DEFLATE:
        case COMP_ZLIB_GZIP: {
            z_stream strm{};
            strm.next_in = (Bytef*)src;
            strm.avail_in = (uInt)srcSize;

            strm.next_out = (Bytef*)dest;
            strm.avail_out = (uInt)*destSize;

            int level{ (alg & COMP_STORED)             ? Z_NO_COMPRESSION
                       : (alg & COMP_HIGH_COMPRESSION) ? Z_BEST_COMPRESSION
                                                       : Z_BEST_SPEED };
            int windowBits{ GetZlibWindow(alg) };

            if (deflateInit2(&strm, level, Z_DEFLATED, windowBits, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
                return false;
            }
            int ret{ deflate(&strm, Z_FINISH) };

            if (ret != Z_STREAM_END) {
                deflateEnd(&strm);
                return false;
            }

            *destSize = *destSize - strm.avail_out;

            deflateEnd(&strm);
            return true;
        }
#endif
#ifdef __ACTS_COMPRESS_HAS_LZ4
        case COMP_LZ4:
#ifdef __ACTS_COMPRESS_HAS_LZ4HC
            if (alg & COMP_HIGH_COMPRESSION) {
                int r{ LZ4_compress_HC((const char*)src, (char*)dest, (int)srcSize, (int)*destSize, LZ4HC_CLEVEL_MAX) };
                if (r < 0)
                    return false;
                *destSize = r;
                return true;
            }
#endif
            {
                int r{ LZ4_compress_default((const char*)src, (char*)dest, (int)srcSize, (int)*destSize) };
                if (r < 0)
                    return false;
                *destSize = r;
                return true;
            }
#endif
#ifdef __ACTS_COMPRESS_HAS_ZSTD
        case COMP_ZSTD: {
            int lvl{ alg & COMP_HIGH_COMPRESSION ? ZSTD_maxCLevel() : ZSTD_defaultCLevel() };
            size_t r{ ZSTD_compress(dest, *destSize, src, srcSize, lvl) };
            if (ZSTD_isError(r))
                return false;
            *destSize = r;
            return true;
        }
#endif
        default:
            throw std::runtime_error(std::format("Can't compress alg {}", alg));
            break;
        }

        return false;
    }

    std::unique_ptr<byte[]> Compress(CompressionAlgorithm alg, const void* src, size_t srcSize, size_t* compressSize) {
        std::unique_ptr<byte[]> res{ std::make_unique<byte[]>(*compressSize = GetCompressSize(alg, srcSize)) };

        if (!Compress(alg, res.get(), compressSize, src, srcSize)) {
            throw std::runtime_error(std::format("Error when compressing {} data", alg));
        }

        return res;
    }

    size_t GetCompressSize(CompressionAlgorithm alg, size_t srcSize) {
        switch (GetCompressionType(alg)) {
        case COMP_NONE:
            return srcSize;
#ifdef __ACTS_COMPRESS_HAS_LZ4
        case COMP_LZ4:
            return LZ4_compressBound((int)srcSize);
#endif
#ifdef __ACTS_COMPRESS_HAS_OODLE
        case COMP_OODLE: {
            deps::oodle::Oodle& oodle{ deps::oodle::GetInstance() };
            return oodle.GetCompressedBufferSizeNeeded(GetOodleCompressor(alg), (int32_t)srcSize);
        }
#endif
#ifdef __ACTS_COMPRESS_HAS_ZLIB
        case COMP_ZLIB:
        case COMP_ZLIB_DEFLATE:
        case COMP_ZLIB_GZIP: {
            return compressBound((uLong)srcSize);
        }
#endif
#ifdef __ACTS_COMPRESS_HAS_ZSTD
        case COMP_ZSTD: {
            return ZSTD_compressBound(srcSize);
        }
#endif
        default:
            throw std::runtime_error(std::format("Can't get compress size for alg {}", alg));
        }
    }

    bool CompressBuffer(CompressionAlgorithm alg, const void* src, size_t srcSize, std::vector<byte>& out) {
        size_t allocSize{ GetCompressSize(alg, srcSize) };
        out.resize(allocSize);

        size_t outLen{ out.size() };
        if (!Compress(alg, out.data(), &outLen, src, srcSize)) {
            return false;
        }

        if (outLen != out.size()) {
            out.resize(outLen);
        }

        return true;
    }

    namespace {
#ifdef __ACTS_COMPRESS_HAS_ZLIB
        int DecompressZLib(std::vector<byte>& vec, const void* src, size_t srcSize, int windowBits) {
            z_stream strm{};

            int err;

            err = inflateInit2(&strm, windowBits);

            if (err != Z_OK) {
                throw std::runtime_error("Can't init zlib stream");
            }
            strm.next_in = (Bytef*)src;
            strm.avail_in = (uInt)srcSize;

            constexpr uInt chunkSize = 0x1000;
            byte chunk[chunkSize];

            do {
                strm.next_out = chunk;
                strm.avail_out = chunkSize;

                err = inflate(&strm, Z_NO_FLUSH);

                if (err == Z_STREAM_ERROR || err == Z_DATA_ERROR || err == Z_MEM_ERROR) {
                    inflateEnd(&strm);
                    return DecompressResult::DCOMP_UNKNOWN_ERROR;
                }

                vec.insert(vec.end(), chunk, &chunk[chunkSize - strm.avail_out]);
            } while (err != Z_STREAM_END);

            inflateEnd(&strm);

            if (err == Z_STREAM_END) {
                return (int)vec.size();
            }

            return DecompressResult::DCOMP_UNKNOWN_ERROR;
        }
#endif

#ifdef __ACTS_COMPRESS_HAS_LZ4
        int DecompressLZ4(std::vector<byte>& vec, const void* src, size_t srcSize) {
            LZ4_streamDecode_t strm{};

            LZ4_setStreamDecode(&strm, nullptr, 0);

            constexpr uInt chunkSize = 0x1000;
            byte chunk[chunkSize];

            while (true) {
                int decodedBytes{
                    LZ4_decompress_safe_continue(&strm, (const char*)src, (char*)chunk, (int)srcSize, chunkSize)
                };

                if (decodedBytes < 0) {
                    return DecompressResult::DCOMP_UNKNOWN_ERROR;
                }

                vec.insert(vec.end(), chunk, &chunk[decodedBytes]);

                if (!decodedBytes) {
                    return (int)vec.size();
                }
            }
        }
#endif

    } // namespace

    int DecompressAll(CompressionAlgorithm alg, std::vector<byte>& vec, const void* src, size_t srcSize) {
        switch (GetCompressionType(alg)) {
        case COMP_NONE:
            vec.resize(srcSize);
            std::memcpy(vec.data(), src, srcSize);
            return (int)srcSize;
#ifdef __ACTS_COMPRESS_HAS_ZLIB
        case COMP_ZLIB:
        case COMP_ZLIB_DEFLATE:
        case COMP_ZLIB_GZIP:
            return DecompressZLib(vec, src, srcSize, GetZlibWindow(alg));
#endif
#ifdef __ACTS_COMPRESS_HAS_LZ4
        case COMP_LZ4:
            return DecompressLZ4(vec, src, srcSize);
#endif

        default:
            throw std::runtime_error(std::format("DecompressAll not supported for {}", alg));
        }
    }

    template<size_t len>
    constexpr std::array<char, len + 1> GetAlgName(const char* name, const char* suffix) {
        std::array<char, len + 1> arr;
        size_t i{};
        while (*name) {
            arr[i++] = *(name++);
        }
        while (*suffix) {
            arr[i++] = *(suffix++);
        }

        arr[i] = 0;
        return arr;
    }

    constinit struct CompressionAlgorithmName {
        const char* name;
        CompressionAlgorithm alg;
        CompressionAlgorithm mask;
    } CompressionAlgorithmNames[]{

    // create one name per flag -> name / name_hc / name_stored
#define __COMP_ALG_NAME(name, alg, mask)                                                                               \
    { name, alg, mask | COMP_FLAGS_MASK }, { name "_hc", alg | COMP_HIGH_COMPRESSION, mask | COMP_FLAGS_MASK },        \
        { name "_stored", alg | COMP_STORED, mask | COMP_FLAGS_MASK }

        __COMP_ALG_NAME("none", COMP_NONE, COMP_TYPE_MASK),
        __COMP_ALG_NAME("zlib", COMP_ZLIB, COMP_TYPE_MASK),
        __COMP_ALG_NAME("deflate", COMP_ZLIB_DEFLATE, COMP_TYPE_MASK),
        __COMP_ALG_NAME("gzip", COMP_ZLIB_GZIP, COMP_TYPE_MASK),
        __COMP_ALG_NAME("lzma", COMP_LZMA, COMP_TYPE_MASK),
        __COMP_ALG_NAME("lz4", COMP_LZ4, COMP_TYPE_MASK),
        __COMP_ALG_NAME("oodle_kraken", COMP_OODLE | COMP_OODLE_TYPE_KRAKEN, COMP_TYPE_MASK | COMP_OODLE_TYPE_MASK),
        __COMP_ALG_NAME(
            "oodle_leviathan", COMP_OODLE | COMP_OODLE_TYPE_LEVIATHAN, COMP_TYPE_MASK | COMP_OODLE_TYPE_MASK
        ),
        __COMP_ALG_NAME("oodle_mermaid", COMP_OODLE | COMP_OODLE_TYPE_MERMAID, COMP_TYPE_MASK | COMP_OODLE_TYPE_MASK),
        __COMP_ALG_NAME("oodle_selkie", COMP_OODLE | COMP_OODLE_TYPE_SELKIE, COMP_TYPE_MASK | COMP_OODLE_TYPE_MASK),
        __COMP_ALG_NAME("oodle_hydra", COMP_OODLE | COMP_OODLE_TYPE_HYDRA, COMP_TYPE_MASK | COMP_OODLE_TYPE_MASK),
        __COMP_ALG_NAME(
            "oodle_unknown", COMP_OODLE | COMP_OODLE_TYPE_KRAKEN, COMP_TYPE_MASK
        ), // default case for bad oodles
        __COMP_ALG_NAME("zstd", COMP_ZSTD, COMP_TYPE_MASK),
#undef __COMP_ALG_NAME
    };

    const char* GetCompressionName(CompressionAlgorithm alg, const char* defaultValue) {
        for (const CompressionAlgorithmName& name : CompressionAlgorithmNames) {
            if ((name.alg == (alg & name.mask))) {
                return name.name;
            }
        }
        return defaultValue;
    }

    CompressionAlgorithm GetConfigName(const char* cfg) {
        if (!cfg || !*cfg) {
            return COMP_NONE;
        }
        for (const CompressionAlgorithmName& name : CompressionAlgorithmNames) {
            if (!_strcmpi(name.name, cfg)) {
                return name.alg;
            }
        }
        throw std::runtime_error(std::format("Invalid compression name {}", cfg));
    }

    const char* DecompressResultName(int res) {
        if (res >= 0)
            return "nothing";
        switch (res) {
        case DCOMP_BAD_ALGORITHM:
            return "bad algorithm";
        case DCOMP_DEST_TOO_SMALL:
            return "too small";
        case DCOMP_UNKNOWN_ERROR:
            return "unknown error";
        default:
            return "unhandled error";
        }
    }

    std::ostream& operator<<(std::ostream& out, CompressionAlgorithm alg) {
        const char* d{ GetCompressionName(alg) };
        if (d) {
            out << d;
        } else {
            out << "comp:" << (size_t)alg;
        }
        return out;
    }
} // namespace utils::compress