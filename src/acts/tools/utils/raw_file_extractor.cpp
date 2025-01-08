#include <includes.hpp>
#include <core/raw_file.hpp>
#include <decryptutils.hpp>
#include "raw_file_extractor.hpp"

namespace tool::utils::raw_file_extractor {

	std::string ExtractRawFile(std::vector<byte>& data) {
		core::bytebuffer::ByteBuffer bytebuff{ data };
		core::raw_file::RawFileReader reader{
			bytebuff,
			[](char* str) -> char* { return acts::decryptutils::DecryptString(str); },
			[](uint64_t h) -> const char* { return hashutils::ExtractTmp("hash", h); }
		};

		return reader.ReadAll();
	}

}