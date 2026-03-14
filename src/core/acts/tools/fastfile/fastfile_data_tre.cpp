#include <includes.hpp>
#include <tools/fastfile/fastfile_data_tre.hpp>


namespace fastfile {
	namespace {
		const char* tFFNames[]{
			"none",
			"zlib",
			"zlib_hc",
			"lz4",
			"lz4_hc",
			"bdelta_uncomp",
			"bdelta_zlib",
			"bdelta_lzma",
			"oodle_kraken",
			"oodle_mermaid",
			"oodle_selkie",
			"oodle_lzna",
		};
	}

	FastFileCompression GetFastFileCompression(const char* name) {
		for (size_t i = 0; i < ARRAYSIZE(tFFNames); i++) {
			if (!_strcmpi(tFFNames[i], name)) {
				return (FastFileCompression)i;
			}
		}
		return FastFileCompression::XFILE_COMPRESSION_COUNT;
	}

	utils::compress::CompressionAlgorithm GetFastFileCompressionAlgorithm(FastFileCompression comp) {
		switch (comp) {
		case fastfile::XFILE_UNCOMPRESSED:
			return utils::compress::COMP_NONE;
		case fastfile::XFILE_ZLIB:
			return utils::compress::COMP_ZLIB;
		case fastfile::XFILE_ZLIB_HC:
			return utils::compress::COMP_ZLIB | utils::compress::COMP_HIGH_COMPRESSION;
		case fastfile::XFILE_LZ4:
			return utils::compress::COMP_LZ4;
		case fastfile::XFILE_LZ4_HC:
			return utils::compress::COMP_LZ4 | utils::compress::COMP_HIGH_COMPRESSION;
		case fastfile::XFILE_OODLE_KRAKEN:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_KRAKEN;
		case fastfile::XFILE_OODLE_MERMAID:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_MERMAID;
		case fastfile::XFILE_OODLE_SELKIE:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_SELKIE;
		case fastfile::XFILE_OODLE_LZNA:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_LZNA;
		default:
			throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)comp));
		}
	}

	const char* GetFastFileCompressionName(FastFileCompression comp) {
		if (comp >= ARRAYSIZE(tFFNames) || comp < 0) {
			return "unknown";
		}
		return tFFNames[comp];
	}

}