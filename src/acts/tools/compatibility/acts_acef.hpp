#pragma once
#include <utils/compress_utils.hpp>
#include <core/memory_allocator.hpp>
#include <tools/gsc_opcodes.hpp>

namespace compatibility::acts_acef {
	constexpr utils::compress::CompressionAlgorithm DEFAULT_COMPRESS = utils::compress::COMP_ZSTD;

	struct AcefBlock {
		void* data;
		size_t size;
		uint64_t name;
	};
	AcefBlock CreateACEFBlockHash(std::map<std::string, std::unordered_set<uint64_t>>& dataMap, core::memory_allocator::MemoryAllocator& alloc);
	void CreateACEFOpCodeBlock(int plts, const tool::gsc::opcode::VmInfo* info, core::memory_allocator::MemoryAllocator& alloc, AcefBlock* blocks, size_t* count);

	void CompressACEFBlock(AcefBlock* blocks, size_t blocksCount, utils::compress::CompressionAlgorithm alg, const std::filesystem::path& outFile);
	inline void CompressACEFBlock(std::vector<AcefBlock>& blocks, utils::compress::CompressionAlgorithm alg, const std::filesystem::path& outFile) {
		CompressACEFBlock(blocks.data(), blocks.size(), alg, outFile);
	}
}