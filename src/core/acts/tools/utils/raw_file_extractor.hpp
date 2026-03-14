#pragma once
#include <core/raw_file_json.hpp>

namespace utils::raw_file_extractor {

	std::string ExtractRawFile(std::vector<byte>& data);

	void WriteRawFileInto(std::vector<byte>& data, std::ostream& out);

	class JsonWriter: public core::raw_file::json::RawFileJsonWriter {
	public:
		using core::raw_file::json::RawFileJsonWriter::RawFileJsonWriter;

		bool WriteToFile(const std::filesystem::path& out);
	};
}