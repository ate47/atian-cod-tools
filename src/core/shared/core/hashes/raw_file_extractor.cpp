#include <includes_shared.hpp>
#include <core/hashes/hash_store.hpp>
#include <core/hashes/raw_file_extractor.hpp>
#include <core/raw_file.hpp>

namespace core::hashes::raw_file_extractor {
	static const char* ExtractHash(uint64_t h, bool escaped) {
		auto [ok, buff] = core::hashes::ExtractTmpPair("hash", h);
		if (ok && escaped) {
			utils::MapString(buff, [](char c) -> char { return c == '\\' ? '/' : c; });
		}
		return buff;
	}
	static char* DecryptString(char* str) {
		return str;
	}

	std::string ExtractRawFile(std::vector<byte>& data) {
		core::bytebuffer::ByteBuffer bytebuff{ data };
		core::raw_file::RawFileReader reader{ bytebuff, DecryptString, ExtractHash };
		return reader.ReadAll();
	}

	void WriteRawFileInto(std::vector<byte>& data, std::ostream& out, DecryptStrFunc DecryptStr) {
		core::bytebuffer::ByteBuffer bytebuff{ data };
		core::raw_file::RawFileReader reader{ bytebuff, DecryptStr ? DecryptStr : DecryptString, ExtractHash };

		reader.ReadAll(out);
	}
	JsonWriter::JsonWriter(core::raw_file::json::JsonFormat format, DecryptStrFunc DecryptStr) 
		: core::raw_file::json::RawFileJsonWriter(format), DecryptStr(DecryptStr) {
	}


	bool JsonWriter::WriteToFile(const std::filesystem::path& out) {
		::utils::OutFileCE os{ out };
		if (!os) return false;

		WriteRawFileInto(Build(), os, DecryptStr);

		return true;
	}
}