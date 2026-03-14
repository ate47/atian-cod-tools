#include <includes.hpp>
#include <tools/fastfile/fastfile_data_iw.hpp>


namespace fastfile {
	namespace {
		const char* iwFFNames[]{
			"invalid",
			"none",
			"zlib_size",
			"zlib_speed",
			"lz4_hc",
			"lz4",
			"oodle_kraken_size",
			"oodle_kraken_speed",
			"hw_zlib_size",
			"hw_zlib_speed",
			"ps4_zlib_size",
			"ps4_zlib_spreed",
			"oodle_leviathan_size",
			"oodle_leviathan_speed",
			"oodle_mermaid_size",
			"oodle_mermaid_speed",
			"oodle_selkie_size",
			"oodle_selkie_speed",
			"zstd_size",
			"zstd_speed",
		};
	}

	const char* GetFastFileCompressionName(FastFileIWCompression comp) {

		if (comp >= ARRAYSIZE(iwFFNames) || comp < 0) {
			return "unknown";
		}
		return iwFFNames[comp];
	}

	FastFileIWCompression GetFastFileIWCompression(const char* name) {
		for (size_t i = 0; i < ARRAYSIZE(iwFFNames); i++) {
			if (!_strcmpi(iwFFNames[i], name)) {
				return (FastFileIWCompression)i;
			}
		}
		return FastFileIWCompression::IWFFC_COUNT;
	}

	utils::compress::CompressionAlgorithm GetFastFileCompressionAlgorithm(FastFileIWCompression comp) {
		switch (comp) {
		case fastfile::IWFFC_NONE:
			return utils::compress::COMP_NONE;
		case fastfile::IWFFC_LZ4:
			return utils::compress::COMP_LZ4;
		case fastfile::IWFFC_LZ4_HC:
			return utils::compress::COMP_LZ4 | utils::compress::COMP_HIGH_COMPRESSION;
		case IWFFC_ZLIB_SIZE:
		case IWFFC_HW_ZLIB_SIZE:
		case IWFFC_PS4_ZLIB_SIZE:
			return utils::compress::COMP_ZLIB | utils::compress::COMP_HIGH_COMPRESSION;
		case IWFFC_ZLIB_SPEED:
		case IWFFC_HW_ZLIB_SPEED:
		case IWFFC_PS4_ZLIB_SPREED:
			return utils::compress::COMP_ZLIB;
		case IWFFC_OODLE_KRAKEN_SIZE:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_KRAKEN | utils::compress::COMP_HIGH_COMPRESSION;
		case IWFFC_OODLE_KRAKEN_SPEED:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_KRAKEN;
		case IWFFC_OODLE_LEVIATHAN_SIZE:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_LEVIATHAN | utils::compress::COMP_HIGH_COMPRESSION;
		case IWFFC_OODLE_LEVIATHAN_SPEED:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_LEVIATHAN;
		case IWFFC_OODLE_MERMAID_SIZE:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_MERMAID | utils::compress::COMP_HIGH_COMPRESSION;
		case IWFFC_OODLE_MERMAID_SPEED:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_MERMAID;
		case IWFFC_OODLE_SELKIE_SIZE:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_SELKIE | utils::compress::COMP_HIGH_COMPRESSION;
		case IWFFC_OODLE_SELKIE_SPEED:
			return utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_SELKIE;
		case IWFFC_ZSTD_SIZE:
			return utils::compress::COMP_ZSTD | utils::compress::COMP_HIGH_COMPRESSION;
		case IWFFC_ZSTD_SPEED:
			return utils::compress::COMP_ZSTD;
		default:
			throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)comp));
		}
	}
}