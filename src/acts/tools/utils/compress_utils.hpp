#pragma once
#include <deps/oodle.hpp>

namespace utils::compress {
	typedef size_t _CompressionAlgorithm;
	enum CompressionAlgorithm : _CompressionAlgorithm {
		// compression type
		COMP_INVALID = 0,
		COMP_NONE,
		COMP_ZLIB,
		COMP_LZMA,
		COMP_LZ4,
		COMP_OODLE,
		COMP_ZSTD,
		COMP_TYPE_MASK = 0xFF,
		// use high compression
		COMP_HIGH_COMPRESSION = 1 << 8,
		// oodle compression type, by default use kraken
		COMP_OODLE_TYPE_KRAKEN = 0ull << 9,
		COMP_OODLE_TYPE_LZH = 1ull << 9,
		COMP_OODLE_TYPE_LZH_LW = 2ull << 9,
		COMP_OODLE_TYPE_LZNIB = 3ull << 9,
		COMP_OODLE_TYPE_NONE = 4ull << 9,
		COMP_OODLE_TYPE_LZB16 = 5ull << 9,
		COMP_OODLE_TYPE_LZBW = 6ull << 9,
		COMP_OODLE_TYPE_LZA = 7ull << 9,
		COMP_OODLE_TYPE_LZNA = 8ull << 9,
		COMP_OODLE_TYPE_MERMAID = 9ull << 9,
		COMP_OODLE_TYPE_BITKNIT = 10ull << 9,
		COMP_OODLE_TYPE_SELKIE = 11ull << 9,
		COMP_OODLE_TYPE_HYDRA = 12ull << 9,
		COMP_OODLE_TYPE_LEVIATHAN = 13ull << 9,
		COMP_OODLE_TYPE_MASK = 15ull << 9,
	};

	enum DecompressResult : int {
		DCOMP_NOTHING = 0,
		DCOMP_BAD_ALGORITHM = -1,
		DCOMP_DEST_TOO_SMALL = -2,
		DCOMP_UNKNOWN_ERROR = -3,
	};

	constexpr utils::compress::CompressionAlgorithm operator|(utils::compress::CompressionAlgorithm a, utils::compress::CompressionAlgorithm b) {
		return (utils::compress::CompressionAlgorithm)((_CompressionAlgorithm)a | (_CompressionAlgorithm)b);
	}

	constexpr utils::compress::CompressionAlgorithm operator&(utils::compress::CompressionAlgorithm a, utils::compress::CompressionAlgorithm b) {
		return (utils::compress::CompressionAlgorithm)((_CompressionAlgorithm)a & (_CompressionAlgorithm)b);
	}
	std::ostream& operator<<(std::ostream& out, CompressionAlgorithm alg);

	constexpr CompressionAlgorithm GetOodleCompressionType(CompressionAlgorithm c) {
		return c & COMP_OODLE_TYPE_MASK;
	}

	constexpr CompressionAlgorithm GetCompressionType(CompressionAlgorithm c) {
		return c & COMP_TYPE_MASK;
	}

	const char* GetCompressionName(CompressionAlgorithm c, const char* defaultValue = "invalid");
	int Decompress2(CompressionAlgorithm alg, void* dest, size_t destSize, const void* src, size_t srcSize);

	inline bool Decompress(CompressionAlgorithm alg, void* dest, size_t destSize, const void* src, size_t srcSize) {
		return Decompress2(alg, dest, destSize, src, srcSize) >= 0;
	}

	std::vector<byte> Decompress(CompressionAlgorithm alg, const void* src, size_t srcSize, float increaseFactor = 1.5);

	bool Compress(CompressionAlgorithm alg, void* dest, size_t* destSize, const void* src, size_t srcSize);
	size_t GetCompressSize(CompressionAlgorithm alg, size_t srcSize);
	deps::oodle::OodleCompressor GetOodleCompressor(CompressionAlgorithm c);

	bool CompressBuffer(CompressionAlgorithm alg, const void* src, size_t srcSize, std::vector<byte>& out);
	inline bool CompressBuffer(CompressionAlgorithm alg, const std::vector<byte>& data, std::vector<byte>& out) {
		return CompressBuffer(alg, data.data(), data.size(), out);
	}
}
template<>
struct std::formatter<utils::compress::CompressionAlgorithm, char> : utils::BasicFormatter<utils::compress::CompressionAlgorithm> {};