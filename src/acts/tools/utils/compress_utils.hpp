#pragma once

namespace utils::compress {
	enum CompressionAlgorithm {
		COMP_INVALID = 0,
		COMP_NONE,
		COMP_ZLIB,
		COMP_LZ4,
		COMP_OODLE,
		COMP_ZSTD,
	};

	bool Decompress(CompressionAlgorithm alg, void* dest, size_t destSize, const void* src, size_t srcSize);
}