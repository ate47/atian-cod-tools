#pragma once
#include <core/raw_file_json.hpp>

namespace core::hashes::raw_file_extractor {
	typedef char* (DecryptStrFunc)(char* str);

	std::string ExtractRawFile(std::vector<byte>& data);

	void WriteRawFileInto(std::vector<byte>& data, std::ostream& out, DecryptStrFunc = nullptr);

	class JsonWriter : public core::raw_file::json::RawFileJsonWriter {
		char* (*DecryptStr)(char* str);
	public:

		JsonWriter(core::raw_file::json::JsonFormat format = core::raw_file::json::JFF_PRETTY,
			DecryptStrFunc DecryptStr = nullptr);

		bool WriteToFile(const std::filesystem::path& out);
	};
}