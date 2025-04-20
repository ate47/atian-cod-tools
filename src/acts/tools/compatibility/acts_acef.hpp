#pragma once
#include <tools/utils/compress_utils.hpp>

namespace compatibility::acts_acef {
	constexpr utils::compress::CompressionAlgorithm DEFAULT_COMPRESS = utils::compress::COMP_ZSTD;

	struct AcefBlock {
		void* data;
		size_t size;
		uint64_t name;
	};
	void CompressACEFBlock(AcefBlock* blocks, size_t blocksCount, utils::compress::CompressionAlgorithm alg, const std::filesystem::path& outFile);
	inline void CompressACEFBlock(std::vector<AcefBlock>& blocks, utils::compress::CompressionAlgorithm alg, const std::filesystem::path& outFile) {
		CompressACEFBlock(blocks.data(), blocks.size(), alg, outFile);
	}
}