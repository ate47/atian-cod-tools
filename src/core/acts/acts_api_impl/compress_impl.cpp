#include <includes.hpp>
#include <utils/compress_utils.hpp>
#include <acts_api/compress.h>


ActsStatus ActsAPICompress_GetCompressSize(ActsAPICompress_Algorithm alg, size_t srcSize, size_t* outSize) {
	ACTS_API_ASSERT_NOT_NULL(outSize);
	try {
		*outSize = utils::compress::GetCompressSize((utils::compress::CompressionAlgorithm)alg, srcSize);
		return ACTS_STATUS_OK;
	}
	catch (std::runtime_error& err) {
		ActsAPISetLastMessage("%s", err.what());
		return ACTS_STATUS_ERROR;
	}
}
ActsStatus ActsAPICompress_Compress(ActsAPICompress_Algorithm alg, void* dest, size_t* destSize, const void* src, size_t srcSize) {
	ACTS_API_ASSERT_NOT_NULL(dest);
	ACTS_API_ASSERT_NOT_NULL(destSize);
	ACTS_API_ASSERT_NOT_NULL(src);

	try {
		if (utils::compress::Compress((utils::compress::CompressionAlgorithm)alg, dest, destSize, src, srcSize)) {
			return ACTS_STATUS_OK;
		}
		ActsAPISetLastMessage("Unknown compression error");
		return ACTS_STATUS_ERROR;
	}
	catch (std::runtime_error& err) {
		ActsAPISetLastMessage("%s", err.what());
		return ACTS_STATUS_ERROR;
	}
}
ActsStatus ActsAPICompress_Decompress(ActsAPICompress_Algorithm alg, void* dest, size_t* destSize, const void* src, size_t srcSize) {
	ACTS_API_ASSERT_NOT_NULL(destSize);
	ACTS_API_ASSERT(!*destSize || dest);
	ACTS_API_ASSERT(!srcSize || src);

	try {
		int decomp{ utils::compress::Decompress2((utils::compress::CompressionAlgorithm)alg, dest, *destSize, src, srcSize) };
		if (decomp >= 0) {
			*destSize = decomp;
			return ACTS_STATUS_OK;
		}
		ActsAPISetLastMessage("%s", utils::compress::DecompressResultName(decomp));
		return ACTS_STATUS_ERROR;
	}
	catch (std::runtime_error& err) {
		ActsAPISetLastMessage("%s", err.what());
		return ACTS_STATUS_ERROR;
	}
}

ActsAPICompress_Algorithm ActsAPICompress_GetAlgorithm(const char* cfg) {
	try {
		return (ActsAPICompress_Algorithm)utils::compress::GetConfigName(cfg);
	}
	catch (...) {
		return ActsAPICompress_Algorithm::COMP_INVALID;
	}
}
