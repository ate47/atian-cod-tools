#include <includes_shared.hpp>
#include <deps/oodle.hpp>
#include <zlib.h>
#include <lz4.h>
#include <lz4hc.h>
#include <zstd.h>
#include "compress_utils.hpp"

namespace utils::compress {
	deps::oodle::OodleCompressor GetOodleCompressor(CompressionAlgorithm c) {
		if (GetCompressionType(c) != COMP_OODLE) throw std::runtime_error("Not an oodle compressor");

		switch (GetOodleCompressionType(c)) {
			case COMP_OODLE_TYPE_KRAKEN: return deps::oodle::OODLE_COMP_KRAKEN;
			case COMP_OODLE_TYPE_LZH: return deps::oodle::OODLE_COMP_LZH;
			case COMP_OODLE_TYPE_LZH_LW: return deps::oodle::OODLE_COMP_LZH_LW;
			case COMP_OODLE_TYPE_LZNIB: return deps::oodle::OODLE_COMP_LZNIB;
			case COMP_OODLE_TYPE_NONE: return deps::oodle::OODLE_COMP_NONE;
			case COMP_OODLE_TYPE_LZB16: return deps::oodle::OODLE_COMP_LZB16;
			case COMP_OODLE_TYPE_LZBW: return deps::oodle::OODLE_COMP_LZBW;
			case COMP_OODLE_TYPE_LZA: return deps::oodle::OODLE_COMP_LZA;
			case COMP_OODLE_TYPE_LZNA: return deps::oodle::OODLE_COMP_LZNA;
			case COMP_OODLE_TYPE_MERMAID: return deps::oodle::OODLE_COMP_MERMAID;
			case COMP_OODLE_TYPE_BITKNIT: return deps::oodle::OODLE_COMP_BITKNIT;
			case COMP_OODLE_TYPE_SELKIE: return deps::oodle::OODLE_COMP_SELKIE;
			case COMP_OODLE_TYPE_HYDRA: return deps::oodle::OODLE_COMP_HYDRA;
			case COMP_OODLE_TYPE_LEVIATHAN: return deps::oodle::OODLE_COMP_LEVIATHAN;
			default: 
				throw std::runtime_error(std::format("Unknown oodle compressor for {}", c));
		}
	}

	int Decompress2(CompressionAlgorithm alg, void* dest, size_t destSize, const void* src, size_t srcSize) {
		CompressionAlgorithm type{ GetCompressionType(alg) };
		switch (type) {
		case COMP_NONE:
			if (destSize < srcSize) {
				return DecompressResult::DCOMP_DEST_TOO_SMALL;
			}
			std::memcpy(dest, src, srcSize);
			return (int)srcSize;
		case COMP_LZ4: {
			int r{ LZ4_decompress_safe((const char*)src, (char*)dest, (int)srcSize, (int)destSize) };
			if (r < 0) {
				LOG_ERROR("lz4 error: {}", r);
				return DecompressResult::DCOMP_UNKNOWN_ERROR;
			}
			return r;
		}
		case COMP_OODLE: {
			deps::oodle::Oodle& oodle{ deps::oodle::GetInstance() };

			int r{ oodle.Decompress(src, (uint32_t)srcSize, dest, (uint32_t)destSize, deps::oodle::OODLE_FS_NO) };

			if (r <= 0) {
				return DecompressResult::DCOMP_UNKNOWN_ERROR;
			}
			return r;
		}
		case COMP_ZLIB: {
			uLongf sizef = (uLongf)destSize;
			uLongf sizef2{ (uLongf)srcSize };
			int r{ uncompress2((Bytef*)dest, &sizef, (const Bytef*)src, &sizef2) };
			switch (r) {
			case Z_OK:
				return sizef;
			case Z_BUF_ERROR:
				return DecompressResult::DCOMP_DEST_TOO_SMALL;
			default:
				return DecompressResult::DCOMP_UNKNOWN_ERROR;
			}
		}
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
		default:
			throw std::runtime_error(std::format("Can't decompress {}", alg));
			return DecompressResult::DCOMP_BAD_ALGORITHM;
		}
	}

