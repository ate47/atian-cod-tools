#include <includes.hpp>
#include <deps/oodle.hpp>
#include <lz4.h>
#include <zstd.h>
#include "compress_utils.hpp"

namespace utils::compress {
	bool Decompress(CompressionAlgorithm alg, void* dest, size_t destSize, const void* src, size_t srcSize) {
		switch (alg) {
		case COMP_NONE:
			std::memcpy(dest, src, srcSize);
			return true;
		case COMP_LZ4:
			return LZ4_decompress_safe((const char*)src, (char*)dest, (int)srcSize, (int)destSize);
		case COMP_OODLE: {
			deps::oodle::Oodle& oodle{ deps::oodle::GetInstance() };

			return oodle.Decompress(src, (uint32_t)srcSize, dest, (uint32_t)destSize, deps::oodle::OODLE_FS_YES) != 0;
		}
		case COMP_ZLIB: {
			uLongf sizef = (uLongf)destSize;
			uLongf sizef2{ (uLongf)srcSize };
			return uncompress2((Bytef*)dest, &sizef, (const Bytef*)src, &sizef2) >= 0;
		}
		case COMP_ZSTD: {
			size_t ret{ ZSTD_decompress(dest, destSize, src, srcSize) };
			return !ZSTD_isError(ret);
		}
		default:
			return false;
		}
	}
}