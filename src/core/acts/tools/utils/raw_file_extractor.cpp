#include <includes.hpp>
#include <core/raw_file.hpp>
#include <decryptutils.hpp>
#include "raw_file_extractor.hpp"

namespace utils::raw_file_extractor {
	static const char* ExtractHash(uint64_t h, bool escaped) {
		auto [ok, buff] = hashutils::ExtractTmpPair("hash", h);
		if (ok && escaped) {
			utils::MapString(buff, [](char c) -> char { return c == '\\' ? '/' : c; });
		}
		return buff;
	}
	static char* DecryptString(char* str) {
		return acts::decryptutils::DecryptString(str);
	}

	std::string ExtractRawFile(std::vector<byte>& data) {
		core::bytebuffer::ByteBuffer bytebuff{ data };
		core::raw_file::RawFileReader reader{ bytebuff, DecryptString, ExtractHash };
		return reader.ReadAll();
	}

	void WriteRawFileInto(std::vector<byte>& data, std::ostream& out) {
		core::bytebuffer::ByteBuffer bytebuff{ data };
		core::raw_file::RawFileReader reader{ bytebuff, DecryptString, ExtractHash };

		reader.ReadAll(out);
	}

	bool JsonWriter::WriteToFile(const std::filesystem::path& out) {
		::utils::OutFileCE os{ out };
		if (!os) return false;

		WriteRawFileInto(Build(), os);

		return true;
	}
}