	int Decompress(CompressionAlgorithm alg, std::vector<byte>& outBuff, const void* src, size_t srcSize, float increaseFactor) {
		if (alg == CompressionAlgorithm::COMP_NONE) {
			size_t len{ srcSize };
			outBuff.resize(len);

			return Decompress2(alg, outBuff.data(), outBuff.size(), src, srcSize);
		}

		if (!srcSize) return 0;

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
		case COMP_OODLE: {
			deps::oodle::Oodle& oodle{ deps::oodle::GetInstance() };
			deps::oodle::OodleCompressionLevel lvl{ alg & COMP_HIGH_COMPRESSION ? deps::oodle::OODLE_COMPL_OPTIMAL5 : deps::oodle::OODLE_COMPL_NORMAL };
			int r{ oodle.Compress(GetOodleCompressor(alg), src, (int32_t)srcSize, dest, lvl) };

			if (r < 0) return false;
			*destSize = r;
			return true;
		}
		case COMP_ZLIB: {
			uLongf destSizef = (uLongf)*destSize;
			int level{ alg & COMP_HIGH_COMPRESSION ? Z_BEST_COMPRESSION : Z_BEST_SPEED };
			if (compress2((Bytef*) dest, &destSizef, (const Bytef*)src, (uLongf)srcSize, level) != Z_OK) return false;
			*destSize = destSizef;
			return true;
		}
		case COMP_LZ4: {
			if (alg & COMP_HIGH_COMPRESSION) {
				int r{ LZ4_compress_HC((const char*)src, (char*)dest, (int)srcSize, (int)*destSize, LZ4HC_CLEVEL_MAX) };
				if (r < 0) return false;
				*destSize = r;
			}
			else {
				int r{ LZ4_compress_default((const char*)src, (char*)dest, (int)srcSize, (int)*destSize) };
				if (r < 0) return false;
				*destSize = r;
			}
			return true;
		}
		case COMP_ZSTD: {
			int lvl{ alg & COMP_HIGH_COMPRESSION ? ZSTD_maxCLevel() : ZSTD_defaultCLevel() };
			size_t r{ ZSTD_compress(dest, *destSize, src, srcSize, lvl) };
			if (ZSTD_isError(r)) return false;
			*destSize = r;
			return true;
		}
		default:
			throw std::runtime_error(std::format("Can't compress alg {}", alg));
			break;
		}

		return false;
	}

	size_t GetCompressSize(CompressionAlgorithm alg, size_t srcSize) {
		switch (GetCompressionType(alg)) {
		case COMP_NONE:
			return srcSize;
		case COMP_LZ4:
			return LZ4_compressBound((int)srcSize);
		case COMP_OODLE: {
			deps::oodle::Oodle& oodle{ deps::oodle::GetInstance() };
			return oodle.GetCompressedBufferSizeNeeded(GetOodleCompressor(alg), (int32_t)srcSize);
		}
		case COMP_ZLIB: {
			return compressBound((uLong)srcSize);
		}
		case COMP_ZSTD: {
			return ZSTD_compressBound(srcSize);
		}
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

	const char* GetCompressionName(CompressionAlgorithm alg, const char* defaultValue) {
		CompressionAlgorithm type{ alg & (COMP_TYPE_MASK | COMP_OODLE_TYPE_MASK) };
		bool hc{ (alg & COMP_HIGH_COMPRESSION) == 0 };
		switch (type) {
			case COMP_NONE: return "none";
			case COMP_ZLIB: return hc ? "zlib" : "zlib_hc";
			case COMP_LZMA: return hc ? "lzma" : "lzma_hc";
			case COMP_LZ4: return hc ? "lz4" : "lz4";
			case COMP_OODLE | COMP_OODLE_TYPE_KRAKEN: return hc ? "oodle_kraken" : "oodle_kraken_hc";
			case COMP_OODLE | COMP_OODLE_TYPE_LEVIATHAN: return hc ? "oodle_leviathan" : "oodle_leviathan_hc";
			case COMP_OODLE | COMP_OODLE_TYPE_MERMAID: return hc ? "oodle_mermaid" : "oodle_mermaid_hc";
			case COMP_OODLE | COMP_OODLE_TYPE_SELKIE: return hc ? "oodle_selkie" : "oodle_selkie_hc";
			case COMP_OODLE | COMP_OODLE_TYPE_HYDRA: return hc ? "oodle_hydra" : "oodle_hydra_hc";
			case COMP_ZSTD: return hc ? "zstd" : "zstd_hc";
			default: return defaultValue;
		}
	}

	CompressionAlgorithm GetConfigName(const char* cfg) {
		if (!cfg || !*cfg || !_strcmpi(cfg, "none")) return COMP_NONE;
		if (!_strcmpi(cfg, "zlib")) return COMP_ZLIB;
		if (!_strcmpi(cfg, "zlib_hc")) return COMP_ZLIB | COMP_HIGH_COMPRESSION;
		if (!_strcmpi(cfg, "lzma")) return COMP_LZMA;
		if (!_strcmpi(cfg, "lzma_hc")) return COMP_LZMA | COMP_HIGH_COMPRESSION;
		if (!_strcmpi(cfg, "lz4")) return COMP_LZ4;
		if (!_strcmpi(cfg, "lz4_hc")) return COMP_LZ4 | COMP_HIGH_COMPRESSION;
		if (!_strcmpi(cfg, "zstd")) return COMP_ZSTD;
		if (!_strcmpi(cfg, "zstd_hc")) return COMP_ZSTD | COMP_HIGH_COMPRESSION;
		if (!_strcmpi(cfg, "oodle")) return COMP_OODLE;
		if (!_strcmpi(cfg, "oodle_hc")) return COMP_OODLE;
		if (!_strcmpi(cfg, "oodle_kraken")) return COMP_OODLE | COMP_HIGH_COMPRESSION; 
		if (!_strcmpi(cfg, "oodle_kraken_hc")) return COMP_OODLE | COMP_OODLE_TYPE_KRAKEN | COMP_HIGH_COMPRESSION;
		if (!_strcmpi(cfg, "oodle_leviathan")) return COMP_OODLE | COMP_OODLE_TYPE_LEVIATHAN; 
		if (!_strcmpi(cfg, "oodle_leviathan_hc")) return COMP_OODLE | COMP_OODLE_TYPE_LEVIATHAN | COMP_HIGH_COMPRESSION;
		if (!_strcmpi(cfg, "oodle_mermaid")) return COMP_OODLE | COMP_OODLE_TYPE_MERMAID; 
		if (!_strcmpi(cfg, "oodle_mermaid_hc")) return COMP_OODLE | COMP_OODLE_TYPE_MERMAID | COMP_HIGH_COMPRESSION;
		if (!_strcmpi(cfg, "oodle_selkie")) return COMP_OODLE | COMP_OODLE_TYPE_SELKIE; 
		if (!_strcmpi(cfg, "oodle_selkie_hc")) return COMP_OODLE | COMP_OODLE_TYPE_SELKIE | COMP_HIGH_COMPRESSION;
		if (!_strcmpi(cfg, "oodle_hydra")) return COMP_OODLE | COMP_OODLE_TYPE_HYDRA; 
		if (!_strcmpi(cfg, "oodle_hydra_hc")) return COMP_OODLE | COMP_OODLE_TYPE_HYDRA | COMP_HIGH_COMPRESSION;
		throw std::runtime_error(std::format("Invalid compression name {}", cfg));
	}

	const char* DecompressResultName(int res) {
		if (res >= 0) return "nothing";
		switch (res) {
		case DCOMP_BAD_ALGORITHM: return "bad algorithm";
		case DCOMP_DEST_TOO_SMALL: return "too small";
		case DCOMP_UNKNOWN_ERROR: return "unknown error";
		default: return "unhandled error";
		}
	}

	std::ostream& operator<<(std::ostream& out, CompressionAlgorithm alg) {
		const char* d{ GetCompressionName(alg) };
		if (d) {
			out << d;
		}
		else {
			out << "comp:" << (size_t)alg;
		}
		return out;
	}
}